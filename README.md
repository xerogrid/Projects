# Maker Projects

Convention-display and droid hardware work in one repository. Each project
owns its firmware or app, documentation, and checks. This root owns license,
ignore rules, and CI.

## Projects

| Path | What it is |
| --- | --- |
| [`Digital-Signage-Appliance`](Digital-Signage-Appliance/) | Raspberry Pi kiosk signage and BUSY Bar control for the Fulcrum Builds booth |
| [`Droid-Foundry`](Droid-Foundry/) | PlatformIO firmware for Pit Droid, Battle Droid, and IG-12 assassin droid builds |
| [`Audio Files`](Audio%20Files/) | Vocoder clips used by the assassin droid DFPlayer |

The original Arduino-IDE experiments live in the archived
[`xerogrid/Droid-Factory`](https://github.com/xerogrid/Droid-Factory) snapshot.
Do not start new work there.

## Default branch

`main`. Feature branches are fine; do not treat `dev` or `master` as the
source of truth.

## Conventions

- Keep generated trees (`.venv`, `.pio`, editor caches) out of Git.
- Put project-specific commands in that project's README.
- Use kebab-case for new machine-facing paths. Existing display names with
  spaces stay as they are so hardware notes and pin maps keep matching the
  folders on disk.
- Store large versioned release artifacts in GitHub Releases, not in the tree.
- Local network names, SSH keys, and booth IP addresses stay off GitHub.

## Checks

```bash
cd Digital-Signage-Appliance
python3 -m unittest discover -s tests

cd ../Droid-Foundry
pio run
```

`pio run` builds the named production environments in
`Droid-Foundry/platformio.ini` (assassin, pit, and battle PIR firmware). It
does not compile every archived sketch.

## License

Original project code and documentation are under the [MIT
License](LICENSE). Third-party components keep their own terms; see
[Third-Party Notices](THIRD_PARTY_NOTICES.md).
