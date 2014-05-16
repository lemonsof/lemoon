local lemoon = require "lemoon"
local util = require "util"
local io = util.io
local timers = util.timers
local make_timeout = util.make_timeout

local server = io:sock(2,2)

local client = io:sock(2,2)

server:bind("localhost","13512")

make_timeout(timers,1000,function( ... )
	print("send udp data")
	client:sendto("hello lemoon udp socket","127.0.0.1","13512")
end)

function recvfrom( err, msg, remote )
	if err ~= nil then
		print("call recfromt exception " .. err)
	else 
		print(string.format("recive data :%s\n\tfrom:%s ",msg,remote))
	end 

	server:recvfrom(56,recvfrom)
end

server:recvfrom(56,recvfrom)

util:run(10000000000)
