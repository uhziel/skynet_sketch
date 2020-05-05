PLAT ?= none
PLATS = linux macosx

.PHONY: all clean none $(PLATS) 

default:
	$(MAKE) $(PLAT)

none:
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "    $(PLATS)"

linux: PLAT = linux
macosx: PLAT = macosx

macosx : SHARED := -fPIC -dynamiclib -Wl,-undefined,dynamic_lookup
macosx : CC := clang
macosx : CCP := clang++
linux : SHARED := -fPIC --shared
linux : CC := gcc
linux : CCP := g++

LUA_INC ?= ../skynet/3rd/lua
CFLAGS = -std=c11 -g -O2 -Wall -I$(LUA_INC)
CCPFLAGS = -std=c++11 -g -O2 -Wall -I$(LUA_INC)

linux macosx:
	$(MAKE) all PLAT=$@ SHARED="$(SHARED)" CC="$(CC)" CCP="$(CCP)" CFLAGS="$(CFLAGS)"

all: netpackh5.so netpackx51.so hproto.so webclient.so

netpackx51.so: lua-netpackx51.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -I../skynet/skynet-src

netpackh5.so: lua-netpackh5.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -I../skynet/skynet-src

hproto.so: hproto.cpp cpp_hproto_utils.cpp
	$(CCP) $(CCPFLAGS) $(SHARED) $^ -o $@

webclient.so: webclient.c
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ -lcurl

clean:
	rm netpackh5.so netpackx51.so hproto.so webclient.so
