local service = {}

function service.run( node, gateway, conn, remote)
	local err, request = node:recv(conn,256)
	if err then
		node:message("setup peer from (%s) failed :\n\t%s",remote, err)
	end

	
end