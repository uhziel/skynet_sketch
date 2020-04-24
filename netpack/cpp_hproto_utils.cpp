#include "cpp_hproto_utils.h"

Stream::Stream()
    : size(0), capacity(BUF_SIZE)
{
  this->buf = (char *)malloc(BUF_SIZE);
  memset(this->buf, 0, BUF_SIZE);
  this->size += HASH_LENGTH;
}

Stream::~Stream()
{
  free(this->buf);
  this->buf = NULL;
}

void Stream::Copy(void *src, size_t len)
{
  memcpy(this->buf + size, src, len);
  this->size += len;
}

ReadStream::ReadStream(size_t _size, const char *_buf) : size(_size), pos(0), buf(_buf) {}

bool ReadStream::CheckHash()
{
  int hash = 0;
  Read(&hash, sizeof(hash));
  //TODO check hash
  return true;
}

void ReadStream::Read(void *dest, size_t len)
{
  memcpy(dest, this->buf + pos, len);
  this->pos += len;
}

void lencode_int(lua_State *L, int index, const char *key, Stream *stream)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);

    int v = (int)lua_tointeger(L, -1); //write
    stream->Copy(&v, sizeof(v));

    lua_pop(L, 1);
}

void ldecode_int(lua_State *L, const char *key, ReadStream *stream)
{
    lua_pushstring(L, key);

    int v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushinteger(L, v);

    lua_settable(L, -3);
}

void lencode_unsigned_int(lua_State *L, int index, Stream *stream)
{
    unsigned int v = (unsigned int)lua_tointeger(L, -1); //write
    stream->Copy(&v, sizeof(v));
}

void lencode_unsigned_int(lua_State *L, int index, const char *key, Stream *stream)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);

    lencode_unsigned_int(L, index, stream);
}

void ldecode_unsigned_int(lua_State *L, const char *key, ReadStream *stream)
{
    lua_pushstring(L, key);

    ldecode_unsigned_int(L, stream);

    lua_settable(L, -3);
}

void ldecode_unsigned_int(lua_State *L, ReadStream *stream)
{
    unsigned int v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushinteger(L, v);
}

short lencode_short(lua_State *L, int index, const char *key, Stream *stream)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);
    short v = (short)lua_tointeger(L, -1); //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
    return v;
}

short ldecode_short(lua_State *L, const char *key, ReadStream *stream)
{
    short v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushinteger(L, v);
    lua_settable(L, -3);
    return v;
}

unsigned short lencode_ushort(lua_State *L, int index, const char *key, Stream *stream)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);
    unsigned short v = (unsigned short)lua_tointeger(L, -1); //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
    return v;
}

unsigned short ldecode_ushort(lua_State *L, const char *key, ReadStream *stream)
{
    unsigned short v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushinteger(L, v);
    lua_settable(L, -3);
    return v;
}

void lencode_long(lua_State *L, int index, const char *key, Stream *stream)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);
    long long v = (long long)lua_tointeger(L, -1); //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
}

long long ldecode_long(lua_State *L, const char *key, ReadStream *stream)
{
    long long v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushinteger(L, v);
    lua_settable(L, -3);
    return v;
}

void lencode_bool(lua_State *L, int index, const char *key, Stream *stream)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);
    bool v = (bool)lua_toboolean(L, -1); //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
}

void ldecode_bool(lua_State *L, const char *key, ReadStream *stream)
{
    bool v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushboolean(L, v);
    lua_settable(L, -3);
}

void lencode_float(lua_State *L, int index, const char *key, Stream *stream)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);
    float v = (float)lua_tonumber(L, -1); //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
}

void ldecode_float(lua_State *L, const char *key, ReadStream *stream)
{
    float v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushnumber(L, v);
    lua_settable(L, -3);
}

void lencode_string(lua_State *L, int index, const char *key, Stream *stream)
{
    lua_pushstring(L, key);
    lua_gettable(L, index);
    size_t len = 0;
    const char *v = lua_tolstring(L, -1, &len); //write
    int tmp = (int)(len);
    stream->Copy(&tmp, sizeof(tmp));
    stream->Copy((void *)v, tmp);
    char c = '\0';
    stream->Copy(&c, sizeof(c));
    lua_pop(L, 1);
}

void ldecode_string(lua_State *L, const char *key, ReadStream *stream)
{
    int len = 0;
    stream->Read(&len, sizeof(len));

    lua_pushstring(L, key);
    lua_pushlstring(L, stream->buf + stream->pos, len);
    stream->pos += (len + 1); //加1是因为尾部用’\0‘做结尾但是没算在字符串长度里
    lua_settable(L, -3);
}
