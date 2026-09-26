
--- Lua 스크립트 암호화
function get_module_name()
             return "ETC_dungeon_prop"
end


--	제어장치 및 코어 점령시 호출될 함수. 
-- 내부적으로 서버에서 제공하는 소유권 변경 함수 및 주변 몹 리젠 함수( respawn_near_mob( range ) )가 사용됨.
function casting_tactical_position( dungeon_id, position_id )

	-- 호출된 지점의 (제어장치 혹은 던전 코어) 소유권을 변경시키는 부분
	change_tactical_position_owner( dungeon_id, position_id )
	
	-- 변경된 지점 주변의 가디언을 리젠
	respawn_guardian_object( dungeon_id, position_id )

	
end

--	던전 출구 클릭시 필드로 텔레포트 시켜 주는 일반 함수.
function exit_dungeon( dungeon_id )

	-- 잃어버린 던전1
	if dungeon_id == 130000 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 155817 + math.random(0,60) , 103724 + math.random(0,60) )
	
	-- 잃어버린 던전2
	elseif dungeon_id == 130600 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 152309 + math.random(0,60) , 102886 + math.random(0,60) )
	
	-- 수정 계곡1
	elseif dungeon_id == 130300 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 103210 + math.random(0,60) , 100366 + math.random(0,60) )

	-- 수정 계곡2
	elseif dungeon_id == 130500 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 99757 + math.random(0,60) , 103236 + math.random(0,60) )

	-- 메마른 달빛의 유적1
	elseif dungeon_id == 130400 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 132995 + math.random(0,60) , 87096 + math.random(0,60) )
	 
	-- 메마른 달빛의 유적2
	elseif dungeon_id == 130700 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 130842 + math.random(0,60) , 79586 + math.random(0,60) )
	
    -- 팔미르 제 1 유적 
	elseif dungeon_id == 130800 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 132680 + math.random(0,60) , 128030 + math.random(0,60) )	

    -- 팔미르 제 2 유적 
	elseif dungeon_id == 130900 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 137441 + math.random(0,60) , 128115 + math.random(0,60) )		

    -- 잃어버린 비밀의 섬(백룡의 쉼터) 
	elseif dungeon_id == 121000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 91942 + math.random(0,60) , 117103 + math.random(0,60) )			

    -- 잃어버린 비밀의 섬(흑룡의 그늘) 
	elseif dungeon_id == 122000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 85752 + math.random(0,60) , 118062 + math.random(0,60) )
		
	-- 잃어버린 비밀의 섬(사룡의 심장) 
	elseif dungeon_id == 123000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 91998 + math.random(0,60) , 124400 + math.random(0,60) )
			
	-- 엘 카시아 워프 프랍 (엘 카시아 -> 마르두카 폭포 필드)
	elseif dungeon_id == 120700 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 146015 + math.random(0,61) , 135591+ math.random(0,10) )

	-- 숨겨진 팔미르 던전 워프 프랍 (숨겨진 팔미르 입구 -> 팔미르 제 1 실 입구 필드)
	elseif dungeon_id == 120292 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 132680 + math.random(0,60) , 128030 + math.random(0,60) )
	
	-- 숨겨진 수정 계곡 던전 워프 프랍 (숨겨진 수정 계곡 입구 -> 수정 계곡 입구 필드)
	elseif dungeon_id == 70192 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 103234+ math.random(0,60) , 100310 + math.random(0,60) )
	
	-- 고대인의 유적 던전 워프 프랍 (고대인의 유적 -> 사이라그 폐허 필드)
	elseif dungeon_id == 121100 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 108674+ math.random(0,60) , 76570 + math.random(0,60) )

	end	
			
end

function common_warp_gate( x, y )
	warp( x + math.random(0,10) , y + math.random(0,10), gv("layer") )
end

function warp_gate( prop_id )
	
	-- 나비스 라미아 갑판 
	if prop_id == 130881 then		
		if find_item ( 1000077 ) == 1 then -- 고리수정
			warp( 190072 + math.random(0,10) , 34171+ math.random(0,10), gv("layer") ) -- 텔레포트 좌표					
		else 
			cprint( "@1233" ) -- '워프게이트 사용을 위한 허가를 받지 못했습니다.'
		end
	end	
	
	-- 나비스 라미아 주방  
	if prop_id == 130883 then
		-- 퀘스트 상태 체크 	get_quest_progress(ID)  
		-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
		local quest_progress3 =  get_quest_progress(1247) -- 유령선 나비스라미아: 진행 3
		-- 퀘스트 도중 다시 처음부터 시작 할때 진행 불가
		if quest_progress3 == 2 or quest_progress3 == 255 then
			warp( 190018 + math.random(0,10) , 37865+ math.random(0,10), gv("layer") ) -- 텔레포트 좌표
		else 
			cprint( "@1233" )
		end
	end
	
	-------------------------------------------------------------------------------
								       --큐브릭 던전 --
	-------------------------------------------------------------------------------

	-- 큐브릭 던전 입구 프랍, 던전 입장 
	if prop_id == 60101 then
		if get_value( 'party_id' ) == 0 or get_value( 'level' ) <  150 then
			--레벨 150이상, 파티를 결성해야 입장 가능 (1인이상, 사실상 의미 인원은 의미 없음)
			cprint( "@9831" )
			return	
		else 
			-- 같은 파티원만 입장할 수 있습니다.
			cprint( "@9830" )			
			-- 던전 입장
			dlg_special( 'confirm_window', 'warp_to_instance_dungeon(30000)', '@9825\v#@dungeon_name@#\v@80030000' )
		end

	elseif prop_id == 60110 then
		-- 입구8
		warp_to_cubric_branch_room( 1 )

	-- 10번방 입장(최종보스)
	elseif prop_id == 60111 then
		local sub_boss1_alive = get_alive_instance_respawn_group_monster_count( 30000, gv('layer'), 1 )
		local sub_boss2_alive = get_alive_instance_respawn_group_monster_count( 30000, gv('layer'), 2 )

		if sub_boss1_alive == 0 and sub_boss2_alive == 0 then

			--모두 제거했는데 입장할래? 다이얼로그                                    
			dlg_special( 'confirm_window', 'warp_to_cubric_boss_room()', '@9828' )

		else 
			--모두 제거해야만 들어갈 수 있다는 다이얼로그
			dlg_general( '@9827' )
			return
		end
		
	-- 9번방 입장
	elseif prop_id == 60145 then
		-- 입구10-1
		warp_to_cubric_branch_room( 2 )

	-- 9번방 입장
	elseif prop_id == 60144 then
		-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	
		if get_quest_progress(3606) == 1 then -- <(version:7.4)>[큐브릭]탈출 시도 #2		
			set_quest_status( 3606, 1, 1 )
			cprint( "@90605055" ) -- <(version:7.4)><#6DD66D>바닥이 진동합니다.			
		else
			warp(99018 , 30995, gv('layer') ) -- 입구9-1
		end
	
	-- 중간보스(아카샤그린) 제거후 나타나는 포탈
	elseif prop_id == 60155 then
		dlg_special( 'confirm_window', 'warp_to_cubric_branch_room( 1 )', '@9829' ) -- 9번방으로 입장

	-- 중간보스(아카샤사이안) 제거후 나타나는 포탈
	elseif prop_id == 60156 then
		dlg_special( 'confirm_window', 'warp_to_cubric_branch_room( 2 )', '@9829' ) -- 9번방으로 입장

--==========================================에픽 9.1 붉은 거미 서커스단 포털
	elseif prop_id == 60192 then -- 붉은거미 서커스

		local level = get_value( 'level' )

		if level >= 150 then
			dlg_special( 'confirm_window', 'warp(35692 , 120963 )', '@9852' )  -- 붉은거미 서커스 지역으로 입장하시겠습니까?		
		elseif level < 150 then
			cprint( "@9254" )		-- 150레벨 이하는 입장할 수 없습니다.
		end		
	end
end

function parallelworld_exit()	
	if get_quest_progress(4009) == 1 then -- [일일]틈새조사 : 타임어택
		dlg_special( 'confirm_window', 'exit_indun( 0 )', '@9911' )
	else
		dlg_special( 'confirm_window', 'exit_indun( 0 )', '@9906' )
	end
end

function quest_start_4009()	
	delete_item( get_item_handle( 2016026 ), 1 )
	warp_to_instance_dungeon(70000)
end

function on_create_parallelworld_instance( dungeon_id, layer )
	local state_code = get_local_info()  -- 국가코드
	local current_time = get_os_date( "%Y-%m-%d %H:%M:%S" ) -- 현재 시간
	
	-- check 가 1이 되야 이벤트 리스폰. 수정체 소환 이벤트
	local check = 0
	
	-- 북미 이벤트 (2월 1일 ~ 9월 29일) : 
	if ( state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 )
		and '2017-09-12 08:00:00' <= current_time and current_time < '2017-10-24 08:00:00' then
		check = 1
	elseif state_code == 256 and '2016-02-29 08:00:00' <= current_time and current_time < '2016-03-21 08:00:00' then
		check = 1
	end
	
	if dungeon_id == 60000 then
		add_instance_dungeon_monster( 60001, 60000, layer )
		add_instance_dungeon_monster( 60002, 60000, layer )
		add_instance_dungeon_monster( 60003, 60000, layer )
		add_instance_dungeon_monster( 60004, 60000, layer )
		add_instance_dungeon_monster( 60005, 60000, layer )
				
		-- 수정체 소환
		if check == 1 then
			add_instance_dungeon_monster( 60051, 60000, layer )
		end
		
	elseif dungeon_id == 70000 then
		broadcast_notice( 1, "@9915", 70000, layer )

		add_instance_dungeon_monster( 70001, 70000, layer )
		add_instance_dungeon_monster( 70002, 70000, layer )
		add_instance_dungeon_monster( 70003, 70000, layer )
		add_instance_dungeon_monster( 70004, 70000, layer )
		add_instance_dungeon_monster( 70005, 70000, layer )
		
		-- 수정체 소환
		if check == 1 then
			add_instance_dungeon_monster( 70051, 70000, layer )
		end
	end
end

function parallelworld_check_respawn_group_clear_1( respawn_group, x, y, layer )
	
	local left_monster_count = get_alive_instance_respawn_group_monster_count( 60000, layer, respawn_group ) -- 패러렐 월드<1>
	
-- 카운트 처리
		if left_monster_count == 0 then
			if respawn_group == 60001 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 1 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60006, 60000, layer )
				add_instance_dungeon_monster( 60007, 60000, layer )
				add_instance_dungeon_monster( 60008, 60000, layer )
				add_instance_dungeon_monster( 60009, 60000, layer )
				add_instance_dungeon_monster( 60010, 60000, layer )
				
			elseif respawn_group == 60006 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 2 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60011, 60000, layer )
				add_instance_dungeon_monster( 60012, 60000, layer )
				add_instance_dungeon_monster( 60013, 60000, layer )
				add_instance_dungeon_monster( 60014, 60000, layer )
				add_instance_dungeon_monster( 60015, 60000, layer )
				
			elseif respawn_group == 60011 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 3 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60016, 60000, layer )
				add_instance_dungeon_monster( 60017, 60000, layer )
				add_instance_dungeon_monster( 60018, 60000, layer )
				add_instance_dungeon_monster( 60019, 60000, layer )
				add_instance_dungeon_monster( 60020, 60000, layer )
				
			elseif respawn_group == 60016 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 4 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60021, 60000, layer )
				add_instance_dungeon_monster( 60022, 60000, layer )
				add_instance_dungeon_monster( 60023, 60000, layer )
				add_instance_dungeon_monster( 60024, 60000, layer )
				add_instance_dungeon_monster( 60025, 60000, layer )
				
			elseif respawn_group == 60021 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 5 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60026, 60000, layer )
				add_instance_dungeon_monster( 60027, 60000, layer )
				add_instance_dungeon_monster( 60028, 60000, layer )
				add_instance_dungeon_monster( 60029, 60000, layer )
				add_instance_dungeon_monster( 60030, 60000, layer )
				
			elseif respawn_group == 60026 then
				broadcast_notice( 1, "@9913", 60000, layer )
			end
		end
end

function parallelworld_check_respawn_group_clear_2( respawn_group, x, y, layer )

	local left_monster_count = get_alive_instance_respawn_group_monster_count( 70000, layer, respawn_group ) -- 패러렐 월드<2>
	
-- 카운트 처리
		if left_monster_count == 0 then
			if respawn_group == 70001 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 1 ) )
				broadcast_notice( 1, text, 70000, layer )
				do_each_player_in_instance_dungeon( 70000, layer, "set_quest_status( 4009, 1, 1 )" )
				
				add_instance_dungeon_monster( 70006, 70000, layer )
				add_instance_dungeon_monster( 70007, 70000, layer )
				add_instance_dungeon_monster( 70008, 70000, layer )
				add_instance_dungeon_monster( 70009, 70000, layer )
				add_instance_dungeon_monster( 70010, 70000, layer )
				
			elseif respawn_group == 70006 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 2 ) )
				broadcast_notice( 1, text, 70000, layer )
				do_each_player_in_instance_dungeon( 70000, layer, "set_quest_status( 4009, 2, 1 )" )
				
				add_instance_dungeon_monster( 70011, 70000, layer )
				add_instance_dungeon_monster( 70012, 70000, layer )
				add_instance_dungeon_monster( 70013, 70000, layer )
				add_instance_dungeon_monster( 70014, 70000, layer )
				add_instance_dungeon_monster( 70015, 70000, layer )
				
			elseif respawn_group == 70011 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 3 ) )
				broadcast_notice( 1, text, 70000, layer )
				do_each_player_in_instance_dungeon( 70000, layer, "set_quest_status( 4009, 3, 1 )" )
				
				add_instance_dungeon_monster( 70016, 70000, layer )
				add_instance_dungeon_monster( 70017, 70000, layer )
				add_instance_dungeon_monster( 70018, 70000, layer )
				add_instance_dungeon_monster( 70019, 70000, layer )
				add_instance_dungeon_monster( 70020, 70000, layer )
				
			elseif respawn_group == 70016 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 4 ) )
				broadcast_notice( 1, text, 70000, layer )
				do_each_player_in_instance_dungeon( 70000, layer, "set_quest_status( 4009, 4, 1 )" )
				
				add_instance_dungeon_monster( 70021, 70000, layer )
				add_instance_dungeon_monster( 70022, 70000, layer )
				add_instance_dungeon_monster( 70023, 70000, layer )
				add_instance_dungeon_monster( 70024, 70000, layer )
				add_instance_dungeon_monster( 70025, 70000, layer )
				
			elseif respawn_group == 70021 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 5 ) )
				broadcast_notice( 1, text, 70000, layer )
				do_each_player_in_instance_dungeon( 70000, layer, "set_quest_status( 4009, 5, 1 )" )
				
				add_instance_dungeon_monster( 70026, 70000, layer )
				add_instance_dungeon_monster( 70027, 70000, layer )
				add_instance_dungeon_monster( 70028, 70000, layer )
				add_instance_dungeon_monster( 70029, 70000, layer )
				add_instance_dungeon_monster( 70030, 70000, layer )
				
			elseif respawn_group == 70026 then
				broadcast_notice( 1, "@9914", 70000, layer )
				do_each_player_in_instance_dungeon( 70000, layer, "set_quest_status( 4009, 6, 1 )" )
			end
		end
