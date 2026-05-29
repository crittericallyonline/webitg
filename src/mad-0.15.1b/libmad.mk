ifdef GENMAPS
CFLAGS+=-MMD -MP -Wall
endif
NAME=libresample.a
DEST+=$(NAME)
SOURCES := $(shell find . -name "*.c")
HEADERS := $(shell find . -name "*.h")
DEFS := $(patsubst %.c, %.d, $(SOURCES))
OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
DEFINES=-DFPM_64BIT -DNDEBUG

main: $(CONFIG_H) $(DEST)

$(CONFIG_H): $(CONFIG_TEMPLATE_H)
	@cp $< $@

$(DEST): $(OBJECTS)
	@emar rcs $@ $(OBJECTS)
	@echo "Built $@"

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS) $(DEFINES)

clean:
	@rm -f $(OBJECTS) $(DEFS) $(DEST)

-include $(DEFS)