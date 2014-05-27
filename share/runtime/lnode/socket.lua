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
	socket:accept()
end



return socket
