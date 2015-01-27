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

module.recvmessage = function(cnn)
    io.readmessage(cnn.sock,function(err,reader)
        if err ~= nil then
            closeconnect(cnn)
            return
        end

        local code = reader:ReadByte()

        reader:ReadUint16() -- read length

        cnn.dhkey:decode(reader)

        if code == Code.Call then
            local call = {}
            call.id = reader:ReadUint16()
            call.service = reader:ReadUint16()
            call.Method = reader:ReadUint16()

            local count = reader:ReadUint16()

            call.parameters = {}


            for i = 0,count - 1 do
                local length = reader:ReadUint16()
                call.parameters[i]= lemoon.reader(reader:bytes(length))
            end

            call.Parameters = function(self)
                return self.parameters
            end

            call.Parameters = function(self)
                return self.parameters
            end

            call.NewParam = function(call)
                local param = lemoon.writer ()
                call.returns[#call.returns + 1] = param
                return param
            end


            call.Invoke = function(call)
                local stream = lemoon.writer ()
                stream:WriteUint16 (call.id);
                stream:WriteUint16 (call.service);
                stream:WriteUint16 (#call.returns);
                for i,v in ipairs(call.params) do
                    stream:WriteUint16(v:length())
                    stream:write(v)
                end

                local msg =
                {
                    code = Code.Return;
                    content = stream;
                }

                module.send(msg)
            end

            -- forward call
        end

        if code == Code.Return then
            local id = reader:ReadUint16()
            reader:ReadUint16() -- read service
            local count = reader:ReadUint16()

            local parameters = {}


            for i = 0,count - 1 do
                local length = reader:ReadUint16()
                parameters[i]= lemoon.reader(reader:bytes(length))
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