end

function on_leave_vulcanus( layer )

	
end

--=================================================================================

function enter_to_secret_dungeon( prop_id )

	if prop_id == 120291 then

		-- 숨겨진 팔미르 유적 ID 70120201
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(120201)', '@728\v#@dungeon_name@#\v@70120201' )
	
	elseif prop_id == 100191 then

		-- 숨겨진 백룡의 쉼터 ID 70100101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(100101)', '@728\v#@dungeon_name@#\v@70100101' )

	elseif prop_id == 90191 then

		-- 숨겨진 흑룡의 그늘 ID 70090101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(90101)', '@728\v#@dungeon_name@#\v@70090101' )

	elseif prop_id == 80191 then

		-- 숨겨진 사룡의 심장 ID 70080101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(80101)', '@728\v#@dungeon_name@#\v@70080101' )

	elseif prop_id == 110191 then
 
		-- 숨겨진 엘카시아 ID 70110101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(110101)', '@728\v#@dungeon_name@#\v@70110101' )

	elseif prop_id == 70191 then

		-- 숨겨진 수정계곡 ID 70070101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(70101)', '@728\v#@dungeon_name@#\v@70070101' )

	else

	end

end


--------------------------------------------------------------------------------------------------------------------------
function exit_instance_dungeon( prop_id )

	if prop_id == 126027 then
		dlg_special( 'confirm_window', 'exit_indun(100101)', '@9807\v#@dungeon_name@#\v@80020000' )	-- 불카누스 던전을 나가겠습니까?
	elseif prop_id == 126023 then
		dlg_special( 'confirm_window', 'exit_indun(100101)', '@9807\v#@dungeon_name@#\v@80020000' )	-- 불카누스 던전을 나가겠습니까?
	elseif prop_id == 60154 or prop_id == 60102 then
		dlg_special( 'confirm_window', 'exit_indun( 30000 )', '@9832\v#@dungeon_name@#\v@80030000' ) -- 30000 인던 큐브릭 ID
	elseif prop_id == 60166 then
		dlg_special( 'confirm_window', 'exit_indun( 40000 )', '@9832\v#@dungeon_name@#\v@80040000' ) -- 메마른 달빛 지하기지 출구		--warp(133066 , 87015 )
	elseif prop_id == 60167 then
		dlg_special( 'confirm_window', 'exit_indun( 41001 )', '@9832\v#@dungeon_name@#\v@80041001' ) -- 발모어 탄광 지하기지 출구		--warp(155868 , 103644 )
	elseif prop_id == 60168 then
		dlg_special( 'confirm_window', 'exit_indun( 42001 )', '@9832\v#@dungeon_name@#\v@80042001' ) -- 수정계곡 지하기지 출구		--warp(103282 , 100401 )
	elseif prop_id == 60169 then 
		dlg_special( 'confirm_window', 'exit_indun( 43001 )', '@9832\v#@dungeon_name@#\v@80043001' ) -- 팔미르 지하기지 출구		--warp(132695 , 128105 )
	elseif prop_id == 60191 then 
		dlg_special( 'confirm_window', 'exit_indun( 50000 )', '@9853\v#@dungeon_name@#\v@80050000' ) -- 서커스 출구		--warp(132695 , 128105 )
	else

	end
end

--------------------------------------------------------------------------------------------------------------------------
function exit_indun_confirm()
	dlg_special( 'confirm_window', 'exit_indun( 0 )', '@9920' )
end

function exit_indun( dungeon_id )
	local hx
	local hy
	if (gv('hx') == nil or gv('hx') == "") and (gv('hy') == nil or gv('hy') == "") then -- 해외	
		hx = get_flag( 'hx' )
		hy = get_flag( 'hy' )
	else		-- 국내
		hx = gv( 'hx' )
		hy = gv( 'hy' )
	end
	warp( hx, hy, 0 )
end




---------------------------------------------------------------------------------------
--  큐브릭
---------------------------------------------------------------------------------------
-- 용도: 큐브릭 인스턴스 던전이 생성될 때마다 호출되는 함수
-- 역할: 큐브릭 던전의 진행상황 정보 초기값 세팅
function on_create_cubric_instance( layer )

	-- 큐브릭 인스턴스 던전 ID: 30000
	set_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount', 0 )	-- 인던 진행 퀘스트 체크 대상인 3607 퀘스트를 진행 중인 유저 수 체크용
	set_instance_dungeon_flag( 30000, layer, 'AdditionalBossRespawned', 0 )	-- 인던 진행 퀘스트로 인해 추가 보스몹들의 리젠 여부 체크용(중첩 리젠 방지용)
	set_instance_dungeon_flag( 30000, layer, 'ExitPropRespawned', 0 )		-- 인던 최종 출구 프랍 리젠 여부 체크용(중첩 리젠 방지용)
	set_instance_dungeon_flag( 30000, layer, 'MissionRevealed', 0 )			-- 아크샤 두 마리 잡으라는 미션을 유저들에게 보여줬는지 체크용(0:활성화 전, 1:활성화 중, 2:미션 완료)

end


-- 용도: 큐브릭 인스턴스 던전에 유저가 참여할 때마다 호출되는 함수
-- 역할: 방금 참여한 유저가 체크 대상인 퀘스트를 보유하고 있었을 경우 던전 내부 체크용 변수 세팅
function on_join_cubric( layer )

	-- 방금 진입한 유저가 3607 퀘스트를 진행 중이라면 던전 상태 변수에 반영
	-- * 이전에 퀘 받아놓고 완료 조건 못 채우고 나갔던 유저면 반영하지만
	--   완료 조건이 달성되어 있는 유저라면 퀘스트를 진행하고 있는 상태라고 볼 수 없으므로 반영하지 않음
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	if get_quest_progress( 3607 ) == 1 then

		-- 큐브릭 인스턴스 던전 ID: 30000
		set_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount', get_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount' ) + 1 )

	end

	-- 미션 UI가 아직 활성화된 이후에 입장하는 거라면 미션 UI 표시
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) == 1 then

		-- 미션 제목
		send_mission_title( '@1234' )

		-- 0번 미션 목표(서쪽의 아크샤)
		send_mission_objective( 0, 1, '@1235' )
		-- 1번 미션 목표(동쪽의 아크샤)
		send_mission_objective( 1, 1, '@1236' )
		-- 2번 미션 목표(보스방 드가세요)
		send_mission_objective( 2, 1, '@1237' )

		-- 미션 보상 안내
		send_mission_reward( '@1238' )

		-- 미션 목표 진행 상태
		update_cubric_mission( false, layer )

	end

end


-- 용도: 큐브릭 인스턴스 던전에서 유저가 이탈할 때마다 호출되는 함수
-- 역할: 방금 이탈한 유저가 체크 대상인 퀘스트를 보유하고 있었을 경우 던전 내부 체크용 변수 세팅
function on_leave_cubric( layer )

	-- 방금 이탈한 유저가 3607 퀘스트를 진행 중이라면 던전 상태 변수에 반영
	-- * 퀘 받아놓고 완료 조건 못 채우고 나가는 유저면 반영하지만
	--   완료 조건이 달성되어 있는 유저라면 퀘스트를 진행 중인 상태라고 볼 수 없으므로 반영하지 않음
	--   (예. 퀘 받고 몹 죽여서 보상 받으면 되지만 보상만 안 받고 나간 경우)
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	if get_quest_progress( 3607 ) == 1 then

		-- 큐브릭 인스턴스 던전 ID: 30000
		set_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount', get_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount' ) - 1 )

	end

	-- 미션 UI가 활성화 된 이후였으면 미션 UI 닫아주기
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) == 1 then

		send_mission_title( '' )

	end

end


-- 용도: 몬스터 죽을 때마다 호출되는 함수
-- 역할: 죽은 몬스터의 리스폰 그룹에 남은 몬스터 수를 체크하여 모두 죽었을 프랍을 열어 주거나 아무 것도 하지 않거나 함
function cubric_check_respawn_group_clear( respawn_group, x, y, layer )
	if respawn_group == 1 or respawn_group == 2 then			-- 부 보스 그룹으로 리젠된 몬스터가 죽은 경우
		add_field_prop( 60156, 0, x, y, layer, 0 )
		update_cubric_mission( true, layer )
	elseif respawn_group == 3 then
		-- 보스는 한 마리밖에 리젠 안 되기 때문에 모두 죽었는지 체크할 필요 없음
		-- 큐브릭 던전 진행 퀘스트가 진행 중일 경우에는 밖으로 출구 프랍을 생성하지 않고,
		-- 퀘스트가 진행 중인 유저가 하나도 없다면 그냥 밖으로 나갈 수 있도록 포탈 열어 줌
		-- 진행 중인지 판별하는 퀘스트는 [ID:3607 <(version:7.4)>[큐브릭]악마 큐브]를 사용(QuestResource.script_start_text, script_end_text, script_drop_text를 이용하여 세팅함)
		if get_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount' ) == 0 then
			add_field_prop( 60102, 0, 99954, 31302, layer, 0 ) -- 출구 프랍 99954, 31302 로 옮겨주면 좋겠다. 기존 좌표 99954, 31539
			set_instance_dungeon_flag( 30000, layer, 'ExitPropRespawned', 1 )
			add_field_prop( 9001, 0, 100180, 31767, layer, -3, 0, 0, 0, 1, 1, 1 )		--보스 생성 프랍			
			broadcast_notice( 1, "@90610144", 30000, layer ) -- 프랍 클릭하면 보스 나온다는 메세지?
		end
	end
end


-- 용도: 아크샤 잡고 생성된 포탈 클릭 시 호출되는 함수
-- 역할: 중앙의 갈림길 방으로 플레이어를 워프시키면서 아크샤를 두 마리 잡아야 한다는 미션 UI를 활성화시켜 줌
function warp_to_cubric_branch_room( side )

	local layer = gv( 'layer' )
	if side == 1 then
		warp( 98858, 30827, layer )			-- 중앙의 방으로 워프시켜 줌
	elseif side == 2 then
		warp( 99180, 30825, layer )
	end
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) == 0 then
		set_instance_dungeon_flag( 30000, layer, 'MissionRevealed', 1 )
		broadcast_mission_title( 1, '@1234', 30000, layer )		
		broadcast_mission_objective( 1, 0, 1, '@1235', 30000, layer )	-- 0번 미션 목표(서쪽의 아크샤)
		broadcast_mission_objective( 1, 1, 1, '@1236', 30000, layer )	-- 1번 미션 목표(동쪽의 아크샤)
		broadcast_mission_objective( 1, 2, 1, '@1237', 30000, layer )	-- 2번 미션 목표(보스방 드가세요)
		broadcast_mission_reward( 1, '@1238', 30000, layer )			-- 미션 보상 안내
		update_cubric_mission( true, layer )

	end

end


-- 용도: 아크샤 두 마리 다 잡고 보스 방으로 들어가는 문 클릭
-- 역할: 보스 방 내부로 플레이어를 워프시키면서 미션 UI를 제거시켜 줌
function warp_to_cubric_boss_room()

	local layer = gv( 'layer' )
	-- 보스 방 내부로 워프
	warp( 99711, 31535, layer )
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) == 1 then
		-- 미션 완료
		broadcast_mission_objective_progress( 1, 2, 1, 30000, layer )
		-- 미션 보상 지급
		do_each_player_in_instance_dungeon( 30000, layer, 'cubric_reward_distributer()' )
		-- 미션 UI 더 이상 활성화되지 않게 플래그 값 변경
		set_instance_dungeon_flag( 30000, layer, 'MissionRevealed', 2 )
		-- 미션 UI 비활성화
		broadcast_mission_title( 1, '', 30000, layer )
	end
end

function cubric_reward_distributer()

	--퀘스트 보상 Lv 150, 1% 수준
	insert_gold( 300000 )
	add_exp_jp( 5131363, 884717 )
	cprint( '@1239' )

end


-- 용도: 미션 UI를 업데이트 해야 하는 몇몇 경우에 호출(부보스 사망, 유저 입장 등)
-- 역할: 미션 UI를 업데이트해야 하는 이벤트 발생 시 방 안의 모든 유저 또는 현재 스크립트 실행 주체 유저에게 미션 내용 알려주기
function update_cubric_mission( broadcast, layer )
	-- 미션이 진행 중이지 않다면 딱히 알려줄 것도 없음
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) ~= 1 then
		return
	end
	if broadcast == true then
		broadcast_mission_objective_progress( 1, 0, 1 - get_alive_instance_respawn_group_monster_count( 30000, layer, 1 ), 30000, layer )
		broadcast_mission_objective_progress( 1, 1, 1 - get_alive_instance_respawn_group_monster_count( 30000, layer, 2 ), 30000, layer)
		broadcast_mission_objective_progress( 1, 2, 0, 30000, layer )
	else
		send_mission_objective_progress( 0, 1 - get_alive_instance_respawn_group_monster_count( 30000, layer, 1 ) )
		send_mission_objective_progress( 1, 1 - get_alive_instance_respawn_group_monster_count( 30000, layer, 2 ) )
		send_mission_objective_progress( 2, 0 )
	end
