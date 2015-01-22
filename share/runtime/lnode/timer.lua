local lemoon = require "lemoon"

local timer =
{
	timer_accuracy = 10
}

local timewheel = lemoon.timer(timer.timer_accuracy)

local timestamp = lemoon.now()

function timer.timeout(node, timeout, callback )
	timewheel:timeout(timeout,callback)
end

function timer.duration( from, to )
	return to.tv_sec *  1000 + to.tv_usec / 1000 - from.tv_sec *  1000 - from.tv_usec / 1000
end

function timer.dispatch()
	local now = lemoon.now()
	if timer.duration( timestamp , now) > timer.timer_accuracy then
		timestamp = now
		timewheel:tick()
	end
end

return timer
