local service = {}


local dispatch_request = function ( node, buff, len )
	-- read the request length header
	if len == -1 then
		if #buff < 2 then
			return buff,len
		end
		len = buff:byte(1) *256 + buff:byte(2)
		buff = buff:sub(3)
	end

	-- wait recv body
	if #buff < len then
		return buff , len
	end

	local request, len = node:encode(buff)

	return buff.sub(len), -1, request
end


function service.run( node, gateway, conn, remote)
	local buff = ""
	local length = -1
	while true do
		local err, tail = node:recv(conn)
		if err then
			error(string.format("recv from peer(%s) exception :\n\t%s",remote,err))
		end

		buff = buff .. tail

		buff,length,request = dispatch_request(node, buff)

		if request then
			node:startservice("lnode.cluster.request", gateway, conn, remote, request)
		end
	end
end

return service