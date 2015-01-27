local gsaoi = require "lib.tap4fun.com.gsdocker.gsaoi"
local gsnet = require "gsdocker.rpc"
local module = {}


module.ReadResLevelOfArea = function(stream)
	local val = {}

	val.AreaLevel = stream:ReadByte()

	val.ResLevel = stream:ReadByte()

	val.Percentage = stream:ReadByte()

	return val
end

module.WriteResLevelOfArea = function(stream,val)

	if val.AreaLevel == nil then
		error("expect ResLevelOfArea#AreaLevel")
	end
	stream:WriteByte(val.AreaLevel)

	if val.ResLevel == nil then
		error("expect ResLevelOfArea#ResLevel")
	end
	stream:WriteByte(val.ResLevel)

	if val.Percentage == nil then
		error("expect ResLevelOfArea#Percentage")
	end
	stream:WriteByte(val.Percentage)

end



module.ReadResTypePercentage = function(stream)
	local val = {}

	val.ResType = stream:ReadByte()

	val.ResName = stream:ReadString()

	val.Percentage = stream:ReadByte()

	return val
end

module.WriteResTypePercentage = function(stream,val)

	if val.ResType == nil then
		error("expect ResTypePercentage#ResType")
	end
	stream:WriteByte(val.ResType)

	if val.ResName == nil then
		error("expect ResTypePercentage#ResName")
	end
	stream:WriteString(val.ResName)

	if val.Percentage == nil then
		error("expect ResTypePercentage#Percentage")
	end
	stream:WriteByte(val.Percentage)

end



module.ReadResRefreshCondition = function(stream)
	local val = {}

	val.SizeX = stream:ReadUint16()

	val.SizeY = stream:ReadUint16()

	val.MaxVal = stream:ReadInt32()

	val.CountDown = stream:ReadUint32()

	val.ClearTime = stream:ReadUint32()

	val.Speedup = stream:ReadUint16()

	val.OnceNum = stream:ReadInt32()

	return val
end

module.WriteResRefreshCondition = function(stream,val)

	if val.SizeX == nil then
		error("expect ResRefreshCondition#SizeX")
	end
	stream:WriteUint16(val.SizeX)

	if val.SizeY == nil then
		error("expect ResRefreshCondition#SizeY")
	end
	stream:WriteUint16(val.SizeY)

	if val.MaxVal == nil then
		error("expect ResRefreshCondition#MaxVal")
	end
	stream:WriteInt32(val.MaxVal)

	if val.CountDown == nil then
		error("expect ResRefreshCondition#CountDown")
	end
	stream:WriteUint32(val.CountDown)

	if val.ClearTime == nil then
		error("expect ResRefreshCondition#ClearTime")
	end
	stream:WriteUint32(val.ClearTime)

	if val.Speedup == nil then
		error("expect ResRefreshCondition#Speedup")
	end
	stream:WriteUint16(val.Speedup)

	if val.OnceNum == nil then
		error("expect ResRefreshCondition#OnceNum")
	end
	stream:WriteInt32(val.OnceNum)

end



module.ReadRes = function(stream)
	local val = {}

	val.Name = stream:ReadString()

	val.Level = stream:ReadByte()

	val.ResType = stream:ReadByte()

	val.ResCnt = stream:ReadUint32()

	val.GatherSpeed = stream:ReadUint32()

	return val
end

module.WriteRes = function(stream,val)

	if val.Name == nil then
		error("expect Res#Name")
	end
	stream:WriteString(val.Name)

	if val.Level == nil then
		error("expect Res#Level")
	end
	stream:WriteByte(val.Level)

	if val.ResType == nil then
		error("expect Res#ResType")
	end
	stream:WriteByte(val.ResType)

	if val.ResCnt == nil then
		error("expect Res#ResCnt")
	end
	stream:WriteUint32(val.ResCnt)

	if val.GatherSpeed == nil then
		error("expect Res#GatherSpeed")
	end
	stream:WriteUint32(val.GatherSpeed)

end



module.ReadApplication = function(stream)
	local val = {}

	val.PlayerID = stream:ReadString()

	val.Remark = stream:ReadString()

	return val
end

module.WriteApplication = function(stream,val)

	if val.PlayerID == nil then
		error("expect Application#PlayerID")
	end
	stream:WriteString(val.PlayerID)

	if val.Remark == nil then
		error("expect Application#Remark")
	end
	stream:WriteString(val.Remark)

end



module.ReadInvitation = function(stream)
	local val = {}

	val.AllianceID = stream:ReadString()

	val.Remark = stream:ReadString()

	return val
end

module.WriteInvitation = function(stream,val)

	if val.AllianceID == nil then
		error("expect Invitation#AllianceID")
	end
	stream:WriteString(val.AllianceID)

	if val.Remark == nil then
		error("expect Invitation#Remark")
	end
	stream:WriteString(val.Remark)

end



module.ReadHelp = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.PlayerID = stream:ReadString()

	val.HelpType = module.ReadHelpType(stream)

	val.HelpCount = stream:ReadByte()

	val.HelpedCount = stream:ReadByte()

	val.Comment = stream:ReadString()

	return val
end

module.WriteHelp = function(stream,val)

	if val.OID == nil then
		error("expect Help#OID")
	end
	stream:WriteString(val.OID)

	if val.PlayerID == nil then
		error("expect Help#PlayerID")
	end
	stream:WriteString(val.PlayerID)

	if val.HelpType == nil then
		error("expect Help#HelpType")
	end
	module.WriteHelpType(stream,val.HelpType)

	if val.HelpCount == nil then
		error("expect Help#HelpCount")
	end
	stream:WriteByte(val.HelpCount)

	if val.HelpedCount == nil then
		error("expect Help#HelpedCount")
	end
	stream:WriteByte(val.HelpedCount)

	if val.Comment == nil then
		error("expect Help#Comment")
	end
	stream:WriteString(val.Comment)

end





module.HelpType =
{

	Building = 0,

	Repair = 2,

	Technology = 1,

}


module.ReadHelpType = function(stream)
	return stream:ReadByte()
end

module.WriteHelpType = function(stream,val)
	stream:WriteByte(val)
end




module.ReadAllianceInfo = function(stream)
	local val = {}

	val.Name = stream:ReadString()

	val.Alias = stream:ReadString()

	val.President = stream:ReadString()

	val.Power = stream:ReadUint32()

	val.MemberCount = stream:ReadUint16()

	val.GiftLevel = stream:ReadByte()

	val.Introduction = stream:ReadString()

	val.Announcement = stream:ReadString()

	return val
end

module.WriteAllianceInfo = function(stream,val)

	if val.Name == nil then
		error("expect AllianceInfo#Name")
	end
	stream:WriteString(val.Name)

	if val.Alias == nil then
		error("expect AllianceInfo#Alias")
	end
	stream:WriteString(val.Alias)

	if val.President == nil then
		error("expect AllianceInfo#President")
	end
	stream:WriteString(val.President)

	if val.Power == nil then
		error("expect AllianceInfo#Power")
	end
	stream:WriteUint32(val.Power)

	if val.MemberCount == nil then
		error("expect AllianceInfo#MemberCount")
	end
	stream:WriteUint16(val.MemberCount)

	if val.GiftLevel == nil then
		error("expect AllianceInfo#GiftLevel")
	end
	stream:WriteByte(val.GiftLevel)

	if val.Introduction == nil then
		error("expect AllianceInfo#Introduction")
	end
	stream:WriteString(val.Introduction)

	if val.Announcement == nil then
		error("expect AllianceInfo#Announcement")
	end
	stream:WriteString(val.Announcement)

end





module.BuildingType =
{

	AncientRelic = 5,

	Embassy = 15,

	LumberYard = 3,

	Market = 14,

	MeadHall = 16,

	MedicalTent = 8,

	Mines = 4,

	NoBuilding = 0,

	OdinEye = 9,

	Oracle = 10,

	Pasture = 1,

	RockCave = 2,

	SkyWatch = 17,

	Tavern = 13,

	ThorForge = 11,

	TrainingCamp = 7,

	Treasury = 12,

	TurfHouse = 6,

	Walls = 18,

}


module.ReadBuildingType = function(stream)
	return stream:ReadByte()
end

module.WriteBuildingType = function(stream,val)
	stream:WriteByte(val)
end






module.BuildingStatus =
{

	Normal = 0,

	Removing = 2,

	Upgrading = 1,

}


module.ReadBuildingStatus = function(stream)
	return stream:ReadByte()
end

module.WriteBuildingStatus = function(stream,val)
	stream:WriteByte(val)
end




module.ReadBuildingCost = function(stream)
	local val = {}

	val.BuildingId = stream:ReadUint32()

	val.Type = module.ReadBuildingType(stream)

	val.Name = stream:ReadString()

	val.Level = stream:ReadByte()

	val.TileType = stream:ReadByte()

	val.DefaultPos = stream:ReadByte()

	val.CanRemove = stream:ReadUint32()

	val.RemoveCost = stream:ReadUint32()

	val.RemoveTime = stream:ReadUint32()

	val.MaxBuildNum = stream:ReadUint32()

	val.PreBuild1Type = module.ReadBuildingType(stream)

	val.PreBuild1Level = stream:ReadByte()

	val.PreBuild2Type = module.ReadBuildingType(stream)

	val.PreBuild2Level = stream:ReadByte()

	val.FoodReq = stream:ReadUint32()

	val.StoneReq = stream:ReadUint32()

	val.WoodReq = stream:ReadUint32()

	val.OreReq = stream:ReadUint32()

	val.SilverReq = stream:ReadUint32()

	val.GoldReq = stream:ReadUint32()

	val.Item1Req = stream:ReadUint32()

	val.Item1ReqCnt = stream:ReadUint32()

	val.Item2Req = stream:ReadUint32()

	val.Item2ReqCnt = stream:ReadUint32()

	val.ProductType = stream:ReadUint32()

	val.ProductCnt = stream:ReadUint32()

	val.Store = stream:ReadUint32()

	val.BuildTime = stream:ReadUint32()

	val.ExpAward = stream:ReadUint32()

	val.PowerAward = stream:ReadUint32()

	val.SpecialType1 = stream:ReadUint32()

	val.SpeciaVall1 = stream:ReadUint32()

	val.SpecialType2 = stream:ReadUint32()

	val.SpeciaVall2 = stream:ReadUint32()

	val.SpecialType3 = stream:ReadUint32()

	val.SpeciaVall3 = stream:ReadUint32()

	val.SpecialType4 = stream:ReadUint32()

	val.SpeciaVall4 = stream:ReadUint32()

	val.SpecialType5 = stream:ReadUint32()

	val.SpeciaVall5 = stream:ReadUint32()

	return val
end

