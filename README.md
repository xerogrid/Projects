# Maker Projects

Convention-display work for the Fulcrum Builds booth.

Droid firmware is no longer in this repository. It lives in
[`xerogrid/Droid-Foundry`](https://github.com/xerogrid/Droid-Foundry)
(`main` is production, `dev` is the workshop branch). The original
Arduino-IDE experiments are the archived
[`xerogrid/Droid-Factory`](https://github.com/xerogrid/Droid-Factory)
snapshot.

## Projects

| Path | What it is |
| --- | --- |
| [`Digital-Signage-Appliance`](Digital-Signage-Appliance/) | Raspberry Pi kiosk signage and BUSY Bar control |

## Default branch

`main`.

## Conventions

- Keep generated trees (`.venv`, editor caches) out of Git.
- Put project-specific commands in that project's README.
- Local network names, SSH keys, and booth IP addresses stay off GitHub.

## Checks

```bash
cd Digital-Signage-Appliance
python3 -m unittest discover -s tests
```

## License

Original project code and documentation are under the [MIT
License](LICENSE). Third-party components keep their own terms; see
[Third-Party Notices](THIRD_PARTY_NOTICES.md).
