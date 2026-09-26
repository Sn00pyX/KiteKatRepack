-- Lua 스크립트 암호화
function get_module_name()
             return "on_player_level_up"
end

function on_player_level_up()

	--local tx, ty, current_x, island_number
	local lv = get_value( "level" )
	local max_reached_level = gv( "max_reached_level" )
	
	
	--=========================================================================================150 레벨 이하 레벨 달성 보상	
	local i
	for i = max_reached_level + 1 , lv do	
		if i == 10 then
			insert_item( 690322, 1 )
			insert_item( 930071, 1 )
			insert_item( 930072, 1 )
			insert_item( 930073, 1 )
			insert_item( 930074, 1 )
			insert_item( 930075, 1 )
			insert_item( 930076, 1 )
			insert_item( 540019, 2 )
			insert_item( 540018, 2 )
			insert_item( 540011, 2 )
			insert_item( 540005, 2 )
			insert_item( 540006, 2 )
			insert_item( 540002, 2 )
			insert_item( 540053, 2 )
			insert_item( 540057, 2 )
			insert_item( 540079, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 20 then
			insert_item( 690322, 1 )
			insert_item( 930071, 1 )
			insert_item( 930072, 1 )
			insert_item( 930073, 1 )
			insert_item( 930074, 1 )
			insert_item( 930075, 1 )
			insert_item( 930076, 1 )
			insert_item( 540019, 2 )
			insert_item( 540018, 2 )
			insert_item( 540011, 2 )
			insert_item( 540005, 2 )
			insert_item( 540006, 2 )
			insert_item( 540002, 2 )
			insert_item( 540053, 2 )
			insert_item( 540057, 2 )
			insert_item( 540079, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 30 then
			insert_item( 2013011, 5 )
			insert_item( 930071, 2 )
			insert_item( 930072, 2 )
			insert_item( 930073, 2 )
			insert_item( 930074, 2 )
			insert_item( 930075, 2 )
			insert_item( 930076, 2 )
			insert_item( 900010, 2 )
			insert_item( 540019, 2 )
			insert_item( 540018, 2 )
			insert_item( 540011, 2 )
			insert_item( 540005, 2 )
			insert_item( 540006, 2 )
			insert_item( 540002, 2 )
			insert_item( 540053, 2 )
			insert_item( 540057, 2 )
			insert_item( 540079, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
				
		elseif i == 40 then
			insert_item( 2013011, 5 )
			insert_item( 930071, 2 )
			insert_item( 930072, 2 )
			insert_item( 930073, 2 )
			insert_item( 930074, 2 )
			insert_item( 930075, 2 )
			insert_item( 930076, 2 )
			insert_item( 900010, 2 )
			insert_item( 540019, 2 )
			insert_item( 540018, 2 )
			insert_item( 540011, 2 )
			insert_item( 540005, 2 )
			insert_item( 540006, 2 )
			insert_item( 540002, 2 )
			insert_item( 540053, 2 )
			insert_item( 540057, 2 )
			insert_item( 540079, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
				
		elseif i == 50 then
			insert_item( 2013011, 5 )
			insert_item( 930071, 3 )
			insert_item( 930072, 3 )
			insert_item( 930073, 3 )
			insert_item( 930074, 3 )
			insert_item( 930075, 3 )
			insert_item( 930076, 3 )
			insert_item( 900010, 2 )
			insert_item( 540019, 2 )
			insert_item( 540018, 2 )
			insert_item( 540011, 2 )
			insert_item( 540005, 2 )
			insert_item( 540006, 2 )
			insert_item( 540002, 2 )
			insert_item( 540053, 2 )
			insert_item( 540057, 2 )
			insert_item( 540079, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
				
		elseif i == 60 then
			insert_item( 2013011, 5 )
			insert_item( 930071, 3 )
			insert_item( 930072, 3 )
			insert_item( 930073, 3 )
			insert_item( 930074, 3 )
			insert_item( 930075, 3 )
			insert_item( 930076, 3 )
			insert_item( 900010, 2 )
			insert_item( 540019, 2 )
			insert_item( 540018, 2 )
			insert_item( 540011, 2 )
			insert_item( 540005, 2 )
			insert_item( 540006, 2 )
			insert_item( 540002, 2 )
			insert_item( 540053, 2 )
			insert_item( 540057, 2 )
			insert_item( 540079, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
				
		elseif i == 70 then
			insert_item( 2013011, 7 )
			insert_item( 930071, 5 )
			insert_item( 930072, 5 )
			insert_item( 930073, 5 )
			insert_item( 930074, 5 )
			insert_item( 930075, 5 )
			insert_item( 930076, 5 )
			insert_item( 900010, 2 )
			insert_item( 2011027, 1 )
			insert_item( 540019, 2 )
			insert_item( 540018, 2 )
			insert_item( 540011, 2 )
			insert_item( 540005, 2 )
			insert_item( 540006, 2 )
			insert_item( 540002, 2 )
			insert_item( 540053, 2 )
			insert_item( 540057, 2 )
			insert_item( 540079, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
				
		elseif i == 80 then
			insert_item( 2013011, 7 )
			insert_item( 930071, 5 )
			insert_item( 930072, 5 )
			insert_item( 930073, 5 )
			insert_item( 930074, 5 )
			insert_item( 930075, 5 )
			insert_item( 930076, 5 )
			insert_item( 900010, 2 )
			insert_item( 540019, 2 )
			insert_item( 540018, 2 )
			insert_item( 540011, 2 )
			insert_item( 540005, 2 )
			insert_item( 540006, 2 )
			insert_item( 540002, 2 )
			insert_item( 540053, 2 )
			insert_item( 540057, 2 )
			insert_item( 540079, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 90 then
			insert_item( 2013011, 7 )
			insert_item( 930071, 5 )
			insert_item( 930072, 5 )
			insert_item( 930073, 5 )
			insert_item( 930074, 5 )
			insert_item( 930075, 5 )
			insert_item( 930076, 5 )
			insert_item( 2011033, 1 )
			insert_item( 540019, 5 )
			insert_item( 540018, 5 )
			insert_item( 540011, 5 )
			insert_item( 540005, 5 )
			insert_item( 540006, 5 )
			insert_item( 540002, 5 )
			insert_item( 540053, 5 )
			insert_item( 540057, 5 )
			insert_item( 540079, 5 )
			insert_item( 900010, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 100 then
			insert_item( 2013011, 10 )
			insert_item( 930071, 10 )
			insert_item( 930072, 10 )
			insert_item( 930073, 10 )
			insert_item( 930074, 10 )
			insert_item( 930075, 10 )
			insert_item( 930076, 10 )
			insert_item( 540200, 5 )
			insert_item( 710008, 5 )
			insert_item( 900010, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 110 then
			insert_item( 2013011, 10 )
			insert_item( 930071, 10 )
			insert_item( 930072, 10 )
			insert_item( 930073, 10 )
			insert_item( 930074, 10 )
			insert_item( 930075, 10 )
			insert_item( 930076, 10 )
			insert_item( 540200, 5 )
			insert_item( 710008, 5 )
			insert_item( 900010, 2 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 120 then
			insert_item( 2013011, 10 )
			insert_item( 930071, 10 )
			insert_item( 930072, 10 )
			insert_item( 930073, 10 )
			insert_item( 930074, 10 )
			insert_item( 930075, 10 )
			insert_item( 930076, 10 )
			insert_item( 540200, 5 )
			insert_item( 710008, 5 )
			insert_item( 900010, 5 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 130 then
			insert_item( 2013011, 10 )
			insert_item( 930071, 10 )
			insert_item( 930072, 10 )
			insert_item( 930073, 10 )
			insert_item( 930074, 10 )
			insert_item( 930075, 10 )
			insert_item( 930076, 10 )
			insert_item( 540201, 5 )
			insert_item( 710008, 5 )
			insert_item( 900010, 5 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 140 then
			insert_item( 2013011, 10 )
			insert_item( 930071, 10 )
			insert_item( 930072, 10 )
			insert_item( 930073, 10 )
			insert_item( 930074, 10 )
			insert_item( 930075, 10 )
			insert_item( 930076, 10 )
			insert_item( 540201, 5 )
			insert_item( 710008, 5 )
			insert_item( 900010, 5 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		elseif i == 150 then
			insert_item( 710008, 20 )
			insert_item( 2012774, 20 )
			insert_item( 2012780, 10 )
			insert_item( 2012824, 5 )
			insert_item( 2012825, 5 )
			private_notice( sconv( "@690000140" , "#@index_lv@#", i ) )
			
		end
	end	
	
	if lv == 5 then
		-- 각 1번 섬의 텔레포트 좌표
		--		tx = 30315
		--		ty = 5480

		-- 현재귀환 위치를 가져옴.
		current_x = get_flag( "rx" )

		-- 몇 번 째 섬인지 확인.
		--		local i
		--		for i = 1, 6 do
		--			if current_x < ( 18816 * (i-1) + tx) and current_x + 3000 > ( (18816 * (i-1) + tx) - 10000) then
		--				island_number = i
		--			end
		--		end
		-- 귀환지역을 현재 초보자섬 캠프으로 설정
		set_flag( "rx", 172543 + math.random(0,100))
		set_flag( "ry", 51847 + math.random(0,100))
	end

	-- 국가 코드 읽어오기
	-- get_local_info()의 반환값들
	--LOCAL_INFO_KOREA                      = 1
	--LOCAL_INFO_HONGKONG                   = 2
	--LOCAL_INFO_AMERICA                    = 4
	--LOCAL_INFO_GERMANY                    = 8
	--LOCAL_INFO_JAPAN                      = 16
	--LOCAL_INFO_TAIWAN                     = 32
	--LOCAL_INFO_CHINA                      = 64
	--LOCAL_INFO_FRANCE                     = 128
	--LOCAL_INFO_RUSSIA                     = 256


	local state_code = get_local_info()	

	-- 18레벨 이상이나 수련자의 섬 내에 있는 경우 종족 마을로 귀환 지점 설정
	local current_x = gv("x")
	local current_y = gv("y")
	local race = get_value( "race" )

	-- 미국이면 무조건 안보냄 (전 국가 적용 안함으로 변경 08.07.29)
	if state_code == 511 then
		-- 아무짓도 안해요
	elseif lv >= 18 then
		-- 수련자의 섬에 있는지 여부 체크
		if current_x >= 161280 and current_x <= 177408 then
			if current_y >= 48384 and current_y <= 64512 then

				-- 현재귀환 위치를 가져옴.
				local return_x = get_flag( "rx" )
				local return_y = get_flag( "ry" )

				-- 수련자의 섬이 귀환인지 체크
				if return_x == 173183 and return_y == 52299 then

					-- 귀환지역을 해당 종족 마을로 설정

					-- 데바일 경우
					if race == 4 then
						set_flag( "rx", 6625 + math.random(0,100))
						set_flag( "ry", 6980 + math.random(0,100))

					-- 아수라일 경우
					elseif race == 5 then
						set_flag( "rx", 116799 + math.random(0,100))
						set_flag( "ry", 58205 + math.random(0,100))

					-- 가이아일 경우
					else
						set_flag( "rx", 153513 + math.random(0,100))
						set_flag( "ry", 77203 + math.random(0,100))
					end -- if race == 4 then

					-- 수련자의 섬에 있고 18레벨 이상이기 때문에 해당 종족 마을로 강제 귀환 설정 됐다는 메시지 날림.
					message( "@235")

				end -- if return_x == 173183 and return_y == 52299
			end -- if current_y >= 48384 and current_y <= 64512 then
		end -- if current_x >= 161280 and current_x <= 177408 then
	end -- if lv >= 18 then

	-- 오토로 세팅된 캐릭터라면 저 멀리 날려 버리자~
	kick_auto_to_another_world()

	-- 수련자 섬에 있는 오토들은 본토로 날려 버리자
	local current_x = gv("x")
	local current_y = gv("y")
	local race = get_value( "race" )

	if current_x >= 161280 and current_x <= 177408 then
		if current_y >= 48384 and current_y <= 64512 then

			local is_auto, quest_count
			quest_count, is_auto = anti_auto_quest_check()

			if is_auto then
				if race == 4 then
					RunTeleport_Auto_TO_City( 6625 , 6980 )
				elseif race == 5 then
					RunTeleport_Auto_TO_City( 116799 , 58205 )
				else
					RunTeleport_Auto_TO_City( 153506 , 77175 )
				end
			end -- if is_auto then
		end -- if current_y >= 48384 and current_y <= 64512 then
	end -- if current_x >= 161280 and current_x <= 177408 then

	-- 레벨업 달성 이벤트 처리
	
		-- 국가 코드 읽어오기
	-- get_local_info()의 반환값들
	--LOCAL_INFO_KOREA			= 1
	--LOCAL_INFO_HONGKONG		= 2
	--LOCAL_INFO_AMERICA			= 4
	--LOCAL_INFO_GERMAN			= 8
	--LOCAL_INFO_JAPAN			= 16
	--LOCAL_INFO_TAIWAN			= 32
	--LOCAL_INFO_CHINA			= 64
	--LOCAL_INFO_FRANCE			= 128
	--LOCAL_INFO_RUSSIA			= 256
	--LOCAL_INFO_MALAYSIA			= 512
	--LOCAL_INFO_SINGAPORE		= 1024
	--LOCAL_INFO_VIETNAM			= 2048
	--LOCAL_INFO_THAILAND			= 4096
	--LOCAL_INFO_MIDEAST			= 8192
	--LOCAL_INFO_TURKEY			= 16384
	
--[[local state_code = get_local_info()	--국가불러오기 현재 상태 숨김

	if state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then --국가확인
	local i
			for i = max_reached_level + 1 , lv do	
					if i == 5 then
						insert_item( 3600289, 1 )
		
					elseif i == 10 then
						insert_item( 3600289, 1 )

					elseif i == 15 then
						insert_item( 3600289, 1 )
			
					elseif i == 20 then
						insert_item( 3600289, 1 )
			
					elseif i == 25 then
						insert_item( 3600289, 1 )
			
					elseif i == 30 then
						insert_item( 3600289, 1 )
				
					elseif i == 35 then
						insert_item( 3600289, 1 )
			
					elseif i == 40 then
						insert_item( 3600289, 1 )
			
					elseif i == 45 then
						insert_item( 3600289, 1 )
			
					elseif i == 50 then
						insert_item( 3600289, 1 )
			
					elseif i == 55 then
						insert_item( 3600289, 1 )
		
					elseif i == 60 then
						insert_item( 3600289, 1 )
			
					elseif i == 65 then
						insert_item( 3600289, 1 )
			
					elseif i == 70 then
						insert_item( 3600289, 1 )
			
					elseif i == 75 then
						insert_item( 3600289, 1 )
			
					elseif i == 80 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 85 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 90 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 95 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 100 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 105 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 110 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
		
					elseif i == 115 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 120 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 125 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 130 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 135 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 140 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 145 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 150 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 155 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
		
					elseif i == 160 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 165 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 170 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
					end
			end

---------------------------------------------------------------------------------------------동남아 세팅
	elseif state_code == 512 or state_code == 1024 then	--동남아(싱가폴, 필리핀, 말레이시아)
		
		local q_flag0 = get_flag( "event_avatar" )		--이벤트 대상 체크
	
		--플레그값 초기화 시켜 주기
		if q_flag0 == "" then
			q_flag0 = 0
		end
		
		if q_flag0 < 1 then
		
			local max_reached_level = gv( "max_reached_level" )
			local i
				for i = max_reached_level + 1 , lv do	
					if i == 5 then
						insert_item( 3600289, 1 )
		
					elseif i == 10 then
						insert_item( 3600289, 1 )

					elseif i == 15 then
						insert_item( 3600289, 1 )
			
					elseif i == 20 then
						insert_item( 3600289, 1 )
			
					elseif i == 25 then
						insert_item( 3600289, 1 )
			
					elseif i == 30 then
						insert_item( 3600289, 1 )
				
					elseif i == 35 then
						insert_item( 3600289, 1 )
			
					elseif i == 40 then
						insert_item( 3600289, 1 )
			
					elseif i == 45 then
						insert_item( 3600289, 1 )
			
					elseif i == 50 then
						insert_item( 3600289, 1 )
			
					elseif i == 55 then
						insert_item( 3600289, 1 )
		
					elseif i == 60 then
						insert_item( 3600289, 1 )
			
					elseif i == 65 then
						insert_item( 3600289, 1 )
			
					elseif i == 70 then
						insert_item( 3600289, 1 )
			
					elseif i == 75 then
						insert_item( 3600289, 1 )
			
					elseif i == 80 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 85 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 90 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
			
					elseif i == 95 then
						insert_item( 3600289, 1 )
						insert_item( 3600289, 1 )
					end
				end			
			end
end ]]

	-- 레벨업 시 가이드 메시지 출력
	on_player_level_joblevel_guide()

	--레벨 달성 시 호칭부여하기
	if lv > max_reached_level then 
		update_title_condition( 9002001, lv )
	end
end

function on_player_level_joblevel_guide()
	--작업중
	local lv = get_value( "level" )
	local job_lv = get_value( "job_level" )
	local job_dp = get_value( "job_depth" )
	
	if job_dp == 0 and lv >= 10 and job_lv >= 10 then
		cprint( "@1200" )
		--cprint( "@254" )
		cprint( "@1201" )		
	elseif lv == 20 then
		cprint( "@1202" )
		cprint( "@1203" )
		
		-- add_npc(172578, 51937, 600, 5)
		
	elseif lv == 50 then
			cprint( "@1204" )
			cprint( "@1205" )	
	elseif job_dp== 1 and lv >= 50 and job_lv >= 40 then
		cprint( "@1206" )
		cprint( "@1207" )	
		cprint( "@1208" )
	elseif lv == 80 then
		cprint( "@1209" )
		cprint( "@1210" )
	elseif lv == 100 then
		cprint( "@1211" )
		cprint( "@1212" )		
	end	
end