local lemoon = require "lemoon"

local client = lemoon.encoder("12345","4294967281")

print("client :" .. tostring(client))


local server = lemoon.encoder("12345","4294967281")
print("server :" .. tostring(server))


server:apply(client:E())
client:apply(server:E())

print("client :" .. tostring(client))
print("server :" .. tostring(server))


local stream = lemoon.buff("i am decode text")

server:encode(stream)

client:decode(stream)

print(stream:ReadBytes(-1))
