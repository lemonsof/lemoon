local lemoon = require "lemoon"

local timerwheel = lemoon.timerwheel(10)

local ticks = 0

function timeout( milliseconds, callback )
	timerwheel:timeout(milliseconds,function( ... )
		callback()
		timeout(milliseconds,callback)
	end)
end

timeout(100,function( ... )
	ticks = ticks + 1
end)

for i = 1, 100 do
	timerwheel:tick()
end

assert(ticks == 10)
