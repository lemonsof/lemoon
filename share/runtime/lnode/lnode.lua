
local datetime = require "lemoon.datetime"

local module = {}

function module.onload( lnode, startservice)

	local context = 
	{
		timer_accuracy = 10, -- milliseconds of one timer tick
		sys = require "lemoon",
	}

	context.timer = context.sys.timer(context.timer_accuracy)
	context.io = context.sys.io()	


	-- 
	-- begin main loop
	-- 
	local timestamp = context.sys.now()

	while true do
		context.io:dispatch(context.timer_accuracy)
		local newtimestamp = context.sys.now()
		if datetime.duration(newtimestamp,timestamp) >= context.timer_accuracy then
			context.timer:tick()
			timestamp = newtimestamp
		end
	end

end


return module