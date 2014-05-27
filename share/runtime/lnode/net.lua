local net = {}

for _, def in ipairs({"lemoon.net.af","lemoon.net.type","lemoon.net.protocol"}) do	
	for k , v in pairs(require(def)) do
		net[k] = v
	end
end

return net