/**
 * BambuLabs VLC Streams – Stream Deck Plugin
 * --------------------------------------------------
 * Node.js entry point (ESM) loaded by Stream Deck 6.4+
 *
 * On key press:
 *   1. Updates the button image to a "loading" state
 *   2. Spawns the Python quadrant launcher script
 *   3. Shows a checkmark (success) or alert (error) when done
 */

import streamDeck, { LogLevel } from "@elgato/streamdeck";
import { execFile, spawn } from "node:child_process";
import { existsSync } from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

const __dirname = path.dirname(fileURLToPath(import.meta.url));

// Path to the Python launcher script (bundled inside the plugin)
const SCRIPT_PATH = path.join(__dirname, "..", "scripts", "bambu_vlc_quadrant.py");

// Configure SDK logging
streamDeck.logger.setLevel(LogLevel.DEBUG);

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

/**
 * Locate a usable Python 3 executable on the host machine.
 * Returns the executable name/path, or null if not found.
 */
async function findPython() {
  const candidates = ["py", "python3", "python"];
  for (const cmd of candidates) {
    const found = await new Promise((resolve) => {
      execFile(cmd, ["--version"], { shell: true }, (err, stdout, stderr) => {
        const out = (stdout + stderr).toLowerCase();
        resolve(!err && out.startsWith("python 3") ? cmd : null);
      });
    });
    if (found) return found;
  }
  return null;
}

// ---------------------------------------------------------------------------
// Action: Open Printer Streams
// ---------------------------------------------------------------------------

const openStreamsAction = {
  UUID: "com.bambu.vlcstreams.openstreams",

  /** Called when the user presses the key down. */
  async onKeyDown(ev) {
    const { action, payload } = ev;

    streamDeck.logger.info("BambuLabs VLC Streams: key pressed");

    // Visual feedback – show a temporary title while working
    await action.setTitle("...");

    if (!existsSync(SCRIPT_PATH)) {
      streamDeck.logger.error(`Script not found: ${SCRIPT_PATH}`);
      await action.setTitle("Error");
      await action.showAlert();
      return;
    }

    const python = await findPython();
    if (!python) {
      streamDeck.logger.error("Python 3 not found on this system.");
      await action.setTitle("No Py");
      await action.showAlert();
      return;
    }

    // Spawn the script detached so it keeps running after Stream Deck exits
    const child = spawn(python, [SCRIPT_PATH], {
      detached: true,
      stdio: "ignore",
      shell: true,
      windowsHide: false, // Show console window so the user can see progress
    });

    child.unref(); // Don't wait for it; let it run independently

    // Give the user immediate positive feedback
    await action.setTitle("OK");
    await action.showOk();

    // Reset the title after a short delay
    setTimeout(() => action.setTitle(""), 2000);

    streamDeck.logger.info(`Launched: ${python} "${SCRIPT_PATH}" (pid ${child.pid ?? "?"})`);
  },

  /** Called when the action instance is first loaded (key appears on deck). */
  async onWillAppear(ev) {
    // Ensure the title starts blank; icon alone is sufficient
    await ev.action.setTitle("");
  },
};

// ---------------------------------------------------------------------------
// Register & connect
// ---------------------------------------------------------------------------

streamDeck.actions.registerAction(openStreamsAction);

streamDeck.connect();