end








function Call_BossMob( monster_handle )

	local monster_id = get_monster_id( monster_handle )
	local layer = ghv( monster_handle, 'layer' )

	local quest_progress3644 = get_quest_progress(3644) -- [일일]던전 토벌: 메두사 사냥
	local quest_progress3645 = get_quest_progress(3645) -- [일일]던전 토벌: 블랙 위도우 사냥
	local quest_progress3646 = get_quest_progress(3646) -- [일일]던전 토벌: 본 드래곤 사냥
	local quest_progress3647 = get_quest_progress(3647) -- [일일]던전 토벌: 미크로랍토르 사냥
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local X_pos = gv('x')
	local Y_pos = gv('y')
	
--==========================================================================1단계
	if monster_id == 20190005 then -- 바질리스크<1단계>
		
		add_npc( 38871, 22943, 20190006, 1, 0, layer )
		add_npc( 38906, 23013, 20190001, 1, 0, layer )
		add_npc( 39215, 22959, 20190003, 1, 0, layer )
		add_npc( 39138, 22881, 20190003, 1, 0, layer )
		add_npc( 39087, 23021, 20190003, 1, 0, layer )
		add_npc( 39067, 22964, 20190003, 1, 0, layer )
		add_npc( 38980, 22920, 20190003, 1, 0, layer )
		add_npc( 28980, 23005, 20190003, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 40000, layer, "set_quest_status( 3644, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3644 == 1 then
			
			if get_quest_status( 3644 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 3 ) + 1
				set_quest_status( 3644, 3, A_day_A_quest )
			end
		end ]]
	        
	elseif monster_id == 20190007 then -- 타란튤라<1단계>
		
		add_npc( 40715, 9563, 20190008, 1, 0, layer )
		add_npc( 40497, 9586, 20190025, 1, 0, layer )
		add_npc( 40763, 9623, 20190004, 1, 0, layer )
		add_npc( 40761, 9591, 20190004, 1, 0, layer )
		add_npc( 40765, 9526, 20190004, 1, 0, layer )
		add_npc( 40697, 9492, 20190004, 1, 0, layer )
		add_npc( 40692, 9547, 20190004, 1, 0, layer )
		add_npc( 40709, 9596, 20190004, 1, 0, layer )
		add_npc( 40715, 9618, 20190004, 1, 0, layer )
		add_npc( 40718, 9660, 20190004, 1, 0, layer )
		add_npc( 40642, 9655, 20190004, 1, 0, layer )
		add_npc( 40623, 9589, 20190004, 1, 0, layer )
		add_npc( 40632, 9543, 20190004, 1, 0, layer )
		add_npc( 40639, 9498, 20190004, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 41001, layer, "set_quest_status( 3645, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3645 == 1 then
			
			if get_quest_status( 3645 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 3 ) + 1
				set_quest_status( 3645, 3, A_day_A_quest )
			end
		end ]]
	
	elseif monster_id == 20190009 then -- 에우오플로케팔루스<1단계>
		
		add_npc( 61203, 30845, 20190012, 1, 0, layer )
		add_npc( 61360, 30953, 20190026, 1, 0, layer )
		add_npc( 61254, 30888, 20190002, 1, 0, layer )
		add_npc( 61107, 30905, 20190002, 1, 0, layer )
		add_npc( 60838, 30876, 20190002, 1, 0, layer )
		add_npc( 60725, 30763, 20190002, 1, 0, layer )
		add_npc( 60794, 30659, 20190002, 1, 0, layer )
		add_npc( 61050, 30627, 20190002, 1, 0, layer )
		add_npc( 60866, 31022, 20190002, 1, 0, layer )
		add_npc( 60770, 30926, 20190002, 1, 0, layer )
		add_npc( 60640, 30799, 20190002, 1, 0, layer )
		add_npc( 60654, 30696, 20190002, 1, 0, layer )
		add_npc( 60517, 30887, 20190002, 1, 0, layer )
		add_npc( 61129, 30766, 20190002, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 42001, layer, "set_quest_status( 3646, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3646 == 1 then
			
			if get_quest_status( 3646 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 3 ) + 1
				set_quest_status( 3646, 3, A_day_A_quest )
			end
		end ]]

		
	elseif monster_id == 20190011 then -- 블러드 슬라임<1단계>

		add_npc( 53668, 4378, 20190010, 1, 0, layer )
		add_npc( 53568, 4384, 20190027, 1, 0, layer )
		add_npc( 54092, 4663, 20190003, 1, 0, layer )
		add_npc( 54073, 4648, 20190003, 1, 0, layer )
		add_npc( 54074, 4534, 20190003, 1, 0, layer )
		add_npc( 54019, 4622, 20190003, 1, 0, layer )
		add_npc( 53969, 4714, 20190003, 1, 0, layer )
		add_npc( 53931, 4595, 20190003, 1, 0, layer )
		add_npc( 53831, 4524, 20190004, 1, 0, layer )
		add_npc( 53757, 4558, 20190004, 1, 0, layer )
		add_npc( 53661, 4569, 20190004, 1, 0, layer )
		add_npc( 53583, 4490, 20190004, 1, 0, layer )
		add_npc( 53638, 4391, 20190004, 1, 0, layer )
		add_npc( 53700, 4573, 20190004, 1, 0, layer )
		add_npc( 53766, 4572, 20190002, 1, 0, layer )
		add_npc( 54062, 4425, 20190002, 1, 0, layer )
		add_npc( 54172, 4572, 20190002, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 43001, layer, "set_quest_status( 3647, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3647 == 1 then
			
			if get_quest_status( 3647 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 3 ) + 1
				set_quest_status( 3647, 3, A_day_A_quest )
			end
		end ]]
	end	

--==========================================================================2단계	
	if monster_id == 20190032 then -- 바질리스크<2단계>
		
		add_npc( 38871, 22943, 20190033, 1, 0, layer )
		add_npc( 38906, 23013, 20190028, 1, 0, layer )
		add_npc( 39215, 22959, 20190030, 1, 0, layer )
		add_npc( 39138, 22881, 20190030, 1, 0, layer )
		add_npc( 39087, 23021, 20190030, 1, 0, layer )
		add_npc( 39067, 22964, 20190030, 1, 0, layer )
		add_npc( 38980, 22920, 20190030, 1, 0, layer )
		add_npc( 28980, 23005, 20190030, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 40000, layer, "set_quest_status( 3644, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3644 == 1 then
			
			if get_quest_status( 3644 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 3 ) + 1
				set_quest_status( 3644, 3, A_day_A_quest )
			end
		end ]]
	        
	elseif monster_id == 20190034 then -- 타란튤라<2단계>
		
		add_npc( 40715, 9563, 20190035, 1, 0, layer )
		add_npc( 40497, 9586, 20190052, 1, 0, layer )
		add_npc( 40763, 9623, 20190031, 1, 0, layer )
		add_npc( 40761, 9591, 20190031, 1, 0, layer )
		add_npc( 40765, 9526, 20190031, 1, 0, layer )
		add_npc( 40697, 9492, 20190031, 1, 0, layer )
		add_npc( 40692, 9547, 20190031, 1, 0, layer )
		add_npc( 40709, 9596, 20190031, 1, 0, layer )
		add_npc( 40715, 9618, 20190031, 1, 0, layer )
		add_npc( 40718, 9660, 20190031, 1, 0, layer )
		add_npc( 40642, 9655, 20190031, 1, 0, layer )
		add_npc( 40623, 9589, 20190031, 1, 0, layer )
		add_npc( 40632, 9543, 20190031, 1, 0, layer )
		add_npc( 40639, 9498, 20190031, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 41001, layer, "set_quest_status( 3645, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3645 == 1 then
			
			if get_quest_status( 3645 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 3 ) + 1
				set_quest_status( 3645, 3, A_day_A_quest )
			end
		end ]]
		
	elseif monster_id == 20190036 then -- 에우오플로케팔루스<2단계>
		
		add_npc( 61203, 30845, 20190039, 1, 0, layer )
		add_npc( 61360, 30953, 20190053, 1, 0, layer )
		add_npc( 61254, 30888, 20190029, 1, 0, layer )
		add_npc( 61107, 30905, 20190029, 1, 0, layer )
		add_npc( 60838, 30876, 20190029, 1, 0, layer )
		add_npc( 60725, 30763, 20190029, 1, 0, layer )
		add_npc( 60794, 30659, 20190029, 1, 0, layer )
		add_npc( 61050, 30627, 20190029, 1, 0, layer )
		add_npc( 60866, 31022, 20190029, 1, 0, layer )
		add_npc( 60770, 30926, 20190029, 1, 0, layer )
		add_npc( 60640, 30799, 20190029, 1, 0, layer )
		add_npc( 60654, 30696, 20190029, 1, 0, layer )
		add_npc( 60517, 30887, 20190029, 1, 0, layer )
		add_npc( 61129, 30766, 20190029, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 42001, layer, "set_quest_status( 3646, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3646 == 1 then
			
			if get_quest_status( 3646 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 3 ) + 1
				set_quest_status( 3646, 3, A_day_A_quest )
			end
		end ]]
		
	elseif monster_id == 20190038 then -- 블러드 슬라임<2단계>

		add_npc( 53668, 4378, 20190037, 1, 0, layer )
		add_npc( 53568, 4384, 20190054, 1, 0, layer )
		add_npc( 54092, 4663, 20190030, 1, 0, layer )
		add_npc( 54073, 4648, 20190030, 1, 0, layer )
		add_npc( 54074, 4534, 20190030, 1, 0, layer )
		add_npc( 54019, 4622, 20190030, 1, 0, layer )
		add_npc( 53969, 4714, 20190030, 1, 0, layer )
		add_npc( 53931, 4595, 20190030, 1, 0, layer )
		add_npc( 53831, 4524, 20190031, 1, 0, layer )
		add_npc( 53757, 4558, 20190031, 1, 0, layer )
		add_npc( 53661, 4569, 20190031, 1, 0, layer )
		add_npc( 53583, 4490, 20190031, 1, 0, layer )
		add_npc( 53638, 4391, 20190031, 1, 0, layer )
		add_npc( 53700, 4573, 20190031, 1, 0, layer )
		add_npc( 53766, 4572, 20190029, 1, 0, layer )
		add_npc( 54062, 4425, 20190029, 1, 0, layer )
		add_npc( 54172, 4572, 20190029, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 43001, layer, "set_quest_status( 3647, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3647 == 1 then
			
			if get_quest_status( 3647 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 3 ) + 1
				set_quest_status( 3647, 3, A_day_A_quest )
			end
		end ]]
	end
	
--==========================================================================3단계	
	if monster_id == 20190059 then -- 바질리스크<3단계>
		
		add_npc( 38871, 22943, 20190060, 1, 0, layer )
		add_npc( 38906, 23013, 20190055, 1, 0, layer )
		add_npc( 39215, 22959, 20190057, 1, 0, layer )
		add_npc( 39138, 22881, 20190057, 1, 0, layer )
		add_npc( 39087, 23021, 20190057, 1, 0, layer )
		add_npc( 39067, 22964, 20190057, 1, 0, layer )
		add_npc( 38980, 22920, 20190057, 1, 0, layer )
		add_npc( 28980, 23005, 20190057, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 40000, layer, "set_quest_status( 3644, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3644 == 1 then
			
			if get_quest_status( 3644 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 3 ) + 1
				set_quest_status( 3644, 3, A_day_A_quest )
			end
		end ]]
	        
	elseif monster_id == 20190061 then -- 타란튤라<3단계>
		
		add_npc( 40715, 9563, 20190062, 1, 0, layer )
		add_npc( 40497, 9586, 20190079, 1, 0, layer )
		add_npc( 40763, 9623, 20190058, 1, 0, layer )
		add_npc( 40761, 9591, 20190058, 1, 0, layer )
		add_npc( 40765, 9526, 20190058, 1, 0, layer )
		add_npc( 40697, 9492, 20190058, 1, 0, layer )
		add_npc( 40692, 9547, 20190058, 1, 0, layer )
		add_npc( 40709, 9596, 20190058, 1, 0, layer )
		add_npc( 40715, 9618, 20190058, 1, 0, layer )
		add_npc( 40718, 9660, 20190058, 1, 0, layer )
		add_npc( 40642, 9655, 20190058, 1, 0, layer )
		add_npc( 40623, 9589, 20190058, 1, 0, layer )
		add_npc( 40632, 9543, 20190058, 1, 0, layer )
		add_npc( 40639, 9498, 20190058, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 41001, layer, "set_quest_status( 3645, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3645 == 1 then
			
			if get_quest_status( 3645 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 3 ) + 1
				set_quest_status( 3645, 3, A_day_A_quest )
			end
		end ]]
	
	elseif monster_id == 20190063 then -- 에우오플로케팔루스<3단계>
		
		add_npc( 61203, 30845, 20190066, 1, 0, layer )
		add_npc( 61360, 30953, 20190080, 1, 0, layer )
		add_npc( 61254, 30888, 20190056, 1, 0, layer )
		add_npc( 61107, 30905, 20190056, 1, 0, layer )
		add_npc( 60838, 30876, 20190056, 1, 0, layer )
		add_npc( 60725, 30763, 20190056, 1, 0, layer )
		add_npc( 60794, 30659, 20190056, 1, 0, layer )
		add_npc( 61050, 30627, 20190056, 1, 0, layer )
		add_npc( 60866, 31022, 20190056, 1, 0, layer )
		add_npc( 60770, 30926, 20190056, 1, 0, layer )
		add_npc( 60640, 30799, 20190056, 1, 0, layer )
		add_npc( 60654, 30696, 20190056, 1, 0, layer )
		add_npc( 60517, 30887, 20190056, 1, 0, layer )
		add_npc( 61129, 30766, 20190056, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 42001, layer, "set_quest_status( 3646, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3646 == 1 then
			
			if get_quest_status( 3646 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 3 ) + 1
				set_quest_status( 3646, 3, A_day_A_quest )
			end
		end ]]
		
	elseif monster_id == 20190065 then -- 블러드 슬라임<3단계>

		add_npc( 53668, 4378, 20190064, 1, 0, layer )
		add_npc( 53568, 4384, 20190081, 1, 0, layer )
		add_npc( 54092, 4663, 20190057, 1, 0, layer )
		add_npc( 54073, 4648, 20190057, 1, 0, layer )
		add_npc( 54074, 4534, 20190057, 1, 0, layer )
		add_npc( 54019, 4622, 20190057, 1, 0, layer )
		add_npc( 53969, 4714, 20190057, 1, 0, layer )
		add_npc( 53931, 4595, 20190057, 1, 0, layer )
		add_npc( 53831, 4524, 20190058, 1, 0, layer )
		add_npc( 53757, 4558, 20190058, 1, 0, layer )
		add_npc( 53661, 4569, 20190058, 1, 0, layer )
		add_npc( 53583, 4490, 20190058, 1, 0, layer )
		add_npc( 53638, 4391, 20190058, 1, 0, layer )
		add_npc( 53700, 4573, 20190058, 1, 0, layer )
		add_npc( 53766, 4572, 20190056, 1, 0, layer )
		add_npc( 54062, 4425, 20190056, 1, 0, layer )
		add_npc( 54172, 4572, 20190056, 1, 0, layer )
		
		
		do_each_player_in_instance_dungeon( 43001, layer, "set_quest_status( 3647, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3647 == 1 then
			
			if get_quest_status( 3647 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 3 ) + 1
				set_quest_status( 3647, 3, A_day_A_quest )
			end
		end ]]
	end
	

