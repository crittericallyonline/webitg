# Lua-5.0
CXX = em++
CWD = ${realpath .}
SRC = $(CWD)/src
INCLUDES = -I$(SRC)
BUILD_DIR = $(CWD)/build
SOURCES := $(shell find $(SRC) -name '*.c')
OBJECTS := $(patsubst $(SRC)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: aes clean

$(BUILD_DIR)/%.o : $(SRC)/%.c
	@mkdir -p $(@D);
	@$(CXX) $(INCLUDES) -c $< -o $@ -Oz

aes: $(OBJECTS)
	@echo "Creating archive $@"
	@emar rcs $@.a $(OBJECTS)
	@echo "Archive created: $@.a"

clean:
	rm -rf $(BUILD_DIR)

