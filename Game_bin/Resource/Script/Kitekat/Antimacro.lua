


function script_on_dead( monster_level, monster_x, monster_y, monster_layer )
local LEVEL_GAP = 15
local antimacroENV = tonumber(get_env("game.antimacro_on")) or 0

	if antimacroENV == 1 then
	local playerLevel = gv('lv')
		
		if (playerLevel - monster_level) < LEVEL_GAP then
			AntiMacro:Trigger( monster_level )
		end
	end

end




AntiMacro = AntiMacro or {}

function AntiMacro:Trigger( monster_level )
local randomNumber = math.random(1,1)
local mathemathics, symbol, randomHandler, textHandler
local currTime = get_os_time()
local macroFlag = tonumber(get_flag("mcr")) or 0
local lastMacroTime = get_flag("lMaT")

-- Initializing maximum wrong attempts for antimacro before punishment
local maxWrongAnswers = tonumber(get_env("game.antimacro_maximum_wrong_answers")) or 5
	
	-- If last time macro was never triggered, we make a flag with current time
	if lastMacroTime == nil or lastMacroTime == '' then
		set_flag("lMaT",currTime)
		lastMacroTime = currTime
		save()
	end
	
	
	
	if ( lastMacroTime + 20) <= currTime then -- 20 seconds between questions
			if randomNumber <= 5000 then -- If you are unlucky, trigger a question
			local playerName = gv("name")
		
				local isAnswered = tonumber( get_env( "is_answered_" .. playerName ) ) or 0
					if isAnswered == 0 then -- Handling non-answered previous question and first question
						isAnswered = 0
						set_env("is_answered_" .. playerName, 0) -- Just in case 
				
						macroFlag = macroFlag + 1 -- Instantly incrementing macro flag to prevent abusing cancel button or just ignoring
						set_flag("mcr", macroFlag)
				
						if macroFlag >= 5 then -- Ignoring/cancelling case
							macro_punishment(1) -- Punish
							return
						end
					end
	
				mathemathics = math.random(1,3) -- Add, Subtract or Multiply 
			
				local number1 = { }	-- Array of first numbers
				local number2 = { } -- Array of second numbers
				local summary = { } -- Array of answers
				
				if mathemathics == 1 then -- Plus (Add)
					for i = 1, 5 do
						number1[i] = math.random(1,50)
						number2[i] = math.random(1,50)
						summary[i] = ( number1[i] + number2[i] )
						symbol = '+'
						
					end
					
				elseif mathemathics == 2 then -- Minus (Subtract)
					for i = 1, 5 do
						number1[i] = math.random(30,50)
						number2[i] = math.random(1,20)
						summary[i] = ( number1[i] - number2[i] )
						symbol = '-'
						
					end
					
				elseif mathemathics == 3 then -- Multiply
					for i = 1, 5 do
						number1[i] = math.random(1,10)
						number2[i] = math.random(1,10)
						summary[i] = ( number1[i] * number2[i] )
						symbol = '*'
						
					end
					
				end
					
				set_flag("lMaT", currTime)		-- Set last macro time to current time
				
					if macroFlag > 5 then		-- Too much times skipped the answer
						AntiMacro:Punish(1)		-- Punishment
					end
					
				save() -- Saving the flags
			
				-- Here we generate random number from 1 to 5. Generated number would be a true number. Then we output 5 lines from 1 to 5. And only 1 of them will be correct (the one randomly generated here)
				randomHandler = math.random(1,5)
				textHandler = "<size:14>" .. number1[randomHandler] .. symbol .. number2[randomHandler] .. "?"
				set_env("is_answered_" .. playerName, 0) -- Setting env to non-answered directly before questions
				dlg_special_menu(textHandler, 'Text', "<size:12>" .. summary[1], 'AntiMacro:Answer('..summary[1]..','..summary[randomHandler]..')', "<size:12>" .. summary[2], 'AntiMacro:Answer('..summary[2]..','..summary[randomHandler]..')', "<size:12>" .. summary[3],  'AntiMacro:Answer('..summary[3]..','..summary[randomHandler]..')', "<size:12>" .. summary[4],  'AntiMacro:Answer('..summary[4]..','..summary[randomHandler]..')', "<size:12>" .. summary[5],  'AntiMacro:Answer('..summary[5]..','..summary[randomHandler]..')')
			end
		end
	
end


function AntiMacro:Answer( picked_number, correct_answer )

-- Initializing maximum wrong attempts for antimacro before punishment
local maxWrongAnswers = tonumber(get_env("game.antimacro_maximum_wrong_answers")) or 5
	
local isAnswered = tonumber(get_env("is_answered_" .. gv("name")))
	if isAnswered == nil or isAnswered == 0 or isAnswered == '' then -- Handling non-answered previous question and first question
		set_env("is_answered_" .. gv("name"), 1) -- Setting this env to answered
	end
	
	if picked_number == correct_answer then		-- If answer is correct
		set_flag("mcr", 0) -- Nullifying macro flag
		
	else	-- If answer is wrong
	local macroCount = get_flag("mcr")
	
		set_flag("mcr", macroCount + 1)		-- Macro flag increment in case of wrong answer
		
		if macroCount > maxWrongAnswers then	-- Reached maximum wrong answers count. Result is a punishment
			AntiMacro:Punish(1)
		end
		
	end -- We are not adding additional count to macro flags before we ALREADY did it before showing window
	
	save() -- Important save
end


-- If player made an incorrect answer X times. Actual punishment
function AntiMacro:Punish( multiplier )

	-- Add your own logic. May be buff or account ban lmfao

	warp(get_flag("rx"),get_flag("ry")) -- Warp player to his RX/RY coords
	add_state(10000001,1,( punishment_time * 100 ))
	dlg_general("You are punished now")
	save() -- Save
end
