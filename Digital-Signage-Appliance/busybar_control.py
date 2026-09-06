#!/usr/bin/env python3
"""Remote control utility for the Fulcrum Builds BUSY Bar."""

from __future__ import annotations

import argparse
import asyncio
import json
import os
import random
import sys
import time
import urllib.error
import urllib.request
from collections.abc import Sequence
from typing import Any

from busylib import AsyncBusyBar, BusyBar, types

APPLICATION_NAME = "fulcrum-builds-signage"
TOKEN_ENV_VAR = "BUSYBAR_API_TOKEN"
ADDRESS_ENV_VAR = "BUSYBAR_ADDRESS"
SIGNAGE_API_ENV_VAR = "SIGNAGE_API"
DEFAULT_SIGNAGE_API = "http://127.0.0.1:4173"
DEFAULT_COLOR = "#00FF41FF"
ACCENT_COLOR = "#00FF41FF"
AGENT_COLOR = "#38BDF8FF"
SLEEPING_COLOR = "#7CFF7CFF"
AWAY_COLOR = "#FBBF24FF"
# Firmware examples use 800 pixels/minute. 2400 is about 3x that.
SCROLL_RATE = 2400
SCROLL_START_DELAY_MS = 200
SCROLL_REPEAT_DELAY_MS = 400
SIGNAGE_PRIORITY = 70
AGENT_PRIORITY = 90
AWAY_TOGGLE_BUTTON = "START"
BUTTON_NAMES = {
    0: "OK",
    1: "BACK",
    2: "START",
    "OK": "OK",
    "BACK": "BACK",
    "START": "START",
    "ok": "OK",
    "back": "BACK",
    "start": "START",
}
ACTION_NAMES = {
    0: "PRESS",
    1: "RELEASE",
    "PRESS": "PRESS",
    "RELEASE": "RELEASE",
    "press": "PRESS",
    "release": "RELEASE",
}
SLEEPING_LINES = (
    "Agent is taking a smoke break",
    "Snoozing",
    "Waiting for input",
    "Stop procrastinating",
)
DISPLAY_WIDTHS = {
    types.DisplayName.FRONT: 72,
    types.DisplayName.BACK: 160,
}
FONT_CHOICES = (
    "tiny",
    "small",
    "normal",
    "condensed",
    "bold",
    "large",
    "extra_large",
    "global",
)


class ConfigurationError(RuntimeError):
    """Raised when required local configuration is missing."""


def require_token() -> str:
    """Read the cloud API token without ever accepting it as a CLI argument."""
    token = os.environ.get(TOKEN_ENV_VAR, "").strip()
    if not token:
        raise ConfigurationError(
            f"{TOKEN_ENV_VAR} is not set. Add a BUSY Bar-scoped token as a "
            "secure environment secret; do not put it in the repository or command line."
        )
    return token


def busybar_kwargs() -> dict[str, Any]:
    """Shared client options for USB and cloud connections."""
    return {"timeout": 15.0, "compatibility_mode": "warn"}


def local_address() -> str:
    """USB or LAN address for the bar. Empty means use the cloud token."""
    return os.environ.get(ADDRESS_ENV_VAR, "").strip()


def open_busybar() -> BusyBar:
    """Prefer a local USB address; otherwise use the cloud token."""
    address = local_address()
    kwargs = busybar_kwargs()
    if address:
        return BusyBar(address, **kwargs)
    return BusyBar(token=require_token(), **kwargs)


def open_async_busybar() -> AsyncBusyBar:
    """Open an async client. Button watch needs a local bar, not cloud."""
    address = local_address()
    kwargs = busybar_kwargs()
    if address:
        return AsyncBusyBar(address, **kwargs)
    raise ConfigurationError(
        f"{ADDRESS_ENV_VAR} is not set. Button watch talks to the bar over "
        "USB or LAN; it cannot use the cloud API."
    )


def signage_api_base() -> str:
    """Local OLED control API. Empty disables the HTTP notify."""
    return os.environ.get(SIGNAGE_API_ENV_VAR, DEFAULT_SIGNAGE_API).rstrip("/")


