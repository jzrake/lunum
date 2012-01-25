




default : $(LUNAR_A) $(LUNAR_SO)

%.o : %.c
	$(CC) $(CFLAGS) -std=c99 -c $<

lunar.o : lunar.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

lunar_capi.o : lunar_capi.c
	$(CC) $(CFLAGS) -std=c99 -c $< -I$(LUA_HOME)/include

$(LUNAR_A)  : numarray.o lunar.o lunar_capi.o
	$(AR) $@ $?

$(LUNAR_SO) : numarray.o lunar.o lunar_capi.o
	$(SO) -o $(LUNAR_SO) $^

clean :
	rm -f *.o $(LUNAR_A) $(LUNAR_SO)
