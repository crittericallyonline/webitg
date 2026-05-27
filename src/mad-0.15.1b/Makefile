CC = emcc
CXX = em++

INCLUDES=-I.
BUILD_DIR=build
FPM=-DFPM_64BIT

SOURCES := $(shell find -name "*.c")
OBJECTS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SOURCES))

$(BUILD_DIR)/%.o : %.c
	@mkdir -p $(@D);
	@$(CC) $(INCLUDES) $(EMFLAGS) $(FPM) -DNDEBUG -c $< -o $@
	@echo "$(CC): $@"

all: $(OBJECTS)
	@emar rcs $(ARCHIVE)/$(NAME).a $(OBJECTS)
	@echo "Archive created $(NAME)"
	@echo "EMFLAGS: $(EMFLAGS)"
	@echo "OUTPUT: $(ARCHIVE)/$(NAME).a"

clean:
	rm -rf ./build