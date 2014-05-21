local lemoon = require "lemoon"
local datetime = require "lemoon.datetime"
local millisecondsOfTick = 10
local now = lemoon.now

local module = 
{
	io = lemoon.io(),
	timers = lemoon.timer(millisecondsOfTick)
}

function module.run(self,times)

	local timestamp = now()	

	for i = 0, times do
		self.io:dispatch(millisecondsOfTick)
		local newtimestamp = now()
		if datetime.duration(newtimestamp,timestamp) >= millisecondsOfTick then
			self.timers:tick()
			timestamp = newtimestamp
		end

	end
end

function module.make_timeout(timerwheel, timeout, func )
	func()
	timerwheel:timeout(timeout,function() module.make_timeout(timerwheel,timeout,func) end)
end

return module