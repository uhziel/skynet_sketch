local hproto = require "hproto"

local luaunit = require "luaunit"

TestHproto = {}

function TestHproto:testEventHuman()
    local human = {m_id=99,id=1,age=2,male=true,name="zhulei",description="ooo",parts={{type=1,weight=2.0}, {type=2, weight=3.0}}}
    local bin = hproto.encode(human)
    local out = hproto.decode(bin)
    luaunit.assertEquals(out, human)
    luaunit.assertNotEquals(out, {m_id=1111})
end

function TestHproto:testUnknownEvent()
    local unknown_event = {m_id=1001,name="zhulei",description="ooo"}
    local bin = hproto.encode(unknown_event)
    local out = hproto.decode(bin)
    luaunit.assertEquals(bin, nil)
    luaunit.assertEquals(out, nil)
    luaunit.assertNotEquals(out, unknown_event)
end

os.exit(luaunit.LuaUnit.run())

-- use in terminal:
--[[
zhuleideMBP:netpack zhulei$ lua test_hproto.lua
..
Ran 2 tests in 0.000 seconds, 2 successes, 0 failures
OK   
]]
