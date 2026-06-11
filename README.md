
**NOTE**: Niko - Hai, I will not be reviving the original repository for the reason that I wish to not bother thoes who have put their time and effort into a great project that is now **archived**.

*~~This project is dead and has been archived. Thanks to those who have helped made it what it is for the last 12 years.~~*

Further ideas and contributions should be made toward the following projects instead:

 * NotITG (more featureful and up-to-date fork of this project): https://noti.tg/
 * OpenITG (parent project): https://github.com/openitg/openitg
 * StepMania (grand parent project): https://github.com/stepmania/stepmania. 


# WebITG

An open-source rhythm dancing game which is a fork of StepMania 3.95
with the goal of adding arcade-like ITG-style behavior and serving as a drop-in
replacement for the ITG binary on arcade cabinents.

* Project homepage: https://github.com/crittericallyonline/webitg/wiki
* Project bug tracker: https://github.com/crittericallyonline/webitg/issues
* Project source code: https://github.com/crittericallyonline/webitg

## TODOs

### Short-term

1. Getting Started Guide - build and development
2. self-contained cache-rebuilding solution
3. OpenGL Driver uses fix function pipeline rather than shader

### Long-term

1. StepMania 4.0 LUA Bindings
2. StepMania 4.0 Theme metrics

## How to check-out the source

```sh
git clone https://github.com/crittericallyonline/webitg.git
```

## How to contribute

1. Create an account at github.com
2. Goto https://github.com/openitg/openitg
3. Click "fork"
4. `git clone https://github.com/<username>/openitg.git`
5. Edit files...
6. `git add <filename>` for every file you add or edit
7. `git commit` # now your change is committed locally
8. `git push` # now your change is pushed to your github
9. From `https://github.com/<username>/webitg`, click "pull request".  Base branch is the
branch you want to put your changes on, and head branch is the branch you made
your changes to already.
10. Write a short description of your change.  Be sure to include the goal, any
bugs fixed, features added, etc, and any credit you wish to have.  Click "send
pull request".

## How to build for wasm

Dependencies:
- [CMake](https://cmake.org)
- [Emscripten](https://emscripten.org)

### Building
1. Edit the [CMakeLists.txt](CMakeLists.txt) file to customize the options given in any specific way you like/prefer.
2. Build [FFmpeg](https://github.com/ffmpeg/ffmpeg):
```sh
cd extern/FFmpeg && \
emconfigure ./configure --disable-asm --ar=emar --arch=x86 --cc=emcc --cxx=em++ --ranlib=emranlib --nm=emnm --disable-doc --disable-programs --enable-gpl --disable-network && \
emmake make -j -s # its laggy but faster, to make your pc usable remove -j or specify -j(NUMBER OF CORES)
```
3. make a entry for [config.h](src/config.h) in the src directory
- src/config.h
```h
#ifndef STEPMANIA_CONFIG_H
#define STEPMANIA_CONFIG_H
#define PRODUCT_NAME "OpenITG"
#define PRODUCT_VER "alpha 6 DEV"
// #define PRODUCT_VER "3.9 alpha 23"
#define PRODUCT_NAME_VER "${PRODUCT_NAME} ${PRODUCT_VER}"

// String used for the install directory and registry locations
// Official releases = "StepMania"; intermediate releases = "StepMania CVS".
#define PRODUCT_ID "OpenITG alpha4"

#define BUILD_DATE "202060531"
#define BUILD_REVISION_TAG "0.1.0"
#define BUILD_VERSION "unknown version"
#define BUILD_REV_DATE "202060531"
#define BUILD_REV_TAG "0.1.0"
#endif // STEPMANIA_CONFIG_H
```
4. Find your `Emscripten.cmake` Toolchain file, if you followed the [Emscripten setup](https://emscripten.org/docs/getting_started/downloads.html) it should be located in your $HOME directory, example: `~/emsdk/cmake/Modules/Platform/Emscripten.cmake`, if you installed it by system package, it will be located in `/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake`, we will use this as a cmake toolchain argument
5. Create the directory in which you will put the final build of the wasm in. `mkdir -p <BUILD_DIR>` then `cd <BUILD_DIR>`
    - . for how many directorys youve made, you will put `../` for each directory entered when building to reach `CMakeLists.txt`.
6. `emcmake cmake ../