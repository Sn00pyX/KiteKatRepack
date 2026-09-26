--- Lua 스크립트 암호화
function get_module_name()
             return "NPC_Adventure_Guide"
end

-- 모험 가이드 클릭 시 호출 함수
function NPC_Adventure_Guide_contact()

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4022 모험가이드 슈린느)
		if npc_id == 4022 then
			dlg_title("@90402201")
		-- 데바 일 때 (1022 모험가이드 레미르)
		elseif npc_id == 1022 then
			dlg_title("@90102201")
		-- 아수라 일 때 (2022 모험가이드 피리아)
		elseif npc_id == 2022 then
			dlg_title("@90202201")
		-- 론도 일 때 (6022 모험가이드 마빈)
		elseif npc_id == 6022 then
			dlg_title("@90602201")
		-- 도시 유적 일 때 (7041 모험가이드 에이릴)
		elseif npc_id == 7041 then
			dlg_title("@90704101")
		-- 시크루트 일 때 (7022 모험 가이드 케인느)
		elseif npc_id == 7022 then
			dlg_title("@90702201")
			
		end -- if 끝
		
	local qstart_text = get_value( "level" )
	local quest_progress1 =  get_quest_progress(2008)
	local quest_progress100 =  get_quest_progress(2000)
	-- <대사 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4022 모험가이드 슈린느)
		if npc_id == 4022 then
			dlg_text("@90402202")
			
			-- 미래를 예언하는 소녀
			if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
			dlg_menu( "@90999617", "quest_rumor3()" )
			end
			
			-- 광신도 암살자
			if quest_progress1 == 255 then
			dlg_menu( "@90999842", "quest_witcharmy1()" )
			end
			
		-- 데바 일 때 (1022 모험가이드 레미르)
		elseif npc_id == 1022 then
			dlg_text("@90102202")
			
			-- 미래를 예언하는 소녀
			if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
			dlg_menu( "@90999617", "quest_rumor4()" )
			end
			
			-- 광신도 암살자
			if quest_progress1 == 255 then
			dlg_menu( "@90999842", "quest_witcharmy2()" )
			end
		-- 아수라 일 때 (2022 모험가이드 피리아)
		elseif npc_id == 2022 then
			dlg_text("@90202202")
			
			-- 미래를 예언하는 소녀
			if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
			dlg_menu( "@90999617", "quest_rumor5()" )
			end
						
			-- 광신도 암살자
			if quest_progress1 == 255 then
			dlg_menu( "@90999842", "quest_witcharmy1()" )
			end
			
		-- 론도 일 때 (6022 모험가이드 마빈)
		elseif npc_id == 6022 then
			dlg_text("@90602202")

--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
		dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]
		-- 도시 유적 일 때 (7041 모험가이드 에이릴)
		elseif npc_id == 7041 then
			dlg_text("@90704102")		
		-- 시크루트 일 때 (7022 모험 가이드 케인느)
		elseif npc_id == 7022 then
			if is_premium() then
				dlg_text("@90702202")
				
--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
	dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]	
			else
				dlg_text( "@90700118" )
			end
		end	

		
	-- 시크루트 NPC이고 프리미엄 서비스이고, 캐쉬 서버 일 때만... (프리미엄 체크시 한국은 테섭도 포함됨)
	if npc_id == 7022 and is_premium() == true and get_env("game.cash_usable_server") == 1 then	
			dlg_menu( "@90704103", "open_market( 'official_riding' )" )
	else
			dlg_menu( "@90704103", "open_market( 'normal_riding' )" )
	end	
			

	dlg_menu( "@90010002", '' )
	dlg_show()

end	

