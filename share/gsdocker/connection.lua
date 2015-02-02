local lemoon = require "lemoon"
local timer = require "gsdocker.timer"
local Code = require "gsdocker.code"
local io = require "gsdocker.io"

local module = {}

-- the connected socket
local connection = nil
local status = io.DISCONNECTED
local cacheQ = {}
local waitQ = {}

local GC = nil

local closeconnect = function(cnn)
    cnn.sock:close()

    if connection == cnn then
        connection = nil
        if status ~= io.CLOSED then
            status = io.DISCONNECTED
        end
    end

    if status ~= io.CLOSED then
        timer.timeout(5000,function()
            doconnect()
        end)
    end
end

module.recvmessage = function(cnn)
    io.readmessage(cnn.sock,function(err,reader)
        if err ~= nil then
            closeconnect(cnn)
            return
        end

        local code = reader:ReadByte()

        reader:ReadUint16() -- read length

        cnn.dhkey:decode(reader)

        if code == Code.Call and GC ~= nil then
            local call = {}
            call.id = reader:ReadUint16()
            call.service = reader:ReadUint16()
            call.Method = reader:ReadUint16()

            local count = reader:ReadUint16()

            call.parameters = {}

            for i = 0,count - 1 do
                local length = reader:ReadUint16()
                call.parameters[i]= lemoon.buff(reader:ReadBytes(length))
            end

            call.Parameters = function(self)
                return self.parameters
            end

            call.Parameters = function(self)
                return self.parameters
            end

            call.returns = {}

            call.NewReturn = function(call)
                local param = lemoon.buff ()
                call.returns[#call.returns + 1] = param
                return param
            end


            call.Invoke = function(call)
                local stream = lemoon.buff ()
                stream:WriteUint16 (call.id);
                stream:WriteUint16 (call.service);
                stream:WriteUint16 (#call.returns);
                for i,v in ipairs(call.returns) do
                    stream:WriteUint16(v:length())
                    stream:WriteBytes(v)
                end

                local msg =
                {
                    code = Code.Return;
                    content = stream;
                }

                module.send(msg)
            end

            GC(call)

            -- forward call
        end

        if code == Code.Return then
            local id = reader:ReadUint16()
            reader:ReadUint16() -- read service
            local count = reader:ReadUint16()

            local parameters = {}


            for i = 0,count - 1 do
                local length = reader:ReadUint16()
                parameters[i]= lemoon.buff(reader:ReadBytes(length))
            end

            if waitQ[id] ~= nil then
                waitQ[id](nil,parameters)
                waitQ[id] = nil
            end

        end

        module.recvmessage(cnn)
    end)
end

local connected = function(cnn)
    if status ~= io.CONNECTING then
        conn.sock:close()
        return
    end

    connection = cnn
    status = io.CONNECTED

    module.recvmessage(cnn)
end

local doconnect = function()

    if status ~= io.DISCONNECTED then return end

    status = io.CONNECTING

    local conn =
    {
        dhkey = lemoon.encoder("12345","4294967281");
        sock  = module.io:sock(io.AF_INET, io.SOCK_STREAM, io.IPPROTO_TCP)
    }

    conn.sock:connect(module.host,module.port,function(err)
        if err ~= nil then
            closeconnect(conn)
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

    local stream = lemoon.buff()
    print("before :" .. tostring(msg.content))
    connection.dhkey:encode(msg.content)
    print("after :" .. tostring(msg.content))
    stream:WriteUint16(msg.content:length() + 3)
    stream:WriteByte(msg.code)
    stream:WriteUint16(msg.content:length())
    stream:WriteBytes(msg.content)

    connection.sock:send(stream,function(err)

        if err ~= nil then
            closeconnect(connection)
            callback(err)
        end

        if msg.code == Code.Call and msg.callback ~= nil then
            waitQ[msg.id] = msg.callback
        end

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


function module.GC(api)
    GC = api
end

function module.send(msg)
    if status == io.CLOSED then return end
    append(cacheQ,msg)
end

function module.close()
    status = io.CLOSED
    if connection ~= nil then
        closeconnect(connection)
    end
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
