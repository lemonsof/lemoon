local lemoon = require "lemoon"
local util = require "util"
local io = util.io
local timers = util.timers
local make_timeout = util.make_timeout


local server = io:sock(2,1)

server:bind("localhost","13512")

server:listen()

make_timeout(timers,1000,function( ... )

	local client = io:sock(2,1)
	client:connect("127.0.0.1","13512",function( err )
		if err ~= nil then
			print(string.format("tcp(%s) connect failed :%s",client,err))
		else
			print(string.format("tcp(%s) connect success",client))
			client:send("hello world",function ( err )
				if err ~= nil then
					print(string.format("tcp(%s) send failed :%s",client,err))
				else 
					print(string.format("tcp(%s) send success",client))
				end
			end)
		end
	end)

end)

function accept( err, conn, remote )
	if err ~= nil then
		print("accept failed " .. err)
	else 
		print(string.format("accept connect(%s) from:%s ", conn, remote))
		conn:recv(56,function ( err, msg)
			if err ~= nil then
				print(string.format("recv from conn(%s) failed :%s", conn, remote))
			else 
				print(string.format("recv from conn(%s) success :%s\n\t%s", conn, remote, msg))
			end
		end)
	end 

	server:accept(accept)
end

server:accept(accept)