def display_names(value: str) -> tuple[types.DisplayName, ...]:
    """Map a CLI display selector to BUSY Bar display names."""
    if value == "both":
        return (types.DisplayName.FRONT, types.DisplayName.BACK)
    return (types.DisplayName(value),)


def build_message_payload(
    text: str,
    *,
    display: str,
    font: str,
    color: str,
) -> types.DisplayElements:
    """Build a scrolling text payload for one or both displays."""
    elements: list[types.DisplayElement] = []
    for display_name in display_names(display):
        elements.append(
            types.TextElement(
                id=f"message-{display_name.value}",
                text=text,
                font=font,
                color=color,
                display=display_name,
                x=0,
                y=1,
                width=DISPLAY_WIDTHS[display_name],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            )
        )

    return types.DisplayElements(
        application_name=APPLICATION_NAME,
        priority=SIGNAGE_PRIORITY,
        led_notification_color=color,
        elements=elements,
    )


def build_signage_payload() -> types.DisplayElements:
    """Build the Fulcrum Builds convention preset for both displays."""
    return types.DisplayElements(
        application_name=APPLICATION_NAME,
        priority=SIGNAGE_PRIORITY,
        led_notification_color=ACCENT_COLOR,
        elements=[
            types.TextElement(
                id="front-marquee",
                text="FULCRUM BUILDS  //  ASK ME ABOUT MY ROBOTS",
                font="bold",
                color=DEFAULT_COLOR,
                display=types.DisplayName.FRONT,
                x=0,
                y=1,
                width=DISPLAY_WIDTHS[types.DisplayName.FRONT],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            ),
            types.TextElement(
                id="back-title",
                text="FULCRUM BUILDS",
                font="large",
                color=DEFAULT_COLOR,
                display=types.DisplayName.BACK,
                x=0,
                y=14,
                width=DISPLAY_WIDTHS[types.DisplayName.BACK],
                align="top_left",
            ),
            types.TextElement(
                id="back-callout",
                text="ASK ME ABOUT MY ROBOTS",
                font="small",
                color=DEFAULT_COLOR,
                display=types.DisplayName.BACK,
                x=0,
                y=46,
                width=DISPLAY_WIDTHS[types.DisplayName.BACK],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            ),
        ],
    )


def build_agent_payload(detail: str = "MAKING CHANGES") -> types.DisplayElements:
    """Show that an agent is actively changing the appliance."""
    callout = detail.strip() or "MAKING CHANGES"
    return types.DisplayElements(
        application_name=APPLICATION_NAME,
        priority=AGENT_PRIORITY,
        led_notification_color=AGENT_COLOR,
        elements=[
            types.TextElement(
                id="front-marquee",
                text=f"AGENT  //  {callout.upper()}",
                font="bold",
                color=AGENT_COLOR,
                display=types.DisplayName.FRONT,
                x=0,
                y=1,
                width=DISPLAY_WIDTHS[types.DisplayName.FRONT],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            ),
            types.TextElement(
                id="back-title",
                text="AGENT WORK",
                font="large",
                color=AGENT_COLOR,
                display=types.DisplayName.BACK,
                x=0,
                y=14,
                width=DISPLAY_WIDTHS[types.DisplayName.BACK],
                align="top_left",
            ),
            types.TextElement(
                id="back-callout",
                text=callout.upper(),
                font="small",
                color=DEFAULT_COLOR,
                display=types.DisplayName.BACK,
                x=0,
                y=46,
                width=DISPLAY_WIDTHS[types.DisplayName.BACK],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            ),
        ],
    )


def choose_sleeping_line(detail: str | None = None, *, rng: random.Random | None = None) -> str:
    """Return an explicit idle line, or pick one of the stock jokes."""
    if detail and detail.strip():
        return detail.strip()
    picker = rng or random
    return picker.choice(SLEEPING_LINES)


