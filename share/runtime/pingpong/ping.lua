local server = {}

function server.run( node )
	local pub = node:newchan(node.PUB)
	local pull = node:newchan(node.PULL)
	local clients = 2
	for i = 1,clients do 
		node:startservice("pingpong.pong",pub,pull)
	end
	pub:send("ping")

	for j = 1, clients do
		print("recv :" .. pull:recv())
	end

	for i = 1,clients do 
		node:startservice("pingpong.pong2",pull)
	end

	for i = 1, clients do
		pull:send("ping" .. i)
	end

end

return server

