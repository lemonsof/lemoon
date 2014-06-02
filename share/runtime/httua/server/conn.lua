local service = {}

function service.request( node, conn, remote )
	-- recv request
	local err, msg = node:recv(conn,256)

	if err then
		conn:close()
		error(err)
	end	

	while msg:sub(-2) ~= "\r\n" do
		local err, tail = node:recv(conn,256)

		if err then
			conn:close()
			error(err)
		end	

		msg = msg .. tail
	end

	local request = {}	 
	local lines = msg:gmatch("([^\n]*)\r\n") 
	for method, path, version in lines():gmatch("(%u+) ([%w/]+) HTTP/(%d.%d)") do
		request.Method = method
		request.Path = path
		request.Version = version
	end
		
	for line in lines do
		for k ,v  in line:gmatch("(%w+): (.*)") do
			request[k] = v
		end
	end
	
	return request
end

function service.run( node, conn, remote )
	node:startservice("httua.server.resp", conn, remote, service.request( node, conn, remote ))
end

return service
