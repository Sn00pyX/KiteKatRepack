--- Lua 스크립트 암호화
function get_module_name()
             return "NPC_huntaholic"
end

-----------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------
------------------------------ 헌터홀릭 npc ---------------------------------------------
-----------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------


-----------------------------------------------------------------------------------------
---------------------------------		쓸개	    -------------------------------------
-----------------------------------------------------------------------------------------
function NPC_huntaholic_sseulgae_contact()

	-- 다이얼로그 출력 인사말과 함께
	dlg_title( "@90996937" )
	dlg_text( "@90996938" )

	-- 당신은 누구죠?
	dlg_menu( "@90996943", 'huntaholic_sseulgae_who_are_you()' )			
	
	-- 베어로드가 뭐죠?
	dlg_menu( "@90996944", 'huntaholic_question()' )			
	
	-- 베어로드 입장
	dlg_menu( "@90996941", 'go_to_huntaholic( 10000, 1.0 )' )			
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end



	-- 당신은 누구? 메뉴 선택시 
function huntaholic_sseulgae_who_are_you()
	-- 다이얼로그 출력
	dlg_title( "@90996937" )
	dlg_text( "@90996939" )
	--돌아가기
	dlg_menu( "@90010003", 'NPC_huntaholic_sseulgae_contact()' )	
	-- 대화종료
	dlg_menu( "@90010002", " " )
	dlg_show()
end
	

	-- 베어로드가 뭐죠? 메뉴 선택시
function huntaholic_question()
	-- 다이얼로그 출력
	-- dlg_title( "@90996937" )	-- 쓸개 NPC 이름 출력
	dlg_title( "@90996953" )	-- 웅담 NPC 이름 출력 쓸개는 이제 삭제 되었기 때문에 웅담이 베어로드에 대한 설명을 해준다.
	-- dlg_text( "@90996940" )	-- 기존의 쓸개가 대답하던 베어로드에 대한 설명 스트링 호출
	dlg_text( "@90999632" )		-- 웅담이 새롭게 베어로드에 대한 설명을 하는 스트링 호출

	--돌아가기
	-- dlg_menu( "@90010003", 'NPC_huntaholic_sseulgae_contact()' )	-- 쓸개 NPC에게 적용되었던 함수
	dlg_menu( "@90010003", 'NPC_huntaholic_woongdam_contact()' )	-- 웅담 NPC가 이제 대답하도록 수정한 함수
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end

	
	-- 베어로드 입장 메뉴 선택시
function go_to_huntaholic( huntaholic_id, point_rate )
	
	local gold = get_value( "gold" )
	
	dlg_title( "@90996937" )

	if gold >= 1000 then
		local result = warp_to_huntaholic_lobby( huntaholic_id, point_rate )
		if result == 0 then
			set_value( "gold", gold - 1000 )
			update_gold_chaos()
			save()
			return
		elseif result == 1 then
			-- 파라미터 비정상 메시지
			dlg_text( "@90996945" )
			dlg_menu( "@90010002", " " )
		elseif result == 2 then
			-- 2번 오류 로비입장 가능 시간대가 아님 메시지
			dlg_text( "@90996946" )
			dlg_menu( "@90010002", " " )
		elseif result == 3 then
			-- 플레이어를 찾을수 없을경우
			dlg_text( "@90996947" )
			dlg_menu( "@90010002", " " )
		elseif result == 4 then
			-- 파티 해산이 불가능할 경우
			dlg_text( "@90996948" )
			dlg_menu( "@90010002", " " )
		elseif result == 5 then
			-- pk on 상태일 경우
			dlg_text( "@90996949" )
			dlg_menu( "@90010002", " " )
		elseif result == 6 then
			-- 헌터홀릭 아이디가 잘못되어 있을 경우
			dlg_text( "@90996950" )
			dlg_menu( "@90010002", " " )
		elseif result == 7 then
			-- 해당 레벨의 로비가 없을 경우
			dlg_text( "@90996951" )
			dlg_menu( "@90010002", " " )
		end
	else
		--돈없다는 메시지
		dlg_text( "@90996942" )
		dlg_menu( "@90010002", " " )
	end

	-- dlg_show()
end 