def build_sleeping_payload(detail: str | None = None, *, rng: random.Random | None = None) -> types.DisplayElements:
    """Show that the agent is idle and waiting."""
    callout = choose_sleeping_line(detail, rng=rng)
    return types.DisplayElements(
        application_name=APPLICATION_NAME,
        priority=SIGNAGE_PRIORITY,
        led_notification_color=SLEEPING_COLOR,
        elements=[
            types.TextElement(
                id="front-marquee",
                text=f"SLEEPING  //  {callout.upper()}",
                font="bold",
                color=SLEEPING_COLOR,
                display=types.DisplayName.FRONT,
                x=0,
                y=1,
                width=DISPLAY_WIDTHS[types.DisplayName.FRONT],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            ),
            types.TextElement(
                id="back-title",
                text="SLEEPING",
                font="large",
                color=SLEEPING_COLOR,
                display=types.DisplayName.BACK,
                x=0,
                y=14,
                width=DISPLAY_WIDTHS[types.DisplayName.BACK],
                align="top_left",
            ),
            types.TextElement(
                id="back-callout",
                text=callout.upper(),
                font="small",
                color=DEFAULT_COLOR,
                display=types.DisplayName.BACK,
                x=0,
                y=46,
                width=DISPLAY_WIDTHS[types.DisplayName.BACK],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            ),
        ],
    )


def build_away_payload(detail: str = "BACK SOON") -> types.DisplayElements:
    """Show that the booth operator has stepped away."""
    callout = detail.strip() or "BACK SOON"
    return types.DisplayElements(
        application_name=APPLICATION_NAME,
        priority=SIGNAGE_PRIORITY,
        led_notification_color=AWAY_COLOR,
        elements=[
            types.TextElement(
                id="front-marquee",
                text=f"AWAY  //  {callout.upper()}",
                font="bold",
                color=AWAY_COLOR,
                display=types.DisplayName.FRONT,
                x=0,
                y=1,
                width=DISPLAY_WIDTHS[types.DisplayName.FRONT],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            ),
            types.TextElement(
                id="back-title",
                text="AWAY",
                font="large",
                color=AWAY_COLOR,
                display=types.DisplayName.BACK,
                x=0,
                y=14,
                width=DISPLAY_WIDTHS[types.DisplayName.BACK],
                align="top_left",
            ),
            types.TextElement(
                id="back-callout",
                text=callout.upper(),
                font="small",
                color=DEFAULT_COLOR,
                display=types.DisplayName.BACK,
                x=0,
                y=46,
                width=DISPLAY_WIDTHS[types.DisplayName.BACK],
                align="top_left",
                scroll_rate=SCROLL_RATE,
                scroll_start_delay=SCROLL_START_DELAY_MS,
                scroll_repeat_delay=SCROLL_REPEAT_DELAY_MS,
            ),
        ],
    )


def normalize_button_event(event: dict[str, Any] | None) -> tuple[str, str] | None:
    """Map a protobuf button event to (BUTTON, ACTION) names.

    OK + PRESS are proto3 defaults, so an empty event is an OK press.
    """
    if event is None:
        return None
    if not isinstance(event, dict):
        return None
    button = BUTTON_NAMES.get(event.get("button", 0))
    action = ACTION_NAMES.get(event.get("action", 0))
    if button is None or action is None:
        return None
    return button, action


def extract_button_events(state: Any) -> list[tuple[str, str]]:
    """Return button events from a /api/status/ws state payload."""
    events: list[tuple[str, str]] = []
    if not isinstance(state, dict):
        return events
    updates = state.get("updates")
    if not isinstance(updates, list):
        return events
    for update in updates:
        if not isinstance(update, dict):
            continue
        payload = update.get("input")
        if not isinstance(payload, dict):
            continue
        parsed = normalize_button_event(payload.get("button_event"))
        if parsed is not None:
            events.append(parsed)
    return events


def is_away_toggle_press(state: Any) -> bool:
    """True when the top Start/Pause button is pressed."""
    return any(
        button == AWAY_TOGGLE_BUTTON and action == "PRESS"
        for button, action in extract_button_events(state)
    )


