
local chan = 
{
	PUB = {};

	RESP = {};

	PULL = {};
}


function chan.newchan( node, prototype )
	local C = { ["node"] = node }
	
	for k , v in prototype do 
		C[k] = v
	end

	return C
end


function chan.PUB.send(C, msg)
	for k,v in pairs(C.SUB) do
		if v ~= nil then
			C.node.wakeup(target, nil,msg)
		end
	end
end

function chan.PUB.recv(C, timeout)
	local self = coroutine.running ()
	assert(C.SUB[self] == nil, "ca't multi call recv")
	C.SUB[self] = timeout
	C.node:timeout(timeout,function ()
		C.SUB[self] = nil 
		C.node.wakeup(target, "timeout", nil)
	end)
end