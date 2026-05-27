NAME = lua-5.0
BUILD_DIR = build
ifndef DEST
DEST = .
endif
ifndef ROOT
ROOT = $(realpath .)
endif
DEST += /$(NAME)
INC = -I$(ROOT)/include
SRC = $(ROOT)/src
SOURCES := $(shell find $(SRC) -name "*.c")
OBJECTS := $(patsubst $(SRC)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: main
main: $(OBJECTS)
	emar rcs $(DEST) $(OBJECTS)


$(BUILD_DIR)/%.o : $(SRC)/%.c
	@echo $(src)
	@mkdir -p $(@D);
	@$(CC) $(DEFINES) $(CFLAGS) -c $< -o $@ $(INC)
	@echo "Finished building: $@ with $(CC)"

clean: