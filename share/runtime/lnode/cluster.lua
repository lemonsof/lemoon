local cluster = {}

local gateway = nil

function cluster.cluster(node, ...)
	if not gateway then
		gateway = { peers = {} }
		node:startservice("lnode.cluster.gateway",gateway, ...)
	end
end

function cluster.cluster_startservice(node, servicename, ... )
	-- body
end

return cluster