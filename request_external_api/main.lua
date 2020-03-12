local skynet = require "skynet"
local json = require "json"

skynet.start(function()
    skynet.error("server starting")
    local weather = skynet.newservice("weather")
    local result = skynet.call(weather, "lua", "getweather")
    skynet.error("result:", json.encode(result))
    skynet.error("server started")
end)
