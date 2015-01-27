local lemoon = require "lemoon"
local timer = require "gsdocker.timer"
local code = require "gsdocker.code"
local io = require "gsdocker.io"

local module = {}

-- the connected socket
local connection = nil
local status = io.DISCONNECTED
local cacheQ = {}
local waitQ = {}

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

local append = function(Q,msg)
    if Q.tail ~= nil then
        Q.tail.next = msg
        Q.tail = msg
    else
        Q.head = msg
        Q.tail = msg
    end
end

local dosend = function(msg,callback)

    local stream = lemoon.writer()
    print("before :" .. msg.content:string())
    connection.dhkey:encode(msg.content)
    print("after :" .. msg.content:string())
    stream:WriteUint16(msg.content:length() + 3)
    stream:WriteByte(msg.code)
    stream:WriteUint16(msg.content:length())
    stream:write(msg.content)

    connection.sock:send(stream,function(err)

        if err ~= nil then
            closeconnect(cnn)
            callback(err)
        end
        print("send encrypt data")
        callback(nil,msg)

    end)
end

local makerror = function(msg)
    local err = {msg = msg}

    err.ToString = function(self)
        return self.msg
    end

    return err
end


function module.connect(io,host,port)
    module.io = io
    module.host = host
    module.port = port
    doconnect()
end

function module.send(msg)
    if status == io.CLOSED then return end
    append(cacheQ,msg)
end

function module.dispatch()
    if status ~= io.CONNECTED then
        return
    end

    local Q = {}

    local current = cacheQ.head

    while current ~= nil do

        if status == io.CONNECTED then
            dosend(current,function(err,msg)
                if err ~= nil then
                    append(Q,msg)
                    return
                end
            end)
        else
            append(Q,current)
        end

        current = current.next
    end

    cacheQ = Q
end


return module
