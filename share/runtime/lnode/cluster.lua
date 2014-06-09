local cluster = {}

local gateway = nil

function cluster.cluster(node, ...)
	if not gateway then
		gateway = { peers = {} }
		node:startservice("lnode.cluster.gateway",gateway, ...)
	end
end

function cluster.newnode(node, nodename)
	if not gateway then
		error("call newnode failed: call cluster function first")
	end
	local cmdline = string.format("lnode.cluster.childnode %d %s", gateway.sockaddr:service(),nodename)
	lnode.__newnode(cmdline)
end

return cluster