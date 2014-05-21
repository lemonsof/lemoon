local lemoon = require "lemoon"
local util = require "util"
local io = util.io
local timers = util.timers
local make_timeout = util.make_timeout
local datetime = require "lemoon.datetime"

local server = io:sock(2,1)

server:bind("localhost","13512")

server:listen()


function make_echo(conn, times, timestamp)
	conn:send(string.format("[%s,%d]",conn,times))
	conn:recv (56 , function ( err, msg )
		if err ~= nil  then
			print(string.format("TCPClient Connection(%s) recv error :%s", conn, err))
		elseif #msg == 0 then
			--print(string.format("TCPClient Connection(%s) remote close connection", conn))
		else
			--print(string.format("TCPClient Connection(%s) recv echo msg :%s", conn, msg))
			if times > 0 then
				make_echo( conn, times -1, timestamp )
			else
				print(string.format("TCPClient Connection(%s) done : %d ms", conn,datetime.duration(lemoon.now(),timestamp)))
				conn:close()				
			end
		end
	end)	
end

for i = 0,10 do 
	local client = io:sock(2,1)
	client:connect("127.0.0.1","13512",function( err )
		if err ~= nil then
			print(string.format("tcp(%s) connect failed :%s",client,err))
		else
			--print("connection created",client)
			make_echo(client,10 , lemoon.now())
		end
	end)
end


function server_echo(conn)
	conn:recv (56 , function ( err, msg )
		if err ~= nil  then
			print(string.format("TCPServer Connection(%s) recv error :%s", conn, err))
		elseif #msg == 0 then
			--print(string.format("TCPServer Connection(%s) remote close connection", conn))
		else
			--print(string.format("TCPServer Connection(%s) recv msg :%s", conn, msg))
			conn:send(msg)
			--print(string.format("TCPServer Connection(%s) send echo msg :%s", conn, msg))
			server_echo(conn)
		end
	end)	
end


function accept( err, conn, remote )
	if err ~= nil then
		print("accept failed " .. err)
	else 
		--print(string.format("accept connect(%s) from:%s ", conn, remote))
		server_echo(conn)
	end 

	server:accept(accept)
end

server:accept(accept)


--util:run(10000000000)