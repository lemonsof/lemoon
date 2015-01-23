local module = {}
local gsnet = require "gsdocker.rpc"


module.ReadPoint = function(stream)
	local val = {}

	val.X = stream:ReadFloat32()

	val.Y = stream:ReadFloat32()

	return val
end

module.WritePoint = function(stream,val)

	if val.X == nil then
		error("expect Point#X")
	end
	stream:WriteFloat32(val.X)

	if val.Y == nil then
		error("expect Point#Y")
	end
	stream:WriteFloat32(val.Y)

end



module.ReadRect = function(stream)
	local val = {}

	val.X = stream:ReadFloat32()

	val.Y = stream:ReadFloat32()

	val.Width = stream:ReadFloat32()

	val.Height = stream:ReadFloat32()

	return val
end

module.WriteRect = function(stream,val)

	if val.X == nil then
		error("expect Rect#X")
	end
	stream:WriteFloat32(val.X)

	if val.Y == nil then
		error("expect Rect#Y")
	end
	stream:WriteFloat32(val.Y)

	if val.Width == nil then
		error("expect Rect#Width")
	end
	stream:WriteFloat32(val.Width)

	if val.Height == nil then
		error("expect Rect#Height")
	end
	stream:WriteFloat32(val.Height)

end



module.ReadLine = function(stream)
	local val = {}

	val.From = module.ReadPoint(stream)

	val.To = module.ReadPoint(stream)

	return val
end

module.WriteLine = function(stream,val)

	if val.From == nil then
		error("expect Line#From")
	end
	module.WritePoint(stream,val.From)

	if val.To == nil then
		error("expect Line#To")
	end
	module.WritePoint(stream,val.To)

end





module.Mode =
{

	Box = 4,

	Marker = 2,

	Watcher = 1,

}


module.ReadMode = function(stream)
	return stream:ReadByte()
end

module.WriteMode = function(stream,val)
	stream:WriteByte(val)
end

return module
