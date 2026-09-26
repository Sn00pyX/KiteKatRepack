


-- Strings used: From 100046001 - to 100110000

function set_debug( var )
local flag = tonumber(get_flag("dbgm")) or 0
	if var == nil or var == '' then
		if flag == 0 then
			set_flag("dbgm", 1)
		else
			set_flag("dbgm", 0)
		end
	else
		set_flag("dbgm", flag)
	end
	
	
end



function debug_notice( noticeString )
local flag = tonumber(get_flag("dbgm")) or 0
	if flag == 1 then
		private_notice("<#272AF5>[Debug]: " .. noticeString)
	end
end


function is_in_form( location )
local pointsArray = {}

	if location == "star" then
		pointsArray =
		{
			{ x = 151898, y = 73295 },
			{ x = 152013, y = 73158 },
			{ x = 152160, y = 73242 },
			{ x = 152213, y = 73059 },
			{ x = 152397, y = 73106 },
			{ x = 152349, y = 72906 },
			{ x = 152554, y = 72864 },
			{ x = 152449, y = 72717 },
			{ x = 152454, y = 72465 },
			{ x = 152538, y = 72345 },
			{ x = 152407, y = 72308 },
			{ x = 152365, y = 72245 },
			{ x = 152397, y = 72119 },
			{ x = 152255, y = 72135 },
			{ x = 152192, y = 72087 },
			{ x = 152166, y = 71961 },
			{ x = 152045, y = 72024 },
			{ x = 151992, y = 72009 },
			{ x = 151914, y = 71909 },
			{ x = 151819, y = 72019 },
			{ x = 151767, y = 72024 },
			{ x = 151641, y = 71956 },
			{ x = 151614, y = 72082 },
			{ x = 151551, y = 72114 },
			{ x = 151425, y = 72103 },
			{ x = 151425, y = 72266 },
			{ x = 151268, y = 72324 },
			{ x = 151357, y = 72471 },
			{ x = 151347, y = 72691 },
			{ x = 151263, y = 72859 },
			{ x = 151410, y = 72875 },
			{ x = 151431, y = 72912 },
			{ x = 151410, y = 73059 },
			{ x = 151520, y = 73043 },
			{ x = 151588, y = 73080 },
			{ x = 151635, y = 73200 },
			{ x = 151756, y = 73132 },
			{ x = 151819, y = 73174 },
			{ x = 151819, y = 73174 }
		}
	
	elseif location == "hori_scarecrows" then
		pointsArray =
		{
			{ x = 154596, y = 76419 },
			{ x = 154486, y = 76371 },
			{ x = 154339, y = 76387 },
			{ x = 154213, y = 76434 },
			{ x = 154134, y = 76513 },
			{ x = 154082, y = 76581 },
			{ x = 154040, y = 76676 },
			{ x = 154029, y = 76849 },
			{ x = 154098, y = 77007 },
			{ x = 154213, y = 77117 },
			{ x = 154350, y = 77159 },
			{ x = 154481, y = 77164 },
			{ x = 154633, y = 77091 },
			{ x = 154764, y = 76975 },
			{ x = 154806, y = 76875 },
			{ x = 154817, y = 76686 },
			{ x = 154770, y = 76566 }
		}
	
	end


local isInside = PolygonsCheck(pointsArray, gv("x"), gv("y"))

	debug_notice( "[" .. location .. "]: Your X: " .. gv('x') .. "; Y: " .. gv('y') )
	if isInside then
		debug_notice("Inside of form")
	else
		debug_notice("Outside")
	end
end

function PolygonsCheck(polygon, px, py)
    local inside = false
    local j = #polygon

    for i = 1, #polygon do
        local xi, yi = polygon[i].x, polygon[i].y
        local xj, yj = polygon[j].x, polygon[j].y

        local intersects = ((yi > py) ~= (yj > py)) and (px < (xj - xi) * (py - yi) / ((yj - yi) + 0.000001) + xi)

        if intersects then
            inside = not inside
        end

        j = i
    end

    return inside
end

