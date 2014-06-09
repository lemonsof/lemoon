local lemoon = require "lemoon"
local trace_print = {}

local trace = {__trace = trace_print}

local trace_header = function ( level )
	local now = lemoon.now()
	return string.format("[%s] %s.%06d\t",level, os.date("%c",now.tv_sec),now.tv_usec / 1000)
end

function trace.msg(node, fmt, ... )
	return node.__trace.msg(fmt,...)
end

function trace.warn(node, fmt, ... )
	return node.__trace.warn(fmt,...)
end

function trace_print.msg( fmt, ...)
	local msg = trace_header("msg") .. string.format(fmt,...)
	print(msg)
	return msg
end

function trace_print.warn( fmt, ...)
	local msg = trace_header("warn") .. string.format(fmt,...)
	print(msg)
	return msg
end

return trace