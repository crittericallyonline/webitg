#!/bin/bash
BUILD_REV_TAG=0
BUILD_REV_VERSION=0
BUILD_REV_DATE="$(date +%k%M%S%Y%m%d)"
BUILD_DATE="$(date +%Y-%m-%d)"
BUILD_REVISION_TAG="master"
BUILD_VERSION=0



if [ -e ./src/config.h ]; then
    echo "./src/config.h Found"
else
    echo "\
// Config, hand typed by Niko (@crittericallyonline)
#ifndef STEPMANIA_CONFIG_H
#define STEPMANIA_CONFIG_H
#define PRODUCT_NAME \"WebITG\"
#define PRODUCT_VER \"alpha 0 DEV\"
#define BUILD_REV_VERSION ${BUILD_REV_VERSION}
#define BUILD_REV_TAG ${BUILD_REV_TAG}
#define BUILD_REV_DATE \"${BUILD_REV_DATE}\"
#define BUILD_VERSION ${BUILD_VERSION}
#define BUILD_REVISION_TAG \"${BUILD_REVISION_TAG}\"
#define BUILD_DATE \"${BUILD_DATE}\"
#endif // STEPMANIA_CONFIG_H
" > ./src/config.h
    echo "Written Config to ./src/config.h"
fi;


mkdir -p out
cd out
emcmake cmake .. -B .
if emmake make -s -j8; then
    # emstrip --strip-unneeded -S WebITG.js
    # cp WebITG.html ../html
    cp WebITG.js ../html
    cp WebITG.wasm ../html
    cp WebITG.data ../html
else
    echo "ERROR WITH COMPILING"
fi