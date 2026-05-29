ifdef GENMAPS
CFLAGS+=-MMD -MP -Wall
endif
NAME=libresample.a
DEST+=$(NAME)
SRC=src
SOURCES := $(shell find $(SRC) -name "*.c")
HEADERS := $(shell find $(SRC) -name "*.h")
DEFS := $(patsubst %.c, %.d, $(SOURCES))
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
INCLUDE=-Iinclude -I$(SRC)

$(DEST): $(OBJECTS)
	@emar rcs $@ $(OBJECTS)
	@echo "Built $@"

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	@rm -f $(OBJECTS) $(DEFS) $(DEST)

-include $(DEFS)