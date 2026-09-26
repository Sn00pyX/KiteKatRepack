-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_Dungeon"
end

   --============================================================
   --             <<<<<< 필드 NPC >>>>>>
   --============================================================


function NPC_Dungeon_Teleport_In_contact()

 	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
	-- 906 = 잃어버린 갱도 	(906001	텔레포터 므네)
	if npc_id == 906001 then
		dlg_title("@1090600101")
	-- 606 = 수정 계곡 			(606001	텔레포터 세이삼)
	elseif npc_id == 606001 then
		dlg_title("@1060600101")
	-- 705 = 열사의 유적 		(705001	텔레포터 소라)
	elseif npc_id == 705001 then
		dlg_title("@1070500101")
	end -- if 끝
		
	-- <대사 출력> 각 NPC에 따라 적합한 
	-- 906 = 잃어버린 갱도 	(906001	텔레포터 므네)
	if npc_id == 906001 then
		dlg_text("@1090600102")
	-- 606 = 수정 계곡 			(606001	텔레포터 세이삼)
	elseif npc_id == 606001 then
		dlg_text("@1060600102")
	-- 705 = 열사의 유적 		(705001	텔레포터 소라)
	elseif npc_id == 705001 then
		dlg_text("@1070500102")
	end -- if 끝

	-- <메뉴출력> 각 NPC에 따라 적합한 
	-- 906 = 잃어버린 갱도 	(906001	텔레포터 므네)
	if npc_id == 906001 then
		dlg_menu( "@1090600103", 'RunTeleport( 0 , 219233 , 14804 )' )
		if get_env( "game.dungeon_siege" ) ~= 0 then
			if is_enterable_siege_dungeon( 130000 ) == 1 then
				dlg_menu( "@1090600104", 'warp_to_siege_dungeon( 130000 )' )
			end
		end

	-- 606 = 수정 계곡 			(606001	텔레포터 세이삼)
	elseif npc_id == 606001 then
		dlg_menu( "@1060600103", 'RunTeleport( 0 , 219233 , 63188 )' )
		if get_env( "game.dungeon_siege" ) ~= 0 then
			if is_enterable_siege_dungeon( 130300 ) == 1 then
				dlg_menu( "@1060600104", 'warp_to_siege_dungeon( 130300 )' )
			end
		end

	-- 705 = 열사의 유적 		(705001	텔레포터 소라)
	elseif npc_id == 705001 then
		dlg_menu( "@1070500103", 'RunTeleport( 0 , 219233 , 47060 )' )
		if get_env( "game.dungeon_siege" ) ~= 0 then
			if is_enterable_siege_dungeon( 130200 ) == 1 then
				dlg_menu( "@1070500104", 'warp_to_siege_dungeon( 130200 )' )
			end
		end

	end -- if 끝


	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 던전 NPC >>>>>>
   --============================================================

function NPC_Dungeon_Teleport_Out_contact()

 	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
	-- 906 = 잃어버린 갱도 	(906002	텔레포터 련군)
	if npc_id == 906002 then
		dlg_title("@1090600201")
	-- 606 = 수정 계곡 			(606002	텔레포터 나츠)
	elseif npc_id == 606002 then
		dlg_title("@1060600201")
	-- 705 = 열사의 유적 		(705002	텔레포터 쿠라)
	elseif npc_id == 705002 then
		dlg_title("@1070500201")
	end -- if 끝
		
	-- <대사 출력> 각 NPC에 따라 적합한 
	-- 906 = 잃어버린 갱도 	(906002	텔레포터 련군)
	if npc_id == 906002 then
		dlg_text("@1090600202")
	-- 606 = 수정 계곡 			(606002	텔레포터 나츠)
	elseif npc_id == 606002 then
		dlg_text("@1060600202")
	-- 705 = 열사의 유적 		(705002	텔레포터 쿠라)
	elseif npc_id == 705002 then
		dlg_text("@1070500202")
	end -- if 끝

	-- <메뉴출력> 각 NPC에 따라 적합한 
	
	-- 906 = 잃어버린 갱도 	(906002	텔레포터 련군)
	if npc_id == 906002 then
		dlg_menu( "@1090600203", 'RunTeleport( 0 , 156300 , 103726 )' )

	-- 606 = 수정 계곡 			(606002	텔레포터 나츠)
	elseif npc_id == 606002 then
		dlg_menu( "@1060600203", 'RunTeleport( 0 , 103161 , 100932 )' )

	-- 705 = 열사의 유적 		(705002	텔레포터 쿠라)
	elseif npc_id == 705002 then
		dlg_menu( "@1070500203", 'RunTeleport( 0 , 122889 , 84560 )' )

	end -- if 끝

	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 엘 카시아 고대 유적탐사단 타미 >>>>>>
   --============================================================
   
function NPC_elkassia_tami_contact()
	
	dlg_title("@90999425")
	dlg_text("@90999426")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


   --============================================================
   --             <<<<<< 엘 카시아 금안족 카멜 >>>>>>
   --============================================================
   
function NPC_elkassia_kamel_contact()
	
	dlg_title("@90999428")
	dlg_text("@90999429")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

--================================================================
--------------------------- 엘카시아 퀘스트 ----------------------
--================================================================


   --============================================================
   --             <<<<<< 엘 카시아 탐사대원 몰리 >>>>>>
   --============================================================
   
 
function NPC_quest_elkassia_moly_contact()
 
   --local quest_progress3012 = get_quest_progress(3012) -- 드웬을 위한 진혼 #3
      
    dlg_title("@90999966")
	dlg_text("@90999967")
	
	--if quest_progress3012 == 1 then
	--	-- 드웬의 옷 구하기
	--	dlg_menu( "@90999941", "moly_talklink_1()" )
	--end
			
			
	-- 종료
	dlg_menu( "@90010002", '' )

	dlg_show()
    
end

function moly_talklink_1()

	dlg_title("@90999966")
	dlg_text("@90999942")
	
	--드웬의 옷 카운트 구하기
	local item_elkassia_dcloth_count = find_item ( 1000100 )
	
	if item_elkassia_dcloth_count >= 1 then

		for i = 0, 10 do
			-- 드웬의 옷 남은 아이템 없애기
			delete_item( get_item_handle( 1000100 ), item_elkassia_dcloth_count )
		end
		
	else
		-- 드웬의 옷 넣어주기
		insert_item( 1000100, 1 )
		cprint( "@90999943" ) -- <#6DD66D>드웬의 옷를 받았습니다.
	end
	
	
		
	--종료
	dlg_menu( "@90010002", '' )
	dlg_show()


end

   --============================================================
   --             <<<<<< 엘 카시아 경비대원 베니 >>>>>>
   --============================================================


function NPC_quest_elkassia_benny_contact()
 
   
    dlg_title("@90999951")
	dlg_text("@90999952")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
    
end


   --============================================================
   --             <<<<<< 엘 카시아 탐사대장 브리트나 >>>>>>
   --============================================================


function NPC_quest_elkassia_breetna_contact()
 
   
    dlg_title("@91000101")
	dlg_text("@91000102")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
    
end


   --============================================================
   --             <<<<<< 엘 카시아 무녀 키샤 >>>>>>
   --============================================================


function NPC_quest_elkassia_kisha_contact()
 
   
    dlg_title("@91000104")
	dlg_text("@91000105")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
    
end


--================================================================
------------------------- 퀘스트 링크 스크립트--------------------
--================================================================


-- 배달 요청_칠흑의 숲 초소
-- 소포 인벤토리에 넣어주기
function quest_start_3000()
		
	--소포 카운트 구하기
	local item_elkassia_package_count = find_item ( 1000094 )
	
	if item_elkassia_package_count >= 1 then
		for i = 0, 10 do
			-- 소포 남은 아이템 없애기
			delete_item( get_item_handle( 1000094 ), item_elkassia_package_count )
		end		
	else
		 -- 소포 넣어주기
		insert_item( 1000094, 1 )
		cprint( "@90999931" ) -- <#6DD66D>소포를 받았습니다.		 
	end
end


-- 배달 요청_칠흑의 숲 초소
-- 소포 인벤토리에서 삭제
function quest_end_3000()
	--소포 카운트 구하기
	local item_elkassia_package_count = find_item ( 1000094 )	
	if item_elkassia_package_count >= 1 then
		for i = 0, 10 do
			-- 소포 남은 아이템 없애기
			delete_item( get_item_handle( 1000094 ), item_elkassia_package_count )
		end		
		cprint( "@90999932" ) -- <#6DD66D>소포를 건네주었습니다.
	end
end
------------------------------------------------------------------------------------------
-- 탐사대 방문
-- 베니의 선물 인벤토리에 넣어주기
function quest_start_3001()
	--베니의 선물 카운트 구하기
	local item_elkassia_bennygift_count = find_item ( 1000095 )	
	if item_elkassia_bennygift_count >= 1 then
		for i = 0, 10 do
			-- 베니의 선물 남은 아이템 없애기
			delete_item( get_item_handle( 1000095 ), item_elkassia_bennygift_count )
		end
	else
		-- 베니의 선물 넣어주기
		insert_item( 1000095, 1 )
		cprint( "@90999933" ) -- <#6DD66D>베니의 선물를 받았습니다.	
	end
end

-- 탐사대 방문
-- 베니의 선물 인벤토리에서 삭제
function quest_end_3001()		
	--베니의 선물 카운트 구하기
	local item_elkassia_bennygift_count = find_item ( 1000095 )	
	if item_elkassia_bennygift_count >= 1 then
		for i = 0, 10 do
			-- 베니의 선물 남은 아이템 없애기
			delete_item( get_item_handle( 1000095 ), item_elkassia_bennygift_count )
		end		
		cprint( "@90999934" ) -- <#6DD66D>베니의 선물를 건네주었습니다.
	end
end


------------------------------------------------------------------------------------------
-- 열정과 무모함
function quest_start_3003()				
	add_state( 1013, 10, 30000) -- 이동속도 증가( 10레벨 , 30초)
	cprint( "@90999935" ) -- <#6DD66D>탐사대원을 위한 켈베로스의 기민함이 느껴집니다.		
end
------------------------------------------------------------------------------------------
function quest_start_3005()	
	add_state( 1011, 10, 30000) -- 공격속도 증가( 10레벨 , 300초)
	cprint( "@90999936" ) -- <#6DD66D>탐사대원을 위한 샐러맨더의 강인함이 느껴집니다.
end
------------------------------------------------------------------------------------------
-- 보고서 전달
-- 보고서 인벤토리에 넣어주기
function quest_start_3006()
	--보고서 카운트 구하기
	local item_elkassia_report_count = find_item ( 1000098 )	
	if item_elkassia_report_count >= 1 then
		for i = 0, 10 do
			-- 보고서 남은 아이템 없애기
			delete_item( get_item_handle( 1000098 ), item_elkassia_report_count )
		end		
	else
		-- 보고서 넣어주기
		insert_item( 1000098, 1 )
		cprint( "@90999937" ) -- <#6DD66D>보고서를 받았습니다.	
	end
end
-- 보고서 전달
-- 보고서 인벤토리에서 삭제
function quest_end_3006()		
	--보고서 카운트 구하기
	local item_elkassia_report_count = find_item ( 1000098 )	
	if item_elkassia_report_count >= 1 then
		for i = 0, 10 do
			-- 보고서 남은 아이템 없애기
			delete_item( get_item_handle( 1000098 ), item_elkassia_report_count )
		end		
		cprint( "@90999938" ) -- <#6DD66D>보고서를 건네주었습니다.
	end
end
------------------------------------------------------------------------------------------
function quest_end_3007()
	cprint( "@90999939" ) -- <#6DD66D>단검을 살피는 브리트나
end
------------------------------------------------------------------------------------------
function quest_start_3008()	
	--단검 카운트 구하기
	local item_elkassia_shortSword_count = find_item ( 1000099 )	
	if item_elkassia_shortSword_count >= 1 then
		for i = 0, 10 do
			-- 단검 남은 아이템 없애기
			delete_item( get_item_handle( 1000099 ), item_elkassia_shortSword_count )
		end	
	else
		-- 보고서 넣어주기
		insert_item( 1000099, 1 )
		cprint( "@91000060" ) -- <ep7><#6DD66D>단검을 받았습니다.		
	end
end

function quest_end_3008()
	cprint( "@91000061" ) -- <ep7><#6DD66D>단검을 보여 주었습니다.
end
------------------------------------------------------------------------------------------
function quest_end_3010()
	--단검 카운트 구하기
	local item_elkassia_shortSword_count = find_item ( 1000099 )	
	if item_elkassia_shortSword_count >= 1 then
		for i = 0, 10 do
			-- 단검 남은 아이템 없애기
			delete_item( get_item_handle( 1000099 ), item_elkassia_shortSword_count )
		end		
		cprint( "@90999940" ) -- <ep7><#6DD66D>단검을 건네주었습니다.
	end
end
------------------------------------------------------------------------------------------
function quest_start_3013()	
	--드웬의 옷 카운트 구하기
	local item_elkassia_dcloth_count = find_item ( 1000100 )	
	if item_elkassia_dcloth_count >= 1 then
		for i = 0, 10 do
			-- 드웬의 옷 남은 아이템 없애기
			delete_item( get_item_handle( 1000100 ), item_elkassia_dcloth_count )
		end	
	else
		-- 드웬의 옷 넣어주기
		insert_item( 1000100, 1 )
		cprint( "@90999943" ) -- <#6DD66D>드웬의 옷를 받았습니다.		
	end
end

function quest_end_3013()
	cprint( "@90999944" ) -- <ep7><#6DD66D>드웬의 옷을 건네주었습니다.
end
------------------------------------------------------------------------------------------
function quest_start_3014()
	--단검 카운트 구하기
	local item_elkassia_shortSword_count = find_item ( 1000099 )	
	if item_elkassia_shortSword_count >= 1 then
		for i = 0, 10 do
			-- 단검 남은 아이템 없애기
			delete_item( get_item_handle( 1000099 ), item_elkassia_shortSword_count )
		end
	else 
		-- 단검 넣어주기
		insert_item( 1000099, 1 )
		cprint( "@91000060" ) -- <ep7><#6DD66D>단검을 받았습니다.		
	end
end

function quest_end_3014()
	--단검 카운트 구하기
	local item_elkassia_shortSword_count = find_item ( 1000098 )	
	if item_elkassia_shortSword_count >= 1 then
		for i = 0, 10 do
			-- 단검 남은 아이템 없애기
			delete_item( get_item_handle( 1000098 ), item_elkassia_shortSword_count )
		end		
		cprint( "@90999940" ) -- <ep7><#6DD66D>단검을 건네주었습니다.
	end
end
------------------------------------------------------------------------------------------
function quest_start_3019()	
	--비상 탐지기 카운트 구하기
	local item_elkassia_detector_count = find_item ( 1000103 )
	if item_elkassia_detector_count >= 1 then
		for i = 0, 10 do
			-- 비상 탐지기 남은 아이템 없애기
			delete_item( get_item_handle( 1000103 ), item_elkassia_detector_count )
		end		
	else
		-- 비상 탐지기 넣어주기
		insert_item( 1000103, 1 )
		cprint( "@90999945" ) -- <#6DD66D>비상 탐지기를 받았습니다.		
	end
end

function quest_end_3019()
	add_state( 6004, 6, 6000) -- 출혈 ( 6레벨 , 60초)
	add_state( 6006, 6, 1000) -- 스턴 ( 6레벨 , 10초)
	cprint( "@91000062" ) -- <ep7><#6DD66D>강력한 물리력에 의해 초소아래로 밀려났습니다.
	--초소 아래로 워프
	warp( 161722 + math.random(0,10) , 128654+ math.random(0,10), gv("layer") )	
	cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.	
	-- 광신도 소환
	add_npc( 160716 + math.random( 2, 100), 128365 + math.random( 2, 100), 141010,  1 + math.random( 1, 3) , 18000 )
	add_npc( 160716 + math.random( 2, 100), 128365 + math.random( 2, 100), 141011,  1 + math.random( 1, 3) , 18000 )
	add_npc( 160716 + math.random( 1, 30), 128365 + math.random( 1, 30), 140007,  1 , 18000 )
end
------------------------------------------------------------------------------------------
function quest_start_3020()	
	add_state( 6004, 6, 6000) -- 출혈 ( 6레벨 , 60초)
	add_state( 6006, 6, 1000) -- 스턴 ( 6레벨 , 10초)
	cprint( "@91000062" ) -- <ep7><#6DD66D>강력한 물리력에 의해 초소아래로 밀려났습니다.	
	
	--초소 아래로 워프
	warp( 161722 + math.random(0,10) , 128654+ math.random(0,10), gv("layer") )	
	cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	
	-- 광신도 소환
	add_npc( 160716 + math.random( 2, 100), 128365 + math.random( 2, 100), 144013,  1 , 30000)
	add_npc( 160716 + math.random( 2, 100), 128365 + math.random( 2, 100), 141010,  1 + math.random( 2, 4) , 30000)
	add_npc( 160716 + math.random( 2, 100), 128365 + math.random( 2, 100), 141011,  1 + math.random( 2, 4) , 30000)	
end
------------------------------------------------------------------------------------------
function quest_end_3021()
	--탐지기 카운트 구하기
	local item_elkassia_detector_count = find_item ( 1000103 )
	
	if item_elkassia_detector_count >= 1 then
		for i = 0, 10 do
			-- 탐지기 남은 아이템 없애기
			delete_item( get_item_handle( 1000103 ), item_elkassia_detector_count )
		end		
		cprint( "@90999946" ) -- <#6DD66D>비상 탐지기를 건네주었습니다.
	end
end
------------------------------------------------------------------------------------------
--		엘카시아 퀘스트 프랍	 --
------------------------------------------------------------------------------------------

-- 열정과 무모함
function quest_prop_3003()

	add_state( 6003, 6, 6000) -- 중독 ( 6레벨 , 60초)
	cprint( "@91000063" ) -- <ep7><#6DD66D>부패하였습니다. 악취가 진동합니다.

	
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3003 = get_quest_progress(3003) -- <ep7>열정과 무모함
	local quest_progress3007 = get_quest_progress(3007) -- <ep7>드웬의 시체 정밀 조사
	
	-- 드웬의 소지품
	local item_elkassia_dwen_3003 = find_item ( 1000097 )
	local item_elkassia_dwen_3003_handle = get_item_handle ( 1000097 )	
	
	-- 피묻은 단검
	local item_elkassia_bloodsword_3007 = find_item ( 1000099 )
	local item_elkassia_bloodsword_3007_handle = get_item_handle ( 1000099 )	
	
	if quest_progress3003 == 1 then	
		for i = 0, 10 do
			-- 남은 아이템 없애기
			delete_item( get_item_handle( 1000097 ), item_elkassia_dwen_3003 )
		end		
	-- 드웬의 소지품 넣어주기
		insert_item( 1000097, 1 )	
		cprint( "@81003004" ) -- <ep7>싸늘해져 버린 드웬의 소지품을 몰리에게 전달하자.	
	end
	
	if quest_progress3007 == 1 then	
		for i = 0, 10 do
			-- 남은 아이템 없애기
			delete_item( get_item_handle( 1000099 ), item_elkassia_bloodsword_3007 )
		end		
		-- 피묻은 단검 넣어주기
		insert_item( 1000099, 1 )
		cprint( "@91000064" ) -- <ep7>단검을 획득하였습니다.
		cprint( "@90999930" ) -- <ep7><#6DD66D>드웬의 시체를 조사해보았다.<br><br>자세히 보니 시체는 왼쪽 손목이 잘려져 나갔다. 마음을 굳게 먹고, 시체를 뒤집으니 배에 단검이 꽂혀있다.
	end
end

-- 드웬의 시체 정밀 조사(시체 프랍 스트링)
function quest_prop_3007()
	cprint( "@90999930" ) -- <ep7><#6DD66D>드웬의 시체를 조사해보았다.<br><br>자세히 보니 시체는 왼쪽 손목이 잘려져 나갔다. 마음을 굳게 먹고, 시체를 뒤집으니 배에 단검이 꽂혀있다.
end

function quest_prop_3018()
	cprint( "@91000065" ) -- <ep7>수상한 단서품을 획득하였습니다.
end

------------------------------------------------------------------------------------------
-------------------------------	 메인 퀘스트 파트2_집착	  --------------------------------
------------------------------------------------------------------------------------------

-- 지역 진입-------------------------------------------------------------------------------

-- 마법 실험지 지역
function mainquest2_region_magicfield_level1()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3205 = get_quest_progress(3205) -- <(version:7.2)>[집착]잔당처리 #1
	local quest_progress_3206 = get_quest_progress(3206) -- <(version:7.2)>[집착]잔당처리 #2
	--local shout = math.random(1, 9)
		
	if quest_progress_3205 == 1 then
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 123008,  1 + math.random( 1, 4) , 36000 )
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 123009,  1 + math.random( 1, 3) , 36000 )	
		
		cprint( "@90999875" ) -- <#6DD66D> 몬스터가 나타났다.	
	elseif quest_progress_3206 == 1 then
	
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 126010,  1 + math.random( 1, 4) , 36000 )
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 126011,  1 + math.random( 1, 3) , 36000 )	
		
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 129010,  1 + math.random( 1, 2) , 36000 )
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 131010,  1 + math.random( 1, 2) , 36000 )
		
		cprint( "@90999875" ) -- <#6DD66D> 몬스터가 나타났다.
	end
end

-- 사이라그 폐허 지역
function mainquest2_region_siragruins_level2()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3205 = get_quest_progress(3205) -- <(version:7.2)>[집착]잔당처리 #1
	local quest_progress_3206 = get_quest_progress(3206) -- <(version:7.2)>[집착]잔당처리 #2
	--local shout = math.random(1, 9)
		
	if quest_progress_3205 == 1 then
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 123008,  1 + math.random( 1, 4) , 36000 )
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 123009,  1 + math.random( 1, 3) , 36000 )	
		
		cprint( "@90999875" ) -- <#6DD66D> 몬스터가 나타났다.	
	elseif quest_progress_3206 == 1 then
	
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 126010,  1 + math.random( 1, 4) , 36000 )
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 126011,  1 + math.random( 1, 3) , 36000 )	
		
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 129010,  1 + math.random( 1, 2) , 36000 )
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 131010,  1 + math.random( 1, 2) , 36000 )
		
		cprint( "@90999875" ) -- <#6DD66D> 몬스터가 나타났다.
	end
