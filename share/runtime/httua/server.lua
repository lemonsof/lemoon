local server = {}

function server.run( node, host, port )
	local socket = node:socket(node.AF_INET, node.SOCK_STREAM, node.IPPROTO_TCP)

	node:bind(socket, host or "localhost", port or "8080")

	node:listen(socket)

	while true do
		local err, conn, remote = node:accept(socket)

		if err then
			print(err)
		else
			node:startservice("httua.server.conn", conn, remote)
		end
	end
end

return server