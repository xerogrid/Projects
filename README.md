# Maker Projects

This repository is a small monorepo for related convention-display and droid
hardware projects. Each project owns its dependencies, documentation, and
tests; the repository root owns shared housekeeping and CI.

## Projects

- [`Digital-Signage-Appliance`](Digital-Signage-Appliance/) — Raspberry Pi
  convention signage and BUSY Bar integration.
- [`Droid-Foundry`](Droid-Foundry/) — PlatformIO-based Arduino animatronics
  and robotics projects.
- [`Audio Files`](Audio%20Files/) — small source audio assets shared by the
  physical builds.

## Conventions

- Keep generated environments (`.venv`, `.pio`, caches) out of Git.
- Put project-specific documentation and commands in that project's README.
- Use kebab-case for new machine-facing paths. Existing display names with
  spaces are retained to avoid breaking hardware documentation.
- Store large, versioned release artifacts in GitHub Releases rather than in
  the source tree.
- The default branch is `main`.

## Checks

```bash
cd Digital-Signage-Appliance
python3 -m unittest discover -s tests

cd ../Droid-Foundry
pio run
```

## License

Original project code and documentation are available under the [MIT
License](LICENSE). Third-party components retain their own terms; see
[Third-Party Notices](THIRD_PARTY_NOTICES.md).
