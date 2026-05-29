ifdef GENMAPS
CFLAGS+=-MMD -MP -Wall
endif
NAME=lualib.a
DEST+=$(NAME)
SRC=src
BUILD_DIR=build
SOURCES := $(shell find $(SRC) -name "*.c")
HEADERS := $(shell find $(SRC) -name "*.h")
DEFS := $(patsubst %.c, %.d, $(SOURCES))
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
INCLUDE=-Iinclude

$(DEST): $(OBJECTS)
	@emar rcs $@ $(OBJECTS)
	@echo "Built $@"

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	@rm -f $(OBJECTS) $(DEFS) $(DEST)

-include $(DEFS)