end

-- 에스포에 -------------------------------------------------------------------------------

-- 에스포에 1_2지역 
function mainquest2_region_espoir_level_10101()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3217 = get_quest_progress(3217) -- <(version:7.2)>[집착]목격자 #2
	--local quest_progress_3206 = get_quest_progress(3206) -- <(version:7.2)>[집착]잔당처리 #2
	--local shout = math.random(1, 9)
		
	if quest_progress_3217 == 1 then		
		set_quest_status( 3217, 1, 1 )
		cprint( "@91000797" ) -- <(version:7.2)><#6DD66D>퀘스트 목표 달성!	
	end
end

-- 에스포에 1_1지역 
function mainquest2_region_espoir_level_10102()

	
end

-- 에스포에 1_1지역 
function mainquest2_region_espoir_level_10103()

	
end


-- 에스포에 1_1지역 
function mainquest2_region_espoir_level_10104()

	
end


-- 에스포에 1_1지역 
function mainquest2_region_espoir_level_10105()

	
end


-- 에스포에 1_1지역 
function mainquest2_region_espoir_level_10106()

	
end


-- 에스포에 1_1지역 
function mainquest2_region_espoir_level_10107()

	
end


-- 에스포에 1_1지역 
function mainquest2_region_espoir_level_10108()

	
end


-- 에스포에 1_1지역 
function mainquest2_region_espoir_level_10109()

	
end




-- 에스포에 2지역 
function mainquest2_region_espoir_level_10120()

	
end


-- 에스포에 2_1지역 
function mainquest2_region_espoir_level_10121()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3225 = get_quest_progress(3225) -- <(version:7.2)>[집착]바늘호수
	--local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	--local shout = math.random(1, 9)
		
	if quest_progress_3225 == 1 then
		set_quest_status( 3225, 1, 1 )
	end
	
end

-- 에스포에 2지역 
function mainquest2_region_espoir_level_10122()

	
end

-- 에스포에 2지역 
function mainquest2_region_espoir_level_10123()

	
end

-- 에스포에 2지역 
function mainquest2_region_espoir_level_10124()

	
end

-- 에스포에 2지역 
function mainquest2_region_espoir_level_10125()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10130()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10131()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3236 = get_quest_progress(3236) -- <(version:7.2)>[집착]마녀 실험체 #1
		
	--local shout = math.random(1, 9)
	
	if quest_progress_3236 == 1 then
		
		add_npc( 27916 + math.random( 2, 100), 128365 + math.random( 2, 100), 171004,  1  , 54000 )
		cprint( "@91000795" ) -- <(version:7.2)><#6DD66D>거대한 마나의 기운으로 공간이 뒤틀린다. 몬스터 소환
			
	end
	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10132()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10133()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10134()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3238 = get_quest_progress(3238) -- <(version:7.2)>[집착]마녀 실험체 #2
		
	--local shout = math.random(1, 9)
	
	--if quest_progress_3238 == 1 then
		
		--add_npc( 25475 + math.random( 2, 100), 27698 + math.random( 2, 100), 175005,  1  , 108000 )
		--cprint( "@91000795" ) -- <(version:7.2)><#6DD66D>거대한 마나의 기운으로 공간이 뒤틀린다. 몬스터 소환
			
	--end
	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10135()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10136()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10137()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3241 = get_quest_progress(3241) -- <(version:7.2)>[집착]사탄소녀의 반대
		
	--local shout = math.random(1, 9)
	local stun_num = get_flag( "warp_num" )
	
	
	if quest_progress_3241 == 1 then
		
		add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 153008,  1 , 600000 )
		cprint( "@91000779" ) -- <(version:7.2)><#6DD66D>사탄소녀가 고대 마법을 이용해 마법을 사용합니다.
		
		set_flag( "warp_num", get_flag( "warp_num" ) + 1 )
		
		if stun_num > 3 then
			--스턴 12초
			add_state(6006, 5, 1200)
		elseif stun_num > 8 then
			--스턴 52초
			add_state(6006, 5, 15200)
		end
		
	end		
	
	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10138()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3241 = get_quest_progress(3241) -- <(version:7.2)>[집착]사탄소녀의 반대
		
	--local shout = math.random(1, 9)
	
	if quest_progress_3241 == 1 then
		
		add_npc( 27976  + math.random( 0, 15), 25307 + math.random( 0, 15), 153008,  1 , 600000 )
		add_npc( 27925  + math.random( 0, 15), 25291 + math.random( 0, 15), 153008,  1 , 600000 )
		add_npc( 27871  + math.random( 0, 15), 25289 + math.random( 0, 15), 153008,  1 , 600000 )
		--add_npc( 27802  + math.random( 0, 15), 25298 + math.random( 0, 15), 153008,  1 , 600000 )
		--add_npc( 27906  + math.random( 0, 15), 25305 + math.random( 0, 15), 153008,  1 , 600000 )
		cprint( "@91000779" ) -- <(version:7.2)><#6DD66D>사탄소녀가 고대 마법을 이용해 마법을 사용합니다.
	end		
	
	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10139()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10140()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10141()

	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10142()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3233 = get_quest_progress(3233) -- <(version:7.2)>[집착]패닉
	--local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	--local shout = math.random(1, 9)
		
	if quest_progress_3233 == 1 then
		set_quest_status( 3233, 1, 1 )
		add_npc( 30159  + math.random( 0, 25), 27757 + math.random( 0, 25), 165013,  2 , 60000 )
		add_npc( 30166  + math.random( 0, 25), 27610 + math.random( 0, 25), 165012,  4 , 60000 )
	end
	
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10143()
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3227 = get_quest_progress(3227) -- <(version:7.2)>[집착]대면
		
	if quest_progress_3227 == 1 then
		set_quest_status( 3227, 1, 1 )
	end
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10144()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3227 = get_quest_progress(3227) -- <(version:7.2)>[집착]대면
		
	if quest_progress_3227 == 1 then
		set_quest_status( 3227, 1, 1 )
	end
end

-- 에스포에 3지역 
function mainquest2_region_espoir_level_10145()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3227 = get_quest_progress(3227) -- <(version:7.2)>[집착]대면
		
	if quest_progress_3227 == 1 then
		set_quest_status( 3227, 1, 1 )
	end	
end

-- 에스포에 4지역 
function mainquest2_region_espoir_level_10150()

	
end

-- 에스포에 4지역 
function mainquest2_region_espoir_level_10151()

	
end

-- 에스포에 4지역 
function mainquest2_region_espoir_level_10152()

	
end

-- 몬스터---------------------------------------------------------------------------------
------------------------------------------------------------------------------------------

-- 마법 실험지 지역
function mainquest2_monster_area8_8()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3205 = get_quest_progress(3205) -- <(version:7.2)>[집착]잔당처리 #1
	local quest_progress_3206 = get_quest_progress(3206) -- <(version:7.2)>[집착]잔당처리 #2
	local shout = math.random(1, 9)
		
	if quest_progress_3205 == 1 then
		if shout > 3 then
				cprint( "@91000787" ) -- <(version:7.2)><#6DD66D>‘살려주세요~’
			else
				cprint( "@91000792" ) -- <(version:7.2)><#6DD66D> 서글픈 눈으로 바라보며 ‘살려주세요~’
		end
	
	elseif quest_progress_3206 == 1 then
	
		if shout > 3 then
			cprint( "@91000787" ) -- <(version:7.2)><#6DD66D>‘살려주세요~’
		else
			cprint( "@91000792" ) -- <(version:7.2)><#6DD66D> 서글픈 눈으로 바라보며 ‘살려주세요~’
		end
	end
end

-- 사이라그 폐허 지역
function mainquest2_monster_area6_4()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3205 = get_quest_progress(3205) -- <(version:7.2)>[집착]잔당처리 #1
	local quest_progress_3206 = get_quest_progress(3206) -- <(version:7.2)>[집착]잔당처리 #2
	local shout = math.random(1, 9)
		
	if quest_progress_3205 == 1 then
		if shout > 3 then
				cprint( "@91000787" ) -- <(version:7.2)><#6DD66D>‘살려주세요~’
			else
				cprint( "@91000792" ) -- <(version:7.2)><#6DD66D> 서글픈 눈으로 바라보며 ‘살려주세요~’
		end
	
	elseif quest_progress_3206 == 1 then
	
		if shout > 3 then
			cprint( "@91000787" ) -- <(version:7.2)><#6DD66D>‘살려주세요~’
		else
			cprint( "@91000792" ) -- <(version:7.2)><#6DD66D> 서글픈 눈으로 바라보며 ‘살려주세요~’
		end
	end
end


-- 에스포에 1_1지역
function mainquest2_monster_area1_1()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3212 = get_quest_progress(3212) -- [집착]주변 탐색
	local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	local shout = math.random(1, 7)

	if quest_progress_3212 == 1 then
	cprint( "@91000786" ) -- <(version:7.2)><#6DD66D>“ΣΥΦΨΩασξμκφ”
	
	elseif quest_progress_3216 == 1 then
	
		if shout > 3 then
			cprint( "@91000787" ) -- <(version:7.2)><#6DD66D>‘살려주세요~’
		else
			cprint( "@91000788" ) -- <(version:7.2)><#6DD66D>‘살려주세요~ 살려주세요~’
		end
	end
end

-- 에스포에 1_2지역
function mainquest2_monster_area1_2()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3212 = get_quest_progress(3212) -- [집착]주변 탐색
	local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	local shout = math.random(1, 7)

	if quest_progress_3212 == 1 then
	cprint( "@91000786" ) -- <(version:7.2)><#6DD66D>“ΣΥΦΨΩασξμκφ”
	
	elseif quest_progress_3216 == 1 then
	
		if shout > 3 then
			cprint( "@91000787" ) -- <(version:7.2)><#6DD66D>‘살려주세요~’
		else
			cprint( "@91000788" ) -- <(version:7.2)><#6DD66D>‘살려주세요~ 살려주세요~’
		end
	end
end

-- 에스포에 3_1지역 사탄소녀
function mainquest2_monster_demongirl()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3241 = get_quest_progress(3241) -- <(version:7.2)>[집착]사탄소녀의 반대
	--local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	local shout = math.random(1, 9)
		
	if quest_progress_3241 == 1 then
	
		if shout == 1 or shout == 2 or shout == 3 or shout == 4 then
			cprint( "@91000789" ) -- <(version:7.2)><#6DD66D>부활실은 가지 말아요. 너무 위험해요..
		elseif shout == 5 or shout == 6 or shout == 7 or shout == 8 then
			cprint( "@91000790" ) -- <(version:7.2)><#6DD66D>날 못보게 되어도 아무 상관없는 건가요? 
		elseif shout == 9 then
			cprint( "@91000791" ) -- <(version:7.2)><#6DD66D>어차피 그렇게 가버릴 사람..
		end
	end
end














-- NPC------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
-- 에스포에 푸크레
function NPC_mainquest2_pucre_contact()
 
    dlg_title("@91000781")
	dlg_text("@91000782")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 	dlg_show()
    
end

-- 에스포에 레카트론
function NPC_mainquest2_recatron_contact()
    
    dlg_title("@91000784")
	dlg_text("@91000785")

		
	-- 종료
	dlg_menu( "@90010002", '' )
 	dlg_show()
    
end

-- 에스포에 폐기물 더미
function NPC_mainquest2_wastes_contact()
    
    dlg_title("@91000771")
	dlg_text("@91000772")

		
	-- 종료
	dlg_menu( "@90010002", '' )
 	dlg_show()
    
end

-- 메인 퀘스트 파트2_집착_프랍
------------------------------------------------------------------------------------------

-- <(version:7.2)>아공간의 균열
function quest_prop_warp_espoir()
    
	-- 에스포에로 워프 
	warp( 20801 + math.random(0,10) , 27527+ math.random(0,10), gv("layer") )
	cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
		
	
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3211 = get_quest_progress(3211) -- <(version:7.2)>[집착]아공간 진입 완료
	
	if quest_progress_3211 == 1 then
	
		-- <(version:7.2)>[집착]아공간 진입 완료
		set_quest_status( 3211, 1, 1 )
	
	end	
		
end

-- <(version:7.2)>빛과 기억의 무지개
function quest_prop_lightprism()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3228 = get_quest_progress(3228) -- <(version:7.2)>[집착]진실 #1
	local quest_progress_3229 = get_quest_progress(3229) -- <(version:7.2)>[집착]진실 #2
	local quest_progress_3230 = get_quest_progress(3230) -- <(version:7.2)>[집착]진실 #3
	local quest_progress_3231 = get_quest_progress(3231) -- <(version:7.2)>[집착]진실 #4
	local quest_progress_3232 = get_quest_progress(3232) -- <(version:7.2)>[집착]진실 #5
	--local shout = math.random(1, 9)
		
	if quest_progress_3228 == 1 then
		set_quest_status( 3228, 1, 1 )
		
	elseif quest_progress_3229 == 1 then
		set_quest_status( 3229, 1, 1 )
		
	elseif quest_progress_3230 == 1 then
		set_quest_status( 3230, 1, 1 )
	
	elseif quest_progress_3231 == 1 then
		set_quest_status( 3231, 1, 1 )
		
	elseif quest_progress_3232 == 1 then
		set_quest_status( 3232, 1, 1 )
		
	end
end

-- <(version:7.2)>폐기물 더미
function quest_prop_wastes_dummy()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3218 = get_quest_progress(3218) -- <(version:7.2)>[집착]목격자 #3
	--local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	local shout = math.random(1, 9)
	local count = find_item( 540073 )
    
	-- 퀘스트 수행중
	if quest_progress_3218 == 1 then
		if get_flag( "demongirl" ) > 5 then
			set_quest_status( 3218, 1, 1 )
			del_flag( "demongirl" )
		else 
			set_flag( "demongirl", get_flag( "demongirl" ) + 1 )
		end
	
	-- 퀘스트 완료 후 사탄 소녀 카드 받기
	elseif quest_progress_3218 == 255 and quest_progress_3219 == 255 then
			
		if count == 0 then
			insert_item( 540073, 1 )
			cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
		end
	
	--퀘스트 수행 조건 미달
	elseif quest_progress_3218 == -1 then	
		
		if shout > 4 then
			add_state( 6003, 5, 1200) -- 중독( 10레벨 , 12초)
			cprint( "@91000775" ) -- <(version:7.2)><#6DD66D>폐기물에 심각한 독성에 타격을 입었다.
		else
			add_state( 6003, 15, 1500) -- 중독( 10레벨 , 12초)
			cprint( "@91000775" ) -- <(version:7.2)><#6DD66D>폐기물에 심각한 독성에 타격을 입었다.
		end
	end
	
end

-- <(version:7.2)>폐기물
function quest_prop_wastes()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3218 = get_quest_progress(3218) -- <(version:7.2)>[집착]목격자 #3
	--local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	local shout = math.random(1, 9)
	local count = find_item( 540073 )
    
	-- 퀘스트 수행중
	if quest_progress_3218 == 1 then
		if get_flag( "demongirl" ) > 8 then
			set_quest_status( 3218, 1, 1 )
			del_flag( "demongirl" )
		else 
			set_flag( "demongirl", get_flag( "demongirl" ) + 1 )
		end
	
	-- 퀘스트 완료 후 사탄 소녀 카드 받기
	elseif quest_progress_3218 == 255 and quest_progress_3219 == 255 then
			
		if count == 0  then
			insert_item( 540073, 1 )
			cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
		end
	
	--퀘스트 수행 조건 미달
	elseif quest_progress_3218 == -1 then	
		
		if shout > 4 then
			add_state( 6003, 3, 1100) -- 중독( 10레벨 , 12초)
			cprint( "@91000775" ) -- <(version:7.2)><#6DD66D>폐기물에 심각한 독성에 타격을 입었다.
		else
			add_state( 6003, 12, 1300) -- 중독( 10레벨 , 12초)
			cprint( "@91000775" ) -- <(version:7.2)><#6DD66D>폐기물에 심각한 독성에 타격을 입었다.
		end
	end
	
	
end

-- <(version:7.2)>기억의 은총
function quest_prop_holly_memory()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3224 = get_quest_progress(3224) -- <(version:7.2)>[집착]기억의 은총 #2
	--local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	--local shout = math.random(1, 9)
		
	if quest_progress_3224 == 1 then
		set_quest_status( 3224, 1, 1 )
	end
    
end

-- <(version:7.2)>부활의 심장
function quest_prop_resurrection_heart()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3243 = get_quest_progress(3243) -- <(version:7.2)>[집착]부활의 심장 탐색
	--local quest_progress_3216 = get_quest_progress(3216) -- [집착]목격자 #1
	--local shout = math.random(1, 9)
		
	if quest_progress_3243 == 1 then
		set_quest_status( 3243, 1, 1 )
	end
	
end


-- <(version:7.2)>고문서 
function quest_prop_ancient_paper()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3235 = get_quest_progress(3235) -- <(version:7.2)>[집착]헥토르 전기
	local quest_progress_3237 = get_quest_progress(3237) -- <(version:7.2)>[집착]날조된 문서
	local quest_progress_3239 = get_quest_progress(3239) -- <(version:7.2)>[집착]마녀 루시앙
	
	local shout = math.random(1, 9)
	
	if shout > 4 then
		--if quest_progress_3235 == 1 then
		--	set_quest_status( 3235, 1, 1 )
			
			--warp( 27912 + math.random(0,10) , 28160+ math.random(0,10), gv("layer") )
			--cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
			
		if quest_progress_3237 == 1 then
			set_quest_status( 3237, 1, 1 )
			
			--warp( 27447 + math.random(0,10) , 27696+ math.random(0,10), gv("layer") )
			--cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
			
		--elseif quest_progress_3239 == 1 then
		--	set_quest_status( 3239, 1, 1 )
					
		end
	end
end


-- <(version:7.2)>고문서 뭉치
function quest_prop_ancient_paperroll()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3235 = get_quest_progress(3235) -- <(version:7.2)>[집착]헥토르 전기
	local quest_progress_3237 = get_quest_progress(3237) -- <(version:7.2)>[집착]날조된 문서
	local quest_progress_3239 = get_quest_progress(3239) -- <(version:7.2)>[집착]마녀 루시앙
	
	local shout = math.random(1, 9)
	
	if shout > 4 then
		--if quest_progress_3235 == 1 then
		--	set_quest_status( 3235, 1, 1 )
			
			--warp( 27912 + math.random(0,10) , 28160+ math.random(0,10), gv("layer") )
			--cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
			
		if quest_progress_3237 == 1 then
			set_quest_status( 3237, 1, 1 )
			
			--warp( 27447 + math.random(0,10) , 27696+ math.random(0,10), gv("layer") )
			--cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
			
		--elseif quest_progress_3239 == 1 then
		--	set_quest_status( 3239, 1, 1 )
					
		end
	end
	
end


-- <(version:7.2)>고대 역사서
function quest_prop_ancient_historybook()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3235 = get_quest_progress(3235) -- <(version:7.2)>[집착]헥토르 전기
	local quest_progress_3237 = get_quest_progress(3237) -- <(version:7.2)>[집착]날조된 문서
	local quest_progress_3239 = get_quest_progress(3239) -- <(version:7.2)>[집착]마녀 루시앙
	
	local shout = math.random(1, 9)
	
	if shout > 6 then
		--if quest_progress_3235 == 1 then
		--	set_quest_status( 3235, 1, 1 )
			
			--warp( 27912 + math.random(0,10) , 28160+ math.random(0,10), gv("layer") )
			--cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
			
		--elseif quest_progress_3237 == 1 then
		--	set_quest_status( 3237, 1, 1 )
			
			--warp( 27447 + math.random(0,10) , 27696+ math.random(0,10), gv("layer") )
			--cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
			
		if quest_progress_3239 == 1 then
			set_quest_status( 3239, 1, 1 )
					
		end
	end
	
end


-- <(version:7.2)>고대 기록서
function quest_prop_ancient_memorybook()    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3235 = get_quest_progress(3235) -- <(version:7.2)>[집착]헥토르 전기
	local quest_progress_3237 = get_quest_progress(3237) -- <(version:7.2)>[집착]날조된 문서
	local quest_progress_3239 = get_quest_progress(3239) -- <(version:7.2)>[집착]마녀 루시앙
	
	local shout = math.random(1, 9)
	
	if shout > 6 then
		if quest_progress_3239 == 1 then
			set_quest_status( 3239, 1, 1 )					
		end
	end	
end


-- <(version:7.2)>고문서 책장
function quest_prop_ancient_bookcase()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3235 = get_quest_progress(3235) -- <(version:7.2)>[집착]헥토르 전기
	local quest_progress_3237 = get_quest_progress(3237) -- <(version:7.2)>[집착]날조된 문서
	local quest_progress_3239 = get_quest_progress(3239) -- <(version:7.2)>[집착]마녀 루시앙
	
	local shout = math.random(1, 9)
	
	if shout > 5 then
		if quest_progress_3239 == 1 then
			set_quest_status( 3239, 1, 1 )
					
		end
	end
	
end


-- <(version:7.2)>헥토르 전기
function quest_prop_ancient_hector_wallpicture()    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3235 = get_quest_progress(3235) -- <(version:7.2)>[집착]헥토르 전기
	local quest_progress_3237 = get_quest_progress(3237) -- <(version:7.2)>[집착]날조된 문서
	local quest_progress_3239 = get_quest_progress(3239) -- <(version:7.2)>[집착]마녀 루시앙
	
	local shout = math.random(1, 9)	
	
	if quest_progress_3235 == 1 then
		set_quest_status( 3235, 1, 1 )
	end	
