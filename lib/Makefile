all: clean lua.a mad.a aes.a
	mv */*.a .

lua.a:
	@$(MAKE) -C lua

mad.a:
	@$(MAKE) -C mad

aes.a:
	@$(MAKE) -C aes

clean:
	@$(MAKE) -C lua clean
	@$(MAKE) -C mad clean
	@$(MAKE) -C aes clean
	@rm -f *.a