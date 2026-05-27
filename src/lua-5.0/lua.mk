ifndef CC
CC = emcc
endif
ifndef CXX
CXX = em++
endif
ifndef NAME
NAME=lua-5.0
endif
ifndef ARCHIVE
ARCHIVE = .
endif

INCLUDES=-I ./include
BUILD_DIR=build
SRC=src

SOURCES := $(shell find $(SRC) -name "*.c")
OBJECTS := $(patsubst $(SRC)/%.c, $(BUILD_DIR)/%.o, $(SOURCES))

all: $(OBJECTS)
	@echo "Creating $(NAME).a"
	@emar rcs $(ARCHIVE)/$(NAME).a $^

clean: 
	@rm -rf ./build

$(BUILD_DIR)/%.o : $(SRC)/%.c
	@mkdir -p $(@D);
	@$(CC) $(INCLUDES) $(EMFLAGS) -c $< -o $@ -Oz
	@echo "$(CC): $@"