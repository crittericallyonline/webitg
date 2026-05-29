ifdef GENMAPS
CFLAGS+=-MMD -MP -Wall
endif
NAME=aes.a
DEST+=$(NAME)
SOURCES := $(shell find . -name "*.c")
HEADERS := $(shell find . -name "*.h")
DEFS := $(patsubst %.c, %.d, $(SOURCES))
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))

$(DEST): $(OBJECTS)
	@emar rcs $@ $(OBJECTS)
	@echo "Built $@"

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS)

clean:
	@rm -f $(OBJECTS) $(DEFS) $(DEST)

-include $(DEFS)