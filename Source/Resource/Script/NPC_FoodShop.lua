-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_FoodShop"
end
 
   --============================================================
   --             <<<<<< 식료품 상인 >>>>>>
   --============================================================

function NPC_food_shop_contact()

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id() 
	
	local qstart_text = get_value( "level" )
	local quest_progress1 =  get_quest_progress(2008)
	local quest_progress100 =  get_quest_progress(2000)

	-- 라크시 디오스
	if npc_id == 1090 then
		dlg_title("@90109001")
		dlg_text( "@90109002" )
		
		-- 미래를 예언하는 소녀
		if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
		dlg_menu( "@90999617", "quest_rumor8()" )
		end
		
		-- 광신도 암살자
		if quest_progress1 == 255 then
		dlg_menu( "@90999842", "quest_witcharmy2()" )
		end
		
	-- 카탄 지강
	elseif npc_id == 2088 then	
		dlg_title("@90208801")
		dlg_text( "@90208802" )
		
--[[할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
		dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]
		-- 미래를 예언하는 소녀
		if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
		dlg_menu( "@90999617", "quest_rumor9()" )
		end
		
		-- 광신도 암살자
		if quest_progress1 == 255 then
		dlg_menu( "@90999842", "quest_witcharmy1()" )
		end
		
	-- 수련자 섬
	elseif npc_id == 3026 then	
		dlg_title("@90302601")
		dlg_text( "@90302602" )
	-- 호라이즌 이호
	elseif npc_id == 4084 then	
		dlg_title("@90408401")

		if get_quest_progress(10173) == 1 and find_item( 1100313 ) == 0 then
			dlg_text_without_quest_menu( "@90408402" )
			dlg_menu( "@80010173", "Quest_Link_115_1()" )
		else
			dlg_text( "@90408402" )
			
			-- 미래를 예언하는 소녀
			if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
			dlg_menu( "@90999617", "quest_rumor7()" )
			end
			
			-- 광신도 암살자
			if quest_progress1 == 255 then
			dlg_menu( "@90999842", "quest_witcharmy1()" )
			end
		end

	-- 론도
	elseif npc_id == 6046 then
		dlg_title("@90604601")
		dlg_text( "@90604602" )
	-- 도시 유적
	elseif npc_id == 7045 then
		dlg_title("@90704501")	
		dlg_text( "@90704502" )
	-- 마리캣 마켓 일 때 (11132 식료품 상인 코니시)
	elseif npc_id == 11132 then
		dlg_title("@90999422")
		dlg_text( "@90999423" )
	end	
	    dlg_menu( "@90010187", "open_market( 'Foodshop_etc' )" ) 
	    dlg_menu( "@90010002", '' ) 
	    dlg_show()
			
	-- 시크루트
	if npc_id == 7044 then
		dlg_title("@90704401")	
		dlg_text( "@90704402" )
--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
		dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]	
		if is_premium() then
			dlg_menu( "@90010187", "open_market( 'secroute_foodshop_etc' )" ) 
		else
			dlg_menu( "@90010187", "open_market( 'Foodshop_etc' )" ) 
		end		
	end

	dlg_menu( "@90010002", '' ) 
	dlg_show()
 
end

function Quest_Link_115_1()
	if find_item( 1100313 ) == 0 then
		dlg_title("@90408401")
		dlg_text_without_quest_menu( "@90999593" )
		dlg_menu("@90010002", '')
		dlg_show()

		insert_item( 1100313, 1 )
	else
		dlg_title("@90408401")
		dlg_menu( "@90010002", '' ) 
		dlg_show()		
	end
end

-- 세부대화 이호
function quest_rumor7()
	-- 다이얼로그 출력
	dlg_title("@90408401")

	dlg_text( "@90999618" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_4()" )
		
	dlg_menu( "@90010002", '' ) 		
	dlg_show()

end

-- 세부대화 
function quest_rumor_a_4()
	-- 다이얼로그 출력
	dlg_title("@90408401")

	dlg_text( "@90999623" )
	
	-- 세부대화 1-2, 소녀에 관한 신상정보
	dlg_menu( "@90999627", "quest_rumor_b_4()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 레샤에 관한 정보를 준다.
function quest_rumor_b_4()
	-- 다이얼로그 출력
	dlg_title("@90408401")

	-- 세부대화 1-3, 소녀에 관한 위치정보
	dlg_text( "@90999629" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 디오스
function quest_rumor8()
	-- 다이얼로그 출력
	dlg_title("@90109001")

	dlg_text( "@90999618" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_5()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 
function quest_rumor_a_5()
	-- 다이얼로그 출력
	dlg_title("@90109001")

	dlg_text( "@90999623" )
	
	-- 세부대화 1-2, 소녀에 관한 신상정보
	dlg_menu( "@90999627", "quest_rumor_b_5()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 레샤에 관한 정보는 없음.
function quest_rumor_b_5()
	-- 다이얼로그 출력
	dlg_title("@90109001")

	-- 세부대화 1-3, 소녀에 관한 위치정보
	dlg_text( "@90999628" )
				
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 지강
function quest_rumor9()
	-- 다이얼로그 출력
	dlg_title("@90208801")

	dlg_text( "@90999620" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_6()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 
function quest_rumor_a_6()
	-- 다이얼로그 출력
	dlg_title("@90208801")

	dlg_text( "@90999624" )
	
	-- 세부대화 1-2, 소녀에 관한 신상정보
	dlg_menu( "@90999627", "quest_rumor_b_6()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 레샤에 관한 정보는 없음.
function quest_rumor_b_6()
	-- 다이얼로그 출력
	dlg_title("@90208801")

	-- 세부대화 1-3, 소녀에 관한 위치정보
	dlg_text( "@90999628" )
				
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end
