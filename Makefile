

# ------------------------------------------------------------------------------
#
# If the Lua include files are not in the system's include path, then build by
# doing
#
# make LUA_HOME=/path/to/lua
#
# ------------------------------------------------------------------------------


default : numlua

numlua : FORCE
	@make -C src

clean :
	make -C src clean

FORCE :

