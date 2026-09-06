#!/usr/bin/env python3
"""Serve the signage site and a small local control API."""

from __future__ import annotations

import argparse
import json
import os
from http.server import SimpleHTTPRequestHandler, ThreadingHTTPServer
from pathlib import Path
from typing import Any
from urllib.parse import urlparse

MODES = ("booth", "away")
DEFAULT_MODE = "booth"
DEFAULT_BIND = "127.0.0.1"
DEFAULT_PORT = 4173
MODE_ENV_VAR = "SIGNAGE_MODE_FILE"


class ConfigurationError(RuntimeError):
    """Raised when the server cannot start with the given options."""


def default_mode_path() -> Path:
    """Prefer an explicit path, then a writable local state file."""
    configured = os.environ.get(MODE_ENV_VAR, "").strip()
    if configured:
        return Path(configured)
    return Path("/tmp/signage-mode.json")


def normalize_mode(value: Any) -> str:
    """Accept only booth or away."""
    mode = str(value or "").strip().lower()
    if mode not in MODES:
        raise ValueError("mode must be booth or away")
    return mode


def read_mode(path: Path) -> str:
    """Read the last mode from disk. Missing or invalid files return booth."""
    try:
        payload = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return DEFAULT_MODE
    try:
        return normalize_mode(payload.get("mode"))
    except ValueError:
        return DEFAULT_MODE


def write_mode(path: Path, mode: str) -> str:
    """Persist the mode and return the stored value."""
    normalized = normalize_mode(mode)
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        json.dumps({"mode": normalized}, indent=2) + "\n",
        encoding="utf-8",
    )
    return normalized


def parse_mode_body(raw: bytes) -> str:
    """Parse a JSON control body into a mode string."""
    if not raw.strip():
        raise ValueError("request body is empty")
    try:
        payload = json.loads(raw.decode("utf-8"))
    except (UnicodeDecodeError, json.JSONDecodeError) as exc:
        raise ValueError("request body must be JSON") from exc
    if not isinstance(payload, dict):
        raise ValueError("request body must be an object")
    return normalize_mode(payload.get("mode"))


class SignageHandler(SimpleHTTPRequestHandler):
    """Static files plus GET/POST /api/mode."""

    mode_path: Path

    def do_GET(self) -> None:  # noqa: N802 - required by BaseHTTPRequestHandler
        if urlparse(self.path).path == "/api/mode":
            self._send_json(200, {"mode": read_mode(self.mode_path)})
            return
        super().do_GET()

    def do_POST(self) -> None:  # noqa: N802 - required by BaseHTTPRequestHandler
        if urlparse(self.path).path != "/api/mode":
            self.send_error(404, "Not found")
            return
        length = int(self.headers.get("Content-Length", "0") or "0")
        if length < 0 or length > 4096:
            self.send_error(400, "Invalid content length")
            return
        raw = self.rfile.read(length)
        try:
            mode = write_mode(self.mode_path, parse_mode_body(raw))
        except ValueError as exc:
            self._send_json(400, {"error": str(exc)})
            return
        self._send_json(200, {"mode": mode})

    def log_message(self, format: str, *args: object) -> None:
        """Keep kiosk logs short."""
        sys_stderr = getattr(self, "_log_stream", None)
        if sys_stderr is None:
            import sys

            sys_stderr = sys.stderr
        sys_stderr.write("%s - %s\n" % (self.address_string(), format % args))

    def _send_json(self, status: int, payload: dict[str, Any]) -> None:
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Cache-Control", "no-store")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Serve Fulcrum Builds signage and the local mode API.",
    )
    parser.add_argument("--bind", default=DEFAULT_BIND)
    parser.add_argument("--port", type=int, default=DEFAULT_PORT)
    parser.add_argument(
        "--directory",
        default=".",
        help="site root; defaults to the current directory",
    )
    parser.add_argument(
        "--mode-file",
        default=None,
        help="JSON file that stores booth/away; overrides SIGNAGE_MODE_FILE",
    )
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    directory = Path(args.directory).resolve()
    if not directory.is_dir():
        print(f"Site directory does not exist: {directory}", flush=True)
        return 2

    mode_file = Path(args.mode_file) if args.mode_file else default_mode_path()

    class BoundHandler(SignageHandler):
        mode_path = mode_file

        def __init__(self, *handler_args: Any, **handler_kwargs: Any) -> None:
            super().__init__(*handler_args, directory=str(directory), **handler_kwargs)

    server = ThreadingHTTPServer((args.bind, args.port), BoundHandler)
    print(
        f"Serving {directory} on http://{args.bind}:{args.port} "
        f"(mode file {mode_file})",
        flush=True,
    )
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("Stopping signage server", flush=True)
    finally:
        server.server_close()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
