-- Lua 스크립트 암호화
function get_module_name()
             return "on_first_login"
end

function on_first_login( name )

	-- 본래 HP 세팅하던 자리 (혹시 몰라서 그냥 냅둠)
	set_value( "hp" , get_value( "max_hp") )
	set_value( "mp" , get_value( "max_mp") )
	
	-- 종족 체크	
	local state_code = get_local_info()
	
	local race = get_value( "race" )
	
	local deva_x, deva_y, asura_x, asura_y, gaia_x, gaia_y, i
	
	-- 각 수련자의 섬 시작 지점 위치
	deva_x = 164474
	deva_y = 52932
	asura_x = 168356
	asura_y = 55399
	gaia_x = 164335
	gaia_y = 49510

	local start_x, start_y, start_layer, start_channel_no
	
	if state_code == 64 then
		insert_item( 2000294, 1 )	
	end

	-- 데바 시작지점 체크
	if race == 4 then
		
		--시작 위치 설정
		start_x = deva_x
		start_y = deva_y

	-- 아수라 시작지점 체크
	elseif race == 5 then
		
		--시작 위치 설정
		start_x = asura_x
		start_y = asura_y

	-- 그외(가이아 3) 시작지점 체크
	else
		
		--시작 위치 설정
		start_x = gaia_x
		start_y = gaia_y

	end
	
	-- 수련자의 섬(채널ID 1000)에서 가장 적절한 채널을 골라 레이어를 세팅한다.
	start_channel_no = get_proper_channel_num( 1000 )
	start_layer = get_layer_of_channel( 1000, start_channel_no )
	

	-- 귀환지역 설정
	set_flag( "rx", start_x + math.random(0,60) - 30)
	set_flag( "ry", start_y + math.random(0,60) - 30)
	
	local text
	
	text = sconv("@90010092", "#@number@#", tostring( start_channel_no ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	message( text ) -- 시스템 메세지로 해당 채널ID 출력.


	------=========== ** 시작아이템 설정해주는 부분은 서버에서 구현 ** ============------		
	-- 아수라(5)면 더크(단검), 데바면 스몰메이스, 가이아는 밀림도를 준다. 
	-- 디폴트 직업을 넣어준다. (데바:가이드, 아수라:스테퍼, 그외:파이터(가이아) )
	
	local race = get_value( "race" )

	if race == 4 then

		-- 초기직업을 가이드(200)로 설정한다.
		set_value( "job" , 200 )
		
	elseif race == 5 then
		
		set_value( "job" , 300 )
		
	else

		set_value( "job" , 100 )

	end

	
	-- 직업 설정 후 세팅된 스탯에 맞는 HP로 다시 채워줌
	set_value( "hp" , get_value( "max_hp") )
	set_value( "mp" , get_value( "max_mp") )
	set_value( "level", 10 ) -- // 테스트 서버용 추가 사항 본서버에 패치 시 삭제 필요
	private_notice( "각 마을에 있는 잡 서포터 NPC를 찾아가 전직하세요." ) -- //테스트서버에서만 적용, 본섭에 들어갈 때는 삭제 필요
	clear_inventory()

	
	-- 국가 코드 읽어오기
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_EUROPE                     = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
	
	--전국가 공통
	local state_code = get_local_info() 	
	--if state_code = 32 then
		if race == 4 then
			if get_env("game.use_auto_trap") == 1 then
			open_popup("game.caution_url", "game.newbiehelp_deva_url", 0,1)
			else
			open_popup("game.newbiehelp_deva_url", 0,1)
			end
		elseif race == 5 then
			if get_env("game.use_auto_trap") == 1 then
			open_popup("game.caution_url", "game.newbiehelp_asura_url", 0,1)
			else
			open_popup("game.newbiehelp_asura_url", 0,1)
			end
		else 
			if get_env("game.use_auto_trap") == 1 then
			open_popup("game.caution_url", "game.newbiehelp_gaia_url", 0,1)
			else
			open_popup("game.newbiehelp_gaia_url", 0,1)
		end
	--	end
	--else
	
	--기존 대만 제거
	--local state_code = get_local_info() 	
	--if state_code ~= 32 then
	--	--if race == 4 then
	--		--if get_env("game.use_auto_trap") == 1 then
	--		open_popup("game.caution_url", "game.newbiehelp_deva_url", 0,1)
	--		else
	--		open_popup("game.newbiehelp_deva_url", 0,1)
	--		end
	--	elseif race == 5 then
	--		if get_env("game.use_auto_trap") == 1 then
	--		open_popup("game.caution_url", "game.newbiehelp_asura_url", 0,1)
	--		else
	--		open_popup("game.newbiehelp_asura_url", 0,1)
	--		end
	--	else 
	--		if get_env("game.use_auto_trap") == 1 then
	--		open_popup("game.caution_url", "game.newbiehelp_gaia_url", 0,1)
	--		else
	--		open_popup("game.newbiehelp_gaia_url", 0,1)
	--		end
	--	end
	
	
	end
	
	-- 시작좌표 설정 캐릭터 생성이 한번에 많이 발생하면 문제가 생겨서 아래로 이동
	
	set_value( "x" , start_x)
	set_value( "y" , start_y)
	set_value( "layer", start_layer )
		
end







