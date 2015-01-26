local lemoon = require "lemoon"
local Code = require "gsdocker.code"

local module = {}


local network = nil

local callmetatable = {}

function callmetatable.NewParam(call)
    local param = lemoon.writer ()
    call.params[#call.params + 1] = param
    return param
end


function callmetatable.Invoke(call)
    local stream = lemoon.writer ()
    stream:WriteUint16 (call.ID);
    stream:WriteUint16 (call.Service);
    stream:WriteUint16 (this.Method);

    for i,v in ipairs(call.params) do
        stream:WriteUint16(v:length())
        stream:Write(v)
    end

    local msg = {code = Code.Call;content = stream}

    network.send(msg)
end


function module.NewCall(service)

    local call = {
        Service = service;

        params = {};

        Return = {}
    }

    call.Return.Add = function(self,callback)
        self.callback = callback
    end

    setmetatable (call, callmetatable)

    return call

end

function module.open(driver)
    network = driver
end


return module
