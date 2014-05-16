local lemoon = require "lemoon"
local datetime = require "lemoon.datetime"
local millisecondsOfTick = 100
local gettimeofday = lemoon.gettimeofday

local module = 
{
	io = lemoon.io_service(),
	timers = lemoon.timerwheel(millisecondsOfTick)
}

function module.run(self,times)

	local timestamp = gettimeofday()	

	for i = 0, times do
		self.io:dispatch_one(millisecondsOfTick)
		if datetime.duration(gettimeofday(),timestamp) >= millisecondsOfTick then
			self.timers:tick()
		end

	end
end

function module.make_timeout(timerwheel, timeout, func )
	func()
	timerwheel:timeout(timeout,function() module.make_timeout(timerwheel,timeout,func) end)
end

return module