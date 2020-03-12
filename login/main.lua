local skynet = require "skynet"

skynet.start(function()
    skynet.error("server starting")
    -- login server
    local logind = skynet.newservice("logind")

    -- chat server
    local gated = skynet.newservice("gated", logind)
    skynet.call(gated, "lua", "open", {
        port = 8888,
        maxclient = 64,
        servername = "game1",
    })

    skynet.error("server started")
    skynet.exit()
    skynet.error("no output")
end)