function levelupmypets(form_slot)
if form_slot == nil or form_slot == '' then form_slot = 0 end



	for i = 1, 180 do
	
	local creature_handle	= get_creature_handle( form_slot )
	local curr_level		= get_creature_value( creature_handle, "lv" )
	local curr_evo			= get_creature_value( creature_handle, "evolution_depth" )
	local id				= get_creature_value( creature_handle, "job" )
	
		if curr_level ~= 60 and curr_level ~= 115 then
			set_creature_value(creature_handle, "lv", ( curr_level + 1 ) )
			
		elseif curr_level == 60 and curr_evo == 1 then
			set_creature_value(creature_handle, "ev_1_ID", id)
			set_creature_value(creature_handle, "ev_1_level", curr_level)
			creature_evolution(creature_handle)
			set_creature_value(creature_handle, "lv", ( curr_level + 1 ) )
			
		elseif curr_level == 115 and curr_evo == 2 then
			set_creature_value(creature_handle, "ev_2_ID", id)
			set_creature_value(creature_handle, "ev_2_level", curr_level)
			creature_evolution(creature_handle)
			set_creature_value(creature_handle, "lv", ( curr_level + 1 ) )
			
		end
	end

end


local function is_leap(year)
    return (year % 4 == 0 and year % 100 ~= 0) or (year % 400 == 0)
end

local function epoch_to_date(ts)
    local seconds_per_day = 86400
    local days = math.floor(ts / seconds_per_day)

    local year = 1970
    while true do
        local days_in_year = is_leap(year) and 366 or 365
        if days >= days_in_year then
            days = days - days_in_year
            year = year + 1
        else
            break
        end
    end

    local month_lengths =
	{
        31,	-- January
		is_leap(year) and 29 or 28,	-- February
        31,	-- March
		30, -- April
		31, -- May
		30, -- June
        31, -- July
		31, -- August
		30, -- September
		31, -- November
		30, -- December
		31	-- January
    }

    local month = 1
    while days >= month_lengths[month] do
        days = days - month_lengths[month]
        month = month + 1
    end

    local day = days + 1

    return sconv("#@day@# / #@month@# / #@year@#", "#@day@#", day, "#@month@#", month, "#@year@#", year)
end


function server_variables()
local process_start = get_env("process.start")
local uptime = get_env("process.uptime")
local process_load = get_env("process.load")
local process_memory = get_env("process.memory")
local paged_memory = get_env("process.paged_memory")

local curr_online = get_env("game.user_count")
local exp_ratio = get_env("game.exp_rate")
local chaos_drop_ratio = get_env("game.chaos_drop_rate")
local gold_drop_ratio = get_env("game.gold_drop_rate")
local item_drop_ratio = get_env("game.item_drop_rate")
local party_rate_0 = get_env("game.party_exp_rate_0")
local party_rate_1 = get_env("game.party_exp_rate_1")
local party_rate_2 = get_env("game.party_exp_rate_2")
local party_rate_3 = get_env("game.party_exp_rate_3")
local party_rate_4 = get_env("game.party_exp_rate_4")
local party_rate_5 = get_env("game.party_exp_rate_5")
local party_rate_6 = get_env("game.party_exp_rate_6")
local party_rate_level = get_env("game.party_exp_penalty_level")

dlg_general("<br><br>Server started at: " .. process_start .. "<br>Server uptime is: " .. uptime .. ".<br>CPU load: " .. process_load .. "%<br>Process RAM Usage: " .. process_memory .. "<br>Process paged memory: " .. paged_memory .. "<br><br><br>Current online is: " .. curr_online .. " players.<br>EXP rate: " .. exp_ratio .. "<br>Chaos rate: " .. chaos_drop_ratio .. "<br>Gold drop rate: " .. gold_drop_ratio .. "<br>Item drop rate: " .. item_drop_ratio .. "<br>Parte rate 0: " .. party_rate_0 .. "<br>Parte rate 1: " .. party_rate_1 .. "<br>Parte rate 2: " .. party_rate_2 .. "<br>Parte rate 3: " .. party_rate_3 .. "<br>Parte rate 4: " .. party_rate_4 .. "<br>Parte rate 5: " .. party_rate_5 .. "<br>Parte rate 6: " .. party_rate_6 .. "<br>Party rate level: " .. party_rate_level .. "<br><br>")
end

function npcid()
	debug_notice("NPC ID: " .. get_npc_id())
end

function get_character_info( characterName )
if characterName == nil or characterName == '' then characterName = gv("name") end
	if gv("login", characterName) ~= 1 then
		message("<b>" .. characterName .. "</b> is currently offline")
		return
	end

