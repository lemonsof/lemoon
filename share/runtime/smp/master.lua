local server = {}

function server.run( node )
	node:cluster("127.0.0.1","")
	node:newnode("smp.slave")
end

return server