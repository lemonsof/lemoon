
local chan = 
{
	PUB = {__init = function ( C )
		C.SUB = {}
	end};

	RESP = {__init = function ( C )
		C.REQ = {}
		C.RESP = {}
	end};

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

local function __pop_request( C )
	local request = table.remove(C.REQ)

	if request  then
		return table.unpack(request)
	end

	return nil
end 

function chan.RESP.recv(C)
	local self = coroutine.running()

	if C.responder == nil then
		C.responder = self
	elseif C.responder ~= self then
		assert(false,"more than one coroutine call RESP.recv")
	end

	if #C.REQ > 0 then
		return __pop_request(C)
	end

	coroutine.yield("RESP.recv")

	return __pop_request(C)
end

function chan.RESP.response( C, msg, coro )
	C.RESP[coro] = msg
	C.node:wakeup(coro, "RESP.response")
end


function chan.RESP.request( C , msg )
	local self = coroutine.running()
	print("request",self,msg)
	table.insert(C.REQ, 1, {self,msg})
	if C.responder ~= nil then
		C.node:wakeup(C.responder, "RESP.recv")
	end

	coroutine.yield("RESP.response")

	return C.RESP[self]
end

return chan