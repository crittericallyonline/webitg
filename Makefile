# # ASSETS = ./assets@/

# CWD=${realpath .}
# TMP=$(CWD)/tmp
# LIB=$(CWD)/lib
# SRC=$(CWD)/src
# ARC=$(CWD)/archive

# CC=emcc
# CXX=em++

# BUILD_DIR = tmp
# OPTIMIZATIONS=-Oz
# EMFLAGS = -sUSE_ZLIB --use-port=vorbis --use-port=ogg -pthread
# WEBGL_FLAGS = -sFULL_ES3=0 --use-port=contrib.glfw3 -sGL_ENABLE_GET_PROC_ADDRESS -sSAFE_HEAP -sLEGACY_GL_EMULATION=1 -lGL \
# -sMIN_WEBGL_VERSION=2 \
# -sMAX_WEBGL_VERSION=2 \
# -sGL_UNSAFE_OPTS=1 \
# -sASSERTIONS

# CFLAGS = -MMD -MP $(OPTIMIZATIONS) $(EMFLAGS)
# INCLUDES = \
# -I$(LIB)/lua/include/ \
# -I$(LIB)/aes/src/ \
# -I$(LIB)/mad/ \
# -I$(LIB)/libusb/include/ \
# -include src/EmscriptenHelper.h \
# -include src/StdString.h \
# -include src/global.h \
# -include src/ProductInfo.h \
# -I /home/niko/.cache/emscripten/ports/contrib.glfw3/include

# SOURCES := $(shell find $(SRC) -name '*.cpp')
# SOURCES += $(shell find $(SRC) -name '*.c')

# DONT_COMPILE := \
# $(SRC)/ScreenSMOnlineLogin.cpp \
# $(SRC)/ScreenArcadePatch.cpp \
# $(SRC)/RageSoundReader_Resample*.cpp \
# $(SRC)/arch/ACIO/ACIO.cpp \
# $(filter-out $(shell find $(SRC)/archutils/Emscripten -name "*.cpp"), $(shell find $(SRC)/archutils/ -name "*.cpp")) \
# $(shell find $(SRC)/BaseClasses/ -name "*.cpp") \
# $(shell find $(SRC)/smlobby/ -name "*.cpp")


# SOURCES := $(filter-out $(DONT_COMPILE), $(SOURCES))
# OBJECTS := $(patsubst $(SRC)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))
# OBJECTS := $(patsubst $(SRC)/%.c, $(BUILD_DIR)/%.o, $(OBJECTS))

# # OBJECTS := $(patsubst $(SRC)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))
# # SOURCES += $(C_SOURCES)
# ARCHIVES := $(shell find $(ARC) -name '*.a')

# DEFINES = -DWITHOUT_NETWORKING -DNDEBUG
# DEPS := $(OBJECTS:.o=.d)

# all: main

# main: # $(OBJECTS)
# 	@$(CXX) $(ARCHIVES) $^ -o out/main.js $(INCLUDES) $(DEFINES) $(OPTIMIZATIONS) $(EMFLAGS) $(WEBGL_FLAGS)
# 	@echo "Completed final build"

# gen-libraries: 
# 	@$(MAKE) -C lib
# 	@mkdir -p $(ARC)
# 	@mv $(LIB)/*.a $(ARC)



# clean:
# 	@rm -rf $(ARC) $(TMP)
# 	@$(MAKE) -C lib clean

# -include $(DEPS)

MAKE = emmake make

#first we have lua-5.0
CWD = $(realpath .)
SRC = $(CWD)/src
ARCHIVE = $(CWD)/archive

INCLUDES = -I$(SRC)

# SOURCES := $(shell find $(SRC) -maxdepth 1 -name "*.cpp")
ARCHIVES=$(ARCHIVE)/lua-5.0.a $(ARCHIVE)/mad-0.15.1b.a

# $(BUILD_DIR)/%.o : $(SRC)/%.cpp
# 	@mkdir -p $(@D);
# 	@$(CXX) $(INCLUDES) $(DEFINES) $(CFLAGS) -c $< -o $@
# 	@echo "Finished building: $@ with $(CXX)"

# $(BUILD_DIR)/%.o : $(SRC)/%.c
# 	@mkdir -p $(@D);
# 	@$(CC) $(DEFINES) $(CFLAGS) -c $< -o $@
# 	@echo "Finished building: $@ with $(CC)"


StepMania: $(ARCHIVES)

clean:
	rm -rf $(ARCHIVE) $(BUILD_DIR)


$(ARCHIVE)/%.a: $(SRC)/%
	@mkdir -p $(@D)
	@$(MAKE) -C $< EMFLAGS=-pthread ARCHIVE=$(ARCHIVE) NAME=$(basename $(notdir $@))

.PHONY: clean all