def notify_signage_mode(mode: str, *, api: str | None = None) -> None:
    """Tell the OLED server to switch booth/away. Ignore a missing server."""
    base = DEFAULT_SIGNAGE_API if api is None else api
    if not base:
        return
    body = json.dumps({"mode": mode}).encode("utf-8")
    request = urllib.request.Request(
        f"{base}/api/mode",
        data=body,
        method="POST",
        headers={"Content-Type": "application/json"},
    )
    try:
        with urllib.request.urlopen(request, timeout=2.0) as response:
            response.read()
    except (urllib.error.URLError, TimeoutError, OSError):
        print(
            f"OLED mode API at {base} is not reachable; BUSY Bar still updated.",
            file=sys.stderr,
        )


def read_signage_mode(*, api: str | None = None) -> str:
    """Read booth/away from the OLED server. Default to booth if missing."""
    base = DEFAULT_SIGNAGE_API if api is None else api
    if not base:
        return "booth"
    request = urllib.request.Request(f"{base}/api/mode")
    try:
        with urllib.request.urlopen(request, timeout=2.0) as response:
            payload = json.loads(response.read().decode("utf-8"))
    except (urllib.error.URLError, TimeoutError, OSError, json.JSONDecodeError):
        return "booth"
    mode = str(payload.get("mode", "booth")).strip().lower()
    return mode if mode in ("booth", "away") else "booth"


def payload_json(payload: types.DisplayElements) -> str:
    """Serialize a payload for dry-run inspection."""
    return json.dumps(payload.model_dump(mode="json", exclude_none=True), indent=2)


def parse_brightness(value: str) -> int | str:
    """Accept auto or an integer percentage."""
    if value == "auto":
        return value
    try:
        brightness = int(value)
    except ValueError as exc:
        raise argparse.ArgumentTypeError("brightness must be auto or 0-100") from exc
    if not 0 <= brightness <= 100:
        raise argparse.ArgumentTypeError("brightness must be auto or 0-100")
    return brightness


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Control a linked BUSY Bar through the BUSY cloud API.",
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="print the outgoing operation without contacting the BUSY Bar",
    )
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("status", help="show device, firmware, and power status")

    message_parser = subparsers.add_parser("message", help="display custom text")
    message_parser.add_argument("text")
    message_parser.add_argument(
        "--display",
        choices=("front", "back", "both"),
        default="both",
    )
    message_parser.add_argument("--font", choices=FONT_CHOICES, default="bold")
    message_parser.add_argument("--color", default=DEFAULT_COLOR)

    subparsers.add_parser(
        "signage",
        help="show the Fulcrum Builds convention preset",
    )
    agent_parser = subparsers.add_parser(
        "agent",
        help="show that an agent is actively making changes",
    )
    agent_parser.add_argument(
        "detail",
        nargs="?",
        default="MAKING CHANGES",
        help="short status line for the rear display",
    )
    sleeping_parser = subparsers.add_parser(
        "sleeping",
        help="show that the agent is idle / waiting",
    )
    sleeping_parser.add_argument(
        "detail",
        nargs="?",
        default=None,
        help="idle line; omit to pick one at random",
    )
    away_parser = subparsers.add_parser(
        "away",
        help="show that the booth operator has stepped away",
    )
    away_parser.add_argument(
        "detail",
        nargs="?",
        default="BACK SOON",
        help="short status line for the rear display",
    )
    subparsers.add_parser(
        "watch",
        help="listen for the top Start button and toggle booth/away",
    )
    subparsers.add_parser("clear", help="clear Fulcrum Builds display content")

    brightness_parser = subparsers.add_parser(
        "brightness",
        help="set display brightness to auto or 0-100",
    )
    brightness_parser.add_argument("value", type=parse_brightness)
    return parser


