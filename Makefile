ASSETS = ./assets@/
FLAGS = -sUSE_WEBGL2 -sFULL_ES3 -sUSE_GLFW -sUSE_ZLIB -sMAIN_MODULE

#compiler flags
CC = emcc
CXX = em++

#paths
SRC = ./src
BIN = ./bin
LIB = ./lib
INC = ./include

WebITG:
# 	$(SRC)/**/*.cpp $(SRC)/**/*.c
	mkdir -p ./out/scripts
	em++ $(BIN)/*.wasm \
	$(FLAGS) \
	$(SRC)/*.cpp \
	-I$(INC)/* -I$(LIB)/mad/ -I$(LIB)/stb/ \
	-DWITHOUT_NETWORKING \
	--use-port=vorbis --use-port=ogg \
	-o out/scripts/OITG.js

#./bin/*.wasm
mkbindir:
	mkdir -p $(BIN)

binaries: mkbindir lua aes mad

# Lua-5.0
lua:
	$(CXX) -shared -I$(LIB)/$@/include -sSIDE_MODULE=2 $(LIB)/$@/src/*.c -Oz -o $(BIN)/$@.wasm $(LIB)/$@/src/lib/*.c

# libmad
mad:
	$(CXX) -shared -I$(LIB)/$@ -sSIDE_MODULE=2 $(LIB)/$@/*.c -O3 -L$(LIB)/$@ -o $(BIN)/$@.wasm -DFPM_64BIT -DNDEBUG

# # AES encryption?
aes:
	$(CXX) -shared -I$(LIB)/$@ -sSIDE_MODULE=2 $(LIB)/$@/*.c -O3 -L$(LIB)/$@ -o $(BIN)/$@.wasm

clean:
	rm -rf out/ bin/