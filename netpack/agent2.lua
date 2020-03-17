local skynet = require "skynet"
local socket = require "skynet.socket"
local netpack = require "netpackh5"

local WATCHDOG

local CMD = {}
local client_fd

local function send_package(pack)
	socket.write(client_fd, skynet.tostring(netpack.pack(pack)))
end

skynet.register_protocol {
	name = "client",
	id = skynet.PTYPE_CLIENT,
	unpack = function (msg, sz)
		return skynet.tostring(msg, sz)
	end,
	dispatch = function (fd, _, text)
		assert(fd == client_fd)	-- You can use fd to reply message
		skynet.ignoreret()	-- session is fd, don't call skynet.ret
		skynet.trace()
		send_package(text)
		if text == "quit" then
			skynet.call(WATCHDOG, "lua", "close", client_fd)
		end
	end
}

function CMD.start(conf)
	local fd = conf.client
	local gate = conf.gate
	WATCHDOG = conf.watchdog
	-- slot 1,2 set at main.lua
	skynet.fork(function()
		while true do
			send_package("heartbeat")
			skynet.sleep(500)
		end
	end)

	client_fd = fd
	skynet.call(gate, "lua", "forward", fd)
end

function CMD.disconnect()
	-- todo: do something before exit
	skynet.exit()
end

skynet.start(function()
	skynet.dispatch("lua", function(_,_, command, ...)
		skynet.trace()
		local f = CMD[command]
		skynet.ret(skynet.pack(f(...)))
	end)
end)
