local server = {}

function server.run( node, pull)
	print("pong2",coroutine.running(),pull:recv())
end

return server