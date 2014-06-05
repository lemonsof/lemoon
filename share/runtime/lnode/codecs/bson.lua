local bson = require("lemoon").bson()

for k, v in pairs(bson) do
	print(k,v)
end

function bson.encode( node, ...)
	-- body
end