local SlotsTable = {}
SlotsTable[0]	= "@6700"		--	Weapon
SlotsTable[1]	= "@6702"		--	Shield
SlotsTable[2]	= "@6701"		--	Armor
SlotsTable[3]	= "@6703"		--	Helmet
SlotsTable[4]	= "@6704"		--	Gloves
SlotsTable[5]	= "@6705"		--	Boots
SlotsTable[6]	= "@6706"		--	Belt
SlotsTable[7]	= "@6707"		--	Cloak
SlotsTable[8]	= "@6820"		--	Necklace
SlotsTable[9]	= "@6818"		--	Ring [1]
SlotsTable[10]	= "@6818"		--	Ring [2]
SlotsTable[11]	= "@6819"		--	Earring
SlotsTable[12]	= "@7193"		--	Mask
SlotsTable[14]	= "@7542"		--	Decorative Weapon
SlotsTable[15]	= "@7543"		--	Decorative Shield
SlotsTable[16]	= "@7544"		--	Decorative Costume
SlotsTable[17]	= "@7545"		--	Decorative Helmet
SlotsTable[18]	= "@7546"		--	Decorative Glove
SlotsTable[19]	= "@7547"		--	Decorative Boots
SlotsTable[20]	= "@7548"		--	Decorative Cloak
SlotsTable[21]	= "@7549"		--	Decorative Shoulder Piece (wings)
SlotsTable[22]	= "@9702"		--	Mount<7.2>
SlotsTable[23]	= "@6711"		--	Bag


--job_classification_table[job_id][2]
local equippedItemsList = {}
equippedItemsList[0]	= "Nothing"
equippedItemsList[1]	= "Nothing"
equippedItemsList[2]	= "Nothing"
equippedItemsList[3]	= "Nothing"
equippedItemsList[4]	= "Nothing"
equippedItemsList[5]	= "Nothing"
equippedItemsList[6]	= "Nothing"
equippedItemsList[7]	= "Nothing"
equippedItemsList[8]	= "Nothing"
equippedItemsList[9]	= "Nothing"
equippedItemsList[10]	= "Nothing"
equippedItemsList[11]	= "Nothing"
equippedItemsList[12]	= "Nothing"
equippedItemsList[14]	= "Nothing"
equippedItemsList[15]	= "Nothing"
equippedItemsList[16]	= "Nothing"
equippedItemsList[17]	= "Nothing"
equippedItemsList[18]	= "Nothing"
equippedItemsList[19]	= "Nothing"
equippedItemsList[20]	= "Nothing"
equippedItemsList[21]	= "Nothing"
equippedItemsList[22]	= "Nothing"
equippedItemsList[23]	= "Nothing"

local equippedItemsIDs = {}
equippedItemsIDs[0]		= 0
equippedItemsIDs[1]		= 0
equippedItemsIDs[2]		= 0
equippedItemsIDs[3]		= 0
equippedItemsIDs[4]		= 0
equippedItemsIDs[5]		= 0
equippedItemsIDs[6]		= 0
equippedItemsIDs[7]		= 0
equippedItemsIDs[8]		= 0
equippedItemsIDs[9]		= 0
equippedItemsIDs[10]	= 0
equippedItemsIDs[11]	= 0
equippedItemsIDs[12]	= 0
equippedItemsIDs[14]	= 0
equippedItemsIDs[15]	= 0
equippedItemsIDs[16]	= 0
equippedItemsIDs[17]	= 0
equippedItemsIDs[18]	= 0
equippedItemsIDs[19]	= 0
equippedItemsIDs[20]	= 0
equippedItemsIDs[21]	= 0
equippedItemsIDs[22]	= 0
equippedItemsIDs[23]	= 0

local characterLevel	= gv("lv", characterName)
local accountName		= gv("account", characterName)
local characterJob		= ( "@" .. job_classification_table[gv("job", characterName)][2] ) -- String!
local characterJlv		= gv("jlv", characterName)
local characterJp		= gv("jp", characterName)
local characterX		= gv("x", characterName)
local characterY		= gv("y", characterName)
local characterGold		= gv("gold", characterName)
local characterAP		= gv("ap", characterName)
local characterChaos	= gv("chaos", characterName)
local characterEthStone	= gv("ethereal_stone", characterName)

local characterPrem		= gv("premium", characterName)
	if characterPrem == 1 then
		local totalTime = gv("prem_remain")
		local days = math.floor((totalTime / 86400) / 100)
		totalTime = totalTime % 86400
	
		local hours = math.floor(totalTime / 3600)
		totalTime = totalTime % 3600
	
		local minutes = math.floor(totalTime / 60)
		local seconds = totalTime % 60
		
		characterPrem = "" .. days .. "D, ".. hours .. "h, " .. minutes .. "m, " .. seconds .. "s"
	end

