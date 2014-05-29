local httpua = {}

function httpua.onload( node , host, port)
	httpua.socket = node:sock(node.net.AF_INET,node.net.SOCK_STREAM,node.net.IPPROTO_TCP)
	node:bind(httpua.socket, host or "localhost", port or "8080")

	node:listen(httpua.socket)

	while true do
		local err, conn, remote = node:accept(httpua.socket)

		if not err then
			node.loadservice("httpua.server.handle", conn, remote)
		end 
	end
end

return httpua
