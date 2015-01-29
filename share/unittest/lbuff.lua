local lemoon = require "lemoon"

local stream = lemoon.buff("hello world")

print(stream)

if stream:ReadBytes(2) ~= "he" then
    error("check ReadBytes error")
end

local stream2 = lemoon.buff()

stream2:WriteBytes(stream,3)


if stream2:ReadBytes(-1) ~= "llo" then
    error("check WriteBytes error")
end

if stream:ReadBytes(8) ~= "llo worl" then
    error("check WriteBytes error")
end


if stream:ReadByte() ~= string.byte('d') then
    error("check ReadByte error")
end

stream:WriteByte(254)

if stream:ReadByte() ~= 254 then
    error("check ReadSByte error")
end


stream:WriteSByte(-4)

if stream:ReadSByte() ~= -4 then
    error("check ReadSByte error")
end



stream:WriteInt16(-4280)


if stream:ReadInt16() ~= -4280 then
    error("check WriteInt16 error")
end


stream:WriteInt64(-4280000)


if stream:ReadInt64() ~= -4280000 then
    error("check WriteInt16 error")
end

stream:WriteFloat64(-4280000000000)


if stream:ReadFloat64() ~= -4280000000000 then
    error("check WriteInt16 error")
end

stream:WriteString("-4280000000000")


if stream:ReadString() ~= "-4280000000000" then
    error("check WriteString error")
end
