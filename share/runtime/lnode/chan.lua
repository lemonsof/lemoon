
local chan = 
{
	PUB = {__init = function ( C )
		C.SUB = {}
	end};

	RESP = {};

	PULL = {__init = function ( C )
		C.Q = {}
		C.PULL = {}
	end};
}


function chan.newchan( node, prototype )
	local C = { ["node"] = node }
	
	for k , v in pairs(prototype) do 
		C[k] = v
	end

	C:__init()

	return C
end

function chan.PUB.send( C, msg )
	for coro , Q in pairs(C.SUB) do 
		if coroutine.status(coro) == "dead" then
			C.SUB[coro] = nil
		else
			table.insert(Q, 1, msg)
			C.node:wakeup(coro, "PUB.recv")
		end
	end
end

function chan.PUB.leave( C )
	local self = coroutine.running()
	if C.SUB[self] then
		C.SUB[self] = nil
	end
end

function chan.PUB.recv( C )
	local self = coroutine.running()
	local Q = C.SUB[self]
	if not Q then
		Q = {}
		C.SUB[self] = Q
	elseif #Q > 0 then
		return table.remove(Q)
	end

	coroutine.yield("PUB.recv")

	return table.remove(Q)
end


function chan.PULL.send( C, msg)
	table.insert(C.Q, 1, msg)
	local puller = table.remove(C.PULL)

	if puller then
		C.node:wakeup(puller, "PULL.recv")
	end
end

function chan.PULL.recv( C )
	if #C.Q > 0 then
		return table.remove(C.Q)
	end
	
	local self = coroutine.running()

	table.insert(C.PULL, 1, self)

	coroutine.yield("PULL.recv")

	return table.remove(C.Q)
end

return chan