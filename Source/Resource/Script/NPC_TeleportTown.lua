-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_TeleportTown"
end

   -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.


   --============================================================
   --      <<<<<< 길드(길드원) 소유 던전 텔레포트 공통함수>>>>>>
   --============================================================

-- 던전으로 워프 시키기
function scf_teleport_to_owned_dungeon()

	-- get_own_dungeon_id() 해당 명령어를 실행한 유저의 길드 또는 해당
	-- 소유한 던전이 없을 경우 리턴값 0
	-- 길드가 소속된 길드 연합이 소유하고 있는 던전의 ID를 반환합니다.
	-- 0 : 던전과 관계 없음
	-- 1 : 던전 소유 길드 또는 연합의 마스터 길드의 길드 마스터
	-- 2 : 던전 소유 길드 또는 연합의 마스터 길드의 길드원
	-- 3 : 던전 소유 연합의 마스터 길드가 아닌 서브 길드의 길드 마스터
	-- 4 : 던전 소유 연합의 마스터 길드가 아닌 서브 길드의 길드원
	-- 5 : 던전 시즈 공격자 길드 또는 공격자 연합 마스터 길드의 길드 마스터
	-- 6 : 던전 시즈 공격자 길드 또는 공격자 연합 마스터 길드의 길드원
	-- 7 : 던전 시즈 공격자 연합의 마스터 길드가 아닌 서브 길드의 길드 마스터
	-- 8 : 던전 시즈 공격자 연합의 마스터 길드가 아닌 서브 길드의 길드원
	-- 9 : 던전 소유 길드의 길드원 중 던전관리 권한을 가진 길드원
	
	local dungeon_id = get_own_dungeon_id()
	-- dungeon_id : 130000, 130300 등등의 해당 유저와 관계를 확인하기 위한 던전ID (계속 추가됨)
	-- 130000	잃어버린 갱도 제 1 탄광
	-- 130300	제 1 수정 계곡
	-- 130600	잃어버린 갱도 제 2 탄광
	-- 130500	제 2 수정 계곡
	-- 130400	메마른 달빛의 유적 제 1 실
	-- 130700	메마른 달빛의 유적 제 2 실
	-- 130800	팔미르 유적 제 1 실
	-- 130900	팔미르 유적 제 2 실
	-- 121000	백룡의 쉼터
	-- 122000	흑룡의 그늘
	-- 123000	사룡의 심장
	-- 120700	엘 카시아
	 
	-- 잃어버린 갱도1
	if dungeon_id == 130000 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 155817 + math.random(0,60) , 103724 + math.random(0,60) )
	
	-- 잃어버린 갱도2
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

    -- 백룡의 쉼터
	elseif dungeon_id == 121000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 91985 + math.random(0,60) , 117044 + math.random(0,60) )			

    -- 흑룡의 그늘
	elseif dungeon_id == 122000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 85720 + math.random(0,60) , 118033 + math.random(0,60) )	

    -- 사룡의 그늘
	elseif dungeon_id == 123000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 92027 + math.random(0,60) , 124430 + math.random(0,60) )	
		
    -- 엘 카시아
	elseif dungeon_id == 120700 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 146188 + math.random(0,60) , 135579 + math.random(0,60) )			
		
		
	end	
	
	
end



function scf_teleport_to_owned_secret_dungeon()
	
	-- get_own_dungeon_id() 해당 명령어를 실행한 유저의 길드 또는 해당
	-- 소유한 던전이 없을 경우 리턴값 0
	-- 길드가 소속된 길드 연합이 소유하고 있는 던전의 ID를 반환합니다.
	
	local dungeon_id = get_own_dungeon_id()
	-- dungeon_id : 130000, 130300 등등의 해당 유저와 관계를 확인하기 위한 던전ID (계속 추가됨)
	-- 130000	잃어버린 갱도 제 1 탄광
	-- 130300	제 1 수정 계곡
	-- 130600	잃어버린 갱도 제 2 탄광
	-- 130500	제 2 수정 계곡
	-- 130400	메마른 달빛의 유적 제 1 실
	-- 130700	메마른 달빛의 유적 제 2 실
	-- 130800	팔미르 유적 제 1 실
	-- 130900	팔미르 유적 제 2 실
	-- 121000	백룡의 쉼터
	-- 122000	흑룡의 그늘
	-- 123000	사룡의 심장
	-- 120700	엘 카시아
	
	-- 수정 계곡
	if dungeon_id == 130300  or dungeon_id == 130500 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp_to_secret_dungeon(70101)

    -- 팔미르 유적 
	elseif dungeon_id == 130800 or dungeon_id == 130900 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp_to_secret_dungeon(120201)

    -- 백룡의 쉼터
	elseif dungeon_id == 121000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp_to_secret_dungeon(100101)		

    -- 흑룡의 그늘
	elseif dungeon_id == 122000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp_to_secret_dungeon(90101)

    -- 사룡의 그늘
	elseif dungeon_id == 123000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp_to_secret_dungeon(80101)
		
    -- 엘 카시아
	elseif dungeon_id == 120700 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp_to_secret_dungeon(110101)		
		
		
	end	

end
   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================

function NPC_TeleportTown_Deva_init()
	cprint( "!텔레포터 가브리엘 가동" )
	set_npc_name(  "@90100500"  )
end
 

function NPC_TeleportTown_Deva_contact()
 	
	-- 다이얼로그 출력
	dlg_title( "@90100501" )
	dlg_text( "@90100502" )

--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
    dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]


	dlg_menu( "@90100507", 'Binding_Deva_001()' )
	dlg_menu( "@90100503", 'RunTeleport( 0 ,122934 , 138140 )' ) -- 라크시 필드

   -- 레벨이 10 이상이면 다른 도시(카탄, 호라이즌, 론도)로 이동가능
   --if get_value( "level" ) > 9 then
      dlg_menu( "@90100506", 'RunTeleport( 500 , 116799 , 58205 )' )  -- 아수라 카탄
      dlg_menu( "@90100510", 'RunTeleport( 500 , 153506 , 77175 )' ) -- 가이아 호라이즌    
      dlg_menu( "@90100511", 'RunTeleport( 50000 , 135609 , 104589 )' ) -- 론도
      dlg_menu( "@90100516", 'RunTeleport( 50000 , 140101 , 102600 )' ) -- 크리쳐 농장
      dlg_menu( "@90010151", 'RunTeleport( 80000 , 152943 , 151081 )' ) -- 도시 유적
	  dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
      dlg_menu( "@90019001", 'RunTeleport_Begin_TO_City( 0 , 172543 , 51847 )' ) -- 수련자의 섬
   --end
   
    local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end
	
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

function NPC_TeleportTown_2_Deva_init()
	cprint( "!텔레포터 루리엘 가동" )
	set_npc_name(  "@90101400"  )
end
  
function NPC_TeleportTown_2_Deva_contact()

	-- 다이얼로그 출력
	dlg_title( "@90101401" )
	dlg_text( "@90101402" )

	dlg_menu( "@90101407", 'Binding_Deva_001()' )
	dlg_menu( "@90101403", 'RunTeleport( 0 ,122934 , 138140 )' )

   -- 레벨이 10 이상이면 다른 도시(카탄, 호라이즌, 론도)로 이동가능
   --if get_value( "level" ) > 9 then
      dlg_menu( "@90101406", 'RunTeleport( 500 , 116799 , 58205 )' )
      dlg_menu( "@90101410", 'RunTeleport( 500 , 153506 , 77175 )' )
      dlg_menu( "@90101411", 'RunTeleport( 50000 , 135609 , 104589 )' )
      dlg_menu( "@90100516", 'RunTeleport( 50000 , 140101 , 102600 )' )
      dlg_menu( "@90010151", 'RunTeleport( 80000 , 152943 , 151081 )' )
	  dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
      dlg_menu( "@90019001", 'RunTeleport_Begin_TO_City( 0 , 172543 , 51847 )' )
   --end
   
   	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


function Binding_Deva_001()

    -- 귀환지역을 라크시로 설정.
    set_flag( "rx", 6625 + math.random(0,100))
    set_flag( "ry", 6980 + math.random(0,100))
    
    -- 설정 됐다는 메시지 날림.
    message( "@90100508")

end

   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================

function NPC_TeleportTown_Asura_init()
	cprint( "!텔레포터 아스몬드 가동" )
	set_npc_name(  "@90200500"  )
end
 

function NPC_TeleportTown_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90200501" )
	dlg_text( "@90200502" )

	dlg_menu( "@90200507", 'Binding_Asura_001()' )

   -- 레벨이 10 이상이면 다른 도시(라크시, 호라이즌, 론도)로 이동가능
   --if get_value( "level" ) > 9 then
      dlg_menu( "@90200505", 'RunTeleport( 500 , 6625 , 6980 )' ) -- 데바 라크시
      dlg_menu( "@90200509", 'RunTeleport( 500 , 153506 , 77175 )' ) --  가이아 호라이즌 
	  dlg_menu( "@90700617\v#@price@#\v60000", 'RunTeleport( 60000 , 107700 , 76121 )' ) --사이라그 페허
	  dlg_menu( "@90700618\v#@price@#\v48000", 'RunTeleport( 48000 , 124939 , 71987 )' ) --애도의 묘지
	  dlg_menu( "@90999300\v#@price@#\v24000", 'RunTeleport( 24000 , 124736 , 57288 )' ) -- 중간캠프 카탄 3시
	  dlg_menu( "@90999301\v#@price@#\v30000", 'RunTeleport( 30000 , 123179 , 65374 )' ) -- 중간캠프 카탄 1시
	  dlg_menu( "@90999302\v#@price@#\v21000", 'RunTeleport( 21000 , 109591 , 58449 )' ) -- 중간캠프 카탄 10시
	  dlg_menu( "@90999304\v#@price@#\v90000", 'RunTeleport( 90000 , 100576 , 82868 )' ) -- 중간캠프 카탄 서쪽 사막 입구 
      dlg_menu( "@90200511", 'RunTeleport( 50000 , 135609 , 104589 )' ) -- 론도
      dlg_menu( "@90100516", 'RunTeleport( 50000 , 140101 , 102600 )' ) -- 크리쳐 농장
      dlg_menu( "@90010151", 'RunTeleport( 80000 , 152943 , 151081 )' ) -- 도시 유적
	  dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
      dlg_menu( "@90019001", 'RunTeleport_Begin_TO_City( 0 , 172543 , 51847 )' ) -- 수련자의 섬
	  
	  -- dlg_menu( "@90999005\v#@price@#\v123000", 'RunTeleport( 123000 , 96900 , 101308 )' )		 -- 수정의 산 인근
  --end
   
  	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

