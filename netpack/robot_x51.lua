local skynet = require "skynet"
local socket = require "skynet.socket"
local netpack = require "netpackx51"
local hproto = require "hproto"
local cmdproto = require "cmdproto"

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
}

local fd
local conn_id
local session = 0
local last = ""
local cur_conn_state = ConnState.CS_FRESH_PUNCH

local function print_request(session, ev)
	skynet.error("------REQUEST----- session:", session)
	for k,v in pairs(ev) do
		skynet.error(k,v)
	end
end

local function print_response(res_ev)
	skynet.error("------RESPONSE-------")
	for k,v in pairs(res_ev) do
		skynet.error(k,v)
	end
end

local function send_pack(stream)
	local pack = netpack.tostring(netpack.pack(stream))
	socket.write(fd, pack)
end

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
	local r = socket.read(fd)
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

local function build_conn(host, port)
    fd = socket.open(host, port)
    send_cmd({cmd_id=ConnCmd.CCMD_PUNCH, peer_pid=0, peer_mid=0})
end

local function load_test_once(count)
    skynet.error("load_test_once count:", count)

    for i = 1, count do
		send_ev({m_id=26042, m_flags=1, m_trans_id=session, m_account_id=101, m_client_ip="192.168.1.133"})
    end
end

local function load_test(count, duration, interval)
    for i = 1, duration, interval do
        load_test_once(count)
        skynet.sleep(interval*100)
    end
    skynet.error("end load_test")
    skynet.exit()
end

local function recv_ev()
    while true do
        dispatch_package()
    end
end

local CMD = {}

-- count 每次发送多少请求
-- duration 总持续时间 单位：s
-- interval 每次间隔多长时间 单位:s
function CMD.load_test(count, duration, interval)
    build_conn("localhost", 26667)
    skynet.fork(load_test, count, duration, interval)
    skynet.fork(recv_ev)
    return "start load_test count:"..count.." duration:"..duration.." interval:"..interval
end

skynet.start(function()
	skynet.dispatch("lua", function(_,_, command, ...)
		skynet.trace()
		local f = CMD[command]
		skynet.ret(skynet.pack(f(...)))
	end)
end)
