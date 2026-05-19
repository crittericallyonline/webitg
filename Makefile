ASSETS = ./assets@/
FLAGS = -sUSE_WEBGL2 -sFULL_ES3 -sUSE_GLFW -sUSE_ZLIB

#compiler flags
CC = emcc
CXX = em++

#paths
SRC = ./src
BIN = ./bin
LIB = ./lib

WebITG:
	mkdir -p ./out/scripts
	em++ $(BIN)/*.wasm -sMAIN_MODULE \
	$(FLAGS) $(SRC)/*.cpp $(SRC)/**/*.cpp $(SRC)/**/*.c \
	-I$(LIB) -I$(LIB)/lua/include/ -I$(LIB)/mad/ -I$(LIB)/stb/ \
	-DWITHOUT_NETWORKING \
	--use-port=vorbis --use-port=ogg \
	-o out/scripts/OITG.js

#./bin/*.wasm
mkbindir:
	mkdir -p $(BIN)

binaries: mkbindir lua aes mad

# Lua-5.0
lua:
	$(CC) -shared -I$(LIB)/$@/include -sSIDE_MODULE $(LIB)/$@/src/*.c -O3 -o $(BIN)/$@.wasm -L$(LIB)/$@/src/lib

# libmad
mad:
	$(CC) -shared -I$(LIB)/$@ -sSIDE_MODULE $(LIB)/$@/*.c -O3 -L$(LIB)/$@ -o $(BIN)/$@.wasm -DFPM_64BIT -DNDEBUG

# # AES encryption?
aes:
	$(CC) -shared -I$(LIB)/$@ -sSIDE_MODULE $(LIB)/$@/*.c -O3 -L$(LIB)/$@ -o $(BIN)/$@.wasm

clean:
	rm -rf out/ bin/