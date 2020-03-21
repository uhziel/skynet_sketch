LUA_INC ?= ../skynet/3rd/lua
CFLAGS = -g -O0 -Wall -I$(LUA_INC)
SHARED := -fPIC -dynamiclib -Wl,-undefined,dynamic_lookup

all: netpackh5.so hproto.so

netpackh5.so: lua-netpackh5.c
	clang $(CFLAGS) $(SHARED) $^ -o $@ -I../skynet/skynet-src

hproto.so: hproto.cpp
	clang++ $(CFLAGS) $(SHARED) $^ -o $@

clean:
	rm netpackh5.so hproto.so