end
--워프 게이트-----------------------------------------------
--<(version:7.2)>가이아 대륙으로<1지역>
function quest_prop_warp_espoir1_0()    
	warp( 139023 + math.random(0,10) , 141656+ math.random(0,10), gv("layer") )
	cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.	
end

-- <(version:7.2)>2지역으로<1지역>
function quest_prop_warp_espoir1_2()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3220 = get_quest_progress(3220) -- <(version:7.2)>[집착]공간이동
		
	if quest_progress_3220 == 1 then
		set_quest_status( 3220, 1, 1 )	
	else
		warp( 21174 + math.random(0,10) , 22021+ math.random(0,10), gv("layer") )
		cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.		
	end	
end

-- <(version:7.2)>1지역으로<2지역>
function quest_prop_warp_espoir2_1()
    
	warp( 19447 + math.random(0,10) , 29318+ math.random(0,10), gv("layer") )
	cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
	
end

-- <(version:7.2)>3지역으로<2지역>
function quest_prop_warp_espoir2_3()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	warp( 27909 + math.random(0,10) , 27357+ math.random(0,10), gv("layer") )
	cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
	
end

-- <(version:7.2)>2지역으로<3지역>
function quest_prop_warp_espoir3_2()
    
	warp( 21186 + math.random(0,10) , 19372+ math.random(0,10), gv("layer") )
	cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
	
end

-- <(version:7.2)>4지역으로<3지역>
function quest_prop_warp_espoir3_4()
    
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3241 = get_quest_progress(3241) -- <(version:7.2)>[집착]사탄소녀의 반대
	local quest_progress_3242 = get_quest_progress(3242) -- <(version:7.2)>[집착]부활실 입성

	local stun_num = get_flag( "warp_num" )
	
	if quest_progress_3241 == 0 then
		
		warp( 27915 + math.random(0,10) , 26228 + math.random(0,10), gv("layer") )
		cprint( "@91000794" ) -- <(version:7.2)><#6DD66D>사탄 소녀가 뒤로 밀친다.
		
		force_start_quest(3241, 81003241)
		
		--스턴 12초
		add_state(6006, 5, 1200)
	
		set_flag( "warp_num", 0 )
		
			
	elseif quest_progress_3241 == 1 then
		
		warp( 27915 + math.random(0,10) , 26228 + math.random(0,10), gv("layer") )
		cprint( "@91000794" ) -- <(version:7.2)><#6DD66D>사탄 소녀가 뒤로 밀친다.
		
	
	elseif quest_progress_3241 == 255 or quest_progress_3242 == 1 or quest_progress_3242 == 2 or quest_progress_3242 == 255 then
		
		set_quest_status( 3242, 1, 1 )
		
		warp( 28518 + math.random(0,10) , 20059 + math.random(0,10), gv("layer") )
		cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
			
	end
end

-- <(version:7.2)>가이아 대륙으로<3지역>
function quest_prop_warp_espoir3_0()
    
	warp( 139023 + math.random(0,10) , 141656+ math.random(0,10), gv("layer") )
	cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
	
end

-- <(version:7.2)>3지역으로<4지역>
function quest_prop_warp_espoir4_3()
    
	warp( 27910 + math.random(0,10) , 24903+ math.random(0,10), gv("layer") )
	cprint( "@91000799" ) -- <(version:7.2)><#6DD66D>아공간이 진동합니다.
	
end

