local handle = {}

local OK = "HTTP/1.0 Status: 200 OK\r\nContent-Type: text/plain\r\n\r\nHello world\r\n\r\n"

function handle.onload( node , conn, remote)
	--print(string.format("remote connect[%s] from %s",conn, remote))
	
	local status, header = node:recv(conn,128)

	while not status do
		
		if header:sub(-2) == "\r\n" then
			break
		end

		status, tail = node:recv(conn,128)

		if not status then
			header = header .. tail
		end
	end

	--print("send ...",conn);

	node:send(conn,OK)

	--print("send OK",conn);

	conn:close()
end

return handle