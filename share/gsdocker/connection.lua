local lemoon = require "lemoon"
local timer = require "gsdocker.timer"
local code = require "gsdocker.code"
local io = require "gsdocker.io"

local module = {}

-- the connected socket
local connection = nil
local status = io.DISCONNECTED

local closeconnect = function(cnn)
    cnn.sock:close()

    if connection == cnn then
        connection = nil
        status = io.DISCONNECTED
    end

    if status ~= io.CLOSED then
        timer.timeout(5000,function()
            doconnect()
        end)
    end
end

local connected = function(cnn)
    if status ~= io.CONNECTING then
        conn.sock:close()
        return
    end

    connection = cnn
    status = io.CONNECTED
end

local doconnect = function()

    if status ~= io.DISCONNECTED then return end

    status = io.CONNECTING

    local conn =
    {
        dhkey = lemoon.dhkey(12345,0xfffffff0);
        sock  = module.io:sock(io.AF_INET, io.SOCK_STREAM, io.IPPROTO_TCP)
    }

    conn.sock:connect(module.host,module.port,function(err)
        if err ~= nil then
            closeconnect(cnn)
            return
        end

        io.doexchangekey(conn,function(err)
            connected(conn)
        end)
    end)


end

function module.connect(io,host,port)
    module.io = io
    module.host = host
    module.port = port
    doconnect()
end


function module.send(msg)
end


return module
