local service = {}

function service.run( node, gateway, host, port)
	
	local socket = node:socket(node.AF_INET, node.SOCK_STREAM, node.IPPROTO_TCP)
	
	node:bind(socket, host or "localhost", port or "")
	
	node:listen(socket)

	gateway.sockaddr = socket:sockname()
	
	while true do
		local err, conn, remote = node:accept(socket)

		if err then
			print(err)
		else
			node:startservice("lnode.cluster.peer", gateway, conn, remote)
		end
	end
end

return service