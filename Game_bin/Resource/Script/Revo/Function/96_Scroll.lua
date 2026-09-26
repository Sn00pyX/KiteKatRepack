function Chakra_scroll()

		local tbuffs =
		{
		--	Buff ID,	Max Lv
			163404,	100,	-- Blessing of Vitality
			163405,	100,	-- Blessing of Intelligence
			163406,	100,		-- Blessing of Wisdom
			163407,	100,	-- Blessing of Strength
			163433,	167,	-- Shining Armor
			2505,	322,	-- Shining Weapon
			2507,	322,	-- Shining Weapon
			13423,	65,		-- Speed of the Wind
			13424,	560,	-- Rock Energy
			13425,	577,	-- Force of Sacred Fire
			163429,	24,		-- Wind Weapon
			2506,	79,		-- Dark Might
			2508,	79,		-- Dark Might
			314049,	356,	-- Insight
			163449,	77,		-- Protector's Force
			163448,	77,		-- Angel's Force
			163440,	234,	-- Rally
			314099,	188,	-- Howl at the Moon
			314039,	10,		-- Demonic Howl
			314016,	210,	-- Divine Purpose
			314017,	360,	-- Asuran Haste
			314018,	210,	-- Gaian Strength
			314077,	2,		-- Heaven 5%
			314078,	2,		-- Heaven 10%
			314079,	2,		-- Heaven 20%
			1014,	110		-- Concentration
		}
		
		local cnt_buffs = table.getn(tbuffs)/2

		for i = 1, cnt_buffs do
			local base = 1+(i-1)*2
			
			local getBuffLv = get_state_level(tbuffs[base])

			if getBuffLv >= 1 and getBuffLv < tbuffs[base+1] then
				add_cstate ( tbuffs[base], getBuffLv, 360000 )
				
			end
			end

end 

function Time_scroll()
local tbuffs =
		{
		--	Buff ID,	Max Lv
			163404,	100,	-- Blessing of Vitality
			163405,	100,	-- Blessing of Intelligence
			163406,	100,	-- Blessing of Wisdom
			163407,	100,	-- Blessing of Strength
			163433,	167,	-- Shining Armor
			2505,	322,	-- Shining Weapon
			2507,	322,	-- Shining Weapon
			13423,	65,		-- Speed of the Wind
			13424,	560,	-- Rock Energy
			13425,	577,	-- Force of Sacred Fire
			163429,	24,		-- Wind Weapon
			2506,	79,		-- Dark Might
			2508,	79,		-- Dark Might
			314049,	356,	-- Insight
			163449,	77,		-- Protector's Force
			163448,	77,		-- Angel's Force
			163440,	234,	-- Rally
			314099,	188,	-- Howl at the Moon
			314039,	10,		-- Demonic Howl
			314016,	210,	-- Divine Purpose
			314017,	360,	-- Asuran Haste
			314018,	210,	-- Gaian Strength
			314077,	2,		-- Heaven 5%
			314078,	2,		-- Heaven 10%
			314079,	2,		-- Heaven 20%
			1014,	110		-- Concentration
		}
		
		local cnt_buffs = table.getn(tbuffs)/2

		for i = 1, cnt_buffs do
			local base = 1+(i-1)*2
			
			local getBuffLv = get_state_level(tbuffs[base])

			if getBuffLv >= 1 and getBuffLv < tbuffs[base+1] then
				add_state ( tbuffs[base], getBuffLv, 360000 )
				
			end
				
		end
	end

 