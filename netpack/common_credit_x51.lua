local skynet = require "skynet"
local httpc = require "http.httpc"
local json = require "json"

local CMD = {}

local host = "127.0.0.1:8082"
local uri = "/cgi-bin/auth/query_common_credit"

skynet.init(function()
    httpc.timeout=100
    httpc.dns()
end)

function CMD.query(ev)
    skynet.error("CMD.query")
    local res_ev = {}

    res_ev.m_id = 26043 -- CLSID_CEventQueryCommonCreditRes
    res_ev.m_flags = 0

    local status, body = httpc.get(host, uri)
    if status ~= 200 then
        res_ev.m_result = 1
    else
        skynet.error("body:", body)
        local result = json.decode(body)
        res_ev.m_trans_id = ev.m_trans_id
        res_ev.m_account_id = ev.m_account_id
        res_ev.m_result = result.ret
        res_ev.m_tag_black_level = result.data.level
        res_ev.m_common_credit_score = result.data.score
    end

    return res_ev
end

skynet.start(function()
	skynet.dispatch("lua", function(_,_, command, ...)
		skynet.trace()
		local f = CMD[command]
		skynet.ret(skynet.pack(f(...)))
	end)
end)
