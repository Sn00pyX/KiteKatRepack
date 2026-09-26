--[[
-*-All Rights Reserved @ Revolution Team 2018 
-*-This script is private script for RevolutionTeam use Only .
https://www.rappelz.ae
]]--

--ÃáÊÇÌÑ ãÇäÏí--
function NPC_RevoTrader_Contact()

local level = tonumber(gv("lv"))

dlg_title( "@99000001" )
dlg_text( "@99000002")
dlg_menu( "@99000003", "Revo_NessasryItems()" )
-- dlg_menu( "@99000004", "Revo_NessasryFights()" )
dlg_menu( "@99000005", "Revo_NessasryZinas()" )
dlg_menu( "@99000006", "Revo_NessasryCreatures()" )
dlg_menu( "@99000007", "Revo_NessasryStones()" )
dlg_menu( "@99000044", "open_market('Revo_Market_Arena')" )

if level > 150 then
	dlg_menu( "@99000055", "Revo_Buffes(0)" )
else
	dlg_menu( "@99000056", "Revo_Buffes(0)" )
end


dlg_menu( "@90010002", "" )
dlg_show()
end

function Revo_NessasryItems()
dlg_title( "@99000001" )
dlg_text( "@99000011")
dlg_menu( "@99000012", "open_market('Revo_Market_Materials')" )
dlg_menu( "@99000013", "open_market('Revo_Market_Development')" )
dlg_menu( "@99000014", "open_market('Revo_Market_Support')" )
dlg_menu( "@99000015", "open_market('Revo_Market_Other')" )
dlg_menu( "@99000016", "NPC_RevoTrader_Contact()" )
dlg_menu( "@90010002", "" )
dlg_show()
end

function Revo_NessasryZinas()
dlg_title( "@99000001" )
dlg_text( "@99000034")
dlg_menu( "@99000028", "open_market('Revo_Market_SkinZinas')" )
dlg_menu( "@99000029", "open_market('Revo_Market_HairZinas')" )
dlg_menu( "@99000030", "open_market('Revo_Market_NewMaskZinas')" )
--dlg_menu( "@99000031", "open_market( 'Revo_Market_FullSetsZinas' )" )
dlg_menu( "@99000032", "open_market('Revo_Market_WeaponsZinas')" )
dlg_menu( "@99000033", "open_market('Revo_Market_WingsZinas')" )
dlg_menu( "@99000016", "NPC_RevoTrader_Contact()" )
dlg_menu( "@90010002", "" )
dlg_show()
end

function Revo_NessasryCreatures()
dlg_title( "@99000001" )
dlg_text( "@99000035")
-- dlg_menu( "@99000036", "open_market('Revo_Market_CreaturesCards')" )
dlg_menu( "@99000037", "open_market('Revo_Market_SetsPets')" )
dlg_menu( "@99000038", "open_market('Revo_Market_AntiDotesPets')" )
dlg_menu( "@99000039", "open_market('Revo_Market_PetsSolutions')" )
dlg_menu( "@99000040", "open_market('Revo_Market_Pets')" )
dlg_menu( "@99000045", "open_market('Revo_Market_Creature_Spirits')" )
dlg_menu( "@99000016", "NPC_RevoTrader_Contact()" )
dlg_menu( "@90010002", "" )
dlg_show()
end

function Revo_NessasryStones()
dlg_title( "@99000001" )
dlg_text( "@99000041")
dlg_menu( "@99000042", "open_market('Revo_Market_LegendesStones')" )
dlg_menu( "@99000043", "open_market('Revo_Market_ProStones')" )
dlg_menu( "@99000016", "NPC_RevoTrader_Contact()" )
dlg_menu( "@90010002", "" )
dlg_show()
end




function open_Revo_market(market) --in case players should pay to open shops
local gold = tonumber(gv("gold"))
local level = tonumber(gv("lv"))
local price = level * 100
if gold >= price then
open_market(market)
sv("gold", gold - price )
else
message(sconv("@99000052", "#@price@#",tostring(price))) 
end
end
--äåÇíÉ ÇáÊÇÌÑ ãÇäÏí--


--ÇáÇÖÇİÇÊ ÇáİæÑíÉ ááÊÇÌÑ--

function on_testserver_case(case)
if case == 1 then --Beta
set_env("game.TestServer",0)
notice("@99000162")
elseif case == 0 then --Main
set_env("game.TestServer",0)
notice("@99000163")
elseif case == 2 then --Test
set_env("game.TestServer",1)
notice("@99000164")
set_env("gmzone",1)
end 
end 

function Revo_Buffes(name)

	local price = 100 * 1000000
	local gold = tonumber(gv("gold"))
	local level = tonumber(gv("lv"))
	
	if gold >= price or level <= 150 then
		if name == 0 then
			name = gv("name")
		else
			name = name
		end
		add_state(314016, 145, 180000, name)
		add_state(163404, 78, 180000, name)
		add_state(163405, 78, 180000, name)
		add_state(163406, 78, 180000, name)
		add_state(163407, 78, 180000, name)
		add_state(2505, 263, 180000, name)
		add_state(2507, 263, 180000, name)
		add_state(163433, 140, 180000, name)
		add_state(13423, 54, 180000, name)
		add_state(13424, 436, 180000, name)
		add_state(13425, 436, 180000, name)
		add_state(163448, 59, 180000, name)
		add_state(163449, 59, 180000, name)
		add_state(314099, 145, 180000, name)
		add_state(314049, 262, 180000, name)
		add_state(314017, 250, 180000, name)
		add_state(163429, 17, 180000, name)
		add_state(2506, 63, 180000, name)
		add_state(2508, 63, 180000, name)
		add_state(314018, 100, 180000, name)
		
		if level > 150 then
			set_value( "gold", gold - price )
			cprint( sconv("@263", "#@gold@#", tostring(price))) 
		else
			cprint( "@99000046" ) 
		end
	else
		message("@283")
	end
end

--[[

function Revo_Super_States()
local state_level_1 = get_state_level( 1055 )
local state_level_2 = get_state_level( 1051 )
local state_level_3 = get_state_level( 1053 )
local state_level_4 = get_state_level( 1052 )
local state_level_5 = get_state_level( 1054 )
local state_level_6 = get_state_level( 1056 )
local state_level_7 = get_state_level( 1057 )
	
if gv("gold") >= 20000000 then

if 	state_level_1 == 0 then
add_state( 1055, 6, 540000 )
end 

if state_level_2 == 0 then
add_state( 1051, 6, 540000 )
end 

if state_level_3 == 0 then
add_state( 1053, 6, 540000 )
end 

if state_level_4 == 0 then
add_state( 1052, 6, 540000 )
end 

if state_level_5 == 0 then
add_state( 1054, 6, 540000 )
end 

if state_level_6 == 0 then
add_state( 1056, 6, 540000 )
end 

if state_level_7 == 0 then
add_state( 1057, 6, 540000 )
end 

sv("gold",gv("gold")-20000000)
end
end  

]]--