module.WriteBuildingCost = function(stream,val)

	if val.BuildingId == nil then
		error("expect BuildingCost#BuildingId")
	end
	stream:WriteUint32(val.BuildingId)

	if val.Type == nil then
		error("expect BuildingCost#Type")
	end
	module.WriteBuildingType(stream,val.Type)

	if val.Name == nil then
		error("expect BuildingCost#Name")
	end
	stream:WriteString(val.Name)

	if val.Level == nil then
		error("expect BuildingCost#Level")
	end
	stream:WriteByte(val.Level)

	if val.TileType == nil then
		error("expect BuildingCost#TileType")
	end
	stream:WriteByte(val.TileType)

	if val.DefaultPos == nil then
		error("expect BuildingCost#DefaultPos")
	end
	stream:WriteByte(val.DefaultPos)

	if val.CanRemove == nil then
		error("expect BuildingCost#CanRemove")
	end
	stream:WriteUint32(val.CanRemove)

	if val.RemoveCost == nil then
		error("expect BuildingCost#RemoveCost")
	end
	stream:WriteUint32(val.RemoveCost)

	if val.RemoveTime == nil then
		error("expect BuildingCost#RemoveTime")
	end
	stream:WriteUint32(val.RemoveTime)

	if val.MaxBuildNum == nil then
		error("expect BuildingCost#MaxBuildNum")
	end
	stream:WriteUint32(val.MaxBuildNum)

	if val.PreBuild1Type == nil then
		error("expect BuildingCost#PreBuild1Type")
	end
	module.WriteBuildingType(stream,val.PreBuild1Type)

	if val.PreBuild1Level == nil then
		error("expect BuildingCost#PreBuild1Level")
	end
	stream:WriteByte(val.PreBuild1Level)

	if val.PreBuild2Type == nil then
		error("expect BuildingCost#PreBuild2Type")
	end
	module.WriteBuildingType(stream,val.PreBuild2Type)

	if val.PreBuild2Level == nil then
		error("expect BuildingCost#PreBuild2Level")
	end
	stream:WriteByte(val.PreBuild2Level)

	if val.FoodReq == nil then
		error("expect BuildingCost#FoodReq")
	end
	stream:WriteUint32(val.FoodReq)

	if val.StoneReq == nil then
		error("expect BuildingCost#StoneReq")
	end
	stream:WriteUint32(val.StoneReq)

	if val.WoodReq == nil then
		error("expect BuildingCost#WoodReq")
	end
	stream:WriteUint32(val.WoodReq)

	if val.OreReq == nil then
		error("expect BuildingCost#OreReq")
	end
	stream:WriteUint32(val.OreReq)

	if val.SilverReq == nil then
		error("expect BuildingCost#SilverReq")
	end
	stream:WriteUint32(val.SilverReq)

	if val.GoldReq == nil then
		error("expect BuildingCost#GoldReq")
	end
	stream:WriteUint32(val.GoldReq)

	if val.Item1Req == nil then
		error("expect BuildingCost#Item1Req")
	end
	stream:WriteUint32(val.Item1Req)

	if val.Item1ReqCnt == nil then
		error("expect BuildingCost#Item1ReqCnt")
	end
	stream:WriteUint32(val.Item1ReqCnt)

	if val.Item2Req == nil then
		error("expect BuildingCost#Item2Req")
	end
	stream:WriteUint32(val.Item2Req)

	if val.Item2ReqCnt == nil then
		error("expect BuildingCost#Item2ReqCnt")
	end
	stream:WriteUint32(val.Item2ReqCnt)

	if val.ProductType == nil then
		error("expect BuildingCost#ProductType")
	end
	stream:WriteUint32(val.ProductType)

	if val.ProductCnt == nil then
		error("expect BuildingCost#ProductCnt")
	end
	stream:WriteUint32(val.ProductCnt)

	if val.Store == nil then
		error("expect BuildingCost#Store")
	end
	stream:WriteUint32(val.Store)

	if val.BuildTime == nil then
		error("expect BuildingCost#BuildTime")
	end
	stream:WriteUint32(val.BuildTime)

	if val.ExpAward == nil then
		error("expect BuildingCost#ExpAward")
	end
	stream:WriteUint32(val.ExpAward)

	if val.PowerAward == nil then
		error("expect BuildingCost#PowerAward")
	end
	stream:WriteUint32(val.PowerAward)

	if val.SpecialType1 == nil then
		error("expect BuildingCost#SpecialType1")
	end
	stream:WriteUint32(val.SpecialType1)

	if val.SpeciaVall1 == nil then
		error("expect BuildingCost#SpeciaVall1")
	end
	stream:WriteUint32(val.SpeciaVall1)

	if val.SpecialType2 == nil then
		error("expect BuildingCost#SpecialType2")
	end
	stream:WriteUint32(val.SpecialType2)

	if val.SpeciaVall2 == nil then
		error("expect BuildingCost#SpeciaVall2")
	end
	stream:WriteUint32(val.SpeciaVall2)

	if val.SpecialType3 == nil then
		error("expect BuildingCost#SpecialType3")
	end
	stream:WriteUint32(val.SpecialType3)

	if val.SpeciaVall3 == nil then
		error("expect BuildingCost#SpeciaVall3")
	end
	stream:WriteUint32(val.SpeciaVall3)

	if val.SpecialType4 == nil then
		error("expect BuildingCost#SpecialType4")
	end
	stream:WriteUint32(val.SpecialType4)

	if val.SpeciaVall4 == nil then
		error("expect BuildingCost#SpeciaVall4")
	end
	stream:WriteUint32(val.SpeciaVall4)

	if val.SpecialType5 == nil then
		error("expect BuildingCost#SpecialType5")
	end
	stream:WriteUint32(val.SpecialType5)

	if val.SpeciaVall5 == nil then
		error("expect BuildingCost#SpeciaVall5")
	end
	stream:WriteUint32(val.SpeciaVall5)

end





module.NoticeType =
{

	ClanClosureWar = 8,

	ClanDeclareWar = 6,

	ClanKeepWar = 7,

	DemolishKingdom = 5,

	EstablishKingdom = 4,

	JoinClan = 0,

	JoinKingdom = 2,

	KingdomBeDeclarWar = 12,

	KingdomBeKeepWar = 13,

	KingdomClosureWar = 11,

	KingdomDeclareWar = 9,

	KingdomGetAlly = 16,

	KingdomGetTerritory = 14,

	KingdomKeepWar = 10,

	KingdomLostAlly = 17,

	KingdomLostTerritory = 15,

	LeaveClan = 1,

	LeaveKingdom = 3,

}


module.ReadNoticeType = function(stream)
	return stream:ReadByte()
end

module.WriteNoticeType = function(stream,val)
	stream:WriteByte(val)
end




