# MAD
EMFLAGS = -pthread
CC = emcc
CWD = ${realpath .}
SRC = $(CWD)
INCLUDES = -I$(CWD)
DEFINES = -DFPM_64BIT -DNDEBUG
BUILD_DIR = $(CWD)/build
SOURCES := $(shell find $(SRC) -name '*.c')
OBJECTS := $(patsubst $(SRC)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: mad clean

$(BUILD_DIR)/%.o : $(SRC)/%.c
	@mkdir -p $(@D);
	@$(CC) $(INCLUDES) -c $< -o $@ -Oz $(DEFINES) $(EMFLAGS)

mad: $(OBJECTS)
	@echo "Creating archive $@ : CC = $(CC)"
	@emar rcs $@.a $(OBJECTS)
	@echo "Archive created: $@.a"

clean:
	rm -rf $(BUILD_DIR)

