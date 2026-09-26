-- Lua 스크립트 암호화
function get_module_name()
             return "BattleArena"
end


function on_enter_battle_arena( arena_id, instance_no, is_reconnect )

end

function on_leave_battle_arena( arena_id, instance_no, is_disconnect )

end

function on_battle_arena_bingo( arena_id, instance_no, team_no, line_count )

	-- 빙고 시 처리 내용
	-- 팀 버프 부여
	-- 배틀 포인트 추가
	-- 실시간 ap 추가
	
	--5113빙고 버프:물리, 마법 공격력 증가1<20%><(version:8.1)>
	--5114빙고 버프:물리, 마법 공격력 증가2<30%><(version:8.1)>


	--5119빙고 버프:물리, 마법 방어력 증가1<20%><(version:8.1)>
	--5120빙고 버프:물리, 마법 방어력 증가2<30%><(version:8.1)>


	--5125빙고 버프:공격, 시전 속도 증가1<20%><(version:8.1)>
	--5126빙고 버프:물리, 마법 방어력 증가2<30%><(version:8.1)>

	--5127빙고 버프:이동속도 증가1<20%><(version:8.1)>
	--5128빙고 버프:이동속도 증가2<30%><(version:8.1)>
	--5129빙고 버프:회피 증가1<20%><(version:8.1)>
	--5130빙고 버프:회피 증가2<30%><(version:8.1)>

	--5135빙고 버프:물리, 마법 명중 증가1<20%><(version:8.1)>
	--5136빙고 버프:물리, 마법 명중 증가2<30%><(version:8.1)>

	--5137빙고 버프:마법 저항 증가1<20%><(version:8.1)>
	--5138빙고 버프:마법 저항 증가2<30%><(version:8.1)>
	--5139빙고 버프:크리티컬 확률 증가1<20%><(version:8.1)>
	--5140빙고 버프:크리티컬 확률 증가2<30%><(version:8.1)>
	--5141빙고 버프:크리티컬 위력 증가1<20%><(version:8.1)>
	--5142빙고 버프:크리티컬 위력 증가2<30%><(version:8.1)>
	--5143빙고 버프:HP 회복1<20%><(version:8.1)>
	--5144빙고 버프:HP 회복2<30%><(version:8.1)>
	--5145빙고 버프:MAX HP/MP 증가1<20%><(version:8.1)>
	--5146빙고 버프:MAX HP/MP 증가2<30%><(version:8.1)>
	
	local bingo_buff = 0
	local buff_id = 0
	
	if line_count == 1 then
	
		buff_id = math.random( 1, 4)
		
		if buff_id == 1 then
			bingo_buff = 5113 --5113빙고 버프:물리, 마법 공격력 증가1<20%><(version:8.1)>
		elseif buff_id == 2 then
			bingo_buff = 5119 --5119빙고 버프:물리, 마법 방어력 증가1<20%><(version:8.1)>
		elseif buff_id == 3 then
			bingo_buff = 5125 --5125빙고 버프:공격, 시전 속도 증가1<20%><(version:8.1)>
		elseif buff_id == 4 then
			bingo_buff = 5135 --5135빙고 버프:물리, 마법 명중 증가1<20%><(version:8.1)>
		end
	
		-- add_state(6006, 1, 100)
		do_each_player_in_battle_arena_team( arena_id, instance_no, team_no, "add_state(" .. bingo_buff .. " ,1 , 10000)" )

	
	elseif  line_count == 2 then
	
		buff_id = math.random( 1, 4)
		
		if buff_id == 1 then
			bingo_buff = 5114 --5114빙고 버프:물리, 마법 공격력 증가2<30%><(version:8.1)>
		elseif buff_id == 2 then
			bingo_buff = 5120 --5120빙고 버프:물리, 마법 방어력 증가2<30%><(version:8.1)>
		elseif buff_id == 3 then
			bingo_buff = 5126 --5126빙고 버프:물리, 마법 방어력 증가2<30%><(version:8.1)>
		elseif buff_id == 4 then
			bingo_buff = 5136 --5136빙고 버프:물리, 마법 명중 증가2<30%><(version:8.1)>
		end
		
		
		do_each_player_in_battle_arena_team( arena_id, instance_no, team_no, "add_state(" .. bingo_buff .. " ,1 , 15000)" )

	
	elseif  line_count == 3 then
	
		do_each_player_in_battle_arena_team( arena_id, instance_no, team_no,
			"add_state(5113 ,1 , 18000);add_state(5119 ,1 , 18000);add_state(5125 ,1 , 18000);add_state(5135 ,1 , 18000);add_state(5143 ,1 , 18000);add_state(5146 ,1 , 18000)" )
	
	end
	

