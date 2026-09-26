





-- ======================================================================================== --
-- ====================== TODO: Whoever would try to use this NPC ========================= --
-- === Give yourself a favor and translate these strings (localize) to other languages ==== --
-- ======================================================================================== --


KitekatManager = KitekatManager or {}

KitekatManager.contact = function()
local playerPerm = gv("permission")

--100050001	0	Kitekat
--100050002	0	Manager
	dlg_start()
	dlg_text( "@100046001" )	-- Welcome, stranger, to our server! What can I do for you?
	
	dlg_menu( "@100046007",	"KitekatManager.petLevelupSelection()"		) -- I want to levelup my pets
	dlg_menu( "@100046002",	"KitekatManager.jobLevelChangeRequest()"	) -- I want to get JobLevel up!
	dlg_menu( "@100046008",	"KitekatManager.getServerInfo()"			) -- Server Information
	
	if playerPerm == 100 then
		dlg_menu(" " , "KitekatManager.contact()")	-- Spacebar just to split admin things from others
		dlg_menu("[Admin] Server settings", "KitekatManager.globalServerSettings()")
		dlg_menu("[Admin] Spawn mobs", "monsters_test_spawn()")
	end
	
	dlg_end()
 
end


function monsters_test_spawn()
local monstersTable = { 22000167, 22000380, 22000384 }

	for i = 1, #monstersTable do
		add_npc(gv("x"), gv("y"), monstersTable[i], 1)
	end
end

KitekatManager.globalServerSettings = function()
local antimacro = tonumber(get_env("game.antimacro_on")) or 0
local freePremium = tonumber(get_env("game.premium_for_everybody")) or 0

--					RED				GREEN
local colors = { "<#ee2222>OFF", "<#06ff00>ON" }


	dlg_start()
	dlg_text( "Admin menu; Server control")
	
	dlg_menu( "Antimacro "		.. colors[antimacro + 1],	"KitekatManager.serverSettingSwitch('game.antimacro_on')")
	dlg_menu( "Free Premium "	.. colors[freePremium + 1],	"KitekatManager.serverSettingSwitch('game.premium_for_everybody')")

	dlg_end()
end

KitekatManager.serverSettingSwitch = function( setting )
local envState = tonumber(get_env( setting )) or 0
	if envState == 0 then
		set_env(setting, 1)
	else
		set_env(setting, 0)
	end
	
	KitekatManager.globalServerSettings()
end

KitekatManager.petLevelupSelection = function()
	dlg_start()
	
	dlg_text( "@100046001" )	-- Welcome, stranger, to our server! What can I do for you?
	
	for i = 0, 5 do
	local creatureHandle = tonumber(get_creature_handle( i )) or 0
		if creatureHandle ~= 0 and get_creature_value(creatureHandle, "lv") < 180 then
			dlg_menu( get_creature_value(creatureHandle, "name"), "levelupmypets(" .. i .. ")" )
		end
	end

	dlg_end()
end

-- Original credits to key_strike. Slight edit (strings) by Fraun
KitekatManager.jobLevelChangeRequest = function()

	dlg_start()
	
    -- Get the players current Job Lvl
	local characterJlv = get_value( "jlv")
   
    -- Get the amount of job points needed to up grade to the next joblvl
    -- To get this we need to know the players rank
    local characterJobRank = job_classification_table[gv("job")][3]

    -- Then we need to look up the amount of Jobpoint needed in the [LevelResource] table
    local reqJP = level_resource_table[characterJlv][characterJobRank] 

	-- Get the amount of job points the player currently has
    local jobpoints = get_value( "jp" )   
        
		
	-- if the value is 0 then we know its hit the maximum joblvl.
	if reqJP == 0 then  
		-- Tell the user they have reached maximum level
		dlg_text( "@100046003" )	-- You have already reached your maximum job level
    	
	elseif jobpoints < reqJP then
        	-- Tell the user they dont have enough job points
		dlg_text( "@100046004" )	-- You dont have enough job points to level up
	
	else
	
		-- Do you want to level up you Job. It will cost #@requiredJP@# to level to job level #@charJlv@#
		sub_text = sconv("@100046005", "#@requiredJP@#", tostring(reqJP), "#@charJlv@#", tostring(characterJlv) )
		dlg_text( sub_text )
		dlg_menu( "@100046006", "KitekatManager.jobLevelChangeCore()" )	-- Level Up Job
	
	end
	
	dlg_end()
	
end

-- Original credits to key_strike. Light edit (strings) by Fraun
KitekatManager.jobLevelChangeCore = function()
	local characterJlv = get_value( "jlv")
	local characterJobRank = job_classification_table[gv("job")][3]
	local reqJP = level_resource_table[characterJlv][characterJobRank] 
	local jobpoints = get_value( "jp" )   
	local remainder_jp = jobpoints - reqJP
	
	sv( "jlv",	characterJlv + 1)
	sv( "jp",	remainder_jp )
	
	KitekatManager.jobLevelChangeRequest()
end


-- Output server info
KitekatManager.getServerInfo = function()
local VERSION = "9.5.2"	-- Define it yourself
local serverInfoString = "<br><br>#@servername@# #@ver@#<br><br>Exp: x#@exp@#<br>Drop: x#@drop@#<br>Gold: x#@gold@#<br>LAK: x#@lak@#<br>Antimacro: #@antimacro@#<br><br><#ffffff>Online: #@online@#<br>"
local serverName = get_env("auth.server_name")
local expRate = get_env("game.exp_rate")
local dropRate = get_env("game.item_drop_rate")
local goldRate = get_env("game.gold_drop_rate")
local lakRate = get_env("game.chaos_drop_rate")
local currentOnline = get_env("game.user_count")
local antimacro = tonumber(get_env("game.antimacro_on")) or 0
local antimacroText = "<#ee2222>OFF"

	if antimacro == 1 then antimacroText = "<#06ff00>ON" end
	
	dlg_general(sconv(serverInfoString, "#@servername@#", serverName, "#@ver@#", VERSION, "#@exp@#", expRate, "#@drop@#", dropRate, "#@gold@#", goldRate, "#@lak@#", lakRate, "#@antimacro@#", antimacroText, "#@online@#", currentOnline))

end