package.cpath = "luaclib/?.so;../netpack/?.so"
package.path = "lualib/?.lua;../netpack/?.lua"

if _VERSION ~= "Lua 5.3" then
	error "Use lua 5.3"
end

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

local socket = require "client.socket"
local netpack = require "netpackx51"
local hproto = require "hproto"
local cmdproto = require "cmdproto"

local conn_id --连接id，从服务器发送过来的，用来做hash计算用

local fd = assert(socket.connect("127.0.0.1", 26667))

local last = ""

local function print_request(session, ev)
	print("------REQUEST----- session:", session)
	for k,v in pairs(ev) do
		print(k,v)
	end
end

local function print_response(res_ev)
	print("------RESPONSE-------")
	for k,v in pairs(res_ev) do
		print(k,v)
	end
end

local function send_pack(stream)
	local pack = netpack.tostring(netpack.pack(stream))
	socket.send(fd, pack)
end

local session = 0

local function send_ev(ev)
	local stream = hproto.encode(ev)
	send_pack(stream)
	session = session + 1
	print_request(session, ev)
end

local function send_cmd(cmd)
	local stream = cmdproto.encode(cmd)
	send_pack(stream)
	session = session + 1
	print_request(session, cmd)
end

local function unpack_package(text)
	local size = #text
	if size < 4 then
		return nil, text
	end
	local s = text:byte(1) + text:byte(2) * 256 + text:byte(3) * (2^16) + text:byte(4) * (2^24)
	if size < s+8 then
		return nil, text
	end

	return text:sub(5,8+s), text:sub(9+s)
end

local function recv_package(last)
	local result
	result, last = unpack_package(last)
	if result then
		return result, last
	end
	local r = socket.recv(fd)
	if not r then
		return nil, last
	end
	if r == "" then
		error "Server closed"
	end
	return unpack_package(last .. r)
end

local function dispatch_package()
	while true do
		local v
		v, last = recv_package(last)
		if not v then
			break
		end

		if cur_conn_state ~= ConnState.CS_ESTABLISHED then
			local cmd = cmdproto.decode(v)
			if cmd then
				print_response(cmd)
				if cmd.cmd_id == ConnCmd.CCMD_SERVER_DATA then
					conn_id = cmd.conn_id
					cur_conn_state = ConnState.CS_ESTABLISHED 
				end
			end
		else
			local res_ev = hproto.decode(v)
			print_response(res_ev)
		end
		
	end
end

local cur_conn_state = ConnState.CS_FRESH_PUNCH

send_cmd({cmd_id=ConnCmd.CCMD_PUNCH, peer_pid=0, peer_mid=0})

while true do
	dispatch_package()
	local cmd = socket.readstdin()
	if cmd then
		if cmd == "query" then
			send_ev({m_id=26042, m_flags=1, m_trans_id=10, m_account_id=101, m_client_ip="192.168.1.133"})
		elseif cmd == "exit" then
			os.exit()
		end
	else
		socket.usleep(100)
	end
end
