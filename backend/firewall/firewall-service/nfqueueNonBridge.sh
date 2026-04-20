#!/bin/sh
set -e

sudo nft delete table inet fw_test 2>/dev/null || true

sudo nft add table inet fw_test
sudo nft add chain inet fw_test input { type filter hook input priority 0 \; }
sudo nft add chain inet fw_test forward { type filter hook forward priority 0 \; }
sudo nft add chain inet fw_test output { type filter hook output priority 0 \; }

sudo nft add rule inet fw_test input iifname != "lo" counter queue num 0-3 bypass

sudo nft add rule inet fw_test forward counter queue num 0-3 bypass
sudo nft add rule inet fw_test output oifname != "lo" counter queue num 0-3 bypass

echo "[*] Rules applied. Traffic is now balanced across queues 0, 1, 2, and 3."
echo "[*] Bypass mode is ENABLED (packets will pass if app is down)."
sudo nft list table inet fw_test