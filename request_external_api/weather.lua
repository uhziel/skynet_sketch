local skynet = require "skynet"
local httpc = require "http.httpc"
local json = require "json"

local CMD = {}

CMD.getweather = function ()
    local host = "http://www.weather.com.cn"
    local uri = "/data/sk/101010100.html"
    
    httpc.timeout = 100
    httpc.dns()

    skynet.error("request GET:", host..uri)
    local status, body = httpc.get(host, uri)
    skynet.error("[status]", status)
    skynet.error("[body] =====>")
    skynet.error(body)

    if status ~= 200 then
        return nil
    else
        local json_body = json.decode(body)
        return json.decode(body).weatherinfo    
    end
end

skynet.start(function()
    skynet.dispatch("lua", function (session, source, cmd, ...)
        local f = assert(CMD[cmd])
        skynet.ret(skynet.pack(f(...)))
    end)
end)
