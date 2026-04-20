#!/bin/bash
set -e

CONFIG_FILE="$1"

if [ -z "${CONFIG_FILE}" ]; then
    echo "Usage: $0 <config_file>"
    exit 1
fi

echo "[*] Reading config..."

INPUT_IF=$(grep "interface:" "$CONFIG_FILE" | grep "input"  | cut -d':' -f2 | cut -d',' -f1 | xargs)
OUTPUT_IF=$(grep "interface:" "$CONFIG_FILE" | grep "output" | cut -d':' -f2 | cut -d',' -f1 | xargs)

if [ -z "$INPUT_IF" ] || [ -z "$OUTPUT_IF" ]; then
    echo "[!] Could not parse interfaces from config"
    exit 1
fi

echo "[*] Input interface : $INPUT_IF"
echo "[*] Output interface: $OUTPUT_IF"

BRIDGE_NAME="brfw"

echo "[*] Cleaning old setup..."

sudo ip link set $INPUT_IF down || true
sudo ip link set $OUTPUT_IF down || true
sudo ip link delete $BRIDGE_NAME type bridge 2>/dev/null || true
# Mažeme tabulku bridge (L2 filtrování)
sudo nft delete table bridge brfw 2>/dev/null || true

echo "[*] Creating L2 bridge..."

sudo ip link add name $BRIDGE_NAME type bridge
sudo ip link set $INPUT_IF master $BRIDGE_NAME
sudo ip link set $OUTPUT_IF master $BRIDGE_NAME

sudo ip link set $INPUT_IF up
sudo ip link set $OUTPUT_IF up
sudo ip link set $BRIDGE_NAME up

echo "[*] Bridge created:"
bridge link

echo "[*] Setting up NFQUEUE (Multiple Queues)..."

sudo nft add table bridge brfw
sudo nft add chain bridge brfw prerouting { type filter hook prerouting priority -300 \; policy accept \; }

sudo nft add rule bridge brfw prerouting iif "$INPUT_IF" counter queue num 0-3
sudo nft add rule bridge brfw prerouting iif "$OUTPUT_IF" counter queue num 0-3

echo ""
echo "[✓] Final nft state:"
sudo nft list table bridge brfw

echo ""
echo "[✓] Setup complete."
echo "Running on 4 parallel queues (0-3)."