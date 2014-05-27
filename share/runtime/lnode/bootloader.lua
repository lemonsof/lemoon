
local datetime = require "lemoon.datetime"
local lemoon = require "lemoon"

local bootloader = 
{
	timer_accuracy = 10;
	timestamp = lemoon.now();
	io = lemoon.io();
	net = require "lnode.net";
}

function bootloader.openlib( libname )
	local lib = require(libname)
	for k ,v  in pairs (lib) do
		bootloader[k] = v
	end
end

function bootloader.dispatch(  )
	local newtimestamp = lemoon.now()
	if datetime.duration(newtimestamp,bootloader.timestamp) >= bootloader.timer_accuracy then
		bootloader.timer:tick()
		bootloader.timestamp = newtimestamp
	end
	bootloader.io:dispatch(bootloader.timer_accuracy)
end

function bootloader.loadservice( servicename, ... )
	local service = require(servicename)
	local coro = coroutine.create(service.onload)
	print(coroutine.resume(coro, bootloader, ... ))
end

function bootloader.onload( nodename, mainservice, ...)
	bootloader.name = nodename

	bootloader.timer = lemoon.timer(bootloader.timer_accuracy)

	bootloader.openlib("lnode.socket")

	bootloader.loadservice( mainservice, ... )

	while true do
		bootloader.dispatch()	
	end

end


return bootloader