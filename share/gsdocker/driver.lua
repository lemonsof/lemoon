local lemoon = require "lemoon"
local timer = require "gsdocker.timer"
local connection = require "gsdocker.connection"


local module = {}

local ioservice = lemoon.io()

function module.connect(host,port)
  connection.connect(ioservice,host,port)
end

function module.send(msg)
    connection.send(msg)
end

function module.GC(api)
    connection.GC(api)
end

function module.close()
    connection.close()
end

-- dispatch io event
function module.dispatch(timeout,times)
  for i = 1,times do
    timer.dispatch()
    connection.dispatch()
    -- no more io event
    if 0 == ioservice:dispatch(timeout) then
      break
    end
  end
end

return module
