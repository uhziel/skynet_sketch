#include "cpp_hproto_utils.h"

//a = hproto.encode({m_id=26042, m_flags=1, m_trans_id=10, m_account_id=101, m_client_ip="192.168.1.133"})
//a = hproto.encode({m_id=26043, m_flags=1, m_trans_id=11, m_account_id=102, m_result=106, m_tag_black_level=11, m_common_credit_score=7})

enum EventID {
    NET_PING = 1,
    NET_PONG = 2,
    NET_SYSTEM_END = 100,
    CLSID_CEventQueryCommonCredit = 26042,
    CLSID_CEventQueryCommonCreditRes = 26043,
    CLSID_CEventThemeChatGMDestory = 56830,
    CLSID_CEventThemeChatGMDestoryRes = 56831,
};

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

static unsigned short
lencode_ev(lua_State *L, int index, Stream *stream) {
    unsigned short id = lencode_ushort(L, index, "m_id", stream);
    if (id > NET_SYSTEM_END) {
        lencode_ushort(L, index, "m_flags", stream);
    }

    if (id == CLSID_CEventQueryCommonCredit) {
        lencode_long(L, index, "m_trans_id", stream);
        lencode_long(L, index, "m_account_id", stream);
        lencode_string(L, index, "m_client_ip", stream);
        return 0;
    } else if (id == CLSID_CEventQueryCommonCreditRes) {
        lencode_long(L, index, "m_trans_id", stream);
        lencode_long(L, index, "m_account_id", stream);
        lencode_int(L, index, "m_result", stream);
        lencode_int(L, index, "m_tag_black_level", stream);
        lencode_int(L, index, "m_common_credit_score", stream);
        return 0;
    } else if (id == NET_PING) {
        lencode_int(L, index, "m_tick", stream);
        lencode_int(L, index, "m_ping", stream);
        return 0;
    } else if (id == NET_PONG) {
        lencode_int(L, index, "m_tick", stream);
        return 0;
    }
    else {
        return id;
    }
}

static unsigned short
ldecode_ev(lua_State *L, ReadStream* stream) {
    lua_newtable(L);

    unsigned short id = ldecode_ushort(L, "m_id", stream);
    if (id > NET_SYSTEM_END) {
        ldecode_ushort(L, "m_flags", stream);
    }

    if (id == CLSID_CEventQueryCommonCredit) {
        ldecode_long(L, "m_trans_id", stream);
        ldecode_long(L, "m_account_id", stream);
        ldecode_string(L, "m_client_ip", stream);
        return 0;
    } else if (id == CLSID_CEventQueryCommonCreditRes) {
        ldecode_long(L, "m_trans_id", stream);
        ldecode_long(L, "m_account_id", stream);
        ldecode_int(L, "m_result", stream);
        ldecode_int(L, "m_tag_black_level", stream);
        ldecode_int(L, "m_common_credit_score", stream);
        return 0;   
    } else if (id == CLSID_CEventThemeChatGMDestory) {
        ldecode_string(L, "m_name", stream);
        ldecode_short(L, "m_trans_id", stream);
        return 0;   
    } else if (id == NET_PING) {
        ldecode_int(L, "m_tick", stream);
        ldecode_int(L, "m_ping", stream); 
        return 0;
    } else if (id == NET_PONG) {
        ldecode_int(L, "m_tick", stream);
        return 0;
    }
    else
    {
        lua_pop(L, 1);
        return id;
    }
}

static int
lencode(lua_State *L) { /* lua table -> buf*/
    int istable = lua_istable(L, 1);
    if (istable == 0) {
        lua_pushnil(L);
        lua_pushliteral(L, "arg #1 must be table.");
        return 2;
    }

    Stream stream;
    int result = lencode_ev(L, 1, &stream);
    if (result > 0) {
        lua_pushnil(L);
		lua_pushfstring(L, "cannot encode event. clsid:%d.", result);      
        return 2;
    }

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
    if (!stream.CheckHash()) {
        lua_pushnil(L);
		lua_pushliteral(L, "check hash fail.");
        return 2;
    }
    int result = ldecode_ev(L, &stream);
    if (result > 0) {
        lua_pushnil(L);
		lua_pushfstring(L, "cannot decode event. clsid:%d.", result);
        return 2;
    }
    
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
