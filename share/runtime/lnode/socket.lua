local lemoon = require "lemoon"

local socket = {}

function socket.sock( node, af, type, protocol)
	return node.io:sock(af,type,protocol)
end

function socket.bind( node, socket, host, port )
	socket:bind(host, port)
end

function socket.listen( node, socket)
	socket:listen()
end

function socket.accept( node, socket)

	local coro = coroutine.running()


	socket:accept(function ( err, cnn, remote)
		node.wakeup(coro, err, cnn, remote)
	end)

	return coroutine.yield()
end

function socket.recv( node, socket, len)
	local coro = coroutine.running()

	socket:recv(len ,function ( err, data )
		node.wakeup( coro, err, data )
	end)

	return coroutine.yield()
end

function socket.send( node, socket, msg)
	local coro = coroutine.running()

	socket:send(msg ,function ( err )
		node.wakeup( coro, err )
	end)

	return coroutine.yield()
end


return socket
