from __future__ import annotations

import json
import tempfile
import threading
import unittest
import urllib.request
from http.server import ThreadingHTTPServer
from pathlib import Path

import serve


class SignageServeTests(unittest.TestCase):
    def test_read_mode_defaults_to_booth(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "mode.json"
            self.assertEqual(serve.read_mode(path), "booth")

    def test_write_and_read_away_mode(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            path = Path(tmp) / "state" / "mode.json"
            self.assertEqual(serve.write_mode(path, "away"), "away")
            self.assertEqual(serve.read_mode(path), "away")
            stored = json.loads(path.read_text(encoding="utf-8"))
            self.assertEqual(stored, {"mode": "away"})

    def test_parse_mode_body_rejects_unknown_values(self) -> None:
        self.assertEqual(serve.parse_mode_body(b'{"mode": "booth"}'), "booth")
        with self.assertRaises(ValueError):
            serve.parse_mode_body(b'{"mode": "party"}')
        with self.assertRaises(ValueError):
            serve.parse_mode_body(b"")

    def test_http_mode_roundtrip(self) -> None:
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            (root / "index.html").write_text("Fulcrum Builds", encoding="utf-8")
            mode_path = root / "mode.json"

            class Handler(serve.SignageHandler):
                def __init__(self, *args: object, **kwargs: object) -> None:
                    super().__init__(*args, directory=str(root), **kwargs)

            Handler.mode_path = mode_path
            server = ThreadingHTTPServer(("127.0.0.1", 0), Handler)
            thread = threading.Thread(target=server.serve_forever, daemon=True)
            thread.start()
            try:
                host, port = server.server_address
                base = f"http://{host}:{port}"
                with urllib.request.urlopen(f"{base}/api/mode", timeout=2) as response:
                    self.assertEqual(
                        json.loads(response.read().decode("utf-8")),
                        {"mode": "booth"},
                    )
                request = urllib.request.Request(
                    f"{base}/api/mode",
                    data=b'{"mode":"away"}',
                    method="POST",
                    headers={"Content-Type": "application/json"},
                )
                with urllib.request.urlopen(request, timeout=2) as response:
                    self.assertEqual(
                        json.loads(response.read().decode("utf-8")),
                        {"mode": "away"},
                    )
                with urllib.request.urlopen(base + "/", timeout=2) as response:
                    self.assertIn(b"Fulcrum Builds", response.read())
            finally:
                server.shutdown()
                server.server_close()
