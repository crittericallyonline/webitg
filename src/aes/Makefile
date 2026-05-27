CC = emcc
CXX = em++

INCLUDES=-I ./
BUILD_DIR=./build
SRC=./src

SOURCES := $(shell find -name "*.c")
OBJECTS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))

$(BUILD_DIR)/%.o : %.c
	@mkdir -p $(@D);
	@$(CC) $(INCLUDES) $(EMFLAGS) -c $< -o $@ -Oz
	@echo "$(CC): $@"

all: $(OBJECTS)
	@emar rcs $(ARCHIVE)/$(NAME).a $(OBJECTS)
	@echo "Archive created $(NAME)"
	@echo "EMFLAGS: $(EMFLAGS)"
	@echo "OUTPUT: $(ARCHIVE)/$(NAME).a"

clean:
	rm -rf ./build