local chatInfo =	"<center><b>#@chName@# | #@acc@#<br>#@job@# #@jlv@# JLv | #@lv@# Lv | #@jp@# JP<br>Premium: #@prem@# | X: #@x@#; Y: #@Y@#<br>#@R@# <b>R<br>#@AP@# <b>AP<br>#@LAK@# <b>LAK<br>#@eth@# <b>Ethereal stone"
local itemString	= "<inv><b>===================================================<br><b>#@s0#@</b>: #@i0@# [ID: #@id0@#]<br><b>#@s1#@</b>: #@i1@# [ID: #@id1@#]<br><b>#@s2#@</b>: #@i2@# [ID: #@id2@#]<br><b>#@s3#@</b>: #@i3@# [ID: #@id3@#]<br><b>#@s4#@</b>: #@i4@# [ID: #@id4@#]<br><b>#@s5#@</b>: #@i5@# [ID: #@id5@#]<br><b>#@s6#@</b>: #@i6@# [ID: #@id6@#]<br><b>#@s7#@</b>: #@i7@# [ID: #@id7@#]<br>"
local decoString	= "<inv><b>===================================================<br><b>#@s14#@</b>: #@i14@# [ID: #@id14@#]<br><b>#@s15#@</b>: #@i15@# [ID: #@id15@#]<br><b>#@s16#@</b>: #@i16@# [ID: #@id16@#]<br><b>#@s17#@</b>: #@i17@# [ID: #@id17@#]<br><b>#@s18#@</b>: #@i18@# [ID: #@id18@#]<br><b>#@s19#@</b>: #@i19@# [ID: #@id19@#]<br><b>#@s20#@</b>: #@i20@# [ID: #@id20@#]<br><b>#@s21#@</b>: #@i21@# [ID: #@id21@#]<br>"
local etcString		= "<inv><b>===================================================<br><b>#@s8#@</b>: #@i8@# [ID: #@id8@#]<br><b>#@s9#@</b>: #@i9@# [ID: #@id9@#]<br><b>#@s10#@</b>: #@i10@# [ID: #@id10@#]<br><b>#@s11#@</b>: #@i11@# [ID: #@id11@#]<br><b>#@s12#@</b>: #@i12@# [ID: #@id12@#]<br><b>#@s22#@</b>: #@i22@# [ID: #@id22@#]<br><b>#@s23@#</b>: #@i23@# [ID: #@id23@#]<br>"

	for i = 0, 5000 do
		if get_wear_item_handle(i) ~= nil and get_wear_item_handle(i) ~= 0 then
			local item_id = get_item_code(get_wear_item_handle(i)) or 0
			local item_name_id = get_item_name_id(item_id) or 0
			local item_name = "#@enhance@# #@itemname@# #@itemlevel@#"

			if item_name_id ~= 0 then
				local itemEnhance = tonumber(get_item_enhance(get_wear_item_handle(i)))
				local itemLv = tonumber(get_item_level(get_wear_item_handle(i)))
				local itemEnhanceString = "+#@enh@#"
				local itemLevelString1 = "#@lvstr@# #@level@#"
				local itemLevelString2 = "@6610"
			
				if itemEnhance > 0 then
					itemEnhanceString = sconv(itemEnhanceString, "#@enh@#", itemEnhance)
				else
					itemEnhanceString = ' '
				end
				
				if itemLv > 0 then
					itemLevelString1 = sconv(itemLevelString1, "#@lvstr@#", itemLevelString2, "#@level@#", itemLv)
				else
					itemLevelString1 = ' '
				end
				
				item_name = sconv(item_name, "#@enhance@#", itemEnhanceString, "#@itemname@#", '@'.. item_name_id, "#@itemlevel@#", itemLevelString1)
				equippedItemsList[i] = item_name
			else
				item_name = "Nothing"
			end
			if item_id ~= 0 and item_id ~= '' and item_id ~= nil then equippedItemsIDs[i] = item_id end
		end

	end
	
	message(sconv(chatInfo,"#@chName@#",characterName,"#@acc@#",accountName,"#@job@#",characterJob,"#@jlv@#",characterJlv,"#@lv@#",characterLevel,"#@jp@#",characterJp,"#@prem@#",characterPrem,"#@x@#",characterX,"#@Y@#",characterY,"#@R@#",characterGold,"#@AP@#",characterAP,"#@LAK@#",characterChaos,"#@eth@#",characterEthStone))
	message(sconv(itemString,"#@s0#@",SlotsTable[0],"#@i0@#",equippedItemsList[0],"#@id0@#",equippedItemsIDs[0],"#@s1#@",SlotsTable[1],"#@i1@#",equippedItemsList[1],"#@id1@#",equippedItemsIDs[1],"#@s2#@",SlotsTable[2],"#@i2@#",equippedItemsList[2],"#@id2@#",equippedItemsIDs[2],"#@s3#@",SlotsTable[3],"#@i3@#",equippedItemsList[3],"#@id3@#",equippedItemsIDs[3],"#@s4#@",SlotsTable[4],"#@i4@#",equippedItemsList[4],"#@id4@#",equippedItemsIDs[4],"#@s5#@",SlotsTable[5],"#@i5@#",equippedItemsList[5],"#@id5@#",equippedItemsIDs[5],"#@s6#@",SlotsTable[6],"#@i6@#",equippedItemsList[6],"#@id6@#",equippedItemsIDs[6],"#@s7#@",SlotsTable[7],"#@i7@#",equippedItemsList[7],"#@id7@#",equippedItemsIDs[7],"#@s8#@",SlotsTable[8],"#@i8@#",equippedItemsList[8],"#@id8@#",equippedItemsIDs[8],"#@s9#@",SlotsTable[9],"#@i9@#",equippedItemsList[9],"#@id9@#",equippedItemsIDs[9],"#@s10#@",SlotsTable[10],"#@i10@#",equippedItemsList[10],"#@id10@#",equippedItemsIDs[10],"#@s11#@",SlotsTable[11],"#@i11@#",equippedItemsList[11],"#@id11@#",equippedItemsIDs[11]))
	message(sconv(decoString,"#@s12#@",SlotsTable[12],"#@i12@#",equippedItemsList[12],"#@id12@#",equippedItemsIDs[12],"#@s14#@",SlotsTable[14],"#@i14@#",equippedItemsList[14],"#@id14@#",equippedItemsIDs[14],"#@s15#@",SlotsTable[15],"#@i15@#",equippedItemsList[15],"#@id15@#",equippedItemsIDs[15],"#@s16#@",SlotsTable[16],"#@i16@#",equippedItemsList[16],"#@id16@#",equippedItemsIDs[16],"#@s17#@",SlotsTable[17],"#@i17@#",equippedItemsList[17],"#@id17@#",equippedItemsIDs[17],"#@s18#@",SlotsTable[18],"#@i18@#",equippedItemsList[18],"#@id18@#",equippedItemsIDs[18],"#@s19#@",SlotsTable[19],"#@i19@#",equippedItemsList[19],"#@id19@#",equippedItemsIDs[19],"#@s20#@",SlotsTable[20],"#@i20@#",equippedItemsList[20],"#@id20@#",equippedItemsIDs[20],"#@s21#@",SlotsTable[21],"#@i21@#",equippedItemsList[21],"#@id21@#",equippedItemsIDs[21],"#@s22#@",SlotsTable[22],"#@i22@#",equippedItemsList[22],"#@id22@#",equippedItemsIDs[22],"#@s23#@",SlotsTable[23],"#@i23@#",equippedItemsList[23],"#@id23@#",equippedItemsIDs[23]))
	message(sconv(etcString,"#@s8#@",SlotsTable[8],"#@i8@#",equippedItemsList[8],"#@id8@#",equippedItemsIDs[8],"#@s9#@",SlotsTable[9],"#@i9@#",equippedItemsList[9],"#@id9@#",equippedItemsIDs[9],"#@s10#@",SlotsTable[10],"#@i10@#",equippedItemsList[10],"#@id10@#",equippedItemsIDs[10],"#@s11#@",SlotsTable[11],"#@i11@#",equippedItemsList[11],"#@id11@#",equippedItemsIDs[11],"#@s12#@",SlotsTable[12],"#@i12@#",equippedItemsList[12],"#@id12@#",equippedItemsIDs[12],"#@s22#@",SlotsTable[22],"#@i22@#",equippedItemsList[22],"#@id22@#",equippedItemsIDs[22],"#@s23@#",SlotsTable[23],"#@i23@#",equippedItemsList[23],"#@id23@#",equippedItemsIDs[23]))
