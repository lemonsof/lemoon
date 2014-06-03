local server = {}

function server.run( node, pull,resp)
	print("pong2",coroutine.running(),pull:recv())

	print("recv response",resp:request(tostring(coroutine.running())))
end

return server