--==========================================================================4단계	
	if monster_id == 20190086 then -- 바질리스크<4단계>
		
		add_npc( 38871, 22943, 20190087, 1, 0, layer )
		add_npc( 38906, 23013, 20190082, 1, 0, layer )
		add_npc( 39215, 22959, 20190084, 1, 0, layer )
		add_npc( 39138, 22881, 20190084, 1, 0, layer )
		add_npc( 39087, 23021, 20190084, 1, 0, layer )
		add_npc( 39067, 22964, 20190084, 1, 0, layer )
		add_npc( 38980, 22920, 20190084, 1, 0, layer )
		add_npc( 28980, 23005, 20190084, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 40000, layer, "set_quest_status( 3644, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3644 == 1 then
			
			if get_quest_status( 3644 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 3 ) + 1
				set_quest_status( 3644, 3, A_day_A_quest )
			end
		end ]]
	        
	elseif monster_id == 20190088 then -- 타란튤라<4단계>
		
		add_npc( 40715, 9563, 20190089, 1, 0, layer )
		add_npc( 40497, 9586, 21190106, 1, 0, layer )
		add_npc( 40763, 9623, 20190085, 1, 0, layer )
		add_npc( 40761, 9591, 20190085, 1, 0, layer )
		add_npc( 40765, 9526, 20190085, 1, 0, layer )
		add_npc( 40697, 9492, 20190085, 1, 0, layer )
		add_npc( 40692, 9547, 20190085, 1, 0, layer )
		add_npc( 40709, 9596, 20190085, 1, 0, layer )
		add_npc( 40715, 9618, 20190085, 1, 0, layer )
		add_npc( 40718, 9660, 20190085, 1, 0, layer )
		add_npc( 40642, 9655, 20190085, 1, 0, layer )
		add_npc( 40623, 9589, 20190085, 1, 0, layer )
		add_npc( 40632, 9543, 20190085, 1, 0, layer )
		add_npc( 40639, 9498, 20190085, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 41001, layer, "set_quest_status( 3645, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3645 == 1 then
			
			if get_quest_status( 3645 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 3 ) + 1
				set_quest_status( 3645, 3, A_day_A_quest )
			end
		end ]]
	
	elseif monster_id == 20190090 then -- 에우오플로케팔루스<4단계>
		
		add_npc( 61203, 30845, 20190093, 1, 0, layer )
		add_npc( 61360, 30953, 21190107, 1, 0, layer )
		add_npc( 61254, 30888, 20190083, 1, 0, layer )
		add_npc( 61107, 30905, 20190083, 1, 0, layer )
		add_npc( 60838, 30876, 20190083, 1, 0, layer )
		add_npc( 60725, 30763, 20190083, 1, 0, layer )
		add_npc( 60794, 30659, 20190083, 1, 0, layer )
		add_npc( 61050, 30627, 20190083, 1, 0, layer )
		add_npc( 60866, 31022, 20190083, 1, 0, layer )
		add_npc( 60770, 30926, 20190083, 1, 0, layer )
		add_npc( 60640, 30799, 20190083, 1, 0, layer )
		add_npc( 60654, 30696, 20190083, 1, 0, layer )
		add_npc( 60517, 30887, 20190083, 1, 0, layer )
		add_npc( 61129, 30766, 20190083, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 42001, layer, "set_quest_status( 3646, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3646 == 1 then
			
			if get_quest_status( 3646 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 3 ) + 1
				set_quest_status( 3646, 3, A_day_A_quest )
			end
		end ]]
		
	elseif monster_id == 20190092 then -- 블러드 슬라임<4단계>

		add_npc( 53668, 4378, 20190091, 1, 0, layer )
		add_npc( 53568, 4384, 21190108, 1, 0, layer )
		add_npc( 54092, 4663, 20190084, 1, 0, layer )
		add_npc( 54073, 4648, 20190084, 1, 0, layer )
		add_npc( 54074, 4534, 20190084, 1, 0, layer )
		add_npc( 54019, 4622, 20190084, 1, 0, layer )
		add_npc( 53969, 4714, 20190084, 1, 0, layer )
		add_npc( 53931, 4595, 20190084, 1, 0, layer )
		add_npc( 53831, 4524, 20190085, 1, 0, layer )
		add_npc( 53757, 4558, 20190085, 1, 0, layer )
		add_npc( 53661, 4569, 20190085, 1, 0, layer )
		add_npc( 53583, 4490, 20190085, 1, 0, layer )
		add_npc( 53638, 4391, 20190085, 1, 0, layer )
		add_npc( 53700, 4573, 20190085, 1, 0, layer )
		add_npc( 53766, 4572, 20190083, 1, 0, layer )
		add_npc( 54062, 4425, 20190083, 1, 0, layer )
		add_npc( 54172, 4572, 20190083, 1, 0, layer )
		
		do_each_player_in_instance_dungeon( 43001, layer, "set_quest_status( 3647, 3, 1 )", X_pos, Y_pos )
		
		--[[ if quest_progress3647 == 1 then
			
			if get_quest_status( 3647 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 3 ) + 1
				set_quest_status( 3647, 3, A_day_A_quest )
			end
		end ]]
	
	end	

end

