# USAGE:
- L2:
    packet-blocker -rid 1 -l L2 -action deny -smac ... -dmac ... 
- L3:
    packet-blocker -rid 1 -l L3 -action deny -sa ../prefix -da ../prefix -ttlMax ... -ttlMin ...  

# Libraries (Linux):
- sudo apt-get install nlohmann-json3-dev