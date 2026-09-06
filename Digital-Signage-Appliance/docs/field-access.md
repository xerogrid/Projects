# Field SSH access

The appliance is designed to be recoverable from a new laptop without any
files from the original development Mac. The private keys live in 1Password;
only their public halves are installed on the Pi and committed here.

## Device record

Last verified: 2026-09-06, including a Pi reboot and a fresh USB DHCP lease
on the Mac.

| Property | Value | Status |
| --- | --- | --- |
| SSH account | `signage` | verified over USB |
| Hostname | `xerogrid-signage` / `xerogrid-signage.local` | verified |
| USB fallback IPv4 | `192.168.7.2/24` | ping, TCP port 22, and SSH verified after reboot |
| Pi USB service | `xerogrid-usb-gadget-network.service` | active; owns the custom USB network configuration |
| Pi USB DHCP | `192.168.7.10` through `192.168.7.50` | `dnsmasq`; advertises no router and no DNS server |
| Mac USB network service | `Raspberry Pi USB Gadget` | DHCP; received `192.168.7.25/24`, Router blank, IPv6 none |
| USB product | Raspberry Pi USB Ethernet gadget, VID `0x2e8a`, PID `0x0013` | observed on Mac |
| Wi-Fi | independent uplink | not required for USB SSH |
| Wired Ethernet | disabled | intentional field configuration |

The USB subnet is deliberately local-only. Because its DHCP response contains
no default router or DNS server, plugging in the Pi does not replace the Mac's
Wi-Fi route or interrupt internet access.

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
4. On macOS, leave the **Raspberry Pi USB Gadget** network service set to
   **Using DHCP**. Its Router field must remain blank. Do not enable Internet
   Sharing for this adapter and do not add a manual router or DNS server.
5. Connect by hostname:

   ```sh
   ssh signage@xerogrid-signage.local
   ```

6. If mDNS is unavailable, use the Pi's fixed USB address:

   ```sh
   ssh signage@192.168.7.2
   ```

From a checkout of this repository, `./scripts/field-ssh.sh` tries both
addresses. The script uses normal SSH agent discovery, so the unlocked
1Password SSH agent supplies either authorized field key. Windows requires a
compatible RNDIS driver; macOS and Linux use CDC-ECM. On any platform, let the
USB adapter use DHCP without installing its offered network as a default route.

References:

- [Raspberry Pi USB gadget mode](https://www.raspberrypi.com/news/usb-gadget-mode-in-raspberry-pi-os-ssh-over-usb/)
- [1Password SSH agent](https://developer.1password.com/docs/ssh/agent/)

## Provision or repair the Pi

Run this from the project directory on the Pi:

```sh
sudo ./scripts/provision-field-access.sh
```

The script is idempotent: it preserves existing authorized keys, adds both
field keys, sets the stable hostname, enables SSH and mDNS, and enables
`xerogrid-usb-gadget-network.service` when the appliance bootstrap has
installed it. It never calls Raspberry Pi's `rpi-usb-gadget` helper because
that helper conflicts with the appliance's fixed `192.168.7.2/24` topology.
`install.sh` invokes the provisioning script by default; set
`SIGNAGE_SKIP_FIELD_ACCESS=1` only for a deliberately isolated installation.
