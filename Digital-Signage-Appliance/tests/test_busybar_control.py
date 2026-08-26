from __future__ import annotations

import io
import os
import unittest
from contextlib import redirect_stderr, redirect_stdout
from unittest import mock

import busybar_control
from busylib import types


class BusyBarControlTests(unittest.TestCase):
    def test_message_payload_targets_both_display_sizes(self) -> None:
        payload = busybar_control.build_message_payload(
            "HELLO",
            display="both",
            font="bold",
            color="#40F4FFFF",
        )

        self.assertEqual(payload.application_name, "fulcrum-builds-signage")
        self.assertEqual(len(payload.elements), 2)
        self.assertEqual(
            [element.display for element in payload.elements],
            [types.DisplayName.FRONT, types.DisplayName.BACK],
        )
        self.assertEqual([element.width for element in payload.elements], [72, 160])
        self.assertTrue(all(element.x == 0 for element in payload.elements))
        self.assertTrue(all(element.align == "top_left" for element in payload.elements))

    def test_signage_payload_contains_booth_copy(self) -> None:
        payload = busybar_control.build_signage_payload()
        text = " ".join(
            element.text
            for element in payload.elements
            if isinstance(element, types.TextElement)
        )

        self.assertIn("FULCRUM BUILDS", text)
        self.assertIn("ASK ME ABOUT MY ROBOTS", text)
        self.assertEqual(payload.led_notification_color, "#FF982AFF")
        self.assertTrue(all(element.x == 0 for element in payload.elements))
        self.assertTrue(all(element.align == "top_left" for element in payload.elements))

    def test_printout_payload_contains_live_activity(self) -> None:
        payload = busybar_control.build_printout_payload(
            "Pulling kiosk launcher on the Pi",
            headline="CURSOR AGENT",
        )

        text = " ".join(
            element.text
            for element in payload.elements
            if isinstance(element, types.TextElement)
        )
        self.assertIn("LIVE", text)
        self.assertIn("Pulling kiosk launcher on the Pi", text)
        self.assertIn("CURSOR AGENT", text)
        self.assertEqual(payload.priority, 80)
        self.assertEqual(
            [element.display for element in payload.elements],
            [types.DisplayName.FRONT, types.DisplayName.BACK, types.DisplayName.BACK],
        )

    def test_missing_token_fails_without_contacting_api(self) -> None:
        stderr = io.StringIO()
        with (
            mock.patch.dict(os.environ, {}, clear=True),
            redirect_stderr(stderr),
        ):
            result = busybar_control.main(["status"])

        self.assertEqual(result, 2)
        self.assertIn("BUSYBAR_API_TOKEN is not set", stderr.getvalue())

    def test_dry_run_does_not_require_token(self) -> None:
        stdout = io.StringIO()
        with (
            mock.patch.dict(os.environ, {}, clear=True),
            redirect_stdout(stdout),
        ):
            result = busybar_control.main(["--dry-run", "signage"])

        self.assertEqual(result, 0)
        self.assertIn('"application_name": "fulcrum-builds-signage"', stdout.getvalue())

    def test_signage_command_uses_cloud_client_and_clears_first(self) -> None:
        client = mock.MagicMock()
        client.display_draw.return_value.result = "ok"
        context = mock.MagicMock()
        context.__enter__.return_value = client
        context.__exit__.return_value = False
        stdout = io.StringIO()

        with (
            mock.patch.dict(
                os.environ,
                {busybar_control.TOKEN_ENV_VAR: "test-token"},
                clear=True,
            ),
            mock.patch.object(busybar_control, "BusyBar", return_value=context) as factory,
            redirect_stdout(stdout),
        ):
            result = busybar_control.main(["signage"])

        self.assertEqual(result, 0)
        factory.assert_called_once_with(
            token="test-token",
            timeout=15.0,
            compatibility_mode="warn",
        )
        client.display_draw.assert_called_once()
        self.assertTrue(client.display_draw.call_args.kwargs["clear_before_draw"])
        self.assertTrue(client.display_draw.call_args.kwargs["sanitize_text"])
        self.assertEqual(stdout.getvalue().strip(), "ok")

    def test_brightness_validation(self) -> None:
        self.assertEqual(busybar_control.parse_brightness("auto"), "auto")
        self.assertEqual(busybar_control.parse_brightness("75"), 75)
        with self.assertRaises(Exception):
            busybar_control.parse_brightness("101")


if __name__ == "__main__":
    unittest.main()