-----------------------------------------------------------------------------------------
---------------------------------		웅담	    -------------------------------------
-----------------------------------------------------------------------------------------


-- 웅담(헌터홀릭 상인)과 대화시
function NPC_huntaholic_woongdam_contact()

--웅담 말시켰을 때 인사말
	dlg_title( "@90996953" )
	dlg_text( "@90996954" )

	-- 당신은 누구죠?
	dlg_menu( "@90996958", 'huntaholic_woongdam_who_are_you()' )			
	
	-- 베어로드가 뭐죠?
	dlg_menu( "@90996944", 'huntaholic_question()' )			
	
	-- 홀릭 포인트란 뭐죠?
	dlg_menu( "@90996959", 'huntaholic_market_question()' )			
	
			
	-- 포인트 상점 이용
	dlg_menu( "@90996957", 'huntaholic_market()' )			
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end
	
function huntaholic_woongdam_who_are_you()
	-- 당신은 누구죠?의 대답
	dlg_title( "@90996953" )
	-- dlg_text( "@90996955" )	-- 기존의 웅담이 쓸개와 같이 있을 때 말하던 스트링 호출
	dlg_text( "@90999633" )		-- 쓸개 NPC가 사라지고 웅담이 혼자 있다고 할 때 설명하는 스트링
	--돌아가기
	dlg_menu( "@90010003", 'NPC_huntaholic_woongdam_contact()' )
	-- 대화종료
	dlg_menu( "@90010002", " " )
	dlg_show()
end
	-- 홀릭 포인트란? 메뉴 선택시
function huntaholic_market_question()
	-- 다이얼로그 출력
	dlg_title( "@90996953" )
	dlg_text( "@90996956" )
	--돌아가기
	dlg_menu( "@90010003", 'NPC_huntaholic_woongdam_contact()' )	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end


-- L포인트 상점 이용
function huntaholic_market()

	dlg_title( "@90996953" )
	dlg_text( "@91000271" ) --L상점 대사
		
	-- L홀릭 포인트 상점
	dlg_menu( "@91000256", 'hunterholic_point_market()' )
	
	
	-- L영혼의 속삭임 주머니 상점
	dlg_menu( "@91000257", 'hunterholic_jpbox()' )
	
		
	-- 대화종료
	dlg_menu( "@90010002", " " )
	dlg_show()
end


-- L영혼의 속삭임 상점 이용
function hunterholic_jpbox()

	dlg_title( "@90996953" )
	dlg_text( "@91000263" ) --L jp로 홀릭포인트 상자 구매
		
	-- L영혼의 속삭임 주머니란? - 가격, 쿨타임 존재
	dlg_menu( "@91000258", 'hunterholic_jpbox_detail()' )
	
	
	
	-- L영혼의 속삭임 주머니 베이직 구매
	dlg_menu( "@91000260", 'hunterholic_jpbox_sell(1)' )
	
	-- L영혼의 속삭임 주머니 레어 구매
	dlg_menu( "@91000261", 'hunterholic_jpbox_sell(2)' )
	
	-- L영혼의 속삭임 주머니 유니크 구매
	dlg_menu( "@91000262", 'hunterholic_jpbox_sell(3)' )
	
	
	--돌아가기
	dlg_menu( "@90010003", 'huntaholic_market()' )	
		
	-- 대화종료
	dlg_menu( "@90010002", " " )
	dlg_show()
end

-- L영혼의 속삭임 주머니란?
function hunterholic_jpbox_detail()

	dlg_title( "@90996953" )
	dlg_text( "@91000259" ) --L 영혼의 속삭임 상자 설명
		
	-- L영혼의 속삭임 주머니란? - 가격
	--dlg_menu( "@90999006", 'hunterholic_jpbox()' )
	
	-- L영혼의 속삭임 주머니란? - 주의점, 쿨타임 존재
	--dlg_menu( "@90999006", 'hunterholic_jpbox()' )
	
	
	--돌아가기
	dlg_menu( "@90010003", 'hunterholic_jpbox()' )	
	
	-- 대화종료
	dlg_menu( "@90010002", " " )
	dlg_show()
end

