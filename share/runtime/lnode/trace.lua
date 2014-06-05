
local trace_print = {}

local trace = {__trace = trace_print}

function trace.message(node, fmt, ... )
	node.__trace.message(fmt,...)
end

function trace_print.message( fmt, ...)
	print(string.format(fmt,...))
end

return trace