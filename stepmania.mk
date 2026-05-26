CC = emcc
CXX = em++
DEP=-MMD -MP
BUILD_DIR=./tmp
SRC=./src
INCLUDES=-I$(SRC)

SOURCES := $(shell find $(SRC) -maxdepth 1 -name "*.cpp") $(shell find $(SRC) -maxdepth 1 -name "*.c")
OBJECTS := $(patsubst $(SRC)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

$(BUILD_DIR)/%.o : $(SRC)/%.cpp
	@mkdir -p $(@D);
	@$(CXX) $(INCLUDES) $(EMFLAGS) -c $< -o $@ $(DEP) $(INCLUDE) $(NETWORKING)
	@echo "$(CC): $@"

$(BUILD_DIR)/%.o : $(SRC)/%.c
	@mkdir -p $(@D);
	@$(CC) $(INCLUDES) $(EMFLAGS) -c $< -o $@ $(DEP) $(INCLUDE) $(NETWORKING)
	@echo "$(CC): $@"

all: $(OBJECTS)
# 	@emar rcs $(ARCHIVE)/$(NAME).a $(OBJECTS)
# 	@echo "Archive created $(NAME)"
# 	@echo "EMFLAGS: $(EMFLAGS)"
# 	@echo "OUTPUT: $(ARCHIVE)/$(NAME).a"

clean:
	rm -rf ./build