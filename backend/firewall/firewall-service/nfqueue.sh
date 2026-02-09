#!/bin/sh
set -e

# 1. Kompletní flush starých pokusů
sudo nft delete table inet fw_test 2>/dev/null || true

# 2. Vytvoření nové tabulky a chainů
sudo nft add table inet fw_test
sudo nft add chain inet fw_test input { type filter hook input priority 0 \; }
sudo nft add chain inet fw_test forward { type filter hook forward priority 0 \; }
sudo nft add chain inet fw_test output { type filter hook output priority 0 \; }

# 3. Přidání pravidel s počítadlem (counter), abys viděl, že tam data tečou
# Tohle pošle VEŠKERÝ IP provoz do fronty 0
sudo nft add rule inet fw_test input counter queue num 0
sudo nft add rule inet fw_test forward counter queue num 0

echo "[*] Rules applied. Current state:"
sudo nft list table inet fw_test