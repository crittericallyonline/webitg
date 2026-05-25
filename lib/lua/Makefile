# Lua-5.0
CC = emcc
CWD = ${realpath .}
SRC = $(CWD)/src
INCLUDES = -I$(CWD)/include
BUILD_DIR = $(CWD)/build
SOURCES := $(shell find $(SRC) -name '*.c')
OBJECTS := $(patsubst $(SRC)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: lua-5.0 clean

$(BUILD_DIR)/%.o : $(SRC)/%.c
	@mkdir -p $(@D);
	@$(CC) $(INCLUDES) -c $< -o $@ -Oz

lua-5.0: $(OBJECTS)
	@echo "Creating archive $@"
	@emar rcs $@.a $(OBJECTS)
	@echo "Archive created: $@.a"

clean:
	rm -rf $(BUILD_DIR)