def dry_run(args: argparse.Namespace) -> bool:
    """Handle a non-network dry run and report whether it was handled."""
    if not args.dry_run:
        return False
    if args.command == "message":
        print(
            payload_json(
                build_message_payload(
                    args.text,
                    display=args.display,
                    font=args.font,
                    color=args.color,
                )
            )
        )
    elif args.command == "signage":
        print(payload_json(build_signage_payload()))
    elif args.command == "agent":
        print(payload_json(build_agent_payload(args.detail)))
    elif args.command == "sleeping":
        print(payload_json(build_sleeping_payload(args.detail)))
    elif args.command == "away":
        print(payload_json(build_away_payload(args.detail)))
    elif args.command == "watch":
        print(
            json.dumps(
                {
                    "operation": "watch",
                    "toggle_button": AWAY_TOGGLE_BUTTON,
                    "signage_api": signage_api_base(),
                }
            )
        )
    elif args.command == "clear":
        print(json.dumps({"operation": "clear", "application_name": APPLICATION_NAME}))
    elif args.command == "brightness":
        print(json.dumps({"operation": "brightness", "value": args.value}))
    else:
        print("status has no mutating dry-run payload", file=sys.stderr)
        return False
    return True


def run_command(args: argparse.Namespace) -> None:
    """Execute one command against the local USB or cloud-linked BUSY Bar."""
    with open_busybar() as busybar:
        if args.command == "status":
            status = busybar.status()
            print(json.dumps(status.model_dump(mode="json"), indent=2))
        elif args.command == "message":
            response = busybar.display_draw(
                build_message_payload(
                    args.text,
                    display=args.display,
                    font=args.font,
                    color=args.color,
                ),
                clear_before_draw=True,
                sanitize_text=True,
            )
            print(response.result)
        elif args.command == "signage":
            response = busybar.display_draw(
                build_signage_payload(),
                clear_before_draw=True,
                sanitize_text=True,
            )
            notify_signage_mode("booth", api=signage_api_base())
            print(response.result)
        elif args.command == "agent":
            response = busybar.display_draw(
                build_agent_payload(args.detail),
                clear_before_draw=True,
                sanitize_text=True,
            )
            print(response.result)
        elif args.command == "sleeping":
            response = busybar.display_draw(
                build_sleeping_payload(args.detail),
                clear_before_draw=True,
                sanitize_text=True,
            )
            print(response.result)
        elif args.command == "away":
            response = busybar.display_draw(
                build_away_payload(args.detail),
                clear_before_draw=True,
                sanitize_text=True,
            )
            notify_signage_mode("away", api=signage_api_base())
            print(response.result)
        elif args.command == "clear":
            response = busybar.display_clear(application_name=APPLICATION_NAME)
            print(response.result)
        elif args.command == "brightness":
            response = busybar.display_brightness_set(args.value)
            print(response.result)
        else:  # pragma: no cover - argparse prevents this branch.
            raise ValueError(f"Unsupported command: {args.command}")


async def apply_oled_mode(busybar: Any, mode: str) -> None:
    """Draw the matching booth or away preset on the bar."""
    payload = build_away_payload() if mode == "away" else build_signage_payload()
    await busybar.display_draw(
        payload,
        clear_before_draw=True,
        sanitize_text=True,
    )


async def watch_away_button() -> None:
    """Toggle booth/away when the top Start/Pause button is pressed."""
    api = signage_api_base()
    mode = read_signage_mode(api=api)
    last_toggle = 0.0
    async with open_async_busybar() as busybar:
        await apply_oled_mode(busybar, mode)
        notify_signage_mode(mode, api=api)
        print(f"watching {AWAY_TOGGLE_BUTTON}; current mode {mode}", flush=True)
        async for state in busybar.stream_status_ws():
            if not is_away_toggle_press(state):
                continue
            now = time.monotonic()
            if now - last_toggle < 1.0:
                continue
            last_toggle = now
            mode = "booth" if mode == "away" else "away"
            await apply_oled_mode(busybar, mode)
            notify_signage_mode(mode, api=api)
            print(mode, flush=True)


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        if dry_run(args):
            return 0
        if args.command == "watch":
            asyncio.run(watch_away_button())
            return 0
        run_command(args)
    except ConfigurationError as exc:
        print(f"Configuration error: {exc}", file=sys.stderr)
        return 2
    except Exception as exc:  # The CLI should report API failures without a traceback.
        print(f"BUSY Bar request failed: {exc}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
