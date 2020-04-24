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
lencode_array_BodyPart(lua_State *L, int index, Stream *stream) {
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
}

static void
lencode_array_BodyPart(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);

    lencode_array_BodyPart(L, index, stream);

    lua_pop(L, 1);
}

static void
lencode_array_unsigned_int(lua_State *L, int index, Stream *stream) {
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

        lencode_unsigned_int(L, lua_gettop(L), stream);

        lua_pop(L, 1);
    }
}

static void
lencode_array_unsigned_int(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);

    lencode_array_unsigned_int(L, index, stream);

    lua_pop(L, 1);
}

static void
lencode_array_array_unsigned_int(lua_State *L, int index, Stream *stream) {
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

        lencode_array_unsigned_int(L, lua_gettop(L), stream);

        lua_pop(L, 1);
    }
}

static void
lencode_array_array_unsigned_int(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);

    lencode_array_array_unsigned_int(L, index, stream);

    lua_pop(L, 1);
}

static void
lencode_array_array_BodyPart(lua_State *L, int index, Stream *stream) {
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

        lencode_array_BodyPart(L, lua_gettop(L), stream);

        lua_pop(L, 1);
    }
}

static void
lencode_array_array_BodyPart(lua_State *L, int index, const char* key, Stream *stream) {
    lua_pushstring(L, key);
    lua_gettable(L, index);

    lencode_array_array_BodyPart(L, index, stream);

    lua_pop(L, 1);
}

static void
lencode_CEventHuman(lua_State *L, int index, Stream *stream) {
    lencode_int(L, index, "id", stream);
    lencode_short(L, index, "age", stream);
    lencode_bool(L, index, "male", stream);
    lencode_string(L, index, "name", stream);
    lencode_string(L, index, "description", stream);
    lencode_array_BodyPart(L, index, "parts", stream);
    lencode_BodyPart(L, index, "test_part1", stream);
    lencode_array_BodyPart(L, index, "test_parts2", stream);
    lencode_array_array_unsigned_int(L, index, "test_tmp1", stream);
    lencode_array_array_BodyPart(L, index, "test_tmp2", stream);
}

static void
ldecode_array_BodyPart(lua_State *L, ReadStream *stream) {
    lua_Integer size = 0;
    stream->Read(&size, sizeof(size));

    lua_createtable(L, size, 0);
    for (lua_Integer i = 1; i <= size; i++) {
        lua_pushinteger(L, i);

        ldecode_BodyPart(L, stream);
        
        lua_settable(L, -3);
    }
}

static void
ldecode_array_BodyPart(lua_State *L, const char* key, ReadStream *stream) {
    lua_pushstring(L, key);

    ldecode_array_BodyPart(L, stream); 

    lua_settable(L, -3);
}

static void
ldecode_array_unsigned_int(lua_State *L, ReadStream *stream) {
    lua_Integer size = 0;
    stream->Read(&size, sizeof(size));

    lua_createtable(L, size, 0);
    for (lua_Integer i = 1; i <= size; i++) {
        lua_pushinteger(L, i);

        ldecode_unsigned_int(L, stream);
        
        lua_settable(L, -3);
    }
}

static void
ldecode_array_unsigned_int(lua_State *L, const char* key, ReadStream *stream) {
    lua_pushstring(L, key);

    ldecode_array_unsigned_int(L, stream); 

    lua_settable(L, -3);
}

static void
ldecode_array_array_unsigned_int(lua_State *L, ReadStream *stream) {
    lua_Integer size = 0;
    stream->Read(&size, sizeof(size));

    lua_createtable(L, size, 0);
    for (lua_Integer i = 1; i <= size; i++) {
        lua_pushinteger(L, i);

        ldecode_array_unsigned_int(L, stream);
        
        lua_settable(L, -3);
    }
}

static void
ldecode_array_array_unsigned_int(lua_State *L, const char* key, ReadStream *stream) {
    lua_pushstring(L, key);

    ldecode_array_array_unsigned_int(L, stream); 

    lua_settable(L, -3);
}

static void
ldecode_array_array_BodyPart(lua_State *L, ReadStream *stream) {
    lua_Integer size = 0;
    stream->Read(&size, sizeof(size));

    lua_createtable(L, size, 0);
    for (lua_Integer i = 1; i <= size; i++) {
        lua_pushinteger(L, i);

        ldecode_array_BodyPart(L, stream);
        
        lua_settable(L, -3);
    }
}

static void
ldecode_array_array_BodyPart(lua_State *L, const char* key, ReadStream *stream) {
    lua_pushstring(L, key);

    ldecode_array_array_BodyPart(L, stream); 

    lua_settable(L, -3);
}

static void
ldecode_CEventHuman(lua_State *L, ReadStream *stream) {
    ldecode_int(L, "id", stream);
    ldecode_short(L, "age", stream);
    ldecode_bool(L, "male", stream);
    ldecode_string(L, "name", stream);
    ldecode_string(L, "description", stream);
    ldecode_array_BodyPart(L, "parts", stream);
    ldecode_BodyPart(L, "test_part1", stream);
    ldecode_array_BodyPart(L, "test_parts2", stream);
    ldecode_array_array_unsigned_int(L, "test_tmp1", stream);
    ldecode_array_array_BodyPart(L, "test_tmp2", stream);
}

static void
lencode_CEventQueryCommonCredit(lua_State *L, int index, Stream *stream) {
    lencode_long(L, index, "m_trans_id", stream);
    lencode_long(L, index, "m_account_id", stream);
    lencode_string(L, index, "m_client_ip", stream);
}

static void
ldecode_CEventQueryCommonCredit(lua_State *L, ReadStream *stream) {
    ldecode_long(L, "m_trans_id", stream);
    ldecode_long(L, "m_account_id", stream);
    ldecode_string(L, "m_client_ip", stream);
}

static void
lencode_CEventQueryCommonCreditRes(lua_State *L, int index, Stream *stream) {
    lencode_long(L, index, "m_trans_id", stream);
    lencode_long(L, index, "m_account_id", stream);
    lencode_int(L, index, "m_result", stream);
    lencode_int(L, index, "m_tag_black_level", stream);
    lencode_int(L, index, "m_common_credit_score", stream);
}

static void
ldecode_CEventQueryCommonCreditRes(lua_State *L, ReadStream *stream) {
    ldecode_long(L, "m_trans_id", stream);
    ldecode_long(L, "m_account_id", stream);
    ldecode_int(L, "m_result", stream);
    ldecode_int(L, "m_tag_black_level", stream);
    ldecode_int(L, "m_common_credit_score", stream);
}

