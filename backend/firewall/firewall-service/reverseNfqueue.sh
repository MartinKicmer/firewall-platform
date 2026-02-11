#!/bin/sh

echo "[*] Cleaning up nftables rules..."

sudo nft delete table inet fw_test 2>/dev/null

if [ $? -eq 0 ]; then
    echo "[+] Table 'inet fw_test' successfully removed. Internet should flow normally."
else
    echo "[!] Table 'inet fw_test' did not exist or could not be removed."
fi
sudo systemctl restart NetworkManager
echo "Restarted NetworkManager"
echo "[*] Current nftables state (filter tables):"
sudo nft list tables | grep fw_test || echo "No firewall tables found."