function Call_Main_Chest( monster_handle )

	local monster_id = get_monster_id( monster_handle )
	local layer = ghv( monster_handle, 'layer' )
	
	local quest_progress3644 = get_quest_progress(3644) -- [일일]던전 토벌: 메두사 사냥
	local quest_progress3645 = get_quest_progress(3645) -- [일일]던전 토벌: 블랙 위도우 사냥
	local quest_progress3646 = get_quest_progress(3646) -- [일일]던전 토벌: 본 드래곤 사냥
	local quest_progress3647 = get_quest_progress(3647) -- [일일]던전 토벌: 미크로랍토르 사냥
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local X_pos = gv('x')
	local Y_pos = gv('y')
	
	if monster_id == 20190001 or monster_id == 20190006 --메던 지하기지 1단계
	or monster_id == 20190028 or monster_id == 20190033 --메던 지하기지 2단계
	or monster_id == 20190055 or monster_id == 20190060 --메던 지하기지 3단계
	or monster_id == 20190082 or monster_id == 20190087 then --메던 지하기지 4단계
	
		local monster_count = get_instance_dungeon_flag( 40000, layer, 'check_open_prop')
		if monster_count == 2 then
			set_instance_dungeon_flag( 40000, layer, 'check_open_prop', 1)
		elseif monster_count == 1 then
			add_field_prop ( 60170, 60000, 38719, 22938, layer )
			set_instance_dungeon_flag( 40000, layer, 'check_open_prop', 0)
			
			if get_instance_dungeon_type_id( 40000, layer ) == 3 then
				add_field_prop( 9002, 0, 38881, 22826, layer, -3, 0, 0, 0, 1, 1, 1 )		--보스 생성 프랍
				broadcast_notice( 1, "@90610144", 40000, layer ) -- 프랍 클릭하면 보스 나온다는 메세지?
			end
		end
		
		if monster_id == 20190001 or monster_id == 20190028 or monster_id == 20190055 or monster_id == 20190082 then
			do_each_player_in_instance_dungeon( 40000, layer, "set_quest_status( 3644, 1, 1 )", X_pos, Y_pos )
			--[[ if get_quest_status( 3644 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 1 ) + 1
				set_quest_status( 3644, 1, A_day_A_quest )
			end ]]
		--[[ elseif quest_progress3644 == 1 and monster_id == 20190028 then
			if get_quest_status( 3644 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 1 ) + 1
				set_quest_status( 3644, 1, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190055 then
			if get_quest_status( 3644 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 1 ) + 1
				set_quest_status( 3644, 1, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190082 then
			if get_quest_status( 3644 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 1 ) + 1
				set_quest_status( 3644, 1, A_day_A_quest )
			end ]]
		elseif monster_id == 20190006 or monster_id == 20190033 or monster_id == 20190060 or monster_id == 20190087 then
			do_each_player_in_instance_dungeon( 40000, layer, "set_quest_status( 3644, 2, 1 )", X_pos, Y_pos )			
			--[[ if get_quest_status( 3644 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 2 ) + 1
				set_quest_status( 3644, 2, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190033 then
			if get_quest_status( 3644 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 2 ) + 1
				set_quest_status( 3644, 2, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190060 then
			if get_quest_status( 3644 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 2 ) + 1
				set_quest_status( 3644, 2, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190087 then
			if get_quest_status( 3644 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 2 ) + 1
				set_quest_status( 3644, 2, A_day_A_quest )
			end ]]
		end
		
	elseif monster_id == 20190025 or monster_id == 20190008 --탄광 지하기지 1단계
	or monster_id == 20190052 or monster_id == 20190035 --탄광 지하기지 2단계
	or monster_id == 20190079 or monster_id == 20190062 --탄광 지하기지 3단계
	or monster_id == 21190106 or monster_id == 20190089 then --탄광 지하기지 4단계
	
		local monster_count = get_instance_dungeon_flag( 41001, layer, 'check_open_prop')
		if monster_count == 2 then
			set_instance_dungeon_flag( 41001, layer, 'check_open_prop', 1)
		elseif monster_count == 1 then
			add_field_prop ( 60171, 60000, 40379, 9573, layer )
			set_instance_dungeon_flag( 41001, layer, 'check_open_prop', 0)
			
			if get_instance_dungeon_type_id( 41001, layer ) == 3 then
				add_field_prop( 9003, 0, 40487, 9490, layer, -3, 0, 0, 0, 1, 1, 1 )		--보스 생성 프랍
				broadcast_notice( 1, "@90610144", 40001, layer ) -- 프랍 클릭하면 보스 나온다는 메세지?
			end
		end
		
		if monster_id == 20190025 or monster_id == 20190052 or monster_id == 20190079 or monster_id == 21190106 then
			do_each_player_in_instance_dungeon( 41001, layer, "set_quest_status( 3645, 1, 1 )", X_pos, Y_pos )	
			--[[ if get_quest_status( 3645 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 1 ) + 1
				set_quest_status( 3645, 1, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190052 then
			if get_quest_status( 3645 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 1 ) + 1
				set_quest_status( 3645, 1, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190079 then
			if get_quest_status( 3645 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 1 ) + 1
				set_quest_status( 3645, 1, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 21190106 then
			if get_quest_status( 3645 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 1 ) + 1
				set_quest_status( 3645, 1, A_day_A_quest )
			end ]]
		elseif monster_id == 20190008 or monster_id == 20190035 or monster_id == 20190062 or monster_id == 20190089 then
			do_each_player_in_instance_dungeon( 41001, layer, "set_quest_status( 3645, 2, 1 )", X_pos, Y_pos )
			--[[ if get_quest_status( 3645 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 2 ) + 1
				set_quest_status( 3645, 2, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190035 then
			if get_quest_status( 3645 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 2 ) + 1
				set_quest_status( 3645, 2, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190062 then
			if get_quest_status( 3645 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 2 ) + 1
				set_quest_status( 3645, 2, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190089 then
			if get_quest_status( 3645 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 2 ) + 1
				set_quest_status( 3645, 2, A_day_A_quest )
			end ]]
		end
		
	elseif monster_id == 20190026 or monster_id == 20190012 
	or monster_id == 20190053 or monster_id == 20190039 
	or monster_id == 20190080 or monster_id == 20190066 
	or monster_id == 21190107 or monster_id == 20190093 then
		
		local monster_count = get_instance_dungeon_flag( 42001, layer, 'check_open_prop')
		if monster_count == 2 then
			set_instance_dungeon_flag( 42001, layer, 'check_open_prop', 1)
		elseif monster_count == 1 then
			add_field_prop ( 60172, 60000, 61784, 31019, layer )
			set_instance_dungeon_flag( 42001, layer, 'check_open_prop', 0)
			
			if get_instance_dungeon_type_id( 42001, layer ) == 3 then
				add_field_prop( 9004, 0, 61621, 30896, layer, -3, 0, 0, 0, 1, 1, 1 )		--보스 생성 프랍
				broadcast_notice( 1, "@90610144", 42001, layer ) -- 프랍 클릭하면 보스 나온다는 메세지?
			end
		end
		
		if monster_id == 20190026 or monster_id == 20190053 or monster_id == 20190080 or monster_id == 21190107 then
			do_each_player_in_instance_dungeon( 42001, layer, "set_quest_status( 3646, 1, 1 )", X_pos, Y_pos )
			--[[ if get_quest_status( 3646 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 1 ) + 1
				set_quest_status( 3646, 1, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190053 then
			if get_quest_status( 3646 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 1 ) + 1
				set_quest_status( 3646, 1, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190080 then
			if get_quest_status( 3646 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 1 ) + 1
				set_quest_status( 3646, 1, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 21190107 then
			if get_quest_status( 3646 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 1 ) + 1
				set_quest_status( 3646, 1, A_day_A_quest )
			end ]]
		elseif monster_id == 20190012 or monster_id == 20190039 or monster_id == 20190066 or monster_id == 20190093 then
			do_each_player_in_instance_dungeon( 42001, layer, "set_quest_status( 3646, 2, 1 )", X_pos, Y_pos )
			--[[ if get_quest_status( 3646 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 2 ) + 1
				set_quest_status( 3646, 2, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190039 then
			if get_quest_status( 3646 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 2 ) + 1
				set_quest_status( 3646, 2, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190066 then
			if get_quest_status( 3646 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 2 ) + 1
				set_quest_status( 3646, 2, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190093 then
			if get_quest_status( 3646 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 2 ) + 1
				set_quest_status( 3646, 2, A_day_A_quest )
			end ]]
		end
		
	elseif monster_id == 20190027 or monster_id == 20190010 
	or monster_id == 20190054 or monster_id == 20190037 
	or monster_id == 20190081 or monster_id == 20190064 
	or monster_id == 21190108 or monster_id == 20190091 then
	
		local monster_count = get_instance_dungeon_flag( 43001, layer, 'check_open_prop')
		if monster_count == 2 then
			set_instance_dungeon_flag( 43001, layer, 'check_open_prop', 1)
		elseif monster_count == 1 then
			add_field_prop ( 60173, 60000, 53659, 4147, layer )
			set_instance_dungeon_flag( 43001, layer, 'check_open_prop', 0)
			
			if get_instance_dungeon_type_id( 43001, layer ) == 3 then
				add_field_prop( 9005, 0, 53512, 4356, layer, -3, 0, 0, 0, 1, 1, 1 )		--보스 생성 프랍
				broadcast_notice( 1, "@90610144", 43001, layer ) -- 프랍 클릭하면 보스 나온다는 메세지?
			end
		end
		
		if monster_id == 20190027 or monster_id == 20190054 or monster_id == 20190081 or monster_id == 21190108 then
			do_each_player_in_instance_dungeon( 43001, layer, "set_quest_status( 3647, 1, 1 )", X_pos, Y_pos )
			--[[ if get_quest_status( 3647 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 1 ) + 1
				set_quest_status( 3647, 1, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190054 then
			if get_quest_status( 3647 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 1 ) + 1
				set_quest_status( 3647, 1, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190081 then
			if get_quest_status( 3647 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 1 ) + 1
				set_quest_status( 3647, 1, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 21190108 then
			if get_quest_status( 3647 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 1 ) + 1
				set_quest_status( 3647, 1, A_day_A_quest )
			end ]]
		elseif monster_id == 20190010 or monster_id == 20190037 or monster_id == 20190064 or monster_id == 20190091 then
			do_each_player_in_instance_dungeon( 43001, layer, "set_quest_status( 3647, 2, 1 )", X_pos, Y_pos )
			--[[ if get_quest_status( 3647 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 2 ) + 1
				set_quest_status( 3647, 2, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190037 then
			if get_quest_status( 3647 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 2 ) + 1
				set_quest_status( 3647, 2, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190064 then
			if get_quest_status( 3647 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 2 ) + 1
				set_quest_status( 3647, 2, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190091 then
			if get_quest_status( 3647 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 2 ) + 1
				set_quest_status( 3647, 2, A_day_A_quest )
			end ]]
		end
		
	end
	
end

function Open_Chest( prop_id )

	local count = find_item( 601100310 )
	
	if prop_id == 60157 or prop_id == 60159 or prop_id == 60161 or prop_id == 60162 or prop_id == 60163 then
				
		if count >= 1 then
			-- 만족하면 열쇠 수거하고 
			delete_item( get_item_handle( 601100310 ), 1 )
		end
	end
	

end

function Open_main_Chest( prop_id )

	local index_num_1  = math.random ( 0, 2 )
		
	if prop_id == 60176 then -- 메두사의 투구가 나오는 프랍
		
		if index_num_1 == 0 then

			insert_item(601100355, 1)
		
		elseif index_num_1 == 1 then
		
			insert_item(601100355, 1)
			insert_item(601100355, 1)
		
		elseif index_num_1 == 2 then
		
			insert_item(601100355, 1)
			insert_item(601100355, 1)
			insert_item(601100355, 1)
		
		end
	
	elseif prop_id == 60179 then -- 블랙 위도우의 부츠가 나오는 프랍
	
		if index_num_1 == 0 then

			insert_item(601100357, 1)
		
		elseif index_num_1 == 1 then
		
			insert_item(601100357, 1)
			insert_item(601100357, 1)
		
		elseif index_num_1 == 2 then
		
			insert_item(601100357, 1)
			insert_item(601100357, 1)
			insert_item(601100357, 1)
		
		end
	
	elseif prop_id == 60182 then -- 본 드래곤의 아머가 나오는 프랍
	
		if index_num_1 == 0 then

			insert_item(math.random ( 601100358, 601100361 ), 1)
		
		elseif index_num_1 == 1 then
		
			insert_item(math.random ( 601100358, 601100361 ), 1)
			insert_item(math.random ( 601100358, 601100361 ), 1)
		
		elseif index_num_1 == 2 then
		
			insert_item(math.random ( 601100358, 601100361 ), 1)
			insert_item(math.random ( 601100358, 601100361 ), 1)
			insert_item(math.random ( 601100358, 601100361 ), 1)
		
		end

	elseif prop_id == 60185 then  -- 미크로랍토르의 글러브가 나오는 프랍
	
		if index_num_1 == 0 then

			insert_item(601100356, 1)
		
		elseif index_num_1 == 1 then
		
			insert_item(601100356, 1)
			insert_item(601100356, 1)
		
		elseif index_num_1 == 2 then
		
			insert_item(601100356, 1)
			insert_item(601100356, 1)
			insert_item(601100356, 1)
		
		end
	
	end
	
	cprint( "@690000089" )
	
end

function Call_Script_Witch_Death( monster_handle )

	local monster_id = get_monster_id( monster_handle )
	local quest_progress3648 = get_quest_progress(3648) -- [반복]지하 기지 소탕
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	
	if monster_id == 20190002 or monster_id == 20190029 or monster_id == 20190056 or monster_id == 20190083 then  
	
		if quest_progress3648 == 1 then
			if get_quest_status( 3648 , 1 ) < 50 then
				local A_day_A_quest = get_quest_status( 3648 , 1 ) + 1
				set_quest_status( 3648, 1, A_day_A_quest )
			end
		end
	
	elseif monster_id == 20190003 or monster_id == 20190030 or monster_id == 20190057 or monster_id == 20190084 then
	
		if quest_progress3648 == 1 then
			if get_quest_status( 3648 , 2 ) < 50 then
				local A_day_A_quest = get_quest_status( 3648 , 2 ) + 1
				set_quest_status( 3648, 2, A_day_A_quest )
			end
		end
	
	elseif monster_id == 20190004 or monster_id == 20190031 or monster_id == 20190058 or monster_id == 20190085 then
	
		if quest_progress3648 == 1 then
			if get_quest_status( 3648 , 3 ) < 50 then
				local A_day_A_quest = get_quest_status( 3648 , 3 ) + 1
				set_quest_status( 3648, 3, A_day_A_quest )
			end
		end
		
	end
end

-- *add_instance_dungeon_monster( 몬스터리젠ID, 인던ID, 레이어번호): 해당 레이어 인던에 해당 몬스터 리젠 ID의 몬스터 그룹 생성*
-- 서커스 던전 내에 특정 몬스터 그룹이 전멸 했을 때 다음 던전 진행을 처리하기 위해 사용되는 스크립트
function circus_check_respawn_group_clear( monster_group, layer )
	-- monster_group 번호의 숫자를 세서 전멸이면 다음 던전 진행 처리를 한다. 
	local cnt = get_alive_instance_respawn_group_monster_count( 50000, layer, monster_group )
	
	-- 카운트 처리
	if monster_group == 1 then
		broadcast_mission_objective_progress( 1, 0, 10 - cnt, 50000, layer )
	elseif monster_group == 2 then
		broadcast_mission_objective_progress( 1, 0, 13 - cnt, 50000, layer )
	elseif monster_group == 3 then
		broadcast_mission_objective_progress( 1, 0, 13 - cnt, 50000, layer )
	elseif monster_group == 4 then
		broadcast_mission_objective_progress( 1, 0, 23 - cnt, 50000, layer )
	elseif monster_group == 5 then
		broadcast_mission_objective_progress( 1, 0, 19 - cnt, 50000, layer )
	end
	
	-- 전멸 처리
	if cnt == 0 then
				
		
		if monster_group == 1 then -- 전멸 그룹이 1번이면
			add_field_prop( 60186, 0, 38958, 137159, layer, 0, 0, 0, 1.5, 0.3, 0.3, 0.3 )	--작은방 입구 프랍 생성
			
			-- private_notice("@90606228")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606228", 50000, layer )
			-- 2번 그룹 리젠
			add_instance_dungeon_monster( 50023, 50000, layer )
			add_instance_dungeon_monster( 50024, 50000, layer )
			add_instance_dungeon_monster( 50025, 50000, layer )
			add_instance_dungeon_monster( 50026, 50000, layer )
			add_instance_dungeon_monster( 50027, 50000, layer )
			add_instance_dungeon_monster( 50028, 50000, layer )
			add_instance_dungeon_monster( 50029, 50000, layer )
			
			
			broadcast_mission_title( 1, '@9901', 50000, layer ) 
			broadcast_mission_objective( 1, 0, 13, '@1224', 50000, layer )

						
		elseif monster_group == 2 then -- 전멸 그룹이 2번이면
			add_field_prop( 60187, 0, 38836, 137150, layer, 0, 0, 0, -1.5, 0.3, 0.3, 0.3 )	--작은방 출구 프랍 생성
			
			-- private_notice("@90606229")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606229", 50000, layer )
			-- 3번 그룹 리젠
			add_instance_dungeon_monster( 50030, 50000, layer)
			add_instance_dungeon_monster( 50031, 50000, layer)
			add_instance_dungeon_monster( 50032, 50000, layer)
			add_instance_dungeon_monster( 50033, 50000, layer)
			add_instance_dungeon_monster( 50034, 50000, layer)
			add_instance_dungeon_monster( 50035, 50000, layer)
			
			broadcast_mission_title( 1, '@9902', 50000, layer ) 
			broadcast_mission_objective( 1, 0, 13, '@1224', 50000, layer )
						
		elseif monster_group == 3 then -- 전멸 그룹이 3번이면
			add_field_prop( 60188, 0, 40155, 137132, layer, 0, 0, 0, -1.5, 0.3, 0.3, 0.3 )	--중보스방 입구 프랍 생성
			
			-- private_notice("@90606230")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606230", 50000, layer )
			-- 4번 그룹 리젠
			add_instance_dungeon_monster( 50036, 50000, layer)
			add_instance_dungeon_monster( 50037, 50000, layer)
			add_instance_dungeon_monster( 50038, 50000, layer)
			add_instance_dungeon_monster( 50039, 50000, layer)
			add_instance_dungeon_monster( 50040, 50000, layer)
			add_instance_dungeon_monster( 50041, 50000, layer)
			add_instance_dungeon_monster( 50042, 50000, layer)
			add_instance_dungeon_monster( 50043, 50000, layer)
			add_instance_dungeon_monster( 50044, 50000, layer)
			add_instance_dungeon_monster( 50045, 50000, layer)
			add_instance_dungeon_monster( 50046, 50000, layer)
			add_instance_dungeon_monster( 50047, 50000, layer)
			add_instance_dungeon_monster( 50048, 50000, layer)
		
			broadcast_mission_title( 1, '@9903', 50000, layer ) 
			broadcast_mission_objective( 1, 0, 23, '@1224', 50000, layer )
		
		elseif monster_group == 4 then -- 전멸 그룹이 4번이면 		
			add_field_prop( 60189, 0, 40271, 136657, layer, 0, 0, 0, 1.5, 0.3, 0.3, 0.3 )	--중보스방 출구
			add_field_prop( 60190, 0, 39554, 136281, layer, 0, 0, 0, 0, 0.3, 0.3, 0.3 )		--보스방 입구
			
			-- private_notice("@90606231")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606231", 50000, layer )
			-- 5번 그룹 리젠
			add_instance_dungeon_monster( 50049, 50000, layer)
			add_instance_dungeon_monster( 50050, 50000, layer)
			add_instance_dungeon_monster( 50051, 50000, layer)
			add_instance_dungeon_monster( 50052, 50000, layer)
			add_instance_dungeon_monster( 50053, 50000, layer)
			add_instance_dungeon_monster( 50054, 50000, layer)
			add_instance_dungeon_monster( 50055, 50000, layer)
			add_instance_dungeon_monster( 50056, 50000, layer)
			add_instance_dungeon_monster( 50057, 50000, layer)
			
			
			broadcast_mission_title( 1, '@9904', 50000, layer ) 
			broadcast_mission_objective( 1, 0, 19, '@1224', 50000, layer )
			
		elseif monster_group == 5 then -- 전멸 그룹이 5번이면 (던전 클리어)
			add_field_prop( 60191, 0, 39552, 136341, layer, 0, 0, 0, 3, 0.3, 0.3, 0.3 )		--보스방 출구
			
			if  get_instance_dungeon_type_id( 50000, layer ) == 3 then
				add_field_prop( 9006, 0, 39556, 137014, layer, -3, 0, 0, 0, 1, 1, 1 )		--보스 생성 프랍
				broadcast_notice( 1, "@90610144", 50000, layer ) -- 프랍 클릭하면 보스 나온다는 메세지?
			end
			
			broadcast_mission_title( 1, '', 50000, layer )
			-- private_notice("@90606232")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606232", 50000, layer )
		end
	end
end

function on_join_circus( layer )
	
	local cnt_1 = get_alive_instance_respawn_group_monster_count( 50000, layer, 1 )
	local cnt_2 = get_alive_instance_respawn_group_monster_count( 50000, layer, 2 )
	local cnt_3 = get_alive_instance_respawn_group_monster_count( 50000, layer, 3 )
	local cnt_4 = get_alive_instance_respawn_group_monster_count( 50000, layer, 4 )
	local cnt_5 = get_alive_instance_respawn_group_monster_count( 50000, layer, 5 )

	-- 현재 미션 1 진행중
	if( cnt_1 > 0 ) then
		send_mission_title( '@9900' ) 
		send_mission_objective( 0, 10, '@1224' )
		send_mission_objective_progress( 0, 10 - cnt_1 )
	-- 현재 미션 2 진행중
	elseif( cnt_2 > 0 ) then
		send_mission_title( '@9901' ) 
		send_mission_objective( 0, 13, '@1224' )
		send_mission_objective_progress( 0, 13 - cnt_2 )
	-- 현재 미션 3 진행중
	elseif( cnt_3 > 0 ) then
		send_mission_title( '@9902' ) 
		send_mission_objective( 0, 13, '@1224' )
		send_mission_objective_progress( 0, 13 - cnt_3 )
	-- 현재 미션 4 진행중
	elseif( cnt_4 > 0 ) then
		send_mission_title( '@9903' ) 
		send_mission_objective( 0, 23, '@1224' )
		send_mission_objective_progress( 0, 23 - cnt_4 )
	-- 현재 미션 5 진행중
	elseif( cnt_5 > 0 ) then
		send_mission_title( '@9904' ) 
		send_mission_objective( 0, 19, '@1224' )
		send_mission_objective_progress( 0, 19 - cnt_5 )
	end
end

-- 서버 시작할 때 스크립트를 통해 추가 할 프랍들이 있으면 여기에 추가한다.
function add_global_prop()

	-- 칠흑의숲 -> 붉은 거미 서커스 워프 프랍
	add_field_prop( 60192, 0, 163100, 116110, 0, 0, 0 ,0, 0.45 , 1, 1, 1)
	add_field_prop( 60193, 0, 35527, 121011, 0, 0, 0 ,0, 1.5 , 1, 1, 1)


end

function summon_boss( X_pos, Y_pos, Mob_id )
		
	local state_code = get_local_info()
	--add_instance_dungeon_monster( 50049, 50000, gv('layer'))
	if state_code == 1 or 16 then
		add_npc( X_pos, Y_pos, Mob_id, 1, 0, gv('layer') )
	else		
		add_npc( X_pos, Y_pos, Mob_id+7, 1, 0, gv('layer') )
	end
	
end



------------------------------------------------------------------------------------------------------------
function NPC_DevildomDungeon()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	dlg_text( "@90610399" )
	local stage = get_instance_dungeon_flag( 80000, gv('layer'), 'stage' )
	if stage >= 11 then
		dlg_menu( "@90610400", "warp( 22626, 54881, gv('layer'))" )
	end
	if stage >= 21 then
		dlg_menu( "@90610401", "warp(  19129, 58384, gv('layer') )" )
	end
	if stage >= 31 then
		dlg_menu( "@90610855", "warp(  25028, 61184, gv('layer') )" )
	end
	if stage >= 41 then
		dlg_menu( "@90610856", "warp(  28028, 56184, gv('layer') )" )
	end
	if stage >= 51 then
		dlg_menu( "@90610857", "warp(  25028, 51184, gv('layer') )" )
	end
	
	dlg_menu( "@90610402", "exit_indun( 0 )" )
	dlg_menu( "@90010002", " " )	-- 대화종료		
	dlg_show()	
end

function MMT_init(layer, stage)

	if layer == -1 then
		layer = gv('layer')
	end

	set_instance_dungeon_flag( 80000, layer, 'stage', stage )
	set_instance_dungeon_flag( 80000, layer, 'boss', stage )	
	local pgate	 =	
	{	-- 1번방 출구					
		19128,51629,0,		19128,52329,0,		19128,53029,0,		19128,53729,0,		19373,54184,-1.57,
		20073,54184,-1.57,	20773,54184,-1.57,	21473,54184,-1.57,	22173,54184,-1.57,	22628,54429,0,
		22628,55129,0,		22628,55829,0,		22628,56529,0,		22628,57229,0,		22383,57684,1.57,
		21683,57684,1.57,	20983,57684,1.57,	20283,57684,1.57,	19583,57684,1.57,	19128,57929,0,	
		19128,58629,0,		19128,59329,0,		19128,60029,0,		19128,60729,0,		19373,61184,-1.57,	
		
		20370,61184,-1.57,	21370,61184,-1.57,	22370,61184,-1.57,	23370,61184,-1.57,	24370,61184,-1.57,
		25370,61184,-1.57,	26370,61184,-1.57,	27370,61184,-1.57,	28370,61184,-1.57,	29028,60842,3.14,	
		
		29028,59842,3.14,	29028,58842,3.14,	29028,57842,3.14,	29028,56842,3.14,	28686,56184,1.57,	
		27686,56184,1.57,	26686,56184,1.57,	25686,56184,1.57,	24686,56184,1.57,	24028,55842,3.14,	
		
		24028,54842,3.14,	24028,53842,3.14,	24028,52842,3.14,	24028,51842,3.14,	24370,51184,-1.57,	
		25370,51184,-1.57,	26370,51184,-1.57,	27370,51184,-1.57,	28370,51184,-1.57,	29028,51526,0,		
		
		29028,52526,0,		29028,53526,0,		28686,54184,1.57,	27686,54184,1.57,	26686,54184,1.57,
		25686,54184,1.57  -- 마지막문 미사용
	}

	local prop	=	10300
	local dur	=	0
	local height	=	280
	local cnt = table.getn(pgate)/3 -1
	for i = stage, cnt do
		local posx =	pgate[1+(i-1)*3+0]
		local posy =	pgate[1+(i-1)*3+1]
		local dir	 =	pgate[1+(i-1)*3+2]
		add_field_prop(prop+i,dur,posx,posy,layer,height,0,0,dir)
	end					

	if stage == 1 then
		set_instance_dungeon_flag( 80000, layer, 'monster_count', 0 )
	elseif stage == 11 then
		set_instance_dungeon_flag( 80000, layer, 'monster_count', 101 )
	elseif stage == 21 then
		set_instance_dungeon_flag( 80000, layer, 'monster_count', 202 )
	elseif stage == 31 then
		set_instance_dungeon_flag( 80000, layer, 'monster_count', 303 )
	elseif stage == 41 then
		set_instance_dungeon_flag( 80000, layer, 'monster_count', 355 )
	elseif stage == 51 then
		set_instance_dungeon_flag( 80000, layer, 'monster_count', 407 )
	end
	
end


function MMT_open_door(stage)
	local layer = gv('layer')
	MMT_action(layer,stage+1)						
	set_instance_dungeon_flag( 80000, layer, 'stage', stage+1 )  
	
end

--- stage:2 는 2번방 몬스터 생성
function MMT_action(layer,stage)
	local state_code = get_local_info()
	
	if layer == -1 then
		layer = gv('layer')
	end
	
	local type, monster
	if math.mod(stage, 5) ~= 0 and math.mod(stage, 5) ~= 1 then -- 중보, 보스, 시작방 빼고
		type = math.random(1, 8)
		if stage <= 30 then
			monster = math.random(22000185, 22000191) + math.floor( stage / 10 )*11
		else
			local Marray	=
			{	0,	22000315,	22000318,	22000321,	0,	0,	22000324,	22000327,	22000330,	0,	-- 31층
				0,	22000333,	22000336,	22000339,	0,	0,	22000342,	22000345,	22000348,	0,	-- 41층
				0,	22000351,	22000354,	22000357,	0,	0,	22000360,	22000363,	22000366,	0,	-- 51층
			}
			local regen_monster = Marray[stage-30]
			monster = math.random(regen_monster, regen_monster+2) -- 31층 이상 일반몹...
		end
		MMT_regen(layer,stage, type, monster)		
	elseif math.mod(stage+5, 10) == 0 then -- 중보		
		if stage <= 30 then
			type = math.random(21, 22)
		else
			if state_code == 8192 then
				type = 23
			else
				type = math.random( 47, 48 )			
			end
		end
		MMT_regen(layer,stage, type, 0)			
	elseif math.mod(stage, 10) == 0 and stage ~= 60 then --보스 // 마지막 방은 포함 안되도록
		if stage <= 30 then
			type = math.random(31, 32)
		else
			if state_code == 8192 then
				type = 23
			else
				type = math.random( 47, 48 )			
			end
		end
		MMT_regen(layer,stage, type, 0)
	elseif stage == 60 then
		type = math.random(41, 45)		
		if state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then
			local awBelt = find_item( 3100048 )
			if awBelt >= 1 then
				MMT_regen(layer,stage, 46, 0)
				set_instance_dungeon_flag( 80000, layer, 'final_count' ,459 )
			else			
				MMT_regen(layer,stage, type, 0)
				set_instance_dungeon_flag( 80000, layer, 'final_count' ,457 )
			end
		else
			MMT_regen(layer,stage, type, 0)
			set_instance_dungeon_flag( 80000, layer, 'final_count' ,457 )
		end
	elseif math.mod(stage, 5) == 1 then
		--cprint("쉬어가셈")
	end	
end


function MMT_regen(layer, stage, generate_type,  monster_id)
	local parray	=
	{	19128,51384,2,	19128,52084,2,	19128,52784,2,	19128,53484,2,	19128,54184,2,
		19828,54184,1,	20528,54184,1,	21228,54184,1,	21928,54184,1,	22628,54184,1,

		22628,54884,2,	22628,55584,2,	22628,56284,2,	22628,56984,2,	22628,57684,2,
		21928,57684,3,	21228,57684,3,	20528,57684,3,	19828,57684,3,	19128,57684,3,

		19128,58384,2,	19128,59084,2,	19128,59784,2,	19128,60484,2,	19128,61184,2,
		20028,61184,1,	21028,61184,1,	22028,61184,1,	23028,61184,1,	24028,61184,1,
		
		25028,61184,1,	26028,61184,1,	27028,61184,1,	28028,61184,1,	29028,61184,1,
		29028,60184,0,	29028,59184,0,	29028,58184,0,	29028,57184,0,	29028,56184,0,
		
		28028,56184,3,	27028,56184,3,	26028,56184,3,	25028,56184,3,	24028,56184,3,
		24028,55184,0,	24028,54184,0,	24028,53184,0,	24028,52184,0,	24028,51184,0,
		
		25028,51184,1,	26028,51184,1,	27028,51184,1,	28028,51184,1,	29028,51184,1,
		29028,52184,2,	29028,53184,2,	29028,54184,2,	28028,54184,3,	27028,54184,3,
	}
	local count 	= 2
	
	if stage >= 31 then
		count = 1
	end
	
	local dur	 	= 0
	local base_idx	= 1+(stage-1) * 3
	local base_x	= parray[base_idx+0]
	local base_y	= parray[base_idx+1]
	local tdir	= parray[base_idx+2]
	
	local ppattern1	= { -18,-2,	-10,3,	-15,14,	-4,18,	4,18,	15,14,	10,3,	18,-2	}
	local ppattern2 = { -14,-8,	-14,-3,	-14,3,	-14,8,	14,8,	14,3,	14,-3,	14,-8	}
	local ppattern3 = { -11,15,	-6,11,	-7,18,	-2,18,	2,18,	7,18,	11,15,	6,11	}
	local ppattern4 = { 0,-6,       -5,-3,	-5,3,	0,6,	5,3,	5,-3,	-10,16,	10,16	}
	local ppattern5 = { -16,-10,	-16,-4,	-7,10,	-4,17,	4,17,	7,10,	16,-4,	16,-10	}
	local ppattern6 = { -9,12,	-9,18,	-4,18,	-4,12,	4,18,	4,12,	9,18,	9,12	}
	local ppattern7 = { 0,-4,	-6,-2,	6,-2,	0,2,	-8,16,	-3,16,	3,16,	8,16	}
	local ppattern8 = { -4,-1,	4,-1,	0,9,	-11,17,	-7,9,	0,18,	7,9,	11,17	}
	
	local ppattern21 = { 0,8,22000194,        -10,0,22000192} -- 중보, 푸른
	local ppattern22 = { 0,8,22000194,        10,0,22000193} -- 중보, 붉은	
	local ppattern23 = { 0,0,22000370,        0,2,22000369} -- 31층 이상 중보, 쫄보
	
	local ppattern31 = { 0,0,22000195,       0,12,22000193,        -10,0,22000192} -- 보스, 붉은, 푸른
	local ppattern32 = { 0,0,22000195,       0,12,22000192,        10,0,22000193} -- 보스, 푸른, 붉
	
	local ppattern41 = { 0,0,22000384,	-2,0,22000379,	2,0,22000380,	0,-2,22000381} -- 보스, 보스쫄 3마리
	local ppattern42 = { 0,0,22000384,	-2,0,22000380,	2,0,22000381,	0,-2,22000382} -- 보스, 보스쫄 3마리
	local ppattern43 = { 0,0,22000384,	-2,0,22000381,	2,0,22000382,	0,-2,22000383} -- 보스, 보스쫄 3마리
	local ppattern44 = { 0,0,22000384,	-2,0,22000379,	2,0,22000382,	0,-2,22000383} -- 보스, 보스쫄 3마리
	local ppattern45 = { 0,0,22000384,	-2,0,22000379,	2,0,22000380,	0,-2,22000383} -- 보스, 보스쫄 3마리
	local ppattern46 = { 0,0,22000384,	-2,0,22000379,	2,0,22000380,	0,-2,22000381,	-2,-2,22000382,	2,2,22000383} -- If user has another world belt. Will spawn all monsters. (Webzen)
	
	local ppattern47 = { 0,0,22000370,       0,2,22000369	} -- Room 31 onwards (Webzen)
	local ppattern48 = { 0,0,22000370,       0,2,22000403	} -- Room 31 onwards with Yushiva (Webzen)
	
	local xv, yv
	
	if stage <= 25 then
		xv = 10
		yv = 10
	else
		xv = 10
		yv = 10
	end
	
	--cprint("패턴"..generate_type)
	
	if	generate_type==1 then		
		MMT_add_npc( base_x,base_y,monster_id,ppattern1,xv,yv,tdir,count,dur,layer,stage)
	elseif	generate_type==2 then
		MMT_add_npc( base_x,base_y,monster_id,ppattern2,xv,yv,tdir,count,dur,layer,stage)
	elseif	generate_type==3 then
		MMT_add_npc( base_x,base_y,monster_id,ppattern3,xv,yv,tdir,count,dur,layer,stage)
	elseif	generate_type==4 then
		MMT_add_npc( base_x,base_y,monster_id,ppattern4,xv,yv,tdir,count,dur,layer,stage)
	elseif	generate_type==5 then
		MMT_add_npc( base_x,base_y,monster_id,ppattern5,xv,yv,tdir,count,dur,layer,stage)
	elseif	generate_type==6 then
		MMT_add_npc( base_x,base_y,monster_id,ppattern6,xv,yv,tdir,count,dur,layer,stage)
	elseif	generate_type==7 then
		MMT_add_npc( base_x,base_y,monster_id,ppattern7,xv,yv,tdir,count,dur,layer,stage)
	elseif	generate_type==8 then
		MMT_add_npc( base_x,base_y,monster_id,ppattern8,xv,yv,tdir,count,dur,layer,stage)

	elseif	generate_type==21 then
		MMT_add_npc_boss( base_x,base_y,ppattern21,xv,yv,tdir,dur,layer,stage)		
	elseif	generate_type==22 then
		MMT_add_npc_boss( base_x,base_y,ppattern22,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==23 then
		MMT_add_npc_boss( base_x,base_y,ppattern23,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==31 then
		MMT_add_npc_boss( base_x,base_y,ppattern31,xv,yv,tdir,dur,layer,stage)		
	elseif	generate_type==32 then
		MMT_add_npc_boss( base_x,base_y,ppattern32,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==41 then
		MMT_add_npc_boss( base_x,base_y,ppattern41,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==42 then
		MMT_add_npc_boss( base_x,base_y,ppattern42,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==43 then
		MMT_add_npc_boss( base_x,base_y,ppattern44,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==44 then
		MMT_add_npc_boss( base_x,base_y,ppattern44,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==45 then
		MMT_add_npc_boss( base_x,base_y,ppattern45,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==46 then
		MMT_add_npc_boss( base_x,base_y,ppattern46,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==47 then -- Webzen only
		MMT_add_npc_boss( base_x,base_y,ppattern47,xv,yv,tdir,dur,layer,stage)
	elseif	generate_type==48 then -- Webzen only
		MMT_add_npc_boss( base_x,base_y,ppattern48,xv,yv,tdir,dur,layer,stage)
	end
end

function MMT_broadcast_mission( layer,total )

	broadcast_mission_title( 1, '@1224', 80000,layer )
	broadcast_mission_reward( 1, '@90610864',80000,layer )
	broadcast_mission_objective( 1, 0, total, '@1224',80000,layer)		-- 미션 목표도 항상 '모든 몬스터를 섬멸하라'에 최대 몬스터 수는 6(4층인 경우만 1이 됨)
	set_instance_dungeon_flag( 80000, layer, 'total' ,total )
	set_instance_dungeon_flag( 80000, layer, 'current' ,0 )

end

function MMT_add_npc( base_x,base_y,monster_id,ppattern,xv,yv,tdir,count,dur,layer,stage)
		
	local total = 0
	local cnt = table.getn(ppattern)/2
	for i = 1, cnt do
		local idx
			idx = (i-1)*2+1
 		local xx = (ppattern[idx]*xv)
			idx = (i-1)*2+2
		local yy = (ppattern[idx]*yv)
		local temp_xx = xx
		local temp_yy = yy
		if	tdir == 0 then   -- ahead west
			temp_xx = -xx
			temp_yy = -yy
		elseif	tdir == 1 then   -- ahead west
			temp_xx = yy
			temp_yy = -xx
		elseif	tdir == 2 then	-- ahead south
			temp_xx = xx
			temp_yy = yy
		elseif	tdir == 3 then  -- ahead eash
			temp_xx = -yy
			temp_yy = xx 
		end	
		
		if stage <= 30 then
			monster = math.random(22000185, 22000191) + math.floor( stage / 10 )*11
		else
			local Marray	=
			{	0,	22000315,	22000318,	22000321,	0,	0,	22000324,	22000327,	22000330,	0,	-- 31층
				0,	22000333,	22000336,	22000339,	0,	0,	22000342,	22000345,	22000348,	0,	-- 41층
				0,	22000351,	22000354,	22000357,	0,	0,	22000360,	22000363,	22000366,	0,	-- 51층
			}
			
			local regen_monster = Marray[stage-30]
			monster = math.random(regen_monster, regen_monster+2) -- 31층 이상 일반몹...
		end
		
		xx = base_x + temp_xx
		yy = base_y + temp_yy
		add_npc( xx,yy,monster,count,dur,layer)
		total = total + count
		--cprint(xx.." "..yy.." "..monster_id.." "..count.." "..dur.." "..layer)		
	end
	
	MMT_broadcast_mission(layer,total)
end

function MMT_add_npc_boss( base_x,base_y,ppattern,xv,yv,tdir,dur,layer,stage)

	local total = 0
	local cnt = table.getn(ppattern)/3
	for i = 1, cnt do
		local idx
			idx = (i-1)*3+1
 		local xx = (ppattern[idx]*xv)
			idx = (i-1)*3+2
		local yy = (ppattern[idx]*yv)

		local temp_xx = xx
		local temp_yy = yy
		if	tdir == 0 then   -- ahead west
			temp_xx = -xx
			temp_yy = -yy
		elseif	tdir == 1 then   -- ahead west
			temp_xx = yy
			temp_yy = -xx
		elseif	tdir == 2 then	-- ahead south
			temp_xx = xx
			temp_yy = yy
		elseif	tdir == 3 then  -- ahead eash
			temp_xx = -yy
			temp_yy = xx 
		end
		
		local monster_id
		if stage <= 30 then
			monster_id = ppattern[(i-1)*3+3] + (math.floor( (stage / 10)-0.5 ))*11
		elseif stage == 60 then
			monster_id = ppattern[(i-1)*3+3]
		else
			monster_id = ppattern[(i-1)*3+3] + ((stage-35)/5)*2 -- 31층 이상 중보 보스...
		end

		xx = base_x + temp_xx
		yy = base_y + temp_yy
		add_npc( xx,yy,monster_id,1,dur,layer)
		total = total + 1
	end
	MMT_broadcast_mission(layer,total)

end

------------------------------------------------------------------------------------------------------------
function MMT_on_leave( layer )													-- 이탈시호출
------------------------------------------------------------------------------------------------------------
	send_mission_title( '' )
end

------------------------------------------------------------------------------------------------------------
function MMT_on_join( layer )													-- 각 방마다 호출
------------------------------------------------------------------------------------------------------------
	local total = get_instance_dungeon_flag( 80000, layer, 'total' )
	local current = get_instance_dungeon_flag( 80000, layer, 'current' )
	if total ~= nil and total ~= "" then
		send_mission_title( '@1224' )	-- 미션 제목은 항상 '모든 몬스터를 섬멸하라'
		send_mission_reward(  '@90610864')
		send_mission_objective( 0, total, '@1224' )		-- 미션 목표도 항상 '모든 몬스터를 섬멸하라'에 최대 몬스터 수는 6(4층인 경우만 1이 됨)
		send_mission_objective_progress( 0, current )
	end
end

function devildom_count(quest1, parm1, quest2, parm2)	
	local layer = get_value('layer')
	if quest1~=nil then
		--cprint( 'quest1:' .. quest1 .. " parm1:".. parm1)
		do_each_player_in_instance_dungeon( 80000,layer, 'set_quest_status(' .. quest1 ..','.. parm1 ..',1)', gv('x'), gv('y') )
	end
	if quest2~=nil then
		--cprint( 'quest2:' .. quest2 .. " parm2:".. parm2)
		do_each_player_in_instance_dungeon( 80000,layer, 'set_quest_status(' .. quest2 ..','.. parm2 ..',1)', gv('x'), gv('y') )
	end

	local monster_count = get_instance_dungeon_flag( 80000, layer, 'monster_count' )
	set_instance_dungeon_flag( 80000, layer, 'monster_count', monster_count+1 )

	local current = get_instance_dungeon_flag( 80000, layer, 'current' )
	current = current + 1;
	set_instance_dungeon_flag( 80000, layer , 'current', current )
	broadcast_mission_objective_progress( 1, 0, current,  80000,layer )
	
	if get_instance_dungeon_flag( 80000, layer, 'final_count' ) == monster_count then
		add_field_prop( 10361, 0, 26748, 54183, gv('layer'), 250, 0, 0, 1.58, 1, 1, 1 )
	end

end


function Call_Script_tula_Death()
	devildom_count( 4035, 1, 4036, 1 )
end
function Call_Script_Barga_Death()
	devildom_count( 4037, 1, 4038, 1 )
end
function Call_Script_Kainen_Death()
	devildom_count( 4039, 1, 4040, 1 )
end
function Call_Script_DevilWood_Death()
	devildom_count( 4041, 1, 4045, 1 )
end
function Call_Script_Slaughterer_Death()
	devildom_count( 4041, 2, 4045, 2 )
end
function Call_Script_StoneGargoyle_Death()
	devildom_count( 4042, 1, 4046, 1 )
end
function Call_Script_MagmaGolem_Death()
	devildom_count( 4042, 2, 4046, 2)
end
function Call_Script_CrazyHagen_Death()
	devildom_count( 4043, 1, 4044, 1 )
end
function Call_Script_DevilReviac_Death()
	devildom_count( 4043, 2, 4044, 2 )
end



------------------------------------------------------------------------------------------------------------
function NPC_bulcanus_joinitem_sell()
--------------------------------------------------------------------------------------------------------------------------
    dlg_title( "@90605268" )
    dlg_text( "@90605790" )
	
	local progress = get_quest_progress(3367)
	if progress == 255 then
        dlg_menu( "@9811", 'NPC_bulcanus_get_ticket()' ) -- 사자의 혼을 받는다
	end
    dlg_menu( "@90010002", '' )
    dlg_show()
 end
--------------------------------------------------------------------------------------------------------------------------
function NPC_bulcanus_get_ticket()
--------------------------------------------------------------------------------------------------------------------------
	local current_time = get_os_date( "%Y-%m-%d" )
	local t_flag = get_flag( "vulcanus_date" )
	dlg_title( "@90605268" )
	if t_flag == nill or  t_flag < current_time then
			set_flag( "vulcanus_date" , current_time )
			insert_item( 2013889, 1 )
			dlg_text( "@90610860" ) -- 여기 있어요!<br>수량이 부족한 관계로 1일1개 밖에는 드릴수 없어요.
	else
			dlg_text( "@90610861" ) -- 오늘은 이미 받으셨어요.<br>수량이 부족한 관계로 1일 1개 밖에는 드릴수 없어요.
	end
	dlg_menu( "@90010002", '' )
	dlg_show()
end

--------------------------------------------------------------------------------------------------------------------------
function NPC_bulcanus_best_record()
--------------------------------------------------------------------------------------------------------------------------
	local best_vul_record = get_global_variable( "best_vul_record" )
	local best_vul_record_name = get_global_variable("best_vul_record_name")

	dlg_title( "@90605268" )
	
	if best_vul_record == nil or best_vul_record == "" then
		dlg_text( "@90605863" )
	else
		local country_time = get_os_date("%H",0)
		local print_best_vul_record = best_vul_record +(24 -country_time) * 3600 
		dlg_text( sconv("@90605862", "#@user_name@#", get_global_variable( "best_vul_record_name" ), "#@vul_record@#", get_os_date("%X",print_best_vul_record)) )
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

------------------------------------------------------------------------------------------------------------
function vulcanus_get_room_pos( floor, room )					-- 각층 각방 x y 좌표
--------------------------------------------------------------------------------------------------------------------------
	local pdata={	197393,28580,	198814,28457,	200543,28458,	202065,28458,
					197047,25954,	198661,25929,	200362,25962,	202169,26121,
					197303,23750,	198586,23765,	200390,23677,	202114,23701,
				 	206474,27625,	0,0,			0,0,			0,0,
				}
	local x =	pdata[ floor*8-7+room*2 ]
	local y =	pdata[ floor*8-7+room*2+1]
	return x,y
end

------------------------------------------------------------------------------------------------------------
function vulcanus_get_clear_reward( layer, floor )					-- 각 방 클리어 보상 Exp,JP,Gold
--------------------------------------------------------------------------------------------------------------------------
	local pdata = 
	{ 	-- F1,		F2,		F3		F4			 F1,		F2,			F3			F4
		19956,		24454,		29555,		58378,			38548,		44904,		51743,		132405,		-- 1	40 ~  
		57408,		64815,		72956,		214137,			76854,		85198,		94077,		269109,		-- 3	60 ~  
		91139,		99583,		108495,		314787,			113358,		122679,		132349,		427830,		-- 5	80 ~  
		131787,		141445,		151556,		561177,			175584,		187230,		199491,		700102,		-- 7	100 ~  
		237120,		251671,		266785,		988754,			292792,		306274,		320012,		1169043,	-- 9	120 ~  
		377019,		392723,		408832,		1785194,		470027,		527619,		620635,		3387588,	-- 11	140 ~  
		769704,		1541102,	3173438,	6718169,		1219563,	1800319,	3922638,	12356309,	-- 13	160 ~  
		1829344,	2700478,	5883957,	18534463,		2744016,	4050717,	8825935,	27801694,	-- 15	180 ~  
		4116024,	6076075,	13238902,	41702541,		6174036,	9114112,	19858353,	62553811,	-- 17	200 ~  
		9261054,	13671168,	29787529,	93830716,		13891581,	20506752,	44681293,	140746074 ,	-- 19	220 ~  
	}

	local lvl = get_instance_dungeon_type_id( 20000, layer )
	local idx = lvl*4+floor
	local exp = pdata[ idx ]
	local jp = exp / 8
	local gold = 31200 * ( 1+ lvl/3)
	if  floor==2  then
		gold = gold + 2100
	elseif  floor==3  then
		gold = gold + 4200
	elseif  floor==4  then
		gold = (gold + 4200 )*2
	end
	return exp, jp, gold
end

--------------------------------------------------------------------------------------------------------------------------
function get_vulcanus_room_number(floor)							-- 아직 클리어하지 않은 방을 찾는다.
--------------------------------------------------------------------------------------------------------------------------
	local layer = get_value('layer')
	local room = 0
	if floor < 4 then
		local room_list = {}
		local room_count = 0
		local flag	= 'Vul' .. tostring( floor )
		local fv = get_instance_dungeon_flag( 20000, layer, flag )
		if fv == 15 then
			return
		end
		if fv==nil or fv=="" then	
			fv = 0
		end
		if fv==0 or fv==2 or fv==4 or fv==8 or fv==6 or fv==10 or fv==12 or fv==14 then
			room_list[ room_count + 1 ] = 0
			room_count = room_count + 1
		end
		if fv==0 or fv==1 or fv==4 or fv==8 or fv==5 or fv==9 or fv==12 or fv==13 then
			room_list[ room_count + 1 ] = 1
			room_count = room_count + 1
		end
		if fv==0 or fv==1 or fv==2 or fv==8 or fv==3 or fv==9 or fv==10 or fv==11 then
			room_list[ room_count + 1 ] = 2
			room_count = room_count + 1
		end
		if fv==0 or fv==1 or fv==2 or fv==4 or fv==3 or fv==5 or fv==6 or fv==7 then
			room_list[ room_count + 1 ] = 3
			room_count = room_count + 1
		end
		local select = math.random( 1, room_count ) 
		room = room_list[select]

	end
	return room
end


--------------------------------------------------------------------------------------------------------------------------
function enter_vulcanus()											-- 필드에 있는 불카누스 프랍 클릭시 
--------------------------------------------------------------------------------------------------------------------------
	local quest_progress_3367 = get_quest_progress(3367)	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	if quest_progress_3367 == 1 then
		set_quest_status( 3367, 1, 1 )
		cprint( "@90604746" )		-- <(version:8.1)><#6DD66D>새로운 게이트를 발견하였습니다.
		return
	end
	local lv = get_value( 'level' )
	if lv<30 then
		cprint( "@9251" )	--<#ff0000>불카누스 던전은 최소 레벨 30 이상부터 입장 가능합니다.
	end
	local base_type = math.floor(lv/10)-4
	if base_type < 0 then
		base_type = 0
	end
	if base_type > 16 then	 -- 일단 13까지 밖에 없기 때문에 10(easy),11(normal),12(hard),13(veryhard)
		base_type = 16
	end

	local menu1 = '@9526| (Level' .. (base_type+3) * 10 ..')'
	local menu2 = '@9527| (Level' .. (base_type+4) * 10 ..')'
	local menu3 = '@9528| (Level' .. (base_type+5) * 10 ..')'
	--local menu4 = '<@FF8080>|@9528| (Level' .. (base_type+6) * 10 ..')'

	local trigger1 = 'enter_vulcanus_confirm(' .. base_type .. ')'
	local trigger2 = 'enter_vulcanus_confirm(' .. base_type+1 .. ')'
	local trigger3 = 'enter_vulcanus_confirm(' .. base_type+2 .. ')'
	--local trigger4 = 'enter_vulcanus_confirm(' .. base_type+3 .. ')'
	--dlg_special_menu( '@9525', 'Warp', menu1, trigger1, menu2, trigger2, menu3, trigger3, menu4, trigger4 )
	dlg_special_menu( '@9525', 'Warp', menu1, trigger1, menu2, trigger2, menu3, trigger3 )

end

--------------------------------------------------------------------------------------------------------------------------
function enter_vulcanus_confirm(sub_type)									-- vulcanus_enter_confirm() 으로 변경
--------------------------------------------------------------------------------------------------------------------------
	local party_id = get_value('party_id')
	if party_id ~= 0 then
		cprint( "@9247" )	--파티 상태에서는 입장 불가함
		return
	end

	local item_count = find_item( 2013889 )	
	if item_count <  1 then
		cprint( "@9810" )
		return
	end
	warp_to_instance_dungeon(20000,sub_type)
	delete_item( get_item_handle(2013889),1)
	--vulcanus_warp_confirm(1)
	open_title(1)

end


--------------------------------------------------------------------------------------------------------------------------
function enter_other_indun( dungeon_id, cur_floor, next_floor, cur_item_count, next_item_count )   
	vulcanus_warp (cur_floor)
end
--------------------------------------------------------------------------------------------------------------------------
function vulcanus_warp (floor)													-- 불카누스 던전 내부 프랍 클릭,
--------------------------------------------------------------------------------------------------------------------------
	local flag = 'Vul' .. tostring(floor )
	local floor_flag = get_instance_dungeon_flag( 20000 , gv( 'layer' ), flag )
	local cur = floor
	local nxt = floor +1
	local nxt_flr = floor+1
	local menu2 = '@9800\v#@next_floor@#\v' .. nxt  
	if floor_flag == 15 then
		dlg_special_menu( '@170126024', 'Warp',menu2, 'vulcanus_warp_confirm(' .. nxt .. ')' )
	else			
		local menu1 = '@9799\v#@current_floor@#\v' .. cur  
		dlg_special_menu( '@170126024', 'Warp', menu1, 'vulcanus_warp_confirm(' .. cur .. ')', menu2, 'vulcanus_warp_confirm(' .. nxt .. ')' )
	end
end

--------------------------------------------------------------------------------------------------------------------------
function vulcanus_warp_confirm(floor,layer)
--------------------------------------------------------------------------------------------------------------------------
	if layer == nil then
		 layer = get_value('layer')
	end

	local room = get_vulcanus_room_number(floor)
	local x,y =	vulcanus_get_room_pos( floor, room )
	warp( x, y, layer )

	send_mission_title( '@1224' )	-- 미션 제목은 항상 '모든 몬스터를 섬멸하라'
	local exp, jp, gold = vulcanus_get_clear_reward( layer, floor )
	local reward_string = '@1225'
	reward_string = reward_string .. '\v#@reward_exp@#\v' .. exp
	reward_string = reward_string .. '\v#@reward_jp@#\v' .. jp
	reward_string = reward_string .. '\v#@reward_gold@#\v' .. gold
	send_mission_reward( reward_string )
	
	if floor == 4 then
		send_mission_objective( 0, 1, '@1226' )		-- 불카누스를 처치하라
		cprint( '@90604914' )						--<(version:7.3)><#6DD66D>나는.. 불카누스다. 나의 의지를 깨운 너는....너는 누구.... 인가. 열쇠를 소지한 자.. 인가?
		cprint( '@90604915' )						--<(version:7.3)><#6DD66D>그렇다면, 내가 숨겨 논 기록을...... 되찾은 자로군. 시간이 없으니...... 너의 지식을 보겠다.

		local monster = 22000414
		local progress1 = get_quest_progress(4047)
		local progress2 = get_quest_progress(4048)
		local progress3 = get_quest_progress(4049)
		local progress4 = get_quest_progress(4050)
		local lvl = get_instance_dungeon_type_id( 20000, layer )
		if (progress1 == 1 or progress2 == 1 or progress3 == 1 or progress4 ==  1) and lvl >= 14 then
			local qcv = get_flag('q_cbk')
			if qcv == nil or qcv == "" then
				qcv = 0
			end

			local rnd = math.random(1, 10-qcv)
			if rnd == 1 then
			   add_npc( 205940, 26915, monster, 1, 0, layer )			
 			   qcv = 0	
			elseif qcv < 7 then
				qcv = qcv + 1
			end 
			set_flag('q_cbk',qcv)

		end

	else
		send_mission_objective( 0, 6, '@1224' )		-- 미션 목표도 항상 '모든 몬스터를 섬멸하라'에 최대 몬스터 수는 6(4층인 경우만 1이 됨)
	end
	send_mission_objective_progress( 0, 0 )
end


------------------------------------------------------------------------------------------------------------
function on_create_vulcanus_instance( layer )										-- 최초 던전 생성시 호출
------------------------------------------------------------------------------------------------------------
	set_instance_dungeon_flag( 20000, layer, 'Vul1', 0 )
	set_instance_dungeon_flag( 20000, layer, 'Vul2', 0 )
	set_instance_dungeon_flag( 20000, layer, 'Vul3', 0 )
	set_instance_dungeon_flag( 20000, layer, 'date', get_os_time() )

	add_field_prop( 126023, 0, 197460, 28579, layer, 10)
	add_field_prop( 126023, 0, 198873, 28457, layer, 10)
	add_field_prop( 126023, 0, 200546, 28391, layer, 10)
	add_field_prop( 126023, 0, 202068, 28403, layer, 10)
	add_field_prop( 126023, 0, 197026, 25926, layer, 10)
	add_field_prop( 126023, 0, 198651, 25901, layer, 10)
	add_field_prop( 126023, 0, 200339, 25945, layer, 10)
	add_field_prop( 126023, 0, 202194, 26114, layer, 10)
	add_field_prop( 126023, 0, 197324, 23734, layer, 10)
	add_field_prop( 126023, 0, 198566, 23754, layer, 10)
	add_field_prop( 126023, 0, 200370, 23657, layer, 10)
	add_field_prop( 126023, 0, 202125, 23673, layer, 10)
end

------------------------------------------------------------------------------------------------------------
function on_leave_vulcanus( layer )													-- 이탈시호출
------------------------------------------------------------------------------------------------------------
	send_mission_title( '' )
end

------------------------------------------------------------------------------------------------------------
function on_join_vulcanus( layer )													-- 각 방마다 호출
------------------------------------------------------------------------------------------------------------
	vulcanus_warp_confirm(1,layer)
	--do_each_player_in_instance_dungeon( 20000, layer, "open_title(1)" )
end

------------------------------------------------------------------------------------------------------------
function vulcanus_check_respawn_group_clear( respawn, x, y, layer )					-- 몬스터 죽을 때마다 호출
------------------------------------------------------------------------------------------------------------
	local pdata={		 --respawn	prop	z,	fl  bit monster_count
					20001,	126024,	10,	1,	1,	6,			
					20002,	126024,	10,	1,	2,	6,		
					20003,	126024,	10,	1,	4,	6,			
					20004,	126024,	10,	1,	8,	6,
					20005,	126025,	10,	2,	1,	6,		
					20006,	126025,	10,	2,	2,	6,		
					20007,	126025,	10,	2,	4,	6,		
					20008,	126025,	10,	2,	8,	6,
					20009,	126026,	10,	3,	1,	6,	
					20010,	126026,	10,	3,	2,	6,		
					20011,	126026,	10,	3,	4,	6,	
					20012,	126026,	10,	3,	8,	6,
					20013,	126027,	35,	4,	1,	1,		}
	local idx = 0
	local total = table.getn( pdata) / 6
	for i = 1, total do
		if  pdata[i*6-5] == respawn  then
			idx = i
			break
		end
	end
	if idx == 0 then
		return
	end

	local prop	=  pdata[idx*6-4]
	local offset	=  pdata[idx*6-3]
	local floor	=  pdata[idx*6-2]
	local bit	=  pdata[idx*6-1]
	local monster	=  pdata[idx*6-0]

	local left_monster = get_alive_instance_respawn_group_monster_count( 20000, layer, respawn )
	broadcast_mission_objective_progress( 1, 0, monster - left_monster, 20000, layer )
	if	left_monster ~= 0 then
		return
	end

	if	floor==4	then
		do_each_player_in_instance_dungeon( 20000, layer, "update_title_condition( 9002002, 1 )" )	-- 불카누스 사냥 시 호칭 부여
	else
		local floor_clear_flag = get_instance_dungeon_flag( 20000, layer, 'Vul' .. floor )
		floor_clear_flag = floor_clear_flag + bit
		set_instance_dungeon_flag( 20000, layer, 'Vul'..floor, floor_clear_flag )
		do_each_player_in_instance_dungeon( 20000, layer, "cprint('@9813');cprint('@9250')" )
	end

	-- 보상 지급 스크립트
	local exp, jp, gold = vulcanus_get_clear_reward( layer, floor )
	local reward_handler = 'add_exp_jp( ' .. exp .. ', ' .. jp .. ', false, false ); insert_gold( ' .. gold .. ', true )'
	do_each_player_in_instance_dungeon( 20000, layer, reward_handler )

	broadcast_mission_title( 1, '', 20000, layer )		-- 미션창 숨김
	add_field_prop( prop, 0, x, y, layer, offset )		-- 다음방으로 가는 프랍 생성



end

