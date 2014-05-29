
local datetime = require "lemoon.datetime"
local lemoon = require "lemoon"

local bootloader = 
{
	timer_accuracy = 10;
	timestamp = lemoon.now();
	io = lemoon.io();
	net = require "lnode.net";
	running = {}
}

function bootloader.wakeup( coro, ... )
	if not bootloader.running[coro] then
		bootloader.running[coro] = table.pack(...)
	end
end

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

	for coro , context in pairs(bootloader.running) do
		if context then 
			local status , msg = coroutine.resume(coro,table.unpack(context))
			if not status then
				print(string.format("load %s failed :%s", coro, msg))
			end
			bootloader.running[coro] = nil
		end
	end

	bootloader.io:dispatch(bootloader.timer_accuracy)
end

function bootloader.loadservice( servicename, ... )
	local service = require(servicename)
	if not service then
		print(string.format("load service(%s) failed : not found",servicename))
		return
	end
	local coro = coroutine.create(service.onload)
	local status , msg = coroutine.resume(coro, bootloader, ... )

	if not status then
		print(string.format("load service(%s) failed :%s",servicename,msg))
	end
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