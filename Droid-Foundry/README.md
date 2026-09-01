# Droid Foundry

PlatformIO firmware for convention animatronics. The microcontroller for
current builds is an **Arduino Nano ATmega328P with the old bootloader**
(`nanoatmega328` in PlatformIO).

The London IG-12 in `Assassin Droid/FinalVersionLondon.cpp` has been running
in production for over a year and is **reference only**. Do not treat that
unit as a deployment target.

Convention status and hardware notes: [PROJECT_LOG.md](PROJECT_LOG.md).

The original Arduino-IDE experiments live in the archived
[`xerogrid/Droid-Factory`](https://github.com/xerogrid/Droid-Factory) snapshot.
Booth signage lives in [`xerogrid/Projects`](https://github.com/xerogrid/Projects).

## Branches

| Branch | Meaning |
| --- | --- |
| `main` | Last known-good firmware for every droid. Flash this if the convention started tomorrow. |
| `dev` | Workshop. All three droids can be dirty here at once. |

`dev` is not production. Promote with a pull request whose diff is under **one**
droid folder. Merging all of `dev` into `main` will take every unfinished
experiment with it.

Feature branches like `pit/pir-tune` or `assassin/debounce` are better than
piling unrelated work on `dev` if more than one droid is in flight.

## Layout

| Path | What belongs there |
| --- | --- |
| `Assassin Droid/` | IG-12 firmware, wiring notes, and mechanical references |
| `Battle Droid/` | Battle Droid head firmware and bench sketches |
| `Pit Droid/` | Pit Droid PIR firmware, BoM, and wiring notes |
| `Audio Files/` | Vocoder clips for the assassin droid DFPlayer |
| `Archive/` | Retired sketches kept so old boards can be reconstructed |
| `WIP Sketches/` | Unfinished experiments, not production |
| `platformio.ini` | Named environments that compile the production firmware files |

Loose bench sketches (serial, IR, breakout, basic signal) live under
`Archive/Bench-Tests/`.

## Production firmware

| Environment | Source | Use |
| --- | --- | --- |
| `assassin-london` | `Assassin Droid/FinalVersionLondon.cpp` | IG-12 reference (servos, NeoPixels, DFPlayer, cockpit switches) |
| `pit-droid-pir` | `Pit Droid/Version 1/WorkingPIRv1.cpp` | Pit Droid demo head, PIR + dual servo + LEDs |
| `battle-droid-pir` | `Battle Droid/BattleDroidWorkingV1PIR.cpp` | Battle Droid demo head, PIR + dual servo |

```bash
pio run                          # all three default environments
pio run -e pit-droid-pir         # one droid
pio run -e pit-droid-pir -t upload
pio device monitor -b 9600
```

Libraries are pulled by PlatformIO from `lib_deps`. There is no vendored
`libraries/` folder here; that only exists in the archived Droid Factory repo.

## Hardware notes

- Assassin droid pin map, servo types, and switch wiring:
  `Assassin Droid/readme.txt`
- Pit Droid BoM and wiring: `Pit Droid/Version 1/BoM & Wiring Notes`
- Assassin droid CAD / diagrams: `IG-12 Core Power Pack v1.0.step` and the
  wiring diagram PNG/SVG next to it
- Vocoder clips: `Audio Files/`

`src/` only contains one-line wrappers. Named environments compile those
wrappers so PlatformIO still resolves libraries; the real firmware stays in
the droid folders.

## New work

1. Put new production firmware in the matching droid folder, not in `src/`.
2. Add a one-line wrapper under `src/` and a matching `[env:...]` block.
3. Leave historical sketches in `Archive/` or `WIP Sketches/`.
4. Keep folder names with spaces; they match the hardware documentation.
5. Open a PR to `main` that only contains the droid you are promoting.

## License

Original project code and documentation are under the [MIT
License](LICENSE). Third-party components keep their own terms; see
[Third-Party Notices](THIRD_PARTY_NOTICES.md).
