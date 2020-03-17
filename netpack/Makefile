CC ?= gcc
LUA_INC ?= ../skynet/3rd/lua
CFLAGS = -g -O2 -Wall -I$(LUA_INC)
SHARED := -fPIC -dynamiclib -Wl,-undefined,dynamic_lookup

all: netpackh5.so

netpackh5.so: lua-netpackh5.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -I../skynet/skynet-src

clean:
	rm netpackh5.so