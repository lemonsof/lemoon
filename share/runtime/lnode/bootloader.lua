local BIF = require "lnode.BIF"

local bootloader = {}

BIF.open( bootloader, "lnode.io")
BIF.open( bootloader, "lnode.chan")
BIF.open( bootloader, "lnode.timer")
BIF.open( bootloader, "lnode.service")

function bootloader.onload( startservice, ... )

	bootloader:startservice(startservice, ...)

	while true do
		bootloader:dispatch( )
	end
end

return bootloader