end

function arena_slaughter_prop()

	local arena_id = get_value( "arena_id" )
	local team_no = get_battle_arena_team_no()
	local prop_id = 0
	
	-- 연합군 or 마녀군
	if team_no == 0 then
	
		prop_id = 30217
	
	elseif team_no == 1 then
	
		prop_id = 30216
	
	end
	
	if arena_id == 2001 then
	
		activate_battle_arena_prop( 1 )
			
		add_field_prop( prop_id, 3, 67610, 83352, gv('layer'), 0, 0, 0, 0, 3, 3, 3 )
				
	elseif arena_id == 2002 then
	
		activate_battle_arena_prop( 1 )
	
		add_field_prop( prop_id, 3, 51482, 83352, gv('layer'), 0, 0, 0, 0, 3, 3, 3 )
	
	elseif arena_id == 2003 then
	
		activate_battle_arena_prop( 1 )
	
		add_field_prop( prop_id, 3, 35354, 83352, gv('layer'), 0, 0, 0, 0, 3, 3, 3 )
		
	elseif arena_id == 2004 then
	
		activate_battle_arena_prop( 1 )
	
		add_field_prop( prop_id, 3, 67610, 67224, gv('layer'), 0, 0, 0, 0, 3, 3, 3 )
		
	elseif arena_id == 2501 then
	
		activate_battle_arena_prop( 1 )
	
		add_field_prop( prop_id, 3, 35354, 51096, gv('layer'), 0, 0, 0, 0, 3, 3, 3 )

	end
		
end






-----------------------------------------------------------------------------------------
---------------------------------		ap 상점	    -------------------------------------
-----------------------------------------------------------------------------------------


function NPC_arena_point_contact()

--웅담 말시켰을 때 인사말
	dlg_title( "@90996953" )
	dlg_text( "@90996954" )

				
	-- L베어로드 전용 물품
	dlg_menu( "@91000264", "open_market( 'bearload_shop_onlybear' )" )		
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function bosspet()

	--insert_item(690501, 1)
	insert_item(690502, 1)
	insert_item(690503, 1)
	--insert_item(690504, 1)
	insert_item(690505, 1)
	insert_item(690506, 1)
	insert_item(690507, 1)
	insert_item(690508, 1)
	insert_item(690509, 1)
	insert_item(690510, 1)
	insert_item(690511, 1)
	insert_item(690512, 1)
	insert_item(690513, 1)
	insert_item(690514, 1)
	insert_item(690515, 1)
	insert_item(690516, 1)
	insert_item(690517, 1)
	insert_item(690518, 1)
	insert_item(690519, 1)
	insert_item(690520, 1)
	--insert_item(690501, 1)
	insert_item(690502, 1)
	insert_item(690503, 1)
	--insert_item(690504, 1)
	insert_item(690505, 1)
	insert_item(690506, 1)
	insert_item(690507, 1)
	insert_item(690508, 1)
	insert_item(690509, 1)
	insert_item(690510, 1)
	insert_item(690511, 1)
	insert_item(690512, 1)
	insert_item(690513, 1)
	insert_item(690514, 1)
	insert_item(690515, 1)
	insert_item(690516, 1)
	insert_item(690517, 1)
	insert_item(690518, 1)
	insert_item(690519, 1)
	insert_item(690520, 1)

end