-- 퀘스트 연결
------------------------------------------------------------------------------------------
function quest_start_3202()
	local count = find_item( 1000200 )
	
	if count == 0 then
		insert_item( 1000200, 1 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	end
end

function quest_end_3202()

	local count = find_item( 1000200 )
	
	if count == 1 then
		delete_item( get_item_handle( 1000200 ), count )
		cprint( "@91000777" ) -- <(version:7.2)><#6DD66D>아이템을 건데 주었습니다.
	end
		
end
------------------------------------------------------------------------------------------
function quest_start_3205()

	local count = find_item( 1000201 )
	
	if count == 0 then
		insert_item( 1000201, 1 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	end
	
	warp( 131981 + math.random(0,10) , 139986+ math.random(0,10), gv("layer") )
	cprint( "@91000941" ) -- <(version:7.2)><#6DD66D>강력한 마력으로 공간이 이동 됩니다.
	
	set_event_area_enter_count( 80866, get_event_area_enter_count( 80866 ) - 1 )
	set_event_area_enter_count( 80867, get_event_area_enter_count( 80867 ) - 1 )
	set_event_area_enter_count( 80868, get_event_area_enter_count( 80868 ) - 1 )
	set_event_area_enter_count( 80869, get_event_area_enter_count( 80869 ) - 1 )
	set_event_area_enter_count( 80870, get_event_area_enter_count( 80870 ) - 1 )
	
end

------------------------------------------------------------------------------------------
function quest_start_3206()

	warp( 108925 + math.random(0,10) , 73107+ math.random(0,10), gv("layer") )
	cprint( "@91000941" ) -- <(version:7.2)><#6DD66D>강력한 마력으로 공간이 이동 됩니다.
	
	set_event_area_enter_count( 60406, get_event_area_enter_count( 60406 ) - 1 )
	set_event_area_enter_count( 60407, get_event_area_enter_count( 60407 ) - 1 )
	set_event_area_enter_count( 60408, get_event_area_enter_count( 60408 ) - 1 )
	set_event_area_enter_count( 60409, get_event_area_enter_count( 60409 ) - 1 )
	set_event_area_enter_count( 60410, get_event_area_enter_count( 60410 ) - 1 )

end
------------------------------------------------------------------------------------------
function quest_start_3209()

	warp( 139032 + math.random(0,10) , 141101+ math.random(0,10), gv("layer") )
	cprint( "@91000941" ) -- <(version:7.2)><#6DD66D>강력한 마력으로 공간이 이동 됩니다.

end

function quest_end_3209()

	force_start_quest(3210, 91000821)
	
end

------------------------------------------------------------------------------------------
function quest_end_3210()

	force_start_quest(3211, 91000823)

end

function quest_end_3211()

	force_start_quest(3212, 91000825)
	
end
------------------------------------------------------------------------------------------
function quest_end_3212()

	force_start_quest(3213, 91000827)
	--cprint( "@91000065" ) -- <ep7>수상한 단서품을 획득하였습니다.
	
end
------------------------------------------------------------------------------------------
function quest_start_3214()

	warp( 21537 + math.random(0,20) , 27364+ math.random(0,20), gv("layer") )
	cprint( "@91000941" ) -- <(version:7.2)><#6DD66D>강력한 마력으로 공간이 이동 됩니다.
	
end
------------------------------------------------------------------------------------------
function quest_end_3215()

	force_start_quest(3216, 91000833)
	
end
------------------------------------------------------------------------------------------
function quest_end_3216()

	force_start_quest(3217, 91000835)
	
end
------------------------------------------------------------------------------------------
function quest_start_3217()

	set_event_area_enter_count( 10101, get_event_area_enter_count( 10101 ) - 1 )
	
end

function quest_end_3217()

	force_start_quest(3218, 91000837)
	
end

------------------------------------------------------------------------------------------
function quest_start_3218()

	-- 프랍 작동 카운트
	set_flag( "demongirl", math.random( 1, 13 ) )
	
end

function quest_end_3218()

	force_start_quest(3219, 91000839)
	
end

------------------------------------------------------------------------------------------
-- 폐기물 분쇄기
function quest_start_3219()

	warp( 19507 + math.random(0,20) , 29867+ math.random(0,20), gv("layer") )
	cprint( "@91000941" ) -- <(version:7.2)><#6DD66D>강력한 마력으로 공간이 이동 됩니다.
	
	add_state( 6012, 10, 1000) -- 석화( 10레벨 , 30초)
	cprint( "@91000798" ) -- <(version:7.2)><#6DD66D>석화에 걸렸습니다.

end

function quest_end_3219()

	force_start_quest(3220, 91000841)
	cprint( "@91000793" ) -- <(version:7.2)><#6DD66D>날 소환해줘요..
	
end

------------------------------------------------------------------------------------------
function quest_start_3220()

	local count = find_item( 1000203 )
	
	if count == 0 then
		insert_item( 1000203, 1 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	end
	
end

function quest_end_3220()

	force_start_quest(3221, 91000843)
	
end

------------------------------------------------------------------------------------------
function quest_end_3221()

	force_start_quest(3222, 91000845)
	
end
------------------------------------------------------------------------------------------
function quest_end_3222()

	force_start_quest(3223, 91000847)
	
end
------------------------------------------------------------------------------------------
function quest_end_3223()

	force_start_quest(3224, 91000849)
	
end
------------------------------------------------------------------------------------------
function quest_end_3224()

	force_start_quest(3225, 91000851)
	
	-- 셍체기억 조각 회수
	local count = find_item( 1000204 )
	
	if count == 1 then
		delete_item( get_item_handle( 1000204 ), count )
		cprint( "@91000777" ) -- <(version:7.2)><#6DD66D>아이템을 건데 주었습니다.
	end	
end
------------------------------------------------------------------------------------------
function quest_start_3225()

	set_event_area_enter_count( 10121, get_event_area_enter_count( 10121 ) - 1 )
	
end

function quest_end_3225()

	force_start_quest(3226, 91000853)
	
end

------------------------------------------------------------------------------------------
function quest_end_3226()

	force_start_quest(3227, 91000855)
	
end

------------------------------------------------------------------------------------------
function quest_start_3227()

	set_event_area_enter_count( 10143, get_event_area_enter_count( 10143 ) - 1 )
	set_event_area_enter_count( 10144, get_event_area_enter_count( 10144 ) - 1 )
	set_event_area_enter_count( 10145, get_event_area_enter_count( 10145 ) - 1 )

end

function quest_end_3227()

	force_start_quest(3228, 91000857)
	
end

------------------------------------------------------------------------------------------
function quest_start_3228()

	set_event_area_enter_count( 10143, get_event_area_enter_count( 10143 ) - 1 )
	set_event_area_enter_count( 10144, get_event_area_enter_count( 10144 ) - 1 )
	set_event_area_enter_count( 10145, get_event_area_enter_count( 10145 ) - 1 )
	
	add_field_prop( 30085, 480, 27700  + math.random(0, 20), 27357 + math.random(0, 20) , gv('layer'), 500 )
	
end

function quest_end_3228()

	force_start_quest(3229, 91000859)
	
end

------------------------------------------------------------------------------------------
function quest_start_3229()

	set_event_area_enter_count( 10143, get_event_area_enter_count( 10143 ) - 1 )
	set_event_area_enter_count( 10144, get_event_area_enter_count( 10144 ) - 1 )
	set_event_area_enter_count( 10145, get_event_area_enter_count( 10145 ) - 1 )
	
	add_field_prop( 30086, 480, 27616  + math.random(0, 20), 27864 + math.random(0, 20) , gv('layer'), 500 )
	
end

function quest_end_3229()

	force_start_quest(3230, 91000861)
	
end

------------------------------------------------------------------------------------------
function quest_start_3230()

	set_event_area_enter_count( 10143, get_event_area_enter_count( 10143 ) - 1 )
	set_event_area_enter_count( 10144, get_event_area_enter_count( 10144 ) - 1 )
	set_event_area_enter_count( 10145, get_event_area_enter_count( 10145 ) - 1 )
	
	add_field_prop( 30087, 480, 27910  + math.random(0, 20), 28118 + math.random(0, 20) , gv('layer'), 500 )
	
end

function quest_end_3230()

	force_start_quest(3231, 91000863)
	
end
------------------------------------------------------------------------------------------
function quest_start_3231()

	set_event_area_enter_count( 10143, get_event_area_enter_count( 10143 ) - 1 )
	set_event_area_enter_count( 10144, get_event_area_enter_count( 10144 ) - 1 )
	set_event_area_enter_count( 10145, get_event_area_enter_count( 10145 ) - 1 )
	
	add_field_prop( 30088, 480, 28164  + math.random(0, 20), 27947 + math.random(0, 20) , gv('layer'), 500 )
	
end

function quest_end_3231()

	force_start_quest(3232, 91000865)
	
end

------------------------------------------------------------------------------------------
function quest_start_3232()
	
	add_field_prop( 30089, 480, 28165  + math.random(0, 20), 27443 + math.random(0, 20) , gv('layer'), 500 )

end

function quest_end_3232()

	force_start_quest(3233, 91000867)
	
end

------------------------------------------------------------------------------------------
function quest_start_3233()

	set_event_area_enter_count( 10142, get_event_area_enter_count( 10142 ) - 1 )

end

function quest_end_3233()

	force_start_quest(3234, 91000869)
	
end

------------------------------------------------------------------------------------------

function quest_end_3234()

	force_start_quest(3235, 91000871)
	
end
------------------------------------------------------------------------------------------

function quest_end_3235()

	force_start_quest(3236, 91000873)
	
end

------------------------------------------------------------------------------------------
function quest_start_3236()

	set_event_area_enter_count( 10131, get_event_area_enter_count( 10131 ) - 1 )
	
end

function quest_end_3236()

	force_start_quest(3237, 91000875)
	
end

------------------------------------------------------------------------------------------
function quest_end_3237()

	force_start_quest(3238, 91000877)
	
end

------------------------------------------------------------------------------------------
function quest_start_3238()

	set_event_area_enter_count( 10134, get_event_area_enter_count( 10134 ) - 1 )
	
end

function quest_end_3238()

	force_start_quest(3239, 91000879)
	
end

------------------------------------------------------------------------------------------
function quest_end_3239()

	force_start_quest(3240, 91000881)
	
end

------------------------------------------------------------------------------------------
function quest_end_3240()

	force_start_quest(3242, 91000883)
	
end

------------------------------------------------------------------------------------------
-- 사탄 소녀의 반대 워프 게이트에서 퀘스트 할당
function quest_start_3241()

	set_event_area_enter_count( 10137, get_event_area_enter_count( 10137 ) - 1 )
	set_event_area_enter_count( 10138, get_event_area_enter_count( 10138 ) - 1 )
	
end

------------------------------------------------------------------------------------------
function quest_end_3242()

	force_start_quest(3243, 91000887)
	
	-- 고대 크리스탈 회수
	local count = find_item( 1000203 )
	
	if count == 1 then
		delete_item( get_item_handle( 1000203 ), count )
		cprint( "@91000777" ) -- <(version:7.2)><#6DD66D>아이템을 건데 주었습니다.
	end
	
end
------------------------------------------------------------------------------------------
function quest_end_3243()

	force_start_quest(3244, 91000889)
	
end
------------------------------------------------------------------------------------------
function quest_end_3244()

	force_start_quest(3245, 91000891)
	
end
------------------------------------------------------------------------------------------
function quest_end_3245()

	force_start_quest(3246, 91000893)
	
end
------------------------------------------------------------------------------------------
function quest_start_3246()

	warp( 138678 + math.random(0,20) , 75277+ math.random(0,20), gv("layer") )
	cprint( "@91000941" ) -- <(version:7.2)><#6DD66D>강력한 마력으로 공간이 이동 됩니다.
	
end

function quest_end_3246()

	force_start_quest(3247, 91000895)
	
end

------------------------------------------------------------------------------------------
function quest_end_3247()
	
	-- 고대 크리스탈 회수
	local count = find_item( 1000201 )
	
	if count == 1 then
		delete_item( get_item_handle( 1000201 ), count )
		cprint( "@91000777" ) -- <(version:7.2)><#6DD66D>아이템을 건데 주었습니다.
	end
	
end
------------------------------------------------------------------------------------------
function quest_end_3248()

	del_flag( "count" )
	del_flag( "sx" )
	del_flag( "sy" )
	del_flag( "ex" )
	del_flag( "ey" )
		
end

------------------------------------------------------------------------------------------
-----------------------------------	 집착 퍼즐판	  ------------------------------------
------------------------------------------------------------------------------------------

function quest_start_3210()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	local sx = 1
	local sy = 1
	local ex = math.random( 1, 5 )
	local ey = math.random( 1, 13 )
	
	
	set_flag( "count", 1 )
	set_flag( "sx", sx )
	set_flag( "sy", sy )
	set_flag( "ex", ex )
	set_flag( "ey", ey )
	
	
	cprint( sconv( "@91000930", "#@month@#", sx, "#@date@#", sy ) )
	cprint( sconv( "@91000931", "#@month@#", ex, "#@date@#", ey ) )
		
	warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
	
end


function quest_puzzle_clear( x, y )
	
	local sx = x
	local sy = y
	local ex = math.random( 1, 5 )
	local ey = math.random( 1, 13 )
	
	local previous = get_flag( "ex" ) * 100 + get_flag( "ey" )
	local current = ex * 100 + ey
	
	while current == previous or current == 307 or current == 407 do
		ex = math.random( 1, 5 )
		ey = math.random( 1, 13 )
		current = ex * 100 + ey
	end
	
	if get_flag( "count" ) > 5 then

		del_flag( "sx" )
		del_flag( "sy" )
		del_flag( "ex" )
		del_flag( "ey" )
		del_flag( "count" )
		
						
		--set_quest_status( QuestCode, StatusIndex, NewStatus )
		set_quest_status( 3210, 1, 1 )
		
		warp( 139033 + math.random(0,10) , 141690+ math.random(0,10), gv("layer") )
		
		cprint( "@91000937" ) -- <(version:7.2)>결계가 풀렸습니다. 미지의 기운이 느껴 집니다.
		
	else	

		local sing = math.random( 1, 4 )
		
		set_flag( "sx", x )
		set_flag( "sy", y )
		set_flag( "ex", ex )
		set_flag( "ey", ey )
		set_flag( "count", get_flag("count") + 1 )
		
					
		cprint( "@91000936" ) -- <(version:7.2)>결계의 힘이 약해집니다. 
		cprint( "@91000938" ) -- <(version:7.2)><#6DD66D>우우우웅~ 성공한 듯 크리스탈의 떨림이 더욱 심해졌다.
		
		if sing == 1 then
			cprint( sconv( "@91000930", "#@month@#", sx, "#@date@#", sy ) )
			cprint( sconv( "@91000932", "#@month@#", ex, "#@date@#", ey ) )
		elseif sing == 2 then
			cprint( sconv( "@91000930", "#@month@#", sx, "#@date@#", sy ) )
			cprint( sconv( "@91000933", "#@month@#", ex, "#@date@#", ey ) )
		elseif sing == 3 then
			cprint( sconv( "@91000930", "#@month@#", sx, "#@date@#", sy ) )
			cprint( sconv( "@91000934", "#@month@#", ex, "#@date@#", ey ) )
		elseif sing == 4 then
			cprint( sconv( "@91000930", "#@month@#", sx, "#@date@#", sy ) )
			cprint( sconv( "@91000935", "#@month@#", ex, "#@date@#", ey ) )
		end
		
		
		if get_flag("count") == 2 then
			add_field_prop( 30064, 180, 139731, 141520 ) --프랍 명: 빛과 기억의 무지개
			add_field_prop( 30064, 0.5, get_value( "x" ), get_value( "y" ) ) -- 현재 위치
		elseif get_flag("count") == 3 then
			add_field_prop( 30064, 180, 138336, 141520 ) --프랍 명: 빛과 기억의 무지개
			add_field_prop( 30064, 0.5, get_value( "x" ), get_value( "y" ) ) -- 현재 위치
		elseif get_flag("count") == 4 then
			add_field_prop( 30064, 180, 138336, 142069 ) --프랍 명: 빛과 기억의 무지개
			add_field_prop( 30064, 0.5, get_value( "x" ), get_value( "y" ) ) -- 현재 위치
		elseif get_flag("count") == 5 then
			add_field_prop( 30064, 180, 139731, 142069 ) --프랍 명: 빛과 기억의 무지개
			add_field_prop( 30064, 0.5, get_value( "x" ), get_value( "y" ) ) -- 현재 위치
		end
		
		
	end
	
end 

------------------------------------------------------------------------------------------
-- 1열
------------------------------------------------------------------------------------------

function mainquest_part2_region_templer1_1()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 1
	
		
	if quest_progress3210 == 1 then
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
				
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return 
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
						
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end

	end
end

function mainquest_part2_region_templer1_2()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 2
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return 
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end

	end
end

function mainquest_part2_region_templer1_3()


	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 3
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end

	end
end

function mainquest_part2_region_templer1_4()


	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 4
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end

	end
end

function mainquest_part2_region_templer1_5()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 5
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end

	end
end

function mainquest_part2_region_templer1_6()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 6
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end

	end
end

function mainquest_part2_region_templer1_7()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 7
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end

	end
end

function mainquest_part2_region_templer1_8()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 8
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer1_9()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 9
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer1_10()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 10
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer1_11()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 11
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer1_12()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 12
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
end

function mainquest_part2_region_templer1_13()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 1
	local y = 13
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end


------------------------------------------------------------------------------------------
-- 2열
------------------------------------------------------------------------------------------

function mainquest_part2_region_templer2_1()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 1
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_2()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 2
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_3()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 3
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer2_4()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 4
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer2_5()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 5
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_6()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 6
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_7()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 7
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_8()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 8
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_9()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 9
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_10()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 10
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_11()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 11
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_12()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 12
	
if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer2_13()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 2
	local y = 13
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end


------------------------------------------------------------------------------------------
-- 3열
------------------------------------------------------------------------------------------

function mainquest_part2_region_templer3_1()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 1
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer3_2()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 2
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_3()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 3
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_4()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 4
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_5()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 5
	
if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_6()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 6
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_7()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 7
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_8()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 8
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_9()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 9
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_10()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 10
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_11()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 11
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_12()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 12
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer3_13()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 3
	local y = 13
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end


------------------------------------------------------------------------------------------
-- 4열
------------------------------------------------------------------------------------------

function mainquest_part2_region_templer4_1()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 1
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_2()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 2
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_3()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 3
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_4()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 4
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_5()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 5
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_6()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 6
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_7()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 7
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end

function mainquest_part2_region_templer4_8()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 8
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_9()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 9
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_10()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 10
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
end

function mainquest_part2_region_templer4_11()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 11
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_12()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 12
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer4_13()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 4
	local y = 13
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end


------------------------------------------------------------------------------------------
-- 5열
------------------------------------------------------------------------------------------

function mainquest_part2_region_templer5_1()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 1
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_2()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 2
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_3()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 3
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_4()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 4
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_5()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 5
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_6()

	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 6
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_7()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 7
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_8()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 8
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_9()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 9
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_10()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 10
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
		
end

function mainquest_part2_region_templer5_11()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 11
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
			
end

function mainquest_part2_region_templer5_12()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 12
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end
	
end


function mainquest_part2_region_templer5_13()
	
	local quest_progress3210 = get_quest_progress(3210) -- [집착]결계해지 #1
	
	local x = 5
	local y = 13
	
	if quest_progress3210 == 1 then
		
		cprint( sconv( "@91000939", "#@month@#", x, "#@date@#", y ) )
		-- 지역 표시
		
		if get_flag( "ex" ) == x and get_flag( "ey" ) ==y then
			quest_puzzle_clear( x, y )
			return
		end
		
		if get_flag( "ey" ) ~=y and get_flag( "sx" ) ~=x then
			set_flag( "ex", math.random( 1, 5  ) )
			set_flag( "ey", math.random( 1, 13 ) )
			
			
			local sing = math.random( 1, 4 )
		
				cprint( "@91000940" ) -- <(version:7.2)><#ff8200>강력한 결계의 힘에 밀려납니다.
		
			if sing == 1 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000932", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 2 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000933", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 3 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000934", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
			elseif sing == 4 then
				cprint( sconv( "@91000930", "#@month@#", get_flag( "sx" ), "#@date@#", get_flag( "sy" ) ) )
				cprint( sconv( "@91000935", "#@month@#", get_flag( "ex" ), "#@date@#", get_flag( "ey" ) ) )
				
			end warp( 139872 + math.random(0,10) , 141691+ math.random(0,10), gv("layer") )
		end
	end

end


------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
-------------------------------	    마스터 클래스 던전    --------------------------------
------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------


-- 프랍    -------------------------------------------------------------------------------
------------------------------------------------------------------------------------------

function quest_prop_warp_master1_1()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3273 = get_quest_progress(3273) -- <(version:7.3)>[마스터]낮익은 풍경
	
	if quest_progress3273 == 255 then
	
		warp( 20195  + math.random(0,10) , 40283 + math.random(0,10), gv("layer") )
	
	else
	
		cprint( "@91002288" ) -- <#6DD66D>퀘스트를 완료 하셔야 다음 지역으로 이동이 가능합니다.
	
	end
	
end

function quest_prop_warp_master1_2()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3281 = get_quest_progress(3281) -- <(version:7.3)>[마스터]전사의 존
	
	if quest_progress3281 == 255 then
	
		warp( 22838  + math.random(0,10) , 40290 + math.random(0,10), gv("layer") )
	
	else
	
		cprint( "@91002288" ) -- <#6DD66D>퀘스트를 완료 하셔야 다음 지역으로 이동이 가능합니다.
	
	end
		
end

function quest_prop_warp_master1_3()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3282 = get_quest_progress(3282) -- <(version:7.3)>[마스터]법사의 존
	
	if quest_progress3282 == 255 then
	
		warp( 25523  + math.random(0,10) , 40286 + math.random(0,10), gv("layer") )
	
	else
	
		cprint( "@91002288" ) -- <#6DD66D>퀘스트를 완료 하셔야 다음 지역으로 이동이 가능합니다.
	
	end
			
end

function quest_prop_warp_master1_4()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3283 = get_quest_progress(3283) -- <(version:7.3)>[마스터]헌터의 존
	
	if quest_progress3283 == 255 then
	
		warp( 28088  + math.random(0,10) , 40289 + math.random(0,10), gv("layer") )
	
	else
	
		cprint( "@91002288" ) -- <#6DD66D>퀘스트를 완료 하셔야 다음 지역으로 이동이 가능합니다.
	
	end
		
end

function quest_prop_warp_master1_5()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3284 = get_quest_progress(3284) -- <(version:7.3)>[마스터]소환사의 존
	
	if quest_progress3284 == 255 then
	
		warp( 30654  + math.random(0,10) , 39428 + math.random(0,10), gv("layer") )
	
	else
	
		cprint( "@91002288" ) -- <#6DD66D>퀘스트를 완료 하셔야 다음 지역으로 이동이 가능합니다.
	
	end
	
	
	
end

------------------------------------------------------------------------------------------

function get_master_quest_number(pquest)

	local count = table.getn( pquest )
	local zonenumber = math.random( 1, count )
	for	 i = 1, count do

		local quest = pquest[zonenumber]
		if	get_quest_progress(quest) ~= 255 then
			return zonenumber
		end
		zonenumber = 	zonenumber   + 1
		if zonenumber > count	then
			zonenumber = 1
		end
	end

	return 0
end

-- 전사의 존
function quest_prop_warp_master2_1()
	
	local pquest = {3291,3292,3293,3297,3298,3299 }
	local zonenumber = get_master_quest_number( pquest )
	if zonenumber == 0 then
		cprint( "@91002380" )
		return
	end
	
	if zonenumber == 1 then
		warp( 17668  + math.random(0,10) , 47134 + math.random(0,10), gv("layer") )
	elseif zonenumber == 2 then
		warp( 20144  + math.random(0,10) , 45677 + math.random(0,10), gv("layer") )
	elseif zonenumber == 3 then
		warp( 23173  + math.random(0,10) , 46453 + math.random(0,10), gv("layer") )
	elseif zonenumber == 4 then
		warp( 17282  + math.random(0,10) , 43927 + math.random(0,10), gv("layer") )
	elseif zonenumber == 5 then
		warp( 20455  + math.random(0,10) , 43097 + math.random(0,10), gv("layer") )
	elseif zonenumber == 6 then
		warp( 23409  + math.random(0,10) , 43717 + math.random(0,10), gv("layer") )
	end
	
end
--전사의 존
function quest_prop_warp_master2_2()
	
	
	--스턴 2초
	add_state(6006, 5, 300)
	
	--중독 2초
	add_state(6003, 5, 500)
	
	cprint( "@91002289" ) -- <(version:7.3)><#6DD66D>워프게이트가 파손 되었습니다.
	
end
-- 법사의 존
function quest_prop_warp_master2_3()
	
	local pquest = {3294,3295,3296, 3300, 3301, 3302, 3319 }
	local zonenumber = get_master_quest_number( pquest )
	if zonenumber == 0 then
		cprint( "@91002381" )
		return
	end
	
	if zonenumber == 1 then
		warp( 25696  + math.random(0,10) , 46006 + math.random(0,10), gv("layer") )
	elseif zonenumber == 2 or zonenumber == 7 then
		warp( 28660  + math.random(0,10) , 46474 + math.random(0,10), gv("layer") )
	elseif zonenumber == 3 then
		warp( 31352  + math.random(0,10) , 47076 + math.random(0,10), gv("layer") )
	elseif zonenumber == 4 then
		warp( 25929  + math.random(0,10) , 42771 + math.random(0,10), gv("layer") )
	elseif zonenumber == 5 then
		warp( 28175  + math.random(0,10) , 43291 + math.random(0,10), gv("layer") )
	elseif zonenumber == 6 then
		warp( 30646  + math.random(0,10) , 43372 + math.random(0,10), gv("layer") )
	end
	
end
-- 법사의 존
function quest_prop_warp_master2_4()
	
	--스턴 2초
	add_state(6006, 5, 300)
	
	--중독 2초
	add_state(6003, 5, 500)
	
	cprint( "@91002289" ) -- <(version:7.3)><#6DD66D>워프게이트가 파손 되었습니다.
	
end

-- 헌터의 존
function quest_prop_warp_master2_5()
	
	--스턴 2초
	add_state(6006, 5, 300)
	
	--중독 2초
	add_state(6003, 5, 500)
	
	cprint( "@91002289" ) -- <(version:7.3)><#6DD66D>워프게이트가 파손 되었습니다.
	
	
end
-- 헌터의 존
function quest_prop_warp_master2_6()
	
	local pquest = {3303, 3304, 3305, 3306, 3307, 3308, 3315, 3316, 3317,   3320}
	local zonenumber = get_master_quest_number( pquest )
	if zonenumber == 0 then
		cprint( "@91002382" )
		return
	end
	
	if zonenumber == 1 then
		warp( 17410   + math.random(0,10) , 37384 + math.random(0,10), gv("layer") )
	elseif zonenumber == 2 then
		warp( 19768   + math.random(0,10) , 35896 + math.random(0,10), gv("layer") )
	elseif zonenumber == 3 then
		warp( 20773   + math.random(0,10) , 36829 + math.random(0,10), gv("layer") )
	elseif zonenumber == 4 then
		warp( 22178   + math.random(0,10) , 35889 + math.random(0,10), gv("layer") )
	elseif zonenumber == 5 then
		warp( 23122   + math.random(0,10) , 36806 + math.random(0,10), gv("layer") )
	elseif zonenumber == 6 then
		warp( 24698   + math.random(0,10) , 36017 + math.random(0,10), gv("layer") )
	elseif zonenumber == 7 or zonenumber == 10 then
		warp( 17573   + math.random(0,10) , 34301 + math.random(0,10), gv("layer") )
	elseif zonenumber == 8 then
		warp( 19897   + math.random(0,10) , 33402 + math.random(0,10), gv("layer") )
	elseif zonenumber == 9 then
		warp( 23568   + math.random(0,10) , 34381 + math.random(0,10), gv("layer") )
	end
	
end

--소환사의 존
function quest_prop_warp_master2_7()
	
	--스턴 2초
	add_state(6006, 5, 300)
	
	--중독 2초
	add_state(6003, 5, 500)
	
	cprint( "@91002289" ) -- <(version:7.3)><#6DD66D>워프게이트가 파손 되었습니다.
	
end
--소환사의 존
function quest_prop_warp_master2_8()
	
	local pquest = {3309,3310,3311,3312,3313,3314,3318}

	local zonenumber = get_master_quest_number( pquest )
	if zonenumber == 0 then
		cprint( "@91002383" )
		return
	end
	
	if zonenumber == 1 then
		warp( 26381   + math.random(0,10) , 36587 + math.random(0,10), gv("layer") )
	elseif zonenumber == 2 then
		warp( 27851   + math.random(0,10) , 36134 + math.random(0,10), gv("layer") )
	elseif zonenumber == 3 then
		warp( 28799   + math.random(0,10) , 36538 + math.random(0,10), gv("layer") )
	elseif zonenumber == 4 then
		warp( 26400   + math.random(0,10) , 35081 + math.random(0,10), gv("layer") )
	elseif zonenumber == 5 then
		warp( 28798   + math.random(0,10) , 35078 + math.random(0,10), gv("layer") )
	elseif zonenumber == 6 then
		warp( 30765   + math.random(0,10) , 36025 + math.random(0,10), gv("layer") )
	elseif zonenumber == 7 then
		warp( 30992   + math.random(0,10) , 33489 + math.random(0,10), gv("layer") )
	end
	
end

------------------------------------------------------------------------------------------

function quest_prop_warp_master3_1()
	
	warp( 20569 + math.random(0,10) , 40643 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_2()
	
	warp( 20569 + math.random(0,10) , 40643 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_3()
	
	warp( 20569 + math.random(0,10) , 40643 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_4()
	
	warp( 23200 + math.random(0,10) , 40641 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_5()
	
	warp( 23200 + math.random(0,10) , 40641 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_6()
	
	warp( 23200 + math.random(0,10) , 40641 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_7()
	
	warp( 20569 + math.random(0,10) , 40643 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_8()
	
	warp( 20569 + math.random(0,10) , 40643 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_9()
	
	warp( 20569 + math.random(0,10) , 40643 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_10()
	
	warp( 23200 + math.random(0,10) , 40641 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_11()
	
	warp( 23200 + math.random(0,10) , 40641 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_12()
	
	warp( 23200 + math.random(0,10) , 40641 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_19()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_20()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_21()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_22()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_23()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_24()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_25()
	
	warp( 28447 + math.random(0,10) , 39989 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_26()
	
	warp( 28447 + math.random(0,10) , 39989 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_27()
	
	warp( 28447 + math.random(0,10) , 39989 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_28()
	
	warp( 28447 + math.random(0,10) , 39989 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_29()
	
	warp( 28447 + math.random(0,10) , 39989 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_30()
	
	warp( 28447 + math.random(0,10) , 39989 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_31()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_32()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_33()
	
	warp( 25883 + math.random(0,10) , 39981 + math.random(0,10), gv("layer") )
	
end

function quest_prop_warp_master3_34()
	
	warp( 28447 + math.random(0,10) , 39989 + math.random(0,10), gv("layer") )
	
end








-- 프랍_마력기둥 -------------------------------------------------------------------------
------------------------------------------------------------------------------------------

function quest_prop_masterlight_30131()		
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3295 = get_quest_progress(3295) -- <(version:7.3)>[마스터]시련의 존5
	local quest_progress3315 = get_quest_progress(3315) -- <(version:7.3)>[마스터]시련의 존31
	local quest_progress3319 = get_quest_progress(3319) -- <(version:7.3)>[마스터]시련의 존5'
	local quest_progress3320 = get_quest_progress(3320) -- <(version:7.3)>[마스터]시련의 존31'
	
	
	if quest_progress3295 == 1 then
	
		-- <(version:7.3)>[마스터]시련의 존5
		local master_light3295 = get_quest_status( 3295 , 1 ) + 1
		
		set_quest_status( 3295, 1, master_light3295 )
		
	elseif quest_progress3315 == 1 then
	
		-- <(version:7.3)>[마스터]시련의 존31
		local master_light3315 = get_quest_status( 3315 , 1 ) + 1
		
		set_quest_status( 3315, 1, master_light3315 )
	
	elseif quest_progress3319 == 1 then
	
		-- <(version:7.3)>[마스터]시련의 존5'
		local master_light3319 = get_quest_status( 3319 , 1 ) + 1
		
		set_quest_status( 3319, 1, master_light3319 )
	
	elseif quest_progress3320 == 1 then
	
		-- <(version:7.3)>[마스터]시련의 존31'
		local master_light3320 = get_quest_status( 3320 , 1 ) + 1
		
		set_quest_status( 3320, 1, master_light3320 )
		
	end
	
end

function quest_prop_masterlight_30132()

	-- <(version:7.3)>[마스터]시련의 존2
	local master_hollylight = get_quest_status( 3292 , 1 ) + 1
	
	set_quest_status( 3292, 1, master_hollylight )
	
	
	
	--진짜 프랍 ID
	local reallight = 30133
	local fakelight = 30138

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
	
	
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	
	if masterlight == 1 then
		add_field_prop( reallight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
	end
	
end

function quest_prop_masterlight_30133()

	-- <(version:7.3)>[마스터]시련의 존2
	local master_hollylight = get_quest_status( 3292 , 1 ) + 1
	
	set_quest_status( 3292, 1, master_hollylight )
	
	--진짜 프랍 ID
	local reallight = 30134
	local fakelight = 30139

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	
	if masterlight == 1 then
		add_field_prop( reallight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
	end
end

function quest_prop_masterlight_30134()

	-- <(version:7.3)>[마스터]시련의 존2
	local master_hollylight = get_quest_status( 3292 , 1 ) + 1
	
	set_quest_status( 3292, 1, master_hollylight )
	
	--진짜 프랍 ID
	local reallight = 30135
	local fakelight = 30140

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	
	if masterlight == 1 then
		add_field_prop( reallight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
	end
	
end

function quest_prop_masterlight_30135()

	-- <(version:7.3)>[마스터]시련의 존2
	local master_hollylight = get_quest_status( 3292 , 1 ) + 1
	
	set_quest_status( 3292, 1, master_hollylight )
	
	--진짜 프랍 ID
	local reallight = 30136
	local fakelight = 30141

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	
	if masterlight == 1 then
		add_field_prop( reallight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
	end
	
end

function quest_prop_masterlight_30136()
	
	-- <(version:7.3)>[마스터]시련의 존2
	local master_hollylight = get_quest_status( 3292 , 1 ) + 1
	
	set_quest_status( 3292, 1, master_hollylight )
	
end

function quest_prop_masterlight_30137()
	
	--스턴 2초
	add_state(6006, 5, 200)
			
end

function quest_prop_masterlight_30138()

	--스턴 2초
	add_state(6006, 5, 200)
	
end

function quest_prop_masterlight_30139()
	
	--스턴 2초
	add_state(6006, 5, 200)
	

end

function quest_prop_masterlight_30140()
	
	--스턴 2초
	add_state(6006, 5, 200)

end

function quest_prop_masterlight_30141()
	
	--스턴 2초
	add_state(6006, 5, 200)

end

function quest_prop_masterlight_30142()

	local celebration
	
	celebration = math.random(1,4)
	
	if celebration == 1 then
	
		--공격속도 증가 30초
		add_state(1011, 30, 3000)
		
	elseif  celebration == 2 then
	
		--시전속도 증가 15초
		add_state(1012, 30, 3000)
			
	elseif  celebration == 3 then
	
		--방어력 증가 15초
		add_state(1009, 30, 3000)
	
	elseif  celebration == 4 then
	
		--회피 증가 15초
		add_state(1015, 5, 3000)
	end
	
end



function quest_prop_masterlight_30143()


	local master_hollylight = get_quest_status( 3316 , 1 ) + 1
	
	set_quest_status( 3316, 1, master_hollylight )

	
	--진짜 프랍 ID
	local reallight = 30144
	local fakelight = 30150

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	if masterlight == 1 then
		add_field_prop( reallight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
end


function quest_prop_masterlight_30144()

	local master_hollylight = get_quest_status( 3316 , 1 ) + 1
	
	set_quest_status( 3316, 1, master_hollylight )

	
	--진짜 프랍 ID
	local reallight = 30145
	local fakelight = 30151

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	
	if masterlight == 1 then
		add_field_prop( reallight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
end

function quest_prop_masterlight_30145()

	local master_hollylight = get_quest_status( 3316 , 1 ) + 1
	
	set_quest_status( 3316, 1, master_hollylight )

	
	--진짜 프랍 ID
	local reallight = 30146
	local fakelight = 30152

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	if masterlight == 1 then
		add_field_prop( reallight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
end


function quest_prop_masterlight_30146()

	local master_hollylight = get_quest_status( 3316 , 1 ) + 1
	
	set_quest_status( 3316, 1, master_hollylight )

	
	--진짜 프랍 ID
	local reallight = 30147
	local fakelight = 30153

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	if masterlight == 1 then
		add_field_prop( reallight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
end


function quest_prop_masterlight_30147()

	local master_hollylight = get_quest_status( 3316 , 1 ) + 1
	
	set_quest_status( 3316, 1, master_hollylight )

	
	--진짜 프랍 ID
	local reallight = 30148
	local fakelight = 30154

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	
	if masterlight == 1 then
		add_field_prop( reallight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
end


function quest_prop_masterlight_30148()

	local master_hollylight = get_quest_status( 3316 , 1 ) + 1
	
	set_quest_status( 3316, 1, master_hollylight )
	
	--진짜 프랍 ID
	local reallight = 30147
	local fakelight = 30153

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
		
	-- 진짜 프랍
	masterlight = selected[ 1 ]
	
	if masterlight == 1 then
		add_field_prop( reallight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( reallight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( reallight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( reallight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( reallight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
	-- 가짜 프랍
	masterlight = selected[ 2 ]
	
	if masterlight == 1 then
		add_field_prop( fakelight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 2 then
		add_field_prop( fakelight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 3 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 4 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 5 then
		add_field_prop( fakelight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 6 then
		add_field_prop( fakelight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 7 then
		add_field_prop( fakelight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 8 then
		add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
	elseif masterlight == 9 then
		add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
	end
	
end


function quest_prop_masterlight_30149()

	--스턴 2초
	add_state(6006, 5, 200)
	
	--중독 2초
	add_state(6003, 5, 200)
			
end


function quest_prop_masterlight_30150()

	--스턴 2초
	add_state(6006, 5, 200)
	
	--중독 2초
	add_state(6003, 5, 200)
	
end


function quest_prop_masterlight_30151()
	
	--스턴 2초
	add_state(6006, 5, 200)
	
	--중독 5초
	add_state(6003, 5, 500)
	
	--규인화 리젠
	add_npc( gv('x') + math.random (50, 100), gv('y')+ math.random (50, 100), 140007,  1, 18000 )
	
end


function quest_prop_masterlight_30152()
	
	--스턴 2초
	add_state(6006, 5, 200)
	
	--중독 5초
	add_state(6003, 5, 500)
	
	--규인화 리젠
	add_npc( gv('x') + math.random (50, 100), gv('y')+ math.random (50, 100), 140007,  1, 18000 )
	
end


function quest_prop_masterlight_30153()

	--스턴 2초
	add_state(6006, 5, 200)
	
	--중독 10초
	add_state(6003, 8, 1000)
	
	--규인화 리젠
	add_npc( gv('x') + math.random (50, 100), gv('y')+ math.random (50, 100), 140007,  1, 18000 )
	
end


function quest_prop_masterlight_30154()
	
	--스턴 2초
	add_state(6006, 5, 200)
	
	--중독 10초
	add_state(6003, 10, 1000)
	
	--규인화 리젠
	add_npc( gv('x') + math.random (50, 100), gv('y')+ math.random (50, 100), 140007,  1, 18000 )
	
end


function quest_prop_masterlight_30156()
	
	local master_cube = get_quest_status( 3313 , 1 ) + 1
	
	set_quest_status( 3313, 1, master_cube )
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3313 = get_quest_progress(3313) -- <(version:7.3)>[마스터]시련의 존29
	
	
end



function quest_prop_master_cube_30157()
	
	local master_cube = get_quest_status( 3294 , 1 ) + 1
	
	set_quest_status( 3294, 1, master_cube )
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3294 = get_quest_progress(3294) -- <(version:7.3)>[마스터]시련의 존4
		
	
end



function quest_prop_master_rock_30158()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3294 = get_quest_progress(3294) -- <(version:7.3)>[마스터]시련의 존4
	local quest_progress3313 = get_quest_progress(3313) -- <(version:7.3)>[마스터]시련의 존4
	
	-- 퀘스트 상태 체크
	if   quest_progress3294 == 1 then
	
		--스턴 20초
		add_state(6006, 5, 2000)
		
	end
	
	if   quest_progress3313 == 1 then
	
		--스턴 20초
		add_state(6006, 5, 2000)
		
	end
	
end

-- 퀘스트 링크    ------------------------------------------------------------------------
function quest_end_3272()
	
	-- 마스터 고대 크리스탈 1000301
	local count = find_item( 1000301 )
	
	if count == 0 then
		insert_item( 1000301, 1 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	end
	
	warp( 17818, 40213, gv("layer") )

end
------------------------------------------------------------------------------------------
function quest_start_3281()
	
	--전사의 존 퀘스트 초기화
	reset_finished_quest( 3291 )
	reset_finished_quest( 3292 )
	reset_finished_quest( 3293 )
	reset_finished_quest( 3297 )
	reset_finished_quest( 3298 )
	reset_finished_quest( 3299 )
	
end
------------------------------------------------------------------------------------------
function quest_start_3282()

	--법사의 존 퀘스트 초기화
	reset_finished_quest( 3294 )
	reset_finished_quest( 3295 )
	reset_finished_quest( 3296 )
	reset_finished_quest( 3300 )
	reset_finished_quest( 3301 )
	reset_finished_quest( 3302 )
	
end
------------------------------------------------------------------------------------------
function quest_start_3283()

	--헌터의 존 퀘스트 초기화
	reset_finished_quest( 3303 )
	reset_finished_quest( 3304 )
	reset_finished_quest( 3305 )
	reset_finished_quest( 3306 )
	reset_finished_quest( 3307 )
	reset_finished_quest( 3308 )
	
	reset_finished_quest( 3315 )
	reset_finished_quest( 3316 )
	reset_finished_quest( 3317 )
	
end
------------------------------------------------------------------------------------------
function quest_start_3284()

	--소환사의 존 퀘스트 초기화
	reset_finished_quest( 3309 )
	reset_finished_quest( 3310 )
	reset_finished_quest( 3311 )
	reset_finished_quest( 3312 )
	reset_finished_quest( 3313 )
	reset_finished_quest( 3314 )
	
	reset_finished_quest( 3318 )
	
end
------------------------------------------------------------------------------------------
function quest_start_3291()
	
	-- 시련존 1
	set_event_area_enter_count( 10201, 0 )
	
	reset_finished_quest( 3291 )
	
	-- 마스터 스펠 브레이커 주문서 1000302
	local count = find_item( 1000302 )
	
	if count == 0 then
		insert_item( 1000302, 5 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	end
	
end

function quest_end_3291()

	
	local quest_target = get_quest_status( 3281, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3281, 1, quest_target  )

end
------------------------------------------------------------------------------------------
function quest_start_3292()

	-- 시련존 2
	set_event_area_enter_count( 10202, 0 )
	reset_finished_quest( 3292 )
	
end

function quest_end_3292()

	--set_flag( "master_spell", "master_spell + math.random(2, 3)" )
	
	local quest_target = get_quest_status( 3281, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3281, 1, quest_target  )

end

------------------------------------------------------------------------------------------
function quest_start_3293()

	-- 시련존 3
	set_event_area_enter_count( 10203, 0 )
	reset_finished_quest( 3293 )
	
end

function quest_end_3293()
	
	--set_flag( "master_spell", "master_spell + math.random(2, 3)" )
	
	local quest_target = get_quest_status( 3281, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3281, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3294()

	-- 시련존 4
	set_event_area_enter_count( 10204, 0 )
	reset_finished_quest( 3294 )
	
end

function quest_end_3294()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3282, 1 ) + math.random( 2, 3 )
	
	set_quest_status(3282, 1, quest_target  )
		
end

------------------------------------------------------------------------------------------
function quest_start_3295()
	
	-- 시련존 5
	set_event_area_enter_count( 10205, 0 )

	reset_finished_quest( 3295 )
	
end

function quest_end_3295()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3282, 1 ) + math.random( 1, 2 )
	
	set_quest_status(3282, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3296()

	-- 시련존 6
	set_event_area_enter_count( 10206, 0 )
	
	reset_finished_quest( 3296 )
	
	
	-- 마스터 스펠 브레이커 주문서 1000302
	local count = find_item( 1000302 )
	
	if count == 0 then
		insert_item( 1000302, 5 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	end
	
end

function quest_end_3296()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3282, 1 ) + math.random( 2, 3 )
	
	set_quest_status(3282, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3297()

	-- 시련존 7
	set_event_area_enter_count( 10207, 0 )
	
	reset_finished_quest( 3297 )
	
end

function quest_end_3297()

	local quest_target = get_quest_status( 3281, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3281, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3298()

	-- 시련존 8_1
	set_event_area_enter_count( 10208, 0 )
	-- 시련존 8_2
	set_event_area_enter_count( 10235, 0 )
	
	reset_finished_quest( 3298 )
	
end

function quest_end_3298()

	local quest_target = get_quest_status( 3281, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3281, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3299()

	-- 시련존 9
	set_event_area_enter_count( 10209, 0 )
	
	reset_finished_quest( 3299 )
	
end

function quest_end_3299()

	local quest_target = get_quest_status( 3281, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3281, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3300()

	-- 시련존 10
	set_event_area_enter_count( 10210, 0 )
	
	reset_finished_quest( 3300 )
	
end

function quest_end_3300()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3282, 1 ) + math.random( 2, 3 )
	
	set_quest_status(3282, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3301()

	-- 시련존 11
	set_event_area_enter_count( 10211, 0 )
	
	reset_finished_quest( 3301 )
	
end

function quest_end_3301()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3282, 1 ) + math.random( 2, 3 )
	
	set_quest_status(3282, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3302()

	-- 시련존 12
	set_event_area_enter_count( 10212, 0 )
	
	reset_finished_quest( 3302 )

	
	-- 마스터 석화 주문서 1000303
	local count = find_item( 1000303 )
	local counts = find_item( 1000304 )
	
	if count == 0 and counts then
		insert_item( 1000303, 10 )
		insert_item( 1000304, 3 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	else
		cprint( "@91002290" ) -- <(version:7.3)><#6DD66D>주문서가 남아 있습니다. 퀘스트를 재시작하여 주세요.
	end
	
end

function quest_end_3302()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3282, 1 ) + math.random( 2, 3 )
	
	set_quest_status(3282, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3303()

	-- 시련존 19
	set_event_area_enter_count( 10219, 0 )
	
	reset_finished_quest( 3303 )

	
end

function quest_end_3303()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3304()

	-- 시련존 20
	set_event_area_enter_count( 10220, 0 )
	
	reset_finished_quest( 3304 )

	
end

function quest_end_3304()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3305()

	-- 시련존 21
	set_event_area_enter_count( 10221, 0 )
	
	reset_finished_quest( 3305 )

	
end

function quest_end_3305()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3306()

	-- 시련존 22
	set_event_area_enter_count( 10222, 0 )
	
	reset_finished_quest( 3306 )

	
end

function quest_end_3306()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3307()

	-- 시련존 23
	set_event_area_enter_count( 10223, 0 )
	
	reset_finished_quest( 3307 )
	
	
	-- 마스터 석화 주문서 1000303
	local count = find_item( 1000303 )
	local counts = find_item( 1000304 )
	
	if count == 0 and counts then
		insert_item( 1000303, 7 )
		insert_item( 1000304, 1 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	else
		cprint( "@91002290" ) -- <(version:7.3)><#6DD66D>주문서가 남아 있습니다. 퀘스트를 재시작하여 주세요.
	end
	
end

function quest_end_3307()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3308()

	-- 시련존 24
	set_event_area_enter_count( 10224, 0 )
	
	reset_finished_quest( 3308 )
	
	-- 마스터 석화 주문서 1000303
	local count = find_item( 1000303 )
	local counts = find_item( 1000304 )
	
	if count == 0 and counts then
		insert_item( 1000303, 7 )
		insert_item( 1000304, 3 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	else
		cprint( "@91002290" ) -- <(version:7.3)><#6DD66D>주문서가 남아 있습니다. 퀘스트를 재시작하여 주세요.
	end
	
end

function quest_end_3308()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3309()

	-- 시련존 25
	set_event_area_enter_count( 10225, 0 )
	
	reset_finished_quest( 3309 )
	
	-- 마스터 스펠 브레이커 주문서 1000302
	local count = find_item( 1000302 )
	
	if count == 0 then
		insert_item( 1000302, 5 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	end
	
end

function quest_end_3309()
	
	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3284, 1 ) + math.random(4, 5)
	
	set_quest_status( 3284, 1, quest_target  )


end

------------------------------------------------------------------------------------------
function quest_start_3310()

	-- 시련존 26_1
	set_event_area_enter_count( 10226, 0 )
	-- 시련존 26_2
	set_event_area_enter_count( 10236, 0 )
	
	reset_finished_quest( 3310 )
	
end

function quest_end_3310()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3284, 1 ) + math.random(4, 5)
	
	set_quest_status( 3284, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3311()

	-- 시련존 27
	set_event_area_enter_count( 10227, 0 )
	
	reset_finished_quest( 3311 )
	
end

function quest_end_3311()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3284, 1 ) + math.random(4, 5)
	
	set_quest_status( 3284, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3312()

	-- 시련존 28
	set_event_area_enter_count( 10228, 0 )
	
	reset_finished_quest( 3312 )
	
end

function quest_end_3312()
	
	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3284, 1 ) + math.random(4, 5)
	
	set_quest_status( 3284, 1, quest_target  )

end

------------------------------------------------------------------------------------------
function quest_start_3313()

	-- 시련존 29
	set_event_area_enter_count( 10229, 0 )
	
	reset_finished_quest( 3313 )
	
end

function quest_end_3313()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3284, 1 ) + math.random(4, 5)
	
	set_quest_status( 3284, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3314()

	-- 시련존 30
	set_event_area_enter_count( 10230, 0 )
	
	reset_finished_quest( 3314 )
	
end

function quest_end_3314()
	
	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3284, 1 ) + math.random(4, 5)
	
	set_quest_status( 3284, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3315()

	-- 시련존 31
	set_event_area_enter_count( 10231, 0 )
	
	reset_finished_quest( 3315 )
	
end

function quest_end_3315()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 1, 2 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3316()

	-- 시련존 32
	set_event_area_enter_count( 10232, 0 )
	
	reset_finished_quest( 3316 )
	
end

function quest_end_3316()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3317()

	-- 시련존 33
	set_event_area_enter_count( 10233, 0 )
	
	reset_finished_quest( 3317 )
	
end

function quest_end_3317()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status(  3283, 1 ) + math.random( 2, 3 )
	
	set_quest_status( 3283, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3318()

	-- 시련존 34
	set_event_area_enter_count( 10234, 0 )
	
	reset_finished_quest( 3318 )
	
end

function quest_end_3318()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3284, 1 ) + math.random(3, 5)
	
	set_quest_status( 3284, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3319()

	-- 시련존 5_1
	set_event_area_enter_count( 10205, 0 )
	
	reset_finished_quest( 3319 )
	
end

function quest_end_3319()

	-- 퀘스트 '<(version:7.3)>[마스터]법사의 존' 목표 달성
	local quest_target = get_quest_status( 3282, 1 ) + math.random(1, 3)
	
	set_quest_status( 3282, 1, quest_target  )
	
end

------------------------------------------------------------------------------------------
function quest_start_3320()

	-- 시련존 31_1
	set_event_area_enter_count( 10231, 0 )
	
	reset_finished_quest( 3320 )
	
end

function quest_end_3320()

	-- 퀘스트 '<(version:7.3)>[마스터]헌터의 존' 목표 달성
	local quest_target = get_quest_status( 3283, 1 ) + math.random(1, 3)
	
	set_quest_status( 3283, 1, quest_target  )
	
end


------------------------------------------------------------------------------------------
function quest_start_3321()

	-- 마스터 존
	set_event_area_enter_count( 10245, 0 )
	
	reset_finished_quest( 3321 )
	
end

-- 지역진입 ------------------------------------------------------------------------------
------------------------------------------------------------------------------------------

function master_region_level_10201()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3291 = get_quest_progress(3291) -- <(version:7.3)>[마스터]시련의 존1
	
	local state_level = get_state_level( 4552 )
	
	cprint( "asd" )
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3291 == 1 then
	
		-- 아브흐바 소환
		add_npc( gv('x'), gv('y'), 154014,  1, 18000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		cprint( "asdasd" )
	end
	
end


function master_region_level_10202()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3292 = get_quest_progress(3292) -- <(version:7.3)>[마스터]시련의 존2
	
	--진짜 프랍 ID
	local reallight = 30132
	local fakelight = 30137

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 , 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
	
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3292 == 1 then
	
		
		-- 진짜 프랍
		masterlight = selected[ 1 ]
		
		if masterlight == 1 then
			add_field_prop( reallight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 2 then
			add_field_prop( reallight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 3 then
			add_field_prop( reallight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 4 then
			add_field_prop( reallight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 5 then
			add_field_prop( reallight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 6 then
			add_field_prop( reallight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 7 then
			add_field_prop( reallight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 8 then
			add_field_prop( reallight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 9 then
			add_field_prop( reallight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
		end
		
		-- 가짜 프랍
		masterlight = selected[ 2 ]
		
		if masterlight == 1 then
			add_field_prop( fakelight, 28, 20457 + math.random( 10, 100), 45732 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 2 then
			add_field_prop( fakelight, 28, 20205 + math.random( 10, 100), 45985 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 3 then
			add_field_prop( fakelight, 28, 20452 + math.random( 10, 100), 46244 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 4 then
			add_field_prop( fakelight, 28, 20709 + math.random( 10, 100), 45992 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 5 then
			add_field_prop( fakelight, 28, 20453 + math.random( 10, 100), 45991 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 6 then
			add_field_prop( fakelight, 28, 20239 + math.random( 10, 100), 46210 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 7 then
			add_field_prop( fakelight, 28, 20645 + math.random( 10, 100), 46164 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 8 then
			add_field_prop( fakelight, 28, 20651 + math.random( 10, 100), 45777 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 9 then
			add_field_prop( fakelight, 28, 20270 + math.random( 10, 100), 45798 + math.random( 10, 100), gv('layer') )
		end
	
	end
	
end

function master_region_level_10203()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3293 = get_quest_progress(3293) -- <(version:7.3)>[마스터]시련의 존3
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3293 == 1 then
			
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
		-- 진짜 몬스터 찾기 Lv 1, 마르두카류 리젠
		add_npc( gv('x'), gv('y'), 136006,  1, 21000 )
		add_npc( gv('x'), gv('y'), 136007,  math.random( 1, 3), 18000 )
		add_npc( gv('x'), gv('y'), 136009,  math.random( 1, 3), 18000 )
		add_npc( gv('x'), gv('y'), 136010,  math.random( 1, 3), 18000 )
		add_npc( gv('x'), gv('y'), 136011,  math.random( 1, 3), 18000 )
		
		--add_npc( gv('x'), gv('y'), 136006,  math.random( 1, 3), 18000 )
	
	end
	
end

function master_region_level_10204()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3294 = get_quest_progress(3294) -- <(version:7.3)>[마스터]시련의 존4
	
	local state_level = get_state_level( 4552 )
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3294 == 1 then
	
		-- 고대 암석 리젠	
		add_field_prop( 30158, 45, 25600, 46282, gv('layer'), - 5, 0, 0, 0, 0.5, 0.5, 0.5 )
		add_field_prop( 30158, 45, 25648, 46282, gv('layer'), - 5, 0, 0, 0, 0.5, 0.5, 0.5 )
		add_field_prop( 30158, 45, 25696, 46282, gv('layer'), - 5, 0, 0, 0, 0.5, 0.5, 0.5 )
		
		add_field_prop( 30158, 45, 25744, 46234, gv('layer'), - 5, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		add_field_prop( 30158, 45, 25744, 46282, gv('layer'), - 5, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		add_field_prop( 30158, 45, 25744, 46330, gv('layer'), - 5, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		
		add_field_prop( 30158, 45, 25696, 46186, gv('layer'), - 5, 0, 0, 0, 0.5, 0.5, 0.5 )
		add_field_prop( 30158, 45, 25744, 46186, gv('layer'), - 5, 0, 0, 0, 0.5, 0.5, 0.5 )
		add_field_prop( 30158, 45, 25792, 46186, gv('layer'), - 5, 0, 0, 0, 0.5, 0.5, 0.5 )
		
		add_field_prop( 30158, 45, 25648, 46138, gv('layer'), - 5, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		add_field_prop( 30158, 45, 25648, 46186, gv('layer'), - 5, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		add_field_prop( 30158, 45, 25648, 46234, gv('layer'), - 5, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		
		-- 고대 큐브 리젠
		add_field_prop( 30157, 45, 25608, 46129, gv('layer'), 10, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		add_field_prop( 30157, 45, 25794, 46141, gv('layer'), 10, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		add_field_prop( 30157, 45, 25611, 46334, gv('layer'), 10, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		add_field_prop( 30157, 45, 25787, 46331, gv('layer'), 10, 0, 0, 1.57, 0.5, 0.5, 0.5 )
		
	end
	
end

function master_region_level_10205()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3295 = get_quest_progress(3295) -- <(version:7.3)>[마스터]시련의 존5
	local quest_progress3319 = get_quest_progress(3319) -- <(version:7.3)>[마스터]시련의 존5'
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 then
	
	
		if quest_progress3295 == 1 then
		
			local masterlight
			local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }

			for i = 1, 3 do
				masterlight = math.random( 1 + i, 9 )
				local temp = selected[ i ]
				selected[ i ] = selected[ masterlight ]
				selected[ masterlight ] = temp
			end
			
			for i = 1, 3 do
				-- 진짜 프랍
				masterlight = selected[ i ]
				
				if masterlight == 1 then
					add_field_prop( 30131, 28, 28887 + math.random( 10, 100), 46627 + math.random( 10, 100), gv('layer') )
				elseif masterlight == 2 then
					add_field_prop( 30131, 28, 28796 + math.random( 10, 100), 46581 + math.random( 10, 100), gv('layer') )
				elseif masterlight == 3 then
					add_field_prop( 30131, 28, 28722 + math.random( 10, 100), 46632 + math.random( 10, 100), gv('layer') )
				elseif masterlight == 4 then
					add_field_prop( 30131, 28, 28739 + math.random( 10, 100), 46734 + math.random( 10, 100), gv('layer') )
				elseif masterlight == 5 then
					add_field_prop( 30131, 28, 28862 + math.random( 10, 100), 46748 + math.random( 10, 100), gv('layer') )
				elseif masterlight == 6 then
					add_field_prop( 30131, 28, 29006 + math.random( 10, 100), 46705 + math.random( 10, 100), gv('layer') )
				elseif masterlight == 7 then
					add_field_prop( 30131, 28, 28781 + math.random( 10, 100), 46854 + math.random( 10, 100), gv('layer') )
				elseif masterlight == 8 then
					add_field_prop( 30131, 28, 28610 + math.random( 10, 100), 46719 + math.random( 10, 100), gv('layer') )
				elseif masterlight == 9 then
					add_field_prop( 30131, 28, 28881 + math.random( 10, 100), 46499 + math.random( 10, 100), gv('layer') )
				end
			end
			
		end
		
		if quest_progress3319 == 1 then
		
			add_state(1013, 18, 800) -- 이동속도 증가 18Lv, 8초
			
				--if masterlight == 1 then
					add_field_prop( 30131, 35, 28887 + math.random( 10, 100), 46627 + math.random( 10, 100), gv('layer') )
				--elseif masterlight == 2 then
					add_field_prop( 30131, 35, 28796 + math.random( 10, 100), 46581 + math.random( 10, 100), gv('layer') )
				--elseif masterlight == 3 then
					add_field_prop( 30131, 35, 28722 + math.random( 10, 100), 46632 + math.random( 10, 100), gv('layer') )
				--elseif masterlight == 4 then
					add_field_prop( 30131, 35, 28739 + math.random( 10, 100), 46734 + math.random( 10, 100), gv('layer') )
				--elseif masterlight == 5 then
					add_field_prop( 30131, 35, 28862 + math.random( 10, 100), 46748 + math.random( 10, 100), gv('layer') )
				--elseif masterlight == 6 then
					add_field_prop( 30131, 35, 29006 + math.random( 10, 100), 46705 + math.random( 10, 100), gv('layer') )
				--elseif masterlight == 7 then
					add_field_prop( 30131, 35, 28781 + math.random( 10, 100), 46854 + math.random( 10, 100), gv('layer') )
				--elseif masterlight == 8 then
					add_field_prop( 30131, 35, 28610 + math.random( 10, 100), 46719 + math.random( 10, 100), gv('layer') )
				--elseif masterlight == 9 then
					add_field_prop( 30131, 35, 28881 + math.random( 10, 100), 46499 + math.random( 10, 100), gv('layer') )
				--end
				
		end
	
	end
	
end

function master_region_level_10206()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3296 = get_quest_progress(3296) -- <(version:7.3)>[마스터]시련의 존6
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3296 == 1 then
	
	
		-- 베트라얄 소환
		add_npc( gv('x'), gv('y'), 154015,  1, 18000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	end
	
end

function master_region_level_10207()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3297 = get_quest_progress(3297) -- <(version:7.3)>[마스터]시련의 존7
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3297 == 1 then
	
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
		-- 복사 몬스터 Lv 1, 마녀 리젠
		add_npc( gv('x'), gv('y'), 152004,  1, 36000 )
		
		-- 복사 몬스터 Lv 1, 광신도  리젠
		add_npc( gv('x'), gv('y'), 153009,  1, 6000 )
		-- 복사 몬스터 Lv 1, 광신도  리젠
		add_npc( gv('x'), gv('y'), 153010,  1, 6000 )
	
	
	end
	
end

-- 8지역
function master_region_level_10208()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3298 = get_quest_progress(3298) -- <(version:7.3)>[마스터]시련의 존8
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3298 == 1 then
		
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	
		-- 공격력 vs 방어력 Lv 3, 공격의 고대기사 리젠
		add_npc( gv('x'), gv('y'), 158024,  math.random( 3, 5), 20000 )
		-- 공격력 vs 방어력 Lv 3, 공격의 고대기사 리젠
		add_npc( gv('x'), gv('y'), 158024,  math.random( 1, 3), 20000 )
		
	end
	
end
-- 8_2지역
function master_region_level_10235()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3298 = get_quest_progress(3298) -- <(version:7.3)>[마스터]시련의 존8
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3298 == 1 then
	
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
		-- 공격력 vs 방어력 Lv 3, 방어의 고대기사 리젠
		add_npc( gv('x'), gv('y'), 158025,  math.random( 4, 6), 20000 )
		-- 공격력 vs 방어력 Lv 3, 방어의 고대기사 리젠
		add_npc( gv('x'), gv('y'), 158025,  math.random( 2, 4), 20000 )
		
	end
	
end


function master_region_level_10209()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3299 = get_quest_progress(3299) -- <(version:7.3)>[마스터]시련의 존9
	
	
	
end

function master_region_level_10210()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3300 = get_quest_progress(3300) -- <(version:7.3)>[마스터]시련의 존10
	
end

function master_region_level_10211()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3301 = get_quest_progress(3301) -- <(version:7.3)>[마스터]시련의 존11
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3301 == 1 then
	
	
		-- 절대 공격력 Lv 1, 바위의 정령  리젠
		add_npc( gv('x'), gv('y'), 154017,  1, 20000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	end
	
end

function master_region_level_10212()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3302 = get_quest_progress(3302) -- <(version:7.3)>[마스터]시련의 존12
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3302 == 1 then
	
		
		-- 절대 방어력 Lv 1, 좀비류  리젠
		add_npc( gv('x'), gv('y'), 131011,  math.random( 3, 4), 20000 )
		add_npc( gv('x'), gv('y'), 131012,  math.random( 2, 4), 20000 )
		add_npc( gv('x'), gv('y'), 131012,  math.random( 2, 4), 20000 )
		add_npc( gv('x'), gv('y'), 131014,  1, 20000 )
		
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	
	end
	
end

function master_region_level_10219()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3303 = get_quest_progress(3303) -- <(version:7.3)>[마스터]시련의 존19
	
end

function master_region_level_10220()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3304 = get_quest_progress(3304) -- <(version:7.3)>[마스터]시련의 존20
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3304 == 1 then
	
	
	-- 절대 공격력 Lv 2, 락 하우스 리젠
	add_npc( gv('x'), gv('y'), 163009,  1, 31000 )
	cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	
	end
	
end

function master_region_level_10221()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3305 = get_quest_progress(3305) -- <(version:7.3)>[마스터]시련의 존21
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3305 == 1 then
	
	
	-- 절대 공격력 Lv 3, 아이무스 리젠
	add_npc( gv('x'), gv('y'), 178011,  1, 43000 )
	cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	
	end
	
end

function master_region_level_10222()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3306 = get_quest_progress(3306) -- <(version:7.3)>[마스터]시련의 존22
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3306 == 1 then
	
		
		-- 복사 몬스터 Lv 2, 마녀 리젠
		add_npc( gv('x'), gv('y'), 162017,  1, 36000 )
		
		-- 복사 몬스터 Lv 2, 광신도  리젠
		add_npc( gv('x'), gv('y'), 163007,  1, 6000 )
		-- 복사 몬스터 Lv 2, 광신도 젠
		add_npc( gv('x'), gv('y'), 163008,  1, 6000 )
		
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	
	end
	
end

function master_region_level_10223()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3307 = get_quest_progress(3307) -- <(version:7.3)>[마스터]시련의 존23
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3307 == 1 then
	
	
		-- 절대 방어력 Lv 2, 문어류 리젠
		add_npc( gv('x'), gv('y'), 154018,  math.random( 3, 5), 20000 )
		add_npc( gv('x'), gv('y'), 154019,  math.random( 2, 5), 20000 )
		--add_npc( gv('x'), gv('y'), 154018,  math.random( 1, 4), 18000 )
		add_npc( gv('x'), gv('y'), 154020,  1, 20000 )
		
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	
	end
	
end

function master_region_level_10224()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3308 = get_quest_progress(3308) -- <(version:7.3)>[마스터]시련의 존24
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3308 == 1 then
	
		
		-- 절대 방어력 Lv 3, 라이디언류 리젠
		add_npc( gv('x'), gv('y'), 181002,  math.random( 3, 6), 20000 )
		add_npc( gv('x'), gv('y'), 181003,  math.random( 2, 4), 20000 )
		
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	end
end

function master_region_level_10225()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3309 = get_quest_progress(3309) -- <(version:7.3)>[마스터]시련의 존25
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3309 == 1 then
	
	
		-- 오시리스 소환
		add_npc( gv('x'), gv('y'), 154016,  1, 21000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	
	end
	
end

-- 26지역
function master_region_level_10226()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3310 = get_quest_progress(3310) -- <(version:7.3)>[마스터]시련의 존26
	
	local state_level = get_state_level( 4552 )
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3310 == 1 then
	
	
		-- 공격력 vs 방어력 Lv 1, 방어의 고대기사 리젠
		add_npc( gv('x'), gv('y'), 178010,  math.random( 4, 6), 31000 )
		-- 공격력 vs 방어력 Lv 1, 방어의 고대기사 리젠
		add_npc( gv('x'), gv('y'), 178010,  math.random( 2, 4), 31000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	end

	
end
-- 26_2지역
function master_region_level_10236()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3310 = get_quest_progress(3310) -- <(version:7.3)>[마스터]시련의 존26
	
	local state_level = get_state_level( 4552 )
	
		-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3310 == 1 then
	
	
		-- 공격력 vs 방어력 Lv 1, 공격의 고대기사 리젠
		add_npc( gv('x'), gv('y'), 178009,  math.random( 3, 5), 31000 )
		-- 공격력 vs 방어력 Lv 1, 공격의 고대기사 리젠
		add_npc( gv('x'), gv('y'), 178009,  math.random( 1, 3), 31000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	end
end


function master_region_level_10227()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3311 = get_quest_progress(3311) -- <(version:7.3)>[마스터]시련의 존27
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3311 == 1 then
	
	
		-- 복사 몬스터 Lv 3, 마녀 리젠
		add_npc( gv('x'), gv('y'), 172010,  1, 21000 )
		
		-- 복사 몬스터 Lv 3, 광신도  리젠
		add_npc( gv('x'), gv('y'), 173008,  1, 6000 )
		-- 복사 몬스터 Lv 3, 광신도  리젠
		add_npc( gv('x'), gv('y'), 173008,  1, 6000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	end
end

function master_region_level_10228()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3312 = get_quest_progress(3312) -- <(version:7.3)>[마스터]시련의 존28
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3312 == 1 then
		-- 진짜 몬스터 찾기 Lv 3, 마르두카류 리젠
		add_npc( gv('x'), gv('y'), 177019,  1, 21000 )
		add_npc( gv('x'), gv('y'), 177015,  math.random( 1, 3), 21000 )
		add_npc( gv('x'), gv('y'), 177017,  math.random( 1, 3), 21000 )
		add_npc( gv('x'), gv('y'), 177018,  math.random( 1, 3), 21000 )
		add_npc( gv('x'), gv('y'), 177008,  math.random( 1, 3), 21000 )
		add_npc( gv('x'), gv('y'), 177020,  math.random( 1, 2), 21000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	end
end

function master_region_level_10229()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3313 = get_quest_progress(3313) -- <(version:7.3)>[마스터]시련의 존29
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3313 == 1 then
	
		--빛의 기둥1
		add_field_prop( 30156, 100, 28826, 35248, gv('layer'), 0, 0, 0, 0, 1, 1, 1 )
		add_field_prop( 30156, 100, 29357, 34865, gv('layer'), 0, 0, 0, 0, 1, 1, 1 )


		-- 1구역(좌상)============================================================================================
		--왼쪽
		add_field_prop( 30158, 100, 29017, 35072, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29017, 35072, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29017, 35072, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )
		
		--왼쪽 아래쪽
		add_field_prop( 30158, 100, 29017, 34974, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29017, 34974, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29017, 34974, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )
		
		--아래쪽1
		add_field_prop( 30158, 100, 29036, 35020, gv('layer'),-23, 0, 0, 1.57, 0.5, 1, 1 )
		add_field_prop( 30158, 100, 29036, 35020, gv('layer'), 10, 0, 0, 1.57, 0.5, 1, 1 )

		--아래쪽2
		add_field_prop( 30158, 100, 29064, 35013, gv('layer'), -10, 0, 0, 1.57, 0.3, 0.5, 0.5 )
		add_field_prop( 30158, 100, 29064, 35013, gv('layer'), 6, 0, 0, 1.57, 0.3, 0.5, 0.5 )
		
		--아래쪽3
		add_field_prop( 30158, 100, 29110, 35013, gv('layer'),-28, 0, 0, 0, 0.8, 0.5, 1 )

		
		
		--위 왼쪽1
		add_field_prop( 30158, 100, 29017, 35145, gv('layer'), -23, 0, 0, 0, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29017, 35145, gv('layer'), 10, 0, 0, 0, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29017, 35145, gv('layer'), 43, 0, 0, 0, 1, 0.5, 1 )

		--위 오른쪽1
		add_field_prop( 30158, 100, 29115, 35145, gv('layer'), -23, 0, 0, 0, 1, 1.3, 1 )
		add_field_prop( 30158, 100, 29115, 35145, gv('layer'), 10, 0, 0, 0, 1, 1.3, 1 )
		add_field_prop( 30158, 100, 29115, 35145, gv('layer'), 43, 0, 0, 0, 1, 1.3, 1 )
		
		
		
		
		--위 왼쪽2
		add_field_prop( 30158, 100, 28958, 35130, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28958, 35130, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28958, 35130, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )

		--위 왼쪽3
		add_field_prop( 30158, 100, 28958, 35032, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28958, 35032, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28958, 35032, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )
		
		--위 왼쪽4
		add_field_prop( 30158, 100, 28900, 34994, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28900, 34994, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28900, 34994, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )


		--위 왼쪽5
		add_field_prop( 30158, 100, 28900, 35092, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28900, 35092, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28900, 35092, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )
		
		
		
		--위쪽1
		add_field_prop( 30158, 100, 28998, 35203, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28998, 35203, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28998, 35203, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )

		--위쪽2
		add_field_prop( 30158, 100, 29017, 35270, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29017, 35270, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29017, 35270, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )
		
		
		
		
		--기타
		add_field_prop( 30158, 100, 28758, 35160, gv('layer'), -23, 0, 0, 0, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 28758, 35160, gv('layer'), 10, 0, 0, 0, 1, 0.5, 1 )
				
		--============================================================================================
		--2구역(우상)
		--오른쪽
		add_field_prop( 30158, 100, 29116, 35072, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29116, 35072, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29116, 35072, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )
		
		--위 오른쪽 돌기무리
		add_field_prop( 30158, 100, 29310, 35299, gv('layer'), -23, 0, 0, 1.57, 0.8, 0.5, 1 )

		add_field_prop( 30158, 100, 29379, 35235, gv('layer'), -23, 0, 0, 0, 0.8, 0.5, 1 )

		add_field_prop( 30158, 100, 29349, 35180, gv('layer'), 53, 2.32, 0, 0, 1, 0.5, 1 )

		add_field_prop( 30158, 100, 29351, 35305, gv('layer'), 53, 2.32, 0, 2.32, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29361, 35305, gv('layer'), 53, 2.32, 0, 2.32, 1, 0.5, 1 )

		
		--============================================================================================
		--3구역(좌하)
		
		--아래 석상 오른쪽
		add_field_prop( 30158, 100, 29040, 34916, gv('layer'), -23, 0, 0, 0, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29040, 34916, gv('layer'), 10, 0, 0, 0, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29040, 34916, gv('layer'), 43, 0, 0, 0, 1, 0.5, 1 )
		
		--============================================================================================
		--4구역(우하)
		
		--아래 오른쪽1
		add_field_prop( 30158, 100, 29129, 34896, gv('layer'), -23, 0, 0, 1.57, 0.9, 0.5, 1 )
		add_field_prop( 30158, 100, 29129, 34896, gv('layer'), 10, 0, 0, 1.57, 0.9, 0.5, 1 )
		add_field_prop( 30158, 100, 29129, 34896, gv('layer'), 43, 0, 0, 1.57, 0.9, 0.5, 1 )
		
		add_field_prop( 30158, 100, 29229, 34855, gv('layer'), -23, 0, 0, 1.57, 0.6, 1, 1 )
		add_field_prop( 30158, 100, 29249, 34855, gv('layer'), 10, 0, 0, 0, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29259, 34855, gv('layer'), 43, 0, 0, 1.57, 0.6, 1, 1 )

		add_field_prop( 30158, 100, 29190, 34855, gv('layer'), -23, 0, 0, 1.57, 0.6, 1, 1 )
				
		
		
		--아래 오른쪽2
		add_field_prop( 30158, 100, 29276, 34920, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29276, 34920, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29276, 34920, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )

		--아래 오른쪽3
		add_field_prop( 30158, 100, 29276, 35018, gv('layer'), -23, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29276, 35018, gv('layer'), 10, 0, 0, 1.57, 1, 0.5, 1 )
		add_field_prop( 30158, 100, 29276, 35018, gv('layer'), 43, 0, 0, 1.57, 1, 0.5, 1 )
		
		--아래 오른쪽4
		add_field_prop( 30158, 100, 29276, 35095, gv('layer'), -23, 0, 0, 1.57, 0.6, 0.5, 1 )

		--아래 오른쪽 돌기1
		add_field_prop( 30158, 100, 29236, 34986, gv('layer'), -23, 0, 0, 0, 0.8, 0.5, 1 )
		add_field_prop( 30158, 100, 29249, 34986, gv('layer'), 10, 0, 0, 0, 0.4, 0.5, 1 )

		--아래 오른쪽 돌기2
		add_field_prop( 30158, 100, 29147, 35072, gv('layer'), -23, 0, 0, 0, 0.8, 0.5, 1 )
		add_field_prop( 30158, 100, 29130, 35072, gv('layer'), 10, 0, 0, 0, 0.4, 0.5, 1 )

		--아래 오른쪽 돌기3
		add_field_prop( 30158, 20, 29351, 35142, gv('layer'), -23, 0, 0, 0, 1, 0.5, 1 )
		add_field_prop( 30158, 20, 29351, 35142, gv('layer'), 23, 0, 0, 0, 0.5, 0.5, 0.5 )

		--아래 오른쪽 돌기4
		add_field_prop( 30158, 20, 29295, 35054, gv('layer'), -22, 0, 0, 0, 0.8, 0.5, 0.8 )
		add_field_prop( 30158, 20, 29295, 35054, gv('layer'), 10, 0, 0, 0, 0.4, 0.5, 0.6 )
		add_field_prop( 30158, 20, 29270, 35054, gv('layer'), 70, 1.57, 0, 0, 0.5, 0.5, 0.5 )


	end
	
end

function master_region_level_10230()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3314 = get_quest_progress(3314) -- <(version:7.3)>[마스터]시련의 존30
	
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3314 == 1 then
	
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.		
		
		-- 1지역
		-- 복사 몬스터 Lv 3, 마녀 리젠
		add_npc( 30694, 36330, 172010,  1, 21000 )
		
		-- 복사 몬스터 Lv 3, 광신도  리젠
		add_npc( 30684, 36330, 173008,  1, 6000 )
		-- 복사 몬스터 Lv 3, 광신도  리젠
		add_npc( 30704, 36330, 173008,  1, 6000 )
		
		-- 2지역
		-- 복사 몬스터 Lv 3, 마녀 리젠
		add_npc( 30831 , 36325, 172010,  1, 21000 )
		
		-- 복사 몬스터 Lv 3, 광신도  리젠
		add_npc( 30831 , 36325, 173008,  1, 6000 )
		-- 복사 몬스터 Lv 3, 광신도  리젠
		add_npc( 30831 , 36325, 173008,  1, 6000 )
		
		-- 마력 기둥
		add_field_prop( 30142, 360, 30694 + math.random( 10, 50), 36330 + math.random( 10, 50), gv('layer') )
		add_field_prop( 30142, 360, 30831 + math.random( 10, 50), 36325 + math.random( 10, 50), gv('layer') )
		add_field_prop( 30142, 360, 30823 + math.random( 10, 50), 36209 + math.random( 10, 50), gv('layer') )
		add_field_prop( 30142, 360, 30703 + math.random( 10, 50), 36222 + math.random( 10, 50), gv('layer') )

	end
	
end

function master_region_level_10231()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3315 = get_quest_progress(3315) -- <(version:7.3)>[마스터]시련의 존31
	local quest_progress3320 = get_quest_progress(3320) -- <(version:7.3)>[마스터]시련의 존31'
	
	local state_level = get_state_level( 4552 )
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 then
	
		if quest_progress3315 == 1 then
		
		add_field_prop( 30131, 45, 17579 + math.random( 10, 100), 34641 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17672 + math.random( 10, 100), 34564 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17621 + math.random( 10, 100), 34472 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17531 + math.random( 10, 100), 34473 + math.random( 10, 100), gv('layer') )
		
		add_field_prop( 30131, 45, 17485 + math.random( 10, 100), 34564 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17428 + math.random( 10, 100), 34343 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17572 + math.random( 10, 100), 34398 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17728 + math.random( 10, 100), 34338 + math.random( 10, 100), gv('layer') )
		
		add_field_prop( 30131, 45, 17764 + math.random( 10, 100), 34480 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17730 + math.random( 10, 100), 34686 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17457 + math.random( 10, 100), 34682 + math.random( 10, 100), gv('layer') )
		add_field_prop( 30131, 45, 17402 + math.random( 10, 100), 34476 + math.random( 10, 100), gv('layer') )
		
		end
		
		if quest_progress3320 == 1 then
		
		add_field_prop( 30131, 90, 17579 + math.random( 5, 50), 34641 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17672 + math.random( 5, 50), 34564 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17621 + math.random( 5, 50), 34472 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17531 + math.random( 5, 50), 34473 + math.random( 5, 50), gv('layer') )
		
		add_field_prop( 30131, 90, 17485 + math.random( 5, 50), 34564 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17428 + math.random( 5, 50), 34343 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17572 + math.random( 5, 50), 34398 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17728 + math.random( 5, 50), 34338 + math.random( 5, 50), gv('layer') )
		
		add_field_prop( 30131, 90, 17764 + math.random( 5, 50), 34480 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17730 + math.random( 5, 50), 34686 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17457 + math.random( 5, 50), 34682 + math.random( 5, 50), gv('layer') )
		add_field_prop( 30131, 90, 17402 + math.random( 5, 50), 34476 + math.random( 5, 50), gv('layer') )
		
		add_field_prop( 30131, 90, 17579 + math.random( 50, 100), 34641 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17672 + math.random( 50, 100), 34564 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17621 + math.random( 50, 100), 34472 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17531 + math.random( 50, 100), 34473 + math.random( 50, 100), gv('layer') )
		
		add_field_prop( 30131, 90, 17485 + math.random( 50, 100), 34564 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17428 + math.random( 50, 100), 34343 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17572 + math.random( 50, 100), 34398 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17728 + math.random( 50, 100), 34338 + math.random( 50, 100), gv('layer') )
		
		add_field_prop( 30131, 90, 17764 + math.random( 50, 100), 34480 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17730 + math.random( 50, 100), 34686 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17457 + math.random( 50, 100), 34682 + math.random( 50, 100), gv('layer') )
		add_field_prop( 30131, 90, 17402 + math.random( 50, 100), 34476 + math.random( 50, 100), gv('layer') )
		end
	
	end	
end

function master_region_level_10232()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3316 = get_quest_progress(3316) -- <(version:7.3)>[마스터]시련의 존32
	
	--진짜 프랍 ID
	local reallight = 30143
	local fakelight = 30149

	local masterlight
	local selected = { 1, 2, 3, 4, 5, 6, 7, 8, 9 }
		
	for i = 1, 2 do
			masterlight = math.random( 1 + i, 9 )
			local temp = selected[ i ]
			selected[ i ] = selected[ masterlight ]
			selected[ masterlight ] = temp
	end
	

	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3316 == 1 then
	
	
		-- 진짜 프랍
		masterlight = selected[ 1 ]
		
		if masterlight == 1 then
			add_field_prop( reallight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 2 then
			add_field_prop( reallight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 3 then
			add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 4 then
			add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 5 then
			add_field_prop( reallight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 6 then
			add_field_prop( reallight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 7 then
			add_field_prop( reallight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 8 then
			add_field_prop( reallight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 9 then
			add_field_prop( reallight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
		end
		
		-- 가짜 프랍
		masterlight = selected[ 2 ]
		
		if masterlight == 1 then
			add_field_prop( fakelight, 45, 20199 + math.random( 10, 100), 33911 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 2 then
			add_field_prop( fakelight, 45, 20407 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 3 then
			add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33505 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 4 then
			add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33707 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 5 then
			add_field_prop( fakelight, 45, 19993 + math.random( 10, 100), 33907 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 6 then
			add_field_prop( fakelight, 45, 20373 + math.random( 10, 100), 33892 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 7 then
			add_field_prop( fakelight, 45, 20409 + math.random( 10, 100), 33490 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 8 then
			add_field_prop( fakelight, 45, 19998 + math.random( 10, 100), 33507 + math.random( 10, 100), gv('layer') )
		elseif masterlight == 9 then
			add_field_prop( fakelight, 45, 20202 + math.random( 10, 100), 33709 + math.random( 10, 100), gv('layer') )
		end
	
	
	end
	
end

function master_region_level_10233()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3317 = get_quest_progress(3317) -- <(version:7.3)>[마스터]시련의 존33
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3317 == 1 then
	
		
		-- 진짜 몬스터 찾기 Lv 2, 마르두카류 리젠
		add_npc( gv('x'), gv('y'), 159014,  1, 21000 )
		add_npc( gv('x'), gv('y'), 159015,  math.random( 1, 3), 21000 )
		add_npc( gv('x'), gv('y'), 159016,  math.random( 1, 3), 21000 )
		add_npc( gv('x'), gv('y'), 159017,  math.random( 1, 3), 21000 )
		add_npc( gv('x'), gv('y'), 159018,  math.random( 1, 3), 21000 )
		add_npc( gv('x'), gv('y'), 159009,  math.random( 1, 3), 21000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
	end
	
end

function master_region_level_10234()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3318 = get_quest_progress(3318) -- <(version:7.3)>[마스터]시련의 존34
	
	local quest_progress3291 = get_quest_progress(3291) -- <(version:7.3)>[마스터]시련의 존1
	local quest_progress3292 = get_quest_progress(3292) -- <(version:7.3)>[마스터]시련의 존2
	local quest_progress3293 = get_quest_progress(3293) -- <(version:7.3)>[마스터]시련의 존3
	local quest_progress3294 = get_quest_progress(3294) -- <(version:7.3)>[마스터]시련의 존4
	local quest_progress3295 = get_quest_progress(3295) -- <(version:7.3)>[마스터]시련의 존5
	
	local quest_progress3296 = get_quest_progress(3296) -- <(version:7.3)>[마스터]시련의 존6
	local quest_progress3297 = get_quest_progress(3297) -- <(version:7.3)>[마스터]시련의 존7
	local quest_progress3298 = get_quest_progress(3298) -- <(version:7.3)>[마스터]시련의 존8
	local quest_progress3299 = get_quest_progress(3299) -- <(version:7.3)>[마스터]시련의 존9
	local quest_progress3300 = get_quest_progress(3300) -- <(version:7.3)>[마스터]시련의 존10
	
	local quest_progress3301 = get_quest_progress(3301) -- <(version:7.3)>[마스터]시련의 존11
	local quest_progress3302 = get_quest_progress(3302) -- <(version:7.3)>[마스터]시련의 존12
	local quest_progress3303 = get_quest_progress(3303) -- <(version:7.3)>[마스터]시련의 존19
	local quest_progress3304 = get_quest_progress(3304) -- <(version:7.3)>[마스터]시련의 존20
	local quest_progress3305 = get_quest_progress(3305) -- <(version:7.3)>[마스터]시련의 존21
	
	local quest_progress3306 = get_quest_progress(3306) -- <(version:7.3)>[마스터]시련의 존22
	local quest_progress3307 = get_quest_progress(3307) -- <(version:7.3)>[마스터]시련의 존23
	local quest_progress3308 = get_quest_progress(3308) -- <(version:7.3)>[마스터]시련의 존24
	local quest_progress3309 = get_quest_progress(3309) -- <(version:7.3)>[마스터]시련의 존25
	local quest_progress3310 = get_quest_progress(3310) -- <(version:7.3)>[마스터]시련의 존26
	
	local quest_progress3311 = get_quest_progress(3311) -- <(version:7.3)>[마스터]시련의 존27
	local quest_progress3312 = get_quest_progress(3312) -- <(version:7.3)>[마스터]시련의 존28
	local quest_progress3313 = get_quest_progress(3313) -- <(version:7.3)>[마스터]시련의 존29
	local quest_progress3314 = get_quest_progress(3314) -- <(version:7.3)>[마스터]시련의 존30
	local quest_progress3315 = get_quest_progress(3315) -- <(version:7.3)>[마스터]시련의 존31
	
	local quest_progress3316 = get_quest_progress(3316) -- <(version:7.3)>[마스터]시련의 존32
	local quest_progress3317 = get_quest_progress(3317) -- <(version:7.3)>[마스터]시련의 존33
	--local quest_progress3318 = get_quest_progress(3318) -- <(version:7.3)>[마스터]시련의 존34
	local quest_progress3319 = get_quest_progress(3319) -- <(version:7.3)>[마스터]시련의 존5'
	local quest_progress3320 = get_quest_progress(3320) -- <(version:7.3)>[마스터]시련의 존31'
	
	
	local state_level = get_state_level( 4552 )
	
		
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3318 == 1 then
	
	
		-- 전사의 존 ============================================================================
		if quest_progress3291 == 255 then cprint( "@91002380" ) --<(version:7.3)>마스터 전사의 존 완료
		else cprint( "@91002384" ) end -- <(version:7.3)>마스터 전사의 존 미완료
		
		
		if quest_progress3292 == 255 then cprint( "@91002380" ) --<(version:7.3)>마스터 전사의 존 완료
		else cprint( "@91002384" ) end -- <(version:7.3)>마스터 전사의 존 미완료
		
		if quest_progress3293 == 255 then cprint( "@91002380" ) --<(version:7.3)>마스터 전사의 존 완료
		else cprint( "@91002384" ) end -- <(version:7.3)>마스터 전사의 존 미완료
		
		if quest_progress3297 == 255 then cprint( "@91002380" ) --<(version:7.3)>마스터 전사의 존 완료
		else cprint( "@91002384" ) end -- <(version:7.3)>마스터 전사의 존 미완료
		
		if quest_progress3298 == 255 then cprint( "@91002380" ) --<(version:7.3)>마스터 전사의 존 완료
		else cprint( "@91002384" ) end -- <(version:7.3)>마스터 전사의 존 미완료
		
		if quest_progress3299 == 255 then cprint( "@91002380" ) --<(version:7.3)>마스터 전사의 존 완료
		else cprint( "@91002384" ) end -- <(version:7.3)>마스터 전사의 존 미완료
		
		
		-- 법사의 존 ============================================================================
		if quest_progress3294 == 255 then cprint( "@91002381" ) --<(version:7.3)>마스터 법사의 존 완료
		else cprint( "@91002385" ) end -- <(version:7.3)>마스터 법사의 존 미완료
		
		if quest_progress3295 == 255 then cprint( "@91002381" ) --<(version:7.3)>마스터 법사의 존 완료
		else cprint( "@91002385" ) end -- <(version:7.3)>마스터 법사의 존 미완료
		
		if quest_progress3296 == 255 then cprint( "@91002381" ) --<(version:7.3)>마스터 법사의 존 완료
		else cprint( "@91002385" ) end -- <(version:7.3)>마스터 법사의 존 미완료
		
		if quest_progress3300 == 255 then cprint( "@91002381" ) --<(version:7.3)>마스터 법사의 존 완료
		else cprint( "@91002385" ) end -- <(version:7.3)>마스터 법사의 존 미완료
		
		if quest_progress3301 == 255 then cprint( "@91002381" ) --<(version:7.3)>마스터 법사의 존 완료
		else cprint( "@91002385" ) end -- <(version:7.3)>마스터 법사의 존 미완료
		
		if quest_progress3302 == 255 then cprint( "@91002381" ) --<(version:7.3)>마스터 법사의 존 완료
		else cprint( "@91002385" ) end -- <(version:7.3)>마스터 법사의 존 미완료
		
		if quest_progress3319 == 255 then cprint( "@91002381" ) --<(version:7.3)>마스터 법사의 존 완료
		else cprint( "@91002385" ) end -- <(version:7.3)>마스터 법사의 존 미완료
		
		
		-- 헌터의 존 ============================================================================
		if quest_progress3303 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3304 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3305 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3306 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3307 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3308 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3315 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3316 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3317 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		if quest_progress3320 == 255 then cprint( "@91002382" ) --<(version:7.3)>마스터 헌터의 존 완료
		else cprint( "@91002386" ) end -- <(version:7.3)>마스터 헌터의 존 미완료
		
		
		-- 소환사의 존 ============================================================================
		if quest_progress3309 == 255 then cprint( "@91002383" ) --<(version:7.3)>마스터 소환사의 존 완료
		else cprint( "@91002387" ) end -- <(version:7.3)>마스터 소환사의 존 미완료
		
		if quest_progress3310 == 255 then cprint( "@91002383" ) --<(version:7.3)>마스터 소환사의 존 완료
		else cprint( "@91002387" ) end -- <(version:7.3)>마스터 소환사의 존 미완료
		
		if quest_progress3311 == 255 then cprint( "@91002383" ) --<(version:7.3)>마스터 소환사의 존 완료
		else cprint( "@91002387" ) end -- <(version:7.3)>마스터 소환사의 존 미완료
		
		if quest_progress3312 == 255 then cprint( "@91002383" ) --<(version:7.3)>마스터 소환사의 존 완료
		else cprint( "@91002387" ) end -- <(version:7.3)>마스터 소환사의 존 미완료
		
		if quest_progress3313 == 255 then cprint( "@91002383" ) --<(version:7.3)>마스터 소환사의 존 완료
		else cprint( "@91002387" ) end -- <(version:7.3)>마스터 소환사의 존 미완료
		
		if quest_progress3314 == 255 then cprint( "@91002383" ) --<(version:7.3)>마스터 소환사의 존 완료
		else cprint( "@91002387" ) end -- <(version:7.3)>마스터 소환사의 존 미완료
		
		
		if quest_progress3291 == 255 and quest_progress3292 == 255 and quest_progress3293 == 255 and
		quest_progress3294 == 255 and quest_progress3295 == 255 and quest_progress3296 == 255 and
		quest_progress3297 == 255 and quest_progress3298 == 255 and quest_progress3299 == 255 and
		quest_progress3300 == 255 and quest_progress3301 == 255 and quest_progress3302 == 255 and
		quest_progress3303 == 255 and quest_progress3304 == 255 and quest_progress3305 == 255 and
		quest_progress3306 == 255 and quest_progress3307 == 255 and quest_progress3308 == 255 and
		quest_progress3309 == 255 and quest_progress3310 == 255 and quest_progress3311 == 255 and
		quest_progress3312 == 255 and quest_progress3313 == 255 and quest_progress3314 == 255 and
		quest_progress3315 == 255 and quest_progress3316 == 255 and quest_progress3317 == 255 and
		quest_progress3319 == 255 and quest_progress3320 == 255 then
		
			
			-- 자격 증명
			add_npc( gv('x'), gv('y'), 145027,  1, 21000 )
			add_npc( gv('x'), gv('y'), 145028,  1, 21000 )
			
			cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
		
		else
		
			cprint( "@91002379" ) -- <(version:7.3)>당신에게는 릭의 자격은 없습니다. 돌아가~
		
		end
		
	end
	
end
--==============================================


function master_region_level_10237()

end

function master_region_level_10238()

end

function master_region_level_10239()

end

function master_region_level_10240()

end

function master_region_level_10241()

end

function master_region_level_10242()

end

function master_region_level_10243()

end

function master_region_level_10244()

end

function master_region_level_10245()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3321 = get_quest_progress(3321) -- <(version:7.3)>[마스터]최종 단계
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  state_level == 1 and quest_progress3321 == 1 then
	
		
		-- 최종 몬스터 처치 
		add_npc( 30538, 40163, 145017,  1, 48000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	
	end
	
end


-- 몬스터  -------------------------------------------------------------------------------
------------------------------------------------------------------------------------------

function master_136006()

end

function master_136007()

end

function master_136008()

end

function master_136009()

end

function master_136010()

end

function master_136011()

end

function master_136012()

end

function master_136013()

end

function master_136014()

end

function master_136015()

end

function master_136016()

end

function master_136017()

end

function master_136018()

end


------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------
function master_145017()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3321 = get_quest_progress(3321) -- <(version:7.3)>[마스터]최종 단계
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  quest_progress3321 == 1 then
	
		-- 최종 몬스터 처치 
		add_npc( 30468 , 40260, 145018,  1, 48000 )
		add_npc( 30468 , 40381, 145019,  1, 48000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	
	end
	
end

function master_145018()

end

function master_145019()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3321 = get_quest_progress(3321) -- <(version:7.3)>[마스터]최종 단계
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  quest_progress3321 == 1 then
	
		
		-- 최종 몬스터 처치 
		add_npc( 30539 , 40478, 145020,  1, 48000 )
		add_npc( 30768 , 40478, 145023,  1, 48000 )
		add_npc( 30838 , 40381, 145022,  1, 48000 )
		add_npc( 30839 , 40261, 145021,  1, 48000 )
		
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	
	end
	
end

function master_145020()

end

function master_145021()

end

function master_145022()

end

function master_145023()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3321 = get_quest_progress(3321) -- <(version:7.3)>[마스터]최종 단계
	
	local state_level = get_state_level( 4552 )
	
	
	-- 고대 크리스탈 효과 상태 체크
	if  quest_progress3321 == 1 then
	
		
		-- 최종 몬스터 처치 
		add_npc( 30768 , 40163, 145024,  1, 48000 )
		cprint( "@90999875" ) -- <#6DD66D>몬스터가 나타났다.
	
	end
	
end

function master_145024()

end

------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------

function master_159008()

end

function master_159009()

end

function master_159010()

end

function master_159011()

end

function master_159012()

end

function master_159013()

end

function master_159014()

end

function master_159015()

end

function master_159016()

end

function master_159017()

end

function master_159018()

end

function master_159019()

end

function master_159020()

end


------------------------------------------------------------------------------------------

function master_177008()

end

function master_177009()

end

function master_177010()

end

function master_177011()

end

function master_177012()

end

function master_177013()

end

function master_177014()

end

function master_177015()

end

function master_177016()

end

function master_177017()

end

function master_177018()

end

function master_177019()

end

function master_177020()

end

------------------------------------------------------------------------------------------
function master_158024()

	local quest_progress3298 = get_quest_progress(3298) -- <(version:7.3)>[마스터]시련의 존8
	
	
	if quest_progress3298 == 1 then
	
		-- <(version:7.3)>[마스터]시련의 존5
		local master_light3298 = get_quest_status( 3298 , 1 ) + 1
		
		set_quest_status( 3298, 1, master_light3298 )
	end
	
end

function master_158025()
	
	local quest_progress3298 = get_quest_progress(3298) -- <(version:7.3)>[마스터]시련의 존8
	
	
	if quest_progress3298 == 1 then
	
		-- <(version:7.3)>[마스터]시련의 존5
		local master_light3298 = get_quest_status( 3298 , 1 ) + 1
		
		set_quest_status( 3298, 1, master_light3298 )
	end
	
end

function master_168008()

end

function master_168009()

end

function master_178009()
	
	local quest_progress3310 = get_quest_progress(3310) -- <(version:7.3)>[마스터]시련의 존8
	
	
	if quest_progress3310 == 1 then
	
		-- <(version:7.3)>[마스터]시련의 존5
		local master_light3310 = get_quest_status( 3310 , 1 ) + 1
		
		set_quest_status( 3310, 1, master_light3310 )
	end
	
	
end

function master_178010()
	
	local quest_progress3310 = get_quest_progress(3310) -- <(version:7.3)>[마스터]시련의 존8
	
	
	if quest_progress3310 == 1 then
	
		-- <(version:7.3)>[마스터]시련의 존5
		local master_light3310 = get_quest_status( 3310 , 1 ) + 1
		
		set_quest_status( 3310, 1, master_light3310 )
	end
	
	
end


-- NPC    --------------------------------------------------------------------------------
------------------------------------------------------------------------------------------

-- 정신잃은 고대사제 
function NPC_master_acientpriest_contact()
	
	dlg_title("@91002201")
	dlg_text("@91002202")

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	--local quest_progress3271 =  get_quest_progress(3271)
	-- 퀘스트 종료시 대사	
	--if quest_progress3271 == 255 then
	
	-- 마스터 던전 외부로으로 이동
	dlg_menu( "@91002087", 'warp( 138666, 75202, gv("layer") )' )
	
	--end	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_a_contact()
	
	dlg_title("@91002201")
	dlg_text("@91002202")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


function NPC_master_acientpriest_b_contact()
	
	dlg_title("@91002201")
	dlg_text("@91002202")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


function NPC_master_acientpriest_c_contact()
	
	dlg_title("@91002201")
	dlg_text("@91002202")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


function NPC_master_acientpriest_d_contact()
	
	dlg_title("@91002201")
	dlg_text("@91002202")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_e_contact()
	
	dlg_title("@91002201")
	dlg_text("@91002202")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


function NPC_master_acientpriest_01_contact()
	
	dlg_title("@91002204")
	dlg_text("@91002205")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_02_contact()
	
	dlg_title("@91002207")
	dlg_text("@91002208")
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_03_contact()
	
	dlg_title("@91002210")
	dlg_text("@91002211")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_04_contact()
	
	dlg_title("@91002213")
	dlg_text("@91002214")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_05_contact()
	
	dlg_title("@91002216")
	dlg_text("@91002217")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_06_contact()
	
	dlg_title("@91002219")
	dlg_text("@91002220")
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_07_contact()
	
	dlg_title("@91002222")
	dlg_text("@91002223")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_08_contact()
	
	dlg_title("@91002225")
	dlg_text("@91002226")
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_09_contact()
	
	dlg_title("@91002228")
	dlg_text("@91002229")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_10_contact()
	
	dlg_title("@91002231")
	dlg_text("@91002232")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_11_contact()
	
	dlg_title("@91002234")
	dlg_text("@91002235")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_12_contact()
	
	dlg_title("@91002237")
	dlg_text("@91002238")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_19_contact()
	
	dlg_title("@91002240")
	dlg_text("@91002241")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_20_contact()
	
	dlg_title("@91002243")
	dlg_text("@91002244")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_21_contact()
	
	dlg_title("@91002246")
	dlg_text("@91002247")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_22_contact()
	
	dlg_title("@91002249")
	dlg_text("@91002250")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_23_contact()
	
	dlg_title("@91002252")
	dlg_text("@91002253")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_24_contact()
	
	dlg_title("@91002255")
	dlg_text("@91002256")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_25_contact()
	
	dlg_title("@91002258")
	dlg_text("@91002259")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_26_contact()
	
	dlg_title("@91002261")
	dlg_text("@91002262")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_27_contact()
	
	dlg_title("@91002264")
	dlg_text("@91002265")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_28_contact()
	
	dlg_title("@91002267")
	dlg_text("@91002268")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_29_contact()
	
	dlg_title("@91002270")
	dlg_text("@91002271")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_30_contact()
	
	dlg_title("@91002273")
	dlg_text("@91002274")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_31_contact()
	
	dlg_title("@91002276")
	dlg_text("@91002277")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_32_contact()
	
	dlg_title("@91002279")
	dlg_text("@91002280")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_33_contact()
	
	dlg_title("@91002282")
	dlg_text("@91002283")

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function NPC_master_acientpriest_34_contact()
	
	dlg_title("@91002285")
	--dlg_text("@91002286")
	
	
	local quest_progress3318 = get_quest_progress(3318) -- <(version:7.3)>[마스터]시련의 존34
	
	
	if quest_progress3318 == 255 then
	
		dlg_text("@91002088")
		
	else
	
		dlg_text("@91002286")
		-- 정보 1
		dlg_menu( "@91002375", "master_npc_talk_34_1()" )
	
	end

	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정보 1
function master_npc_talk_34_1()
	
	dlg_title("@91002285")
	dlg_text("@91002377")

	
	-- 정보 2
	dlg_menu( "@91002376", "master_npc_talk_34_2()" )
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정보 1
function master_npc_talk_34_2()
	
	dlg_title("@91002285")
	dlg_text("@91002378")

	
	-- 정보 2
	dlg_menu( "@91002165", "master_npc_talk_34_3()" )
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정보 1
function master_npc_talk_34_3()
	
	dlg_title("@91002285")
	dlg_text("@91002166")

	
	-- 정보 2
	dlg_menu( "@91002167", "master_npc_talk_34_4()" )
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


-- 정보 1
function master_npc_talk_34_4()
	
	dlg_title("@91002285")
	dlg_text("@91002168")

	
		-- 정보 2
		dlg_menu( "@91002169", "master_npc_talk_34_5()" )
		
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정보 1
function master_npc_talk_34_5()
	
	local quest_progress3318 = get_quest_progress(3318) -- <(version:7.3)>[마스터]시련의 존34
	
	
	if quest_progress3318 == 0 then
	
		force_start_quest(3318, 91002159)
		
	else
		
		cprint("@91002292") --<(version:7.3)><#6DD66D>퀘스트를 받을 수 없습니다.
	
	end
end

function NPC_master_acientpriest_9_1_contact()
	
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3299 = get_quest_progress(3299) -- <(version:7.3)>[마스터]시련의 존9
	
		
	if quest_progress3299 == 1 then
	
		dlg_title("@91002296")
		dlg_text("@91002331")
	
		-- 정보 1
		dlg_menu( "@91002332", "master_npc_talk_9_1()" )
	else
	
		dlg_title("@91002296")
		dlg_text("@91002297")
	
	end
		
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정보 1
function master_npc_talk_9_1()
	
	dlg_title("@91002296")
	dlg_text("@91002333")

	
	-- 정보 2
	dlg_menu( "@91002334", "master_npc_talk_9_2()" )
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정보 2
function master_npc_talk_9_2()
	
	dlg_title("@91002296")
	dlg_text("@91002335")

	
	-- 정보 3
	dlg_menu( "@91002336", "master_npc_talk_9_3()" )
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정보 3
function master_npc_talk_9_3()
	
	dlg_title("@91002296")
	dlg_text("@91002337")

	
	-- 정답
	dlg_menu( "@91002338", "master_npc_answer_9()" )
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


-- 정답
function master_npc_answer_9()
	
	dlg_title("@91002296")
	dlg_text("@91002339")
	
	-- 답 1
	dlg_menu( "@91002340", "master_npc_answer_end_9_1()" )
	-- 답 2
	dlg_menu( "@91002341", "master_npc_answer_end_9_1()" )
	-- 답 3
	dlg_menu( "@91002342", "master_npc_answer_end_9_1()" )
	-- 답 4
	dlg_menu( "@91002343", "master_npc_answer_end_9_2()" )
	-- 답 5
	dlg_menu( "@91002344", "master_npc_answer_end_9_1()" )
	-- 답 6
	dlg_menu( "@91002345", "master_npc_answer_end_9_1()" )
	-- 답 7
	dlg_menu( "@91002346", "master_npc_answer_end_9_1()" )
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정답 결과
function master_npc_answer_end_9_1()
	
	dlg_title("@91002296")
	dlg_text("@91002347")
	
	-- 지속효과 삭제
	remove_state( 4554, 20 )
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정답 결과
function master_npc_answer_end_9_2()
	
	dlg_title("@91002296")
	dlg_text("@91002348")

	set_quest_status( 3299, 1, 1 )
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


--============================================================
function NPC_master_acientpriest_10_1_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3300 = get_quest_progress(3300) -- <(version:7.3)>[마스터]시련의 존10
			
	if quest_progress3300 == 1 then
	
		dlg_title("@91002296")
		dlg_text("@91002360")
	
		-- 정답 고르기
		dlg_menu( "@91002372", "master_npc_answer_10()" )
		
	else
	
		dlg_title("@91002299")
		dlg_text("@91002300")
	
	end
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


function master_npc_answer_10()

	dlg_title("@91002296")
	dlg_text("@91002360")
	
	-- 정답 고르기
	dlg_menu( "@91002365", "master_npc_answer_10_1()" )
	-- 정답 고르기
	dlg_menu( "@91002366", "master_npc_answer_10_2()" )
	-- 정답 고르기
	dlg_menu( "@91002367", "master_npc_answer_10_1()" )
	-- 정답 고르기
	dlg_menu( "@91002368", "master_npc_answer_10_1()" )

	
end

-- 정답 결과
function master_npc_answer_10_1()
	
	dlg_title("@91002296")
	dlg_text("@91002373")
	
	-- 지속효과 삭제
	remove_state( 4554, 20 )
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정답 결과
function master_npc_answer_10_2()
	
	dlg_title("@91002296")
	dlg_text("@91002374")
		
	set_quest_status( 3300, 1, 1 )
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


function NPC_master_acientpriest_10_2_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3300 = get_quest_progress(3300) -- <(version:7.3)>[마스터]시련의 존10
			
	if quest_progress3300 == 1 then
	
		dlg_title("@91002302")
		dlg_text("@91002361")
	
	else
	
		dlg_title("@91002302")
		dlg_text("@91002303")
	
	end
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end



function NPC_master_acientpriest_10_3_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3300 = get_quest_progress(3300) -- <(version:7.3)>[마스터]시련의 존10
			
	if quest_progress3300 == 1 then
	
		dlg_title("@91002305")
		dlg_text("@91002362")
	
	else
	
		dlg_title("@91002305")
		dlg_text("@91002306")
	
	end
			
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end



function NPC_master_acientpriest_10_4_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3300 = get_quest_progress(3300) -- <(version:7.3)>[마스터]시련의 존10
			
	if quest_progress3300 == 1 then
	
		dlg_title("@91002308")
		dlg_text("@91002363")
	
	else
	
		dlg_title("@91002308")
		dlg_text("@91002309")
	
	end
		
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end



function NPC_master_acientpriest_10_5_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3300 = get_quest_progress(3300) -- <(version:7.3)>[마스터]시련의 존10
			
	if quest_progress3300 == 1 then
	
		dlg_title("@91002311")
		dlg_text("@91002364")
	
	else
	
		dlg_title("@91002311")
		dlg_text("@91002312")
	
	end
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


--============================================================
function NPC_master_acientpriest_19_1_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3303 = get_quest_progress(3303) -- <(version:7.3)>[마스터]시련의 존19
			
	if quest_progress3303 == 1 then
	
		dlg_title("@91002314")
		dlg_text("@91002349")
		
		-- 정답 고르기
		dlg_menu( "@91002369", "master_npc_answer_19()" )
	
	else
	
		dlg_title("@91002314")
		dlg_text("@91002315")
	
	end
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function master_npc_answer_19()

	dlg_title("@91002314")
	dlg_text("@91002349")
	
	-- 정답 고르기
	dlg_menu( "@91002355", "master_npc_answer_19_1()" )
	-- 정답 고르기
	dlg_menu( "@91002356", "master_npc_answer_19_1()" )
	-- 정답 고르기
	dlg_menu( "@91002357", "master_npc_answer_19_1()" )
	-- 정답 고르기
	dlg_menu( "@91002358", "master_npc_answer_19_2()" )
	-- 정답 고르기
	dlg_menu( "@91002359", "master_npc_answer_19_1()" )
	
end

-- 정답 결과
function master_npc_answer_19_1()
	
	dlg_title("@91002314")
	dlg_text("@91002370")
	
	-- 지속효과 삭제
	remove_state( 4554, 20 )
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

-- 정답 결과
function master_npc_answer_19_2()
	
	dlg_title("@91002314")
	dlg_text("@91002371")

	set_quest_status( 3303, 1, 1 )
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end


function NPC_master_acientpriest_19_2_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3303 = get_quest_progress(3303) -- <(version:7.3)>[마스터]시련의 존19
			
	if quest_progress3303 == 1 then
	
		dlg_title("@91002317")
		dlg_text("@91002350")
	
	else
	
		dlg_title("@91002317")
		dlg_text("@91002318")
	
	end
	

	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end



function NPC_master_acientpriest_19_3_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3303 = get_quest_progress(3303) -- <(version:7.3)>[마스터]시련의 존19
			
	if quest_progress3303 == 1 then
	
		dlg_title("@91002320")
		dlg_text("@91002351")
	
	else
	
		dlg_title("@91002320")
		dlg_text("@91002321")
	
	end
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end



function NPC_master_acientpriest_19_4_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3303 = get_quest_progress(3303) -- <(version:7.3)>[마스터]시련의 존19
			
	if quest_progress3303 == 1 then
	
		dlg_title("@91002323")
		dlg_text("@91002352")
	
	else
	
		dlg_title("@91002323")
		dlg_text("@91002324")
	
	end
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end



function NPC_master_acientpriest_19_5_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3303 = get_quest_progress(3303) -- <(version:7.3)>[마스터]시련의 존19
			
	if quest_progress3303 == 1 then
	
		dlg_title("@91002326")
		dlg_text("@91002353")
	
	else
	
		dlg_title("@91002326")
		dlg_text("@91002327")
	
	end
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end



function NPC_master_acientpriest_19_6_contact()
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3303 = get_quest_progress(3303) -- <(version:7.3)>[마스터]시련의 존19
			
	if quest_progress3303 == 1 then
	
		dlg_title("@91002329")
		dlg_text("@91002354")
	
	else
	
		dlg_title("@91002329")
		dlg_text("@91002330")
	
	end
	
	
	-- 종료
	dlg_menu( "@90010002", '' )
 
	dlg_show()
end

function Vulcanus_level1_1()

	cprint( "!포털 가동" )
	add_field_prop( 126023, 300, 196947 + math.random(0, 300), 28512 + math.random(0, 300))

end

 --============================================================
   --             <<<<<< 4대 인던 지하기지 입구 NPC >>>>>>
 --============================================================
   
function NPC_Teleport_instanceDuneGeon_contact()
 	
	local state_code = get_local_info()
	if get_state_level( 201085 ) > 0 and state_code == 256 then
		return
	end
	
	local npc_id = get_npc_id()
	local level = get_value( 'level' )
	
		if level < 150 then
	
			if npc_id == 11812 then
				dlg_title( "@90605758" )--선발대 잭
				dlg_text( "@90605763" )		
					
			elseif npc_id == 11813 then
				dlg_title( "@90605759" )--선발대 케이트
				dlg_text( "@90605763" )		
					
			elseif npc_id == 11814 then
				dlg_title( "@90605760" )--선발대 사이드
				dlg_text( "@90605763" )		
			elseif npc_id == 11815 then
				dlg_title( "@90605761" )--선발대 진수
				dlg_text( "@90605763" )		
			elseif npc_id == 13000 then
				dlg_title( "@90606149" )--선발대 피에 (서커스)
				dlg_text( "@90605763" )	
			end

		else
			if npc_id == 11812 then
				dlg_title( "@90605758" )--선발대 잭
				dlg_text( "@90605762" )
				dlg_menu("@690000090", "warp_to_instance_dungeon(40000, 0)")--메마른 딜빛 유적 지하기지 1단계 입장
				dlg_menu("@690000091", "warp_to_instance_dungeon(40000, 1)")--메마른 딜빛 유적 지하기지 2단계 입장
			elseif npc_id == 11813 then
				dlg_title( "@90605759" )--선발대 케이트
				dlg_text( "@90605762" )	
				dlg_menu("@690000090", "warp_to_instance_dungeon(41001, 0)")--발모어 탄광 지하기지 1단계 입장
				dlg_menu("@690000091", "warp_to_instance_dungeon(41001, 1)")--발모어 탄광 지하기지 2단계 입장
			elseif npc_id == 11814 then
				dlg_title( "@90605760" )--선발대 사이드
				dlg_text( "@90605762" )
				dlg_menu("@690000090", "warp_to_instance_dungeon(42001, 0)")--수정계곡 지하기지 1단계 입장
				dlg_menu("@690000091", "warp_to_instance_dungeon(42001, 1)")--수정계곡 지하기지 2단계 입장
			elseif npc_id == 11815 then
				dlg_title( "@90605761" )--선발대 진수
				dlg_text( "@90605762" )
				dlg_menu("@690000090", "warp_to_instance_dungeon(43001, 0)")--붉은 거미 서커스 1단계 입장
				dlg_menu("@690000091", "warp_to_instance_dungeon(43001, 1)")--붉은 거미 서커스 2단계 입장
			elseif npc_id == 13000 then
				dlg_title( "@90606149" )--서커스 입장 npc
				dlg_text( "@90606148" )
				dlg_menu("@690000141", "warp_to_instance_dungeon(50000, 0)")--붉은 거미 서커스 1단계 입장
				dlg_menu("@690000142", "warp_to_instance_dungeon(50000, 1)")--붉은 거미 서커스 2단계 입장
			end 
		end 	

		if level >= 160 then

			if npc_id == 11812 then
				dlg_menu("@690000092", "warp_to_instance_dungeon(40000, 2)")--메마른 딜빛 유적 지하기지 3단계 입장
				dlg_menu("@690000093", "warp_to_instance_dungeon(40000, 3)")--메마른 딜빛 유적 지하기지 4단계 입장
			elseif npc_id == 11813 then
				dlg_menu("@690000092", "warp_to_instance_dungeon(41001, 2)")--발모어 탄광 지하기지 3단계 입장
				dlg_menu("@690000093", "warp_to_instance_dungeon(41001, 3)")--발모어 탄광 지하기지 4단계 입장
			elseif npc_id == 11814 then
				dlg_menu("@690000092", "warp_to_instance_dungeon(42001, 2)")--수정계곡 지하기지 3단계 입장
				dlg_menu("@690000093", "warp_to_instance_dungeon(42001, 3)")--수정계곡 지하기지 4단계 입장
				
			elseif npc_id == 11815 then
				dlg_menu("@690000092", "warp_to_instance_dungeon(43001, 2)")--붉은 거미 서커스 3단계 입장
				dlg_menu("@690000093", "warp_to_instance_dungeon(43001, 3)")--붉은 거미 서커스 4단계 입장
			elseif npc_id == 13000 then
				dlg_menu("@690000143", "warp_to_instance_dungeon(50000, 2)")--붉은 거미 서커스 3단계 입장
				dlg_menu("@690000144", "warp_to_instance_dungeon(50000, 3)")--붉은 거미 서커스 4단계 입장
						
			end	
			
		end
		
		dlg_menu( "@90010002", " " )
			

	dlg_show()
	
end


--============================================================================
--                패러렐 월드(version:9.2) : 파견된 특별 조사관
--============================================================================

function NPC_Teleport_parallelworld_contact()

	local t_flag = get_flag( "get_ticket_time" )
	local current_time = get_os_date( "%Y-%m-%d" )
	local npc_id = get_npc_id()
	
		if npc_id == 13081 then
			dlg_title( "@90610000" )--파견된 라크시 특별 조사관
			dlg_text( "@90610005" )
			
		elseif npc_id == 13082 then
			dlg_title( "@90610001" )--파견된 카탄 특별 조사관
			dlg_text( "@90610005" )	
						
		elseif npc_id == 13083 then
			dlg_title( "@90610002" )--파견된 호라이즌 특별 조사관
			dlg_text( "@90610005" )	
						
		elseif npc_id == 13084 then
			dlg_title( "@90610003" )--파견된 론도 특별 조사관
			dlg_text( "@90610005" )

		elseif npc_id == 13085 then
			dlg_title( "@90610081" )--파견된 도시유적 특별 조사관
			dlg_text( "@90610005" )
			
		elseif npc_id == 13086 then
			dlg_title( "@90610004" )--파견된 시크루트 특별 조사관
			dlg_text( "@90610005" )	
		end

		dlg_menu( "@90610006", "NPC_teleport_parallelworld_dlg()" )
					
		if t_flag < current_time or t_flag == nil or t_flag == 0 then	
			dlg_menu( "@90610017", "NPC_teleport_parallelworld_getticket()" )
		end
		
		if find_item( 2016025 ) > 0 then
			dlg_menu( "@90610011", "NPC_teleport_parallelworld_warp(1)" )
		end
		
		if get_state_level( 41110007 ) > 0 then -- 재접속 가능 지속효과가 있을 경우 나타내줌
			dlg_menu( "@90610083", "NPC_teleport_parallelworld_warp(2)" )
		end
		
		dlg_menu( "@90010002", " " )

		dlg_show()
end
   
function NPC_teleport_parallelworld_dlg()

	local npc_id = get_npc_id()

		if npc_id == 13081 then
			dlg_title( "@90610000" )--파견된 라크시 특별 조사관
			dlg_text( "@90610005" )
			
		elseif npc_id == 13082 then
			dlg_title( "@90610001" )--파견된 카탄 특별 조사관
			dlg_text( "@90610005" )	
						
		elseif npc_id == 13083 then
			dlg_title( "@90610002" )--파견된 호라이즌 특별 조사관
			dlg_text( "@90610005" )	
						
		elseif npc_id == 13084 then
			dlg_title( "@90610003" )--파견된 론도 특별 조사관
			dlg_text( "@90610005" )

		elseif npc_id == 13085 then
			dlg_title( "@90610081" )--파견된 도시유적 특별 조사관
			dlg_text( "@90610005" )
			
		elseif npc_id == 13086 then
			dlg_title( "@90610004" )--파견된 시크루트 특별 조사관
			dlg_text( "@90610005" )	
		end
		
		dlg_text( "@90610007" )
		dlg_menu( "@90010003", "NPC_Teleport_parallelworld_contact()" )
		dlg_menu( "@90010002", " " )
		
		dlg_show()
		
end

function NPC_teleport_parallelworld_getticket()
	
	local quest_progress4009 = get_quest_progress(4009)
	local t_flag = get_flag( "get_ticket_time" )
	local current_time = get_os_date( "%Y-%m-%d" )

	local npc_id = get_npc_id()

		if npc_id == 13081 then
			dlg_title( "@90610000" )--파견된 라크시 특별 조사관
			dlg_text( "@90610005" )
			
		elseif npc_id == 13082 then
			dlg_title( "@90610001" )--파견된 카탄 특별 조사관
			dlg_text( "@90610005" )	
						
		elseif npc_id == 13083 then
			dlg_title( "@90610002" )--파견된 호라이즌 특별 조사관
			dlg_text( "@90610005" )	
						
		elseif npc_id == 13084 then
			dlg_title( "@90610003" )--파견된 론도 특별 조사관
			dlg_text( "@90610005" )

		elseif npc_id == 13085 then
			dlg_title( "@90610081" )--파견된 도시유적 특별 조사관
			dlg_text( "@90610005" )
			
		elseif npc_id == 13086 then
			dlg_title( "@90610004" )--파견된 시크루트 특별 조사관
			dlg_text( "@90610005" )	
		end		
		
		if get_value( 'level' ) > 149 then
			if find_item( 2016026 ) == 0 then
				insert_item( 2016026, 1 )
				set_flag( "get_ticket_time", current_time )
				if quest_progress4009 == 1 or quest_progress4009 == 100 or quest_progress4009 == 255 then
					dlg_text( "@90610018" )
				else
					dlg_text( "@90610018" )
				end
			elseif find_item( 2016026 ) > 0 then
				dlg_text( "@90610019" )
			end
		elseif get_value( 'level' ) < 150 then
			dlg_text ( "@9908" )
		end
				
		dlg_menu( "@90010003", "NPC_Teleport_parallelworld_contact()" )
		dlg_menu( "@90010002", " " )
		
		dlg_show()
end

function NPC_teleport_parallelworld_warp( number )

	local npc_id = get_npc_id()
	
		if npc_id == 13081 then
			dlg_title( "@90610000" )--파견된 라크시 특별 조사관
			dlg_text( "@90610005" )
			
		elseif npc_id == 13082 then
			dlg_title( "@90610001" )--파견된 카탄 특별 조사관
			dlg_text( "@90610005" )	
						
		elseif npc_id == 13083 then
			dlg_title( "@90610002" )--파견된 호라이즌 특별 조사관
			dlg_text( "@90610005" )	
						
		elseif npc_id == 13084 then
			dlg_title( "@90610003" )--파견된 론도 특별 조사관
			dlg_text( "@90610005" )

		elseif npc_id == 13085 then
			dlg_title( "@90610081" )--파견된 도시유적 특별 조사관
			dlg_text( "@90610005" )
			
		elseif npc_id == 13086 then
			dlg_title( "@90610004" )--파견된 시크루트 특별 조사관
			dlg_text( "@90610005" )	
		end
		
		if number == 1 then
			if get_value( 'level' ) < 150 then
				dlg_text ( "@9908" )
			elseif get_value( 'level' ) > 149 then
				if find_item( 2016025 ) > 0 then
					dlg_text( "@90610012" )
					dlg_special( 'confirm_window', 'parallelworld_warp( 1 )', '@9907' )  -- 패러렐 월드<1>로 입장하시겠습니까?
				elseif find_item( 2016025 ) == 0 then
					dlg_text( "@9912" )
				end
			end
		elseif number == 2 then
			dlg_text( "@90610084" )
			dlg_special( 'confirm_window', 'parallelworld_warp( 2 )', '@9907' )  -- 패러렐 월드<1>로 입장하시겠습니까?
		end
	
		dlg_menu( "@90010003", "NPC_Teleport_parallelworld_contact()" )
		dlg_menu( "@90010002", " " )
			
		dlg_show()
end

function parallelworld_warp( number )
	if number == 1 then	
		if find_item( 2016025 ) > 0 then
			delete_item( get_item_handle( 2016025 ), 1 )
			
			if get_state_level( 41110007 ) > 0 then
				remove_state( 41110007, 1 ) -- 재조사로 들어갈 시 효과 없애줌
				add_state( 41110007, 1, 360000 ) -- 1시간동안 지속효과 부여
			else
				add_state( 41110007, 1, 360000 ) -- 1시간동안 지속효과 부여
			end			
			warp_to_instance_dungeon(60000)
		end
	elseif number == 2 then
		if get_state_level( 41110007 ) > 0 then
			warp_to_instance_dungeon( 60000 )
			remove_state( 41110007, 1 ) -- 재조사로 들어갈 시 효과 없애줌
		elseif get_state_level( 41110007 ) == 0 then
			private_notice( "@90610085" ) -- 그동안 재조사 시간이 모두 지난 경우
		end
	end
end

------------------------------------------------------------------------------------------

function quest_end_4015()

	local count = find_item( 1000521 )
	
	if count == 1 then
		delete_item( get_item_handle( 1000521 ), count )
		cprint( "@91000777" ) -- <(version:7.2)><#6DD66D>아이템을 건데 주었습니다.
	end
		
end

function quest_start_4019()

	local count = find_item( 1000490 )
	
	if count == 0 then
		insert_item( 1000490, 1 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
	end
	

end

function quest_end_4019()

	local count = find_item( 1000490 )
	
	if count == 1 then
		delete_item( get_item_handle( 1000490 ), count )
		cprint( "@91000777" ) -- <(version:7.2)><#6DD66D>아이템을 건데 주었습니다.
	end
		
end

------------------------------------------------------------------------------------------

function quest_end_4023()

	local talentp = get_value( 'tp' )
	talentp = talentp + 1
	set_value( "tp", talentp )
	
	cprint( "@90604780" ) -- <(version:7.3)><#6DD66D>TP 포인트가 1 지급 되었습니다.
	
end

function quest_end_4026()
	
	local talentp = get_value( 'tp' )
	talentp = talentp + 1
	set_value( "tp", talentp )
	
	cprint( "@90604780" ) -- <(version:7.3)><#6DD66D>TP 포인트가 1 지급 되었습니다.
	
end

------------------------------------------------------------------------------------------
-- add_npc( X좌표, Y좌표, 몬스터 ID,  1, 레이어 )
function field_prop_summon(num)
	
	mob_count = 8
	
	if num == 1 then
		mob_id = 22000172
	elseif num == 2 then
		mob_id = 22000171
	elseif num == 3 then
		mob_id = 22000170
	elseif num == 4 then
		mob_id = 22000169
	end
	
	for i = 1, mob_count do
		add_npc( get_value( "x" )  + math.random( -50, 50), get_value( "y" ) + math.random( -50, 50), mob_id,  1 )
	end
end

function field_9_4_boss_summon(num)

	local selected
	local x_pos
	local y_pos

	if num == 4 then -- 중보
		add_npc( 166813, 153551, 22000168,  1 , 0 )
	else	
		if num == 1 then
			selected = { 22000164, 22000165 }
			x_pos = 166841
			y_pos = 148262
		elseif num == 2 then
			selected = { 22000162, 22000163 }
			x_pos = 164012
			y_pos = 151713
		elseif num == 3 then
			selected = { 22000166, 22000167 }
			x_pos = 164050
			y_pos = 157091
		end
		
		delete_item( get_item_handle( 2016092 ), 10 )
		
		local monster_id = selected[math.random( 1, 2 )]	
		
		add_npc( x_pos, y_pos, monster_id,  1 , 0 )
	end
		
end