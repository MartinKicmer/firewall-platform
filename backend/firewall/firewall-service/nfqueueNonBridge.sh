#!/bin/sh
set -e

sudo nft delete table inet fw_test 2>/dev/null || true

sudo nft add table inet fw_test
sudo nft add chain inet fw_test input { type filter hook input priority 0 \; }
sudo nft add chain inet fw_test forward { type filter hook forward priority 0 \; }
sudo nft add chain inet fw_test output { type filter hook output priority 0 \; }

sudo nft add rule inet fw_test input counter queue num 0
sudo nft add rule inet fw_test forward counter queue num 0
sudo nft add rule inet fw_test output counter queue num 0

echo "[*] Rules applied (In/Forward/Out). Current state:"
sudo nft list table inet fw_test