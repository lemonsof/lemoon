local BIF = {}

function BIF.open( node, modulename )
	local target = require(modulename)

	assert(type(target) == "table",string.format("module[%s] must return a table",modulename))

	for k, v in pairs(target) do 
		if type(k) ~= "number" then
			if k == "dispatch" then
				local header = node[k]

				if header then
					node[k] = function ( ... ) 
						header( ... ) 
						v( ... )
					end
				else
					node[k] = v
				end
			elseif node[k] then
				error(string.format("load library(%s) failed : symbole %s already loaded", modulename, k))
			else
				node[k] = v
			end
		end
	end
end

return BIF