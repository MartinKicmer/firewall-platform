#!/bin/bash
set -e

X_QUEUES="$1"

if [ -z "${X_QUEUES}" ]; then
    echo "Usage: sudo $0 <number_of_queues>"
    echo "Example: sudo $0 1"
    exit 1
fi

if [ "$X_QUEUES" -eq 1 ]; then
    QUEUE_RANGE="0"
else
    MAX_QUEUE=$((X_QUEUES - 1))
    QUEUE_RANGE="0-$MAX_QUEUE"
fi

echo "[*] Cleaning old IP tables..."
sudo nft delete table inet fw_test 2>/dev/null || true

echo "[*] Setting up NFQUEUE for $X_QUEUES queues (Range: $QUEUE_RANGE)..."

sudo nft add table inet fw_test

sudo nft add chain inet fw_test input { type filter hook input priority 0 \; policy accept \; }
sudo nft add chain inet fw_test forward { type filter hook forward priority 0 \; policy accept \; }
sudo nft add chain inet fw_test output { type filter hook output priority 0 \; policy accept \; }


echo "[*] Adding rules to queues..."

sudo nft add rule inet fw_test input iifname != "lo" counter queue num "$QUEUE_RANGE"

sudo nft add rule inet fw_test forward counter queue num "$QUEUE_RANGE"

sudo nft add rule inet fw_test output oifname != "lo" counter queue num "$QUEUE_RANGE"

echo ""
echo "[✓] Rules applied. Traffic is now directed to: $QUEUE_RANGE"
sudo nft list table inet fw_test