local lemoon = require "lemoon"

local io = 
{
	AF_UNSPEC = 0;
	AF_INET = 2;
	AF_IMPLINK = 3;
	AF_PUP = 4;
	AF_CHAOS = 5;
	AF_NS = 6;
	AF_INET6 = 30;
	AF_IEEE80211 = 37;
	SOCK_STREAM = 1;
	SOCK_DGRAM = 2;
	SOCK_RAW = 3;
	SOCK_SEQPACKET = 5;
	IPPROTO_ICMP = 1;
	IPPROTO_TCP = 6;
	IPPROTO_UDP = 17;
}

local ioservice = lemoon.io()

--- TODO: import timeout arg for all async functions

function io.dispatch( node )
	ioservice:dispatch( node.timer_accuracy )
end

function io.socket( node, domain, type, protocol )
	return ioservice:sock(domain, type, protocol)
end

function io.bind( node, sock, host, port )
	return sock:bind(host, port)
end

function io.listen( node, sock, ... )
	sock:listen( ... )
end


function io.accept( node, sock )
	local self = coroutine.running ()
	sock:accept(function ( err, conn, remote )
		node:wakeup(self, "accept", err, conn, remote )
	end)

	return coroutine.yield("accept")
end

function io.connect( node, sock, ... )
	local self = coroutine.running ()
	sock:connect( ... , function ( err )
		node:wakeup(self,"connect", err )
	end)

	return coroutine.yield("connect")
end

function io.send( node, sock, ... )
	local self = coroutine.running ()
	sock:send(... , function ( err, bytes )
		node:wakeup(self, "send",  err, bytes  )
	end)

	return coroutine.yield("send")
end

function io.sendto( node, sock, msg, host, ... )
	local self = coroutine.running ()

	local callback = function ( err, bytes )
		node:wakeup(self, "sendto", err, bytes  )
	end

	if type(host) == "sockaddr.sockaddr" then
		sock:send(msg, host, callback, ...)
	else
		local port = select(1, ...)
		if select("#", ...) == 2 then
			sock:send(msg, host, port, callback, select(-1, ...))
		else
			sock:send(msg, host, port, callback)
		end
	end

	return coroutine.yield("sendto")
end

function io.recvfrom( node, sock, len, ... )
	local self = coroutine.running ()

	sock:recvfrom(len, function ( err, msg, remote )
		node:wakeup(self, "recvfrom", err, msg, remote  )
	end, ...)

	return coroutine.yield("recvfrom")
end


function io.recv( node, sock, msg, ... )
	local self = coroutine.running ()
	sock:recv(msg , function ( err, bytes )
		node:wakeup(self, "recv", err, bytes  )
	end, ...)

	return coroutine.yield("recv")
end

return io
