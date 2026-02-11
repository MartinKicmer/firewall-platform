#!/bin/sh
set -e

BRIDGE_NAME="$1"

if [ -z "$BRIDGE_NAME" ]; then 
    echo "Choose bridge name"
    exit 1
fi

echo "[*] Removing nftables bridge table"
sudo nft delete table bridge brfw 2>/dev/null || true

echo "[*] Removing bridge device $BRIDGE_NAME"
sudo ip link set dev "$BRIDGE_NAME" down 2>/dev/null || true
sudo ip link delete "$BRIDGE_NAME" type bridge 2>/dev/null || true

echo "[*] Restoring interfaces"
sudo nmcli device reapply enp10s0
sudo nmcli device reapply eth1

echo "[*] Restarting NetworkManager"
sudo systemctl restart NetworkManager

echo "[✓] Reversal complete"
