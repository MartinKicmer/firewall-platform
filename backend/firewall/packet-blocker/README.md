# USAGE:
- L2:
    packet-blocker -rid 1 -l L2 -action deny -smac ... -dmac ... -save(optional) 
- L3:
    packet-blocker -rid 1 -l L3 -action deny -sa ../prefix -da ../prefix -ttlMax ... -ttlMin ... -proto ... -allowFrag ... -tos ... -save(optional) 
- REDIRECT PACKETS:
    packet-blocker -l L3 -action deny redirect 10(count)    

# Libraries (Linux):
- sudo apt-get install nlohmann-json3-dev