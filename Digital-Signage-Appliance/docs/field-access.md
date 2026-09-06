# Field SSH access

The appliance is designed to be recoverable from a new laptop without any
files from the original development Mac. The private keys live in 1Password;
only their public halves are installed on the Pi and committed here.

## Device record

Last discovery attempt: 2026-09-06.

| Property | Value | Status |
| --- | --- | --- |
| SSH account | `signage` | configured identity |
| Hostname | `xerogrid-signage` / `xerogrid-signage.local` | configured identity |
| USB fallback IPv4 | `10.12.194.1/28` | `rpi-usb-gadget` default; Pi did not answer it while bridged |
| USB product | Raspberry Pi USB Ethernet gadget, VID `0x2e8a`, PID `0x0013` | observed on Mac |
| USB host-side MAC | `ee:fd:d1:3c:5f:78` | current Mac interface `en10`; regenerated after USB re-enumeration |
| USB Pi-side MAC | `1e:bb:a6:7a:26:aa` | observed in the macOS USB bridge and confirmed by advertised EUI-64 |
| USB ICS bridge | Mac `192.168.2.1/24` | link active; Pi did not take a DHCP lease |
| Wi-Fi IPv4 | not observed | `192.168.251.183` rejected SSH; `192.168.7.2` was announced before the USB network reset but was unreachable |
| Wi-Fi MAC | not observed | pending live inventory |
| Wired Ethernet | disabled | intentional field configuration |

Run `scripts/device-inventory.sh` on the Pi after connecting and replace the
pending values above with live results.

## Keys

Both public keys are installed by `scripts/provision-field-access.sh`. The
matching private key items are stored in the 1Password **Private** vault.

| 1Password SSH Key item | SHA-256 fingerprint |
| --- | --- |
| `Mac SSH — brendan@Mac.lan` | `SHA256:bMrTE6IDxkWpkEJGSE3FIX9yePHwEyvaKFTFkooeJgI` |
| `Digital Signage Appliance — Backup SSH Key` | `SHA256:JL15JOfZDCNnUmkuTtTt6TkQ3ZUtrDg1y62/YzlhN14` |

Do not export or commit either private key.

## Connect from a new field laptop

1. Install 1Password and sign in to the account containing the Private vault.
2. In 1Password, enable **Settings > Developer > Use the SSH agent** and unlock
   the vault. No private-key file needs to be copied onto the laptop.
3. Plug the Pi's USB-C data/power port directly into the laptop and wait up to
   one minute for the USB Ethernet adapter.
4. Connect by hostname:

   ```sh
   ssh signage@xerogrid-signage.local
   ```

5. If mDNS is unavailable, use the gadget's standalone address:

   ```sh
   ssh signage@10.12.194.1
   ```

From a checkout of this repository, `./scripts/field-ssh.sh` tries both
addresses. On macOS, if the standalone address does not appear, share Wi-Fi to
the **Raspberry Pi USB Gadget** adapter in **System Settings > General >
Sharing > Internet Sharing**, then retry the hostname. Windows requires the
official Raspberry Pi RNDIS driver; macOS and Linux use CDC-ECM.

References:

- [Raspberry Pi USB gadget mode](https://www.raspberrypi.com/news/usb-gadget-mode-in-raspberry-pi-os-ssh-over-usb/)
- [1Password SSH agent](https://developer.1password.com/docs/ssh/agent/)

## Provision or repair the Pi

Run this from the project directory on the Pi:

```sh
sudo ./scripts/provision-field-access.sh
sudo reboot
```

The script is idempotent: it preserves existing authorized keys, adds both
field keys, sets the stable hostname, enables SSH and mDNS, and enables
`rpi-usb-gadget` when that package is installed. `install.sh` invokes it by
default; set `SIGNAGE_SKIP_FIELD_ACCESS=1` only for a deliberately isolated
installation.
