local lemoon = require "lemoon"

local check_tick = function ( tick, timeout )
	print(string.format("check timeout tick(%d,%d)",tick,timeout))
	local durationOfTick = tick
	local timeout = timeout	

	local timer = lemoon.timer(durationOfTick)	

	local status = false	

	timer:timeout(timeout,function ( ... )
		status = true
	end)	
	

	for i = 0 , timeout /  durationOfTick do
		timer:tick()
	end	

	assert(status,"check timeout tick failed !!!!!!!!!!!")

end 

check_tick(10,1000)
check_tick(10,5000)
check_tick(10,262144)

