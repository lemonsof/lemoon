local lemoon = require "lemoon"

local module = {}


local network = nil



local callmetatable = {}

function callmetatable.NewParam(call)
    local param = lemoon.writer ()
    call.params[#call.params + 1] = param
    return param
end


function callmetatable.Invoke(call)

end


function module.NewCall(service)

    local call = {
        service = service;

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
