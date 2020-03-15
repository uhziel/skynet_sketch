local skynet = require "skynet"
local xml2lua = require "xml2lua"

local xmlconfig
local CMD = {}

CMD.notify_xml_content = function (config_name, config_content)
    skynet.error("notify_xml_content config_name:", config_name)
    xml2lua.printable(config_content)
end

skynet.start(function()
    skynet.dispatch("lua", function(session, source, cmd, ...)
        local f = assert(CMD[cmd])
        skynet.ret(skynet.pack(f(...)))
    end)

    xmlconfig = skynet.queryservice("xmlconfig")
    local ret = skynet.call(xmlconfig, "lua", "subscribe", "people", "../xmlconfig/people.xml")
    skynet.error("ret:", ret)
end)