end





function on_change_weapon()
local isEnabled = tonumber(get_env("game.antiweaponswap")) == 1
	if not isEnabled then return end

local currTime = get_os_time()
local playerName = gv("name")
local lastSwap = tonumber(get_env("last_swap_" .. playerName)) or 0
local swapCount = tonumber(get_env("swap_cnt_" .. playerName)) or 0

	if currTime - lastSwap <= 2 then
		swapCount = swapCount + 1
	else
		swapCount = 1
	end

	if swapCount >= 5 then
		notice(playerName .. " swapped weapons too often and got punishment!")
		warp_to_revive_position()
		add_state(6012, 10, 5000)
		swapCount = 0
	end

	set_env("last_swap_" .. playerName, currTime)
	set_env("swap_cnt_" .. playerName, swapCount)
end



function test_except()
for k,v in pairs(_G) do
  private_notice("k: " .. k)
end
end


function output_inventory()
local itemsTable = {103100,108401,112100,230100,230102,240100,307050,490001,540002,540005,540006,540009,540010,540011,540012,540013,540018,540019,540052,540053,540055,540057,540059,540065,540067,540079,540200,540201,540202,603001,690435,690448,700654,705002,705004,705007,705008,705011,710008,810223,810224,810226,810227,810228,810231,810232,810233,810234,900010,900011,900016,910005,910023,910025,910064,910065,910066,910067,910068,910069,910070,910071,910072,910073,910074,910085,910090,910109,930071,930072,930073,930074,930075,930076,950019,950073,950117,1000541,1100101,1100102,1100408,2000145,2010454,2010739,2011033,2011279,2012073,2012116,2012240,2012430,2012431,2012774,2012780,2012787,2012788,2012819,2012820,2012821,2012823,2012824,2012825,2012832,2013011,2013584,2013816,2016027,2016028,2016030,2016031,2016032,2016033,2016034,2016035,2016037,2016038,2016039,2016126,2016132,2016153,2016154,3100034,3100036,3630334,3630339,103730509,106730509,108730509,700000806,700000805,700000804,700000803,601100285,210730709,113730509,112730509, 3701701, 710107}

	for i = 1, #itemsTable do
		if find_item(itemsTable[i]) > 0 then
			local itemHandle = get_item_handle(itemsTable[i])
			local itemName = '@' .. get_item_name_id(itemsTable[i])
			local itemLevel = get_item_level(itemHandle)
			local itemEnhance = get_item_enhance(itemHandle)

			local noticeText = "[Item] ID: #@item_id@#; Handle: #@item_handle@#; Level: #@item_level@#; Enhance: #@item_enhance@#; Name: #@item_name@#"
			
			private_notice(sconv(noticeText, "#@item_id@#", itemsTable[i], "#@item_handle@#", itemHandle, "#@item_level@#", itemLevel, "#@item_enhance@#", itemEnhance, "#@item_name@#", itemName))
		end
	end