function NPC_TeleportTown_2_Asura_init()
	cprint( "!텔레포터 이리오소 가동" )
	set_npc_name(  "@90996931"  )
end
 

function NPC_TeleportTown_2_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90996932" )
	dlg_text( "@90996933" )

	dlg_menu( "@90200507", 'Binding_Asura_001()' )

   -- 레벨이 10 이상이면 다른 도시(라크시, 호라이즌, 론도)로 이동가능
   --if get_value( "level" ) > 9 then
      dlg_menu( "@90200505", 'RunTeleport( 500 , 6625 , 6980 )' ) -- 데바 라크시
      dlg_menu( "@90200509", 'RunTeleport( 500 , 153506 , 77175 )' ) --  가이아 호라이즌 
	  dlg_menu( "@90700617\v#@price@#\v60000", 'RunTeleport( 60000 , 107700 , 76121 )' ) --사이라그 페허
	  dlg_menu( "@90700618\v#@price@#\v48000", 'RunTeleport( 48000 , 124939 , 71987 )' ) --애도의 묘지
	  dlg_menu( "@90999300\v#@price@#\v24000", 'RunTeleport( 24000 , 124736 , 57288 )' ) -- 중간캠프 카탄 3시
	  dlg_menu( "@90999301\v#@price@#\v30000", 'RunTeleport( 30000 , 123179 , 65374 )' ) -- 중간캠프 카탄 1시
	  dlg_menu( "@90999302\v#@price@#\v21000", 'RunTeleport( 21000 , 109591 , 58449 )' ) -- 중간캠프 카탄 10시
	  dlg_menu( "@90999304\v#@price@#\v90000", 'RunTeleport( 90000 , 100576 , 82868 )' ) -- 중간캠프 카탄 서쪽 사막 입구 
      dlg_menu( "@90200511", 'RunTeleport( 50000 , 135609 , 104589 )' ) -- 론도
      dlg_menu( "@90100516", 'RunTeleport( 50000 , 140101 , 102600 )' ) -- 크리쳐 농장
      dlg_menu( "@90010151", 'RunTeleport( 80000 , 152943 , 151081 )' ) -- 도시 유적
	  dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
      dlg_menu( "@90019001", 'RunTeleport_Begin_TO_City( 0 , 172543 , 51847 )' ) -- 수련자의 섬
  --end
   
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

function Binding_Asura_001()

    -- 귀환지역을 카탄으로 설정
	set_flag( "rx", 116799 + math.random(0,100))
	set_flag( "ry", 58205 + math.random(0,100))
   
    -- 설정 됐다는 메시지 날림.
    message( "@90200508")

end

   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================

function NPC_TeleportTown_Gaia_init()
	cprint( "!텔레포터 리벤델 가동" )
	set_npc_name(  "@90400500"  )
end
 

function NPC_TeleportTown_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90400501" )
	dlg_text( "@90400502" )

	dlg_menu( "@90400508", 'Binding_Gaia_001()' )

   -- 레벨이 10 이상이면 다른 도시(라크시 또는 카탄)으로 이동가능
   --if get_value( "level" ) > 9 then
	dlg_menu( "@90400506", 'RunTeleport( 500 , 6625 , 6980 )' ) -- 데바 라크시
	dlg_menu( "@90400507", 'RunTeleport( 500 , 116799 ,  58205 )' ) -- 아수라 카탄
	dlg_menu( "@90700620\v#@price@#\v48000", 'RunTeleport( 48000 , 139982 , 85162 )' ) --월하의 공동 묘지
	dlg_menu( "@90700621\v#@price@#\v27000", 'RunTeleport( 27000 , 155533 , 85778 )' ) -- 제 1 발모어 탄광
	dlg_menu( "@90999309\v#@price@#\v21000", 'RunTeleport( 21000 , 146894 , 77650 )' ) -- 중간캠프 호라이즌 10시
	dlg_menu( "@90999310\v#@price@#\v51000", 'RunTeleport( 51000 , 155104 , 93975 )' ) -- 중간캠프 호라이즌 12시
	dlg_menu( "@90400511", 'RunTeleport( 50000 , 135609 , 104589 )' ) -- 론도 
      	dlg_menu( "@90100516", 'RunTeleport( 50000 , 140101 , 102600 )' ) -- 크리쳐 농장
	dlg_menu( "@90010151", 'RunTeleport( 80000 , 152943 , 151081 )' ) -- 도시 유적
	dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
    dlg_menu( "@90019001", 'RunTeleport_Begin_TO_City( 0 , 172543 , 51847 )' ) -- 수련자의 섬
	--end
	
	dlg_menu( "@90999617", "quest_rumor6()" )

	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end
   
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

