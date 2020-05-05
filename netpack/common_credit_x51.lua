local skynet = require "skynet"
local httpc = require "http.httpc"
local json = require "json"

local CMD = {}

local host
local uri = "/cgi-bin/auth/query_common_credit"
local url
local webclientd

skynet.init(function()
    httpc.timeout=100
    httpc.dns()
    host = skynet.getenv("ccx_host")
    url = host..uri
    webclientd = skynet.uniqueservice("webclientd")
end)

function CMD.query(ev)
    --skynet.error("CMD.query")
    local res_ev = {}

    res_ev.m_id = 26043 -- CLSID_CEventQueryCommonCreditRes
    res_ev.m_flags = 0

    local ret, body = skynet.call(webclientd, "lua", "request", url, {uin=ev.m_account_id, client_ip=ev.m_client_ip})
    if not ret then
        --skynet.error(body)

        res_ev.m_result = 1
    else
        --skynet.error("body:", body)
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
		--skynet.trace()
		local f = CMD[command]
		skynet.ret(skynet.pack(f(...)))
	end)
end)
