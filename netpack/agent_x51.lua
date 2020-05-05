local skynet = require "skynet"
local socket = require "skynet.socket"
local netpack = require "netpackx51"
local hproto = require "hproto"
local cmdproto = require "cmdproto"

local WATCHDOG

local CMD = {}
local EVT_HANDLER = {}
local client_fd --这个就当作炫舞的conn_id
local peer_pid --对方的进程id
local peer_mid --对方的机器id
local cur_conn_state --当前conn的状态
local last_ping
local common_credit_service

local ConnCmd = {
	CCMD_PUZZLE = 1,
	CCMD_REP_PUZZLE = 2,
	CCMD_PUZZLE_OK = 3,
	CCMD_PUZZLE_FAIL = 4,
	CCMD_ACCOUNT = 5,
	CCMD_PASSWD = 6,
	CCMD_PASSWD_OK = 7,
	CCMD_PASSWD_FAIL = 8,
	CCMD_PUNCH = 9,
	CCMD_SERVER_DATA = 10,
}

local ConnState = {
	  CS_INVALID = 0,
	  CS_FRESH_PUNCH = 1,
	  CS_FRESH_EXCH_DATA = 2,
	  CS_FRESH = 3,
	  CS_PUZZLE_SOLVING = 4,
	  CS_PUZZLE_RESULT = 5,
	  CS_ACCOUNT = 6,
	  CS_WAIT_PASSWD = 7,
	  CS_PASSWD_VERIFY = 8,
	  CS_FAIL_PUNISH = 9,
	  CS_ESTABLISHED = 10,
	  CS_CLOSING = 11,
	  CS_CLOSED = 12,
};

local EventName = {
	NET_PING = 1,
	NET_PONG = 2,
	NET_SPEED_DETECT = 3,
	NET_SPEED_DETECT_RESP = 4,
	NET_SPEED_DETECT_RESULT = 5,
	NET_ACK = 10,
	NET_SYSTEM_END = 100,
}

local function send_event(ev)
	local pack = hproto.encode(ev, client_fd)
	socket.write(client_fd, skynet.tostring(netpack.pack(pack)))
end

local function send_net_cmd(cmd)
	local pack = cmdproto.encode(cmd)
	socket.write(client_fd, skynet.tostring(netpack.pack(pack)))
end

skynet.register_protocol {
	name = "client",
	id = skynet.PTYPE_CLIENT,
	unpack = function (msg, sz)
		if cur_conn_state == ConnState.CS_ESTABLISHED then
			return hproto.decode(skynet.tostring(msg, sz), client_fd) --client_fd用来计算哈希值用
		else
			return cmdproto.decode(skynet.tostring(msg, sz))
		end
	end,
	dispatch = function (fd, _, ev, reason)
		assert(fd == client_fd)	-- You can use fd to reply message
		skynet.ignoreret()	-- session is fd, don't call skynet.ret
		--skynet.trace()
		--skynet.error("ev:", ev, " reason:", reason)

		if cur_conn_state == ConnState.CS_FRESH_PUNCH and ev.cmd_id == ConnCmd.CCMD_PUNCH then
			skynet.error("cmdproto cmd_id:", ev.cmd_id, " pid:", ev.peer_pid, " mid:", ev.peer_mid)
			peer_pid = ev.peer_pid
			peer_mid = ev.peer_mid

			local res_cmd = {}
			res_cmd.cmd_id = ConnCmd.CCMD_SERVER_DATA
			res_cmd.conn_id = client_fd
			res_cmd.peer_pid = 0
			res_cmd.peer_mid = 0
			send_net_cmd(res_cmd)
			cur_conn_state = ConnState.CS_ESTABLISHED
			skynet.error("CS_ESTABLISHED client_fd:", client_fd)
		elseif cur_conn_state == ConnState.CS_ESTABLISHED then
			if ev == nil then
				skynet.error("agent recv decode fail. reason:", reason)
				return
			end
			local res_ev = {}
			if ev.m_id == EventName.NET_PING then
				skynet.error("tick:", ev.m_tick, " ping:", ev.m_ping)
				res_ev.m_id = EventName.NET_PONG
				res_ev.m_tick = ev.m_tick
				last_ping = ev.m_ping
				send_event(res_ev)
			elseif ev.m_id == 26042 then -- CLSID_CEventQueryCommonCreditRes
				local res_ev = skynet.call(common_credit_service, "lua", "query", ev)
				send_event(res_ev)
			end
		end
	end
}

EVT_HANDLER[1] = function (ev)
	send_event(ev)
end

function CMD.start(conf)
	local fd = conf.client
	local gate = conf.gate
	WATCHDOG = conf.watchdog

	client_fd = fd
	cur_conn_state = ConnState.CS_FRESH_PUNCH
	skynet.call(gate, "lua", "forward", fd)
end

function CMD.disconnect()
	-- todo: do something before exit
	skynet.exit()
end

skynet.init(function()
	common_credit_service = skynet.queryservice("common_credit_x51")
	skynet.error("common_credit_service:", common_credit_service)
end)

skynet.start(function()
	skynet.dispatch("lua", function(_,_, command, ...)
		--skynet.trace()
		local f = CMD[command]
		skynet.ret(skynet.pack(f(...)))
	end)
end)
