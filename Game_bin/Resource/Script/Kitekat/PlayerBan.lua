-- Enhanced account ban script for Rappelz (Lua 5.1)
-- Original script made by YoSiem
-- Adjusted by Fraun

--[[
--	Usage:
--	BanPlayer( name, duration, reason)
--	name -> player name to ban (string)
--	duration - a number from BanDurations table, corresponding to table numbers themselves (default is from 1 to 7)
--	reason - a number from BanReasons table, corresponding to table numbers themselves (default is from 1 to 5)
--	Example: BanPlayer("Fraun", 4, 2) -> Bans player "Fraun" for 1 day for "Insulting players" reason
]]--



-- Constants for flag names and state IDs
local BAN_FLAG         = "yBa"	-- Is ban active
local REASON_FLAG      = "yBr"	-- Reason for ban
local UNTIL_FLAG       = "yBu"	-- Flag contains timestamp until which ban would be

-- State IDs for punishment effects
local PETRIFY_STATE_ID	= 6003  -- Petrification
local YAK_STATE_ID		= 6007  -- YAK
local STUN_STATE_ID		= 6006  -- Stun
local STATE_LEVEL		= 666   -- Common level for all three

-- Messages
local BAN_MESSAGE_TEMPLATE = "@100046011"	-- Your account is banned until #@timestamp@#<br>Reason: #@reason@#<br>Contact us on Discord to appeal
local UNBAN_MESSAGE = "@100046012"			-- Your account has been unbanned. Thank you for your patience

-- Predefined ban reasons
local BanReasons =
{
	[1] = "@100046021",	-- AFK farming
	[2] = "@100046022",	-- Insulting players
	[3] = "@100046023",	-- Unauthorized third-party software
	[4] = "@100046024",	-- Exploiting game bugs
	[5] = "@100046025"	-- Other
}

-- Predefined durations (in seconds)
local BanDurations =
{
	[1]	= { label = "1h",	seconds = 3600 },
	[2]	= { label = "3h",	seconds = 3 * 3600 },
	[3]	= { label = "6h",	seconds = 6 * 3600 },
	[4]	= { label = "1d",	seconds = 24 * 3600 },
	[5]	= { label = "3d",	seconds = 3 * 24 * 3600 },
	[6]	= { label = "7d",	seconds = 7 * 24 * 3600 },
	[7]	= { label = "30d",	seconds = 30 * 24 * 3600 }
}

--[[
    AnnounceBan:
    Broadcasts to the server that a player was banned by someone.
    Uses gv('permission')==100 to distinguish Admin vs Moderator.
    @param targetName  (string)
    @param durationKey (int)
    @param reasonKey   (int)
]]

local function AnnounceBan(targetName, durationKey, reasonKey)
    local durInfo = BanDurations[durationKey]
    if not durInfo then return end

    local reasonText = BanReasons[reasonKey] or reasonKey
    local callerName = gv('name') or "System"
    local perm       = tonumber(gv('permission')) or 0
    local callerRole = (perm == 100) and "Admin" or "Moderator"
	
	-- [#@perm@#] #@name@# has banned #@player@# for #@time@#. Reason: #@reason@#.
	local msg = sconv("@100046031", "#@perm@#", callerRole, "#@name@#", callerName, "#@player@#", targetName, "#@time@#", durInfo.label, "#@reason@#", reasonText)

    announce(msg)
end

--[[
    BanPlayer:
    @param playerName  (string)
    @param durationKey (int)
    @param reasonKey   (int)
]]
function BanPlayer(playerName, durationKey, reasonKey)
    local durInfo = BanDurations[durationKey]
    local reason  = BanReasons[reasonKey]
	
    if not durInfo or not reason then
        private_notice("@100046032")	-- Invalid ban duration or reason.
        return
    end

    -- Server‐provided time
    local now      = get_os_time()
    local unbanAt  = now + durInfo.seconds

    -- Set flags
    set_flag(BAN_FLAG,    1,           playerName)
    set_flag(REASON_FLAG, reasonKey,   playerName)
    set_flag(UNTIL_FLAG,  unbanAt,     playerName)

    -- Warp & apply all three states
    warp(222202, 20317, playerName)
    add_state(PETRIFY_STATE_ID, STATE_LEVEL, durInfo.seconds, playerName)
    add_state(YAK_STATE_ID,     STATE_LEVEL, durInfo.seconds, playerName)
    add_state(STUN_STATE_ID,    STATE_LEVEL, durInfo.seconds, playerName)

    -- Announce to server
    AnnounceBan(playerName, durationKey, reasonKey)

    -- Notify the banned player
    dlg_general( sconv(BAN_MESSAGE_TEMPLATE, "#@timestamp@#", epoch_to_date(unbanAt), "#@reason@#", reason) )

	save()
    saveall()
end

--[[
    UnbanPlayer:
    @param playerName (string)
]]
function UnbanPlayer(playerName)
    -- Clear flags
    del_flag(BAN_FLAG)
    del_flag(REASON_FLAG)
    del_flag(UNTIL_FLAG)

    -- Remove all three states
    remove_state(PETRIFY_STATE_ID,	STATE_LEVEL, playerName)
    remove_state(YAK_STATE_ID,		STATE_LEVEL, playerName)
    remove_state(STUN_STATE_ID,		STATE_LEVEL, playerName)

    -- Notify
    dlg_general(UNBAN_MESSAGE, playerName)

    saveall()
end

--[[
    OnLoginCheck:
    Called in on_login to enforce or lift a ban.
]]
function OnLoginCheck()
    local name    = gv('name')
    local banned  = tonumber(get_flag(BAN_FLAG, name)) == 1
    if not banned then return end

    local now     = get_os_time()
    local untilAt = tonumber(get_flag(UNTIL_FLAG, name)) or 0

    if now >= untilAt then
        UnbanPlayer(name)
    else
        -- Still banned: warp, reapply states, remind
        warp(222202, 20317, name)
        add_state(PETRIFY_STATE_ID,	STATE_LEVEL, (untilAt - now) )
        add_state(YAK_STATE_ID,		STATE_LEVEL, (untilAt - now) )
        add_state(STUN_STATE_ID,	STATE_LEVEL, (untilAt - now) )

        local reasonKey = get_flag(REASON_FLAG, name) or "OTHER"
        local reason    = BanReasons[reasonKey] or "Unknown"
        
		local msgFunc = sconv(BAN_MESSAGE_TEMPLATE, "#@timestamp@#", epoch_to_date(unbanAt), "#@reason@#", reason)
		-- Doing that without dlg_general because we're kicking the player after these messages
		-- Because "You have been disconnected from the server" dlg_general message overrides the current one
		private_notice( msgFunc )
		message( msgFunc )
		
		kick(name)	-- Kick the player
    end
end

-- Utility: check if a player is banned
function IsPlayerBanned(playerName)
    return tonumber(get_flag(BAN_FLAG, playerName)) == 1
end
