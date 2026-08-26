#!/usr/bin/env python3
"""Remote control utility for the Fulcrum Builds BUSY Bar."""

from __future__ import annotations

import argparse
import json
import os
import sys
from collections.abc import Sequence
from typing import Any

from busylib import BusyBar, types

APPLICATION_NAME = "fulcrum-builds-signage"
TOKEN_ENV_VAR = "BUSYBAR_API_TOKEN"
ADDRESS_ENV_VAR = "BUSYBAR_ADDRESS"
DEFAULT_COLOR = "#00FF41FF"
ACCENT_COLOR = "#00FF41FF"
AGENT_COLOR = "#38BDF8FF"
# Firmware examples use 800 pixels/minute. 2400 is about 3x that.
SCROLL_RATE = 2400
SCROLL_START_DELAY_MS = 200
SCROLL_REPEAT_DELAY_MS = 400
SIGNAGE_PRIORITY = 70
AGENT_PRIORITY = 90
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


def open_busybar() -> BusyBar:
    """Prefer a local USB address; otherwise use the cloud token."""
    address = os.environ.get(ADDRESS_ENV_VAR, "").strip()
    kwargs: dict[str, Any] = {"timeout": 15.0, "compatibility_mode": "warn"}
    if address:
        return BusyBar(address, **kwargs)
    return BusyBar(token=require_token(), **kwargs)


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
            print(response.result)
        elif args.command == "agent":
            response = busybar.display_draw(
                build_agent_payload(args.detail),
                clear_before_draw=True,
                sanitize_text=True,
            )
            print(response.result)
        elif args.command == "clear":
            response = busybar.display_clear(application_name=APPLICATION_NAME)
            print(response.result)
        elif args.command == "brightness":
            response = busybar.display_brightness_set(args.value)
            print(response.result)
        else:  # pragma: no cover - argparse prevents this branch.
            raise ValueError(f"Unsupported command: {args.command}")


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        if not dry_run(args):
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
