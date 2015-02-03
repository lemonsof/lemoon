local lemoon = require "lemoon"
local Code = require "gsdocker.code"

local module = {}


local network = nil

local callmetatable = {}

local idgen = 0

function callmetatable.NewParam(call)
    local param = lemoon.buff ()
    call.params[#call.params + 1] = param
    return param
end


function callmetatable.Invoke(call)
    local stream = lemoon.buff ()
    stream:WriteUint16 (idgen);
    stream:WriteUint16 (call.Service);
    stream:WriteUint16 (call.Method);
    stream:WriteUint16 (#call.params);
    for i,v in ipairs(call.params) do
        stream:WriteUint16(v:length())
        stream:WriteBytes(v)
    end

    local msg =
    {
        code = Code.Call;
        content = stream;
        id = idgen;
        callback = call.Return.callback;
    }

    network.send(msg)

    idgen = idgen + 1
end


function module.NewCall(self,service)

    local call = {
        Service = service;

        params = {};

        Return = {}
    }

    call.Return.Add = function(self,callback)
        self.callback = callback
    end

    call.Invoke = callmetatable.Invoke
    call.NewParam = callmetatable.NewParam

    return call

end

function module.open(driver)
    network = driver
end


return module
