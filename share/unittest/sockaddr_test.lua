local lemoon = require "lemoon"

for i,v in ipairs(lemoon.nslookup("localhost","")) do 
	print(string.format("(%d):%s",i,v))
end

local now = lemoon.now()

for i,v in pairs(now) do 
	print(string.format("(%s):%s",i,v))
end

print(string.format("%s.%06d",os.date("%c",now.tv_sec),now.tv_usec))
