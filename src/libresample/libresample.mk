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
CONFIG_TEMPLATE_H=$(SRC)/configtemplate.h
CONFIG_H=$(SRC)/config.h

main: $(CONFIG_H) $(DEST)

$(CONFIG_H): $(CONFIG_TEMPLATE_H)
	@cp $< $@

$(DEST): $(OBJECTS)
	@emar rcs $@ $(OBJECTS)
	@echo "Built $@"

%.o: %.c
	@$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean:
	@rm -f $(OBJECTS) $(DEFS) $(DEST)

-include $(DEFS)