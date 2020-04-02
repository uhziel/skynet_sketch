local cmdproto = {}

function cmdproto.encode(cmd)
    return string.pack("!1<i4i4i8i8i8", 0, cmd.cmd_id, cmd.conn_id, cmd.peer_pid, cmd.peer_mid)
end

function cmdproto.decode(buf)
    local cmd = {}
    _, cmd.cmd_id, cmd.peer_pid, cmd.peer_mid = string.unpack("!1<i4i4i8i8", buf)
    return cmd
end

return cmdproto