-- 세부대화 슈린느
function quest_rumor3()
	-- 다이얼로그 출력
	dlg_title("@90402201")

	dlg_text( "@90999619" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end


	
-- 세부대화 레미르
function quest_rumor4()
	-- 다이얼로그 출력
	dlg_title("@90102201")

	dlg_text( "@90999618" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_1()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화
function quest_rumor_a_1()
	-- 다이얼로그 출력
	dlg_title("@90102201")

	dlg_text( "@90999622" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999625", "quest_rumor_b_1()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화
function quest_rumor_b_1()
	-- 다이얼로그 출력
	dlg_title("@90102201")

	dlg_text( "@90999626" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999627", "quest_rumor_c_1()" )
		
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 결국 레샤에 관한 정보는 없음.
function quest_rumor_c_1()
	-- 다이얼로그 출력
	dlg_title("@90102201")

	dlg_text( "@90999628" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end
				
-- 세부대화 피리아
function quest_rumor5()
	-- 다이얼로그 출력
	dlg_title("@90202201")

	dlg_text( "@90999618" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_2()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화
function quest_rumor_a_2()
	-- 다이얼로그 출력
	dlg_title("@90202201")

	dlg_text( "@90999622" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999625", "quest_rumor_b_2()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화
function quest_rumor_b_2()
	-- 다이얼로그 출력
	dlg_title("@90202201")

	dlg_text( "@90999626" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999627", "quest_rumor_c_2()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 결국 레샤에 관한 정보는 없음.
function quest_rumor_c_2()
	-- 다이얼로그 출력
	dlg_title("@90202201")

	dlg_text( "@90999628" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 광신도 암살자_슈린느 & 피리아
function quest_witcharmy1()

	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4022 모험가이드 슈린느)
		if npc_id == 4022 then
			dlg_title("@90402201")
		-- 데바 일 때 (1022 모험가이드 레미르)
		elseif npc_id == 1022 then
			dlg_title("@90102201")
		-- 아수라 일 때 (2022 모험가이드 피리아)
		elseif npc_id == 2022 then
			dlg_title("@90202201")
		end	
	
	dlg_text( "@90999843" )
	dlg_menu( "@90999845", "quest_witcharmy1_1()" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

function quest_witcharmy1_1()

	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4022 모험가이드 슈린느)
		if npc_id == 4022 then
			dlg_title("@90402201")
		-- 데바 일 때 (1022 모험가이드 레미르)
		elseif npc_id == 1022 then
			dlg_title("@90102201")
		-- 아수라 일 때 (2022 모험가이드 피리아)
		elseif npc_id == 2022 then
			dlg_title("@90202201")
		end	
	
	dlg_text( "@90999847" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 광신도 암살자_레미르
function quest_witcharmy2()
	-- 다이얼로그 출력
	dlg_title("@90402201")

	dlg_text( "@90999844" )
	dlg_menu( "@90999845", "quest_witcharmy2_1()" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 광신도 암살자_레미르
function quest_witcharmy2_1()
	-- 다이얼로그 출력
	dlg_title("@90402201")

	dlg_text( "@90999848" )
	--dlg_menu( "@90999845", "quest_witcharmy2_1()" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end



--============================================================================
--======================	잃어버린 섬 확장 NPC		======================
--============================================================================


-- <ep7>사냥꾼 <ep7>트로프
function NPC_lost_island_trof_contact()

	-- 다이얼로그 출력
	dlg_title("@91000401")

	dlg_text( "@91000402" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>사냥꾼 나키텍
function NPC_lost_island_nakitec_contact()

	-- 다이얼로그 출력
	dlg_title("@91000404")

	dlg_text( "@91000405" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>사냥꾼 케텍
function NPC_lost_island_ketehc_contact()

	-- 다이얼로그 출력
	dlg_title("@91000407")

	dlg_text( "@91000408" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>신관 리아프
function NPC_lost_island_riaf_contact()

	-- 다이얼로그 출력
	dlg_title("@91000410")

	dlg_text( "@91000411" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>가이드 라브
function NPC_lost_island_liav_contact()

	-- 다이얼로그 출력
	dlg_title("@91000413")

	dlg_text( "@91000414" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>사냥꾼 노키로프
function NPC_lost_island_nokurof_contact()

	-- 다이얼로그 출력
	dlg_title("@91000416")

	dlg_text( "@91000417" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>몬스터 연구원 시카
function NPC_lost_island_sika_contact()

	-- 다이얼로그 출력
	dlg_title("@91000419")

	dlg_text( "@91000420" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>호위대원 카이독
function NPC_lost_island_kaydoc_contact()

	-- 다이얼로그 출력
	dlg_title("@91000422")

	dlg_text( "@91000423" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>연구원 제드아
function NPC_lost_island_jedoa_contact()

	-- 다이얼로그 출력
	dlg_title("@91000425")

	dlg_text( "@91000426" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>연구원 시트카티스
function NPC_lost_island_sitkatis_contact()

	-- 다이얼로그 출력
	dlg_title("@91000428")

	dlg_text( "@91000429" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>병사 베넹
function NPC_lost_island_bethel_contact()

	-- 다이얼로그 출력
	dlg_title("@91000431")

	dlg_text( "@91000432" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>병사 라스카
function NPC_lost_island_laska_contact()

	-- 다이얼로그 출력
	dlg_title("@91000434")

	dlg_text( "@91000435" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>병사 콜바이
function NPC_lost_island_colbai_contact()

	-- 다이얼로그 출력
	dlg_title("@91000437")

	dlg_text( "@91000438" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>지휘관 알루스니엡
function NPC_lost_island_alusniab_contact()

	-- 다이얼로그 출력
	dlg_title("@91000440")

	dlg_text( "@91000441" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>설원 마르두카족 나키흐
function NPC_lost_island_nakich_contact()

	-- 다이얼로그 출력
	dlg_title("@91000443")

	dlg_text( "@91000444" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>수수께끼 노인 누즈
function NPC_lost_island_rooze_contact()

	-- 다이얼로그 출력
	dlg_title("@91000446")

	dlg_text( "@91000447" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end


-- <ep7>사냥꾼 레텝
function NPC_lost_island_lateb_contact()

	-- 다이얼로그 출력
	dlg_title("@91000449")

	dlg_text( "@91000450" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>사냥꾼 페니슐라
function NPC_lost_island_penisulla_contact()

	-- 다이얼로그 출력
	dlg_title("@91000452")

	dlg_text( "@91000453" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>설원 탐사대 킨지크
function NPC_lost_island_kinzich_contact()

	-- 다이얼로그 출력
	dlg_title("@91000455")

	dlg_text( "@91000456" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>설원 탐사대 코디아
function NPC_lost_island_kodia_contact()

	-- 다이얼로그 출력
	dlg_title("@91000458")

	dlg_text( "@91000459" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>설원 탐사대 케치칸
function NPC_lost_island_kechikan_contact()

	-- 다이얼로그 출력
	dlg_title("@91000461")

	dlg_text( "@91000462" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end



-- <ep7>설원 눈털보 포요콘
function NPC_lost_island_fortyukon_contact()

	-- 다이얼로그 출력
	dlg_title("@91000556")

	dlg_text( "@91000557" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- <ep7>설원 금빛요정 키느
function NPC_lost_island_kinh_contact()

	-- 다이얼로그 출력
	dlg_title("@91000565")

	dlg_text( "@91000566" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end


-- <ep7> 설원분지 출구<잃어버린섬 입구행 워프게이트>
function quest_prop_LostIsland_basin_outgate()

	-- 드래곤의 둥지로 워프
	warp( 83792 + math.random(0,10) , 116145+ math.random(0,10), gv("layer") )
	
	
	-- 드래곤의 둥지로 warp( 105168 + math.random(0,10) , 135924+ math.random(0,10), gv("layer") )
end


-- <ep7> 드래곤의 둥지 출구<잃어버린섬 설원분지행 워프게이트>
function quest_prop_LostIsland_Dnest_outgate()
	
	-- 설원분지로 워프
	warp( 89080 + math.random(0,10) , 121407+ math.random(0,10), gv("layer") )

end
