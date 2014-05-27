local httpua = {}

function httpua.onload( node , host, port)
	httpua.socket = node:sock(node.net.AF_INET,node.net.SOCK_STREAM,node.net.IPPROTO_TCP)
	node:bind(httpua.socket, host or "localhost", port or "8080")
	node:listen(httpua.socket)
end

return httpua
