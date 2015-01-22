local driver = require "gsdocker.driver"

driver.connect("172.20.101.245","13512")


while true do
  driver.dispatch(10,100)
end
