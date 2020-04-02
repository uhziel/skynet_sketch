LUA_INC ?= ../skynet/3rd/lua
CFLAGS = -g -O0 -Wall -I$(LUA_INC)
SHARED := -fPIC -dynamiclib -Wl,-undefined,dynamic_lookup

all: netpackh5.so netpackx51.so hproto.so

.PHONY: clean

netpackx51.so: lua-netpackx51.c
	clang $(CFLAGS) $(SHARED) $^ -o $@ -I../skynet/skynet-src

netpackh5.so: lua-netpackh5.c
	clang $(CFLAGS) $(SHARED) $^ -o $@ -I../skynet/skynet-src

hproto.so: hproto.cpp
	clang++ $(CFLAGS) $(SHARED) $^ -o $@

clean:
	rm netpackh5.so netpackx51.so hproto.so
