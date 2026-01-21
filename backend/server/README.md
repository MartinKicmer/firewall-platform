# Naklonovat repo
git clone https://github.com/pistacheio/pistache.git
cd pistache
meson setup build
meson install -C build

#Potom přidat includy
-#include <pistache/http.h>
-#include <pistache/router.h>
-#include <pistache/endpoint.h>