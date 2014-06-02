local service = {}

function service.response( node, conn, resp)
	local header = string.format([[HTTP/%s Status: %s %s\r\n]], resp.Version, resp.Status, resp.Phrase)

	for k ,v in pairs(resp.Fields or {}) do
		header = header .. k .. ": " .. v .. "\r\n" 
	end

	local msg = header .. "\r\n" .. resp.Content or ""

	node:send(conn, msg)

	conn:close()
end

function service.run( node, conn, remote, request )

	service.response( node, conn, 
	{
		Version = 1.1;
		Status = "200";
		Phrase = "OK";
		Fields = {["Content-Type"]= "text/plain"};
		Content = "hello world";
	})
end

return service