local lemoon = require "lemoon"
local timer = require "gsdocker.timer"


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
}

local module = {}

local connection = {}

local doconnect = function()
  if connection.sock ~= nil then return end

  connection.sock = connection.io:sock(io.AF_INET, io.SOCK_STREAM, io.IPPROTO_TCP)

  connection.sock:connect( connection.host,connection.port , function ( err )
    if err ~= nil then
      timer.timeout(2000,function()
        -- reconnect
        doconnect()
        end)
      end
  end)
end

function module.connect(io,host,port)
  connection.io = io
  connection.host = host
  connection.port = port
  doconnect()
end



return module
