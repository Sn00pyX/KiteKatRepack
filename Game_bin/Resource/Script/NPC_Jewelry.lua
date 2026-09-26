-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_Jewelry"
end
 
   --============================================================
   --             <<<<<< 소울스톤 세공인 >>>>>>
   --============================================================

function NPC_Jewelry_contact()

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id() 
	-- 라크시 튜시너
		
	local qstart_text = get_value( "level" )
	local quest_progress1 =  get_quest_progress(2008)
	local quest_progress100 =  get_quest_progress(2000)
	
	if npc_id == 1087 then
		dlg_title("@90108701")
		dlg_text( "@90108702" )
		
		-- 미래를 예언하는 소녀
		if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
		dlg_menu( "@90999617", "quest_rumor12()" )
		end
		
		-- 광신도 암살자
		if quest_progress1 == 255 then
		dlg_menu( "@90999842", "quest_witcharmy2()" )
		end
					
	-- 카탄 마티네
	elseif npc_id == 2085 then	
		dlg_title("@90208501")
		dlg_text( "@90208502" )
		
		-- 미래를 예언하는 소녀
		if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
		dlg_menu( "@90999617", "quest_rumor13()" )
		end
		
		-- 광신도 암살자
		if quest_progress1 == 255 then
		dlg_menu( "@90999842", "quest_witcharmy1()" )
		end
		
	-- 호라이즌 게나론
	elseif npc_id == 4083 then	
		dlg_title("@90408301")
		dlg_text( "@90408302" )
	-- 론도 허드슨
	elseif npc_id == 6045 then
		dlg_title("@90604501")
		dlg_text( "@90604502" )
	-- 시크루트 나노아
	elseif npc_id == 7042 then
		dlg_title("@90704201")	
		dlg_text( "@90704202" )
	-- 마레마을 코이나
	elseif npc_id == 7043 then
		dlg_title("@90704301")	
		dlg_text( "@90704302" )
	-- 마리캣 마켓 일 때 (11131 소울스톤 세공사 쿤)
	elseif npc_id == 11131 then
		dlg_title("@90999419")	
		dlg_text( "@90999420" )
	end
	
	
	dlg_menu( "@90010167", 'question_Jewelry()' ) 
	dlg_menu( "@90010169", 'show_soulstone_craft_window()' ) 
	dlg_menu( "@90010170", 'show_soulstone_repair_window()' ) 
	
		
	dlg_menu( "@90010002", '' ) 
	dlg_show()
 
end

-- 세부대화 튜시너
function quest_rumor12()
	-- 다이얼로그 출력
	dlg_title("@90108701")

	dlg_text( "@90999619" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_9()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 소녀에 대한 이야기를 꺼림
function quest_rumor_a_9()
	-- 다이얼로그 출력
	dlg_title("@90108701")

	dlg_text( "@90999630" )
	
	dlg_menu( "@90010002", '' ) 		
	dlg_show()

end

-- 세부대화 마티네
function quest_rumor13()
	-- 다이얼로그 출력
	dlg_title("@90208501")

	dlg_text( "@90999618" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_10()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 
function quest_rumor_a_10()
	-- 다이얼로그 출력
	dlg_title("@90208501")

	dlg_text( "@90999623" )
	
	-- 세부대화 1-2, 소녀에 관한 신상정보
	dlg_menu( "@90999627", "quest_rumor_b_10()" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 레샤에 관한 정보는 없음.
function quest_rumor_b_10()
	-- 다이얼로그 출력
	dlg_title("@90208501")

	-- 세부대화 1-3, 소녀에 관한 위치정보
	dlg_text( "@90999628" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end


-- 소울스톤이란?
function question_Jewelry()

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id() 
	
	-- 라크시 튜시너
	if npc_id == 1087 then
		dlg_title("@90108701")
		dlg_text_without_quest_menu( "@90108703" )
	-- 카탄 마티네
	elseif npc_id == 2085 then	
		dlg_title("@90208501")
		dlg_text_without_quest_menu( "@90108703" )
	-- 호라이즌 게나론
	elseif npc_id == 4083 then	
		dlg_title("@90408301")
		dlg_text_without_quest_menu( "@90108703" )
	-- 론도 허드슨
	elseif npc_id == 6045 then
		dlg_title("@90604501")
		dlg_text_without_quest_menu( "@90108703" )
	-- 시크루트 나노아
	elseif npc_id == 7042 then
		dlg_title("@90704201")	
		dlg_text_without_quest_menu( "@90108703" )
	-- 마레마을 코이나
	elseif npc_id == 7043 then
		dlg_title("@90704301")	
		dlg_text_without_quest_menu( "@90108703" )
	-- 마리캣 마켓 일 때 (11131 소울스톤 세공사 쿤)
	elseif npc_id == 11131 then
		dlg_title("@90999419")	
		dlg_text_without_quest_menu( "@90108703" )
	end
	
	dlg_menu( "@90010003", 'NPC_Jewelry_contact()' ) 
	dlg_menu( "@90010002", '' ) 
	dlg_show()
end
