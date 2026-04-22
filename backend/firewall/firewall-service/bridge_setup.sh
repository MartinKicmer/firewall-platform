#!/bin/bash
set -e

CONFIG_FILE="$1"
X_QUEUES="$2"

if [ -z "${CONFIG_FILE}" ] || [ -z "${X_QUEUES}" ]; then
    echo "Usage: sudo $0 <config_file> <number_of_queues>"
    echo "Example: sudo $0 config.sql 1"
    exit 1
fi

# --- VÝPOČET ROZSAHU FRONT ---
if [ "$X_QUEUES" -lt 1 ]; then
    echo "[!] Number of queues must be at least 1"
    exit 1
fi

if [ "$X_QUEUES" -eq 1 ]; then
    QUEUE_RANGE="0"
else
    MAX_QUEUE=$((X_QUEUES - 1))
    QUEUE_RANGE="0-$MAX_QUEUE"
fi

echo "[*] Starting Firewall Bridge Setup..."
echo "[*] Config file  : $CONFIG_FILE"
echo "[*] Target queues: $X_QUEUES (Range: $QUEUE_RANGE)"

INPUT_IF=$(grep "interface:" "$CONFIG_FILE" | grep "input" | awk '{print $2}' | xargs)
OUTPUT_IF=$(grep "interface:" "$CONFIG_FILE" | grep "output" | awk '{print $2}' | xargs)

if [ -z "$INPUT_IF" ] || [ -z "$OUTPUT_IF" ]; then
    echo "[!] Could not parse interfaces. Check your config file format."
    exit 1
fi

BRIDGE_NAME="brfw"

echo "[*] Cleaning old setup..."
sudo ip link set $INPUT_IF down || true
sudo ip link set $OUTPUT_IF down || true
sudo ip link delete $BRIDGE_NAME type bridge 2>/dev/null || true
sudo nft delete table bridge brfw 2>/dev/null || true

echo "[*] Creating L2 bridge ($INPUT_IF <-> $OUTPUT_IF)..."
sudo ip link add name $BRIDGE_NAME type bridge
sudo ip link set $INPUT_IF master $BRIDGE_NAME
sudo ip link set $OUTPUT_IF master $BRIDGE_NAME

sudo ip link set $INPUT_IF up
sudo ip link set $OUTPUT_IF up
sudo ip link set $BRIDGE_NAME up

echo "[*] Bridge status:"
bridge link show $BRIDGE_NAME

echo "[*] Configuring nftables NFQUEUE..."

sudo nft add table bridge brfw
sudo nft add chain bridge brfw prerouting { type filter hook prerouting priority -300 \; policy accept \; }

sudo nft add rule bridge brfw prerouting iif "$INPUT_IF" counter queue num "$QUEUE_RANGE"
sudo nft add rule bridge brfw prerouting iif "$OUTPUT_IF" counter queue num "$QUEUE_RANGE"

echo ""
echo "[✓] Final nftables configuration:"
sudo nft list table bridge brfw

echo ""
echo "[✓] Setup complete."
echo "[!] Now run: sudo ./firewallService -debug -queues $X_QUEUES"