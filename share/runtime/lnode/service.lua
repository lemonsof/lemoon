local service = { services = {} }

local seq = 0

function service.startservice( node, servicename, ... )
	local target = require(servicename)
	assert(type(target) == "table" and type(target.run) == "function" ,string.format("service[%s] must return a table",servicename))
	local coro = coroutine.create(target.run)

	local status, err = coroutine.resume( coro, node, ... )
	if not status then
		error(err)
	end

	local id = seq
	seq = seq + 1

	if coroutine.status(coro) ~= "dead" then
		node.services[id] = coro
		node.services[coro] = id
	end	

	return id
end

function service.removeservice( node, coro )
	local id = node.services[coro]
	node.services[coro] = nil
	node.services[id] = nil
end

return service