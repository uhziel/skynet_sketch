local skynet = require "skynet"

local CMD = {}

function CMD.query(ev)
    skynet.error("CMD.query")
    local res_ev = {}
    res_ev.m_id = 26043 -- CLSID_CEventQueryCommonCreditRes
    res_ev.m_flags = 0
    res_ev.m_trans_id = ev.m_trans_id
    res_ev.m_account_id = ev.m_account_id
    res_ev.m_result = 1
    res_ev.m_tag_black_level = 5
    res_ev.m_common_credit_score = 110
    return res_ev
end

skynet.start(function()
	skynet.dispatch("lua", function(_,_, command, ...)
		skynet.trace()
		local f = CMD[command]
		skynet.ret(skynet.pack(f(...)))
	end)
end)
