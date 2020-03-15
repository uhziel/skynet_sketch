local skynet = require "skynet"
local xml2lua = require "xml2lua"
local handler = require "xmlhandler.tree"
local io = require "io"
local lfs = require "lfs"

local xmlparser = xml2lua.parser(handler)

local config_rt_table = {}
local CMD = {}

local UPDATE_INTERVAL = 100 --1秒

local function get_xml_content(config_path)
    io.input(config_path)
    local xml_text = io.read("*all")
    xmlparser:parse(xml_text)
    return handler.root
end

local function update()
    skynet.error("time:", skynet.time())

    for config_path, config_rt in pairs(config_rt_table) do
        local modified_time = lfs.attributes(config_path, "modification")
        if config_rt.last_modified_time ~= modified_time then
            local xml_content = get_xml_content(config_path)
            for subscriber, subscribe_info in pairs(config_rt.subscribers) do
                skynet.send(subscriber, "lua", "notify_xml_content",
                    subscribe_info.config_name, xml_content)
            end
            config_rt.last_modified_time = modified_time
        end
    end

    skynet.timeout(UPDATE_INTERVAL, update)
end

CMD.subscribe = function (source, config_name, config_path)
    if not config_rt_table[config_path] then
        config_rt_table[config_path] = {
            subscribers = {
                [source] = {config_name = config_name}
            },
            last_modified_time = 0,
        }
        skynet.error("add subscriber. source:", source)
        return true
    end

    local config_rt = config_rt_table[config_path]
    local subscribe_info = config_rt.subscribers[source]
    if subscribe_info then
        skynet.error("donot subscribe repeatly source:", source, " config_path:", config_path)
        return false
    end

    skynet.error("add subscriber. source:", source)
    config_rt.subscribers[source] = {
        config_name = config_name
    }

    return true
end

skynet.start(function()
    skynet.dispatch("lua", function(session, source, cmd, ...)
        local f = assert(CMD[cmd])
        skynet.ret(skynet.pack(f(source, ...)))
    end)

    skynet.fork(update)
end)
