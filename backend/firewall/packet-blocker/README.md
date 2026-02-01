# USAGE:
- L2:
    packet-blocker -rid 1 -l L2 -action deny -smac ... -dmac ... -save(optional) 
- L3:
    packet-blocker -rid 1 -l L3 -action deny -sa ../prefix -da ../prefix -ttlMax ... -ttlMin ... -proto ... -allowFrag ... -tos ... -save(optional) 
- L4-simple:
    packet-blocker -rid 1 -l L4Simple -action deny -sport ... -dport ...  
- REDIRECT PACKETS:
    packet-blocker -l L3 -action deny redirect 10(all)
- SELECT RULES:
    packet-blocker select -rid 1(optional) -action deny -l L2 -memory false(true)  
- REMOVE RULES:
    packet-blocker remove -rid 1 -memory false(true)
- UPDATE RULES:
    packet-blocker update -rid 1 -memory false(true) -l LX .....    

# Libraries (Linux):
- sudo apt-get install nlohmann-json3-dev