module.ReadNotice = function(stream)
	local val = {}

	val.Type = module.ReadNoticeType(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = stream:ReadByte()
		end

		return val
	end
	val.Content = f()

	return val
end

module.WriteNotice = function(stream,val)

	if val.Type == nil then
		error("expect Notice#Type")
	end
	module.WriteNoticeType(stream,val.Type)

	if val.Content == nil then
		error("expect Notice#Content")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			stream:WriteByte(v)
		end
	end
	f(val.Content)

end



module.ReadJoinClanNotice = function(stream)
	local val = {}

	val.PlayerName = stream:ReadString()

	return val
end

module.WriteJoinClanNotice = function(stream,val)

	if val.PlayerName == nil then
		error("expect JoinClanNotice#PlayerName")
	end
	stream:WriteString(val.PlayerName)

end



module.ReadLeaveClanNotice = function(stream)
	local val = {}

	val.PlayerName = stream:ReadString()

	return val
end

module.WriteLeaveClanNotice = function(stream,val)

	if val.PlayerName == nil then
		error("expect LeaveClanNotice#PlayerName")
	end
	stream:WriteString(val.PlayerName)

end



module.ReadJoinKingdomNotice = function(stream)
	local val = {}

	val.ClanName = stream:ReadString()

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteJoinKingdomNotice = function(stream,val)

	if val.ClanName == nil then
		error("expect JoinKingdomNotice#ClanName")
	end
	stream:WriteString(val.ClanName)

	if val.KingdomName == nil then
		error("expect JoinKingdomNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadLeaveKingdomNotice = function(stream)
	local val = {}

	val.ClanName = stream:ReadString()

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteLeaveKingdomNotice = function(stream,val)

	if val.ClanName == nil then
		error("expect LeaveKingdomNotice#ClanName")
	end
	stream:WriteString(val.ClanName)

	if val.KingdomName == nil then
		error("expect LeaveKingdomNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadEstablishKingdomNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteEstablishKingdomNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect EstablishKingdomNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadDemolishKingdomNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteDemolishKingdomNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect DemolishKingdomNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadClanDeclareWarNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	val.CountDown = stream:ReadString()

	return val
end

module.WriteClanDeclareWarNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect ClanDeclareWarNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

	if val.CountDown == nil then
		error("expect ClanDeclareWarNotice#CountDown")
	end
	stream:WriteString(val.CountDown)

end



module.ReadClanKeepWarNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteClanKeepWarNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect ClanKeepWarNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadClanClosureWarNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteClanClosureWarNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect ClanClosureWarNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadKingdomDeclareWarNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	val.CountDown = stream:ReadString()

	return val
end

module.WriteKingdomDeclareWarNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect KingdomDeclareWarNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

	if val.CountDown == nil then
		error("expect KingdomDeclareWarNotice#CountDown")
	end
	stream:WriteString(val.CountDown)

end



module.ReadKingdomKeepWarNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteKingdomKeepWarNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect KingdomKeepWarNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadKingdomClosureWarNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteKingdomClosureWarNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect KingdomClosureWarNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadKingdomBeDeclarWarNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	val.CountDown = stream:ReadString()

	return val
end

module.WriteKingdomBeDeclarWarNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect KingdomBeDeclarWarNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

	if val.CountDown == nil then
		error("expect KingdomBeDeclarWarNotice#CountDown")
	end
	stream:WriteString(val.CountDown)

end



module.ReadKingdomBeKeepWarNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteKingdomBeKeepWarNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect KingdomBeKeepWarNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadKingdomGetTerritoryNotice = function(stream)
	local val = {}

	val.TerritoryName = stream:ReadString()

	return val
end

module.WriteKingdomGetTerritoryNotice = function(stream,val)

	if val.TerritoryName == nil then
		error("expect KingdomGetTerritoryNotice#TerritoryName")
	end
	stream:WriteString(val.TerritoryName)

end



module.ReadKingdomLostTerritoryNotice = function(stream)
	local val = {}

	val.TerritoryName = stream:ReadString()

	return val
end

module.WriteKingdomLostTerritoryNotice = function(stream,val)

	if val.TerritoryName == nil then
		error("expect KingdomLostTerritoryNotice#TerritoryName")
	end
	stream:WriteString(val.TerritoryName)

end



module.ReadKingdomGetAllyNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteKingdomGetAllyNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect KingdomGetAllyNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadKingdomLostAllyNotice = function(stream)
	local val = {}

	val.KingdomName = stream:ReadString()

	return val
end

module.WriteKingdomLostAllyNotice = function(stream,val)

	if val.KingdomName == nil then
		error("expect KingdomLostAllyNotice#KingdomName")
	end
	stream:WriteString(val.KingdomName)

end



module.ReadMail = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.PlayerID = stream:ReadString()

	val.Content = stream:ReadString()

	return val
end

module.WriteMail = function(stream,val)

	if val.ID == nil then
		error("expect Mail#ID")
	end
	stream:WriteString(val.ID)

	if val.PlayerID == nil then
		error("expect Mail#PlayerID")
	end
	stream:WriteString(val.PlayerID)

	if val.Content == nil then
		error("expect Mail#Content")
	end
	stream:WriteString(val.Content)

end





module.ReportType =
{

	BeInforcement = 13,

	BeScout = 6,

	BeTrade = 9,

	GarrisonSuccess = 16,

	GatherOver = 8,

	PVE = 7,

	PVP = 0,

	Raily = 2,

	ReceiveGift = 10,

	Reinforcement = 3,

	ScoutCitySuccess = 4,

	ScoutFail = 5,

	ScoutGarrisonSuccess = 14,

	ScoutGatherSuccess = 15,

	TargetChanged = 1,

	TradeFail = 11,

	TradeSuccess = 12,

}


module.ReadReportType = function(stream)
	return stream:ReadByte()
end

module.WriteReportType = function(stream,val)
	stream:WriteByte(val)
end




module.ReadReport = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.Type = module.ReadReportType(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = stream:ReadByte()
		end

		return val
	end
	val.Content = f()

	val.Time = stream:ReadString()

	return val
end

module.WriteReport = function(stream,val)

	if val.ID == nil then
		error("expect Report#ID")
	end
	stream:WriteString(val.ID)

	if val.Type == nil then
		error("expect Report#Type")
	end
	module.WriteReportType(stream,val.Type)

	if val.Content == nil then
		error("expect Report#Content")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			stream:WriteByte(v)
		end
	end
	f(val.Content)

	if val.Time == nil then
		error("expect Report#Time")
	end
	stream:WriteString(val.Time)

end



module.ReadPVPReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	val.Result = module.ReadAttackResult(stream)

	val.MyBattleInfo = module.ReadBattleInfo(stream)

	val.EnemyBattleInfo = module.ReadBattleInfo(stream)

	val.Resource = module.ReadResource(stream)

	return val
end

module.WritePVPReport = function(stream,val)

	if val.Point == nil then
		error("expect PVPReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect PVPReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

	if val.Result == nil then
		error("expect PVPReport#Result")
	end
	module.WriteAttackResult(stream,val.Result)

	if val.MyBattleInfo == nil then
		error("expect PVPReport#MyBattleInfo")
	end
	module.WriteBattleInfo(stream,val.MyBattleInfo)

	if val.EnemyBattleInfo == nil then
		error("expect PVPReport#EnemyBattleInfo")
	end
	module.WriteBattleInfo(stream,val.EnemyBattleInfo)

	if val.Resource == nil then
		error("expect PVPReport#Resource")
	end
	module.WriteResource(stream,val.Resource)

end



module.ReadBattleInfo = function(stream)
	local val = {}

	val.PowerReduce = stream:ReadUint32()

	val.PlayerName = stream:ReadString()

	val.KingdomName = stream:ReadString()

	val.AllianceName = stream:ReadString()

	val.CityLocation =  gsaoi.ReadPoint(stream)

	val.Hero = module.ReadHero(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.UnitsBefore = f()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.UnitsAfter = f()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.UnitsInjured = f()

	return val
end

module.WriteBattleInfo = function(stream,val)

	if val.PowerReduce == nil then
		error("expect BattleInfo#PowerReduce")
	end
	stream:WriteUint32(val.PowerReduce)

	if val.PlayerName == nil then
		error("expect BattleInfo#PlayerName")
	end
	stream:WriteString(val.PlayerName)

	if val.KingdomName == nil then
		error("expect BattleInfo#KingdomName")
	end
	stream:WriteString(val.KingdomName)

	if val.AllianceName == nil then
		error("expect BattleInfo#AllianceName")
	end
	stream:WriteString(val.AllianceName)

	if val.CityLocation == nil then
		error("expect BattleInfo#CityLocation")
	end
	gsaoi.WritePoint(stream,val.CityLocation)

	if val.Hero == nil then
		error("expect BattleInfo#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.UnitsBefore == nil then
		error("expect BattleInfo#UnitsBefore")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.UnitsBefore)

	if val.UnitsAfter == nil then
		error("expect BattleInfo#UnitsAfter")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.UnitsAfter)

	if val.UnitsInjured == nil then
		error("expect BattleInfo#UnitsInjured")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.UnitsInjured)

end



module.ReadTargetChangedReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	return val
end

module.WriteTargetChangedReport = function(stream,val)

	if val.Point == nil then
		error("expect TargetChangedReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect TargetChangedReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

end



module.ReadRailyReport = function(stream)
	local val = {}

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	val.Result = module.ReadAttackResult(stream)

	return val
end

module.WriteRailyReport = function(stream,val)

	if val.TargetName == nil then
		error("expect RailyReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect RailyReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect RailyReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

	if val.Point == nil then
		error("expect RailyReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect RailyReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

	if val.Result == nil then
		error("expect RailyReport#Result")
	end
	module.WriteAttackResult(stream,val.Result)

end



module.ReadReinforcementReport = function(stream)
	local val = {}

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	val.Result = module.ReadAttackResult(stream)

	return val
end

module.WriteReinforcementReport = function(stream,val)

	if val.TargetName == nil then
		error("expect ReinforcementReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect ReinforcementReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect ReinforcementReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

	if val.Point == nil then
		error("expect ReinforcementReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect ReinforcementReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

	if val.Result == nil then
		error("expect ReinforcementReport#Result")
	end
	module.WriteAttackResult(stream,val.Result)

end



module.ReadScoutCitySuccessReport = function(stream)
	local val = {}

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	val.Point =  gsaoi.ReadPoint(stream)

	val.Hero = module.ReadHero(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadReinforcementPlayer(stream)
		end

		return val
	end
	val.ReinforcementPlayers = f()

	val.Resource = module.ReadResource(stream)

	return val
end

module.WriteScoutCitySuccessReport = function(stream,val)

	if val.TargetName == nil then
		error("expect ScoutCitySuccessReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect ScoutCitySuccessReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect ScoutCitySuccessReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

	if val.Point == nil then
		error("expect ScoutCitySuccessReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.Hero == nil then
		error("expect ScoutCitySuccessReport#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.Units == nil then
		error("expect ScoutCitySuccessReport#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

	if val.ReinforcementPlayers == nil then
		error("expect ScoutCitySuccessReport#ReinforcementPlayers")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteReinforcementPlayer(stream,v)
		end
	end
	f(val.ReinforcementPlayers)

	if val.Resource == nil then
		error("expect ScoutCitySuccessReport#Resource")
	end
	module.WriteResource(stream,val.Resource)

end



module.ReadScoutGarrisonSuccessReport = function(stream)
	local val = {}

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	val.Point =  gsaoi.ReadPoint(stream)

	val.Hero = module.ReadHero(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()

	return val
end

module.WriteScoutGarrisonSuccessReport = function(stream,val)

	if val.TargetName == nil then
		error("expect ScoutGarrisonSuccessReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect ScoutGarrisonSuccessReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect ScoutGarrisonSuccessReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

	if val.Point == nil then
		error("expect ScoutGarrisonSuccessReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.Hero == nil then
		error("expect ScoutGarrisonSuccessReport#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.Units == nil then
		error("expect ScoutGarrisonSuccessReport#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

end



module.ReadScoutGatherSuccessReport = function(stream)
	local val = {}

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	val.Point =  gsaoi.ReadPoint(stream)

	val.Hero = module.ReadHero(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()

	return val
end

module.WriteScoutGatherSuccessReport = function(stream,val)

	if val.TargetName == nil then
		error("expect ScoutGatherSuccessReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect ScoutGatherSuccessReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect ScoutGatherSuccessReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

	if val.Point == nil then
		error("expect ScoutGatherSuccessReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.Hero == nil then
		error("expect ScoutGatherSuccessReport#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.Units == nil then
		error("expect ScoutGatherSuccessReport#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

end



module.ReadReinforcementPlayer = function(stream)
	local val = {}

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	val.Hero = module.ReadHero(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()

	return val
end

module.WriteReinforcementPlayer = function(stream,val)

	if val.TargetName == nil then
		error("expect ReinforcementPlayer#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect ReinforcementPlayer#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect ReinforcementPlayer#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

	if val.Hero == nil then
		error("expect ReinforcementPlayer#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.Units == nil then
		error("expect ReinforcementPlayer#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

end



module.ReadScoutFailReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	return val
end

module.WriteScoutFailReport = function(stream,val)

	if val.Point == nil then
		error("expect ScoutFailReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.TargetName == nil then
		error("expect ScoutFailReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect ScoutFailReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect ScoutFailReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

end



module.ReadBeScoutReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	return val
end

module.WriteBeScoutReport = function(stream,val)

	if val.Point == nil then
		error("expect BeScoutReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.TargetName == nil then
		error("expect BeScoutReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect BeScoutReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect BeScoutReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

end



module.ReadPVEReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	val.PlayerName = stream:ReadString()

	val.AllianceName = stream:ReadString()

	val.KingdomName = stream:ReadString()

	val.Result = module.ReadAttackResult(stream)

	val.Resource = module.ReadResource(stream)

	val.BattleInfo = module.ReadBattleInfo(stream)

	val.Hero = module.ReadHero(stream)

	val.CreepName = stream:ReadString()

	val.CreepLevel = stream:ReadByte()

	val.CreepRemainHp = stream:ReadFloat32()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.UnitsBefore = f()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.UnitsAfter = f()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.UnitsInjured = f()

	return val
end

module.WritePVEReport = function(stream,val)

	if val.Point == nil then
		error("expect PVEReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect PVEReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

	if val.PlayerName == nil then
		error("expect PVEReport#PlayerName")
	end
	stream:WriteString(val.PlayerName)

	if val.AllianceName == nil then
		error("expect PVEReport#AllianceName")
	end
	stream:WriteString(val.AllianceName)

	if val.KingdomName == nil then
		error("expect PVEReport#KingdomName")
	end
	stream:WriteString(val.KingdomName)

	if val.Result == nil then
		error("expect PVEReport#Result")
	end
	module.WriteAttackResult(stream,val.Result)

	if val.Resource == nil then
		error("expect PVEReport#Resource")
	end
	module.WriteResource(stream,val.Resource)

	if val.BattleInfo == nil then
		error("expect PVEReport#BattleInfo")
	end
	module.WriteBattleInfo(stream,val.BattleInfo)

	if val.Hero == nil then
		error("expect PVEReport#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.CreepName == nil then
		error("expect PVEReport#CreepName")
	end
	stream:WriteString(val.CreepName)

	if val.CreepLevel == nil then
		error("expect PVEReport#CreepLevel")
	end
	stream:WriteByte(val.CreepLevel)

	if val.CreepRemainHp == nil then
		error("expect PVEReport#CreepRemainHp")
	end
	stream:WriteFloat32(val.CreepRemainHp)

	if val.UnitsBefore == nil then
		error("expect PVEReport#UnitsBefore")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.UnitsBefore)

	if val.UnitsAfter == nil then
		error("expect PVEReport#UnitsAfter")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.UnitsAfter)

	if val.UnitsInjured == nil then
		error("expect PVEReport#UnitsInjured")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.UnitsInjured)

end



module.ReadGatherOverReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	val.Resource = module.ReadResource(stream)

	return val
end

module.WriteGatherOverReport = function(stream,val)

	if val.Point == nil then
		error("expect GatherOverReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect GatherOverReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

	if val.Resource == nil then
		error("expect GatherOverReport#Resource")
	end
	module.WriteResource(stream,val.Resource)

end



module.ReadBeTradeReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	val.Resource = module.ReadResource(stream)

	return val
end

module.WriteBeTradeReport = function(stream,val)

	if val.Point == nil then
		error("expect BeTradeReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect BeTradeReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

	if val.TargetName == nil then
		error("expect BeTradeReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect BeTradeReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect BeTradeReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

	if val.Resource == nil then
		error("expect BeTradeReport#Resource")
	end
	module.WriteResource(stream,val.Resource)

end



module.ReadReceiveGiftReport = function(stream)
	local val = {}

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	return val
end

module.WriteReceiveGiftReport = function(stream,val)

	if val.TargetName == nil then
		error("expect ReceiveGiftReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect ReceiveGiftReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect ReceiveGiftReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

end



module.ReadTradeFailReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	val.TargetName = stream:ReadString()

	val.TargetAlliance = stream:ReadString()

	val.TargetKingdom = stream:ReadString()

	return val
end

module.WriteTradeFailReport = function(stream,val)

	if val.Point == nil then
		error("expect TradeFailReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect TradeFailReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

	if val.TargetName == nil then
		error("expect TradeFailReport#TargetName")
	end
	stream:WriteString(val.TargetName)

	if val.TargetAlliance == nil then
		error("expect TradeFailReport#TargetAlliance")
	end
	stream:WriteString(val.TargetAlliance)

	if val.TargetKingdom == nil then
		error("expect TradeFailReport#TargetKingdom")
	end
	stream:WriteString(val.TargetKingdom)

end



module.ReadTradeSuccessReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	return val
end

module.WriteTradeSuccessReport = function(stream,val)

	if val.Point == nil then
		error("expect TradeSuccessReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect TradeSuccessReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

end



module.ReadBeInforcementReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.MarchType = module.ReadMarchType(stream)

	return val
end

module.WriteBeInforcementReport = function(stream,val)

	if val.Point == nil then
		error("expect BeInforcementReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.MarchType == nil then
		error("expect BeInforcementReport#MarchType")
	end
	module.WriteMarchType(stream,val.MarchType)

end



module.ReadGarrisonSuccessReport = function(stream)
	local val = {}

	val.Point =  gsaoi.ReadPoint(stream)

	val.Hero = module.ReadHero(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()

	return val
end

module.WriteGarrisonSuccessReport = function(stream,val)

	if val.Point == nil then
		error("expect GarrisonSuccessReport#Point")
	end
	gsaoi.WritePoint(stream,val.Point)

	if val.Hero == nil then
		error("expect GarrisonSuccessReport#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.Units == nil then
		error("expect GarrisonSuccessReport#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

end



module.ReadResource = function(stream)
	local val = {}

	val.Food = stream:ReadUint32()

	val.Wood = stream:ReadUint32()

	val.Ore = stream:ReadUint32()

	val.Gold = stream:ReadUint32()

	val.Granit = stream:ReadUint32()

	return val
end

module.WriteResource = function(stream,val)

	if val.Food == nil then
		error("expect Resource#Food")
	end
	stream:WriteUint32(val.Food)

	if val.Wood == nil then
		error("expect Resource#Wood")
	end
	stream:WriteUint32(val.Wood)

	if val.Ore == nil then
		error("expect Resource#Ore")
	end
	stream:WriteUint32(val.Ore)

	if val.Gold == nil then
		error("expect Resource#Gold")
	end
	stream:WriteUint32(val.Gold)

	if val.Granit == nil then
		error("expect Resource#Granit")
	end
	stream:WriteUint32(val.Granit)

end





module.ResPointType =
{

	Ancient = 6,

	Caves = 2,

	LCamps = 4,

	Mine = 3,

	Ranch = 1,

	Villages = 5,

}


module.ReadResPointType = function(stream)
	return stream:ReadByte()
end

module.WriteResPointType = function(stream,val)
	stream:WriteByte(val)
end




module.ReadResPoint = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.Type = stream:ReadByte()

	val.Level = stream:ReadByte()

	val.Yields = stream:ReadUint32()

	val.GatherSpeed = stream:ReadUint32()

	return val
end

module.WriteResPoint = function(stream,val)

	if val.OID == nil then
		error("expect ResPoint#OID")
	end
	stream:WriteString(val.OID)

	if val.Type == nil then
		error("expect ResPoint#Type")
	end
	stream:WriteByte(val.Type)

	if val.Level == nil then
		error("expect ResPoint#Level")
	end
	stream:WriteByte(val.Level)

	if val.Yields == nil then
		error("expect ResPoint#Yields")
	end
	stream:WriteUint32(val.Yields)

	if val.GatherSpeed == nil then
		error("expect ResPoint#GatherSpeed")
	end
	stream:WriteUint32(val.GatherSpeed)

end





module.Err =
{

	AOID = 30,

	ActorName = 9,

	ApplyAllianceAlready = 27,

	Auth = 2,

	BeInvited = 24,

	BuildingCanNotMove = 37,

	BuildingCanNotRemove = 36,

	BuildingInNormal = 16,

	BuildingInUpgrading = 13,

	BuildingPosConflict = 34,

	CreateMarchFailed = 6,

	HelpOnlyOnce = 38,

	HelpTimeout = 39,

	InAllianceAlready = 25,

	LevelDeficient = 40,

	MapChat = 43,

	MapServerError = 7,

	MapTarget = 8,

	MaxBookmark = 31,

	MaxBuildingLevel = 12,

	MaxMarch = 51,

	MaxTradeMarch = 50,

	MaxTrain = 44,

	NeedApplication = 21,

	NeedNotApplication = 22,

	NeedPreBuilding = 14,

	NeedUnits = 56,

	NoAlliance = 17,

	NoAuthorization = 19,

	NoEmbassy = 53,

	NoMarket = 46,

	NoMoreBuildingPosition = 35,

	NotEnoughMoney = 11,

	NotEnoughResource = 10,

	NotEnoughUnit = 3,

	NotFoundAlliance = 26,

	NotFoundApplication = 20,

	NotFoundBuilding = 15,

	NotFoundCreep = 57,

	NotFoundGarrison = 59,

	NotFoundHero = 5,

	NotFoundInvitation = 23,

	NotFoundPlayer = 28,

	NotFoundResource = 58,

	OOS = 1,

	OutOfRange = 29,

	OverHeroLimit = 4,

	OverMaxDefenceLimit = 54,

	OverTradeLimit = 47,

	OverTrainLimit = 45,

	Success = 0,

	TargetIsAllianceMember = 55,

	TargetIsNotAllianceMember = 49,

	TargetIsNotMember = 18,

	TargetPlayerOffline = 41,

	UniqueBuilding = 32,

	Unknown = 100,

	WorldChat = 42,

	WrongBuildingPos = 33,

	WrongMarchStatus = 52,

	WrongMarchType = 48,

}


module.ReadErr = function(stream)
	return stream:ReadByte()
end

module.WriteErr = function(stream,val)
	stream:WriteByte(val)
end




module.ReadHeroAttr = function(stream)
	local val = {}

	val.Name = stream:ReadString()

	val.Code = stream:ReadByte()

	val.Level = stream:ReadByte()

	val.Hp = stream:ReadUint32()

	val.Atk = stream:ReadUint32()

	val.Def = stream:ReadUint32()

	val.Exp = stream:ReadUint32()

	return val
end

module.WriteHeroAttr = function(stream,val)

	if val.Name == nil then
		error("expect HeroAttr#Name")
	end
	stream:WriteString(val.Name)

	if val.Code == nil then
		error("expect HeroAttr#Code")
	end
	stream:WriteByte(val.Code)

	if val.Level == nil then
		error("expect HeroAttr#Level")
	end
	stream:WriteByte(val.Level)

	if val.Hp == nil then
		error("expect HeroAttr#Hp")
	end
	stream:WriteUint32(val.Hp)

	if val.Atk == nil then
		error("expect HeroAttr#Atk")
	end
	stream:WriteUint32(val.Atk)

	if val.Def == nil then
		error("expect HeroAttr#Def")
	end
	stream:WriteUint32(val.Def)

	if val.Exp == nil then
		error("expect HeroAttr#Exp")
	end
	stream:WriteUint32(val.Exp)

end





module.ArmyType =
{

	Archer = 1,

	Cavalrylry = 3,

	Infantry = 4,

	Magician = 2,

}


module.ReadArmyType = function(stream)
	return stream:ReadByte()
end

module.WriteArmyType = function(stream,val)
	stream:WriteByte(val)
end




module.ReadArmy = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.Name = stream:ReadString()

	val.Type = stream:ReadUint16()

	val.Level = stream:ReadUint16()

	val.Atk = stream:ReadUint32()

	val.Def = stream:ReadUint32()

	val.Hp = stream:ReadUint32()

	val.Speed = stream:ReadFloat32()

	val.Load = stream:ReadUint32()

	val.Unkeep = stream:ReadUint32()

	val.PreBuildType = stream:ReadUint16()

	val.PreBuildLevel = stream:ReadUint16()

	val.FoodReq = stream:ReadUint32()

	val.StoneReq = stream:ReadUint32()

	val.WoodReq = stream:ReadUint32()

	val.OreReq = stream:ReadUint32()

	val.SilverReq = stream:ReadUint32()

	val.TrainTime = stream:ReadUint32()

	val.Power = stream:ReadUint32()

	val.RepairTime = stream:ReadUint32()

	val.Str_Type = stream:ReadString()

	val.Strong = stream:ReadUint16()

	val.Weak = stream:ReadUint16()

	val.ATKStar = stream:ReadUint16()

	val.DEFStar = stream:ReadUint16()

	val.HPStar = stream:ReadUint16()

	return val
end

module.WriteArmy = function(stream,val)

	if val.ID == nil then
		error("expect Army#ID")
	end
	stream:WriteString(val.ID)

	if val.Name == nil then
		error("expect Army#Name")
	end
	stream:WriteString(val.Name)

	if val.Type == nil then
		error("expect Army#Type")
	end
	stream:WriteUint16(val.Type)

	if val.Level == nil then
		error("expect Army#Level")
	end
	stream:WriteUint16(val.Level)

	if val.Atk == nil then
		error("expect Army#Atk")
	end
	stream:WriteUint32(val.Atk)

	if val.Def == nil then
		error("expect Army#Def")
	end
	stream:WriteUint32(val.Def)

	if val.Hp == nil then
		error("expect Army#Hp")
	end
	stream:WriteUint32(val.Hp)

	if val.Speed == nil then
		error("expect Army#Speed")
	end
	stream:WriteFloat32(val.Speed)

	if val.Load == nil then
		error("expect Army#Load")
	end
	stream:WriteUint32(val.Load)

	if val.Unkeep == nil then
		error("expect Army#Unkeep")
	end
	stream:WriteUint32(val.Unkeep)

	if val.PreBuildType == nil then
		error("expect Army#PreBuildType")
	end
	stream:WriteUint16(val.PreBuildType)

	if val.PreBuildLevel == nil then
		error("expect Army#PreBuildLevel")
	end
	stream:WriteUint16(val.PreBuildLevel)

	if val.FoodReq == nil then
		error("expect Army#FoodReq")
	end
	stream:WriteUint32(val.FoodReq)

	if val.StoneReq == nil then
		error("expect Army#StoneReq")
	end
	stream:WriteUint32(val.StoneReq)

	if val.WoodReq == nil then
		error("expect Army#WoodReq")
	end
	stream:WriteUint32(val.WoodReq)

	if val.OreReq == nil then
		error("expect Army#OreReq")
	end
	stream:WriteUint32(val.OreReq)

	if val.SilverReq == nil then
		error("expect Army#SilverReq")
	end
	stream:WriteUint32(val.SilverReq)

	if val.TrainTime == nil then
		error("expect Army#TrainTime")
	end
	stream:WriteUint32(val.TrainTime)

	if val.Power == nil then
		error("expect Army#Power")
	end
	stream:WriteUint32(val.Power)

	if val.RepairTime == nil then
		error("expect Army#RepairTime")
	end
	stream:WriteUint32(val.RepairTime)

	if val.Str_Type == nil then
		error("expect Army#Str_Type")
	end
	stream:WriteString(val.Str_Type)

	if val.Strong == nil then
		error("expect Army#Strong")
	end
	stream:WriteUint16(val.Strong)

	if val.Weak == nil then
		error("expect Army#Weak")
	end
	stream:WriteUint16(val.Weak)

	if val.ATKStar == nil then
		error("expect Army#ATKStar")
	end
	stream:WriteUint16(val.ATKStar)

	if val.DEFStar == nil then
		error("expect Army#DEFStar")
	end
	stream:WriteUint16(val.DEFStar)

	if val.HPStar == nil then
		error("expect Army#HPStar")
	end
	stream:WriteUint16(val.HPStar)

end



module.ReadHero = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.Code = stream:ReadByte()

	val.ArmyLimit = stream:ReadUint32()

	val.Level = stream:ReadByte()

	val.StarLevel = stream:ReadByte()

	val.Experience = stream:ReadUint32()

	val.Status = module.ReadHeroStatus(stream)

	return val
end

module.WriteHero = function(stream,val)

	if val.ID == nil then
		error("expect Hero#ID")
	end
	stream:WriteString(val.ID)

	if val.Code == nil then
		error("expect Hero#Code")
	end
	stream:WriteByte(val.Code)

	if val.ArmyLimit == nil then
		error("expect Hero#ArmyLimit")
	end
	stream:WriteUint32(val.ArmyLimit)

	if val.Level == nil then
		error("expect Hero#Level")
	end
	stream:WriteByte(val.Level)

	if val.StarLevel == nil then
		error("expect Hero#StarLevel")
	end
	stream:WriteByte(val.StarLevel)

	if val.Experience == nil then
		error("expect Hero#Experience")
	end
	stream:WriteUint32(val.Experience)

	if val.Status == nil then
		error("expect Hero#Status")
	end
	module.WriteHeroStatus(stream,val.Status)

end





module.HeroStatus =
{

	InCity = 0,

	InMarch = 1,

	InPrison = 2,

}


module.ReadHeroStatus = function(stream)
	return stream:ReadByte()
end

module.WriteHeroStatus = function(stream,val)
	stream:WriteByte(val)
end






module.MarchType =
{

	Construct = 8,

	Defence = 5,

	Garrison = 2,

	Gather = 1,

	PVE = 6,

	PVP = 3,

	Raily = 4,

	Scout = 0,

	Trade = 7,

}


module.ReadMarchType = function(stream)
	return stream:ReadByte()
end

module.WriteMarchType = function(stream,val)
	stream:WriteByte(val)
end






module.MarchStatus =
{

	Arrived = 1,

	Finish = 3,

	Goto = 0,

	Return = 2,

}


module.ReadMarchStatus = function(stream)
	return stream:ReadByte()
end

module.WriteMarchStatus = function(stream,val)
	stream:WriteByte(val)
end






module.MarchFlag =
{

	Normal = 0,

	Warn = 1,

}


module.ReadMarchFlag = function(stream)
	return stream:ReadByte()
end

module.WriteMarchFlag = function(stream,val)
	stream:WriteByte(val)
end




module.ReadMarch = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.PlayerID = stream:ReadString()

	val.TargetID = stream:ReadString()

	val.StartPos =  gsaoi.ReadPoint(stream)

	val.EndPos =  gsaoi.ReadPoint(stream)

	val.Type = module.ReadMarchType(stream)

	val.Status = module.ReadMarchStatus(stream)

	val.Hero = module.ReadHero(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()

	val.Resource = module.ReadResource(stream)

	val.StartTime = stream:ReadString()

	val.Distance = stream:ReadFloat32()

	val.Speed = stream:ReadFloat32()

	val.Extra = module.ReadMarchFlag(stream)

	return val
end

module.WriteMarch = function(stream,val)

	if val.OID == nil then
		error("expect March#OID")
	end
	stream:WriteString(val.OID)

	if val.PlayerID == nil then
		error("expect March#PlayerID")
	end
	stream:WriteString(val.PlayerID)

	if val.TargetID == nil then
		error("expect March#TargetID")
	end
	stream:WriteString(val.TargetID)

	if val.StartPos == nil then
		error("expect March#StartPos")
	end
	gsaoi.WritePoint(stream,val.StartPos)

	if val.EndPos == nil then
		error("expect March#EndPos")
	end
	gsaoi.WritePoint(stream,val.EndPos)

	if val.Type == nil then
		error("expect March#Type")
	end
	module.WriteMarchType(stream,val.Type)

	if val.Status == nil then
		error("expect March#Status")
	end
	module.WriteMarchStatus(stream,val.Status)

	if val.Hero == nil then
		error("expect March#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.Units == nil then
		error("expect March#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

	if val.Resource == nil then
		error("expect March#Resource")
	end
	module.WriteResource(stream,val.Resource)

	if val.StartTime == nil then
		error("expect March#StartTime")
	end
	stream:WriteString(val.StartTime)

	if val.Distance == nil then
		error("expect March#Distance")
	end
	stream:WriteFloat32(val.Distance)

	if val.Speed == nil then
		error("expect March#Speed")
	end
	stream:WriteFloat32(val.Speed)

	if val.Extra == nil then
		error("expect March#Extra")
	end
	module.WriteMarchFlag(stream,val.Extra)

end



module.ReadMarchTime = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.StartTime = stream:ReadString()

	val.MarchTime = stream:ReadUint32()

	return val
end

module.WriteMarchTime = function(stream,val)

	if val.OID == nil then
		error("expect MarchTime#OID")
	end
	stream:WriteString(val.OID)

	if val.StartTime == nil then
		error("expect MarchTime#StartTime")
	end
	stream:WriteString(val.StartTime)

	if val.MarchTime == nil then
		error("expect MarchTime#MarchTime")
	end
	stream:WriteUint32(val.MarchTime)

end



module.ReadTroops = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.PlayerID = stream:ReadString()

	val.Hero = module.ReadHero(stream)


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()

	val.Resource = module.ReadResource(stream)

	return val
end

module.WriteTroops = function(stream,val)

	if val.OID == nil then
		error("expect Troops#OID")
	end
	stream:WriteString(val.OID)

	if val.PlayerID == nil then
		error("expect Troops#PlayerID")
	end
	stream:WriteString(val.PlayerID)

	if val.Hero == nil then
		error("expect Troops#Hero")
	end
	module.WriteHero(stream,val.Hero)

	if val.Units == nil then
		error("expect Troops#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

	if val.Resource == nil then
		error("expect Troops#Resource")
	end
	module.WriteResource(stream,val.Resource)

end





module.AttackResult =
{

	Defeat = 2,

	Victory = 1,

	WrongTarget = 3,

}


module.ReadAttackResult = function(stream)
	return stream:ReadByte()
end

module.WriteAttackResult = function(stream,val)
	stream:WriteByte(val)
end




module.ReadUnit = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.Count = stream:ReadUint32()

	return val
end

module.WriteUnit = function(stream,val)

	if val.ID == nil then
		error("expect Unit#ID")
	end
	stream:WriteString(val.ID)

	if val.Count == nil then
		error("expect Unit#Count")
	end
	stream:WriteUint32(val.Count)

end



module.ReadTarget = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.Type = module.ReadMapObj(stream)

	return val
end

module.WriteTarget = function(stream,val)

	if val.OID == nil then
		error("expect Target#OID")
	end
	stream:WriteString(val.OID)

	if val.Type == nil then
		error("expect Target#Type")
	end
	module.WriteMapObj(stream,val.Type)

end



module.ReadViewChanged = function(stream)
	local val = {}

	val.OID = stream:ReadUint32()

	val.Pos =  gsaoi.ReadPoint(stream)

	return val
end

module.WriteViewChanged = function(stream,val)

	if val.OID == nil then
		error("expect ViewChanged#OID")
	end
	stream:WriteUint32(val.OID)

	if val.Pos == nil then
		error("expect ViewChanged#Pos")
	end
	gsaoi.WritePoint(stream,val.Pos)

end





module.MapObj =
{

	Ancient = 11,

	Castle = 1,

	Caves = 7,

	Creeps = 2,

	Garrison = 3,

	LCamps = 9,

	March = 4,

	Mine = 8,

	Monitor = 5,

	Ranch = 6,

	Space = 0,

	TerritoryDefender = 12,

	Villages = 10,

}


module.ReadMapObj = function(stream)
	return stream:ReadByte()
end

module.WriteMapObj = function(stream,val)
	stream:WriteByte(val)
end




module.ReadViewData = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.Type = module.ReadMapObj(stream)

	val.Pos =  gsaoi.ReadPoint(stream)

	return val
end

module.WriteViewData = function(stream,val)

	if val.OID == nil then
		error("expect ViewData#OID")
	end
	stream:WriteString(val.OID)

	if val.Type == nil then
		error("expect ViewData#Type")
	end
	module.WriteMapObj(stream,val.Type)

	if val.Pos == nil then
		error("expect ViewData#Pos")
	end
	gsaoi.WritePoint(stream,val.Pos)

end



module.ReadMarchBuilder = function(stream)
	local val = {}

	val.HeroID = stream:ReadString()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()

	val.Resource = module.ReadResource(stream)

	val.TargetID = stream:ReadString()

	val.EndPos =  gsaoi.ReadPoint(stream)

	val.Type = module.ReadMarchType(stream)

	return val
end

module.WriteMarchBuilder = function(stream,val)

	if val.HeroID == nil then
		error("expect MarchBuilder#HeroID")
	end
	stream:WriteString(val.HeroID)

	if val.Units == nil then
		error("expect MarchBuilder#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

	if val.Resource == nil then
		error("expect MarchBuilder#Resource")
	end
	module.WriteResource(stream,val.Resource)

	if val.TargetID == nil then
		error("expect MarchBuilder#TargetID")
	end
	stream:WriteString(val.TargetID)

	if val.EndPos == nil then
		error("expect MarchBuilder#EndPos")
	end
	gsaoi.WritePoint(stream,val.EndPos)

	if val.Type == nil then
		error("expect MarchBuilder#Type")
	end
	module.WriteMarchType(stream,val.Type)

end





module.TokenType =
{

	Money = 1,

	Resource = 2,

}


module.ReadTokenType = function(stream)
	return stream:ReadByte()
end

module.WriteTokenType = function(stream,val)
	stream:WriteByte(val)
end






module.TimerType =
{

	Building = 2,

	Technology = 3,

	Train = 1,

}


module.ReadTimerType = function(stream)
	return stream:ReadByte()
end

module.WriteTimerType = function(stream,val)
	stream:WriteByte(val)
end




module.ReadPlayerTimer = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.Type = module.ReadTimerType(stream)

	val.StartTime = stream:ReadString()

	val.TotalTime = stream:ReadUint32()

	val.Resource = module.ReadResource(stream)

	return val
end

module.WritePlayerTimer = function(stream,val)

	if val.OID == nil then
		error("expect PlayerTimer#OID")
	end
	stream:WriteString(val.OID)

	if val.Type == nil then
		error("expect PlayerTimer#Type")
	end
	module.WriteTimerType(stream,val.Type)

	if val.StartTime == nil then
		error("expect PlayerTimer#StartTime")
	end
	stream:WriteString(val.StartTime)

	if val.TotalTime == nil then
		error("expect PlayerTimer#TotalTime")
	end
	stream:WriteUint32(val.TotalTime)

	if val.Resource == nil then
		error("expect PlayerTimer#Resource")
	end
	module.WriteResource(stream,val.Resource)

end





module.CityStatus =
{

	Normal = 1,

}


module.ReadCityStatus = function(stream)
	return stream:ReadByte()
end

module.WriteCityStatus = function(stream,val)
	stream:WriteByte(val)
end






module.CityEvent =
{

	DefenseDefeat = 2,

	DefenseVictory = 1,

	None = 0,

}


module.ReadCityEvent = function(stream)
	return stream:ReadByte()
end

module.WriteCityEvent = function(stream,val)
	stream:WriteByte(val)
end




module.ReadCityStatusInfo = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.AllianceID = stream:ReadString()

	val.KingdomID = stream:ReadString()

	val.Level = stream:ReadUint16()

	val.Power = stream:ReadUint32()

	val.Status = module.ReadCityStatus(stream)

	val.Event = module.ReadCityEvent(stream)

	return val
end

module.WriteCityStatusInfo = function(stream,val)

	if val.ID == nil then
		error("expect CityStatusInfo#ID")
	end
	stream:WriteString(val.ID)

	if val.AllianceID == nil then
		error("expect CityStatusInfo#AllianceID")
	end
	stream:WriteString(val.AllianceID)

	if val.KingdomID == nil then
		error("expect CityStatusInfo#KingdomID")
	end
	stream:WriteString(val.KingdomID)

	if val.Level == nil then
		error("expect CityStatusInfo#Level")
	end
	stream:WriteUint16(val.Level)

	if val.Power == nil then
		error("expect CityStatusInfo#Power")
	end
	stream:WriteUint32(val.Power)

	if val.Status == nil then
		error("expect CityStatusInfo#Status")
	end
	module.WriteCityStatus(stream,val.Status)

	if val.Event == nil then
		error("expect CityStatusInfo#Event")
	end
	module.WriteCityEvent(stream,val.Event)

end





module.ResourceStatus =
{

	Gathering = 2,

	Normal = 1,

}


module.ReadResourceStatus = function(stream)
	return stream:ReadByte()
end

module.WriteResourceStatus = function(stream,val)
	stream:WriteByte(val)
end






module.ResourceEvent =
{

	DefenseDefeat = 2,

	DefenseVictory = 1,

	None = 0,

}


module.ReadResourceEvent = function(stream)
	return stream:ReadByte()
end

module.WriteResourceEvent = function(stream,val)
	stream:WriteByte(val)
end




module.ReadResourceStatusInfo = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.PlayerID = stream:ReadString()

	val.AllianceID = stream:ReadString()

	val.KingdomID = stream:ReadString()

	val.Level = stream:ReadByte()

	val.GatherSpeed = stream:ReadUint32()

	val.Yields = stream:ReadUint32()

	val.Status = module.ReadResourceStatus(stream)

	val.Event = module.ReadResourceEvent(stream)

	return val
end

module.WriteResourceStatusInfo = function(stream,val)

	if val.ID == nil then
		error("expect ResourceStatusInfo#ID")
	end
	stream:WriteString(val.ID)

	if val.PlayerID == nil then
		error("expect ResourceStatusInfo#PlayerID")
	end
	stream:WriteString(val.PlayerID)

	if val.AllianceID == nil then
		error("expect ResourceStatusInfo#AllianceID")
	end
	stream:WriteString(val.AllianceID)

	if val.KingdomID == nil then
		error("expect ResourceStatusInfo#KingdomID")
	end
	stream:WriteString(val.KingdomID)

	if val.Level == nil then
		error("expect ResourceStatusInfo#Level")
	end
	stream:WriteByte(val.Level)

	if val.GatherSpeed == nil then
		error("expect ResourceStatusInfo#GatherSpeed")
	end
	stream:WriteUint32(val.GatherSpeed)

	if val.Yields == nil then
		error("expect ResourceStatusInfo#Yields")
	end
	stream:WriteUint32(val.Yields)

	if val.Status == nil then
		error("expect ResourceStatusInfo#Status")
	end
	module.WriteResourceStatus(stream,val.Status)

	if val.Event == nil then
		error("expect ResourceStatusInfo#Event")
	end
	module.WriteResourceEvent(stream,val.Event)

end





module.CreepStatus =
{

	Normal = 1,

}


module.ReadCreepStatus = function(stream)
	return stream:ReadByte()
end

module.WriteCreepStatus = function(stream,val)
	stream:WriteByte(val)
end






module.CreepEvent =
{

	AttackDefeat = 2,

	AttackVictory = 1,

	None = 0,

}


module.ReadCreepEvent = function(stream)
	return stream:ReadByte()
end

module.WriteCreepEvent = function(stream,val)
	stream:WriteByte(val)
end




module.ReadCreepStatusInfo = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.Level = stream:ReadByte()

	val.HP = stream:ReadFloat32()

	val.Status = module.ReadCreepStatus(stream)

	val.Event = module.ReadCreepEvent(stream)

	return val
end

module.WriteCreepStatusInfo = function(stream,val)

	if val.ID == nil then
		error("expect CreepStatusInfo#ID")
	end
	stream:WriteString(val.ID)

	if val.Level == nil then
		error("expect CreepStatusInfo#Level")
	end
	stream:WriteByte(val.Level)

	if val.HP == nil then
		error("expect CreepStatusInfo#HP")
	end
	stream:WriteFloat32(val.HP)

	if val.Status == nil then
		error("expect CreepStatusInfo#Status")
	end
	module.WriteCreepStatus(stream,val.Status)

	if val.Event == nil then
		error("expect CreepStatusInfo#Event")
	end
	module.WriteCreepEvent(stream,val.Event)

end





module.GarrisonStatus =
{

	Normal = 1,

}


module.ReadGarrisonStatus = function(stream)
	return stream:ReadByte()
end

module.WriteGarrisonStatus = function(stream,val)
	stream:WriteByte(val)
end






module.GarrisonEvent =
{

	DefenseDefeat = 2,

	DefenseVictory = 1,

	None = 0,

}


module.ReadGarrisonEvent = function(stream)
	return stream:ReadByte()
end

module.WriteGarrisonEvent = function(stream,val)
	stream:WriteByte(val)
end




module.ReadGarrisonStatusInfo = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.PlayerID = stream:ReadString()

	val.AllianceID = stream:ReadString()

	val.KingdomID = stream:ReadString()

	val.Status = module.ReadGarrisonStatus(stream)

	val.Event = module.ReadGarrisonEvent(stream)

	return val
end

module.WriteGarrisonStatusInfo = function(stream,val)

	if val.ID == nil then
		error("expect GarrisonStatusInfo#ID")
	end
	stream:WriteString(val.ID)

	if val.PlayerID == nil then
		error("expect GarrisonStatusInfo#PlayerID")
	end
	stream:WriteString(val.PlayerID)

	if val.AllianceID == nil then
		error("expect GarrisonStatusInfo#AllianceID")
	end
	stream:WriteString(val.AllianceID)

	if val.KingdomID == nil then
		error("expect GarrisonStatusInfo#KingdomID")
	end
	stream:WriteString(val.KingdomID)

	if val.Status == nil then
		error("expect GarrisonStatusInfo#Status")
	end
	module.WriteGarrisonStatus(stream,val.Status)

	if val.Event == nil then
		error("expect GarrisonStatusInfo#Event")
	end
	module.WriteGarrisonEvent(stream,val.Event)

end




module.GW = {}

module.GW.LoginWithUDID = function(arg0,callback)
	local call = gsnet:NewCall(1)

	call.Method = 0


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)



	call:Invoke()
end






module.GS = {}

module.GS.AddBookmark = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 6


	local stream = call:NewParam()
	module.WriteBookmark(stream,arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.AddCollectionMail = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 55


	local stream = call:NewParam()
	module.WriteMail(stream,arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.AllLocation = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 59





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] =  gsaoi.ReadPoint(stream)
		end

		return val
	end
	local arg0 = f()

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.ApplyToJoinAlliance = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 21


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteString(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.BuildBuilding = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 13


	local stream = call:NewParam()
	stream:WriteByte(arg0)

	local stream = call:NewParam()
	module.WriteBuildingType(stream,arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.CancelMarch = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 9


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.CancelTrainUnits = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 52


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.CancelUpgradeBuilding = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 15


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.ChangePresident = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 29


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.CloseAllianceChat = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 44





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.CloseMapChat = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 46





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.CloseWorldChat = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 42





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.CreateAlliance = function(arg0,arg1,arg2,callback)
	local call = gsnet:NewCall(2)

	call.Method = 18


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteString(arg1)

	local stream = call:NewParam()
	stream:WriteByte(arg2)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.CreateMarch = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 8


	local stream = call:NewParam()
	module.WriteMarchBuilder(stream,arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		local stream = params[1]
		local arg1 = module.ReadMarch(stream)

		callback(nil,arg0,arg1)
	end)


	call:Invoke()
end
module.GS.CreateView = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 3


	local stream = call:NewParam()
	stream:WriteFloat32(arg0)

	local stream = call:NewParam()
	stream:WriteFloat32(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.DefenceReturn = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 11


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.DelCollectionMail = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 56


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.DelView = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 4






	call:Invoke()
end
module.GS.DeleteBookmark = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 7


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.DirectJoinAlliance = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 19


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.DismissAlliance = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 34





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.GarrisonReturn = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 10


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.GetAllApplicaitons = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 24





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		local stream = params[1]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadApplication(stream)
		end

		return val
	end
	local arg1 = f()

		callback(nil,arg0,arg1)
	end)


	call:Invoke()
end
module.GS.GetCollectionMail = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 57





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadMail(stream)
		end

		return val
	end
	local arg0 = f()

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.GetCollectionMailList = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 58





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = stream:ReadString()
		end

		return val
	end
	local arg0 = f()

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.GetHelps = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 40





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		local stream = params[1]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadHelp(stream)
		end

		return val
	end
	local arg1 = f()

		callback(nil,arg0,arg1)
	end)


	call:Invoke()
end
module.GS.HandleTheApplication = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 25


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteBool(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.InvitePlayer = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 26


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteString(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.InvitedToJoin = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 20


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.KickPlayer = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 28


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.LetDefenceReturn = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 12


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.MoveBuilding = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 17


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteByte(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.MoveCity = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 2


	local stream = call:NewParam()
	gsaoi.WritePoint(stream,arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.OnViewChanged = function(arg0,arg1)
	local call = gsnet:NewCall(2)

	call.Method = 5


	local stream = call:NewParam()
	stream:WriteFloat32(arg0)

	local stream = call:NewParam()
	stream:WriteFloat32(arg1)





	call:Invoke()
end
module.GS.OpenAllianceChat = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 43





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.OpenMapChat = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 45





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.OpenWorldChat = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 41





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.QueryCity = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 60


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		local stream = params[1]
		local arg1 = stream:ReadString()

		local stream = params[2]
		local arg2 = stream:ReadString()

		local stream = params[3]
		local arg3 = stream:ReadString()

		local stream = params[4]
		local arg4 = stream:ReadByte()

		callback(nil,arg0,arg1,arg2,arg3,arg4)
	end)


	call:Invoke()
end
module.GS.QueryCreep = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 62


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		local stream = params[1]
		local arg1 = stream:ReadByte()

		local stream = params[2]
		local arg2 = stream:ReadFloat32()

		callback(nil,arg0,arg1,arg2)
	end)


	call:Invoke()
end
module.GS.QueryGarrison = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 63


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		local stream = params[1]
		local arg1 = stream:ReadString()

		local stream = params[2]
		local arg2 = stream:ReadString()

		local stream = params[3]
		local arg3 = stream:ReadString()

		callback(nil,arg0,arg1,arg2,arg3)
	end)


	call:Invoke()
end
module.GS.QueryMarches = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 1





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadMarch(stream)
		end

		return val
	end
	local arg0 = f()

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.QueryPlayerInfo = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 0





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadPlayerInfo(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.QueryResource = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 61


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		local stream = params[1]
		local arg1 = stream:ReadByte()

		local stream = params[2]
		local arg2 = module.ReadResourceStatus(stream)

		callback(nil,arg0,arg1,arg2)
	end)


	call:Invoke()
end
module.GS.QuitAlliance = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 22





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.RemoveBuilding = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 16


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.RemoveUnits = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 53


	local stream = call:NewParam()

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.RequestHelp = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 38


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.ResponseHelp = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 39


	local stream = call:NewParam()

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			stream:WriteString(v)
		end
	end
	f(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.RevokeApplyToJoinAlliance = function(callback)
	local call = gsnet:NewCall(2)

	call.Method = 23





	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SendMail = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 54


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteString(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SendMessageAlliance = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 48


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SendMessageMap = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 49


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SendMessagePrivate = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 50


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteString(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SendMessageWorld = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 47


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SetAlias = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 33


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SetAnnouncement = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 35


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SetAuto = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 31


	local stream = call:NewParam()
	stream:WriteBool(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SetBlacklist = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 37


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteBool(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SetIntroduction = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 36


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SetLanguage = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 30


	local stream = call:NewParam()
	stream:WriteByte(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SetName = function(arg0,callback)
	local call = gsnet:NewCall(2)

	call.Method = 32


	local stream = call:NewParam()
	stream:WriteString(arg0)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.SetRank = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 27


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	stream:WriteByte(arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end
module.GS.TrainUnits = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 51


	local stream = call:NewParam()

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(arg0)

	local stream = call:NewParam()
	module.WriteTokenType(stream,arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		local stream = params[1]
		local arg1 = module.ReadPlayerTimer(stream)

		callback(nil,arg0,arg1)
	end)


	call:Invoke()
end
module.GS.UpgradeBuilding = function(arg0,arg1,callback)
	local call = gsnet:NewCall(2)

	call.Method = 14


	local stream = call:NewParam()
	stream:WriteString(arg0)

	local stream = call:NewParam()
	module.WriteTokenType(stream,arg1)




	call.Return:Add(function(e,params)
		if e ~= nil then
			callback(e:ToString())
			return
		end

		local stream = params[0]
		local arg0 = module.ReadErr(stream)

		callback(nil,arg0)
	end)


	call:Invoke()
end





module.GC = {}

module.GC.Call = function(call)

	Debug_Log(call.Length)

	for i = 0,call.Length - 1 do
		module.GC.Call2(call[i])
	end
end

module.GC.Call2 = function(call)

	local params = call:Parameters()

	if call.Method == 2 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadHero(stream)
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.HeroesChanged(arg0)



		return
	end

	if call.Method == 1 then

		local stream = params[0]
		local arg0 = stream:ReadUint32()


		module.GC.metatable.MoneyChanged(arg0)



		return
	end

	if call.Method == 4 then

		local stream = params[0]
		local arg0 = module.ReadMarch(stream)


		module.GC.metatable.OnMarchChanged(arg0)



		return
	end

	if call.Method == 6 then

		local stream = params[0]
		local arg0 = stream:ReadString()

		local stream = params[1]
		local arg1 = module.ReadMarchFlag(stream)


		module.GC.metatable.OnMarchFlagChanged(arg0,arg1)



		return
	end

	if call.Method == 5 then

		local stream = params[0]
		local arg0 = stream:ReadString()

		local stream = params[1]
		local arg1 = module.ReadTarget(stream)


		module.GC.metatable.OnMarchTargetChanged(arg0,arg1)



		return
	end

	if call.Method == 9 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadViewData(stream)
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.OnViewDataEnter(arg0)



		return
	end

	if call.Method == 11 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = stream:ReadString()
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.OnViewDataLeave(arg0)



		return
	end

	if call.Method == 10 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadViewData(stream)
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.OnViewDataMoved(arg0)



		return
	end

	if call.Method == 7 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadMarch(stream)
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.OnViewMarchEnter(arg0)



		return
	end

	if call.Method == 8 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = stream:ReadString()
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.OnViewMarchLeave(arg0)



		return
	end

	if call.Method == 18 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadMail(stream)
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.RecvMail(arg0)


		local stream = call:NewReturn()
		module.WriteErr(stream,arg0)


		call:Invoke()

		return
	end

	if call.Method == 14 then

		local stream = params[0]
		local arg0 = stream:ReadString()

		local stream = params[1]
		local arg1 = stream:ReadString()


		module.GC.metatable.RecvMessageAlliance(arg0,arg1)



		return
	end

	if call.Method == 12 then

		local stream = params[0]
		local arg0 = stream:ReadString()

		local stream = params[1]
		local arg1 = stream:ReadString()


		module.GC.metatable.RecvMessageMap(arg0,arg1)



		return
	end

	if call.Method == 15 then

		local stream = params[0]
		local arg0 = stream:ReadString()

		local stream = params[1]
		local arg1 = stream:ReadString()


		module.GC.metatable.RecvMessagePrivate(arg0,arg1)



		return
	end

	if call.Method == 13 then

		local stream = params[0]
		local arg0 = stream:ReadString()

		local stream = params[1]
		local arg1 = stream:ReadString()


		module.GC.metatable.RecvMessageWorld(arg0,arg1)



		return
	end

	if call.Method == 17 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadNotice(stream)
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.RecvNotice(arg0)


		local stream = call:NewReturn()
		module.WriteErr(stream,arg0)


		call:Invoke()

		return
	end

	if call.Method == 16 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadReport(stream)
		end

		return val
	end
	local arg0 = f()


	module.GC.metatable.RecvReport(arg0)


		local stream = call:NewReturn()
		module.WriteErr(stream,arg0)


		call:Invoke()

		return
	end

	if call.Method == 0 then

		local stream = params[0]
		local arg0 = module.ReadResource(stream)


		module.GC.metatable.ResourceChanged(arg0)



		return
	end

	if call.Method == 19 then

		local stream = params[0]
		local arg0 = module.ReadCityStatusInfo(stream)


		module.GC.metatable.SetCityStatusInfo(arg0)



		return
	end

	if call.Method == 21 then

		local stream = params[0]
		local arg0 = module.ReadCreepStatusInfo(stream)


		module.GC.metatable.SetCreepStatusInfo(arg0)



		return
	end

	if call.Method == 22 then

		local stream = params[0]
		local arg0 = module.ReadGarrisonStatusInfo(stream)


		module.GC.metatable.SetGarrisonStatusInfo(arg0)



		return
	end

	if call.Method == 20 then

		local stream = params[0]
		local arg0 = module.ReadResourceStatusInfo(stream)


		module.GC.metatable.SetResourceStatusInfo(arg0)



		return
	end

	if call.Method == 3 then

		local stream = params[0]

	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	local arg0 = f()


		module.GC.metatable.UnitsChanged(arg0)



		return
	end

end




module.ReadCreepInfo = function(stream)
	local val = {}

	val.CreepID = stream:ReadUint32()

	val.Pos =  gsaoi.ReadPoint(stream)

	val.Name = stream:ReadString()

	val.CreepType = stream:ReadByte()

	val.Level = stream:ReadByte()

	return val
end

module.WriteCreepInfo = function(stream,val)

	if val.CreepID == nil then
		error("expect CreepInfo#CreepID")
	end
	stream:WriteUint32(val.CreepID)

	if val.Pos == nil then
		error("expect CreepInfo#Pos")
	end
	gsaoi.WritePoint(stream,val.Pos)

	if val.Name == nil then
		error("expect CreepInfo#Name")
	end
	stream:WriteString(val.Name)

	if val.CreepType == nil then
		error("expect CreepInfo#CreepType")
	end
	stream:WriteByte(val.CreepType)

	if val.Level == nil then
		error("expect CreepInfo#Level")
	end
	stream:WriteByte(val.Level)

end



module.ReadCreepAttr = function(stream)
	local val = {}

	val.Id = stream:ReadUint16()

	val.Name = stream:ReadString()

	val.CreepType = stream:ReadByte()

	val.Level = stream:ReadByte()

	val.Hp = stream:ReadUint32()

	val.Atk = stream:ReadUint32()

	val.Def = stream:ReadUint32()

	val.AddTpye = stream:ReadByte()

	val.AddPro = stream:ReadByte()

	val.CutType = stream:ReadByte()

	val.CutPro = stream:ReadByte()

	val.Power = stream:ReadUint32()

	val.AtkStar = stream:ReadByte()

	val.DefStar = stream:ReadByte()

	val.CreepRank = stream:ReadUint16()

	val.Exp = stream:ReadUint16()

	val.Food = stream:ReadUint32()

	val.Wood = stream:ReadUint32()

	val.Stone = stream:ReadUint32()

	val.Ore = stream:ReadUint32()

	val.Silver = stream:ReadUint32()

	return val
end

module.WriteCreepAttr = function(stream,val)

	if val.Id == nil then
		error("expect CreepAttr#Id")
	end
	stream:WriteUint16(val.Id)

	if val.Name == nil then
		error("expect CreepAttr#Name")
	end
	stream:WriteString(val.Name)

	if val.CreepType == nil then
		error("expect CreepAttr#CreepType")
	end
	stream:WriteByte(val.CreepType)

	if val.Level == nil then
		error("expect CreepAttr#Level")
	end
	stream:WriteByte(val.Level)

	if val.Hp == nil then
		error("expect CreepAttr#Hp")
	end
	stream:WriteUint32(val.Hp)

	if val.Atk == nil then
		error("expect CreepAttr#Atk")
	end
	stream:WriteUint32(val.Atk)

	if val.Def == nil then
		error("expect CreepAttr#Def")
	end
	stream:WriteUint32(val.Def)

	if val.AddTpye == nil then
		error("expect CreepAttr#AddTpye")
	end
	stream:WriteByte(val.AddTpye)

	if val.AddPro == nil then
		error("expect CreepAttr#AddPro")
	end
	stream:WriteByte(val.AddPro)

	if val.CutType == nil then
		error("expect CreepAttr#CutType")
	end
	stream:WriteByte(val.CutType)

	if val.CutPro == nil then
		error("expect CreepAttr#CutPro")
	end
	stream:WriteByte(val.CutPro)

	if val.Power == nil then
		error("expect CreepAttr#Power")
	end
	stream:WriteUint32(val.Power)

	if val.AtkStar == nil then
		error("expect CreepAttr#AtkStar")
	end
	stream:WriteByte(val.AtkStar)

	if val.DefStar == nil then
		error("expect CreepAttr#DefStar")
	end
	stream:WriteByte(val.DefStar)

	if val.CreepRank == nil then
		error("expect CreepAttr#CreepRank")
	end
	stream:WriteUint16(val.CreepRank)

	if val.Exp == nil then
		error("expect CreepAttr#Exp")
	end
	stream:WriteUint16(val.Exp)

	if val.Food == nil then
		error("expect CreepAttr#Food")
	end
	stream:WriteUint32(val.Food)

	if val.Wood == nil then
		error("expect CreepAttr#Wood")
	end
	stream:WriteUint32(val.Wood)

	if val.Stone == nil then
		error("expect CreepAttr#Stone")
	end
	stream:WriteUint32(val.Stone)

	if val.Ore == nil then
		error("expect CreepAttr#Ore")
	end
	stream:WriteUint32(val.Ore)

	if val.Silver == nil then
		error("expect CreepAttr#Silver")
	end
	stream:WriteUint32(val.Silver)

end



module.ReadCreepDrop = function(stream)
	local val = {}

	val.CreepRank = stream:ReadUint16()

	val.ItemID = stream:ReadUint32()

	val.KingdomDrop = stream:ReadUint16()

	val.DropPro = stream:ReadUint16()

	val.DropNum = stream:ReadUint32()

	val.MaxDropKind = stream:ReadUint16()

	return val
end

module.WriteCreepDrop = function(stream,val)

	if val.CreepRank == nil then
		error("expect CreepDrop#CreepRank")
	end
	stream:WriteUint16(val.CreepRank)

	if val.ItemID == nil then
		error("expect CreepDrop#ItemID")
	end
	stream:WriteUint32(val.ItemID)

	if val.KingdomDrop == nil then
		error("expect CreepDrop#KingdomDrop")
	end
	stream:WriteUint16(val.KingdomDrop)

	if val.DropPro == nil then
		error("expect CreepDrop#DropPro")
	end
	stream:WriteUint16(val.DropPro)

	if val.DropNum == nil then
		error("expect CreepDrop#DropNum")
	end
	stream:WriteUint32(val.DropNum)

	if val.MaxDropKind == nil then
		error("expect CreepDrop#MaxDropKind")
	end
	stream:WriteUint16(val.MaxDropKind)

end



module.ReadItem = function(stream)
	local val = {}

	val.ItemID = stream:ReadUint32()

	val.ItemName = stream:ReadString()

	val.ItemType = stream:ReadUint16()

	val.EffectCode = stream:ReadUint32()

	val.EffectVal = stream:ReadUint32()

	val.EffectTime = stream:ReadUint32()

	val.PileUpNum = stream:ReadUint16()

	val.StoragePlace = stream:ReadUint16()

	return val
end

module.WriteItem = function(stream,val)

	if val.ItemID == nil then
		error("expect Item#ItemID")
	end
	stream:WriteUint32(val.ItemID)

	if val.ItemName == nil then
		error("expect Item#ItemName")
	end
	stream:WriteString(val.ItemName)

	if val.ItemType == nil then
		error("expect Item#ItemType")
	end
	stream:WriteUint16(val.ItemType)

	if val.EffectCode == nil then
		error("expect Item#EffectCode")
	end
	stream:WriteUint32(val.EffectCode)

	if val.EffectVal == nil then
		error("expect Item#EffectVal")
	end
	stream:WriteUint32(val.EffectVal)

	if val.EffectTime == nil then
		error("expect Item#EffectTime")
	end
	stream:WriteUint32(val.EffectTime)

	if val.PileUpNum == nil then
		error("expect Item#PileUpNum")
	end
	stream:WriteUint16(val.PileUpNum)

	if val.StoragePlace == nil then
		error("expect Item#StoragePlace")
	end
	stream:WriteUint16(val.StoragePlace)

end



module.ReadBookmark = function(stream)
	local val = {}

	val.ID = stream:ReadString()

	val.Name = stream:ReadString()

	val.Pos =  gsaoi.ReadPoint(stream)

	val.Type = stream:ReadInt32()

	return val
end

module.WriteBookmark = function(stream,val)

	if val.ID == nil then
		error("expect Bookmark#ID")
	end
	stream:WriteString(val.ID)

	if val.Name == nil then
		error("expect Bookmark#Name")
	end
	stream:WriteString(val.Name)

	if val.Pos == nil then
		error("expect Bookmark#Pos")
	end
	gsaoi.WritePoint(stream,val.Pos)

	if val.Type == nil then
		error("expect Bookmark#Type")
	end
	stream:WriteInt32(val.Type)

end



module.ReadPlayerInfo = function(stream)
	local val = {}

	val.PlayerID = stream:ReadString()

	val.PlayerName = stream:ReadString()

	val.AllianceID = stream:ReadString()

	val.KingdomID = stream:ReadString()

	val.ServerName = stream:ReadString()

	val.Level = stream:ReadUint16()

	val.Power = stream:ReadUint32()

	val.CityLocation =  gsaoi.ReadPoint(stream)

	val.Resource = module.ReadResource(stream)

	val.Money = stream:ReadUint32()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadHero(stream)
		end

		return val
	end
	val.Heroes = f()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadBookmark(stream)
		end

		return val
	end
	val.Bookmarks = f()

	return val
end

module.WritePlayerInfo = function(stream,val)

	if val.PlayerID == nil then
		error("expect PlayerInfo#PlayerID")
	end
	stream:WriteString(val.PlayerID)

	if val.PlayerName == nil then
		error("expect PlayerInfo#PlayerName")
	end
	stream:WriteString(val.PlayerName)

	if val.AllianceID == nil then
		error("expect PlayerInfo#AllianceID")
	end
	stream:WriteString(val.AllianceID)

	if val.KingdomID == nil then
		error("expect PlayerInfo#KingdomID")
	end
	stream:WriteString(val.KingdomID)

	if val.ServerName == nil then
		error("expect PlayerInfo#ServerName")
	end
	stream:WriteString(val.ServerName)

	if val.Level == nil then
		error("expect PlayerInfo#Level")
	end
	stream:WriteUint16(val.Level)

	if val.Power == nil then
		error("expect PlayerInfo#Power")
	end
	stream:WriteUint32(val.Power)

	if val.CityLocation == nil then
		error("expect PlayerInfo#CityLocation")
	end
	gsaoi.WritePoint(stream,val.CityLocation)

	if val.Resource == nil then
		error("expect PlayerInfo#Resource")
	end
	module.WriteResource(stream,val.Resource)

	if val.Money == nil then
		error("expect PlayerInfo#Money")
	end
	stream:WriteUint32(val.Money)

	if val.Units == nil then
		error("expect PlayerInfo#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

	if val.Heroes == nil then
		error("expect PlayerInfo#Heroes")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteHero(stream,v)
		end
	end
	f(val.Heroes)

	if val.Bookmarks == nil then
		error("expect PlayerInfo#Bookmarks")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteBookmark(stream,v)
		end
	end
	f(val.Bookmarks)

end



module.ReadPlayerBrief = function(stream)
	local val = {}

	val.OID = stream:ReadString()

	val.PlayerName = stream:ReadString()

	val.ServerName = stream:ReadString()

	val.Level = stream:ReadUint16()

	val.Power = stream:ReadUint32()

	val.CityLocation =  gsaoi.ReadPoint(stream)

	val.Resource = module.ReadResource(stream)

	val.Money = stream:ReadUint32()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadUnit(stream)
		end

		return val
	end
	val.Units = f()


	local f = function()
		local val = {}
		local length = stream:ReadUint16()
		for i=1,length do
			val[i] = module.ReadHero(stream)
		end

		return val
	end
	val.Heroes = f()

	return val
end

module.WritePlayerBrief = function(stream,val)

	if val.OID == nil then
		error("expect PlayerBrief#OID")
	end
	stream:WriteString(val.OID)

	if val.PlayerName == nil then
		error("expect PlayerBrief#PlayerName")
	end
	stream:WriteString(val.PlayerName)

	if val.ServerName == nil then
		error("expect PlayerBrief#ServerName")
	end
	stream:WriteString(val.ServerName)

	if val.Level == nil then
		error("expect PlayerBrief#Level")
	end
	stream:WriteUint16(val.Level)

	if val.Power == nil then
		error("expect PlayerBrief#Power")
	end
	stream:WriteUint32(val.Power)

	if val.CityLocation == nil then
		error("expect PlayerBrief#CityLocation")
	end
	gsaoi.WritePoint(stream,val.CityLocation)

	if val.Resource == nil then
		error("expect PlayerBrief#Resource")
	end
	module.WriteResource(stream,val.Resource)

	if val.Money == nil then
		error("expect PlayerBrief#Money")
	end
	stream:WriteUint32(val.Money)

	if val.Units == nil then
		error("expect PlayerBrief#Units")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteUnit(stream,v)
		end
	end
	f(val.Units)

	if val.Heroes == nil then
		error("expect PlayerBrief#Heroes")
	end

	local f = function(arg)
		stream:WriteUint16(#arg)
		for i,v in ipairs(arg) do
			module.WriteHero(stream,v)
		end
	end
	f(val.Heroes)

end
return module
