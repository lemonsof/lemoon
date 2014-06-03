local server = {}

function server.run( node, push,pull)
	print(coroutine.running(),push:recv())
	pull:send("pong")
	
end

return server