-- L영혼의 속삭임 주머니 구매
function hunterholic_jpbox_sell( jpbox_grade )

	--dlg_title( "@90996953" )
	--dlg_text( "@91000263" ) --L 
	
	local jp = get_value( "jp" )
	
	if jpbox_grade == 1 then
	
		if jp < 20000 then
	
			cprint( "@91000273" ) -- <ep7><#6DD66D>jp가 부족합니다.
		
		else
			set_value( "jp", jp - 20000 )
			--jp = jp - 20000
			
			insert_item( 1100703, 1 )
			
			cprint( "@91000272" ) -- <ep7><#6DD66D>jp가 소모 되었습니다.
		end
		
	elseif jpbox_grade == 2 then
	
		if jp < 90000 then
	
			cprint( "@91000273" ) -- <ep7><#6DD66D>jp가 부족합니다.
		
		else
			set_value( "jp", jp - 90000 )
			--jp = jp - 80000
		
			insert_item( 1100702, 1 )
			
			cprint( "@91000272" ) -- <ep7><#6DD66D>jp가 소모 되었습니다.
		end
				
	elseif jpbox_grade == 3 then
		
		if jp < 200000 then
	
			cprint( "@91000273" ) -- <ep7><#6DD66D>jp가 부족합니다.
		
		else
			set_value( "jp", jp - 200000 )
			--jp = jp - 100000
			
			insert_item( 1100701, 1 )
			
			cprint( "@91000272" ) -- <ep7><#6DD66D>jp가 소모 되었습니다.
		end
		
	end
	
	--cprint( "@91000272" ) -- <ep7><#6DD66D>jp가 소모 되었습니다.
	
		
	-- 대화종료
	--dlg_menu( "@90010002", " " )
	--dlg_show()
end




-- L홀릭 포인트 상점 이용
function hunterholic_point_market()

	local state_code = get_local_info()
	
	dlg_title( "@90996953" )
	dlg_text( "@91000274" ) --L상점 대사
	
	-- L베어로드 전용 물품
	dlg_menu( "@91000264", "open_market( 'bearload_shop_onlybear' )" )
	
	-- L베어로드 꾸미기
	dlg_menu( "@91000265", "open_market( 'bearload_shop_beardeco' )" )
	
	-- L크리쳐 카드
	if state_code == 8192 then --  중동 전용
			
		dlg_menu( "@91000266", "open_market( 'bearload_shop_creturecard_me' )" )
		
	else -- 중동 제외 나머지
		
		dlg_menu( "@91000266", "open_market( 'bearload_shop_creturecard' )" )
		
	end
		
	-- L장비 아이템
	dlg_menu( "@91000267", "open_market( 'bearload_shop_equipment' )" )
	
	-- L스킬 카드
	dlg_menu( "@91000268", "open_market( 'bearload_shop_skillcard' )" )
	
	-- L소울 스톤
	dlg_menu( "@91000269", "open_market( 'bearload_shop_soulstone' )" )
	
	-- L큐브
	--dlg_menu( "@91000270", "open_market( 'bearload_shop_cube' )" )
	
	
	--돌아가기
	dlg_menu( "@90010003", 'huntaholic_market()' )	
	
	-- 대화종료
	dlg_menu( "@90010002", " " )
	dlg_show()
end

-----------------------------------------------------------------------------------------
---------------------------------		마늘	    -------------------------------------
-----------------------------------------------------------------------------------------

-- 마늘(이벤트 헌터홀릭 상인)과 대화시
function NPC_huntaholic_koreagarlic_contact()

--마늘 말시켰을 때 인사말
	dlg_title( "@90996961" )
	dlg_text( "@90996962" )
		-- 당신은 누구죠?
	dlg_menu( "@90996943", 'huntaholic_koreagarlic_who_are_you()' )		
	-- 포인트 상점 이용
	dlg_menu( "@90996964", "open_market( 'bearload_eventshop' )" )			
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end
 

	-- 당신은 누구? 메뉴 선택시 
function huntaholic_koreagarlic_who_are_you()
	-- 다이얼로그 출력
	dlg_title( "@90996961" )
	dlg_text( "@90996963" )
	--돌아가기
	dlg_menu( "@90010003", 'NPC_huntaholic_koreagarlic_contact()' )	
	-- 대화종료
	dlg_menu( "@90010002", " " )
	dlg_show()
end




