ifdef GENMAPS
CFLAGS+=-MMD -MP -Wall
endif
NAME=libtomcrypt.a
DEST+=$(NAME)
# SRC=src
# SOURCES := $(shell find $(SRC) -name "*.c")
# HEADERS := $(shell find $(SRC) -name "*.h")
# tomcrypt_h = $(shell find $(SRC) -name "tomcrypt.h")
# DEFS := $(patsubst %.c, %.d, $(SOURCES))
# OBJECTS := $(patsubst %.c, %.o, $(SOURCES))
# INCLUDE=-I$(SRC)/headers

$(DEST): $(NAME) # $(OBJECTS)

$(NAME): makefile
	@make -C . -f $< RANLIB=emranlib CC=emcc AR=emar ARFLAGS=rcs LIBTOMMATH=../libtommath library
	@mv $(NAME) $(DEST)

# %.o: %.c
# 	@echo "$(CC) -> $<"
# 	@$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean: makefile
	@make -C . -f $< clean
	@echo "Cleaned $<"

-include $(DEFS)