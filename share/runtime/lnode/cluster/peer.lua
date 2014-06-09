local service = {}

local dispatch_request = function ( node, buff )
	-- read the request length header
	if #buff < 2 then
		return buff
	end
end


function service.run( node, gateway, conn, remote)
	local buff = ""
	while true do
		local err, tail = node:recv(conn)
		if err then
			error(string.format("recv from peer(%s) exception :\n\t%s",remote,err))
		end

		buff = buff .. tail

		buff = dispatch_request(node, buff)
	end
end