#include "cpp_hproto_utils.h"

static void
lencode_BodyPart(lua_State *L, int index, Stream *stream) {
    lencode_int(L, index, "type", stream);
    lencode_float(L, index, "weight", stream);
}

static void
lencode_BodyPart(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);

    lencode_BodyPart(L, index, stream);

    lua_pop(L, 1);
}

static void
ldecode_BodyPart(lua_State *L, ReadStream *stream) {
    lua_newtable(L);

    ldecode_int(L, "type", stream);
    ldecode_float(L, "weight", stream);
}

static void
ldecode_BodyPart(lua_State *L, const char* key, ReadStream *stream) {
    lua_pushstring(L, key);

    ldecode_BodyPart(L, stream);

    lua_settable(L, -3);
}

static void
lencode_vector_BodyPart(lua_State *L, int index, const char* key, Stream *stream) {
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

        lencode_BodyPart(L, lua_gettop(L), stream);

        lua_pop(L, 1);
    }

    lua_pop(L, 1);
}

static void
ldecode_vector_BodyPart(lua_State *L, const char* key, ReadStream *stream) {
    lua_Integer size = 0;
    stream->Read(&size, sizeof(size));

    lua_pushstring(L, key);
    lua_createtable(L, size, 0);
    for (lua_Integer i = 1; i <= size; i++) {
        lua_pushinteger(L, i);

        ldecode_BodyPart(L, stream);
        
        lua_settable(L, -3);
    }    

    lua_settable(L, -3);
}

static void
lencode_Human(lua_State *L, int index, Stream *stream) {
    lencode_int(L, index, "id", stream);
    lencode_short(L, index, "age", stream);
    lencode_bool(L, index, "male", stream);
    lencode_string(L, index, "name", stream);
    lencode_string(L, index, "description", stream);
    lencode_vector_BodyPart(L, index, "parts", stream);
}

static void
ldecode_Human(lua_State *L, ReadStream* stream) {
    ldecode_int(L, "id", stream);
    ldecode_short(L, "age", stream);
    ldecode_bool(L, "male", stream);
    ldecode_string(L, "name", stream);
    ldecode_string(L, "description", stream);
    ldecode_vector_BodyPart(L, "parts", stream);
}
