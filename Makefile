

default : numlua

numlua : FORCE
	@make -C src

clean :
	make -C src clean

FORCE :

