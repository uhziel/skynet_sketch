local skynet = require "skynet"
local sprotoloader = require "sprotoloader"

local max_client = 64
local listen_port = 26667 -- 26667 mall server 10016 global_transmitcenter

skynet.start(function()
	skynet.error("Server start")
	if not skynet.getenv "daemon" then
		local console = skynet.newservice("console")
	end
	skynet.newservice("debug_console",8000)
	local watchdog = skynet.newservice("watchdog_x51")
	skynet.call(watchdog, "lua", "start", {
		port = listen_port,
		maxclient = max_client,
		nodelay = true,
	})
	skynet.error("Watchdog listen on", listen_port)
	skynet.exit()
end)
