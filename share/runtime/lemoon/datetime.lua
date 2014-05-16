local module = {}

function module.count( point )
	return point.tv_sec *  1000 + point.tv_usec / 1000
end

function module.duration( lhs,rhs )
	return module.count(lhs) - module.count(rhs)
end

return module