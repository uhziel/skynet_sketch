#pragma once

#include <lua.hpp>
#include <string.h>
#include <stdlib.h>

#define HASH_LENGTH 4
#define BUF_SIZE 1024

struct Stream
{
    Stream();
    ~Stream();

    size_t size;
    size_t capacity;
    char *buf;

    void Copy(void *src, size_t len);
};

struct ReadStream
{
    ReadStream(size_t _size, const char *_buf);

    size_t size;
    size_t pos;
    const char *buf;

    bool CheckHash();

    void Read(void *dest, size_t len);
};

void lencode_int(lua_State *L, int index, const char *key, Stream *stream);
void ldecode_int(lua_State *L, const char *key, ReadStream *stream);

short lencode_short(lua_State *L, int index, const char *key, Stream *stream);
short ldecode_short(lua_State *L, const char *key, ReadStream *stream);

unsigned short lencode_ushort(lua_State *L, int index, const char *key, Stream *stream);
unsigned short ldecode_ushort(lua_State *L, const char *key, ReadStream *stream);

void lencode_long(lua_State *L, int index, const char *key, Stream *stream);
long long ldecode_long(lua_State *L, const char *key, ReadStream *stream);

void lencode_bool(lua_State *L, int index, const char *key, Stream *stream);
void ldecode_bool(lua_State *L, const char *key, ReadStream *stream);

void lencode_float(lua_State *L, int index, const char *key, Stream *stream);
void ldecode_float(lua_State *L, const char *key, ReadStream *stream);

void lencode_string(lua_State *L, int index, const char *key, Stream *stream);
void ldecode_string(lua_State *L, const char *key, ReadStream *stream);
