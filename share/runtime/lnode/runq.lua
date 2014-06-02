local runq = {}

local Q = {}


function runq.wakeup( coro, ... )
	assert(not Q[coro])

	Q[coro] = table.pack(...)
end

function runq.dispatch( node )

	for k, v in pairs(Q) do

		if v then
			local status, err = coroutine.resume( k, table.unpack(v) )
			
			if not status then
				print(err)
			end

			Q[k] = nil
			if coroutine.status(k) == "dead" then
				node:removeservice(k)
			end
		end
	end
end

return runq