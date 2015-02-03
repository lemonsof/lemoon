local driver = require "gsdocker.driver"
local rpc = require "gsdocker.rpc"
local ygg = require "lib.tap4fun.com.403-server.ygg"

rpc.open(driver)

driver.connect("172.20.101.195","13512")

ygg.GW.LoginWithUDID("helloworld",function(err,status)
    if err ~= nil then print(err) end

    if status ~= ygg.Err.Success then
        print("login err :" .. status)
    end
    print("hello")
end)

while true do
  driver.dispatch(10,100)
end
