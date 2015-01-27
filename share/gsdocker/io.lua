local lemoon = require "lemoon"
local Code = require "gsdocker.code"
local io =
{
    AF_UNSPEC = 0;
    AF_INET = 2;
    AF_IMPLINK = 3;
    AF_PUP = 4;
    AF_CHAOS = 5;
    AF_NS = 6;
    AF_INET6 = 30;
    AF_IEEE80211 = 37;
    SOCK_STREAM = 1;
    SOCK_DGRAM = 2;
    SOCK_RAW = 3;
    SOCK_SEQPACKET = 5;
    IPPROTO_ICMP = 1;
    IPPROTO_TCP = 6;
    IPPROTO_UDP = 17;

    DISCONNECTED = 1;
    CONNECTING = 2;
    CONNECTED = 3;
    CLOSED = 4;
}

io.readfull = function(sock, reader, len, callback)
    sock:recv(len,function(err,bytes)

        if err ~= nil then
            callback(err)
            return
        end

        reader:append(bytes)

        len = len - #bytes

        if len > 0 then
            io.readfull(sock,reader,len,callback)
        else
            callback(nil,reader)
        end

    end)
end

io.readmessage = function(sock, callback)
    local reader = lemoon.reader()
    io.readfull(sock,reader,2,function(err)

        if err ~= nil then
            callback(err)
            return
        end

        local len = reader:ReadUint16()

        io.readfull(sock,reader,len,function(err)
            callback(err,reader)
        end)

    end)
end



io.doexchangekey = function(conn,callback)
    local stream = lemoon.writer()

    local E = conn.dhkey:E()

    stream:WriteUint16(#E + 3)
    stream:WriteByte(Code.DHExchange)
    stream:WriteUint16(#E);
    stream:write(E)
    conn.sock:send(stream,function(err)
        if err ~= nil then
            callback(err)
            return
        end

        io.readmessage(conn.sock,function(err,reader)
            if err ~= nil then
                callback(err)
                return
            end

            local code = reader:ReadByte()

            if code ~= Code.DHExchange then
                sock:close()
                callback("dh exchange step2 err")
                return
            end

            reader:ReadUint16()

            conn.dhkey:Gen(reader:bytes())

            print(conn.dhkey:Key())

            callback(nil)

        end)
    end)


end

return io
