local cmdproto = {}

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

function cmdproto.encode(cmd)
    if cmd.cmd_id == ConnCmd.CCMD_PUNCH then
        return string.pack("!1<i4i4i8i8", 0, cmd.cmd_id, cmd.peer_pid, cmd.peer_mid)
    elseif cmd.cmd_id == ConnCmd.CCMD_SERVER_DATA then
        return string.pack("!1<i4i4i8i8i8", 0, cmd.cmd_id, cmd.conn_id, cmd.peer_pid, cmd.peer_mid)
    else
        return nil, "cmdproto.encode unsupported cmd_id:" .. cmd.cmd_id
    end
end

function cmdproto.decode(buf)
    local cmd = {}
    _, cmd.cmd_id, unread_pos = string.unpack("!1<i4i4", buf)
    if cmd.cmd_id == ConnCmd.CCMD_PUNCH then
        cmd.peer_pid, cmd.peer_mid = string.unpack("!1<i8i8", buf, unread_pos)
        return cmd
    elseif cmd.cmd_id == ConnCmd.CCMD_SERVER_DATA then
        --print("the size of buf:", #buf)
        cmd.conn_id, cmd.peer_pid, cmd.peer_mid = string.unpack("!1<i8i8i8", buf, unread_pos)
        return cmd
    else
        return nil, "cmdproto.decode unsupported cmd_id:" .. cmd.cmd_id
    end
end

return cmdproto