-- 세부대화 리벤델
function quest_rumor6()
	-- 다이얼로그 출력
	dlg_title( "@90400501" )

	dlg_text_without_quest_menu( "@90999620" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_3()" )
			
	dlg_menu( "@90010001", " " )
	dlg_show()

end

-- 세부대화 
function quest_rumor_a_3()
	-- 다이얼로그 출력
	dlg_title( "@90400501" )

	dlg_text_without_quest_menu( "@90999624" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999627", "quest_rumor_b_3()" )
	
	dlg_menu( "@90010001", " " )	
	dlg_show()

end

-- 세부대화
function quest_rumor_b_3()
	-- 다이얼로그 출력
	dlg_title( "@90400501" )

	dlg_text_without_quest_menu( "@90999630" )
	
	dlg_menu( "@90010001", " " )
	dlg_show()

end




function NPC_TeleportTown_2_Gaia_init()
	cprint( "!텔레포터 어딘델 가동" )
	set_npc_name(  "@90996934"  )
end
 

function NPC_TeleportTown_2_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90996935" )
	dlg_text( "@90400502" )

	dlg_menu( "@90400508", 'Binding_Gaia_001()' )

   -- 레벨이 10 이상이면 다른 도시(라크시 또는 카탄)으로 이동가능
   --if get_value( "level" ) > 9 then
	dlg_menu( "@90400506", 'RunTeleport( 500 , 6625 , 6980 )' ) -- 데바 라크시
	dlg_menu( "@90400507", 'RunTeleport( 500 , 116799 ,  58205 )' ) -- 아수라 카탄
	dlg_menu( "@90700620\v#@price@#\v48000", 'RunTeleport( 48000 , 139982 , 85162 )' ) --월하의 공동 묘지
	dlg_menu( "@90700621\v#@price@#\v27000", 'RunTeleport( 27000 , 155533 , 85778 )' ) -- 제 1 발모어 탄광
	dlg_menu( "@90999309\v#@price@#\v21000", 'RunTeleport( 21000 , 146894 , 77650 )' ) -- 중간캠프 호라이즌 10시
	dlg_menu( "@90999310\v#@price@#\v51000", 'RunTeleport( 51000 , 155104 , 93975 )' ) -- 중간캠프 호라이즌 12시
	dlg_menu( "@90400511", 'RunTeleport( 50000 , 135609 , 104589 )' ) -- 론도 
      	dlg_menu( "@90100516", 'RunTeleport( 50000 , 140101 , 102600 )' ) -- 크리쳐 농장
	dlg_menu( "@90010151", 'RunTeleport( 80000 , 152943 , 151081 )' ) -- 도시 유적
	dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
    dlg_menu( "@90019001", 'RunTeleport_Begin_TO_City( 0 , 172543 , 51847 )' ) -- 수련자의 섬
   --end
   
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end
   
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

function Binding_Gaia_001()

    -- 귀환지역을 호라이즌로 설정. (가이드 자리...)
    set_flag( "rx", 153513 + math.random(0,100))
    set_flag( "ry", 77203 + math.random(0,100))
    
    -- 설정 됐다는 메시지 날림.
    message( "@90400509")

end


   --============================================================
   --             <<<<<< 초보자섬 NPC >>>>>>
   --============================================================

function NPC_TeleportField_Beginner_init()
	cprint( "!텔레포터 오시어 가동" )
	set_npc_name(  "@90300500"  )
end
 

function NPC_TeleportField_Beginner_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90300501" )
	dlg_text( "@90300502" )

	local current_x, current_y, target_x, target_y

	current_x = get_value( "x" )
	current_y = get_value( "y" )

	-- 귀환지역 설정
	dlg_menu( "@90300507", 'Binding_Beginner_001()' )
	
	-- 동쪽 해안가로 이동을 위한 상급 교관 찾아가기 퀘스트 수행 체크
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress1 =  get_quest_progress(1025)
	if quest_progress1 == 255 then
		dlg_menu( "@90300503", 'RunTeleport( 0 , 175711 ,56887 )' )	
    end	
    
	-- 종족 체크	
	local race = get_value( "race" )
	
	-- 전직후엔 다른 도시(라크시 또는 카탄)으로 이동가능. 종족ID = 가이아 3, 데바 4, 아수라 5
	if get_value( "job_depth" ) > 0 then
		if race == 4 then
			dlg_menu( "@90300505", 'RunTeleport_Begin_TO_City( 10 , 6625 , 6980 )' )
		elseif race == 5 then
			dlg_menu( "@90300506", 'RunTeleport_Begin_TO_City( 10 , 116799 , 58205 )' )
		else
			dlg_menu( "@90300512", 'RunTeleport_Begin_TO_City( 10 , 153506 , 77175 )' )
		end
	end

	-- 다른 수련자 캠프로 이동
	dlg_menu( "@90300513", 'Teleport_channel( 1000 )')
	
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


function Binding_Beginner_001()

    -- 귀환지역을 현재 초보자섬으로 설정
    set_flag( "rx", 172185 + math.random(0,10))
    set_flag( "ry", 52095 + math.random(0,10))
    
    -- 설정 됐다는 메시지 날림.
    message( "@90300508")

end


   --============================================================
   --             <<<<<< 론도 측 NPC >>>>>>
   --============================================================

function NPC_TeleportTown_Rondoh_init()
	cprint( "!텔레포터 레쿠 가동" )
	set_npc_name(  "@90600500"  )
end

function NPC_TeleportTown_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90600501" )
	dlg_text( "@90600502" )

	dlg_menu( "@90600508", 'Binding_Rondoh_001()' )

   -- 레벨이 10 이상이면 다른 도시(라크시, 카탄,호라이즌)로 이동가능
	--if get_value( "level" ) > 9 then
		dlg_menu( "@90600505", 'RunTeleport( 50000 , 6625 , 6980 )' ) -- 데바 라크시
		dlg_menu( "@90600506", 'RunTeleport( 50000, 116799 , 58205 )' ) -- 아수라 카탄
		dlg_menu( "@90600510", 'RunTeleport( 50000, 153506 , 77175 )' ) -- 가이아 호라이즌
		dlg_menu( "@90010151", 'RunTeleport( 80000 , 152943 , 151081 )' ) -- 도시 유적
		dlg_menu( "@90010171", 'RunTeleport( 1000 , 129112 , 109201 )' ) -- 대련장
      		dlg_menu( "@90100517", 'RunTeleport( 1000 , 140101 , 102600 )' ) -- 크리쳐 농장
		dlg_menu( "@90700623\v#@price@#\v81000", 'RunTeleport( 81000 , 108976 , 103279 )' ) -- 수정의 산
		dlg_menu( "@90700624\v#@price@#\v30000", 'RunTeleport( 30000 , 134029 , 115307 )' ) -- 팔미르 고원 입구
		dlg_menu( "@90700625\v#@price@#\v60000", 'RunTeleport( 60000 , 150691 , 117877 )' ) -- 칠흑의 숲
		dlg_menu( "@90999005\v#@price@#\v123000", 'RunTeleport( 123000 , 96900 , 101308 )' )		 -- 수정의 산 인근
		dlg_menu( "@90999306\v#@price@#\v39000", 'RunTeleport( 39000 , 129250 , 94073 )' ) -- 중간캠프 론도 7시
		dlg_menu( "@90999307\v#@price@#\v36000", 'RunTeleport( 36000 , 123635 , 103436 )' ) -- 중간캠프 론도 9시
		dlg_menu( "@90999308\v#@price@#\v48000", 'RunTeleport( 48000 , 150508 , 111503 )' ) -- 중간캠프 론도 2시
		dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
	--end                                                                                                    
   
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

   	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

function NPC_TeleportTown_Rondoh_init()
	cprint( "!텔레포터 지니 가동" )
	set_npc_name(  "@90601400"  )
end

function NPC_TeleportTown_2_Rondoh_contact()

 
	-- 다이얼로그 출력
	dlg_title( "@90601401" )
	dlg_text( "@90601402" )

	dlg_menu( "@90601408", 'Binding_Rondoh_002()' )

	-- 레벨이 10 이상이면 다른 도시(라크시, 카탄, 호라이즌)로 이동가능
	--if get_value( "level" ) > 9 then
		dlg_menu( "@90600505", 'RunTeleport( 50000 , 6625 , 6980 )' ) -- 데바 라크시
		dlg_menu( "@90600506", 'RunTeleport( 50000, 116799 , 58205 )' ) -- 아수라 카탄
		dlg_menu( "@90600510", 'RunTeleport( 50000, 153506 , 77175 )' ) -- 가이아 호라이즌
		dlg_menu( "@90010151", 'RunTeleport( 80000 , 152943 , 151081 )' ) -- 도시 유적
		dlg_menu( "@90010171", 'RunTeleport( 1000 , 129112 , 109201 )' ) -- 대련장
      		dlg_menu( "@90100517", 'RunTeleport( 1000 , 140101 , 102600 )' ) -- 크리쳐 농장
		dlg_menu( "@90700623\v#@price@#\v81000", 'RunTeleport( 81000 , 108976 , 103279 )' ) -- 수정의 산
		dlg_menu( "@90700624\v#@price@#\v30000", 'RunTeleport( 30000 , 134029 , 115307 )' ) -- 팔미르 고원 입구
		dlg_menu( "@90700625\v#@price@#\v60000", 'RunTeleport( 60000 , 150691 , 117877 )' ) -- 칠흑의 숲
		dlg_menu( "@90999005\v#@price@#\v123000", 'RunTeleport( 123000 , 96900 , 101308 )' )		 -- 수정의 산 인근
		dlg_menu( "@90999306\v#@price@#\v39000", 'RunTeleport( 39000 , 129250 , 94073 )' ) -- 중간캠프 론도 7시
		dlg_menu( "@90999307\v#@price@#\v36000", 'RunTeleport( 36000 , 123635 , 103436 )' ) -- 중간캠프 론도 9시
		dlg_menu( "@90999308\v#@price@#\v48000", 'RunTeleport( 48000 , 150508 , 111503 )' ) -- 중간캠프 론도 2시
		dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
	--end
	
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

function Binding_Rondoh_001()

    -- 귀환지역을 론도로 설정.
    set_flag( "rx", 135609 + math.random(0,100))
    set_flag( "ry", 104589 + math.random(0,100))
    
    -- 설정 됐다는 메시지 날림.
    message( "@90600509")

end

function Binding_Rondoh_002()

    -- 귀환지역을 론도로 설정.
    set_flag( "rx", 140019 + math.random(0,100))
    set_flag( "ry", 106038 + math.random(0,100))
    
    -- 설정 됐다는 메시지 날림.
    message( "@90600509")

end

   --============================================================
   --             <<<<<< 도시 유적 측 NPC >>>>>>
   --============================================================

function NPC_TeleportTown_Ancient_relic_init()
	cprint( "!텔레포터 이간 가동" )
	set_npc_name(  "@90703300"  )
end
 

function NPC_TeleportTown_Ancient_relic_contact()
 	
	-- 다이얼로그 출력
	dlg_title( "@90703301" )
	dlg_text( "@90703302" )

	dlg_menu( "@90703307", 'Binding_Ancient_relic_001()' )
	
   -- 레벨이 10 이상이면 다른 도시(라크시, 카탄, 호라이즌, 론도)로 이동가능
   --if get_value( "level" ) > 9 then
   	  dlg_menu( "@90703303", 'RunTeleport( 80000 , 6625 , 6980 )' ) -- 라크시
      dlg_menu( "@90703304", 'RunTeleport( 80000 , 116799 , 58205 )' ) -- 카탄
      dlg_menu( "@90703305", 'RunTeleport( 80000 , 153506 , 77175 )' ) -- 호라이즌
      dlg_menu( "@90703306", 'RunTeleport( 80000 , 135609 , 104589 )' ) -- 론도
      dlg_menu( "@90100518", 'RunTeleport( 80000 , 140101 , 102600 )' ) -- 크리쳐 농장
	  dlg_menu( "@90999001\v#@price@#\v51000", 'RunTeleport( 51000 , 159628 , 135186 )' ) --120~ 마레마을 북쪽 삼거리
	  dlg_menu( "@90999002\v#@price@#\v45000", 'RunTeleport( 45000 , 148005 , 136193 )' ) --130~ 폭포
	  dlg_menu( "@90606233\v#@price@#\v200000", 'RunTeleport( 200000 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구
	  --dlg_menu( "@90999003\v#@price@#\v12000", 'RunTeleport( 12000 , 150846 , 147357 )' ) --140~ 도시유적과 마르두카 경계선
	  --dlg_menu( "@90999004\v#@price@#\v24000", 'RunTeleport( 24000 , 145149 , 150854 )' ) --150~ 리자드맨 서식지와 도시유적 경계선
	             
	--end
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end
   
	dlg_menu( "@90010001", " " )
	dlg_show()
 
end


function Binding_Ancient_relic_001()

    -- 귀환지역을 도시 유적으로 설정. (가이드 자리...)
    set_flag( "rx", 152634 + math.random(0,100))
    set_flag( "ry", 151508 + math.random(0,100))
    
    -- 설정 됐다는 메시지 날림.
    message( "@90703308")

end


   --============================================================
   --             <<<<<< 던전워프 NPC >>>>>>
   --============================================================


function NPC_TeleportDungeon_horizon()	-- 호라이즌
 
	-- 다이얼로그 출력
	dlg_title( "@90606145" )
	dlg_text( "@90606127" )

	--if get_value( "level" ) > 9 then
		dlg_menu( "@90606128\v#@price@#\v10000", 'RunTeleport( 10000 , 132995 , 87096 )' ) -- 메마른 달빛의 유적 제 1실
      	dlg_menu( "@90606129\v#@price@#\v10000", 'RunTeleport( 10000 , 130842 , 79586 )' ) -- 메마른 달빛의 유적 제 2실
		dlg_menu( "@90606130\v#@price@#\v50000", 'RunTeleport( 50000 , 155817, 103724 )' ) -- 잃어버린 갱도 제 1탄광
		dlg_menu( "@90606131\v#@price@#\v50000", 'RunTeleport( 50000, 152309 , 102886 )' ) -- 잃어버린 갱도 제 2탄광
		dlg_menu( "@90606132\v#@price@#\v50000", 'RunTeleport( 50000, 103210 , 100366 )' ) -- 제 1 수정계곡
		dlg_menu( "@90606133\v#@price@#\v50000", 'RunTeleport( 50000 , 99757 , 103236 )' ) -- 제 2 수정계곡
		dlg_menu( "@90606134\v#@price@#\v50000", 'RunTeleport( 50000 , 132680 , 128030 )' ) -- 팔미르 유적 제 1실
		dlg_menu( "@90606135\v#@price@#\v50000", 'RunTeleport( 50000 , 137441 , 128115 )' ) -- 팔미르 유적 제 2실
		dlg_menu( "@90606136\v#@price@#\v150000", 'RunTeleport( 150000 , 146188 , 135579 )' ) -- 엘 카시아	
		dlg_menu( "@90606137\v#@price@#\v150000", 'RunTeleport( 150000 , 91958 , 117044 )' ) -- 백룡의 쉼터
		dlg_menu( "@90606138\v#@price@#\v150000", 'RunTeleport( 150000 , 85720 , 118033 )' ) -- 흑룡의 심장
		dlg_menu( "@90606139\v#@price@#\v150000", 'RunTeleport( 150000 , 92027 , 124430 )' ) -- 사룡의 심장
		dlg_menu( "@90606140\v#@price@#\v200000", 'RunTeleport( 200000 , 98965 , 129204 )' ) -- 큐브릭 던전
	--end                                                                                                    
   
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

   	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


function NPC_TeleportDungeon_katan()	-- 카탄
 
	-- 다이얼로그 출력
	dlg_title( "@90606146" )
	dlg_text( "@90606127" )

	--if get_value( "level" ) > 9 then
		dlg_menu( "@90606128\v#@price@#\v10000", 'RunTeleport( 10000 , 132995 , 87096 )' ) -- 메마른 달빛의 유적 제 1실
      	dlg_menu( "@90606129\v#@price@#\v10000", 'RunTeleport( 10000 , 130842 , 79586 )' ) -- 메마른 달빛의 유적 제 2실
		dlg_menu( "@90606130\v#@price@#\v50000", 'RunTeleport( 50000 , 155817, 103724 )' ) -- 잃어버린 갱도 제 1탄광
		dlg_menu( "@90606131\v#@price@#\v50000", 'RunTeleport( 50000, 152309 , 102886 )' ) -- 잃어버린 갱도 제 2탄광
		dlg_menu( "@90606132\v#@price@#\v50000", 'RunTeleport( 50000, 103210 , 100366 )' ) -- 제 1 수정계곡
		dlg_menu( "@90606133\v#@price@#\v50000", 'RunTeleport( 50000 , 99757 , 103236 )' ) -- 제 2 수정계곡
		dlg_menu( "@90606134\v#@price@#\v50000", 'RunTeleport( 50000 , 132680 , 128030 )' ) -- 팔미르 유적 제 1실
		dlg_menu( "@90606135\v#@price@#\v50000", 'RunTeleport( 50000 , 137441 , 128115 )' ) -- 팔미르 유적 제 2실
		dlg_menu( "@90606136\v#@price@#\v150000", 'RunTeleport( 150000 , 146188 , 135579 )' ) -- 엘 카시아	
		dlg_menu( "@90606137\v#@price@#\v150000", 'RunTeleport( 150000 , 91958 , 117044 )' ) -- 백룡의 쉼터
		dlg_menu( "@90606138\v#@price@#\v150000", 'RunTeleport( 150000 , 85720 , 118033 )' ) -- 흑룡의 심장
		dlg_menu( "@90606139\v#@price@#\v150000", 'RunTeleport( 150000 , 92027 , 124430 )' ) -- 사룡의 심장
		dlg_menu( "@90606140\v#@price@#\v200000", 'RunTeleport( 200000 , 98965 , 129204 )' ) -- 큐브릭 던전
	--end                                                                                                    
   
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

   	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


function NPC_TeleportDungeon_deva()	-- 라크시
 
	-- 다이얼로그 출력
	dlg_title( "@90606147" )
	dlg_text( "@90606127" )

	--if get_value( "level" ) > 9 then
		dlg_menu( "@90606128\v#@price@#\v10000", 'RunTeleport( 10000 , 132995 , 87096 )' ) -- 메마른 달빛의 유적 제 1실
      	dlg_menu( "@90606129\v#@price@#\v10000", 'RunTeleport( 10000 , 130842 , 79586 )' ) -- 메마른 달빛의 유적 제 2실
		dlg_menu( "@90606130\v#@price@#\v50000", 'RunTeleport( 50000 , 155817, 103724 )' ) -- 잃어버린 갱도 제 1탄광
		dlg_menu( "@90606131\v#@price@#\v50000", 'RunTeleport( 50000, 152309 , 102886 )' ) -- 잃어버린 갱도 제 2탄광
		dlg_menu( "@90606132\v#@price@#\v50000", 'RunTeleport( 50000, 103210 , 100366 )' ) -- 제 1 수정계곡
		dlg_menu( "@90606133\v#@price@#\v50000", 'RunTeleport( 50000 , 99757 , 103236 )' ) -- 제 2 수정계곡
		dlg_menu( "@90606134\v#@price@#\v50000", 'RunTeleport( 50000 , 132680 , 128030 )' ) -- 팔미르 유적 제 1실
		dlg_menu( "@90606135\v#@price@#\v50000", 'RunTeleport( 50000 , 137441 , 128115 )' ) -- 팔미르 유적 제 2실
		dlg_menu( "@90606136\v#@price@#\v150000", 'RunTeleport( 150000 , 146188 , 135579 )' ) -- 엘 카시아	
		dlg_menu( "@90606137\v#@price@#\v150000", 'RunTeleport( 150000 , 91958 , 117044 )' ) -- 백룡의 쉼터
		dlg_menu( "@90606138\v#@price@#\v150000", 'RunTeleport( 150000 , 85720 , 118033 )' ) -- 흑룡의 심장
		dlg_menu( "@90606139\v#@price@#\v150000", 'RunTeleport( 150000 , 92027 , 124430 )' ) -- 사룡의 심장
		dlg_menu( "@90606140\v#@price@#\v200000", 'RunTeleport( 200000 , 98965 , 129204 )' ) -- 큐브릭 던전
	--end                                                                                                    
   
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

   	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


function NPC_TeleportDungeon_rondo()	-- 론도
 
	-- 다이얼로그 출력
	dlg_title( "@90606125" )
	dlg_text( "@90606127" )

	--if get_value( "level" ) > 9 then
		dlg_menu( "@90606128\v#@price@#\v10000", 'RunTeleport( 10000 , 132995 , 87096 )' ) -- 메마른 달빛의 유적 제 1실
      	dlg_menu( "@90606129\v#@price@#\v10000", 'RunTeleport( 10000 , 130842 , 79586 )' ) -- 메마른 달빛의 유적 제 2실
		dlg_menu( "@90606130\v#@price@#\v50000", 'RunTeleport( 50000 , 155817, 103724 )' ) -- 잃어버린 갱도 제 1탄광
		dlg_menu( "@90606131\v#@price@#\v50000", 'RunTeleport( 50000, 152309 , 102886 )' ) -- 잃어버린 갱도 제 2탄광
		dlg_menu( "@90606132\v#@price@#\v50000", 'RunTeleport( 50000, 103210 , 100366 )' ) -- 제 1 수정계곡
		dlg_menu( "@90606133\v#@price@#\v50000", 'RunTeleport( 50000 , 99757 , 103236 )' ) -- 제 2 수정계곡
		dlg_menu( "@90606134\v#@price@#\v50000", 'RunTeleport( 50000 , 132680 , 128030 )' ) -- 팔미르 유적 제 1실
		dlg_menu( "@90606135\v#@price@#\v50000", 'RunTeleport( 50000 , 137441 , 128115 )' ) -- 팔미르 유적 제 2실
		dlg_menu( "@90606136\v#@price@#\v150000", 'RunTeleport( 150000 , 146188 , 135579 )' ) -- 엘 카시아	
		dlg_menu( "@90606137\v#@price@#\v150000", 'RunTeleport( 150000 , 91958 , 117044 )' ) -- 백룡의 쉼터
		dlg_menu( "@90606138\v#@price@#\v150000", 'RunTeleport( 150000 , 85720 , 118033 )' ) -- 흑룡의 심장
		dlg_menu( "@90606139\v#@price@#\v150000", 'RunTeleport( 150000 , 92027 , 124430 )' ) -- 사룡의 심장
		dlg_menu( "@90606140\v#@price@#\v200000", 'RunTeleport( 200000 , 98965 , 129204 )' ) -- 큐브릭 던전
	--end                                                                                                    
   
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

   	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end



function NPC_TeleportDungeon_secroute()	-- 시크루트
 
	-- 다이얼로그 출력
	dlg_title( "@90606126" )
	dlg_text( "@90606127" )

	--if get_value( "level" ) > 9 then
		dlg_menu( "@90606128\v#@price@#\v3000", 'RunTeleport( 3000 , 132995 , 87096 )' ) -- 메마른 달빛의 유적 제 1실
      	dlg_menu( "@90606129\v#@price@#\v3000", 'RunTeleport( 3000 , 130842 , 79586 )' ) -- 메마른 달빛의 유적 제 2실
		dlg_menu( "@90606130\v#@price@#\v3000", 'RunTeleport( 3000 , 155817, 103724 )' ) -- 잃어버린 갱도 제 1탄광
		dlg_menu( "@90606131\v#@price@#\v3000", 'RunTeleport( 3000, 152309 , 102886 )' ) -- 잃어버린 갱도 제 2탄광
		dlg_menu( "@90606132\v#@price@#\v3000", 'RunTeleport( 3000, 103210 , 100366 )' ) -- 제 1 수정계곡
		dlg_menu( "@90606133\v#@price@#\v3000", 'RunTeleport( 3000 , 99757 , 103236 )' ) -- 제 2 수정계곡
		dlg_menu( "@90606134\v#@price@#\v3000", 'RunTeleport( 3000 , 132680 , 128030 )' ) -- 팔미르 유적 제 1실
		dlg_menu( "@90606135\v#@price@#\v3000", 'RunTeleport( 3000 , 137441 , 128115 )' ) -- 팔미르 유적 제 2실
		dlg_menu( "@90606136\v#@price@#\v3000", 'RunTeleport( 3000 , 146188 , 135579 )' ) -- 엘 카시아	
		dlg_menu( "@90606137\v#@price@#\v3000", 'RunTeleport( 3000 , 91958 , 117044 )' ) -- 백룡의 쉼터
		dlg_menu( "@90606138\v#@price@#\v3000", 'RunTeleport( 3000 , 85720 , 118033 )' ) -- 흑룡의 심장
		dlg_menu( "@90606139\v#@price@#\v3000", 'RunTeleport( 3000 , 92027 , 124430 )' ) -- 사룡의 심장
		dlg_menu( "@90606140\v#@price@#\v3000", 'RunTeleport( 3000 , 98965 , 129204 )' ) -- 큐브릭 던전
	--end                                                                                                    
   
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

   	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================

-- 시크루트 일 때 (7005 텔레포터 유리에)
function NPC_TeleportTown_1_Secroute_contact()
 	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	
	-- 텔레포터 유리에
	if npc_id == 7005 then
		-- 다이얼로그 출력
		dlg_title( "@90700501" )
		dlg_text( "@90700502" )
	
	-- 텔레포터 야미
	elseif npc_id == 11237 then
	
		dlg_title( "@90999704" )
		dlg_text( "@90999705" )
	
	end
	
		
   -- 레벨이 10 이상이면 다른 도시(라크시, 카탄,호라이즌)로 이동가능
	--if get_value( "level" ) > 9 then	9이하 캐릭터도 자유롭게 이동가능 
		dlg_menu( "@90700505", 'RunTeleport( 0 , 6625 , 6980 )' )
		dlg_menu( "@90700506", 'RunTeleport( 0, 116799 , 58205 )' )
		dlg_menu( "@90700510", 'RunTeleport( 0, 153506 , 77175 )' )
		dlg_menu( "@90700511", 'RunTeleport( 2000 , 135609 , 104589 )' )
     		dlg_menu( "@90100519", 'RunTeleport( 2000 , 140101 , 102600 )' ) -- 크리쳐 농장
		dlg_menu( "@90700512", 'RunTeleport( 4000 , 152943 , 151081 )' )
	    dlg_menu( "@90019001", 'RunTeleport_Begin_TO_City( 0 , 172543 , 51847 )' )
	--end
	
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end
	
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

-- 시크루트 일 때 (7006 텔레포터 자쿠)
function NPC_TeleportTown_2_Secroute_contact()
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
		
		
	if is_premium() then
		
		-- 텔레포터 자쿠
		if npc_id == 7006 then
		
			-- 다이얼로그 출력
			dlg_title( "@90700601" )
			dlg_text( "@90700602" )
		
		-- 텔레포터 엠제이
		elseif npc_id == 11238 then
		
			dlg_title( "@90999707" )
			dlg_text( "@90999708" )
		
		end
	
		-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
		
		if is_premium() then
			-- 라크시 사냥터
			dlg_menu( "@90700613", 'NPC_TeleportTown_2_Secroute_Sub( 1 )' )
			-- 카탄 사냥터
			dlg_menu( "@90700616", 'NPC_TeleportTown_2_Secroute_Sub( 2 )' )
			-- 호라이즌 사냥터
			dlg_menu( "@90700619", 'NPC_TeleportTown_2_Secroute_Sub( 3 )' )
			-- 론도 사냥터
			dlg_menu( "@90700622", 'NPC_TeleportTown_2_Secroute_Sub( 4 )' )
		end
	else
		dlg_text( "@90700118" )
	end

	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end

	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


function NPC_TeleportTown_2_Secroute_Sub( select )
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	
	-- 텔레포터 자쿠
	if npc_id == 7006 then
	
		-- 다이얼로그 출력
		dlg_title( "@90700601" )
		dlg_text( "@90700602" )
	
	-- 텔레포터 엠제이
	elseif npc_id == 11238 then
	
		dlg_title( "@90700601" )
		dlg_text( "@90700602" )
	
	end	

	
	if is_premium() then
		-- 라크시 사냥터 (세이렌의 섬, 리저드맨 서식지)
		if select == 1 then
			dlg_menu( "@90700614\v#@price@#\v3000", 'RunTeleport( 3000 , 142086 , 132207 )' ) -- 세이렌의 섬
			dlg_menu( "@90700615\v#@price@#\v3000", 'RunTeleport( 3000 , 142013 , 147483 )' ) -- 리자드맨 서식지
			dlg_menu( "@90999311\v#@price@#\v4500", 'RunTeleport( 4500 , 120718 , 143033 )' ) -- 라크시 북쪽 삼거리
			dlg_menu( "@90999312\v#@price@#\v4500", 'RunTeleport( 4500 , 131613 , 136593 )' ) -- 라크시 동쪽 요정의 숲 출구
		-- 카탄 사냥터 (사이라그 폐허, 애도의 묘지)
		elseif select == 2 then
			dlg_menu( "@90700617\v#@price@#\v3000", 'RunTeleport( 3000 , 107700 , 76121 )' ) --사이라그 페허
			dlg_menu( "@90700618\v#@price@#\v3000", 'RunTeleport( 3000 , 124939 , 71987 )' ) --애도의 묘지
			dlg_menu( "@90999300\v#@price@#\v4500", 'RunTeleport( 4500 , 124736 , 57288 )' ) -- 카탄 필드 동쪽
		    dlg_menu( "@90999301\v#@price@#\v4500", 'RunTeleport( 4500 , 123179 , 65374 )' ) -- 카탄 북동쪽 통로 
		    dlg_menu( "@90999302\v#@price@#\v4500", 'RunTeleport( 4500 , 109591 , 58449 )' ) -- 카탄 서쪽 필드
		    dlg_menu( "@90999304\v#@price@#\v4500", 'RunTeleport( 4500 , 100576 , 82868 )' ) -- 단절의 장벽 입구
		-- 호라이즌 사냥터 (월화의 공동묘지, 제 1 발모어 탄광)
		elseif select == 3 then
			dlg_menu( "@90700620\v#@price@#\v3000", 'RunTeleport( 3000 , 139982 , 85162 )' ) --월하의 공동 묘지
			dlg_menu( "@90700621\v#@price@#\v3000", 'RunTeleport( 3000 , 155533 , 85778 )' ) -- 제 1 발모어 탄광
			dlg_menu( "@90999309\v#@price@#\v4500", 'RunTeleport( 4500 , 146894 , 77650 )' ) -- 호라이즌 서쪽 필드
			dlg_menu( "@90999310\v#@price@#\v4500", 'RunTeleport( 4500 , 155104 , 93975 )' ) -- 우거진 대나무 숲
		-- 론도 사냥터 (수정의 산, 팔미르 고원 입구, 칠흑의 숲)
		elseif select == 4 then
			dlg_menu( "@90700623\v#@price@#\v5000", 'RunTeleport( 5000 , 108976 , 103279 )' ) -- 수정의 산
			dlg_menu( "@90700624\v#@price@#\v5000", 'RunTeleport( 5000 , 134029 , 115307 )' ) -- 팔미르 고원 입구
			dlg_menu( "@90700625\v#@price@#\v5000", 'RunTeleport( 5000 , 150691 , 117877 )' ) -- 칠흑의 숲
			dlg_menu( "@90999306\v#@price@#\v7500", 'RunTeleport( 7500 , 129250 , 94073 )' ) -- 호라이즌 인근
			dlg_menu( "@90999307\v#@price@#\v7500", 'RunTeleport( 7500 , 123635 , 103436 )' ) -- 붉은 농장 가는길
			dlg_menu( "@90999308\v#@price@#\v7500", 'RunTeleport( 7500 , 150508 , 111503 )' ) -- 마르두카 감시탑
			dlg_menu( "@90999502\v#@price@#\v7500", 'RunTeleport( 7500 , 159670 , 124498 )' ) -- 마르두카 군락지 입구
			dlg_menu( "@90999503\v#@price@#\v7500", 'RunTeleport( 7500 , 83871 , 115911 )' ) -- 잃어버린 비밀의 섬	
			dlg_menu( "@90606233\v#@price@#\v7500", 'RunTeleport( 7500 , 162990 , 116317 )' ) -- 붉은 거미 서커스장 입구				
		end
	end
	--돌아가기
	dlg_menu( "@90010003", 'NPC_TeleportTown_2_Secroute_contact()' )

	dlg_menu( "@90010001", " " )
 
	dlg_show()
   
end

   --============================================================
   --             <<<<<< 시크루트 측 - 각 마을 파견 - NPC >>>>>>
   --============================================================

function NPC_TeleportSecroute_Town_contact()
 	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	
	-- <이름 출력> 각 NPC에 따라 적합한 
	-- 라크시 -> 시크루트 일 때 (1016 시크루트 텔레포터 롤라이)
	if npc_id == 1016 then
		dlg_title("@90101601")
	-- 카탄 -> 시크루트 일 때 (2016 시크루트 텔레포터 크리스)
	elseif npc_id == 2016 then
		dlg_title("@90201601")
	-- 수련자의 섬 -> 시크루트 일 때 (3024 시크루트 텔레포터 리아)
	elseif npc_id == 3024 then
		dlg_title("@90302401")
	-- 호라이즌 -> 시크루트 일 때 (4016 시크루트 텔레포터 제스)
	elseif npc_id == 4016 then
		dlg_title("@90401601")
	-- 론도 -> 시크루트 일 때 (6016 시크루트 텔레포터 티앙)
	elseif npc_id == 6016 then
		dlg_title("@90601601")
	-- 마레 마을 -> 시크루트 일 때 (7027 시크루트 텔레포터 시뮨)
	elseif npc_id == 7027 then
		dlg_title("@90702701")
	-- 도시 유적 -> 시크루트 일 때 (7040 시크루트 텔레포터 란코프)
	elseif npc_id == 7040 then
		dlg_title("@90704001")
		
	end
	
	-- <대사 출력> 각 NPC에 따라 적합한 
	-- 라크시 -> 시크루트 일 때 (1016 시크루트 텔레포터 롤라이)
	if npc_id == 1016 then
		dlg_text("@90101602")
	-- 카탄 -> 시크루트 일 때 (2016 시크루트 텔레포터 크리스)
	elseif npc_id == 2016 then
		dlg_text("@90201602")
	-- 수련자의 섬 -> 시크루트 일 때 (3024 시크루트 텔레포터 리아)
	elseif npc_id == 3024 then
		dlg_text("@90302402")
	-- 호라이즌 -> 시크루트 일 때 (4016 시크루트 텔레포터 제스)
	elseif npc_id == 4016 then
		dlg_text("@90401602")
	-- 론도 -> 시크루트 일 때 (6016 시크루트 텔레포터 티앙)
	elseif npc_id == 6016 then
		dlg_text("@90601602")
	-- 마레 마을 -> 시크루트 일 때 (7027 시크루트 텔레포터 시뮨)
	elseif npc_id == 7027 then
		dlg_text("@90702702")
	-- 도시 유적 -> 시크루트 일 때 (7040 시크루트 텔레포터 시뮨)
	elseif npc_id == 7040 then
		dlg_text("@90704002")
	end

	-- TO DO : 시크루트 마을 텔레포트 좌표 세팅
	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)

	if is_premium() then
		-- <시크루트로 텔레포트 하는 메뉴 출력> 각 NPC에 따라 
		-- 라크시 -> 시크루트 일 때 (1016 시크루트 텔레포터 롤라이)
		if npc_id == 1016 then
			dlg_menu("@90101603", 'RunTeleport_Secroute( 0 , 222175 , 17949 )')
		-- 카탄 -> 시크루트 일 때 (2016 시크루트 텔레포터 크리스)
		elseif npc_id == 2016 then
			dlg_menu("@90201603", 'RunTeleport_Secroute( 0 , 222175 , 17949 )')
		-- 수련자의 섬 -> 시크루트 일 때 (3024 시크루트 텔레포터 리아)
		elseif npc_id == 3024 then
			dlg_menu("@90302403", 'RunTeleport_Secroute( 0 , 222175 , 17949 )')
		-- 호라이즌 -> 시크루트 일 때 (4016 시크루트 텔레포터 제스)
		elseif npc_id == 4016 then
			dlg_menu("@90401603", 'RunTeleport_Secroute( 0 , 222175 , 17949 )')
		-- 론도 -> 시크루트 일 때 (6016 시크루트 텔레포터 티앙)
		elseif npc_id == 6016 then
			dlg_menu("@90601603", 'RunTeleport_Secroute( 0 , 222175 , 17949 )')
		-- 마레 마을 -> 시크루트 일 때 (7027 시크루트 텔레포터 시뮨)
		elseif npc_id == 7027 then
			dlg_menu("@90601603", 'RunTeleport_Secroute( 0 , 222175 , 17949 )')
		-- 도시 유적 -> 시크루트 일 때 (7040 시크루트 텔레포터 란코프)
		elseif npc_id == 7040 then
			dlg_menu("@90601603", 'RunTeleport_Secroute( 0 , 222175 , 17949 )')
		end
	end
	
	-- <서브메뉴> 각 NPC에 따라 
	-- 라크시 -> 시크루트 일 때 (1016 시크루트 텔레포터 롤라이)
	if npc_id == 1016 then
		dlg_menu("@90101611", 'NPC_TeleportSecroute_Town_sub( 1 )')
		dlg_menu("@90101613", 'NPC_TeleportSecroute_Town_sub( 2 )')
	-- 카탄 -> 시크루트 일 때 (2016 시크루트 텔레포터 크리스)
	elseif npc_id == 2016 then
		dlg_menu("@90201611", 'NPC_TeleportSecroute_Town_sub( 1 )')
		dlg_menu("@90201613", 'NPC_TeleportSecroute_Town_sub( 2 )')
	-- 수련자의 섬 -> 시크루트 일 때 (3024 시크루트 텔레포터 리아)
	elseif npc_id == 3024 then
		dlg_menu("@90302411", 'NPC_TeleportSecroute_Town_sub( 1 )')
		dlg_menu("@90302413", 'NPC_TeleportSecroute_Town_sub( 2 )')
	-- 호라이즌 -> 시크루트 일 때 (4016 시크루트 텔레포터 제스)
	elseif npc_id == 4016 then
		dlg_menu("@90401611", 'NPC_TeleportSecroute_Town_sub( 1 )')
		dlg_menu("@90401613", 'NPC_TeleportSecroute_Town_sub( 2 )')
	-- 론도 -> 시크루트 일 때 (6016 시크루트 텔레포터 티앙)
	elseif npc_id == 6016 then
		dlg_menu("@90601611", 'NPC_TeleportSecroute_Town_sub( 1 )')
		dlg_menu("@90601613", 'NPC_TeleportSecroute_Town_sub( 2 )')
	-- 마레 마을 -> 시크루트 일 때 (7027 시크루트 텔레포터 시뮨)
	elseif npc_id == 7027 then
		dlg_menu("@90601611", 'NPC_TeleportSecroute_Town_sub( 1 )')
		dlg_menu("@90601613", 'NPC_TeleportSecroute_Town_sub( 2 )')
	-- 도시 유적 -> 시크루트 일 때 (7040 시크루트 텔레포터 란코프)
	elseif npc_id == 7040 then
	dlg_menu("@90601611", 'NPC_TeleportSecroute_Town_sub( 1 )')
	dlg_menu("@90601613", 'NPC_TeleportSecroute_Town_sub( 2 )')
	end
	
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()

end

function NPC_TeleportSecroute_Town_sub( select )
 	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	
	-- <이름 출력> 각 NPC에 따라 적합한 
	-- 라크시 -> 시크루트 일 때 (1016 시크루트 텔레포터 롤라이)
	if npc_id == 1016 then
		dlg_title("@90101601")
	-- 카탄 -> 시크루트 일 때 (2016 시크루트 텔레포터 크리스)
	elseif npc_id == 2016 then
		dlg_title("@90201601")
	-- 수련자의 섬 -> 시크루트 일 때 (3024 시크루트 텔레포터 리아)
	elseif npc_id == 3024 then
		dlg_title("@90302401")
	-- 호라이즌 -> 시크루트 일 때 (4016 시크루트 텔레포터 제스)
	elseif npc_id == 4016 then
		dlg_title("@90401601")
	-- 론도 -> 시크루트 일 때 (6016 시크루트 텔레포터 티앙)
	elseif npc_id == 6016 then
		dlg_title("@90601601")
	-- 마레 마을 -> 시크루트 일 때 (7027 시크루트 텔레포터 시뮨)
	elseif npc_id == 7027 then
		dlg_title("@90702701")
	-- 도시 유적 -> 시크루트 일 때 (7040 시크루트 텔레포터 란코프)
	elseif npc_id == 7040 then
		dlg_title("@90704001")
	end
	
	-- 선택된 메뉴에 따라 <대사 출력>
	-- 시크루트에 대한 설명
	if select == 1 then
		-- 라크시 -> 시크루트 일 때 (1016 시크루트 텔레포터 롤라이)
		if npc_id == 1016 then
			dlg_text("@90101612")
		-- 카탄 -> 시크루트 일 때 (2016 시크루트 텔레포터 크리스)
		elseif npc_id == 2016 then
			dlg_text("@90201612")
		-- 수련자의 섬 -> 시크루트 일 때 (3024 시크루트 텔레포터 리아)
		elseif npc_id == 3024 then
			dlg_text("@90302412")
		-- 호라이즌 -> 시크루트 일 때 (4016 시크루트 텔레포터 제스)
		elseif npc_id == 4016 then
			dlg_text("@90401612")
		-- 론도 -> 시크루트 일 때 (6016 시크루트 텔레포터 티앙)
		elseif npc_id == 6016 then
			dlg_text("@90601612")
		-- 마레 마을 -> 시크루트 일 때 (7027 시크루트 텔레포터 시뮨)
		elseif npc_id == 7027 then
			dlg_text("@90601612")
		-- 도시 유적 -> 시크루트 일 때 (7040 시크루트 텔레포터 란코프)
		elseif npc_id == 7040 then
			dlg_text("@90601612")
		end
	-- 시크루트 프리패스에 대한 설명
	elseif select == 2 then
		-- 라크시 -> 시크루트 일 때 (1016 시크루트 텔레포터 롤라이)
		if npc_id == 1016 then
			dlg_text("@90101614")
		-- 카탄 -> 시크루트 일 때 (2016 시크루트 텔레포터 크리스)
		elseif npc_id == 2016 then
			dlg_text("@90201614")
		-- 수련자의 섬 -> 시크루트 일 때 (3024 시크루트 텔레포터 리아)
		elseif npc_id == 3024 then
			dlg_text("@90302414")
		-- 호라이즌 -> 시크루트 일 때 (4016 시크루트 텔레포터 제스)
		elseif npc_id == 4016 then
			dlg_text("@90401614")
		-- 론도 -> 시크루트 일 때 (6016 시크루트 텔레포터 티앙)
		elseif npc_id == 6016 then
			dlg_text("@90601614")
		-- 마레 마을 -> 시크루트 일 때 (7027 시크루트 텔레포터 시뮨)
		elseif npc_id == 7027 then
			dlg_text("@90601614")
		-- 도시 유적 -> 시크루트 일 때 (7040 시크루트 텔레포터 시뮨)
		elseif npc_id == 7040 then
			dlg_text("@90601614")
		end
	end
	
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()

end

   --============================================================
   --             <<<<<< 텔레포트 작동 (공통) >>>>>>
   --============================================================

function RunTeleport( cost , x_pos , y_pos )



	-- 상위펑션에서 호출한 비용을 체크
        -- 돈 모자라면 KIN
        local gold = get_value( "gold" )
        
        if gold < cost then

		-- 시스템메세지로 돈 부족하다고 출력
		message( "@90010008" )

		return
        end


	-- 돈 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()

	save()

	-- 상위 펑션에서 호출한 x, y좌표에 +100을 랜덤값으로 텔레포트함
	warp( x_pos + math.random(0,10) , y_pos + math.random(0,10) )

end


   --============================================================
   --             <<<<<< 텔레포트 작동 (마을에서 시크루트로 가는 전용) >>>>>>
   --============================================================

function RunTeleport_Secroute( cost , x_pos , y_pos )



	-- 상위펑션에서 호출한 비용을 체크
        -- 돈 모자라면 KIN
        local gold = get_value( "gold" )
        
        if gold < cost then

		-- 시스템메세지로 돈 부족하다고 출력
		message( "@90010008" )

		return
        end


	-- 돈 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()

	save()

	-- 상위 펑션에서 호출한 x, y좌표에 +100을 랜덤값으로 텔레포트함
	warp( x_pos + math.random(0,60) , y_pos + math.random(0,60) )

end



   --============================================================
   --             <<<<<< 초보자섬에서 종족도시로 >>>>>>
   --============================================================

function RunTeleport_Begin_TO_City( cost , x_pos , y_pos )



	-- 상위펑션에서 호출한 비용을 체크
        -- 돈 모자라면 KIN
        local gold = get_value( "gold" )
        
        if gold < cost then

		-- 시스템메세지로 돈 부족하다고 출력
		message( "@90010008" )

		return
        end


	-- 돈 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()

	-- 귀환지역을 대상도시로 설정
    --set_flag( "rx", x_pos + math.random(0,100))
    --set_flag( "ry", y_pos + math.random(0,100))

	save()

	-- 상위 펑션에서 호출한 x, y좌표에 +100을 랜덤값으로 텔레포트함
	warp( x_pos + math.random(0,100) , y_pos + math.random(0,100) )


end

-- 종족도시에서 수련자의 섬으로 텔레포트(공통)
function RunTeleport_City_To_Camp( cost , x_pos , y_pos )


	-- 상위펑션에서 호출한 비용을 체크
        -- 돈 모자라면 KIN
        local gold = get_value( "gold" )
        
        if gold < cost then

		-- 시스템메세지로 돈 부족하다고 출력
		message( "@90010008" )

		return
        end


	-- 돈 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()

	-- 귀환지역을 대상캠프로 설정
    set_flag( "rx", x_pos + math.random(0,10))
    set_flag( "ry", y_pos + math.random(0,10))

	save()

	-- 상위 펑션에서 호출한 x, y좌표에 +100을 랜덤값으로 텔레포트함
	warp( x_pos + math.random(0,100) , y_pos + math.random(0,100) )


end


 --============================================================
   --             <<<<<< 채널 텔레포트 작동 >>>>>>
   --============================================================

function Teleport_channel( channel_id )

	local start_channel, end_channel
					
	-- 이동 가능한 채널 갯수 확인 및 이동 가능할 경우의 시작 번호와 끝 번호의 변수 정의
	start_channel = 1 -- 이동 가능한 채널 번호의 시작번호
	end_channel = get_max_channel_num( channel_id ) -- 이동 가능한 채널 번호의 종료번호
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- 다이얼로그 출력
	-- 란슬롯 일 때
	if npc_id == 3016 then
		dlg_title("@90301601")
	-- 텔레포터 오시어 일 때
	elseif npc_id == 3005 then
		dlg_title("@90300501")
	-- 혹시나 그냥 오시어
	elseif npc_id == 3022 then
		dlg_title("@90300501")
	-- 마리캣 주인 마리캣!
	elseif npc_id == 11120 then
		dlg_title("@90999373")
	-- 시크루트 가이드 티미!
	elseif npc_id == 7008 then
		dlg_title("@90700801")
	end	

	-- 채널 갯수에 따른 다이얼로그 출력 분기 구성
	-- 이동 가능한 채널이 없을 경우의 다이얼로그 출력
	if end_channel == 1 then
		dlg_text( "@90300516" )
	elseif end_channel > 1 then
		text = sconv("@90300514", "#@number1@#", tostring( start_channel ) , "#@number2@#",tostring( end_channel ))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_text( text )       -- 대사창에 텍스트 삽입
	end
		
			
	--메뉴 구성 (이동 가능 여부에 따른 메뉴 구성)
			
	-- 채널이동이 불가능할 경우
	if end_channel == 1 then
		-- 돌아가기
		if npc_id == 3016 then
			dlg_menu( "@90010003", "NPC_Tutorial_Instructor_3_contact()" )
		-- 텔레포터 오시어 일 때
		elseif npc_id == 3005 then
			dlg_menu( "@90010003", "NPC_TeleportField_Beginner_contact()" )
		-- 혹시나 그냥 오시어
		elseif npc_id == 3022 then
			dlg_menu( "@90010003", "NPC_TeleportField_Beginner_contact()" )
		-- 마리캣 주인 마리캣!
		elseif npc_id == 11120 then
			dlg_menu( "@90010003", "NPC_maricat_market_maricat_contact()" )
		-- 시크루트 가이드 티미!
		elseif npc_id == 7008 then
			dlg_menu( "@90010003", "NPC_GuideTown_Secroute_contact()" )
		end	

	-- 채널이동이 가능할 경우
	elseif end_channel > 1 then
		-- 채널 번호 입력 텍스트박스 호출
		dlg_menu( "@90300513", "show_channel_set()" )
	end

		-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end

function on_channel_set( channel_number )

    local input_layer, player_layer, channel_id, start_channel, end_channel

    -- 임시 변수 선언과 동시에 NPC ID 가져오기
    local npc_id = get_npc_id()

    -- NPC 별 채널 ID 변수 및 다이얼로그 설정
    -- 란슬롯 일 때 ( 3016 )
    if npc_id == 3016 then
        channel_id = 1000
	    dlg_title("@90301601")
    -- 텔레포터 오시어 일 때 ( 3005 )
    elseif npc_id == 3005 then
        channel_id = 1000
        dlg_title("@90300501")
    -- 혹시나 그냥 오시어 ( 3022 )
    elseif npc_id == 3022 then
        channel_id = 1000
        dlg_title("@90300501")
    -- 공동묘지01 츄이일 때 ( 9982 )
	elseif npc_id == 9982 then
		channel_id = 110000
        dlg_title( "@90996801" )
	-- 공동묘지02 츄이일 때 ( 9983 )
	elseif npc_id == 9983 then
		channel_id = 90000
        dlg_title( "@90996801" )
	-- 공동묘지03 츄이일 때 ( 9984 )
	elseif npc_id == 9984 then
		channel_id = 70000
        dlg_title( "@90996801" )
	-- 나이트메어01 츄이일 때 ( 9985 )
	elseif npc_id == 9985 then
		channel_id = 120000
        dlg_title( "@90996801" )
	-- 나이트메어02 츄이일 때 ( 9986 )
	elseif npc_id == 9986 then
		channel_id = 100000
        dlg_title( "@90996801" )	
	-- 마리캣 주인 마리캣!
	elseif npc_id == 11120 then
		channel_id = 120400
		dlg_title("@90999373")
	-- 시크루트 가이드 티미!
	elseif npc_id == 7008 then
		channel_id = 130100
		dlg_title("@90700801")
    end	

    input_layer = get_layer_of_channel( channel_id, channel_number )
    player_layer = gv("layer")

    -- 아무도 없는 채널 번호를 입력했을 경우
	if get_user_count_in_channel( channel_id , channel_number ) == 0 then

        -- 이동 불가 관련 메세지 다이얼로그 호출
        dlg_text( "@90300518")

        -- 메뉴 구성
        -- 채널 번호 입력 텍스트박스 호출
        dlg_menu( "@90300513", "show_channel_set()" )		
        -- 대화종료 
        dlg_menu( "@90010002", " " )

        dlg_show()

        return

	end

	-- 현재 채널과 동일한 채널 번호를 입력한 경우
	if input_layer == player_layer then

		-- 이동 불가 관련 메세지 다이얼로그 호출
		dlg_text( "@90300519")

		-- 메뉴 구성
		-- 채널 번호 입력 텍스트박스 호출
		dlg_menu( "@90300513", "show_channel_set()" )
		-- 대화종료 
		dlg_menu( "@90010002", " " )

		dlg_show()

		return

	end

	-- 전혀 무관한 숫자 입력한 경우
	start_channel = get_min_channel_num( channel_id )   -- 이동 가능한 채널 번호의 최소번호
	end_channel = get_max_channel_num( channel_id )     -- 이동 가능한 채널 번호의 최대번호

	if channel_number < start_channel or channel_number > end_channel then
		message( "@90010093" )
		return
	end

	-- 이동 가능한 채널 번호를 입력한 경우 해당 채널로 텔레포트	
    -- 상위 펑션에서 호출한 x, y좌표에 +100을 랜덤값으로 텔레포트함
    warp( gv("x") + math.random(0,10) , gv("y") + math.random(0,10) , channel_id , channel_number )

end





   --============================================================
   --      <<<<<< 마리캣 마켓 공용 함수>>>>>>
   --============================================================

-- 마켓으로 보내주는 워프게이트
function warp_to_market()

	set_flag( 'mx', get_value('x') )
	set_flag( 'my', get_value('y') )
	
	local gate_num = math.random( 0, 2)
	
	-- 랜덤으로 워프 시킴
	if gate_num == 0 then
	warp( 200898 + math.random( 0, 5 ), 72983 + math.random( 0, 5 ) )
	
	elseif gate_num == 1 then
	warp( 202322 + math.random( 0, 5 ), 72989 + math.random( 0, 5 ) )
	
	elseif gate_num == 2 then
	warp( 201596 + math.random( 0, 5 ), 71744 + math.random( 0, 5 ) )
	end
	
end

-- 마을로 돌려 보내주는 워프게이트
function quit_market()

	local mx = get_flag( 'mx' )
	local my = get_flag( 'my' )
	
	if mx == nil or my == nil or mx == 0 or my == 0 or mx == '' or my == '' then
		mx = get_flag( 'rx' )
		my = get_flag( 'ry' )
	end
	
	if mx == nil or my == nil or mx == 0 or my == 0 or mx == '' or my == '' then
		mx = 185788
		my = 72361
	end
	
	del_flag( 'mx' )
	del_flag( 'my' )
	
	warp( mx + math.random( 0, 10 ), my + math.random( 0, 10 ) )
	
end	
	
-- 마리캣 마켓의 오벨리스크
function market_obelisk_tower()
	

	cprint("!오벨리스크 가동")
	local item_small_sujung_count = find_item( 1100303 )
	
	if item_small_sujung_count == 1 then
	
		delete_item ( get_item_handle( 1100303 ), item_small_sujung_count )		-- 작은 수정 삭제
		
	else

	end

end	
	

   --============================================================
   --      <<<<<< 마리캣 마켓 NPC>>>>>>
   --============================================================

   
   --============================================================
   --      <<<<<< 마켓주인 마리캣 >>>>>>
   --============================================================
function NPC_maricat_market_maricat_init()
	cprint( "!마켓주인 마리캣 가동" )
	set_npc_name(  "@90999372"  )
end
 
function NPC_maricat_market_maricat_contact()
 	
	-- 다이얼로그 출력
	dlg_title( "@90999373" )
	dlg_text( "@90999374" )

	-- 마리캣 마켓이란의 신상?
	dlg_menu( "@90999375", "maricat_talkLink_1()" )
	
	-- 마을로 돌아가기?
	dlg_menu( "@90999377", "maricat_talkLink_2()" )
	
	-- 마리캣의 신상?
	dlg_menu( "@90999379", "maricat_talkLink_3()" )
	
	-- 채널 이동 해드림(마리켓 마켓)
	dlg_menu( "@90300513", 'Teleport_channel( 120400 )')
	
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


-- 부가정보 1 	마리캣 - 마켓이란?
function maricat_talkLink_1()

	-- 다이얼로그 출력
	dlg_title( "@90999373" )

	dlg_text_without_quest_menu( "@90999376" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_maricat_market_maricat_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 2 	마리캣 - 마을 돌아가기
function maricat_talkLink_2()

	-- 다이얼로그 출력
	dlg_title( "@90999373" )

	dlg_text_without_quest_menu( "@90999378" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_maricat_market_maricat_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 3 	마리캣 - 마리캣이란?
function maricat_talkLink_3()

	-- 다이얼로그 출력
	dlg_title( "@90999373" )

	dlg_text_without_quest_menu( "@90999380" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_maricat_market_maricat_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


   --============================================================
   --      <<<<<< 마켓가드 파푸캣 >>>>>>
   --============================================================
function NPC_maricat_market_guard_init()
	cprint( "!마켓가드 파푸캣 가동" )
	set_npc_name(  "@90999381"  )
end
 
function NPC_maricat_market_guard_contact()
 	
	-- 다이얼로그 출력
	dlg_title( "@90999382" )
	dlg_text( "@90999383" )

	-- 마리캣의 신상?
	dlg_menu( "@90999384", "maricat_guard_talkLink_1()" )
	
	-- 마리캣 마켓이란??
	dlg_menu( "@90999386", "maricat_guard_talkLink_2()" )
	
	
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


-- 부가정보 1 	마켓 가드 - 마리캣이란?
function maricat_guard_talkLink_1()

	-- 다이얼로그 출력
	dlg_title( "@90999382" )

	dlg_text_without_quest_menu( "@90999385" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_maricat_market_guard_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 2 	마켓 가드 - 마리캣 마을은?
function maricat_guard_talkLink_2()

	-- 다이얼로그 출력
	dlg_title( "@90999382" )

	dlg_text_without_quest_menu( "@90999387" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_maricat_market_guard_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

   --============================================================
   --      <<<<<< 마켓 안내인 >>>>>>
   --============================================================

function NPC_maricat_market_teleport_contact()
	
	local npc_id = get_npc_id()

	
		if npc_id == 11122 then
			dlg_title( "@90999400" )
		elseif npc_id == 11123 then
			dlg_title("@90999404")
		elseif npc_id == 11124 then
			dlg_title("@90999402")
		elseif npc_id == 11125 then
			dlg_title("@90999389")
		elseif npc_id == 11126 then
			dlg_title("@90999398")
		elseif npc_id == 11127 then
			dlg_title("@90999396")
		elseif npc_id == 11240 then	 -- 마켓 안내인 코렛(시크루트)
			dlg_title("@90999702")
		end
		
	dlg_text( "@90999390" )
	
	-- 마켓 이동
	dlg_menu( "@90999405", "warp_to_market()" )

	-- 마켓 안내인의 신상?
	dlg_menu( "@90999391", "maricat_teleport_talkLink_1()" )
	
	-- 자유무역지구란??
	dlg_menu( "@90999393", "maricat_teleport_talkLink_2()" )
	
	
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


-- 부가정보 1 	마켓 안내인 - 마켓 안내인이란?
function maricat_teleport_talkLink_1()

	local npc_id = get_npc_id()

	
		if npc_id == 11122 then
			dlg_title( "@90999400" )
		elseif npc_id == 11123 then
			dlg_title("@90999404")
		elseif npc_id == 11124 then
			dlg_title("@90999402")
		elseif npc_id == 11125 then
			dlg_title("@90999389")
		elseif npc_id == 11126 then
			dlg_title("@90999398")
		elseif npc_id == 11127 then
			dlg_title("@90999396")
		end

	dlg_text_without_quest_menu( "@90999392" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_maricat_market_teleport_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 2 	마켓 안내인 - 자유무역지구?
function maricat_teleport_talkLink_2()

	local npc_id = get_npc_id()

	
		if npc_id == 11122 then
			dlg_title( "@90999400" )
		elseif npc_id == 11123 then
			dlg_title("@90999404")
		elseif npc_id == 11124 then
			dlg_title("@90999402")
		elseif npc_id == 11125 then
			dlg_title("@90999389")
		elseif npc_id == 11126 then
			dlg_title("@90999398")
		elseif npc_id == 11127 then
			dlg_title("@90999396")
		end

	dlg_text_without_quest_menu( "@90999394" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_maricat_market_teleport_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

   --============================================================
   --      <<<<<< 라마단 텔레포터 >>>>>>
   --============================================================

function NPC_TeleportTown_ramadan_contact()
	
	local npc_id = get_npc_id()

		--라크시 텔레포터
		if npc_id == 11753 then
			dlg_title( "@90605374" )
		--카탄
		elseif npc_id == 11754 then
			dlg_title("@90605376")
		--호라이즈
		elseif npc_id == 11755 then
			dlg_title("@90605378")
		--론도
		elseif npc_id == 11756 then
			dlg_title("@90605380")
		end
		
	dlg_text( "@90605386" )
	
	-- 라마단 기도실 이동
	dlg_menu( "@90605383", "warp_to_ramadan()" )

	-- 라마단 기도실이란
	--dlg_menu( "@90999391", "ramadan_teleport_talkLink_1()" )
	
	-- 라마단 기도의상
	--dlg_menu( "@90999393", "ramadan_teleport_talkLink_2()" )
	
	
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end

function NPC_TeleportTown_warpback_contact()
	
	local npc_id = get_npc_id()

		--라마단 기도실 텔레포터
		if npc_id == 11757 then
			dlg_title( "@90605382" )
		end
		
	dlg_text( "@90605389" )
	
	-- 마을로 이동
	dlg_menu( "@90605390", "quit_ramadan()" )

	-- 라마단 기도실이란
	--dlg_menu( "@90999391", "ramadan_teleport_talkLink_1()" )
	
	-- 라마단 기도의상
	--dlg_menu( "@90999393", "ramadan_teleport_talkLink_2()" )
	
	
	dlg_menu( "@90010001", " " )
 
	dlg_show()
 
end


-- 라마단 기도실로 보내주는 워프게이트
function warp_to_ramadan()

	--변신 관련 지속효과
	 --4505변신<예티>
	 --4506변신<샐러맨더>
	 --4507변신<토깽이>
	 --4508변신<크리스탈>
	 --4509변신<마인샤프트>
	 --4528변신<세이렌>
	 --4529변신<오크>
	 --4530변신<스켈레톤>
	 --4531변신<블루픽시>
	 --4532변신<예티 프라임>
	 --4533변신<아발란체>
	 --4534변신<샐러맨더 크레센트>
	 --4535변신<샐러맨더 킹>
	 --4536변신<세이렌 레이디>
	 --4537변신<세이렌 퀸>
	 --4538변신<오크 워리어>
	 --4539변신<오크 로드>
	 --4540변신<스켈레톤 워리어>
	 --4541변신<스켈레톤 나이트>
	 --4542변신<아쿠아 픽시>
	 --4543변신<오션 페어리>
	 --4550변신<유령>
	 --4555변신<큐브>
	 --4556변신<크루드 큐브>
	 --4557변신<네오 큐브>
	 --13754야크 강림
	 --164001생명의 융합
	 --164003금지된 생명의 융합
	 --6513생명의 융합<시스템>
	 --6522금지된 생명의 융합<시스템>

	local state_id_polymorph = { 4505, 4506, 4507, 4508, 4509, 4528, 4529, 4530, 4531, 4532,
		4533, 4534, 4535, 4536, 4537, 4538, 4539, 4540, 4541, 4542, 4543, 4550, 4555, 4556, 4557,
		164001, 164003, 6513, 6522 }
	
	for i = 1, table.getn( state_id_polymorph )
	do

		if get_state_level( state_id_polymorph[ i ] ) > 0 then

			cprint("@90605391")
			return

		end
	end
	
	set_flag( 'ramdanx', get_value('x') )
	set_flag( 'ramdany', get_value('y') )
		
	warp( 24927 + math.random( 0, 50 ), 8778 + math.random( 0, 50 ), 0 )
	
end

-- 마을로 돌려 보내주는 워프게이트
function quit_ramadan()

	local ramdanx = get_flag( 'ramdanx' )
	local ramdany = get_flag( 'ramdany' )
		
	if ramdanx == nil or ramdany == nil or ramdanx == 0 or ramdany == 0 or ramdanx == '' or ramdany == '' then
		ramdanx = get_flag( 'rx' )
		ramdany = get_flag( 'ry' )
	end
	
	del_flag( 'ramdanx' )
	del_flag( 'ramdany' )
	
	warp( ramdanx + math.random( 0, 10 ), ramdany + math.random( 0, 10 ), 0 )
	
end	
		