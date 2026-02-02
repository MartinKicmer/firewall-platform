# Pistache framework
## Clone Repo
1. git clone https://github.com/pistacheio/pistache.git
2. cd pistache
3. meson setup build
4. meson install -C build
- #include <pistache/http.h>
- #include <pistache/router.h>
- #include <pistache/endpoint.h>

# WebSocket framework

- sudo apt update
- sudo apt install cmake g++ libssl-dev zlib1g-dev

# IXWebSocket
1. git clone https://github.com/machinezone/IXWebSocket.git
2. cd IXWebSocket
3. mkdir build && cd build
4. cmake ..
5. make -j$(nproc)
6. sudo make install
7. sudo ldconfig

# Boost Process lib
-  sudo apt install libboost-all-dev
