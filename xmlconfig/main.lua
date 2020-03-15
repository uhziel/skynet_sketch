local skynet = require "skynet"

skynet.start(function()
    skynet.error("server starting")
    skynet.uniqueservice("xmlconfig")
    skynet.newservice("reader")
    skynet.newservice("reader2")
    skynet.error("server started")
end)