end



function dlg_start()		-- Function to output NPC title. So all you do is type "dlg_start()" now instead of long ass "dlg_title(get_npc_title())"
	dlg_title(get_npc_title())
end

function get_npc_title() 								-- Function to get NPC title; Using this way: npc_title(get_npc_title())
local firstName = '@' .. get_npc_type()	-- .. ' ' .. '@' .. get_npc_name() 
local secondName = '@' .. get_npc_name()
	if firstName == nil or firstName == "" then firstName = " " end
	if secondName == nil or secondName == "" then secondName = " " end
local result = sconv("#@first_name@# #@second_name@#","#@first_name@#",firstName,"#@second_name@#",secondName)
	return result
end

function dlg_end()
	dlg_menu( "@90010001", '' )
	dlg_show()
end


function r()
	refresh('script')
end


function get_segment_data()
local x = gv("x")
local y = gv("y")
local x_id = math.floor(x / 16128)
local y_id = math.floor(y / 16128)

    if x_id > 9 and y_id > 9 then
		message("Map name: m0" .. x_id .. "_0" .. y_id)
    elseif x_id > 9 then
		message("Map name: m0" .. x_id .. "_00" .. y_id)
    elseif y_id > 9 then
		message("Map name: m00" .. x_id .. "_0" .. y_id)
    else
		message("Map name: m00" .. x_id .. "_00" .. y_id)
    end
    local loc_x = x - x_id * 16128
    local loc_y = y - y_id * 16128
    local segment = math.floor(loc_y / 252) * 64 + math.floor(loc_x / 252)
		message("Segment ID: " .. segment)
    local segment_x = loc_x - math.floor(loc_x / 252) * 252
    local segment_y = loc_y - math.floor(loc_y / 252) * 252
		message("Segment X: " .. segment_x)
		message("Segment Y: " .. segment_y)
end


function test1(id)
	id = id or 0
	
    dlg_special("confirm_window", "test1(".. id + 1 .. ")", "id = " .. id )
end