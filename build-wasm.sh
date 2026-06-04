#!/bin/bash

mkdir -p out
cd out
emcmake cmake .. -B . -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake
if emmake make -s -j; then
    # emstrip --strip-unneeded -S WebITG.js
    # cp WebITG.html ../html
    cp WebITG.js ../html
    cp WebITG.wasm ../html
else
    @echo "ERROR WITH COMPILING"
fi