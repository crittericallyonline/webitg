ifdef GENMAPS
CFLAGS+=-MMD -MP -Wall
endif
NAME=libtommath.a
DEST+=$(NAME)

$(DEST): $(NAME) # $(OBJECTS)

$(NAME): makefile
	@make -C . -f $< RANLIB=emranlib CC=emcc AR=emar ARFLAGS=rcs
	@mv $(NAME) $(DEST)

# %.o: %.c
# 	@echo "$(CC) -> $<"
# 	@$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE)

clean: makefile
	@make -C . -f $< clean
	@echo "Cleaned $<"

-include $(DEFS)