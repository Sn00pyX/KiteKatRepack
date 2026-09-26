-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_TeleportField"
end

   -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.


   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================

function NPC_TeleportField_Deva_init()
	cprint( "!텔레포터 오드리 가동 (필드)" )
	set_npc_name( "@90100600"  )
end
 

function NPC_TeleportField_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90100601" )
	dlg_text( "@90100602" )

	dlg_menu( "@90100603", 'RunTeleport( 0 , 6625 , 6980 )' )
	dlg_menu( "@90999311\v#@price@#\v15000", 'RunTeleport( 15000 , 120718 , 143033 )' ) -- 라크시 북쪽 삼거리
	dlg_menu( "@90999312\v#@price@#\v27000", 'RunTeleport( 27000 , 131613 , 136593 )' ) -- 라크시 동쪽 요정의 숲 출구
	dlg_menu( "@90100505\v#@price@#\v35000", 'RunTeleport( 35000 , 132546 , 139965 )' ) -- 마법 실험지	
	dlg_menu( "@90700615\v#@price@#\v63000", 'RunTeleport( 63000 , 142013 , 147483 )' ) -- 리자드맨 서식지
	dlg_menu( "@90100504\v#@price@#\v61000", 'RunTeleport( 61000 , 139011 , 141396 )' ) -- 템플러 헤드쿼터
	dlg_menu( "@90700614\v#@price@#\v60000", 'RunTeleport( 60000 , 142086 , 132207 )' ) -- 세이렌의 섬	
	--g_menu( "@160028001", 'RunTeleport( 0 ,124443 , 134010 )' )                                   
		-- RunTeleport 함수는 NPC_TeleportTown.lua 파일에 정의되어있슈
		
	dlg_menu( "@90010001", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 중간 캠프 테스트 텔레포트 >>>>>>
   --============================================================
   
 
-- 라크시 중간 캠프 텔레포터 테스트1
function NPC_Teleportfield_Deva_middleconnect_test_contact()
 	
	-- 다이얼로그 출력
	dlg_title( "@90100501" )
	dlg_text( "@90100502" )

	--dlg_menu( "@90100507", 'Binding_Deva_001()' )
	dlg_menu( "@90100503", 'RunTeleport( 0 ,122934 , 138140 )' )

   --end

	dlg_menu( "@90010001", " " )
 	dlg_show()
 
end

--============================================================
   --             <<<<<< 중간 캠프 기본 설정 >>>>>>
   --============================================================
   
 
-- 라크시 중간 캠프 텔레포터 테스트1
function NPC_Teleportfield_init_middlecamp_guide_contact()
 	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	-- 다이얼로그 출력
	
	-- 중간 캠프 엘리트 사냥꾼 존스<120>
		if npc_id == 11002 then
			dlg_title("@90999106")
			dlg_text( "@90999105" )
	-- 중간 캠프 엘리트 사냥꾼 게일<130>	
	elseif npc_id == 11003 then
			dlg_title("@90999107")
			dlg_text( "@90999105" )	
	--중간 캠프 엘리트 사냥꾼 에르켄<160,170>
		elseif npc_id == 11007 then
			dlg_title("@90999108")
			dlg_text( "@90999105" )	
		-- 중간캠프 카탄 3시	
		elseif npc_id == 11058 then
			dlg_title("@90999315")
	    -- 중간캠프 카탄 1시
		elseif npc_id == 11059 then
			dlg_title("@90999316")
		-- 중간캠프 카탄 10시 
		elseif npc_id == 11060 then
			dlg_title("@90999317")
		-- 중간캠프 어쌔씬 길드
		elseif npc_id == 11061 then
			dlg_title("@90999318")
		-- 중간캠프 카탄 서쪽 사막 입구
		elseif npc_id == 11062 then
			dlg_title("@90999319")
		-- 중간캠프 오아시스
		elseif	npc_id == 11063 then
			dlg_title("@90999320")
		-- 중간캠프 론도 7시
		elseif npc_id == 11064 then
			dlg_title("@90999321")			
		-- 중간캠프 론도 9시
		elseif npc_id == 11065 then
			dlg_title("@90999322")
		-- 중간캠프 론도 2시
		elseif npc_id == 11066 then
			dlg_title("@90999323")
		-- 중간캠프 호라이즌 10시
		elseif npc_id == 11067 then
			dlg_title("@90999324")
		-- 중간캠프 호라이즌 12시
		elseif npc_id == 11068 then
			dlg_title("@90999325")
		-- 중간캠프 라크시 11시
		elseif npc_id == 11069 then
			dlg_title("@90999326")			
		-- 중간캠프 라크시 북쪽 1시
		elseif npc_id == 11070 then
			dlg_title("@90999327")
	   	-- 중간캠프 라크시 4시
		elseif npc_id == 11071 then
			dlg_title("@90999328")
		end	
		
		-- 2차 중간 캠프 텔레포터 공통 대사
		if npc_id > 11057 and npc_id < 11072 then
			dlg_text( "@90999314" )		
		end		
		
	--dlg_menu( "@90100507", 'Binding_Deva_001()' )	

   --end
	
	-- 중간캠프 카탄 10시 
	if	 npc_id == 11060 then
		dlg_menu( "@90999303\v#@price@#\v42000", 'RunTeleport( 42000 , 101497 , 69731 )' ) -- 중간캠프 어쌔씬 길드
	-- 중간캠프 론도 7시
	elseif npc_id == 11064 then
		dlg_menu( "@90999305\v#@price@#\v51000", 'RunTeleport( 51000 , 113326 , 88329 )' ) -- 중간캠프 오아시스
	-- 중간캠프 라크시 11시
	elseif npc_id == 11069 then
		dlg_menu( "@90999312\v#@price@#\v30000", 'RunTeleport( 30000 , 127680 , 149692 )' ) -- 중간캠프 라크시 북쪽 1시	
	end
	   
	dlg_menu( "@90010001", " " )
 	dlg_show()
 
end
