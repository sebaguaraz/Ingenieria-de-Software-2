#!/bin/bash
set -e

# Build "en criollo":
# 1) compila json_db como libreria dinamica (DLL/SO)
# 2) compila http_bridge como libreria dinamica (DLL/SO)
# 3) compila un ejecutable chiquito que solo llama al bridge
#
# Que prevenimos con esto:
# - recompilar siempre headers gigantes (httplib/json)
# - acoplar toda la app al main.cpp
# - builds lentos cuando solo cambias logica de app
mkdir build -Force
mkdir server\uploads -Force

COMMON_FLAGS="-O3 -s -std=c++17 -I server/include"

if [[ "$OS" == "Windows_NT" ]] || [[ "$(uname -s)" == MINGW* ]] || [[ "$(uname -s)" == MSYS* ]]; then
  echo "[build] Windows (DLL)"

  # JSON como DLL.
  g++ -O3 -s -std=c++17 -I server\include -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DJSONDB_EXPORTS -shared server\src\json_db.cpp "-Wl,--out-implib,build\libjsondb.dll.a" -o build\jsondb.dll

  # HTTP bridge como DLL, enlazando jsondb y sockets de Windows.
  g++ -O3 -s -std=c++17 -I server\include -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -DHTTPBRIDGE_EXPORTS -shared server\src\http_bridge.cpp -Lbuild -ljsondb -lws2_32 "-Wl,--out-implib,build\libhttpbridge.dll.a" -o build\httpbridge.dll

  # Binario final liviano.
  g++ -O3 -s -std=c++17 -I server\include -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 server\main.cpp -Lbuild -lhttpbridge -ljsondb -lws2_32 -o build\intraned.exe

  echo "[ok] Ejecutar: ./build/intraned.exe"

else
  echo "[build] GNU/Linux (SO)"

  # JSON como .so.
  g++ $COMMON_FLAGS -fPIC -DJSONDB_EXPORTS \
    -shared server/src/json_db.cpp \
    -o build/libjsondb.so

  # HTTP bridge como .so.
  g++ $COMMON_FLAGS -fPIC -DHTTPBRIDGE_EXPORTS \
    -shared server/src/http_bridge.cpp \
    -Lbuild -ljsondb \
    -o build/libhttpbridge.so

  # Binario final con rpath local para encontrar .so en /build.
  g++ $COMMON_FLAGS server/main.cpp \
    -Lbuild -lhttpbridge -ljsondb \
    -Wl,-rpath,'$ORIGIN' \
    -o build/intraned

  echo "[ok] Ejecutar: ./build/intraned"
fi
