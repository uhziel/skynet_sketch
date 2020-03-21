#include <lua.hpp>

#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 1024

struct Stream
{   
    Stream() : size(0), capacity(BUF_SIZE)
    {
        this->buf = (char*)malloc(BUF_SIZE);
        memset(this->buf, 0, BUF_SIZE);
    }

    ~Stream()
    {
        free(this->buf);
        this->buf = NULL;
    }
    size_t size;
    size_t capacity;
    char* buf;

    void Copy(void* src, size_t len)
    {
        memcpy(this->buf+size, src, len);
        this->size += len;
    }
};

struct ReadStream
{   
    ReadStream(size_t _size, const char *_buf) : size(_size), pos(0), buf(_buf) {}

    size_t size;
    size_t pos;
    const char *buf;

    void Read(void* dest, size_t len)
    {
        memcpy(dest, this->buf+pos, len);
        this->pos += len;
    }
};

static void
lencode_int(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);
    int v = (int)lua_tointeger(L, -1);  //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
}

static void
ldecode_int(lua_State *L, const char* key, ReadStream *stream) {
    int v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushinteger(L, v);
    lua_settable(L, -3);
}

static void
lencode_short(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);
    short v = (short)lua_tointeger(L, -1);  //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
}

static void
ldecode_short(lua_State *L, const char* key, ReadStream *stream) {
    short v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushinteger(L, v);
    lua_settable(L, -3);
}

static void
lencode_bool(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);
    bool v = (bool)lua_toboolean(L, -1);  //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
}

static void
ldecode_bool(lua_State *L, const char* key, ReadStream *stream) {
    bool v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushboolean(L, v);
    lua_settable(L, -3);
}

static void
lencode_float(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);
    float v = (float)lua_tonumber(L, -1);  //write
    stream->Copy(&v, sizeof(v));
    lua_pop(L, 1);
}

static void
ldecode_float(lua_State *L, const char* key, ReadStream *stream) {
    float v = 0;
    stream->Read(&v, sizeof(v));
    lua_pushstring(L, key);
    lua_pushnumber(L, v);
    lua_settable(L, -3);
}

static void
lencode_cstring(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);
    size_t len = 0;
    const char* v = lua_tolstring(L, -1, &len);  //write
    stream->Copy(&len, sizeof(len));
    stream->Copy((void*)v, len);
    lua_pop(L, 1);
}

static void
ldecode_cstring(lua_State *L, const char* key, ReadStream *stream) {
    size_t len = 0;
    stream->Read(&len, sizeof(len));
    
    lua_pushstring(L, key);
    lua_pushlstring(L, stream->buf+stream->pos, len);
    stream->pos += len;
    lua_settable(L, -3);
}

static inline void
lencode_string(lua_State *L, int index, const char* key, Stream *stream) {
    lencode_cstring(L, index, key, stream);
}

static void
ldecode_string(lua_State *L, const char* key, ReadStream *stream) {
    ldecode_cstring(L, key, stream);
}

static void
lencode_bodypart(lua_State *L, int index, Stream *stream) {
    lencode_int(L, index, "type", stream);
    lencode_float(L, index, "weight", stream);
}

static void
ldecode_bodypart(lua_State *L, ReadStream *stream) {
    lua_newtable(L);

    ldecode_int(L, "type", stream);
    ldecode_float(L, "weight", stream);
}

static void
lencode_vector(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);

    lua_Integer size = 0;
    if (lua_istable(L, -1))
    {
        lua_len(L, -1);
        size = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    stream->Copy(&size, sizeof(size));
    for (lua_Integer i = 1; i <= size; i++) {
        lua_pushinteger(L, i);
        lua_gettable(L, -2);

        lencode_bodypart(L, lua_gettop(L), stream);

        lua_pop(L, 1);
    }

    lua_pop(L, 1);
}

static void
ldecode_vector(lua_State *L, const char* key, ReadStream *stream) {
    lua_Integer size = 0;
    stream->Read(&size, sizeof(size));

    lua_pushstring(L, key);
    lua_createtable(L, size, 0);
    for (lua_Integer i = 1; i <= size; i++) {
        lua_pushinteger(L, i);

        ldecode_bodypart(L, stream);
        
        lua_settable(L, -3);
    }    

    lua_settable(L, -3);
}

static void
lencode_human(lua_State *L, int index, Stream *stream) {
    lencode_int(L, index, "id", stream);
    lencode_short(L, index, "age", stream);
    lencode_bool(L, index, "male", stream);
    lencode_cstring(L, index, "name", stream);
    lencode_string(L, index, "description", stream);
    lencode_vector(L, index, "parts", stream);
}

static void
ldecode_human(lua_State *L, ReadStream* stream) {
    lua_newtable(L);

    ldecode_int(L, "id", stream);
    ldecode_short(L, "age", stream);
    ldecode_bool(L, "male", stream);
    ldecode_cstring(L, "name", stream);
    ldecode_string(L, "description", stream);
    ldecode_vector(L, "parts", stream);
}

static int
lencode(lua_State *L) { /* lua table -> buf*/
	lua_Number tmp = lua_tonumber(L, 1);
    int type = 0;
    lua_numbertointeger(tmp, &type);
    int istable = lua_istable(L, 2);
    if (istable == 0) {
        lua_pushnil(L);
        lua_pushliteral(L, "arg #2 must be table.");
        return 2;
    }

    Stream stream;
    lencode_human(L, 2, &stream);
    lua_pushlstring(L, stream.buf, stream.size);
	return 1;
}

static int
ldecode(lua_State *L) { /* buf -> lua table */
    size_t size = 0;
	const char *ptr = lua_tolstring(L, 1, &size);
	if (ptr == NULL) {
        lua_pushnil(L);
		lua_pushliteral(L, "please input valid data.");
        return 2;
	}

    ReadStream stream(size, ptr);
    ldecode_human(L, &stream);
    
	return 1;
}

/*
hproto.encode(1, {id=1,age=2,male=true,name="zhulei",description="ooo",parts={{type=1,weight=2.0}, {type=2, weight=3.0}}})
*/

extern "C" {
LUAMOD_API int
luaopen_hproto(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "encode", lencode },
		{ "decode", ldecode },
		{ NULL, NULL },
	};
	luaL_newlib(L,l);

	return 1;
}

}
