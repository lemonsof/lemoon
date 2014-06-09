local service = { sleeping = {}; running = {} }

local seq = 0

local sleep = function ( node, coro, status, event)

	if not status then
		node:warn(event)
	end

	if coroutine.status(coro) ~= "dead" then
		node.sleeping[coro] = event
		return coro
	end

	return nil
end

function service.startservice( node, servicename, ... )
	local target = require(servicename)
	assert(type(target) == "table" and type(target.run) == "function" ,string.format("service[%s] must return a table",servicename))
	local coro = coroutine.create(target.run)
	return sleep(node, coro, coroutine.resume( coro, node, ... ))
end



function service.wakeup( node, coro, event, ...)
	local e = node.sleeping[coro]
	if e == nil then
		return false
	end

	if e == event then
		node.sleeping[coro] = nil
		node.running[coro] = table.pack(...)
		return true
	end

	return false
end

function service.dispatch( node )

	local Q = node.running
	node.running = {}

	for k , v in pairs(Q) do
		sleep(node, k, coroutine.resume( k, table.unpack(v) ))
	end
end


return service