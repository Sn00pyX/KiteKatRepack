--- Lua 스크립트 암호화
function get_module_name()
             return "NPC_QuestClient"
end

-- 퀘스트 의뢰인 클릭 시 호출 함수
function NPC_QuestClient_contact()

		
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4024 퀘스트 의뢰인 유완)
		if npc_id == 4024 then
			dlg_title("@90402401")
		-- 데바 일 때 (1024 퀘스트 의뢰인 로하스)
		elseif npc_id == 1024 then
			dlg_title("@90102401")
		-- 아수라 일 때 (2024 퀘스트 의뢰인 잉그마르)
		elseif npc_id == 2024 then
			dlg_title("@90202401")
		-- 시크루트 일 때 (7024 퀘스트 의뢰인 사아)
		elseif npc_id == 7024 then
			dlg_title("@90702401")
		end -- if 끝
		
	-- <대사 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4024 퀘스트 의뢰인 유완)
		if npc_id == 4024 then
			dlg_text("@90402402")
		-- 데바 일 때 (1024 퀘스트 의뢰인 로하스)
		elseif npc_id == 1024 then
			dlg_text("@90102402")
		-- 아수라 일 때 (2024 퀘스트 의뢰인 잉그마르)
		elseif npc_id == 2024 then
			dlg_text("@90202402")
		-- 시크루트 일 때 (7024 퀘스트 의뢰인 사아)
		elseif npc_id == 7024 then
			if is_premium() then
				dlg_text("@90702402")
			else
				dlg_text_without_quest_menu( "@90700118" )
			end
		end -- if 끝

	-- <공통 메뉴>
	-- 대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()
	
end-- 펑션 끝

-- 랜덤 퀘스트 수락시 나오는 대사
function NPC_Random_Quest_Start()

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()


	-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4024 퀘스트 의뢰인 유완)
		if npc_id == 4024 then
			dlg_title("@90402401")
		-- 데바 일 때 (1024 퀘스트 의뢰인 로하스)
		elseif npc_id == 1024 then
			dlg_title("@90102401")
		-- 아수라 일 때 (2024 퀘스트 의뢰인 잉그마르)
		elseif npc_id == 2024 then
			dlg_title("@90202401")
		-- 시크루트 일 때 (7024 퀘스트 의뢰인 사아)
		elseif npc_id == 7024 then
			dlg_title("@90702401")
		end -- if 끝
		
	-- <수락링크 출력> 내용 변환을 위해 랜덤퀘스트 수락링크를 불러오는 방법 : quest_text_without_quest_menu( quest_id, quest_text_id )
	-- <마지막으로 수락한 퀘스트 ID 체크> 레벨에 따라 퀘스트 ID가 분류된 것을 체크하는 함수 : code = get_last_accept_quest()
	local code = get_last_accept_quest();
	
		-- 가이아 일 때 (4024 퀘스트 의뢰인 유완)
		if npc_id == 4024 then
			quest_text_without_quest_menu( code, 90402406 )
		-- 데바 일 때 (1024 퀘스트 의뢰인 로하스)
		elseif npc_id == 1024 then
			quest_text_without_quest_menu( code, 90102406 )
		-- 아수라 일 때 (2024 퀘스트 의뢰인 잉그마르)
		elseif npc_id == 2024 then
			quest_text_without_quest_menu( code, 90202406 )
		-- 시크루트 일 때 (7024 퀘스트 의뢰인 사아)
		elseif npc_id == 7024 then
			if is_premium() then
				quest_text_without_quest_menu( code, 90702406 )
			else
				dlg_text_without_quest_menu( "@90700118" )
			end
		end -- if 끝

	-- <공통 메뉴>
	-- 대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()
	
end-- 펑션 끝


-- NPC_Quest_Lakcity_Highpriest  	라크시 고위 신관 라르갈
-- NPC_Quest_Fairywood_Forestranger 요정의 숲 숲지기 에란
-- NPC_Quest_Katan_Councilmessenger 카탄 장로회 전령 젠
-- NPC_Quest_Cremationground_Shaman 화장터 무녀 제이나
-- NPC_Quest_Horizon_Questmerchant  호라이즌 교역상 엘민



-- NPC_Quest_Templarheadquarter_Templar	템플러 오유
-- NPC_Quest_Moonfirecemetery_Fighter	투사 류엔
-- NPC_Quest_Desert_Adventurer			일급 모험가 라우글
-- NPC_Quest_Desert_Inquirer			조사대 미오
-- NPC_Merchant_Etc_Desert			 	잡화상인 세이시아

-- NPC_Quest_Horizon_Hunter 호라이즌 사냥꾼 켄


   --============================================================
   --===============		라크시 고위 신관 라르갈
   --============================================================

function NPC_Quest_Lakcity_Highpriest_init()
	cprint( "!고위 신관 라르갈 가동" )
	set_npc_name(  "@90108100"  )
end



function NPC_Quest_Lakcity_Highpriest_contact()

	-- 다이얼로그 출력
	dlg_title( "@90108101" )
	dlg_text( "@90108102" )
    
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function Quest_Link_109_1()
	local count = find_item( 1100303 )
	if count == 0 then
	insert_item( 1100303, 1 )
	else
	end
end

function Quest_Link_111_1()
	local count = find_item( 1100304 )
	if count == 0 then
	insert_item( 1100304, 1 )
	else
	end
end


   --============================================================
   --===============		요정의 숲 숲지기 에란
   --============================================================

function NPC_Quest_Fairywood_Forestranger_init()
	cprint( "!숲지기 에란 가동" )
	set_npc_name(  "@90108200"  )
end



function NPC_Quest_Fairywood_Forestranger_contact()

	-- 다이얼로그 출력
	dlg_title( "@90108201" )
	dlg_text( "@90108202" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end



   --============================================================
   --===============	 카탄 장로회 전령 젠
   --============================================================

function NPC_Quest_Katan_Councilmessenger_init()
	cprint( "!장로회 전령 젠 가동" )
	set_npc_name(  "@90208100"  )
end



function NPC_Quest_Katan_Councilmessenger_contact()

	-- 다이얼로그 출력
	dlg_title( "@90208101" )
	dlg_text( "@90208102" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


   --============================================================
   --===============		화장터 무녀 제이나
   --============================================================

function NPC_Quest_Cremationground_Shaman_init()
	cprint( "!무녀 제이나 가동" )
	set_npc_name(  "@90208200"  )
end

function NPC_Quest_Cremationground_Shaman_contact()

	-- 다이얼로그 출력
	dlg_title( "@90208201" )

	
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress100 =  get_quest_progress(1081)
	
	local quest_progress3351 =  get_quest_progress(3351) -- <(version:7.3)>강철의 왕
	
	local quest_progress3613 =  get_quest_progress(3613) -- <(version:8.1)>긴급호출
	
		
	-- 퀘스트 종료시 대사	
	if quest_progress100 == 255 then
		dlg_text( "@90208213" )	
		-- 퀘스트 수행 전, 수행 중 대사
	else 
		dlg_text( "@90208202" )
	end
	
	if quest_progress3613 == 2 then
		dlg_text( "@90605478" )	
		-- 퀘스트 수행 전, 수행 중 대사
	else 
		dlg_text( "@90208202" )
	end
	
	if quest_progress3351 == 1 then
		dlg_menu( "@90604779", "open_market( 'tp_quest' )" ) 
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end


function Quest_Link_107_1()
	local count = find_item( 1000032 )
	if count == 0 then
	insert_item( 1000032, 1 )
	else
	end
end






   --============================================================
   --===============		호라이즌 교역상 엘민
   --============================================================

function NPC_Quest_Horizon_Questmerchant_init()
	cprint( "!교역상 엘민 가동" )
	set_npc_name(  "@90408100"  )
end



function NPC_Quest_Horizon_Questmerchant_contact()

	-- 다이얼로그 출력
	dlg_title( "@90408101" )
	dlg_text( "@90408102" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end









   --============================================================
   --===============		템플러 헤드쿼터 템플러 오유
   --============================================================

function NPC_Quest_Templarheadquarter_Templar_init()
	cprint( "!템플러 오유 가동" )
	set_npc_name(  "@90108300"  )
end



function NPC_Quest_Templarheadquarter_Templar_contact()

	-- 다이얼로그 출력
	dlg_title( "@90108301" )
	dlg_text( "@90108302" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


   --============================================================
   --===============		월화의 공동묘지 투사 류엔
   --============================================================

function NPC_Quest_Moonfirecemetery_Fighter_init()
	cprint( "!투사 류엔 가동" )
	set_npc_name(  "@90408200"  )
end



function NPC_Quest_Moonfirecemetery_Fighter_contact()

	-- 다이얼로그 출력
	dlg_title( "@90408201" )
	

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress101 =  get_quest_progress(1084)

	-- 퀘스트 종료시 대사	
	if quest_progress101 == 255 then
		dlg_text( "@90408212" )	
	
	-- 퀘스트 수행 전, 수행 중 대사
	else dlg_text( "@90408202" )
	
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function Quest_Link_106_1()
	local count = find_item( 1000028 )
	if count == 0 then
	insert_item( 1000028, 1 )
	else
	end
end


function Quest_Link_108_1()
	local count = find_item( 1000029 )
	if count == 0 then
	insert_item( 1000029, 1 )
	else
	end
end






   --============================================================
   --===============		세리우 사막 일급 모험가 라우글
   --============================================================

function NPC_Quest_Desert_Adventurer_init()
	cprint( "!일급 모험가 라우글 가동" )
	set_npc_name(  "@90508100"  )
end


function NPC_Quest_Desert_Adventurer_contact()

	-- 다이얼로그 출력
	dlg_title( "@90508101" )
	
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress103 =  get_quest_progress(1086)

	-- 퀘스트 종료시 대사	
	if quest_progress103 == 255 then
		dlg_text( "@90508109" )	
	
	-- 퀘스트 수행 전, 수행 중 대사
	else dlg_text( "@90508102" )
	
	end

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
	
	
end


   --============================================================
   --===============		세리우 사막 조사대 미오
   --============================================================

function NPC_Quest_Desert_Inquirer_init()
	cprint( "!조사대 미오 가동" )
	set_npc_name(  "@90508200"  )
end



function NPC_Quest_Desert_Inquirer_contact()

	-- 다이얼로그 출력
	dlg_title( "@90508201" )
	dlg_text( "@90508202" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end





   --============================================================
   --             <<<<<< 세리우 사막 잡화상인 세이시아 >>>>>>
   --============================================================
function NPC_Merchant_Etc_Desert_init()
	cprint( "!잡화상인 세이시아 가동" )
	set_npc_name(  "@90508300"  )
end
 

function NPC_Merchant_Etc_Desert_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90508301" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress102 =  get_quest_progress(1091)

	-- 퀘스트 종료시 대사	
	if quest_progress102 == 255 then
		dlg_text( "@90508307" )	
			
	-- 퀘스트 수행 전, 수행 중 대사
	else dlg_text( "@90508302" )
			
	end
	
	-- 잡화 메뉴 및 대화 종료	
	dlg_menu( "@90508306", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
	dlg_show()	
 
end






-- NPC_Quest_Stump_Woodcutter			벌목장 벌목꾼 크라우
-- NPC_Quest_Lizardhabitat_Monsterresearcher	리자드맨 서식지 몬스터 연구원 네드락
-- NPC_Quest_Ivorytower_Guard		 	상아탑지기 베르나르

-- NPC_Quest_Woodworker					목수 사이퍼드
-- NPC_Quest_Assassinguild_Assassin		어쎄신 라밀

-- NPC_Quest_crystalmountain_madman		미치광이 해럴드


   --============================================================
   --===============		그루터기 벌목장 벌목꾼 크라우
   --============================================================

function NPC_Quest_Stump_Woodcutter_init()
	cprint( "!벌목꾼 크라우 가동" )
	set_npc_name(  "@90108400"  )
end



function NPC_Quest_Stump_Woodcutter_contact()

	-- 다이얼로그 출력
	dlg_title( "@90108401" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress108 =  get_quest_progress(1101)

	-- 퀘스트 종료시 대사	
	if quest_progress108 == 255 then
		dlg_text( "@90108409" )	
			
	-- 퀘스트 수행 전, 수행 중 대사
	else dlg_text( "@90108402" )

	end			

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 end


 
--============================================================
   --===============		리자드맨 서식지 몬스터 연구원 네드락
   --============================================================

function NPC_Quest_Lizardhabitat_Monsterresearcher_init()
	cprint( "!몬스터 연구원 네드락 가동" )
	set_npc_name(  "@90108500"  )
end


function NPC_Quest_Lizardhabitat_Monsterresearcher_contact()

	-- 다이얼로그 출력
	dlg_title( "@90108501" )
	
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress104 =  get_quest_progress(1108)

	-- 퀘스트 종료시 대사	
	if quest_progress104 == 255 then
		dlg_text( "@90108515" )	
	
	-- 퀘스트 수행 전, 수행 중 대사
	else dlg_text( "@90108502" )
	
	end

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
	
	
end


--============================================================
   --===============		상아탑지기 베르나르
   --============================================================

function NPC_Quest_Ivorytower_Guard_init()
	cprint( "!상아탑지기 베르나르 가동" )
	set_npc_name(  "@90108600"  )
end



function NPC_Quest_Ivorytower_Guard_contact()

	-- 다이얼로그 출력
	dlg_title( "@90108601" )
	dlg_text( "@90108602" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function Quest_Link_104_1()
	local count = find_item( 1000025 )
	if count == 0 then
	insert_item( 1000025, 1 )
	else
	end
end







--============================================================
   --===============		목수 사이퍼드
   --============================================================

function NPC_Quest_Woodworker_init()
	cprint( "!목수 사이퍼드 가동" )
	set_npc_name(  "@90208300"  )
end



function NPC_Quest_Woodworker_contact()

	-- 다이얼로그 출력
	dlg_title( "@90208301" )
	dlg_text( "@90208302" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


--============================================================
   --===============		어쎄신 길드 어쎄신 라밀
   --============================================================

function NPC_Quest_Assassinguild_Assassin_init()
	cprint( "!어쎄신 라밀 가동" )
	set_npc_name(  "@90208400"  )
end



function NPC_Quest_Assassinguild_Assassin_contact()

	-- 다이얼로그 출력
	dlg_title( "@90208401" )
	dlg_text( "@90208402" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end




function Quest_Link_101_1()
	local count = find_item( 1000031 )
	if count == 0 then
	insert_item( 1000031, 1 )
	else
	end
end







   --============================================================
   --===============		수정의 산 미치광이 해럴드
   --============================================================

function NPC_Quest_crystalmountain_madman_init()
	cprint( "!미치광이 해럴드 가동" )
	set_npc_name(  "@90508400"  )
end



function NPC_Quest_crystalmountain_madman_contact()


	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress000 =  get_quest_progress(1225)
	-- 퀘스트 종료시 대사	
	if quest_progress000 == 255 then
		dlg_title( "@90508401" )
		dlg_text( "@90508418" )
	-- 퀘스트 수행 전, 수행 중 대사
	else 
		-- 다이얼로그 출력
		dlg_title( "@90508401" )
		dlg_text( "@90508402" )
	end
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end









---------------------------------
--- quest_progress108까지 사용---
---------------------------------




-- NPC_Quest_Horizon_Hunter 호라이즌 사냥꾼 켄




   --============================================================
   --===============		호라이즌 사냥꾼 켄
   --============================================================

function NPC_Quest_Horizon_Hunter_init()
	cprint( "!사냥꾼 켄 가동" )
	set_npc_name(  "@90508500"  )
end



function NPC_Quest_Horizon_Hunter_contact()

	-- 다이얼로그 출력
	dlg_title( "@90508501" )
	dlg_text( "@90508502" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


	
	--============================================================
	--===============		월드컵 이벤트 NPC
	--============================================================


		--COUNT01 = 파
		--COUNT02 = 이
		--COUNT03 = 팅
		   
		--COUNT04 = 태
		--COUNT05 = 극
		--COUNT06 = 전
		--COUNT07 = 사
		   
		--COUNT08 = 필
		--COUNT09 = 승
		--COUNT10 = 코
		--COUNT11 = 리
		--COUNT12 = 아
   
   
   
   
-- 월드컵 이벤트 NPC 클릭 시 호출 함수
function NPC_event_contact()

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	
	-- <이름 출력> 각 NPC에 따라 적합한 
	
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_title("@90750101")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_title("@90750201")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_title("@90750301")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_title("@90750401")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_title("@90750501")	
	
	end
	
	-- <대사 출력> 각 NPC에 따라 적합한 
		
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_text("@90750102")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_text("@90750202")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_text("@90750302")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_text("@90750402")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_text("@90750502")
	
	end		
	
	-- <공통 메뉴>
		-- 월드컵 이벤트에 관해 설명해준다
	dlg_menu( "@90010109", 'worldcupEvent_explanation()' )		
	-- 카드를 보상 아이템으로 교환한다.
	dlg_menu( "@90010110", 'worldcupEvent_card_to_item()' )
		--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()
	
end


-- 월드컵 이벤트에 관해 설명한다

function worldcupEvent_explanation()
	-- 이벤트 설명
	dlg_text("@90750103")
	--돌아가기
	dlg_menu( "@90010003", 'NPC_event_contact()' )	
	--대화 종료
	dlg_menu( "@90010002", "" )
	-- 다이얼로그 출력하기
	dlg_show()

	
	
	
		
end


-- 카드를 아이템으로 교환한다.
function worldcupEvent_card_to_item()

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
		
	-- <이름 출력> 각 NPC에 따라 적합한 
	
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_title("@90750101")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_title("@90750201")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_title("@90750301")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_title("@90750401")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_title("@90750501")	
	
	end
		
	-- 문자 카운트	
	local count = {}
	local item_id = 2000401
	-- 맞춘 문자 셋
	local event_set = 0
	
	for i = 1, 12 do

		count[i] = find_item( item_id )
		
		-- 카드가 없으면 현재 몇개째 인지 체크 하고 나가리.
		if count[i] == 0 then
			if i < 3 then
				event_set = 0
			
			elseif count[3] ~=0 and i < 7 then
				event_set = 1
								
			end
		
			-- 밖으로 빠져나간다.
			break
		end
	
		-- 3개째 맞게 되면(안 맞으면 알아서 빠져나가니까.) 세트 1 완성
		if count[3] ~=0 and i == 3 then
			event_set = 1
		end
		-- 7개째 맞게 되면 세트 2완성
		if count[7] ~=0 and i == 7 then
			event_set = 2
		end
		-- 12개째 맞게 되면 세트 3 완성.
		if count[12] ~=0 and i == 12 then
			event_set = 3
		end
		
		item_id = item_id + 1
	end
	
	-- <대사 출력> 각 NPC에 따라 적합한 
	if event_set == 0 then
		-- 이벤트 도우미 코리앙 일때
		if npc_id == 7501 then
			dlg_text("@90750110")
		-- 이벤트 도우미 스위즈 일때
		elseif npc_id == 7502 then	
			dlg_text("@90750210")
		-- 이벤트 도우미 프란스 일때
		elseif npc_id == 7503 then	
			dlg_text("@90750310")
		-- 이벤트 도우미 토구 일때
		elseif npc_id == 7504 then
			dlg_text("@90750410")
		-- 이벤트 도우미 사카 일때
		elseif npc_id == 7505 then
			dlg_text("@90750510")
		end						
	
	else	
		-- 이벤트 도우미 코리앙 일때
		if npc_id == 7501 then
			dlg_text("@90750104")
		-- 이벤트 도우미 스위즈 일때
		elseif npc_id == 7502 then	
			dlg_text("@90750204")
		-- 이벤트 도우미 프란스 일때
		elseif npc_id == 7503 then	
			dlg_text("@90750304")
		-- 이벤트 도우미 토구 일때
		elseif npc_id == 7504 then
			dlg_text("@90750404")
		-- 이벤트 도우미 사카 일때
		elseif npc_id == 7505 then
			dlg_text("@90750504")
	
		end		
	end

	-- 세트가 완성되어 있다면...
	if event_set >= 1 then
		dlg_menu( "파이팅 셋트", 'WorldcupEvent_question_1()' )
	end
	if event_set >= 2 then
		dlg_menu( "태극전사 셋트", 'WorldcupEvent_question_2()' )
	end
	if event_set >= 3 then
		dlg_menu( "필승코리아 셋트", 'WorldcupEvent_question_3()' )
	end
			
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()	

end

-- 카드 교환 최종 확인(파이팅 셋트)
function WorldcupEvent_question_1()
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
	
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_title("@90750101")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_title("@90750201")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_title("@90750301")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_title("@90750401")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_title("@90750501")	
	
	end
	
	-- <대사 출력> 각 NPC에 따라 적합한 
		
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_text("@90750105")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_text("@90750205")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_text("@90750305")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_text("@90750405")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_text("@90750505")
	
	end		
		--확인 버튼
		dlg_menu( "확인", 'WorldcupEvent_Item_change(1)' )
		-- 취소 버튼
		dlg_menu( "취소", 'worldcupEvent_card_to_item()' )
		-- 다이얼로그 출력하기
		dlg_show()			
end

-- 카드 교환 최종 확인(태극전사 셋트)
function WorldcupEvent_question_2()
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
	
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_title("@90750101")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_title("@90750201")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_title("@90750301")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_title("@90750401")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_title("@90750501")	
	
	end
	
	-- <대사 출력> 각 NPC에 따라 적합한 
		
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_text("@90750106")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_text("@90750206")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_text("@90750306")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_text("@90750406")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_text("@90750506")
	
	end		
		--확인 버튼
		dlg_menu( "확인", 'WorldcupEvent_Item_change(2)' )
		-- 취소 버튼
		dlg_menu( "취소", 'worldcupEvent_card_to_item()' )
		-- 다이얼로그 출력하기
		dlg_show()			
end


-- 카드 교환 최종 확인(필승코리아 셋트)
function WorldcupEvent_question_3()
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
	
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_title("@90750101")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_title("@90750201")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_title("@90750301")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_title("@90750401")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_title("@90750501")	
	
	end
	
	-- <대사 출력> 각 NPC에 따라 적합한 
		
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_text("@90750107")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_text("@90750207")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_text("@90750307")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_text("@90750407")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_text("@90750507")
	
	end		
		--확인 버튼
		dlg_menu( "확인", 'WorldcupEvent_Item_change(3)' )
		-- 취소 버튼
		dlg_menu( "취소", 'worldcupEvent_card_to_item()' )
		-- 다이얼로그 출력하기
		dlg_show()			
end


		
-- 아이템 지급
function WorldcupEvent_Item_change( event_set )
	

	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
	
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_title("@90750101")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_title("@90750201")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_title("@90750301")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_title("@90750401")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_title("@90750501")	
	
	end
	
	-- <대사 출력> 각 NPC에 따라 적합한 
		
	-- 이벤트 도우미 코리앙 일때
	if npc_id == 7501 then
		dlg_text("@90750109")
	-- 이벤트 도우미 스위즈 일때
	elseif npc_id == 7502 then	
		dlg_text("@90750209")
	-- 이벤트 도우미 프란스 일때
	elseif npc_id == 7503 then	
		dlg_text("@90750309")
	-- 이벤트 도우미 토구 일때
	elseif npc_id == 7504 then
		dlg_text("@90750409")
	-- 이벤트 도우미 사카 일때
	elseif npc_id == 7505 then
		dlg_text("@90750509")
	
	end		
	
	-- 만약을 위해서 다시 한번 체크
	local item_id = 2000401
	local count = {}
	
	local temp_event_set = 0
	
	for i = 1, 12 do

		count[i] = find_item( item_id )
		
		-- 카드가 없으면 현재 몇개째 인지 체크 하고 나가리.
		if count[i] == 0 then
			if i < 3 then
				temp_event_set = 0
			elseif count[3] ~=0 and i < 7 then
				temp_event_set = 1
			end
			-- 밖으로 빠져나간다.
			break
		end
		
		-- 3개째 맞게 되면(안 맞으면 알아서 빠져나가니까.) 세트 1 완성
		if count[3] ~=0 and i == 3 then
			temp_event_set = 1
		end
		-- 7개째 맞게 되면 세트 2완성
		if count[7] ~=0 and i == 7 then
			temp_event_set = 2
		end
		-- 12개째 맞게 되면 세트 3 완성.
		if count[12] ~=0 and i == 12 then
			temp_event_set = 3
		end

		item_id = item_id + 1
		
	end

	local flag = 1
	-- 꽁수 부리기 퇴치 (현재 완성된 카드셋의 종류보다 보상으로 고른 것이 더 클때, 즉 보상을 큰 것을 고르고 카드를 빼돌린 상태)
	if event_set > temp_event_set then
	
		flag = 0
		-- 이벤트 도우미 코리앙 일때
		if npc_id == 7501 then
			dlg_text("@90750110")
		-- 이벤트 도우미 스위즈 일때
		elseif npc_id == 7502 then	
			dlg_text("@90750210")
		-- 이벤트 도우미 프란스 일때
		elseif npc_id == 7503 then	
			dlg_text("@90750310")
		-- 이벤트 도우미 토구 일때
		elseif npc_id == 7504 then
			dlg_text("@90750410")
		-- 이벤트 도우미 사카 일때
		elseif npc_id == 7505 then
			dlg_text("@90750510")
		end						

	end
	
	if flag == 1 then	
		-- 지급할 아이템 세팅
		local item_set = {}
		local item_set_percentage = {}
		
		if event_set == 1 then
			item_set = { 700102, 700202, 602302, 602305, 602402, 2900500, 2000204, 700401 }
			item_set_percentage = { 4, 20, 35, 50, 65, 70, 95, 100 }
	
		elseif event_set == 2 then
			item_set = { 700401, 700103, 700203, 2901000, 2000205, 900001, 602303, 602403 }
			item_set_percentage = { 15, 17, 25, 35, 40, 50, 75, 100 }
		
		elseif event_set == 3 then
			item_set = { 540002, 540005, 540006, 540011, 540018, 540019, 900000, 2010201 }
			item_set_percentage = { 7, 14, 21, 28, 35, 42, 90, 100 }
		end
	
	
			
		local event_item = math.random( 100 )
		local count_item = 0
		-- 아이템 ID 초기화
		item_id = 0
	
		-- 아이템 지급
		if temp_event_set > 0 then
		
			if event_item <= item_set_percentage[1] then
				item_id = item_set[1]
			elseif event_item <= item_set_percentage[2] then
				item_id = item_set[2]
			elseif event_item <= item_set_percentage[3] then
				item_id = item_set[3]
			elseif event_item <= item_set_percentage[4] then
				item_id = item_set[4]
			elseif event_item <= item_set_percentage[5] then
				item_id = item_set[5]
			elseif event_item <= item_set_percentage[6] then
				item_id = item_set[6]
			elseif event_item <= item_set_percentage[7] then
				item_id = item_set[7]
			elseif event_item <= item_set_percentage[8] then
				item_id = item_set[8]
			end
			
			insert_item( item_id, 1)
			-- 시스템 메시지 출력
			local text = sconv("@941", "#@item_name@#", "@" .. tostring( item_id+10000000 ) )
			message( text )
		
			-- 파이팅 대한민국 필승코리아 삭제.
			item_id = 2000401
			local count_item = 3
			
			if event_set == 1 then
				count_item = 3
			elseif event_set == 2 then
				count_item = 7
			elseif event_set == 3 then
				count_item = 12
			end
		
			-- 주르륵 삭제.
			for i = 1, count_item do
			
				if count_item < 0 or count_item > 12 then
					break
				end
				delete_item( get_item_handle(item_id), 1)
				item_id = item_id + 1
			end
	
		end
	end
			
	--돌아가기
	dlg_menu( "@90010003", 'NPC_event_contact()' )	
	
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()
	
end







   --============================================================
   --===============		던전 관리인 미이나
   --============================================================
   -- 일반 플레이어
   -- 대화 - 던전 정보 보기 - 소유 길드란?
   --                       - 세율이란?
   -- 
   -------------------------------------------------------------- 
   -- 길드 마스터
   -- 대화 - 던전 정보 보기 - 소유 길드란?
   --      !                - 세율이란?
   --      !
   --      - 관리 메뉴 - 세금 수금(세금 + 라크)
   --                  - 세율 조정
   --------------------------------------------------------------  

--	소유 길드 이름을 가져 옴: get_own_guild_name( dungeon_id ) 
--	던전 과의 관계를 가져 옴: get_dungeon_relation( dungeon_id ) 
--	현재 세율: get_tax_rate( dungeon_id )
--	세율 변경: set_tax_rate( dungeon_id, tax_rate )
--	현재 세금: get_tax_amount()
--	세금 저장: draw_tax()
--	get_tax_chaos_amount() : 현재 저장된 라크 양 함수
--	draw_tax_chaos() : 꺼내는 함수

   
   
function NPC_dungeon_siege_manager_init()
	cprint( "!던전 관리인 미이나 가동" )
	set_npc_name(  "@90408500"  )
end


function NPC_dungeon_siege_manager_contact( dungeon_id )

	-- scf_is_owned_dungeon()의 리턴 값 : get_dungeon_relation( dungeon_id )로 해당 던전을 소유했다는 것을 체크한 후 값 리턴
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
	
	-- 던전 소유 길드의 마스터인지 길드원인지 아니면 일반인인지 받아온다.
	
	-- 잘못된 내용으로 레드마인 버그 수정 [QA]_버그 #14539
	-- 마스터=1 길드원=2 일반인=0 던전 관리 권한 소유 길드원=9
	
	
	local dungeon_relation = get_dungeon_relation( dungeon_id )
	-- 마스터는 관리 메뉴로, 길드원은 던전 정보 메뉴로, 일반인은 일반 정보 메뉴로 보낸다. 
	if dungeon_relation == 1 or dungeon_relation == 9 then
		NPC_dungeon_siege_guildmaster_contact( dungeon_id )
	elseif dungeon_relation == 2 or dungeon_relation == 3 or dungeon_relation == 4 then
		NPC_dungeon_siege_guildmember_contact( dungeon_id )
	elseif dungeon_relation == 0 or dungeon_relation == 5 or dungeon_relation == 6 or dungeon_relation == 7 or dungeon_relation == 8 then
		NPC_dungeon_siege_adventurer_contact( dungeon_id )
	end
 
end



function NPC_dungeon_siege_adventurer_contact( dungeon_id )

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	dlg_text( "@90408502" )
	

	-- 숨겨진 던전 리스트 : 130300 수정계곡 / 130800 팔미르1 / 130900 팔미르2 / 120700 엘카시아 / 121000 백룡 / 122000 흑룡 / 123000 사룡 	
	if dungeon_id == 130300 or dungeon_id == 130800 or dungeon_id == 120700 or dungeon_id == 130900 or dungeon_id == 121000 or dungeon_id == 122000 or dungeon_id == 123000 then
	-- 숨겨진 던전 정보 보기
		dlg_menu( "@90010121", 'dungeon_information( '..dungeon_id..' )' )
		dlg_menu( "@90019007", 'secret_dungeon_information( '..dungeon_id..' )' )
	elseif
	-- 일반 정보 보기
		dlg_menu( "@90010121", 'dungeon_information( '..dungeon_id..' )' )	then
	end		
			
	--대화 종료
	dlg_menu( "@90010002", "" )
	dlg_show()
end


function NPC_dungeon_siege_guildmaster_contact( dungeon_id )

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	dlg_text( "@90408502" )
	
local dungeon_check = dungeon_id
	
	-- 숨겨진 던전 리스트 : 130300 수정계곡 / 130800 팔미르1 / 130900 팔미르2 / 120700 엘카시아 / 121000 백룡 / 122000 흑룡 / 123000 사룡 	
	if dungeon_id == 130300 or dungeon_id == 130800 or dungeon_id == 120700 or dungeon_id == 130900 or dungeon_id == 121000 or dungeon_id == 122000 or dungeon_id == 123000 then
	-- 숨겨진 던전 정보 보기
		dlg_menu( "@90019007", 'secret_dungeon_information( '..dungeon_id..' )' )
	elseif
	-- 일반 정보 보기
		dlg_menu( "@90010121", 'dungeon_information( '..dungeon_id..' )' )	then
	end		

	-- 관리 메뉴 - 해당 던전 소유 길드 마스터에게만 보임.
	dlg_menu( "@90010114", 'management_menu( '..dungeon_id..' )' )		
	
	-- 던전 포기 - 해당 던전 소유 길드 마스터에게만 보임.
	dlg_menu( "@90010158", 'dungeon_drop( '..dungeon_id..' )' )		
	
	--대화 종료
	dlg_menu( "@90010002", "" )
	dlg_show()
 
end


-- 던전 포기?
function dungeon_drop( dungeon_id )

	dlg_title( "@90408501" )
	dlg_text( "@90408518" )
	-- 확인
	dlg_menu( "@90010016", 'click_dungeon_drop( '..dungeon_id..' )' )		

	-- 취소
	dlg_menu( "@90010001", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )		
	dlg_show()
end

-- 클릭 던전 포기
function click_dungeon_drop( dungeon_id )

	-- 던전 포기 실행 되면서 값이 drop_result에 들어감 성공시 1, 실패시 0
	local drop_result = drop_dungeon_owner_ship()

	if drop_result == 0 then
		dlg_title( "@90408501" )
		dlg_text( "@90408519" )
	elseif drop_result == 1 then
		dlg_title( "@90408501" )
		dlg_text( "@90408520" )
		warp( get_value( "x" ), get_value( "y" ), 0 )
	end
	--대화 종료
	dlg_menu( "@90010002", "" )
	dlg_show()
end



function NPC_dungeon_siege_guildmember_contact( dungeon_id )

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	dlg_text( "@90408502" )
	
	-- 숨겨진 던전 리스트 : 130300 수정계곡 / 130800 팔미르1 / 130900 팔미르2 / 120700 엘카시아 / 121000 백룡 / 122000 흑룡 / 123000 사룡 	
	if dungeon_id == 130300 or dungeon_id == 130800 or dungeon_id == 120700 or dungeon_id == 130900 or dungeon_id == 121000 or dungeon_id == 122000 or dungeon_id == 123000 then
	-- 숨겨진 던전 정보 보기
		dlg_menu( "@90019007", 'secret_dungeon_information( '..dungeon_id..' )' )
	elseif
	-- 일반 정보 보기
		dlg_menu( "@90010121", 'dungeon_information( '..dungeon_id..' )' )	then
	end					
	
	--대화 종료
	dlg_menu( "@90010002", "" )
	dlg_show()
 
end



-- 던전 일반 정보 보기(일반인 전용 메뉴)
function dungeon_information( dungeon_id )

	-- 현재 세율값을 받아온다.
	local tax_rate = get_tax_rate( dungeon_id )
	-- 던전 소유 길드명을 받아온다.
	local own_guild_name = get_own_guild_name( dungeon_id ) 

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	dlg_text(sconv("@90408510" , "#@own_guild@#" , own_guild_name , "#@tax_rate@#" , tostring(tax_rate)))

	-- 소유 길드란?
	dlg_menu( "@90010122", 'question_dungeon_owner('..dungeon_id..')' )
	-- 세율이란?
	dlg_menu( "@90010123", 'question_tax_rate('..dungeon_id..')' )		

	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()

end


-- 숨겨진 던전 일반 정보 보기(일반인 전용 메뉴)
function secret_dungeon_information( dungeon_id )

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 숨겨진 던전 소개
	dlg_text( "@90408806" ) 

	-- 들어가는 방법?
	dlg_menu( "@90019008", 'question_secret_dungeon_enter('..dungeon_id..')' )
	-- 키 몬스터란?
	dlg_menu( "@90019009", 'question_secret_dungeon_keymonster('..dungeon_id..')' )
	-- 나오는 방법?
	dlg_menu( "@90019010", 'question_secret_dungeon_exit('..dungeon_id..')' )	
	-- 주의사항
	dlg_menu( "@90019011", 'question_secret_dungeon_caution('..dungeon_id..')' )		

	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()

end

function question_secret_dungeon_enter(dungeon_id)

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 들어가는 방법?
	dlg_text( "@90408807" ) 
	-- 더 물어보기
	dlg_menu( "@90019012", 'question_secret_dungeon_enter_add('..dungeon_id..')' )
	-- 키 몬스터란?
	dlg_menu( "@90019009", 'question_secret_dungeon_keymonster('..dungeon_id..')' )
	-- 나오는 방법?
	dlg_menu( "@90019010", 'question_secret_dungeon_exit('..dungeon_id..')' )	
	-- 주의사항
	dlg_menu( "@90019011", 'question_secret_dungeon_caution('..dungeon_id..')' )


	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()
end

function question_secret_dungeon_enter_add( dungeon_id )
	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 나가는 방법?
	dlg_text( "@90408811" ) 
	
	-- 들어가는 방법?
	dlg_menu( "@90019008", 'question_secret_dungeon_enter('..dungeon_id..')' )
	-- 키 몬스터란?
	dlg_menu( "@90019009", 'question_secret_dungeon_keymonster('..dungeon_id..')' )
	-- 주의사항
	dlg_menu( "@90019011", 'question_secret_dungeon_caution('..dungeon_id..')' )

	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()


end

function question_secret_dungeon_keymonster(dungeon_id)

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 키몬스터는...
	dlg_text( "@90408808" ) 
	
	-- 들어가는 방법?
	dlg_menu( "@90019008", 'question_secret_dungeon_enter('..dungeon_id..')' )
	-- 나오는 방법?
	dlg_menu( "@90019010", 'question_secret_dungeon_exit('..dungeon_id..')' )	
	-- 주의사항
	dlg_menu( "@90019011", 'question_secret_dungeon_caution('..dungeon_id..')' )

	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()
end

function question_secret_dungeon_exit(dungeon_id)

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 나가는 방법?
	dlg_text( "@90408809" ) 
	-- 더 물어보기
	dlg_menu( "@90019012", 'question_secret_dungeon_exit_add('..dungeon_id..')' )
	-- 들어가는 방법?
	dlg_menu( "@90019008", 'question_secret_dungeon_enter('..dungeon_id..')' )
	-- 키 몬스터란?
	dlg_menu( "@90019009", 'question_secret_dungeon_keymonster('..dungeon_id..')' )
	-- 주의사항
	dlg_menu( "@90019011", 'question_secret_dungeon_caution('..dungeon_id..')' )

	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()
end


function question_secret_dungeon_exit_add( dungeon_id )
	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 나가는 방법?
	dlg_text( "@90408812" ) 
	-- 들어가는 방법?
	dlg_menu( "@90019008", 'question_secret_dungeon_enter('..dungeon_id..')' )
	-- 키 몬스터란?
	dlg_menu( "@90019009", 'question_secret_dungeon_keymonster('..dungeon_id..')' )
	-- 주의사항
	dlg_menu( "@90019011", 'question_secret_dungeon_caution('..dungeon_id..')' )

	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()


end

function question_secret_dungeon_caution(dungeon_id)

	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 주의사항?
	dlg_text( "@90408810" ) 
	-- 더 물어보기
	dlg_menu( "@90019012", 'question_secret_dungeon_caution_add('..dungeon_id..')' )
	-- 들어가는 방법?
	dlg_menu( "@90019008", 'question_secret_dungeon_enter('..dungeon_id..')' )
	-- 키 몬스터란?
	dlg_menu( "@90019009", 'question_secret_dungeon_keymonster('..dungeon_id..')' )
	-- 나오는 방법?
	dlg_menu( "@90019010", 'question_secret_dungeon_exit('..dungeon_id..')' )	
	
	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()
end



function question_secret_dungeon_caution_add( dungeon_id )
	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 나가는 방법?
	dlg_text( "@90408813" ) 
	-- 더 물어보기
	dlg_menu( "@90019012", 'question_secret_dungeon_caution_add_1('..dungeon_id..')' )
	-- 들어가는 방법?
	dlg_menu( "@90019008", 'question_secret_dungeon_enter('..dungeon_id..')' )
	-- 키 몬스터란?
	dlg_menu( "@90019009", 'question_secret_dungeon_keymonster('..dungeon_id..')' )
	-- 주의사항
	dlg_menu( "@90019011", 'question_secret_dungeon_caution('..dungeon_id..')' )

	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()


end

function question_secret_dungeon_caution_add_1( dungeon_id )
	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	-- 나가는 방법?
	dlg_text( "@90408814" ) 
	-- 들어가는 방법?
	dlg_menu( "@90019008", 'question_secret_dungeon_enter('..dungeon_id..')' )
	-- 키 몬스터란?
	dlg_menu( "@90019009", 'question_secret_dungeon_keymonster('..dungeon_id..')' )
	-- 주의사항
	dlg_menu( "@90019011", 'question_secret_dungeon_caution('..dungeon_id..')' )

	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()


end

-- 소유 길드란?
function question_dungeon_owner( dungeon_id )
	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	dlg_text( "@90408511" )
	--돌아가기
	dlg_menu( "@90010003", 'dungeon_information( '..dungeon_id..' )' )	
	dlg_show()
end


-- 세율이란?
function question_tax_rate( dungeon_id )
	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	dlg_text( "@90408512" )
	--돌아가기
	dlg_menu( "@90010003", 'dungeon_information( '..dungeon_id..' )' )	
	dlg_show()
end



-- 관리 메뉴(던전 소유 길드원에게만 보임.)
function dungeon_guild_information( dungeon_id )
	-- 현재 세율값을 받아온다.
	local tax_rate = get_tax_rate( dungeon_id )
	-- 걷힌 세금 얻어옴
	local tax = get_tax_amount()
	-- 걷힌 라크 얻어옴
	local lak = get_tax_chaos_amount()
			
	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	dlg_text(sconv("@90408515" , "#@tax_rate@#" , tostring(tax_rate) , "#@tax@#" , tostring(tax) , "#@lak@#" , tostring(lak)))
		
	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_manager_contact( '..dungeon_id..' )' )	
	dlg_show()

end


-- 관리 메뉴(길드 마스터에게만 보임.)
function management_menu( dungeon_id )
	-- 걷힌 세금 얻어옴
	local tax = get_tax_amount()
	-- 현재 세율값을 받아온다.
	local tax_rate = get_tax_rate( dungeon_id )
	-- 걷힌 라크 얻어옴
	local lak = get_tax_chaos_amount()
	
	
	-- 다이얼로그 출력
	dlg_title( "@90408501" )
	dlg_text(sconv("@90408515" , "#@tax_rate@#" , tostring(tax_rate) , "#@tax@#" , tostring(tax) , "#@lak@#" , tostring(lak)))

	-- 세금 수금
	dlg_menu( "@90010116", 'tax_collection( '..dungeon_id..' )' )		

	-- 라크 수금
	dlg_menu( "@90010125", 'lak_collection( '..dungeon_id..' )' )		
	
	-- 세율 조정
	dlg_menu( "@90010118", 'tax_rate_adjust( '..dungeon_id..' )' )
	
	--돌아가기
	dlg_menu( "@90010003", 'NPC_dungeon_siege_guildmaster_contact('..dungeon_id..')' )	
	dlg_show()
end



-- 세금 수금
function tax_collection( dungeon_id )
	
	-- 걷힌 세금 얻어옴
	local tax = get_tax_amount()
		
	-- 수금하시겠습니까?
	dlg_title( "@90408501" )
	dlg_text(sconv("@90408504" , "#@tax@#" , tostring(tax)))
	
	-- 확인(돈 지급)
	dlg_menu( "@90010030", 'tax_collection_click( '..dungeon_id..' )' )

	-- 취소(전 메뉴로 돌아감)
	dlg_menu( "@90010001", 'management_menu(' .. dungeon_id .. ')' )
	dlg_show()

end


-- 수금 버튼 눌러서 돈 지급
function tax_collection_click( dungeon_id )
	-- 걷힌 세금 얻어옴
	local tax = get_tax_amount()

	-- 소지금 제한 루틴
	local nummoney, player_money
	player_money = gv("gold")	--	유저 현재 소지금을 가져온다.
	nummoney = tax + player_money	--	유저 현재 소지금과 걷힌 세금을 합친다.
	if nummoney > 10000000000 then	--	세금과 유저 현재 소지금을 합쳐서 100억이 넘는지 체크 한다.
	    dlg_title( "@90408501" )
		dlg_text("@577")	-- 	소지금이 초과되었다는 메시지출력
		dlg_menu( "@90010003", 'management_menu(' .. dungeon_id .. ')' )	-- 메뉴 호출
		dlg_show()	-- 보이기
		
	-- 이미 걷어간 상태이거나 수금할 돈이 없다면
	elseif tax == 0 then
		dlg_title( "@90408501" )
		dlg_text( "@90408513" )
		--돌아가기
		dlg_menu( "@90010003", 'management_menu( '..dungeon_id..' )' )	
		dlg_show()
	else
		-- 세금 걷고 저장
		draw_tax()
		-- 저장	
		save()
		-- 관리 메뉴로 돌아감
		management_menu( dungeon_id )
	end
		
end

-- 라크 수금
function lak_collection( dungeon_id )
	
	-- 걷힌 라크 얻어옴
	local lak = get_tax_chaos_amount()
		
	-- 수금하시겠습니까?
	dlg_title( "@90408501" )
	dlg_text( "@90408516" )
	
	-- 확인(라크 지급)
	dlg_menu( "@90010030", 'lak_collection_click( '..dungeon_id..' )' )
	-- 취소(전 메뉴로 돌아감)
	dlg_menu( "@90010001", 'management_menu(' .. dungeon_id .. ')' )
	dlg_show()

end


-- 수금 버튼 눌러서 라크 지급
function lak_collection_click( dungeon_id )
	-- 걷힌 라크 얻어옴
	local lak = get_tax_chaos_amount()
	-- 이미 걷어간 상태이거나 수금할 돈이 없다면
	if lak == 0 then
		dlg_title( "@90408501" )
		dlg_text( "@90408517" )
		--돌아가기
		dlg_menu( "@90010003", 'management_menu( '..dungeon_id..' )' )	
		dlg_show()
	else	
		-- 라크 걷고 저장
		draw_tax_chaos()
		-- 저장	
		save()
		-- 관리 메뉴로 돌아감
		management_menu( dungeon_id )
	end
		
end

-- 세율 조정
function tax_rate_adjust( dungeon_id )
	-- 현재 세율값을 받아온다.
	local tax_rate = get_tax_rate( dungeon_id )
	
	dlg_title( "@90408501" )
	dlg_text( "@90408506" )
	dlg_text(sconv("@90408506" , "#@tax_rate@#" , tostring(tax_rate)))
		
	-- 세율 1% 증가 메뉴
	dlg_menu( "@90010119", 'tax_rate_increase('..dungeon_id..')' )
	-- 세율 1% 감소 메뉴
	dlg_menu( "@90010120", 'tax_rate_decrease('..dungeon_id..')' )
	
	--돌아가기
	dlg_menu( "@90010003", 'management_menu('..dungeon_id..')' )
	dlg_show()
end
	
function tax_rate_increase( dungeon_id )

	-- 현재 세율값을 받아온다.
	local tax_rate = get_tax_rate( dungeon_id )
	-- 세율이 이미 10%라면 퇴짜
	if tax_rate == 10 then
		dlg_title( "@90408501" )
		dlg_text( "@90408507" )
		--돌아가기
		dlg_menu( "@90010003", 'tax_rate_adjust('..dungeon_id..')' )	
		dlg_show()
	
	-- 세율 변경: 
	elseif tax_rate < 10 then
		set_tax_rate( dungeon_id, tax_rate+1 )
		
	-- 윗 단계로 돌아감
	tax_rate_adjust( dungeon_id )
	end
end

 
function tax_rate_decrease( dungeon_id )
	-- 현재 세율값을 받아온다.
	local tax_rate = get_tax_rate( dungeon_id )
			
	-- 세율이 이미 1%이라면 퇴짜
	if tax_rate == 1 then
		dlg_title( "@90408501" )
		dlg_text( "@90408508" )
		--돌아가기
		dlg_menu( "@90010003", 'tax_rate_adjust('..dungeon_id..')' )	
		dlg_show()
	
	-- 세율 변경: 	
	elseif tax_rate > 1 then
		set_tax_rate( dungeon_id, tax_rate-1 )
	
	-- 윗 단계로 돌아감
	tax_rate_adjust( dungeon_id )
	end
end


----------------------------------------------------------------------------
----------------------------------------------------------------------------
------------------- 벚꽃 이벤트 npc 단술 동호회 회원 -----------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------


function NPC_sweet_drink_init()

	cprint( "!단술 동호회 회원" )
	set_npc_name(  "@90991200"  )
end



function NPC_sweet_drink_contact()

	-- 다이얼로그 출력
	dlg_title( "@90991201" )
	dlg_text( "@90991202" )

	-- 당신은 누구?
	dlg_menu( "@90010148", 'sweet_drink_who_are_you()' )			
	
	-- 단술이란?
	dlg_menu( "@90010149", 'sweet_drink_question()' )			
	
	-- 단술 교환
	dlg_menu( "@90010150", 'sweet_drink_exchange()' )			
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end



-- 당신은 누구?
function sweet_drink_who_are_you()
	-- 다이얼로그 출력
	dlg_title( "@90991201" )
	dlg_text( "@90991203" )
	--돌아가기
	dlg_menu( "@90010003", 'NPC_sweet_drink_contact()' )	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end
	
	

-- 단술이란?
function sweet_drink_question()
	-- 다이얼로그 출력
	dlg_title( "@90991201" )
	dlg_text( "@90991204" )

	--돌아가기
	dlg_menu( "@90010003", 'NPC_sweet_drink_contact()' )	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end

	
-- 단술 교환
function sweet_drink_exchange()


	local item_count = find_item ( 2000140 )
	item_handle = get_item_handle ( 2000140 )
	
	if item_count >= 50 then
		-- 단술 교환 성공시
		dlg_title( "@90991201" )
		dlg_text( "@90991205" )
	    delete_item ( item_handle, 50 )
    	insert_item ( 2000141, 1 )
		    
	else
		-- 단술 모자랄 경우
		dlg_title( "@90991201" )
		dlg_text( "@90991206" )
		
	end
		--돌아가기
		dlg_menu( "@90010003", 'NPC_sweet_drink_contact()' )		
		-- 대화종료 
		dlg_menu( "@90010002", " " )
		dlg_show()
end
	


   --============================================================
   --===============		호라이즌 조합장 가더
   --============================================================

function NPC_Quest_Gangway_Administrator_init()
	cprint( "!조합장 가더 가동" )
	set_npc_name(  "@90409100"  )
end

function NPC_Quest_Gangway_Administrator_contact()

	-- 다이얼로그 출력
	dlg_title( "@90409101" )
	dlg_text( "@90409102" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		잃어버린 갱도 생존자 튠
   --============================================================

function NPC_Quest_Gangway_survivor_init()
	cprint( "!생존자 튠 가동" )
	set_npc_name(  "@90409200"  )
end

function NPC_Quest_Gangway_survivor_contact()

	-- 다이얼로그 출력
	dlg_title( "@90409201" )
	dlg_text( "@90409202" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end



   --============================================================
   --===============		론도 무역 조합 사원 라카드
   --============================================================

function NPC_Trade_member_Rondoh_init()

	cprint( "!무역 조합 사원 라카드 가동" )
	set_npc_name(  "@90601700"  )
end

function NPC_Trade_member_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90601701" )
	dlg_text( "@90601702" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 양아치 키리오
   --============================================================

function NPC_Beggar_Rondoh_init()

	cprint( "!양아치 키리오 가동" )
	set_npc_name(  "@90601800"  )
end

function NPC_Beggar_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90601801" )
	dlg_text( "@90601802" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 건달 파두
   --============================================================

function NPC_Scamp_Rondoh_init()
	cprint( "!건달 파두 가동" )
	set_npc_name(  "@90601900"  )
end

function NPC_Scamp_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90601901" )
	dlg_text( "@90601902" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 시장 에일레나
   --============================================================

function NPC_Mayoress_Rondoh_init()
	cprint( "!시장 에일레나 가동" )
	set_npc_name(  "@90602000"  )
end

function NPC_Mayoress_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90602001" )
	dlg_text( "@90602002" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 무역 조합장 미륜
   --============================================================

function NPC_Trade_master_Rondoh_init()
	cprint( "!무역 조합장 미륜 가동" )
	set_npc_name(  "@90602100"  )
end

function NPC_Trade_master_Rondoh_contact()


	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress000 =  get_quest_progress(1226)
	-- 퀘스트 종료시 대사	
	if quest_progress000 == 255 then
		dlg_title( "@90602101" )
		dlg_text( "@90602106" )
	-- 퀘스트 수행 전, 수행 중 대사
	else 
		-- 다이얼로그 출력
		dlg_title( "@90602101" )
		dlg_text( "@90602102" )
	end
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 섀도우헌터 토르하
   --============================================================

function NPC_Shadow_Hunter_Rondoh_init()
	cprint( "!섀도우헌터 토르하 가동" )
	set_npc_name(  "@90602300"  )
end

function NPC_Shadow_Hunter_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90602301" )
	dlg_text( "@90602302" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 경비병
   --============================================================

function NPC_Guard_Rondoh_contact()

   	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- <이름 출력> 각 NPC에 따라 적합한 
		
		if npc_id == 6024 then
			dlg_title("@90602401")
			dlg_text( "@90602402" )
		elseif npc_id == 6025 then
			dlg_title("@90602501")
			dlg_text( "@90602502" )
		elseif npc_id == 6026 then
			dlg_title("@90602601")
			dlg_text( "@90602602" )
		elseif npc_id == 6027 then
			dlg_title("@90602701")
			dlg_text( "@90602702" )
		elseif npc_id == 6028 then
			dlg_title("@90602801")
			dlg_text( "@90602802" )
		elseif npc_id == 6029 then
			dlg_title("@90602901")
			dlg_text( "@90602902" )
		elseif npc_id == 6030 then
			dlg_title("@90603001")
			dlg_text( "@90603002" )
		elseif npc_id == 6031 then
			dlg_title("@90603101")
			dlg_text( "@90603102" )
		elseif npc_id == 6040 then
			dlg_title("@90604001")
			dlg_text( "@90604002" )
		elseif npc_id == 6041 then
			dlg_title("@90604101")
			dlg_text( "@90604102" )
		end

	-- 끝내기
	dlg_menu( "@90010002", "" )
	-- 다이얼로그 출력하기
	dlg_show()
	
end
   

   --============================================================
   --===============		론도 경비대장 쿠둔
   --============================================================

function NPC_Guard_Commander_Rondoh_init()
	cprint( "!경비대장 쿠둔 가동" )
	set_npc_name(  "@90603200"  )
end

function NPC_Guard_Commander_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90603201" )
	dlg_text( "@90603202" )
	
--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
	dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]		 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 대부호 쿠랄리느
   --============================================================

function NPC_Great_Rich_Rondoh_init()
	cprint( "!대부호 쿠랄리느 가동" )
	set_npc_name(  "@90603300"  )
end

function NPC_Great_Rich_Rondoh_contact()
	dlg_title( "@90603301" )
	dlg_text( "@90603302" )
	
	local state_code = get_local_info() 
	
	if state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then
		dlg_menu( "@90610497", "NPC_Event_2017_Devildom_Crystal_Trade_About()" ) -- Can I make an investment?
		
		if get_env( "game.devildom_crystal_trade" ) == 1 then --If feature is active			
			dlg_menu( "@90610498", "NPC_Event_2017_Devildom_Crystal_Trade()" ) -- Trade Crystals
		end
	end 			
	
	dlg_menu( "@90010002", " " ) -- 대화종료 
 	dlg_show()
 
end

   --============================================================
   --===============		론도 흑랑 길드원 츠바쿠
   --============================================================

function NPC_Dark_Wolf_member_1_Rondoh_init()
	cprint( "!흑랑 길드원 츠바쿠 가동" )
	set_npc_name(  "@90603400"  )
end

function NPC_Dark_Wolf_member_1_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90603401" )
	dlg_text( "@90603402" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 흑랑 길드원 드뷴
   --============================================================

function NPC_Dark_Wolf_member_2_Rondoh_init()
	cprint( "!흑랑 길드원 드뷴 가동" )
	set_npc_name(  "@90603500"  )
end

function NPC_Dark_Wolf_member_2_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90603501" )
	dlg_text( "@90603502" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 아스트랄 교인 미노아
   --============================================================

function NPC_Astral_Believer_Rondoh_init()
	cprint( "!아스트랄 교인 미노아 가동" )
	set_npc_name(  "@90603600"  )
end

function NPC_Astral_Believer_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90603601" )
	dlg_text( "@90603602" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 연인 제니아
   --============================================================

function NPC_lover_1_Rondoh_init()
	cprint( "!연인 제니아 가동" )
	set_npc_name(  "@90603700"  )
end

function NPC_lover_1_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90603701" )
	dlg_text( "@90603702" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 연인 케일
   --============================================================

function NPC_lover_2_Rondoh_init()
	cprint( "!연인 케일 가동" )
	set_npc_name(  "@90603800"  )
end

function NPC_lover_2_Rondoh_contact()
	dlg_title( "@90603801" )
	
	--2016 발렌타인 이벤트 기간만 동작하게
	local state_code = get_local_info()
	local current_time = get_os_date( "%Y-%m-%d %H:%M:%S" )
	local check = 0
	local SoloBuff = tonumber( get_global_variable( "solo_buff_2016" ) ) or 0
	if state_code == 1 and '2016-02-11 10:00:00' <= current_time and current_time < '2016-03-03 10:00:00' then
		check = 1
	elseif state_code == 16 and '2016-02-12 11:00:00' <= current_time and current_time < '2016-03-01 11:00:00' then
		check = 1
	elseif state_code == 256 and '2016-02-08 08:00:00' <= current_time and current_time < '2016-02-29 08:00:00' then
		check = 1
	elseif state_code == 512 and '2016-02-19 11:00:00' <= current_time and current_time < '2016-03-04 11:00:00' then
		check = 1
	end
	
	if check == 1 then
		if SoloBuff == 0 then	--	솔로버프 없을 경우
			dlg_text( "@90610187" ) -- 다이얼로그 출력			
			dlg_menu( "@90610184", "Valentine_2016_Kale_Give()" ) -- 쿠피드의 선물을 건네준다
		else
			dlg_text( "@90610189" ) -- 다이얼로그 출력	
		end		
		
	else			
		dlg_text( "@90603802" ) -- 다이얼로그 출력
	end
	
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		론도 보디가드 파푸가
   --============================================================

function NPC_BodyGuard_Rondoh_init()
	cprint( "!보디가드 파푸가 가동" )
	set_npc_name(  "@90603900"  )
end

function NPC_BodyGuard_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90603901" )
	dlg_text( "@90603902" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end
	




   --============================================================
   --===============		수정의 산 에이전트 케이
   --============================================================

function NPC_Agent_K_Rondoh_init()
	cprint( "!에이전트 케이 기동" )
	set_npc_name(  "@90604200"  )
end

function NPC_Agent_K_Rondoh_contact()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress000 =  get_quest_progress(1224)
	-- 퀘스트 종료시 대사	
	if quest_progress000 == 255 then
		dlg_title( "@90604201" )
		dlg_text( "@90604217" )
	-- 퀘스트 수행 전, 수행 중 대사
	else 
		dlg_title( "@90604201" )
		dlg_text( "@90604202" )
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()

end


--============================================================
--=============== 대련장 치료사 시유
--============================================================

function NPC_healer_contact()

	local npc_id = get_npc_id()

		if npc_id == 6043 then
			dlg_title( "@90604301" )
			dlg_text( "@90604302" )
		elseif npc_id == 1088 then
			dlg_title("@90108801")
			dlg_text( "@90108802" )
		elseif npc_id == 4093 then
			dlg_title("@90409301")
			dlg_text( "@90409302" )
		elseif npc_id == 2086 then
			dlg_title("@90208601")
			dlg_text( "@90208602" )
		end
			
	-- 대화종료 
	dlg_menu( "@90010156", "question_vie()" )
	dlg_menu( "@90010157", "vie_healling()" )

	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end





function question_vie()
	local npc_id = get_npc_id()

		if npc_id == 6043 then
			dlg_title( "@90604301" )
			dlg_text( "@90604303" )
		elseif npc_id == 1088 then
			dlg_title("@90108801")
			dlg_text( "@90108803" )
		elseif npc_id == 4093 then
			dlg_title("@90409301")
			dlg_text( "@90409303" )
		elseif npc_id == 2086 then
			dlg_title("@90208601")
			dlg_text( "@90208603" )
		end
	
	dlg_menu( "@90010003", "NPC_healer_contact()" )

	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end



function vie_healling()
	
	local gold = get_value( "gold" )		
	local level = get_value( "level" )

	local npc_id = get_npc_id()
	
	-- 기존 가격	
--	local cost = level * 100
	
	-- 변경된 가격
	local cost = math.floor( 1.9* (level ^ 1.85) + 9 )
	
		if npc_id == 6043 then
			dlg_title( "@90604301" )
		elseif npc_id == 1088 then
			dlg_title("@90108801")
		elseif npc_id == 4093 then
			dlg_title("@90409301")
		elseif npc_id == 2086 then
			dlg_title("@90208601")
		end

	temp_text =sconv("@90604304", "#@healling_cost@#", cost)
    dlg_text(temp_text)
	       
    dlg_menu( "@90010016", "pay_heal()" )	
	dlg_menu( "@90010001", "NPC_healer_contact()" )
	dlg_show()
 
end



function pay_heal()

	local gold = get_value( "gold" )
	local level = get_value( "level" )
	local npc_id = get_npc_id()

	-- 기존 가격	
--	local cost = level * 100
	
	-- 변경된 가격
	local cost = math.floor( 1.9* (level ^ 1.85) + 9 )

		if npc_id == 6043 then
			dlg_title( "@90604301" )
		elseif npc_id == 1088 then
			dlg_title("@90108801")
		elseif npc_id == 4093 then
			dlg_title("@90409301")
		elseif npc_id == 2086 then
			dlg_title("@90208601")
		end
	
	
	
	
	
	if cost > gold then
		if npc_id == 6043 then
			dlg_title( "@90604301" )
		elseif npc_id == 1088 then
			dlg_title("@90108801")
		elseif npc_id == 4093 then
			dlg_title("@90409301")
		elseif npc_id == 2086 then
			dlg_title("@90208601")
		end
		dlg_text( "@90604305" )
	else
		if npc_id == 6043 then
			dlg_title( "@90604301" )
		elseif npc_id == 1088 then
			dlg_title("@90108801")
		elseif npc_id == 4093 then
			dlg_title("@90409301")
		elseif npc_id == 2086 then
			dlg_title("@90208601")
		end
	dlg_text( "@90604306" )
	set_value( "hp" , get_value( "max_hp") )
	set_value( "gold", gold - cost )
	update_gold_chaos()
	end
	
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




	
   --============================================================
   --===============	스템 프러 리 
   --============================================================
function NPC_stamp_master_contact()

	-- 퀘스트 중인지 받아온다. 1이면 퀘스트 중. 0이면 도전 시작
	local q_flag = get_flag( "q4" )
	local q_flag1 = get_flag( "q5" )
	local q_flag2 = get_flag( "q6" )
	local q_flag3 = get_flag( "q7" )
	local q_flag4 = get_flag( "q8" )
	local q_flag5 = get_flag( "q9" )
	local q_flag6 = get_flag( "q10" )
	local q_flag7 = get_flag( "q11" )
	local q_flag8 = get_flag( "q12" )
	local q_flag9 = get_flag( "q13" )
	local q_flag10 = get_flag( "q14" )
	local q_flag00 = get_flag( "q15" )

	-- 퀘스트 완료해버린 뒤. 할 게 없음
	if q_flag00 == 1 then
		-- 다이얼로그 출력
		dlg_title( "@90991901" )
		dlg_text( "@90991913" )
	-- 퀘스트 받은 상태에서 모두 모았을 때
	elseif q_flag == 1 and q_flag1 == 1 and q_flag2 == 1 and q_flag3 == 1 and q_flag4 == 1 and q_flag5 == 1 and q_flag6 == 1 and q_flag7 == 1 and q_flag8 == 1 and q_flag9 == 1 and q_flag10 == 1 then
			-- 다이얼로그 출력
			dlg_title( "@90991901" )
			dlg_text( "@90991912" )
			-- 퀘스트 완전히 완료
			set_flag( "q15", "1")
			delete_item( get_item_handle(2010247), 1 )
			delete_item( get_item_handle(2010248), 1 )
			delete_item( get_item_handle(2010249), 1 )
			delete_item( get_item_handle(2010250), 1 )
			delete_item( get_item_handle(2010251), 1 )
			delete_item( get_item_handle(2010252), 1 )
			delete_item( get_item_handle(2010253), 1 )
			delete_item( get_item_handle(2010254), 1 )
			delete_item( get_item_handle(2010255), 1 )
			delete_item( get_item_handle(2010256), 1 )
			stamp_master_clear()
			insert_item( 2901000, 3 )
	elseif q_flag == 1 then
			-- 다이얼로그 출력
			dlg_title( "@90991901" )
			dlg_text( "@90991905" )
			-- 대화종료 
			dlg_menu( "@90010159", 'question_stamp()' )
			dlg_menu( "@90010162", 'question_stamp_master()' )
	elseif q_flag == 0 then
			-- 다이얼로그 출력
			dlg_title( "@90991901" )
			dlg_text( "@90991902" )
			-- 대화종료 
			dlg_menu( "@90010159", 'question_stamp()' )
			dlg_menu( "@90010162", 'question_stamp_master()' )	
			dlg_menu( "@90010160", 'challenge_stamp_master()' )	
	else
			-- 다이얼로그 출력
			dlg_title( "@90991901" )
			dlg_text( "@90991902" )
			-- 대화종료 
			dlg_menu( "@90010159", 'question_stamp()' )
			dlg_menu( "@90010162", 'question_stamp_master()' )	
			dlg_menu( "@90010160", 'challenge_stamp_master()' )	
		
			q_flag = set_flag( "q4" , "0")
			q_flag1 = set_flag( "q5" , "0")
			q_flag2 = set_flag( "q6" , "0")
			q_flag3 = set_flag( "q7" , "0")
			q_flag4 = set_flag( "q8" , "0")
			q_flag5 = set_flag( "q9" , "0")
			q_flag6 = set_flag( "q10" , "0")
			q_flag7 = set_flag( "q11" , "0")
			q_flag8 = set_flag( "q12" , "0")
			q_flag9 = set_flag( "q13" , "0")
			q_flag10 = set_flag( "q14" , "0")
			q_flag00 = set_flag( "q15" , "0")
			
			
	end
	dlg_menu( "@90010002", " " )
 	dlg_show()
end


-- 도전 스탬프러리!
function challenge_stamp_master()
	-- 다이얼로그 출력
	dlg_title( "@90991901" )
	dlg_text( "@90991904" )
	--확인		
	dlg_menu( "@90010016", 'yes_question_stamp()' )
	--취소
	dlg_menu( "@90010001", 'NPC_stamp_master_contact()' )	
	dlg_show()
end

-- 도전 허락
function yes_question_stamp()
	-- 다이얼로그 출력
	dlg_title( "@90991901" )
	dlg_text( "@90991914" )
	set_flag( "q4", "1")
	dlg_menu( "@90010002", " " )
	dlg_show()
end


--스탬프러리란?	
function question_stamp()
	-- 다이얼로그 출력
	dlg_title( "@90991901" )
	dlg_text( "@90991903" )
	dlg_menu( "@90010003", ' NPC_stamp_master_contact() ' )
	dlg_menu( "@90010002", " " )
	-- 대화종료 
	dlg_show()
end


-- 스탬프러리는 어디에?	
function question_stamp_master()
	-- 다이얼로그 출력
	dlg_title( "@90991901" )
	dlg_text( "@90991907" )
	
	dlg_menu( "@90010163", 'question_stamp_master_katan()' )			
	dlg_menu( "@90010164", 'question_stamp_master_larc()' )	
	dlg_menu( "@90010165", 'question_stamp_master_rondo()' )	
	dlg_menu( "@90010166", 'question_stamp_master_etc()' )	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end		
	
function question_stamp_master_katan()
	-- 다이얼로그 출력
	dlg_title( "@90991901" )
	dlg_text( "@90991908" )
	-- 대화종료 
	dlg_menu( "@90010003", 'question_stamp_master()' )
	dlg_menu( "@90010002", " " )
 	dlg_show()
end

function question_stamp_master_larc()
	-- 다이얼로그 출력
	dlg_title( "@90991901" )
	dlg_text( "@90991909" )
	-- 대화종료 
	dlg_menu( "@90010003", 'question_stamp_master()' )
	dlg_menu( "@90010002", " " )
 	dlg_show()
end

function question_stamp_master_rondo()
	-- 다이얼로그 출력
	dlg_title( "@90991901" )
	dlg_text( "@90991910" )
	-- 대화종료 
	dlg_menu( "@90010003", 'question_stamp_master()' )
	dlg_menu( "@90010002", " " )
 	dlg_show()
end

function question_stamp_master_etc()
	-- 다이얼로그 출력
	dlg_title( "@90991901" )
	dlg_text( "@90991911" )
	-- 대화종료 
	dlg_menu( "@90010003", 'question_stamp_master()' )
	dlg_menu( "@90010002", " " )
 	dlg_show()
end



   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_1_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q5" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q5", "1")
			insert_item( 2010247, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	
		

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_2_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q6" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q6", "1")
			insert_item( 2010248, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_3_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q7" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q7", "1")
			insert_item( 2010249, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_4_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q8" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q8", "1")
			insert_item( 2010250, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_5_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q9" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q9", "1")
			insert_item( 2010251, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_6_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q10" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q10", "1")
			insert_item( 2010252, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_7_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q11" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q11", "1")
			insert_item( 2010253, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_8_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q12" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q12", "1")
			insert_item( 2010254, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_9_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q13" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q13", "1")
			insert_item( 2010255, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	

   --============================================================
   --===============	스탬프러리 아가씨
   --============================================================

function NPC_stamp_lady_10_contact()
	--퀘스트 중인지 읽어온다. 퀘스트 중이면 q4가 1. 아니면 0
	local q_flag1 = get_flag( "q4" )
	--스탬프를 받았는지 읽어온다. 받았다면 1. 아니라면 0
	local q_flag2 = get_flag( "q14" )
	-- 골든 위크 퀘스트 중이 아니라면 즐
	if q_flag1 == 0 then
		-- 다이얼로그 출력
		dlg_title( "@90992001" )
		dlg_text( "@90992002" )
	-- 만약 퀘스트 중이라면
	elseif q_flag1 == 1 then
		if q_flag2 == 1 then
			-- 스탬프를 받은 상태라면 1 받지 않았다면 0
			-- 스탬프 받았으니 즐
			-- 다이얼로그 출력
			dlg_title( "@90992001" )
			dlg_text( "@90992004" )
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		else 
			-- 스탬프를 받지 않았다면 준다. 축하!
			dlg_title( "@90992001" )
			dlg_text( "@90992003" )
			-- 스탬프 넣어주고 플래그 1로 채크		
			set_flag( "q14", "1")
			insert_item( 2010256, 1 )
			stamp_master_clear()
			dlg_menu( "@90010162", 'question_stamp_lady()' )
		end
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
end	


	
	
function question_stamp_lady()
	-- 다이얼로그 출력
	dlg_title( "@90992001" )
	dlg_text( "@90992005" )
	
	dlg_menu( "@90010163", 'question_stamp_lady_katan()' )			
	dlg_menu( "@90010164", 'question_stamp_lady_larc()' )	
	dlg_menu( "@90010165", 'question_stamp_lady_rondo()' )	
	dlg_menu( "@90010166", 'question_stamp_lady_etc()' )	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
		
end		

		
function question_stamp_lady_katan()
	-- 다이얼로그 출력
	dlg_title( "@90992001" )
	dlg_text( "@90992006" )
	-- 대화종료 
	dlg_menu( "@90010003", 'question_stamp_lady()' )
	dlg_menu( "@90010002", " " )
 	dlg_show()
end

function question_stamp_lady_larc()
	-- 다이얼로그 출력
	dlg_title( "@90992001" )
	dlg_text( "@90992007" )
	-- 대화종료 
	dlg_menu( "@90010003", 'question_stamp_lady()' )
	dlg_menu( "@90010002", " " )
 	dlg_show()
end

function question_stamp_lady_rondo()
	-- 다이얼로그 출력
	dlg_title( "@90992001" )
	dlg_text( "@90992008" )
	-- 대화종료 
	dlg_menu( "@90010003", 'question_stamp_lady()' )
	dlg_menu( "@90010002", " " )
 	dlg_show()
end

function question_stamp_lady_etc()
	-- 다이얼로그 출력
	dlg_title( "@90992001" )
	dlg_text( "@90992009" )
	-- 대화종료 
	dlg_menu( "@90010003", 'question_stamp_lady()' )
	dlg_menu( "@90010002", " " )
 	dlg_show()
end

-- 보상 처리 구역
function stamp_master_clear()
	local level = get_value( "level" )
	local expe = get_value( "exp" )
	local jpe = get_value( "jp" )
	local experien = math.floor( level * 2.4 + level^1.46 + ( level^ 2 * 0.1) + 2 )  * 40
	local jp = 	math.floor( level * 0.8 * 40)
	set_value( "jp", jpe + jp )		
	set_value( "exp", expe + experien )
end





   --============================================================
   --===============	선물 지급 도우미
   --============================================================

function NPC_Second_present_contact()
	local q_flag0 = get_flag( "q18" )	
	local q_flag1 = get_flag( "q19" )	
	local Second_event = get_value( "event_code" )

	if Second_event == 1 then
		-- 플래그 초기화 시키기!
		if q_flag0 == 0 or q_flag1 == 0 then
			-- 다이얼로그 출력
			dlg_title( "@90993000" )          --제목 : 선물대여도우미
			dlg_text( "@90993002" )           --인사말
			-- 대화종료 
			dlg_menu( "@90010172", 'second_present()' )   --선물받기
			dlg_menu( "@90010002", " " )                  --대화종료
		
		elseif q_flag0 == 1 or q_flag1 == 1 then
			-- 다이얼로그 출력
			dlg_title( "@90993000" )          --제목 : 선물대여도우미
			dlg_text( "@90993002" )           --인사말
			-- 대화종료 
			dlg_menu( "@90010172", 'second_present()' )   --선물받기
			dlg_menu( "@90010002", " " )                  --대화종료
		else 	
			-- 다이얼로그 출력
			dlg_title( "@90993000" )          --제목 : 선물대여도우미
			dlg_text( "@90993002" )           --인사말
			-- 대화종료 
			dlg_menu( "@90010172", 'second_present()' )   --선물받기
			dlg_menu( "@90010002", " " )                  --대화종료
			set_flag( "q18", "0")	
			set_flag( "q19", "0")	
		end	
	else
		-- 다이얼로그 출력
		dlg_title( "@90993000" )  --제목 : 선물대여도우미
		dlg_text( "@90993004" )   --죄송하지만 이번 선물은 라펠즈의 세계에 오랜만에 오신 분들만 받을 수 있습니다.
		dlg_menu( "@90010002", " " )    --대화종료
	end
 	dlg_show()
end	
	

-- 선물 받기
function second_present()
	
	-- 현재 직업 id를 받아놓는다.
	local job = get_value( "job" )
	-- 레벨 받아옴
	local level = get_value( "level" )
	local q_flag0 = get_flag( "q18" )	
	local q_flag1 = get_flag( "q19" )	
	
	-- 기본 직업은 즐 때림
	if job == 100 or job == 200 or job == 300 then
		-- 다이얼로그 출력
		dlg_title( "@90993000" )        --제목 : 선물대여도우미
		dlg_text( "@90993009" )         --선물을 대여 받으려면 먼저 1차 직업으로 전직을 해야 합니다.
		
		
	elseif level <= 49 then --랭크2무기대여
	    --1차 직업을 가지고 있던 2차 직업을 가지고 있던 모두 그에 맞는 아이템을 지급해 준다.
	    if job == 201 or job == 202 or job == 203 or job == 301 or job == 302 or job == 303 or job == 101 or job == 102 or job == 103 or job == 210 or job == 211 or job == 212 or job == 213 or job == 214 or job == 310 or job == 311 or job == 312 or job == 313 or job == 314 or job == 110 or job == 111 or job == 112 or job == 113 or job == 114 then
			if q_flag0 == 1 and q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목 : 선물대여도우미
				dlg_text( "@90993005" )       --이미 선물을 대여 받으셨네요.
			elseif q_flag0 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )		--방어구받기
			
			elseif q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			elseif 	q_flag0 == 0 and q_flag1 == 0 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )		    --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )   --방어구받기
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			end
		end	
		
		
	elseif level >= 50 and level <= 79 then --랭크3무기대여
	    --1차 직업을 가지고 있던 2차 직업을 가지고 있던 모두 그에 맞는 아이템을 지급해 준다.
	    if job == 201 or job == 202 or job == 203 or job == 301 or job == 302 or job == 303 or job == 101 or job == 102 or job == 103 or job == 210 or job == 211 or job == 212 or job == 213 or job == 214 or job == 310 or job == 311 or job == 312 or job == 313 or job == 314 or job == 110 or job == 111 or job == 112 or job == 113 or job == 114 then
			if q_flag0 == 1 and q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목 : 선물대여도우미
				dlg_text( "@90993005" )       --이미 선물을 대여 받으셨네요.
			elseif q_flag0 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )		--방어구받기
			
			elseif q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			elseif 	q_flag0 == 0 and q_flag1 == 0 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )		    --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )   --방어구받기
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			end
		end	
	
	
	elseif level >= 80 and level <= 99 then --랭크4무기대여
	    --파이터 등 1차 직업 들은 2차로 전직해라는 문구 출력
	    if job == 201 or job == 202 or job == 203 or job == 301 or job == 302 or job == 303 or job == 101 or job == 102 or job == 103 then
	    dlg_title( "@90993000" )        --제목 : 선물대여도우미
	    dlg_text( "@90993010" )         --2차직업으로 전직해야합니다.
	
		elseif job == 210 or job == 211 or job == 212 or job == 213 or job == 214 or job == 310 or job == 311 or job == 312 or job == 313 or job == 314 or job == 110 or job == 111 or job == 112 or job == 113 or job == 114 then
			if q_flag0 == 1 and q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목 : 선물대여도우미
				dlg_text( "@90993005" )       --이미 선물을 대여 받으셨네요.
			elseif q_flag0 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )		--방어구받기
			
			elseif q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			elseif 	q_flag0 == 0 and q_flag1 == 0 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )		    --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )   --방어구받기
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			end
		end

		
	elseif level >= 100 and level <= 119 then --랭크5무기대여
	    --파이터 등 1차 직업 들은 2차로 전직해라는 문구 출력
	    if job == 201 or job == 202 or job == 203 or job == 301 or job == 302 or job == 303 or job == 101 or job == 102 or job == 103 then
	    dlg_title( "@90993000" )        --제목 : 선물대여도우미
	    dlg_text( "@90993010" )         --2차직업으로 전직해야합니다.
	
		elseif job == 210 or job == 211 or job == 212 or job == 213 or job == 214 or job == 310 or job == 311 or job == 312 or job == 313 or job == 314 or job == 110 or job == 111 or job == 112 or job == 113 or job == 114 then
			if q_flag0 == 1 and q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목 : 선물대여도우미
				dlg_text( "@90993005" )       --이미 선물을 대여 받으셨네요.
			elseif q_flag0 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )		--방어구받기
			
			elseif q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			elseif 	q_flag0 == 0 and q_flag1 == 0 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )		    --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )   --방어구받기
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			end
		end		

		
	elseif level >= 120 and level <= 170 then --랭크6무기대여
	    --파이터 등 1차 직업 들은 2차로 전직해라는 문구 출력
	    if job == 201 or job == 202 or job == 203 or job == 301 or job == 302 or job == 303 or job == 101 or job == 102 or job == 103 then
	    dlg_title( "@90993000" )        --제목 : 선물대여도우미
	    dlg_text( "@90993010" )         --2차직업으로 전직해야합니다.
	
		elseif job == 210 or job == 211 or job == 212 or job == 213 or job == 214 or job == 310 or job == 311 or job == 312 or job == 313 or job == 314 or job == 110 or job == 111 or job == 112 or job == 113 or job == 114 then
			if q_flag0 == 1 and q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목 : 선물대여도우미
				dlg_text( "@90993005" )       --이미 선물을 대여 받으셨네요.
			elseif q_flag0 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )		--방어구받기
			
			elseif q_flag1 == 1 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )       --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			elseif 	q_flag0 == 0 and q_flag1 == 0 then
				-- 다이얼로그 출력
				dlg_title( "@90993000" )      --제목
				dlg_text( "@90993003" )		    --어떤 선물을 받으시겠습니까?
				dlg_menu( "@90010174", 'second_present_armor()' )   --방어구받기
				dlg_menu( "@90010173", 'second_present_weapon()' )  --무기받기
			end
		end
		
		
	end
	dlg_menu( "@90010002", " " )      --대화종료
 	dlg_show()
end	

	
-- 무기 지급
function second_present_weapon()

	-- 현재 직업 id를 받아놓는다.
	local job = get_value( "job" )
	-- 레벨 받아옴
	local level = get_value( "level" )
	
		--캐릭터 레벨은 낮으나 2차직업을 가진 캐릭터들에 대한 예외처리로
		--2차직업도 검색 조건에 포함시킨다.
	
		if level <= 49 then           --랭크2무기대여
			-- 파이터, 나이트, 워리어 직업 체크
			if 	job == 201 or job == 210 or job == 211 then
				insert_item( 3000301, 1 ) --나이츠소드
				set_flag( "q18", "1")	
			
			-- 클레릭, 메이지, 프리스트
			elseif job == 202 or job == 212 or job == 213 then
				insert_item( 3000317, 1 )	--파쇄곤
				set_flag( "q18", "1")	
			
			-- 테이머, 브리더
			elseif job == 203 or job == 214 then
				insert_item( 3000317, 1 )	--파쇄곤
				set_flag( "q18", "1")
			
			-- 스트라이더, 어쌔신, 레인저
			elseif job == 301 or job == 310 or job == 311 then
				insert_item( 3000301, 1 )	
				insert_item( 3000301, 1 )	--나이츠소드2개대여
				insert_item( 3000309, 1 )
				set_flag( "q18", "1")	
			
			-- 매지션, 소서러, 다크매지션
			elseif job == 302 or job == 312 or job == 313 then
				insert_item( 3000321, 1 )	
				set_flag( "q18", "1")	
			
			-- 서모너, 배틀서모너
			elseif job == 303 or job == 314 then
				insert_item( 3000395, 1 )	--아이론대거
				set_flag( "q18", "1")	
			
			-- 투사, 전사, 궁사
			elseif job == 101 or job == 110 or job == 111 then
				insert_item( 3000387, 1 )	
				insert_item( 3000387, 1 )	--아이스 엑스2개대여
				insert_item( 3000305, 1 ) --링클 보우
				set_flag( "q18", "1")	
			
			-- 주술사, 마도사, 마투사
			elseif job == 102 or job == 112 or job == 113 then
				insert_item( 3000396, 1 )	--양날 도끼
				set_flag( "q18", "1")	
			
			-- 야수사, 마수사
			elseif job == 103 or job == 114 then
				insert_item( 3000313, 1 )	
				set_flag( "q18", "1")	

			else
				return
			end
			
		elseif level >= 50 and level <= 79 then --랭크3무기대여
		
		  --아래 1차직업들은 예외사항으로 캐릭터레벨은 높으나
		  --전직을 하지 않은 캐릭터를 위한 예외사항이다.
		  --그 레벨에 맞는 아이템을 전직에 맞게 준다.
		  
			-- 파이터
			if 	job == 201 then
				insert_item( 3000302, 1 ) --파염
				insert_item( 3000397, 1 ) --바스타드소드
				set_flag( "q18", "1")	
			
			-- 클레릭
			elseif job == 202 then
				insert_item( 3000318, 1 )	--쥬얼리메이스
				set_flag( "q18", "1")	
			
			-- 테이머
			elseif job == 203 then
				insert_item( 3000318, 1 )	--쥬얼리메이스
				set_flag( "q18", "1")
			
			-- 스트라이더
			elseif job == 301 then
				insert_item( 3000302, 1 ) --파염
				insert_item( 3000302, 1 ) --파염
				insert_item( 3000310, 1 )	--그레이트보우
				set_flag( "q18", "1")	
			
			-- 매지션
			elseif job == 302 then
				insert_item( 3000322, 1 )	--크리스탈스태프
				set_flag( "q18", "1")	
			
			-- 서모너
			elseif job == 303 then
				insert_item( 3000388, 1 )	--어둠의손길
				set_flag( "q18", "1")	
			
			-- 투사
			elseif job == 101 then
				insert_item( 3000389, 1 ) --블러디엑스
				insert_item( 3000389, 1 ) --블러디엑스
				insert_item( 3000306, 1 )	--레인져보우
				set_flag( "q18", "1")	
			
			-- 주술사
			elseif job == 102 then
				insert_item( 3000390, 1 )	--자이언트엑스
				set_flag( "q18", "1")	
			
			-- 야수사
			elseif job == 103 then
				insert_item( 3000389, 1 ) --블러디엑스
				insert_item( 3000389, 1 ) --블러디엑스
				set_flag( "q18", "1")	
		
		
		  --정상적인 전직 진행을 한 유저들은 아래와 같이
		  --2차전직한 직업을 가지고 있다.
		  
		  
			-- 나이트
			elseif 	job == 210 then
				insert_item( 3000302, 1 )	          --파염
				set_flag( "q18", "1")	


      -- 워리어
			elseif job == 211 then
				insert_item( 3000397, 1 )	          --바스타드소드
				set_flag( "q18", "1")	


			-- 메이지
			elseif job == 212 then
				insert_item( 3000318, 1 )	          --쥬얼리메이스
				set_flag( "q18", "1")	


      -- 프리스트
			elseif job == 213 then
				insert_item( 3000318, 1 )	          --쥬얼리메이스
				set_flag( "q18", "1")	


			-- 브리더
			elseif job == 214 then
				insert_item( 3000318, 1 )	          --쥬얼리메이스
				set_flag( "q18", "1")	


			-- 어쌔신
			elseif job == 310 then
				insert_item( 3000302, 1 )	          
				insert_item( 3000302, 1 )	          --파염2개대여
				set_flag( "q18", "1")	

			-- 레인저
			elseif job == 311 then
				insert_item( 3000310, 1 )	          --그레이트보우
				set_flag( "q18", "1")	
				
			-- 소서러
			elseif job == 312 then
				insert_item( 3000322, 1 )	          --크리스탈스태프
				set_flag( "q18", "1")	


			-- 다크매지션
			elseif job == 313 then
				insert_item( 3000322, 1 )				    
				set_flag( "q18", "1")	


			-- 배틀서모너
			elseif job == 314 then
				insert_item( 3000388, 1 )	          --어둠의손길
				set_flag( "q18", "1")	


			-- 전사
			elseif job == 110 then
				insert_item( 3000389, 1 )          	
				insert_item( 3000389, 1 )          	--블러디엑스2개대여
				set_flag( "q18", "1")	
				
				
			-- 궁사
			elseif job == 111 then
				insert_item( 3000306, 1 )				    --레인져보우
				set_flag( "q18", "1")	


			-- 마도사
			elseif job == 112 then
				insert_item( 3000390, 1 )				    --자이언트엑스
				set_flag( "q18", "1")	


			-- 마투사
			elseif job == 113 then
				insert_item( 3000390, 1 )				     --자이언트엑스
				set_flag( "q18", "1")	


			-- 마수사
			elseif job == 114 then
				insert_item( 3000389, 1 )	          
				insert_item( 3000389, 1 )	          --블러디엑스2개대여
				set_flag( "q18", "1")
					
			else
				return
			end
			
		elseif level >= 80 and level <= 99 then --랭크4무기대여
		
			-- 나이트
			if 	job == 210 then
				insert_item( 3000303, 1 )				    --소브오브빅토리
				set_flag( "q18", "1")	


			-- 워리어
			elseif job == 211 then
				insert_item( 3000398, 1 )				    --익스큐터너
				set_flag( "q18", "1")	


			-- 메이지, 프리스트
			elseif job == 212 or job == 213 then
				insert_item( 3000319, 1 )				    --해머
				set_flag( "q18", "1")	


			-- 브리더
			elseif job == 214 then
				insert_item( 3000319, 1 )				    --해머
				set_flag( "q18", "1")	

			-- 어쌔신
			elseif job == 310 then
				insert_item( 3000303, 1 )	          
				insert_item( 3000303, 1 )	          --소드오브빅토리2개대여
				set_flag( "q18", "1")	
	

			-- 레인저
			elseif job == 311 then
				insert_item( 3000311, 1 )				    --마크보우
				set_flag( "q18", "1")		

	
			-- 소서러, 다크매지션
			elseif job == 312 or job == 313 then
				insert_item( 3000323, 1 )				    --라운드스태프
				set_flag( "q18", "1")


			-- 배틀서모너
			elseif job == 314 then
				insert_item( 3000391, 1 )	          --쏘우
				set_flag( "q18", "1")


			-- 전사
			elseif job == 110 then
				insert_item( 3000392, 1 )	          
  			insert_item( 3000392, 1 )	          --썬더엑스2개대여
				set_flag( "q18", "1")	

				
			-- 궁사
			elseif job == 111 then
				insert_item( 3000307, 1 )	          --자이언트보우
				set_flag( "q18", "1")	
				
				
			-- 마도사, 마투사
			elseif job == 112 or job == 113 then
				insert_item( 3000315, 1 )				    --그레이트액스
				set_flag( "q18", "1")	


			-- 마수사
			elseif job == 114 then
				insert_item( 3000392, 1 )				    
				insert_item( 3000392, 1 )				    --썬더액스2개대여
				set_flag( "q18", "1")	
			else
				return
			end
			
		elseif level >= 100 and level <= 119 then --랭크5무기대여
		
			-- 나이트
			if 	job == 210 then
				insert_item( 3000304, 1 )				      --베오울프
				set_flag( "q18", "1")	


			-- 워리어
			elseif job == 211 then
				insert_item( 3000399, 1 )				      --일룬
				set_flag( "q18", "1")	


			-- 메이지, 프리스트
			elseif job == 212 or job == 213 then
				insert_item( 3000320, 1 )				      --메테오로이달커젤
				set_flag( "q18", "1")	


			-- 브리더
			elseif job == 214 then
				insert_item( 3000320, 1 )				      --메테오로이달커젤
				set_flag( "q18", "1")	

			-- 어쌔신
			elseif job == 310 then
				insert_item( 3000304, 1 )				      
				insert_item( 3000304, 1 )				      --베오울프2개대여
				set_flag( "q18", "1")	
	

			-- 레인저
			elseif job == 311 then
				insert_item( 3000312, 1 )				      --그랜드크로스보우
				set_flag( "q18", "1")		

	
			-- 소서러, 다크매지션
			elseif job == 312 or job == 313 then
				insert_item( 3000324, 1 )				      --스태프오브세이지
				set_flag( "q18", "1")


			-- 배틀서모너
			elseif job == 314 then
				insert_item( 3000393, 1 )				      --트위스트대거
				set_flag( "q18", "1")


			-- 전사
			elseif job == 110 then
				insert_item( 3000394, 1 )				      
				insert_item( 3000394, 1 )				      --질풍의도끼2개대여
				set_flag( "q18", "1")	

				
			-- 궁사
			elseif job == 111 then
				insert_item( 3000308, 1 )				      --크리티컬보우
				set_flag( "q18", "1")	
				
				
			-- 마도사, 마투사
			elseif job == 112 or job == 113 then
				insert_item( 3000316, 1 )				      --토네이도오브소울
				set_flag( "q18", "1")	


			-- 마수사
			elseif job == 114 then
				insert_item( 3000394, 1 )				      
				insert_item( 3000394, 1 )				      --질풍의도끼2개대여
				set_flag( "q18", "1")	
			else
				return
			end			
			
		elseif level >= 120 and level <= 170 then --랭크6무기대여
		
			-- 나이트
			if 	job == 210 then
				insert_item( 3000401, 1 )				      --프라가라흐
				set_flag( "q18", "1")	


			-- 워리어
			elseif job == 211 then
				insert_item( 3000407, 1 )				      --사인검
				set_flag( "q18", "1")	


			-- 메이지, 프리스트
			elseif job == 212 or job == 213 then
				insert_item( 3000409, 1 )				      --살가타너스
				set_flag( "q18", "1")	


			-- 브리더
			elseif job == 214 then
				insert_item( 3000409, 1 )				      --살가타너스
				set_flag( "q18", "1")	

			-- 어쌔신
			elseif job == 310 then
				insert_item( 3000401, 1 )				      
				insert_item( 3000401, 1 )				      --프라가라흐2개대여
				set_flag( "q18", "1")	
	

			-- 레인저
			elseif job == 311 then
				insert_item( 3000412, 1 )				      --파멸의눈
				set_flag( "q18", "1")		

	
			-- 소서러, 다크매지션
			elseif job == 312 or job == 313 then
				insert_item( 3000413, 1 )				      --용의절규
				set_flag( "q18", "1")


			-- 배틀서모너
			elseif job == 314 then
				insert_item( 3000414, 1 )				      --오리할콘
				set_flag( "q18", "1")


			-- 전사
			elseif job == 110 then
				insert_item( 3000415, 1 )				      
				insert_item( 3000415, 1 )				      --사신의도끼2개대여
				set_flag( "q18", "1")	

				
			-- 궁사
			elseif job == 111 then
				insert_item( 3000416, 1 )				      --바일런트게일
				set_flag( "q18", "1")	
				
				
			-- 마도사, 마투사
			elseif job == 112 or job == 113 then
				insert_item( 3000417, 1 )				      --굉뢰
				set_flag( "q18", "1")	


			-- 마수사
			elseif job == 114 then
				insert_item( 3000415, 1 )				      
				insert_item( 3000415, 1 )				      --사신의도끼2개대여
				set_flag( "q18", "1")	
			else
				return
			end			
			
		end			

	-- 다이얼로그 출력
	dlg_title( "@90993000" )
	dlg_text( "@90993007" )
	-- 무기 지급 받았음)
	set_flag( "q18", "1")	
	dlg_menu( "@90010002", " " )
	dlg_show()
end					


--데바
--디바인솔져	201
--클레릭		203
--브리더		204

--아수라
--스트라이더	301
--다크매지션	303
--소서러		304

--가이아
--투사			101
--주술사		103
--마수사		104


-- 방어구 지급
function second_present_armor()

	-- 현재 직업 id를 받아놓는다.
	local job = get_value( "job" )
	-- 레벨 받아옴
	local level = get_value( "level" )
	
		if level <= 49 then                 --랭크2방어구대여
		--캐릭터 레벨은 낮으나 2차직업을 가진 캐릭터들에 대한 예외처리로
		--2차직업도 검색 조건에 포함시킨다.
			-- 파이터, 나이트, 워리어
			if 	job == 201 or job == 210 or job == 211 then
				insert_item( 3000327, 1 )				--화이트메일
				insert_item( 3000375, 1 )       --와일드레더슈즈
				insert_item( 3000379, 1 )       --와일드레더글러브
				insert_item( 3000367, 1 )       --헤비쉴드
				insert_item( 3000383, 1 )       --우든헬멧
				insert_item( 3000371, 1 )       --라이트벨트
				set_flag( "q19", "1")	
			
			-- 클레릭, 메이지, 프리스트
			elseif job == 202 or job == 212 or job == 213 then
				insert_item( 3000345, 1 )	      --화이트로브
				insert_item( 3000375, 1 )
				insert_item( 3000379, 1 )
				insert_item( 3000383, 1 )
				insert_item( 3000371, 1 )
				set_flag( "q19", "1")	
			
			-- 테이머, 브리더
			elseif job == 203 or job == 214 then
				insert_item( 3000356, 1 )	      --실버코트
				insert_item( 3000375, 1 )
				insert_item( 3000379, 1 )
				insert_item( 3000383, 1 )
				insert_item( 3000371, 1 )
				set_flag( "q19", "1")	
			
			-- 스트라이더, 어쌔신, 레인저
			elseif job == 301 or job == 310 or job == 311 then
				insert_item( 3000325, 1 )	      --라이트클로스
				insert_item( 3000375, 1 )
				insert_item( 3000379, 1 )
				insert_item( 3000383, 1 )
				insert_item( 3000371, 1 )
				set_flag( "q19", "1")	
			
			-- 매지션, 소서러, 다크매지션
			elseif job == 302 or job == 312 or job == 313 then
				insert_item( 3000344, 1 )	      --아수라로브
				insert_item( 3000375, 1 )
				insert_item( 3000379, 1 )
				insert_item( 3000383, 1 )
				insert_item( 3000371, 1 )
				set_flag( "q19", "1")	
			
			-- 서모너, 배틀서모너
			elseif job == 303 or job == 314 then
				insert_item( 3000357, 1 )	      --아수라코트
				insert_item( 3000375, 1 )
				insert_item( 3000379, 1 )
				insert_item( 3000367, 1 )       --헤비쉴드
				insert_item( 3000383, 1 )
				insert_item( 3000371, 1 )
				set_flag( "q19", "1")	
			
			-- 투사, 전사, 궁사
			elseif job == 101 or job == 110 or job == 111 then
				insert_item( 3000326, 1 )				--심혼의갑옷
				insert_item( 3000375, 1 )
				insert_item( 3000379, 1 )
				insert_item( 3000383, 1 )
				insert_item( 3000371, 1 )
				set_flag( "q19", "1")	
			
			-- 주술사, 마도사, 마투사
			elseif job == 102 or job == 112 or job == 113 then
				insert_item( 3000343, 1 )	      --심혼의법의
				insert_item( 3000375, 1 )
				insert_item( 3000379, 1 )
				insert_item( 3000383, 1 )
				insert_item( 3000371, 1 )
				set_flag( "q19", "1")	
			
			-- 야수사, 마수사
			elseif job == 103 or job == 114 then
				insert_item( 3000355, 1 )	       --가죽외투
				insert_item( 3000375, 1 )
				insert_item( 3000379, 1 )
				insert_item( 3000383, 1 )
				insert_item( 3000371, 1 )
				set_flag( "q19", "1")	
			else
				return
			end
			
		elseif level >= 50 and level <= 79 then   --랭크3방어구대여
		
		--무기와 마찬가지로 캐릭터 레벨은 높으나
		--2차직업으로 전직하지 않은 유저를 위한 예외 처리이다.
		
		
			-- 파이터
			if 	job == 201 then
				insert_item( 3000329, 1 )				      --플래티넘아머
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000368, 1 )             --강철의가호
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
			
			-- 클레릭
			elseif job == 202 then
				insert_item( 3000347, 1 )				      --플래티넘로브
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
			
			-- 테이머
			elseif job == 203 then
				insert_item( 3000358, 1 )				      --플래티넘코트
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
			
			-- 스트라이더
			elseif job == 301 then
				insert_item( 3000330, 1 )				      --헬리쉬아머
				insert_item( 3000338, 1 )				      --헬리쉬타이즈
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
			
			-- 매지션
			elseif job == 302 then
				insert_item( 3000348, 1 )				      --헬리쉬가브
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
			
			-- 서모너
			elseif job == 303 then
				insert_item( 3000359, 1 )				      --헬리쉬코트
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000368, 1 )             --강철의가호
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
			
			-- 투사
			elseif job == 101 then
				insert_item( 3000328, 1 )				      --맹세의갑옷
				insert_item( 3000337, 1 )				      --맹세의수렵복
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
			
			-- 주술사
			elseif job == 102 then
				insert_item( 3000346, 1 )				      --대자연의법의
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
			
			-- 야수사
			elseif job == 103 then
				insert_item( 3000360, 1 )				      --환술의사포
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	
		
		
		--아래 직업은 2차전직을 한 상태의 유저들에게
		--적용하는 아이템들이다. 내용은 1차전직을 한 캐릭터와 동일하다.
		
			-- 나이트
			elseif job == 210 then
				insert_item( 3000329, 1 )				      --플래티넘아머
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000368, 1 )             --강철의가호
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	


			-- 워리어
			elseif job == 211 then
				insert_item( 3000329, 1 )				      --플래티넘아머
				insert_item( 3000376, 1 )             --실키즈슈즈
				insert_item( 3000380, 1 )             --실키즈글러브
				insert_item( 3000384, 1 )             --본헬멧
				insert_item( 3000372, 1 )             --그레이트벨트
				set_flag( "q19", "1")	


			-- 메이지, 프리스트
			elseif job == 212 or job == 213 then
				insert_item( 3000347, 1 )				      --플래티넘로브
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	


			-- 브리더
			elseif job == 214 then
				insert_item( 3000358, 1 )				      --플래티넘코트
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	


			-- 어쌔신
			elseif job == 310 then
				insert_item( 3000330, 1 )				      --헬리쉬아머
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	

			-- 레인저
			elseif job == 311 then
				insert_item( 3000338, 1 )				      --헬리쉬타이즈
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	
				
			-- 소서러, 다크매지션
			elseif job == 312 or job == 313 then
				insert_item( 3000348, 1 )				      --헬리쉬가브
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	


			-- 배틀서모너
			elseif job == 314 then
				insert_item( 3000359, 1 )				      --헬리쉬코트
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000368, 1 )             --강철의가호
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	


			-- 전사
			elseif job == 110 then
				insert_item( 3000328, 1 )				      --맹세의갑옷
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	
				
				
			-- 궁사
			elseif job == 111 then
				insert_item( 3000337, 1 )				      --맹세의수렵복
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	


			-- 마도사, 마투사
			elseif job == 112 or job == 113 then
				insert_item( 3000346, 1 )				      --대자연의법의
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	


			-- 마수사
			elseif job == 114 then
				insert_item( 3000360, 1 )				      --환술의사포
				insert_item( 3000376, 1 )
				insert_item( 3000380, 1 )
				insert_item( 3000384, 1 )
				insert_item( 3000372, 1 )
				set_flag( "q19", "1")	
			else
				return
			end
			
		elseif level >= 80 and level <= 99 then    --랭크4방어구대여
		
			-- 나이트
			if 	job == 210 then
				insert_item( 3000333, 1 )				        --홀리아머
				insert_item( 3000377, 1 )               --스트라이더부츠
				insert_item( 3000381, 1 )               --스트라이더글러브
				insert_item( 3000369, 1 )               --엑셀런트쉴드
				insert_item( 3000385, 1 )               --엑셀런트헬름
				insert_item( 3000373, 1 )               --엑셀런트벨트
				set_flag( "q19", "1")	


			-- 워리어
			elseif job == 211 then
				insert_item( 3000333, 1 )				        --홀리아머
				insert_item( 3000377, 1 )               --스트라이더부츠
				insert_item( 3000381, 1 )               --스트라이더글러브
				insert_item( 3000385, 1 )               --엑셀런트헬름
				insert_item( 3000373, 1 )               --엑셀런트벨트
				set_flag( "q19", "1")	


			-- 메이지, 프리스트
			elseif job == 212 or job == 213 then
				insert_item( 3000351, 1 )				        --홀리로브
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	


			-- 브리더
			elseif job == 214 then
				insert_item( 3000363, 1 )				        --홀리코트
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	

			-- 어쌔신
			elseif job == 310 then
				insert_item( 3000331, 1 )				        --섀도우메일
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	

			-- 레인저
			elseif job == 311 then
				insert_item( 3000339, 1 )				        --섀도우타이즈
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	

	
			-- 소서러, 다크매지션
			elseif job == 312 or job == 313 then
				insert_item( 3000349, 1 )				        --섀도우가브
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	


			-- 배틀서모너
			elseif job == 314 then
				insert_item( 3000361, 1 )				        --섀도우코트
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000369, 1 )               --엑셀런트쉴드
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	


			-- 전사
			elseif job == 110 then
				insert_item( 3000332, 1 )				        --쌍두룡갑
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	

				
			-- 궁사
			elseif job == 111 then
				insert_item( 3000340, 1 )				        --쌍듀룡의비늘
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	

			-- 마도사, 마투사
			elseif job == 112 or job == 113 then
				insert_item( 3000350, 1 )				        --현자의법의
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	


			-- 마수사
			elseif job == 114 then
				insert_item( 3000362, 1 )				        --신비주술사의외투
				insert_item( 3000377, 1 )
				insert_item( 3000381, 1 )
				insert_item( 3000385, 1 )
				insert_item( 3000373, 1 )
				set_flag( "q19", "1")	
			else
				return
			end		
				
		elseif level >= 100 and level <= 119 then   --랭크5방어구대여
		
			-- 나이트
			if 	job == 210 then
				insert_item( 3000334, 1 )				        --사룡의갑주
				insert_item( 3000378, 1 )               --루나레이드부츠
				insert_item( 3000382, 1 )               --루나레이드글러브
				insert_item( 3000370, 1 )               --에인션트쉴드
				insert_item( 3000386, 1 )               --에인션트헬름
				insert_item( 3000374, 1 )               --에인션트벨트
				set_flag( "q19", "1")	


			-- 워리어
			elseif job == 211 then
				insert_item( 3000334, 1 )				        --사룡의갑주
				insert_item( 3000378, 1 )               --루나레이드부츠
				insert_item( 3000382, 1 )               --루나레이드글러브
				insert_item( 3000386, 1 )               --에인션트헬름
				insert_item( 3000374, 1 )               --에인션트벨트
				set_flag( "q19", "1")	


			-- 메이지, 프리스트
			elseif job == 212 or job == 213 then
				insert_item( 3000352, 1 )				        --강철사신의로브
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	


			-- 브리더
			elseif job == 214 then
				insert_item( 3000365, 1 )				        --오리엔탈소울아머
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	

			-- 어쌔신
			elseif job == 310 then
				insert_item( 3000334, 1 )				        --사룡의갑주
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	

			-- 레인저
			elseif job == 311 then
				insert_item( 3000341, 1 )				        --용각의깃
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	

	
			-- 소서러, 다크매지션
			elseif job == 312 or job == 313 then
				insert_item( 3000352, 1 )				        --강철사신의로브
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	


			-- 배틀서모너
			elseif job == 314 then
				insert_item( 3000365, 1 )				        --오리엔탈소울아머
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000370, 1 )               --에인션트쉴드
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	


			-- 전사
			elseif job == 110 then
				insert_item( 3000334, 1 )				        --사룡의갑주
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	

				
			-- 궁수
			elseif job == 111 then
				insert_item( 3000341, 1 )				        --용각의깃
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	

			-- 마도사, 마투사
			elseif job == 112 or job == 113 then
				insert_item( 3000352, 1 )				        --강철사신의로브
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	


			-- 마수사
			elseif job == 114 then
				insert_item( 3000365, 1 )				        --오리엔탈소울아머
				insert_item( 3000378, 1 )
				insert_item( 3000382, 1 )
				insert_item( 3000386, 1 )
				insert_item( 3000374, 1 )
				set_flag( "q19", "1")	
			else
				return
			end
			
		elseif level >= 120 and level <= 170 then   --랭크6방어구대여
		
			-- 나이트
			if 	job == 210 then
				insert_item( 3000400, 1 )				        --슈터블포트리스
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000404, 1 )               --윈드크러스트
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	


			-- 워리어
			elseif job == 211 then
				insert_item( 3000400, 1 )				        --슈터블포트리스
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	


			-- 메이지, 프리스트
			elseif job == 212 or job == 213 then
				insert_item( 3000408, 1 )				        --거절의도포
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	


			-- 브리더
			elseif job == 214 then
				insert_item( 3000410, 1 )				        --복종의포스아머
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	

			-- 어쌔신
			elseif job == 310 then
				insert_item( 3000400, 1 )				        --슈터블포트리스
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	

			-- 레인저
			elseif job == 311 then
				insert_item( 3000411, 1 )				        --슈츠오브헬리오스
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	

	
			-- 소서러, 다크매지션
			elseif job == 312 or job == 313 then
				insert_item( 3000408, 1 )				        --거절의도포
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	


			-- 배틀서모너
			elseif job == 314 then
				insert_item( 3000410, 1 )				        --복종의포스아머
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000404, 1 )               --윈드크러스트
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	


			-- 전사
			elseif job == 110 then
				insert_item( 3000400, 1 )				        --슈터블포트리스
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	

				
			-- 궁사
			elseif job == 111 then
				insert_item( 3000411, 1 )				        --슈츠오브헬리오스
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	

			-- 마도사, 마투사
			elseif job == 112 or job == 113 then
				insert_item( 3000408, 1 )				        --거절의도포
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	


			-- 마수사
			elseif job == 114 then
				insert_item( 3000410, 1 )				        --복종의포스아머
				insert_item( 3000402, 1 )               --제니셔리언부츠
				insert_item( 3000403, 1 )               --제니셔리언글러브
				insert_item( 3000405, 1 )               --가들리헬름
				insert_item( 3000406, 1 )               --가들리벨트
				set_flag( "q19", "1")	
			else
				return
			end
			
		end

	-- 다이얼로그 출력
	dlg_title( "@90993000" )
	dlg_text( "@90993008" )
	
	-- 무기 지급 받았음)
	set_flag( "q19", "1")	
	dlg_menu( "@90010002", " " )
	dlg_show()
end	
			
	
   --============================================================
   --===============	레벨 업 도우미
   --============================================================
   -- 캐릭터가 레벨업 했을 때 보상을 주는 이벤트
   -- 2009.03.03 현재 일본에는 항상 진행중입니다.

      
	  
function get_reward_item_of_level_up( achieve_level )

	-- 국가 코드(2009.03.03 현재)
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_Germany                    = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
		--LOCAL_INFO_France                     = 128
		--LOCAL_INFO_Russia                     = 256
		--LOCAL_INFO_Malaysia                   = 512
		--LOCAL_INFO_Singapore                  = 1024
		--LOCAL_INFO_Vietnam                   	= 2048
		--LOCAL_INFO_Thailand                  	= 4096
		--LOCAL_INFO_Middle East              	= 8192


	local state_code = get_local_info() 
	
	-- 국가코드 16 (일본), 국가코드 32(대만)
	if state_code == 16 or state_code == 32 then
		
		if achieve_level == 10 then
			return 2902141 -- 10레벨 보상 상자 아이템 코드
		elseif achieve_level == 20 then
			return 2902142 -- 20레벨 보상 상자 아이템 코드
		elseif achieve_level == 30 then
			return 2902143 -- 30레벨 보상 상자 아이템 코드
		elseif achieve_level == 40 then
			return 2902144 -- 40레벨 보상 상자 아이템 코드
		elseif achieve_level == 50 then
			return 2902145 -- 50레벨 보상 상자 아이템 코드
		end
			
	else
	
		--국가코드 8(독일), 128(프랑스) 즉, 유럽국가들		
		if state_code == 8 or state_code == 128 then
			
			if achieve_level == 10 then
				return 2902146 -- 10 레벨 업 선물 상자<2009유럽레벨업이벤트>
			elseif achieve_level == 20 then
				return 2902147 -- 20 레벨 업 선물 상자<2009유럽레벨업이벤트>
			elseif achieve_level == 30 then
				return 2902148 -- 30 레벨 업 선물 상자<2009유럽레벨업이벤트>
			elseif achieve_level == 40 then
				return 2902149 -- 40 레벨 업 선물 상자<2009유럽레벨업이벤트>
			elseif achieve_level == 50 then
				return 2902150 -- 50 레벨 업 선물 상자<2009유럽레벨업이벤트>
			end
			
	else
		return 0
		
		end
	end
end

   
function NPC_Second_level_up_contact()

-- 국가 코드(2009.03.03 현재)
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_Germany                    = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
		--LOCAL_INFO_France                     = 128
		--LOCAL_INFO_Russia                     = 256
		--LOCAL_INFO_Malaysia                   = 512
		--LOCAL_INFO_Singapore                  = 1024
		--LOCAL_INFO_Vietnam                   	= 2048
		--LOCAL_INFO_Thailand                  	= 4096
		--LOCAL_INFO_Middle East              	= 8192
		
	local state_code = get_local_info() 
	
	-- 국가코드 16 (일본), 국가코드 32(대만)
	if state_code == 16 or state_code == 32 then
	
		-- 다이얼로그 출력
		dlg_title( "@90993400" )
		dlg_text( "@90993402" )
		dlg_menu( "@90010175", "Second_level_up()" )
		
	else
	
		-- 국가코드 8 (독일), 국가코드 128 (프랑스)
		
		if state_code == 8 or state_code == 128 then
		
			-- 다이얼로그 출력
			dlg_title( "@90993400" )
			dlg_text( "@90993402" )
			dlg_menu( "@90010175", "Second_level_up()" )
		end
		
	end
	
	dlg_menu( "@90010002", " " )
	dlg_show()
end


-- 기존 레벨 저장된 상태
function Second_level_up()
	local level = get_value( "level" )
	local max_achieved_level = get_flag( "q21" )
	local is_quest_started = get_flag( "q22" )
	
	-- 국가 코드(2009.03.03 현재)
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_Germany                    = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
		--LOCAL_INFO_France                     = 128
		--LOCAL_INFO_Russia                     = 256
		--LOCAL_INFO_Malaysia                   = 512
		--LOCAL_INFO_Singapore                  = 1024
		--LOCAL_INFO_Vietnam                   	= 2048
		--LOCAL_INFO_Thailand                  	= 4096
		--LOCAL_INFO_Middle East              	= 8192
	local state_code = get_local_info() 
	
	--국가코드 16 (일본), 국가코드 32(대만)
	
	if state_code == 16 or state_code == 32 then

		if max_achieved_level == "" then
			max_achieved_level = 0
		end
		if is_quest_started == "" then
			is_quest_started = 0
		end

		local current_achieve_level = ( math.floor( level / 10 ) + 1 ) * 10

		-- 대화창 제목(npc 이름) 설정
		dlg_title( "@90993400" )

		-- 퀘스트 진행 중이 아닐 경우
		if is_quest_started == 0 then
			-- 지금까지 달성했던(퀘스트로) 최대 레벨보다 현재 달성 목표 레벨이 높다면 퀘스트 시작 가능
			if current_achieve_level > max_achieved_level then
				-- 퀘스트 시작 가능 안내(보상은 current_achieve_level에 따라서)
				if current_achieve_level == 10 then
					dlg_text( "@90993409" )	-- 10 레벨 보상 안내 및 10레벨 달성하셈

				elseif current_achieve_level == 20 then
					dlg_text( "@90993410" )	-- 20 레벨 보상 안내 및 20레벨 달성하셈

				elseif current_achieve_level == 30 then
					dlg_text( "@90993411" )	-- 30 레벨 보상 안내 및 30레벨 달성하셈

				elseif current_achieve_level == 40 then
					dlg_text( "@90993412" )	-- 40 레벨 보상 안내 및 40레벨 달성하셈

				elseif current_achieve_level == 50 then
					dlg_text( "@90993413" )	-- 50 레벨 보상 안내 및 50레벨 달성하셈

				else
					dlg_text( "@90993408" )	-- 이벤트 대상 레벨이 아님
								
					dlg_menu( "@90010002", " " )
					dlg_show()
					return
				end
				
				-- 퀘스트 시작 상태 저장
				set_flag( "q21", current_achieve_level )
				set_flag( "q22", "1" )
			else
				-- 퀘스트를 이미 했거나 렙다한 경우
				dlg_text( "@90993406" )	-- 현재 레벨대 달성 보상을 이미 받으셨네요. 다음 렙대로 업하고 오든가 집에 가셈
			end

		-- 퀘스트 진행 중인 경우
		else
			-- 목표 달성 레벨보다 현재 레벨이 높으면 완료
			if max_achieved_level <= level then
				insert_item( get_reward_item_of_level_up( max_achieved_level ), 1 )	-- 보상 상자 지급
				-- 퀘스트 상태 저장
				set_flag( "q22", "0" )
				dlg_text( "@90993404" )	-- 추카추카 선물 받으셨삼
				dlg_menu( "@90010175", "Second_level_up()" )	-- 다시 퀘스트 받기 체크
			else
				-- 목표 레벨보다 현재 레벨이 낮음
				dlg_text( "@90993405" )	-- 아직 수련이 부족하구나~ 더 업하고 오거라
			end
		end
		
	else
		-- 국가코드 8 (독일) 국가코드 128 (프랑스)
		
		if state_code == 8 or state_code == 128 then
		
			if max_achieved_level == "" then
				max_achieved_level = 0
			end
			if is_quest_started == "" then
				is_quest_started = 0
			end
			local current_achieve_level = ( math.floor( level / 10 ) + 1 ) * 10

			-- 대화창 제목(npc 이름) 설정
			dlg_title( "@90993400" )

			-- 퀘스트 진행 중이 아닐 경우
			if is_quest_started == 0 then
				-- 지금까지 달성했던(퀘스트로) 최대 레벨보다 현재 달성 목표 레벨이 높다면 퀘스트 시작 가능
				if current_achieve_level > max_achieved_level then
					-- 퀘스트 시작 가능 안내(보상은 current_achieve_level에 따라서)
					if current_achieve_level == 10 then
						dlg_text( "@90993409" )	-- 10 레벨 보상 안내 및 10레벨 달성하셈
						
						elseif current_achieve_level == 20 then
						dlg_text( "@90993410" )	-- 20 레벨 보상 안내 및 20레벨 달성하셈
						
						elseif current_achieve_level == 30 then
						dlg_text( "@90993411" )	-- 30 레벨 보상 안내 및 30레벨 달성하셈
						
						elseif current_achieve_level == 40 then
						dlg_text( "@90993412" )	-- 40 레벨 보상 안내 및 40레벨 달성하셈
						
						elseif current_achieve_level == 50 then
						dlg_text( "@90993413" )	-- 50 레벨 보상 안내 및 50레벨 달성하셈
						
						else
						dlg_text( "@90993408" )	-- 이벤트 대상 레벨이 아님
									
						dlg_menu( "@90010002", " " )
						dlg_show()
						return
					end
					
					-- 퀘스트 시작 상태 저장
					set_flag( "q21", current_achieve_level )
					set_flag( "q22", "1" )
				else
					-- 퀘스트를 이미 했거나 렙다한 경우
					dlg_text( "@90993406" )	-- 현재 레벨대 달성 보상을 이미 받으셨네요. 다음 렙대로 업하고 오든가 집에 가셈
				end
				
			-- 퀘스트 진행 중인 경우
			else
				-- 목표 달성 레벨보다 현재 레벨이 높으면 완료
				if max_achieved_level <= level then
					insert_item( get_reward_item_of_level_up( max_achieved_level ), 1 )	-- 보상 상자 지급
					-- 퀘스트 상태 저장
					set_flag( "q22", "0" )
					dlg_text( "@90993404" )	-- 추카추카 선물 받으셨삼
					dlg_menu( "@90010175", "Second_level_up()" )	-- 다시 퀘스트 받기 체크
				else
					-- 목표 레벨보다 현재 레벨이 낮음
					dlg_text( "@90993405" )	-- 아직 수련이 부족하구나~ 더 업하고 오거라
				end
			end
		end
	end

	dlg_menu( "@90010002", " " )	
	dlg_show()
end




----------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------
-------------------------------------흑 송 미 녀 이 벤 트 ------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------


function NPC_darkwood_lady_contact()

	-- 다이얼로그 출력
	dlg_title( "@90993801" )
	dlg_text( "@90408502" )
	dlg_menu( "@90993803", "darkwood_event_1()" )

--	안 팔린다고 아래 두 메뉴는 사용 안함.
--	dlg_menu( "@90993804", "open_market( 'darkwood_event_etc' )" )
--	dlg_menu( "@90993805", "darkwood_event_2()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function darkwood_event_1()
   
	if get_value( "level" ) >= 15 then
		open_popup("game.taiwan_darkwood_event_url", 1,0)
	else
	dlg_title( "@90993801" )
	dlg_text( "@90993810" )
	end
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function darkwood_event_2()
	-- 다이얼로그 출력
	dlg_title( "@90993801" )
	dlg_text( "@90993802" )
	dlg_menu( "@90993806", "darkwood_point()" )
	dlg_menu( "@90010003", " NPC_darkwood_lady_contact() " )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function darkwood_point()

	local count1 = find_item( 2010262 )
	local count2 = find_item( 2010263 )
	local count3 = find_item( 2010264 )
	local count4 = find_item( 2010265 )
	local count5 = find_item( 2010266 )
	
	if count1 >= 1 and count2 >= 1 and count3 >= 1 and count4 >= 1 and count5 >= 1 then
	dlg_title( "@90993801" )
	dlg_text( "@90993807" )	
	insert_item( 2010272, 1 )
	delete_item( get_item_handle( 2010262), 1 ) 
	delete_item( get_item_handle( 2010263), 1 ) 
	delete_item( get_item_handle( 2010264), 1 ) 
	delete_item( get_item_handle( 2010265), 1 ) 
	delete_item( get_item_handle( 2010266), 1 ) 
	else
	dlg_title( "@90993801" )
	dlg_text( "@90993808" )	
	end
	dlg_menu( "@90010003", " darkwood_event_2() " )
	dlg_menu( "@90010002", " " )
	dlg_show()
end





----------------------------------------------------------------------------
----------------------------------------------------------------------------
------------------- 골든 위크 이벤트 npc (기존 단술/벚꽃 이벤트)------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------


function NPC_golden_event_init()

	cprint( "!찰떡 도매상" )
	set_npc_name(  "@90996900"  )
end



function NPC_golden_event_contact()

	-- 다이얼로그 출력
	dlg_title( "@90996901" )
	dlg_text( "@90996902" )

	-- 당신은 누구죠?
	dlg_menu( "@90996907", 'golden_event_who_are_you()' )			
	
	-- 무엇을 도와드릴까요?
	dlg_menu( "@90996908", 'golden_event_question()' )			
	
	-- 찰떡 주문서 교환
	dlg_menu( "@90996909", 'golden_event_exchange()' )			
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end



-- 당신은 누구?
function golden_event_who_are_you()
	-- 다이얼로그 출력
	dlg_title( "@90996901" )
	dlg_text( "@90996903" )
	--돌아가기
	dlg_menu( "@90010003", 'NPC_golden_event_contact()' )	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end
	
	

-- 무엇을 도와드릴까요?
function golden_event_question()
	-- 다이얼로그 출력
	dlg_title( "@90996901" )
	dlg_text( "@90996904" )

	--돌아가기
	dlg_menu( "@90010003", 'NPC_golden_event_contact()' )	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end

	
-- 찰떡 교환
function golden_event_exchange()


	local item_count = find_item ( 2000258 )
	item_handle = get_item_handle ( 2000258 )
	
	if item_count >= 50 then
		-- 찰떡 주문서 교환 성공시(6개의 정제된 조각 지급)
		dlg_title( "@90996901" )
		dlg_text( "@90996905" )
	    delete_item ( item_handle, 50 )
    	insert_item ( 2600001, 1 )
		insert_item ( 2600002, 1 )
		insert_item ( 2600003, 1 )
		insert_item ( 2600004, 1 )
		insert_item ( 2600005, 1 )
		insert_item ( 2600006, 1 )
	else
		-- 찰떡 주문서 모자랄 경우
		dlg_title( "@90996901" )
		dlg_text( "@90996906" )
		
	end
		--돌아가기
		dlg_menu( "@90010003", 'NPC_golden_event_contact()' )		
		-- 대화종료 
		dlg_menu( "@90010002", " " )
		dlg_show()
end



----------------------------------------------------------------------------
--------------linrey 퀘스트 테스트 용 임시 ---------------------------------
----------------------------------------------------------------------------

function NPC_linrey_test1_init()
	cprint( "!linrey test" )
	set_npc_name(  "@90999241"  )
end

function NPC_linrey_test1_contact()

	-- 다이얼로그 출력
	dlg_title( "@90999240" )
	dlg_text( "@90999242" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end


   --============================================================
   --===============		상인 오르니토<로밍 테스트>
   --============================================================
function	NPC_Quest_roaming_merchant_init()
	cprint( "!선착장 경비병<좌> 가동" )
	set_npc_name(  "@90999039"  )
end
function	NPC_Quest_roaming_merchant_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999040" )
	dlg_text( "@90999041" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


--============================================================
   --===============		나무꾼 트리슈
   --============================================================

function NPC_Quest_firewood_Treeshoe_init()
	cprint( "!나무꾼 트리슈 가동" )
	set_npc_name(  "@90999543"  )
end


function NPC_Quest_firewood_Treeshoe_contact()

	-- 다이얼로그 출력
	dlg_title( "@90999544" )
	dlg_text( "@90999545" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function Quest_Link_110_1()
	local count = find_item( 1000051 )
	if count == 0 then
	insert_item( 1000051, 15 )
	else
	end
end

--============================================================
   --===============		마도사 케르바
   --============================================================

function NPC_Quest_ascetic_kerva_init()
	cprint( "!마도사 케르바 가동" )
	set_npc_name(  "@90999548"  )
end


function NPC_Quest_ascetic_kerva_contact()

	-- 다이얼로그 출력
	dlg_title( "@90999549" )
	dlg_text( "@90999550" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function Quest_Link_112_1()
	local count = find_item( 1100305 )
	if count == 0 then
	insert_item( 1100305, 1 )
	else
	end
end


--============================================================
   --===============		붉은농장 쥬시
   --============================================================

function	NPC_mainquest_juicy_init()
	cprint( "!붉은농장 쥬시 가동" )
	set_npc_name(  "@90999634"  )
end

function	NPC_mainquest_juicy_contact()
	
	local gender = get_value('sex')
	
	local state_level = get_state_level( 6007 )
	
	local quest_progress1 =  get_quest_progress( 2004 )
	
	if quest_progress1 == 255 then
	
	-- 상처입은 쥬시 다이얼로그 출력
		dlg_title( "@90999857" )
		dlg_text( "@90999858" )
		
		-- 대화종료 
		dlg_menu( "@90010002", " " )

		dlg_show()
	
	else
		-- 홀드 상태인지를 체크(펜릴의 단검을 뽑아주다 걸린다.)
		if  state_level == 1 then
			
			if gender == 2 then
			-- 다이얼로그 출력
			dlg_title( "@90999635" )
			dlg_text( "@90999640" )
			
			-- 조각을 뽑는다.
			dlg_menu( "@90999655", "juicy_talkLink_2()" )
		
		
			-- 대화종료 
			dlg_menu( "@90010002", " " )

			dlg_show()
			

			-- 여성일때
			elseif gender == 1 then
			dlg_title( "@90999635" )
			dlg_text( "@90999641" )
			
			-- 조각을 뽑는다.
			dlg_menu( "@90999655", "juicy_talkLink_2()" )
		
		
			-- 대화종료 
			dlg_menu( "@90010002", " " )
		
			dlg_show()
			
			end
			
		
		else
			-- 남성일때
			if gender == 2 then
			-- 다이얼로그 출력
			dlg_title( "@90999635" )
			dlg_text( "@90999636" )

			-- 여성일때
			elseif gender == 1 then
			dlg_title( "@90999635" )
			dlg_text( "@90999637" )
			
			end

		-- 붉은 머리 소녀에 대해 이야기힌다.
		dlg_menu( "@90999638", "juicy_talkLink_1()" )
		
		
		-- 대화종료 
		dlg_menu( "@90010002", " " )

		dlg_show()
		
		
		end
	
	end -- 상처입은 쥬시
 
end

function	juicy_talkLink_1()
	-- 다이얼로그 출력
	dlg_title( "@90999635" )
	dlg_text( "@90999639" )

		
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

-- 단검 뽑기를 도와준다.
function	juicy_talkLink_2()
	
	local gender = get_value('sex')
	
	-- 다이얼로그 출력
	dlg_title( "@90999635" )
	
	-- 남성일때
		if gender == 2 then
		-- 다이얼로그 출력
		dlg_text( "@90999645" )

		-- 여성일때
		elseif gender == 1 then
		dlg_text( "@90999646" )
		
		end

	-- 붉은 머리 소녀에 대해 이야기힌다.
	dlg_menu( "@90999638", "juicy_talkLink_2_1()" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

-- 단검 뽑기를 도와준다_2
function	juicy_talkLink_2_1()
	
	local gender = get_value('sex')
	
	
	-- 다이얼로그 출력
	dlg_title( "@90999635" )
	
	-- 남성일때
		if gender == 2 then
		-- 다이얼로그 출력
		dlg_text( "@90999683" )

		-- 여성일때
		elseif gender == 1 then
		dlg_text( "@90999684" )
		
		end
		
	-- 내가 누구인지 물어본다.
	dlg_menu( "@90999686", "juicy_talkLink_2_1_1()" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
	
end


-- 내가 누구인지 물어본다.
function	juicy_talkLink_2_1_1()
	
	-- 다이얼로그 출력
	dlg_title( "@90999635" )
	dlg_text( "@90999687" )

	local quest_progress10 =  get_quest_progress(2001) -- 엘로힘 강인 한다리

	if quest_progress10 == 0 then
	-- 내가 누구인지 재차 물어본다.
	dlg_menu( "@90999688", "juicy_talkLink_2_1_1_1()" )
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
	
end


-- 엘로힘 퀘스트 시작
function	juicy_talkLink_2_1_1_1()
		
	show_quest_info_without_npc( 2001 ) 
	
end



--============================================================
   --===============		펜릴 레인카스
   --============================================================

function	NPC_mainquest_rainkas_init()
	cprint( "!펜릴 레인카스 가동" )
	set_npc_name(  "@90999647"  )
end

function	NPC_mainquest_rainkas_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999648" )
	dlg_text( "@90999649" )

	-- 머리를 쓰다듬는다.
	dlg_menu( "@90999650", "fenrir_talkLink_1()" )
	
	-- 먹이를 준다.
	dlg_menu( "@90999651", "fenrir_talkLink_2()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

-- 머리를 쓰다듬는다.
function	fenrir_talkLink_1()
	-- 다이얼로그 출력
	--dlg_title( "@90999648" )
	--dlg_text( "@90999639" )
	
	cprint( "@90999653" )
	
	add_state(6006, 1, 100)

			
	-- 대화종료 
	--dlg_menu( "@90010002", " " )

	--dlg_show()
 
end


-- 먹이를 준다.
function	fenrir_talkLink_2()
	
	
	
	local item_fenrir_food = find_item ( 1000092 )
	
	local item_fenrir_food_handle = get_item_handle ( 1000092 )
   	
	-- 먹이가 없을 경우
	if item_fenrir_food == 0 or is_erasable_item( item_fenrir_food_handle ) == 0 then
	
		cprint( "@90999652" )
		--add_state(6006, 1, 100)
		
		-- 퀘스트 상태 체크 	get_quest_progress(ID)  
		-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
		--local quest_progress100 =  get_quest_progress(2028) -- 레인카스 먹이 구하기
			
		--if quest_progress100 == 0 then
		
		--	show_quest_info_without_npc( 2028 )
		
		--end
						
	else
	
	-- 다이얼로그 출력
	dlg_title( "@90999648" )
	dlg_text( "@90999654" )
	
	delete_item( get_item_handle( 1000092 ), 1 )
	
	-- 단검을 뽑는다.
	dlg_menu( "@90999655", "fenrir_talkLink_2_1()" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
	
	
	end
	 
end


-- 단검을 뽑는다.
function	fenrir_talkLink_2_1()
	-- 다이얼로그 출력
	dlg_title( "@90999648" )
	dlg_text( "@90999656" )

	-- 지속효과 홀드 레벨 1, 3초간 유효
	add_state(6007, 1, 1500)
	
				
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


--=====================================================================================
--===============		'엘로힘'의 자격: 강인한 다리
--=====================================================================================

--엘로힘의 자격:강인한 다리
function quest_start_2001()
	
	--local dice = math.random( 0, 2)
	
	--if dice == 0 then 
		add_field_prop( 30028, 300, 128707 + math.random(0, 300), 102013 + math.random(0, 300))--, gv('layer'), 5, 0, 0, 0, 0, 0, 0, 0, false )

	--elseif dice == 1 then
		add_field_prop( 30028, 300, 126761 + math.random(0, 300), 100882 + math.random(0, 300))
	
	--elseif dice == 2 then
		add_field_prop( 30028, 300, 125554 + math.random(0, 300), 102645 + math.random(0, 300))

	--end
	
end


function quest_end_2001()
	
	--엘로힘의 문장 카운트 구하기
	local item_ellohimmark_count = find_item ( 1000086 )

	if item_ellohimmark_count >= 1 then

		for i = 0, 10 do
			-- 엘로힘의 문장 남은 아이템 없애기
			delete_item( get_item_handle( 1000086 ), item_ellohimmark_count )
		end
					
	end
	
	
end


function quest_end_2001_prop()
	
	add_state( 1013, 3, 3000) -- 이동속도 증가( 10레벨 , 30초)	
	
end
--=====================================================================================
--===============		'엘로힘'의 자격: 차가운 눈
--=====================================================================================

-- 엘로힘의 자격: 차가운 눈
function quest_start_2002()
	
	local dice = math.random( 0, 1)
	
	local questflag01 = get_flag( "mainquest_warp" )
	
	-- 붉은 농장 수정 워프게이트 좌측
	--if dice == 0 then 
	
	--add_field_prop( 30029, 30, 126537 + math.random(0, 30), 102257 + math.random(0, 30) )
	--add_field_prop( 30032, 30, 126621 + math.random(0, 30), 102258 + math.random(0, 30) )
	
	-- 붉은 농장 수정 워프게이트 우측
	--elseif dice == 1 then 
	
	--add_field_prop( 30032, 30, 126537 + math.random(0, 30), 102257 + math.random(0, 30) )
	--add_field_prop( 30029, 30, 126621 + math.random(0, 30), 102258 + math.random(0, 30) )
	
	--end
	if dice == 0 then
	
		set_flag( "mainquest_warp", "0")
	elseif dice == 1 then
	
		set_flag( "mainquest_warp", "1")
	end
	
	cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.
			
end


function quest_start_2002_1()
	
	local dice = math.random( 0, 1)
	
	warp( 121277  + math.random(0,10) , 110241 + math.random(0,10) )
	
	-- 여명의 과수원 수정 워프게이트 좌측
	if dice == 0 then 
	
	add_field_prop( 30030, 30, 120996  + math.random(0, 30), 110200 + math.random(0, 40) )
	add_field_prop( 30032, 30, 121040  + math.random(0, 30), 110326 + math.random(0, 40) )
	
	-- 여명의 과수원 수정 워프게이트 우측
	elseif dice == 1 then 
	
	add_field_prop( 30032, 30, 120996  + math.random(0, 30), 110200 + math.random(0, 40) )
	add_field_prop( 30030, 30, 121040  + math.random(0, 30), 110326 + math.random(0, 40) )
	
	end
	
	cprint( "@90999700" ) -- <#6DD67D>서쪽에서 워프게이트의 마력이 느껴진다.
			
end


function quest_start_2002_2()
	
	local dice = math.random( 0, 1)
	
	warp(  141842 + math.random(0,10) , 134561 + math.random(0,10) )
	
	-- 세이렌 섬 수정 워프게이트 좌측
	if dice == 0 then 
	
	add_field_prop( 30031, 30, 142343 + math.random(0, 30), 134663 + math.random(0, 50) )
	add_field_prop( 30032, 30, 142357 + math.random(0, 30), 134476 + math.random(0, 50) )
	
	-- 세이렌 섬 수정 워프게이트 우측
	elseif dice == 1 then 
	
	add_field_prop( 30032, 30, 142343 + math.random(0, 30), 134663 + math.random(0, 50) )
	add_field_prop( 30031, 30, 142357 + math.random(0, 30), 134476 + math.random(0, 50) )
	
	end
	
	cprint( "@90999697" ) -- <#6DD67D>언덕 위에서 워프게이트의 마력이 느껴진다.
			
end


function quest_start_2002_3()
	
		
	warp(  134442 + math.random(0,10) , 157208 + math.random(0,10) )
	
	
	add_field_prop( 30027, 30, 126537 + math.random(0, 30), 102257 + math.random(0, 50) )
			
	cprint( "@90999871" ) -- <#6DD66D>폭포 수 방향에서 마나의 흐름이 느껴진다.
	cprint( "@90999699" ) -- <#6DD66D>엘로힘 마력이 느껴진다. 영혼석을 탐색하자.
			
end


-- 퀘스트 프랍 무저갱 행(퀘스트 실패)
function quest_start_2002_end()

	warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
	
	-- 처음으로 다시 돌아왔다.
	local dice = math.random( 0, 1)
	
	-- 붉은 농장 수정 워프게이트 좌측
	if dice == 0 then 
	
	add_field_prop( 30029, 30, 126537 + math.random(0, 30), 102257 + math.random(0, 30) )
	add_field_prop( 30032, 30, 126621 + math.random(0, 30), 102258 + math.random(0, 30) )
	
	-- 붉은 농장 수정 워프게이트 우측
	elseif dice == 1 then 
	
	add_field_prop( 30032, 30, 126537 + math.random(0, 30), 102257 + math.random(0, 30) )
	add_field_prop( 30029, 30, 126621 + math.random(0, 30), 102258 + math.random(0, 30) )
	
	end
	
	cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.

end

-- 퀘스트 완료시 쥬시에게로 워프 
function quest_end_2002()

	local qwarpflag_end = get_flag( "mainquest_warp" )
	local dice = math.random( 0, 1 )
	
	--엘로힘의 목걸이 카운트 구하기
	local item_ellohimnecklace_count = find_item ( 1000093 )
	
	if qwarpflag_end == 3 then
		warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
	
		if item_ellohimnecklace_count >= 1 then

			for i = 0, 10 do
				-- 엘로힘의 목걸이 남은 아이템 없애기
				delete_item( get_item_handle( 1000093 ), item_ellohimnecklace_count )
			end
		end
		
		-- 엘로힘의 목걸이 넣어주기
		insert_item( 1000093, 1 )	
		
	else
		warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
		cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
	end
	

end


-- 퀘스트 완료시 쥬시에게로 워프 
function quest_end_2005()

	warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )

end

-- 퀘스트 완료시 쥬시에게로 워프 
function quest_end_2006()

	warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )

end

-- 퀘스트 완료시 쥬시에게로 워프 
function quest_end_2007()

	warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )

end

--=====================================================================================
--===============		'엘로힘'의 자격: 차가운 눈 워프 게이트 수정
--=====================================================================================

-- 0이 정답인 프랍
function quest_start_2002_prop_redfarm_1()

	local qwarpflag01 = get_flag( "mainquest_warp" )
	local dice = math.random( 0, 1 )
	
	
	if qwarpflag01 == 0 then
	
		warp( 121277  + math.random(0,10) , 110241 + math.random(0,10) )
		cprint( "@90999700" ) -- <#6DD67D>서쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
	
	elseif qwarpflag01 == 1 then
			
		warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
		cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
		
	end

end

-- 1이 정답인 프랍
function quest_start_2002_prop_redfarm_2()

	local qwarpflag01 = get_flag( "mainquest_warp" )
	local dice = math.random( 0, 1 )
	
	
	if qwarpflag01 == 1 then
	
		warp( 121277  + math.random(0,10) , 110241 + math.random(0,10) )
		cprint( "@90999700" ) -- <#6DD67D>서쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
	
	elseif qwarpflag01 == 0 then
			
		warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
		cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
		
	end
	

end

-- 0이 정답인 프랍_여명의 과수원
function quest_start_2002_prop_fruit_1()

	local qwarpflag02 = get_flag( "mainquest_warp" )
	local dice = math.random( 0, 1 )
	
	
	if qwarpflag02 == 0 then
	
		warp(  141842 + math.random(0,10) , 134561 + math.random(0,10) )
		cprint( "@90999697" ) -- <#6DD67D>언덕 위에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
	
	elseif qwarpflag02 == 1 then
			
		warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
		cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
		
	end
	

end

-- 1이 정답인 프랍_여명의 과수원
function quest_start_2002_prop_fruit_2()

	local qwarpflag02 = get_flag( "mainquest_warp" )
	local dice = math.random( 0, 1 )
	
	if qwarpflag02 == 1 then
	
		warp(  141842 + math.random(0,10) , 134561 + math.random(0,10) )
		cprint( "@90999697" ) -- <#6DD67D>언덕 위에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
	
	elseif qwarpflag02 == 0 then
			
		warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
		cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
		
	end
	

end

-- 0이 정답인 프랍_세이렌 섬
function quest_start_2002_prop_siland_1()

	local qwarpflag03 = get_flag( "mainquest_warp" )
	local dice = math.random( 0, 1 )
	
	
	if qwarpflag03 == 0 then
	
		warp(  134442 + math.random(0,10) , 157208 + math.random(0,10) )
		--add_field_prop( 30027, 30, 126537 + math.random(0, 30), 102257 + math.random(0, 50) )
			
		cprint( "@90999871" ) -- <#6DD66D>차가운 눈의 자격을 증명하였다.
		cprint( "@90999699" ) -- <#6DD66D>폭포 수 방향의 냉철함의 영혼석을 탐색하자.
		
		--if dice == 0 then
			set_flag( "mainquest_warp", "3")
		--elseif dice == 1 then
			--set_flag( "mainquest_warp", "0")
		--end
	
	elseif qwarpflag03 == 1 then
			
		warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
		cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
		
	end
	

end

-- 1이 정답인 프랍_세이렌 섬
function quest_start_2002_prop_siland_2()

	local qwarpflag03 = get_flag( "mainquest_warp" )
	local dice = math.random( 0, 1 )
	
	
	if qwarpflag03 == 1 then
	
		warp(  134442 + math.random(0,10) , 157208 + math.random(0,10) )
		--add_field_prop( 30027, 30, 126537 + math.random(0, 30), 102257 + math.random(0, 50) )
			
		cprint( "@90999871" ) -- <#6DD66D>차가운 눈의 자격을 증명하였다.
		cprint( "@90999699" ) -- <#6DD66D>폭포 수 방향의 냉철함의 영혼석을 탐색하자.
		
		--if dice == 0 then
			set_flag( "mainquest_warp", "3")
		--elseif dice == 1 then
			--set_flag( "mainquest_warp", "0")
		--end
	
	elseif qwarpflag03 == 0 then
			
		warp( 126572 + math.random(0,10) , 102406 + math.random(0,10) )
		cprint( "@90999698" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.
		
		if dice == 0 then
			set_flag( "mainquest_warp", "1")
		elseif dice == 1 then
			set_flag( "mainquest_warp", "0")
		end
		
	end

end

--=====================================================================================
--===============		메인 퀘스트 etc
--=====================================================================================

-- 쥬시 복수하기 (광신도 암살자)
function quest_start_2004_end()

	cprint( "@90999870" ) -- <#6DD66D>급격한 마나의 흐름으로 데미지를 입었습니다.
	-- 지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
	add_state( 12602, 30, 60000) -- 정신통일(크리티컬 증가) 10레벨
	add_state( 6004, 5, 30000) -- 출혈(무속성 지속데미지) 5레벨
	
	--show_quest_info_without_npc( 2008 )

end



-- 대륙의 과거 무녀제이나에게 워프 
function quest_start_2005()

	warp( 120321 + math.random(0,10) , 72102 + math.random(0,10) )

end

-- 대륙의 과거 사막지기 엘레노에게 워프 
function quest_start_2006()

	warp( 120752 + math.random(0,10) , 86799 + math.random(0,10) )

end

-- 대륙의 과거 엘로힘 시아페에게 워프 
function quest_start_2007()

	warp( 127542 + math.random(0,10) , 121326 + math.random(0,10) )

end


-- 광신도 암살자 진행을 위한 힌트
function quest_end_2008()

	cprint("@90999877") --<<#6DD66D>대륙의 광신도를 무찌르자. 광신도의 정보를 얻기 위해 마을로 가자.
	cprint("@90999878") --<#6DD66D>사람들은 쥬시의 정체는 모르더라도 광신도에 대해서는 알 것이다. 미래를 예언하는 소녀 쥬시의 원수!
	--cprint("@90999726") --<#6DD66D>(외침) 엄청난 마법이다. 마법 실험지 북쪽에 엄청난 마력의 마녀가 부활했다!
	
	del_flag( "mainquest_warp" )

end


-- 반스치 만나기 워프
function quest_start_2014_end()

	warp( 109354 + math.random(10,20) , 71864 + math.random(10,20) )

end

-- 하인젤워커 만나기 워프
function quest_start_2016_end()

	warp( 132678 + math.random(10,20) , 140055 + math.random(10,20) )

end


-- 마녀 부활의 외침들
function quest_end_2017()

	cprint("@90999739") --<#6DD66D>(외침)마녀가 나타났다! 마녀가 부활했다! 으악~ 살려줘!
	cprint("@90999725") --<#6DD66D>(외침) 마법 실험지에 마녀가 나타났어! 실험지 북쪽이다!
	cprint("@90999726") --<#6DD66D>(외침) 엄청난 마법이다. 마법 실험지 북쪽에 엄청난 마력의 마녀가 부활했다!

end


-- 마법 실험지 몬스터 리젠
--[[ function quest_start_2017_start()
		
	--local dice = math.random( 0, 3)
	
	-- 광신도 소환
	add_npc( 131766 + math.random( 10, 50), 140998 + math.random( 10, 50), 85005,  1 + math.random( 0, 3) , 60000 )
	add_npc( 131766 + math.random( 10, 50), 140998 + math.random( 10, 50), 85006,  1 + math.random( 0, 1) , 60000 )		
	
	add_npc( 130646 + math.random( 10, 50), 142164 + math.random( 10, 50), 85005,  1 + math.random( 0, 3) , 60000 )
	add_npc( 130646 + math.random( 10, 50), 142164 + math.random( 10, 50), 85006,  1 + math.random( 0, 1) , 60000 )

	add_npc( 132888 + math.random( 10, 50), 142000 + math.random( 10, 50), 85005,  1 + math.random( 0, 3) , 80000 )
	add_npc( 132888 + math.random( 10, 50), 142000 + math.random( 10, 50), 85006,  1 + math.random( 0, 1) , 80000 )

	add_npc( 132910 + math.random( 10, 50), 141977 + math.random( 10, 50), 85005,  1 + math.random( 0, 3) , 80000 )
	add_npc( 132910 + math.random( 10, 50), 141977 + math.random( 10, 50), 85006,  1 + math.random( 0, 1) , 80000 )	
	
	add_npc( 131230 + math.random( 10, 50), 139236 + math.random( 10, 50), 85005,  1 + math.random( 0, 3) , 60000 )
	add_npc( 131230 + math.random( 10, 50), 139236 + math.random( 10, 50), 85006,  1 + math.random( 0, 1) , 60000 )
	
	add_npc( 130271 + math.random( 10, 50), 139426 + math.random( 10, 50), 85005,  1 + math.random( 0, 3) , 60000 )
	add_npc( 130271 + math.random( 10, 50), 139426 + math.random( 10, 50), 85006,  1 + math.random( 0, 1) , 60000 )
	--end
	
	cprint( "@90999865" ) -- <#6DD66D>마녀의 어둠의 마나의 기운이 느껴진다. 마법실험지를 정화하자.

	
end ]]


-- 마법실험지 가짜마녀 리젠 
function quest_start_2018_start()

	cprint( "@90999879" ) -- <#6DD67D>마녀의 부활? 마법 실험지 북쪽이다!
	--add_npc( 132678 + math.random(0, 10), 140055 + math.random(0, 10), 76006,  1, 180000 )
	
end


-- 루시앙 만나기 워프(마녀 부활 보고)
function quest_start_2019_start()

	warp( 138903 + math.random(10,20) , 142444 + math.random(10,20) )

end


-- 광신도 출몰의 외침들
function quest_end_2019()

	cprint("@90999772") --<#6DD66D>(외침)라크 시에 광신도가 나타났다는 전갈이옵니다!
	cprint("@90999773") --<#6DD66D>(외침)호라이즌에도 광신도와 마녀가 나타났다는 전갈이옵니다!
	cprint("@90999774") --<#6DD66D>(외침)카탄을 비롯한 가이아 대륙 곳곳에 광신도의 습격을 받았다는 전갈 이옵니다.

end

-- 페넌트 만나기(광신도 척결 선포, 상아탑 광신도 소탕)
function quest_start_2020_start()

	warp( 121506 + math.random(10,20) , 152792 + math.random(10,20) )

end


-- 리처드 만나기 워프(어쌔신 길드 지원)
function quest_start_2022_start()

	warp( 102577 + math.random(10,20) , 71378 + math.random(10,20) )

end

-- 루시앙 만나기 워프(어쌔신 길드 소탕 보고)
function quest_start_2024_end()

	warp( 138903 + math.random(10,20) , 142444 + math.random(10,20) )

end


-- 헥토르 소환 워프
function quest_start_2025_start()

	local quest_respawn_start = get_flag( "hectorspawn" )
	set_flag( "hectorspawn", "0" )
	warp( 120899 + math.random(10,20) , 73265 + math.random(10,20) )
	
	-- 지역진입 카운트 초기화 
	--set_event_area_enter_count( 이벤트영역ID, get_event_area_enter_count( 이벤트영역ID ) - 1 )
	set_event_area_enter_count( 70401, get_event_area_enter_count( 70401 ) - 1 )
	set_event_area_enter_count( 70402, get_event_area_enter_count( 70402 ) - 1 )
	set_event_area_enter_count( 70403, get_event_area_enter_count( 70403 ) - 1 )
	set_event_area_enter_count( 70404, get_event_area_enter_count( 70404 ) - 1 )
	set_event_area_enter_count( 70405, get_event_area_enter_count( 70405 ) - 1 )
	set_event_area_enter_count( 70406, get_event_area_enter_count( 70406 ) - 1 )
	set_event_area_enter_count( 70407, get_event_area_enter_count( 70407 ) - 1 )
		
end

-- 마녀사냥 지원 워프
function quest_start_2026_start()

	warp( 138665 + math.random(10,20) , 75192 + math.random(10,20) )
	
	--순백의 바스타드 소드 카운트 구하기
	local item_whitesword_count = find_item ( 1000090 )
	if item_whitesword_count >= 1 then

		for i = 0, 10 do
			-- 순백의 바스타드 소드 남은 아이템 없애기
			delete_item( get_item_handle( 1000090 ), item_whitesword_count )
		end
			
	end
	
	-- 순백의 바스타드 소드 넣어주기
	insert_item( 1000090, 1 )	

end


-- 마녀사냥 워프게이트 생성
function quest_start_2027()
		
	--add_field_prop( 30040, 180, 139305 + math.random(1, 2), 73822 + math.random(1, 2) )
	warp( 139541  + math.random(0,2) , 73447 + math.random(0,2) )
	
end


-- 마녀사냥 순백의 바스타드 소드 삭제
function quest_end_2027()
		
	--순백의 바스타드 소드 카운트 구하기
	local item_whitesword_count = find_item ( 1000090 )
	if item_whitesword_count >= 1 then

		for i = 0, 10 do
			-- 순백의 바스타드 소드 남은 아이템 없애기
			delete_item( get_item_handle( 1000090 ), item_whitesword_count )
		end
		
	end
	
	del_flag( "hectorspawn" )

end


-- 마녀사냥 워프게이트 생성
function quest_start_2027_prop()
		
	--add_field_prop( 30040, 180, 139305 + math.random(1, 2), 73822 + math.random(1, 2) )
	warp( 139256 + math.random(0,2) , 73908 + math.random(0,2) )
	
end



-- 레인카스 먹이 구하기
function quest_start_2028()

	--warp( 138615 + math.random(10,20) , 75156 + math.random(10,20) )

end


-- 상아탑 광신도 리젠
--[[ function quest_start_2021_start()

	add_npc( 119644  + math.random(0, 10), 156120 + math.random(0, 10), 81007,  1, 60000 )
	add_npc( 119644  + math.random(0, 10), 156120 + math.random(0, 10), 81008,  2+ math.random( 0, 4), 60000 )
	add_npc( 120394  + math.random(0, 20), 156403 + math.random(0, 20), 81009,  1+ math.random( 0, 3), 60000 )
	add_npc( 120394  + math.random(0, 20), 156403 + math.random(0, 20), 81006,  5+ math.random( 0, 4), 60000 )
	
	add_npc( 120762  + math.random(0, 50), 156596 + math.random(0, 50), 81008,  2+ math.random( 0, 4), 70000 )
	add_npc( 120762  + math.random(0, 50), 156596 + math.random(0, 50), 81009,  1, 70000 )
	add_npc( 121451  + math.random(0, 50), 155799 + math.random(0, 50), 81006,  4+ math.random( 0, 5), 70000 )
	add_npc( 121451  + math.random(0, 50), 155799 + math.random(0, 50), 81007,  1+ math.random( 0, 3), 70000 )
	
	add_npc( 121740  + math.random(0, 50), 154852 + math.random(0, 50), 81006,  5+ math.random( 0, 4), 80000 )
	add_npc( 120892  + math.random(0, 50), 154332 + math.random(0, 50), 81008,  5+ math.random( 0, 4), 80000 )
	
end ]]

--어쌔신 길드 광신도 리젠
--[[ function quest_start_2023_start()

	add_npc( 102720  + math.random(0, 10), 69459 + math.random(0, 10), 85006,  1, 70000 )
	add_npc( 102720  + math.random(0, 20), 69459 + math.random(0, 20), 85008,  1, 70000 )
	add_npc( 102720  + math.random(0, 50), 69459 + math.random(0, 50), 85005,  4+ math.random( 0, 3), 70000 )
	add_npc( 102720  + math.random(0, 50), 69459 + math.random(0, 50), 85007,  3+ math.random( 0, 3), 70000 )
	--add_npc( 102720  + math.random(0, 50), 69459 + math.random(0, 50), 85007,  4, 600 )
	--add_npc( 102720  + math.random(0, 50), 69459 + math.random(0, 50), 85005,  5, 600 )
	
	--add_npc( 101960  + math.random(0, 10), 69294 + math.random(0, 10), 85006,  1, 600 )
	--add_npc( 101960  + math.random(0, 20), 69294 + math.random(0, 20), 85008,  1, 600 )
	add_npc( 101960  + math.random(0, 50), 69294 + math.random(0, 50), 85005,  2+ math.random( 0, 3), 80000 )
	add_npc( 101960  + math.random(0, 50), 69294 + math.random(0, 50), 85007,  3+ math.random( 0, 3), 80000 )
	add_npc( 101960  + math.random(0, 50), 69294 + math.random(0, 50), 85007,  4+ math.random( 0, 3), 80000 )
	add_npc( 101960  + math.random(0, 50), 69294 + math.random(0, 50), 85005,  5+ math.random( 0, 3), 80000 )
	
	add_npc( 102570  + math.random(0, 10), 70130 + math.random(0, 10), 85006,  1+ math.random( 0, 2), 70000 )
	add_npc( 102570  + math.random(0, 20), 70130 + math.random(0, 20), 85008,  1+ math.random( 0, 2), 70000 )
	--add_npc( 102570  + math.random(0, 50), 70130 + math.random(0, 50), 85005,  2, 600 )
	--add_npc( 102570  + math.random(0, 50), 70130 + math.random(0, 50), 85007,  3, 600 )
	add_npc( 102570  + math.random(0, 50), 70130 + math.random(0, 50), 85007,  4+ math.random( 0, 3), 70000 )
	add_npc( 102570  + math.random(0, 50), 70130 + math.random(0, 50), 85005,  5+ math.random( 0, 3), 70000 )
	
end ]]


--=====================================================================================
--===============		'엘로힘'의 자격: 영롱한 영혼
--=====================================================================================


function quest_start_2003()
	
	--영롱한 백합 카운트 구하기
	local item_lily_count = find_item ( 1000088 )

	if item_lily_count >= 1 then

		for i = 0, 10 do
			-- 영롱한 백합 남은 아이템 제거
			delete_item( get_item_handle( 1000088 ), item_lily_count )
		end
					
	end
		
	-- 영롱한 백합 아이템 새로 넣어준다.
	insert_item( 1000088, 1 )
	
	-- 지역진입 카운트 초기화 
	--set_event_area_enter_count( 이벤트영역ID, get_event_area_enter_count( 이벤트영역ID ) - 1 )
	set_event_area_enter_count( 70601, get_event_area_enter_count( 70601 ) - 1 )
	set_event_area_enter_count( 70602, get_event_area_enter_count( 70602 ) - 1 )
	set_event_area_enter_count( 70603, get_event_area_enter_count( 70603 ) - 1 )
	set_event_area_enter_count( 70604, get_event_area_enter_count( 70604 ) - 1 )
	set_event_area_enter_count( 70605, get_event_area_enter_count( 70605 ) - 1 )
	set_event_area_enter_count( 70606, get_event_area_enter_count( 70606 ) - 1 )
	set_event_area_enter_count( 70607, get_event_area_enter_count( 70607 ) - 1 )
	set_event_area_enter_count( 70608, get_event_area_enter_count( 70608 ) - 1 )
		
end

function quest_end_2003()
	
	--영롱한 백합 카운트 구하기
	local item_lily_count = find_item ( 1000088 )

	if item_lily_count >= 1 then

		for i = 0, 10 do
			-- 영롱한 백합 남은 아이템 제거
			delete_item( get_item_handle( 1000088 ), item_lily_count )
		end
	cprint( "@90999849" ) -- <#6DD66D>영롱한 백합이 사라졌습니다.
	end
		
end

function mainquest_region_redfarm_level1()
	
	--local dice = math.random( 0, 3)
	
	-- 붉은 농장 수정 워프게이트 좌측
	--if dice == 0 then 
	
	--영롱한 백합 카운트 구하기
	local item_lily_count = find_item ( 1000088 )
	
	if item_lily_count >= 1 then
	
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 52014,  1 + math.random( 2, 6) , 60000 )
				
		cprint( "@90999863" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.

	else
	
		cprint( "@90999876" ) -- <#6DD66D>영롱한 백합이 요정들의 관심을 모을 수 있습니다.
	
	end
end


function quest_start_2015()
			
	-- 지역진입 카운트 초기화 
	--set_event_area_enter_count( 이벤트영역ID, get_event_area_enter_count( 이벤트영역ID ) - 1 )
	set_event_area_enter_count( 60401, get_event_area_enter_count( 60401 ) - 1 )
	set_event_area_enter_count( 60402, get_event_area_enter_count( 60402 ) - 1 )
	set_event_area_enter_count( 60403, get_event_area_enter_count( 60403 ) - 1 )
	set_event_area_enter_count( 60404, get_event_area_enter_count( 60404 ) - 1 )
	set_event_area_enter_count( 60405, get_event_area_enter_count( 60405 ) - 1 )
			
end


function mainquest_region_redfarm_level2()
	
	-- 처음으로 다시 돌아왔다.
	--local dice = math.random( 0, 3)
	
	-- 붉은 농장 수정 워프게이트 좌측
	--if dice == 0 then 
	
	
	--영롱한 백합 카운트 구하기
	local item_lily_count = find_item ( 1000088 )
	
	if item_lily_count >= 1 then
	
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 52014,  1 + math.random( 2, 6) , 60000 )

		cprint( "@90999863" ) -- <#6DD67D>남쪽에서 워프게이트의 마력이 느껴진다.

	else
	
		cprint( "@90999876" ) -- <#6DD66D>영롱한 백합이 요정들의 관심을 모을 수 있습니다.
	
	end
	
end

-- 헥토르 소환(화형터 지역)
function mainquest_region_pyresite_level1()
	
	local dice = math.random( 0, 3)
	local dice2 = math.random( 0, 3)
	local quest_respawn02 = get_flag( "hectorspawn" )
		
	if quest_respawn02 == 0 then
		-- 광신도 소환
		if dice == 0 or dice == 1 then 
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85007,  1 + math.random( 1, 4) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85007,  1 + math.random( 1, 4) , 60000 )	
		
		cprint( "@90999864" ) -- <#6DD66D>광신도가 나타났다.
		cprint( "@90999881" ) -- <#6DD67D>나를 찾으러 왔나? 나 헥토르를?
		set_flag( "hectorspawn", "1" )
		
		elseif dice == 2 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86005,  1 , 120000 )	
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		cprint( "@90999882" ) -- <#6DD67D>헥토르의 사념이 담긴 도플갱어입니다.
		set_flag( "hectorspawn", "7" )
		
		elseif dice == 3 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 0, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85007,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		set_flag( "hectorspawn", "2" )
		
		end
	
	elseif quest_respawn02 == 1 then
			
		if dice2 == 0 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86005,  1 , 120000 )	
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		cprint( "@90999882" ) -- <#6DD67D>헥토르의 사념이 담긴 도플갱어입니다.
		set_flag( "hectorspawn", "7" )
		
		elseif dice2 == 1 or dice2 == 2 or dice2 == 3 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 0, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85007,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		set_flag( "hectorspawn", "2" )
		
		end
		
	elseif quest_respawn02 == 2 then
				
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86005,  1 , 120000 )	
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		cprint( "@90999882" ) -- <#6DD67D>헥토르의 사념이 담긴 도플갱어입니다.
		set_flag( "hectorspawn", "7" )
		
	end
end


-- 헥토르 소환(화형터 지역)
function mainquest_region_pyresite_level2()
	
	local dice = math.random( 0, 3)
	local dice2 = math.random( 0, 3)
	local quest_respawn02 = get_flag( "hectorspawn" )
		
	if quest_respawn02 == 0 then
		-- 광신도 소환
		if dice == 0 or dice == 1 then 
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85007,  1 + math.random( 1, 4) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85007,  1 + math.random( 1, 4) , 60000 )	
		
		cprint( "@90999864" ) -- <#6DD66D>광신도가 나타났다.
		cprint( "@90999881" ) -- <#6DD67D>나를 찾으러 왔나? 나 헥토르를?
		set_flag( "hectorspawn", "1" )
		
		elseif dice == 2 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86005,  1 , 120000 )	
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		cprint( "@90999882" ) -- <#6DD67D>헥토르의 사념이 담긴 도플갱어입니다.
		set_flag( "hectorspawn", "7" )
		
		elseif dice == 3 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 0, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85007,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		set_flag( "hectorspawn", "2" )
		
		end
	
	elseif quest_respawn02 == 1 then
			
		if dice2 == 0 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86005,  1 , 120000 )	
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		cprint( "@90999882" ) -- <#6DD67D>헥토르의 사념이 담긴 도플갱어입니다.
		set_flag( "hectorspawn", "7" )
		
		elseif dice2 == 1 or dice2 == 2 or dice2 == 3 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 0, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85007,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		set_flag( "hectorspawn", "2" )
		
		end
		
	elseif quest_respawn02 == 2 then
		
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86005,  1 , 120000 )	
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 85006,  1 + math.random( 2, 3) , 60000 )
		add_npc( get_value( "x" )  + math.random( 10, 50), get_value( "y" ) + math.random( 10, 50), 86006,  1 + math.random( 1, 4) , 60000 )
		
		cprint( "@90999880" ) -- <#6DD66D>헥토르의 도플갱어가 나타났습니다. 시간이 지나면 사라집니다.
		cprint( "@90999882" ) -- <#6DD67D>헥토르의 사념이 담긴 도플갱어입니다.
		set_flag( "hectorspawn", "7" )
			
	end
end

-- 마녀의 흔적(사이라그 폐허)
function mainquest_region_siragruins_level1()

	--local dice = math.random( 0, 3)
	
	-- 광신도 소환
	--if dice == 0 or dice == 1 or dice == 2 then 
	
	add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 75007,  1 + math.random( 2, 5) , 60000 )
	add_npc( get_value( "x" )  + math.random( 5, 25), get_value( "y" ) + math.random( 5, 25), 77010,  1 + math.random( 1, 5) , 60000 )	
	
	cprint( "@90999875" ) -- <#6DD66D> 몬스터가 나타났다.
	
	--end
	
end

function mainquest_item_lily()
	
	--add_npc( 126590 + math.random(0, 10), 102545 + math.random(0, 10), 1004,  1 + math.random(0, 5))
	
end



-- 지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function mainquest_item_whitesword()
		
	add_state( 1003, 10, 30000 ) --( HP재생률 향상, 10레벨, 5분)
 
	add_state( 1001, 10, 300 ) --( MAX HP 증가, 10레벨, 5분)
  	
	cprint( "@90999855" ) -- <#6DD66D>순백의 바스타드에서 성스러운 기운이 퍼집니다.
			
end

--============================================================
   --===============		마을사람 데바 남
   --============================================================

function	NPC_mainquest_dem__init()
	cprint( "!마을사람 데바 남 가동" )
	set_npc_name(  "@90999039"  )
end

function	NPC_mainquest_dem_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999040" )
	dlg_text( "@90999041" )

	-- 대화 1?
	dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

--============================================================
   --===============		마을 주민 데바 여
   --============================================================

function	NPC_mainquest_def__init()
	cprint( "!마을 주민 데바 여 가동" )
	set_npc_name(  "@90999039"  )
end

function	NPC_mainquest_def_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999040" )
	dlg_text( "@90999041" )

	-- 대화 1?
	dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

--============================================================
   --===============		마을 주민 가이아 남
   --============================================================

function	NPC_mainquest_gam__init()
	cprint( "!마을 주민 가이아 남 가동" )
	set_npc_name(  "@90999039"  )
end

function	NPC_mainquest_gam_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999040" )
	dlg_text( "@90999041" )

	-- 대화 1?
	dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


--============================================================
   --===============		마을 주민 가이아 여
   --============================================================

function	NPC_mainquest_gaf__init()
	cprint( "!마을 주민 가이아 여 가동" )
	set_npc_name(  "@90999039"  )
end

function	NPC_mainquest_gaf_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999040" )
	dlg_text( "@90999041" )

	-- 대화 1?
	dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


--============================================================
   --===============		마을 주민 아수라 남
   --============================================================

function	NPC_mainquest_asm__init()
	cprint( "!마을 주민 아수라 남 가동" )
	set_npc_name(  "@90999039"  )
end

function	NPC_mainquest_asm_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999040" )
	dlg_text( "@90999041" )

	-- 대화 1?
	dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


--============================================================
   --===============		마을 주민 아수라 여
   --============================================================

function	NPC_mainquest_asf__init()
	cprint( "!마을 주민 아수라 여 가동" )
	set_npc_name(  "@90999039"  )
end

function	NPC_mainquest_asf_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999040" )
	dlg_text( "@90999041" )

	-- 대화 1?
	dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end



--============================================================
   --===============		마녀 사냥의 영웅 헥토르
   --============================================================



function	NPC_mainquest_hector_init()
	cprint( "!마녀 사냥의 영웅 헥토르 가동" )
	set_npc_name(  "@90999850"  )
end

function	NPC_mainquest_hector_contact()
	
	-- 다이얼로그 출력
	dlg_title( "@90999851" )
	dlg_text( "@90999852" )

	--대화 헥토르의 정체
	dlg_menu( "@90999853", "hector_talkLink_1()" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress3272 = get_quest_progress(3272) -- <(version:7.3)>[마스터]마스터 던전으로
	local quest_progress3322 = get_quest_progress(3322) -- <(version:7.3)>[마스터]새로운 기운	
	local quest_progress3335 = get_quest_progress(3335) -- <(version:7.3)>[마스터]마지막 시련2
	-- 퀘스트 종료시 대사	
	if quest_progress3272 == 255 then
	
	-- 마스터 던전으로 이동
	dlg_menu( "@91002085", 'warp( 17818, 40213, gv("layer") )' )
	
	end	
	
	if quest_progress3335 == 255 then		

		dlg_menu( "@90010002", " " )
		dlg_show()

	elseif quest_progress3335 == 1 or quest_progress3335 == 2 then

		dlg_menu( "@91002474" , "card_choose()" ) --넘긴다.
		dlg_menu( "@90010002", " " )
		dlg_show()
	else

	end
	
	--마스터 클래스이면 TP 포인트에 대한 설명 추가
	if get_value( "job_depth" ) == 3 then
		--TP 포인트란?
		dlg_menu( "@91002550", "tp_point()")

	else
		

	end

	if get_value( "job_depth" ) == 2 and quest_progress3322 == 255 and get_value( "level" ) > 147 and get_value( "job_level" ) == 50 then
		--좋아~ 마스터 사탄에게 보내주겠네
	
		dlg_text( "@91002562" )		
		dlg_menu( "@91002561", "goto_master_satan()")

	elseif get_value( "job_depth" ) == 3 then

		dlg_menu( "@90010002", " " )
		dlg_show()

	elseif quest_progress3322 == 255 then
		--아직이야! 더 자격을 갖추라고!
	--	dlg_title( "@90999851" )
		dlg_text( "@91002560")	

	else	
		
	end

	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

function goto_master_satan()

	warp( 30651, 41454, gv("layer") )

end

function	hector_talkLink_1()
	-- 다이얼로그 출력
	dlg_title( "@90999851" )
	dlg_text( "@90999854" )

	-- 대화종료 
	dlg_menu( "@90010003","NPC_mainquest_hector_contact()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


function tp_point()

	-- 다이얼로그 출력
	dlg_title( "@90999851" )
	dlg_text( "@91002551" )


	--특성 스킬이란?
	dlg_menu( "@91002554", "tp_skill()")

	-- 대화종료 
	dlg_menu( "@90010003","NPC_mainquest_hector_contact()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end		

function tp_skill()

	-- 다이얼로그 출력
	dlg_title( "@90999851" )

	--TP 포인트 분배는 어쩌구 저쩌구
	dlg_text( "@91002553" )

	--상위 특성 스킬이란?
	dlg_menu( "@91002556", "upper_tp_skill()")
	
	-- 하위 특성 스킬이란?
	dlg_menu( "@91002558", "lower_tp_skill()")

	-- 대화종료 

	dlg_menu( "@90010003","tp_point()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end	

function 	upper_tp_skill()

	-- 다이얼로그 출력
	dlg_title( "@90999851" )

	-- 상위 특성 스킬은 어쩌구 저쩌구
	dlg_text( "@91002557" )

	-- 대화종료 

	dlg_menu( "@90010003","tp_skill()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end

function 	lower_tp_skill()

	-- 다이얼로그 출력
	dlg_title( "@90999851" )

	-- 하위 특성 스킬은 어쩌구 저쩌구
	dlg_text( "@91002559" )


	-- 대화종료 

	dlg_menu( "@90010003","tp_skill()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end


function quest_start_3334()

	local quest_progress3335 = get_quest_progress(3335) -- <(version:7.3)>[마스터]마지막 시련2	

	if quest_progress3335 == 255 then
		dlg_text( "@91002472" )

		dlg_menu( "@90010002", " " )	-- 종료
		dlg_show()
	else
		dlg_menu( "@90010002", " " )	-- 종료
		dlg_show()
	end
end

function quest_end_3334()

	local talentp = get_value( 'tp' )
	talentp = talentp + 1
	-- set_value( "tp", talentp )

end

function quest_start_3335()

	local quest_progress3334 = get_quest_progress(3334) -- <(version:7.3)>[마스터]마지막 시련1	
	local quest_progress3335 = get_quest_progress(3335) -- <(version:7.3)>[마스터]마지막 시련2

	if quest_progress3334 == 255 then
		dlg_text( "@91002472" )

		dlg_menu( "@90010002", " " )	-- 종료
		dlg_show()
	else
		dlg_menu( "@90010002", " " )	-- 종료
		dlg_show()
	end
end

function quest_end_3335()

		local talentp = get_value( 'tp' )
		talentp = talentp + 2
		-- set_value( "tp", talentp )
		
		dlg_text_without_quest_menu( "@91002477" )
		dlg_menu( "@90010002", " " )	-- 종료
		dlg_show()
end


function card_choose()
	-- 다이얼로그 출력
	dlg_title( "@90999851" )
		
	-- 가지고 있는 스페셜 레어 카드 장수를 세고, 메뉴로 보여준다(켈베로스, 지니, 미스틱지니, 큐브)
	local checked_card_id = { 540061, 540063, 540036, 540076 }
	local existed_card_id = {}
	local existed_card_count = 0
	local found_item_handle = {}

	local i, j
	
	for i = 1, 4 do
		found_item_handle = get_item_handle_list( checked_card_id[i] )
		
		if found_item_handle ~= 0 then
			for j =1, table.getn( found_item_handle	) do
				existed_card_count = existed_card_count + 1
				existed_card_id[existed_card_count] = checked_card_id[i]
			end
		else

		end
	end

	-- 카드가 몇장 있구만! 어떤걸로 받치겠는가
	if existed_card_count == 0 then
		--크리처를 보긴 한건가?! 
		dlg_text_without_quest_menu( "@91002475" )
	else
		dlg_text_without_quest_menu( "@91002476" )
	
		for i = 1, existed_card_count do
			dlg_menu( tostring( get_item_name_by_code( existed_card_id[i] ) ), "card_delete(" .. existed_card_id[i] .. ")" )
		end		
	end

	dlg_menu( "@90010003","NPC_mainquest_hector_contact()" )	-- 돌아가기
	dlg_show()	-- 보이기

end

function card_delete( item_code )

	if is_erasable_item ( get_item_handle( item_code ) ) == 0 then
		--정말 자네가 편성한 크리처를 받칠 것인가?! 다시 생각해보게.
		dlg_text_without_quest_menu( "@91002481" )
		dlg_menu( "@90010003","NPC_mainquest_hector_contact()" )
		dlg_show()
	end

	local success = delete_item( get_item_handle( item_code ), 1 )

	if( success == 1 ) then
		-- 성공
		-- 퀘스트 완료 상태로 만들어줌
		
		set_quest_status( 3335, 1, 1  )
		
		dlg_text_without_quest_menu( "@91002482" )
		cprint( '@91002483' )				
		dlg_menu( "@90010002", " " )
		dlg_show()
	else
		-- 실패

	end

end









--============================================================
   --===============		빛의 대교주 루시앙
   --============================================================



function	NPC_mainquest_rusian_init()
	cprint( "!빛의 대교주 루시앙 가동" )
	set_npc_name(  "@90999765"  )
end

function	NPC_mainquest_rusian_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999766" )
	dlg_text( "@90999767" )

	-- 대화 1?
	--dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end



--============================================================
   --===============		사막지기 엘레노
   --============================================================



function	NPC_mainquest_eleno_init()
	cprint( "!사막지기 엘레노" )
	set_npc_name(  "@90999720"  )
end

function	NPC_mainquest_eleno_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999721" )
	dlg_text( "@90999722" )

	-- 대화 1?
	--dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


--============================================================
   --===============		교육관 반스치
   --============================================================



function	NPC_mainquest_vansch_init()
	cprint( "!교육관 반스치" )
	set_npc_name(  "@90999727"  )
end

function	NPC_mainquest_vansch_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999728" )
	dlg_text( "@90999729" )

	-- 대화 1?
	--dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end



--============================================================
   --===============		교육관 하인젤워커
   --============================================================



function	NPC_mainquest_hwalker_init()
	cprint( "!교육관 하인젤 워커" )
	set_npc_name(  "@90999734"  )
end

function	NPC_mainquest_hwalker_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999735" )
	dlg_text( "@90999736" )

	-- 대화 1?
	--dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


--============================================================
   --===============		엘로힘 시아페
   --============================================================



function	NPC_mainquest_siape_init()
	cprint( "!엘로힘 시아페" )
	set_npc_name(  "@90999758"  )
end

function	NPC_mainquest_siape_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999759" )
	dlg_text( "@90999760" )

	-- 대화 1?
	--dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

--============================================================
   --===============		소탕군 사령관 페넌트
   --============================================================



function	NPC_mainquest_pennent_init()
	cprint( "!소탕군 사령관 페넌트" )
	set_npc_name(  "@90999777"  )
end

function	NPC_mainquest_pennent_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999778" )
	dlg_text( "@90999779" )

	-- 대화 1?
	--dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

--============================================================
--===============		소탕군 사령관 리처드
--============================================================



function	NPC_mainquest_richard_init()
	cprint( "!소탕군 사령관 리처드" )
	set_npc_name(  "@90999785"  )
end

function	NPC_mainquest_richard_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999786" )
	dlg_text( "@90999787" )

	-- 대화 1?
	--dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end



-- <ep7>간호대원 아가덴
function NPC_lost_island_agaden_contact()

	-- 다이얼로그 출력
	dlg_title("@91000687")

	dlg_text( "@91000689" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()

end



-- <ep7>설원 탐사대 데릭
function NPC_lost_island_derick_contact()

	-- 다이얼로그 출력
	dlg_title("@91000690")

	dlg_text( "@91000692" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end




-- <ep7>몬스터 연구원 시카
function quest_start_3041()

	local count = find_item( 1000132)
	
	if count == 0 then
	insert_item( 1000130, 1 )
	else
	end
end

-- <ep7>몬스터 연구원 시카
function quest_end_3041()

	local count = find_item( 1000132)
	
	if count == 0 then
	delete_item( 1000130, 1 )
	else
	end
end


-- <ep7>간호대원 아가덴
function quest_start_3054()

	local count = find_item( 1000141)
	
	if count == 0 then
	insert_item( 1000141, 1 )
	else
	end
end


-- <ep7>간호대원 아가덴
function quest_end_3054()

	local count = find_item( 1000141)
	
	if count == 1 then
	delete_item(  get_item_handle(1000141), count )
	else
	end
end

-- <ep7>간호대원 아가덴
function quest_end_3051()

	local count = find_item( 1000144)
	
	if count == 1 then
	delete_item( get_item_handle(1000144), count )
	else
	end
end

-- <ep7>봉인 해체(카이넨 퀘스트)
function quest_start_3069()

	local count = find_item( 1000152)
	
	if count == 0 then
	insert_item( 1000152, 1 )
	else
	end
end

-- <ep7>몬스터 연구원 시카
function quest_end_3069()

	local count = find_item( 1000152)
	
	if count == 1 then
	delete_item(  get_item_handle(1000152), count )
	else
	end
end



-- <ep7>병사 캐치칸
function quest_start_3073()

	local count = find_item( 1000122)
	
	if count == 0 then
	insert_item(  1000122, 1 )
	else
	end
end


-- <ep7>병사 캐치칸
function quest_end_3073()

	local count = find_item( 1000122)
	
	if count == 1 then
	delete_item( get_item_handle(1000122), count )
	else
	end
end


-- <ep7>설원 탐사대 체샤
function NPC_lost_island_kodia_checha()

	-- 다이얼로그 출력
	dlg_title("@91000702")

	dlg_text( "@91000703" )
	
	dlg_menu( "@90010002", " " )

	dlg_show()
	

end



-- <(version:7.2)> 신비한 광석1
function quest_start_3058()

	local count = find_item( 1000146 )
	
	if count == 0 then
	insert_item( 1000146, 1 )
	else
	end
end

-- <(version:7.2)> 신비한 광석1
function quest_end_3058()

	local count = find_item( 1000146)
	
	if count == 1 then
	delete_item( get_item_handle(1000146), 1 )
	else
	end
end

-- <(version:7.2)> 신비한 광석2
function quest_start_3059()

	local count = find_item( 1000146)
	
	if count == 0 then
	insert_item( 1000146, 1 )
	else
	end
end

-- <(version:7.2)> 신비한 광석2
function quest_end_3059()

	local count = find_item( 1000146)
	
	if count == 1 then
	delete_item( get_item_handle(1000146), 1 )
	else
	end
end



--============================================================
--===============		탐사대장 케인
--============================================================
function	NPC_lost_island_kain_init()
	cprint( "!탐사대장 케인" )
	set_npc_name(  "@91000550"  )
end

function	NPC_lost_island_kain_contact()
	-- 다이얼로그 출력
	dlg_title( "@91000551" )
	dlg_text( "@91000552" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

--============================================================
--===============		간호대원 루엘리
--============================================================
function	NPC_lost_island_ruely_init()
	cprint( "!간호대원 루엘리" )
	set_npc_name(  "@91000553"  )
end

function	NPC_lost_island_ruely_contact()
	-- 다이얼로그 출력
	dlg_title( "@91000554" )
	dlg_text( "@91000555" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

--============================================================
--===============		탐사대원 치닉
--============================================================
function	NPC_lost_island_chignik_init()
	cprint( "!탐사대원 치닉" )
	set_npc_name(  "@91000559"  )
end

function	NPC_lost_island_chignik_contact()
	-- 다이얼로그 출력
	dlg_title( "@91000560" )
	dlg_text( "@91000561" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

--============================================================
--===============		탐사대원 마르테즈
--============================================================
function	NPC_lost_island_maldez_init()
	cprint( "!탐사대원 마르테즈" )
	set_npc_name(  "@91000562"  )
end

function	NPC_lost_island_maldez_contact()
	-- 다이얼로그 출력
	dlg_title( "@91000563" )
	dlg_text( "@91000564" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


--============================================================
--===============		용의 신관 에스터
--============================================================
function	NPC_dragonservicer_easter_init()
	cprint( "!용의 신관 에스터" )
	set_npc_name(  "@91001003"  )
end

function	NPC_dragonservicer_easter_contact()
	-- 다이얼로그 출력
	dlg_title( "@91001004" )
	dlg_text( "@91001005" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end
	
--============================================================
--===============		약초 채집가 발룬
--============================================================
function	NPC_medicalherbcollector_balune_init()
	cprint( "!약초 채집가 발룬" )
	set_npc_name(  "@91001006"  )
end

function	NPC_medicalherbcollector_balune_contact()
	-- 다이얼로그 출력
	dlg_title( "@91001007" )
	dlg_text( "@91001008" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end	
	
--============================================================
--===============		여행자 실피느
--============================================================
function	NPC_traveler_silpyne_init()
	cprint( "!여행자 실피느" )
	set_npc_name(  "@91000994"  )
end

function	NPC_traveler_silpyne_contact()
	-- 다이얼로그 출력
	dlg_title( "@91000995" )
	dlg_text( "@91000996" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress3263 =  get_quest_progress(3263)
	-- 퀘스트 종료시 대사	
	if quest_progress3263 == 0 then
	
		local count = find_item( 1000114 )

		if count == 0 then
			cprint("@91002083")
			insert_item( 1000114, 1 )
		else
		end

	-- 퀘스트 수행 전, 
	elseif 	quest_progress3263 == 255 or quest_progress3263 == 0 or quest_progress3263 == -1 then
		dlg_title( "@91000995" )
		dlg_text( "@91000996" )
	end	



	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end	

--============================================================
--===============		여행자 실피느의 유니콘
--============================================================
function	NPC_traveler_silpyne_unicorn_init()
	cprint( "!유니콘" )
	set_npc_name(  "@91000997"  )
end

function	NPC_traveler_silpyne_unicorn_contact()
	-- 다이얼로그 출력
	dlg_title( "@91000998" )
	dlg_text( "@91000999" )


	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end	

--============================================================
--===============		골드픽시 미리엘
--============================================================
function	NPC_goldpixi_miriel_init()
	cprint( "!골드픽시 미리엘" )
	set_npc_name(  "@91001000"  )
end

function	NPC_goldpixi_miriel_contact()
		

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress3263 = get_quest_progress(3263)
	local quest_progress3264 = get_quest_progress(3264)
	local quest_progress3265 = get_quest_progress(3265)
	local count = find_item(1000114)

	-- 퀘스트 종료시 대사	
	if quest_progress3265 == 255 and count == 1	then

		dlg_title( "@91001001" )
		dlg_text( "@91001009" )

	elseif quest_progress3264 == 255 and count == 1 then

		dlg_title( "@91001001" )
		dlg_text( "@91000983" )

	-- 퀘스트 수행 전, 수행 중 대사
	elseif 	quest_progress3263 == 255 and count == 1 then

		dlg_title( "@91001001" )
		dlg_text( "@91000980" )
		
	-- 퀘스트 수행 전, 수행 중 대사
	else 
		dlg_title( "@91001001" )
		dlg_text( "@91001002" )
	end	


	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end	

--============================================================
--===============		시체가 가진 편지 퀘스트 
--============================================================

-- <(version:7.2)>

 
function quest_prop_lostlsland_exploerer()

	local count = find_item(1000105)
	
	if count == 0 then
	insert_item( 1000105, 1 )
	else
	end
end

--============================================================
--===============		귀환 퀘스트 
--============================================================
function quest_start_3267()

end

function quest_end_3267()

	local count = find_item(1000114)

	if count == 1 then
		delete_item( get_item_handle(1000114), 1 )
		cprint("@91002084")
	else
	end
end



--============================================================
--===============		잃어버린 섬 카이넨 퀘스트 
--============================================================
function quest_prop_add_npc( id )
	cprint( "@91002015" )
	local x = gv( "x" ) + math.random( 30, 50 )
	local y = gv( "y" ) + math.random( 30, 50 )
	add_npc( x, y, id, 1, 60000 )
end

--============================================================
--===============		잃어버린 섬 카이넨 퀘스트  봉인의열쇠
--============================================================

 function quest_start_3065()
	
	warp( 201346, 167805, gv("layer") )
	
end


 function quest_start_3066()
	
	warp( 201409, 151671, gv("layer") )
	
end


 function quest_start_3067()
	
	warp( 201338, 135572, gv("layer") )
	
end


 function quest_start_3068()
	
	warp( 89082, 121414, gv("layer") )
	
end


function prop_quest_LostIsland_WindyPaper()

end

--============================================================
--===============		NPC 돈 크리스 ( 7.2단품퀘스트)
--============================================================

function	NPC_qeest_single_doncris_contact()
	-- 다이얼로그 출력
	dlg_title( "@91001012" )
	dlg_text( "@91001014" )


	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end	
--============================================================
--===============		NPC 대장장이 티에리 ( 7.2단품퀘스트)
--============================================================

function	NPC_qeest_single_thierry_contact()
	-- 다이얼로그 출력
	dlg_title( "@91001015" )
	
	if get_local_info() == 1 then
		dlg_text( "@90610217" )
		dlg_menu( "@90100703", "open_market( 'Broken_Taming_Card' )" )
	else
		dlg_text( "@91001017" )
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	
	

	dlg_show()
 
end	--============================================================
--===============		NPC 종단사무관 칸 ( 7.2단품퀘스트)
--============================================================

function	NPC_qeest_single_khan_contact()
	-- 다이얼로그 출력
	dlg_title( "@91001018" )
	dlg_text( "@91001020" )


	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end	--============================================================
--===============		NPC 지 브릴 ( 7.2단품퀘스트)
--============================================================

function	NPC_qeest_single_gbril_contact()
	-- 다이얼로그 출력
	dlg_title( "@91001021" )
	dlg_text( "@91001023" )


	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end	

-- <(version:7.2)>  해고통보
function quest_start_3323()

	local count = find_item( 1000156 )
	
	if count == 0 then
	insert_item( 1000156, 1 )
	else
	end
end



-- <(version:7.2)> 해고통보
function quest_end_3323()

	local count = find_item( 1000156)
	
	if count == 1 then
	delete_item( get_item_handle(1000156), 1 )
	else
	end
end


-- <(version:7.2)>  위기의 대장장이
function quest_start_3325()

	local count = find_item( 1000157 )
	
	if count == 0 then
	insert_item( 1000157, 1 )
	else
	end
end

-- <(version:7.2)> 위기의 대장장이
function quest_end_3325()

	local count = find_item( 1000157)
	
	if count == 1 then
	delete_item( get_item_handle(1000157), 1 )
	else
	end
end

-- <(version:7.2)>  지브릴의 책 조사1
function quest_start_3328()

	local count = find_item( 1000158 )
	
	if count == 0 then
	insert_item( 1000158, 1 )
	else
	end
end


-- <(version:7.2)> 지브릴의 책 조사2
function quest_end_3329()

	local count = find_item( 1000158)
	
	if count == 1 then
	delete_item( get_item_handle(1000158), 1 )
	else
	end
end



-- <(version:7.2)>  마법해제 2
function quest_start_3331()

	local count = find_item( 1000159 )
	
	if count == 0 then
	insert_item( 1000159, 1 )
	else
	end
end

-- <(version:7.2)> 마법해제 2
function quest_end_3331()

	local count = find_item( 1000159)
	
	if count == 1 then
	delete_item( get_item_handle(1000159), 1 )
	else
	end
end


-- <(version:7.2)>  부메랑
function quest_start_3332()

	local count = find_item( 1000156 )
	
	if count == 0 then
	insert_item( 1000156, 1 )
	else
	end
end

-- <(version:7.2)> 부메랑
function quest_end_3332()

	local count = find_item( 1000156)
	
	if count == 1 then
	delete_item( get_item_handle(1000156), 1 )
	else
	end
end

--============================================================
--===============		NPC 마스터 사탄 소녀 ( 7.3 )
--============================================================

function quest_start_3336()

	dlg_title( "@91001001" )
	dlg_text( "@91002484" )

	warp( 138645, 75174, gv("layer") )

	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()

end

   --============================================================
   --===============		대륙 무역 조합원 ( 8.1 불카누스 던전 )
   --============================================================
-- 론도 : 프란츠
function NPC_Vulcanus_Rondoh_init()

	cprint( "!불카누스 던전 NPC 프란츠 가동" )
	set_npc_name(  "@90604700"  )
end

function NPC_Vulcanus_Rondoh_contact()

	-- 다이얼로그 출력
	dlg_title( "@90604701" )
	dlg_text( "@90604702" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

-- 라크시 : 알폰소
function NPC_Vulcanus_Deva_init()

	cprint( "!불카누스 던전 NPC 알폰소 가동" )
	set_npc_name(  "@90604783"  )
end

function NPC_Vulcanus_Deva_contact()

	-- 다이얼로그 출력
	dlg_title( "@90604784" )
	dlg_text( "@90604702" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

--카탄 : 피르케
function NPC_Vulcanus_Asura_init()

	cprint( "!불카누스 던전 NPC 피르케 가동" )
	set_npc_name(  "@90604785"  )
end

function NPC_Vulcanus_Asura_contact()

	-- 다이얼로그 출력
	dlg_title( "@90604786" )
	dlg_text( "@90604702" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

--호라이즌 : 에인세
function NPC_Vulcanus_Gaia_init()

	cprint( "!불카누스 던전 NPC 에인세 가동" )
	set_npc_name(  "@90604787"  )
end

function NPC_Vulcanus_Gaia_contact()

	-- 다이얼로그 출력
	dlg_title( "@90604788" )
	dlg_text( "@90604702" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end





   --============================================================
   --=============== <8.1>불카누스 던전 퀘스트 - 일반 몹
   --============================================================
function Vulcanus_hunt_collect_quest()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3368 = get_quest_progress(3368) -- 던전탐사 : 고대인의 유물 1개 수집
	local quest_progress_3369 = get_quest_progress(3369) -- 고대인의 유산 : 불카누스의 일지 상,중,하 수집
	local quest_progress_3370 = get_quest_progress(3370) -- 불카누스의 열쇠 : 열쇠 몸통,머리,보석
	local quest_progress_3373 = get_quest_progress(3373) -- 탄생의 비밀 : 몬스터의 알 20개 수집
	local quest_progress_3374 = get_quest_progress(3374) -- 탄생의 비밀-재료수집 : 몬스터의 알 20개 수집
	local quest_progress_3375 = get_quest_progress(3375) -- 저주받은 나무 : 나무껍질 20개 수집
	local quest_progress_3376 = get_quest_progress(3376) -- 저주받은 나무-재료수집 : 나무껍질 20개 수집
	local quest_progress_3377 = get_quest_progress(3377) -- 고대인의 흔적 : 고대석판 20개 수집
	local quest_progress_3378 = get_quest_progress(3378) -- 고대인의 흔적-자료수집 : 고대석판 20개 수집
	local shout = math.random(1, 10)

	if quest_progress_3368 == 1 then  -- 던전탐사 : 고대인의 유물 1개 획득
		if shout >= 9 then
		
			local count = find_item ( 1000406 )
			if count == 0 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000406, 1 )
			end
		end
	end

	if quest_progress_3369 == 1 then  -- 고대인의 유산 : 불카누스의 일지 상,중,하
		if shout == 1 or shout == 2 then
		
			--불카누스 일지 상
			local count = find_item ( 1000407 )
			if count == 0 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000407, 1 )
			end
		  
		elseif shout == 3 or shout == 4 then
		
			--불카누스 일지 중
			local count = find_item ( 1000408 )
			if count == 0 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000408, 1 )
			end
		  
		elseif shout == 5 or shout == 6 then
		
			--불카누스 일지 하
			local count = find_item ( 1000409 )
			if count == 0 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000409, 1 )
			end
		end
	end

	if quest_progress_3370 == 1 then  -- 불카누스의 열쇠 : 열쇠 몸통,머리,보석
		if shout == 1 or shout == 2 then
		
			--불카누스 열쇠 몸통
			local count = find_item ( 1000418 )
			if count == 0 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000418, 1 )
			end
		  
		elseif shout == 3 or shout == 4 then
		
			--불카누스 열쇠 머리
			local count = find_item ( 1000419 )
			if count == 0 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000419, 1 )
			end
		  
		elseif shout == 5 or shout == 6 then
		
			--불카누스 열쇠 보석
			local count = find_item ( 1000420 )
			if count == 0 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000420, 1 )
			end
		end
	end

	if quest_progress_3373 == 1 or quest_progress_3374 == 1 then  -- 탄생의 비밀 : 몬스터의 알 20개 수집 (반복퀘 포함)
		if shout > 5 then

			local count = find_item ( 1000412 )
			if count < 20 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000412, 1 )
			end
		end
	end

	if quest_progress_3375 == 1 or quest_progress_3376== 1 then  -- 저주받은 나무 : 나무껍질 20개 수집 (반복퀘 포함)
		if shout > 5 then

			local count = find_item ( 1000413 )
			if count < 20 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000413, 1 )
			end
		end
	end

	if quest_progress_3377 == 1 or quest_progress_3378 == 1 then  -- 고대인의 흔적 : 고대석판 20개 수집 (반복퀘 포함)
		if shout > 5 then

			local count = find_item ( 1000414 )
			if count < 20 then
			drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000414, 1 )
			end
		end
	end
end

   --============================================================
   --============= <8.1>불카누스 던전 퀘스트 - 보스 몹 : 불카누스
   --============================================================
function Vulcanus_boss_hunt_quest()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3371 = get_quest_progress(3371) -- 진실의 상자 : 불카누스의 심장 1개 수집
	local quest_progress_3379 = get_quest_progress(3379) -- 생명의 원천 : 빛의 크리스탈, 어둠의 크리스탈, 정령석 1개씩 수집
	local quest_progress_3380 = get_quest_progress(3380) -- 생명의 원천-끝없는부활 : 빛의 크리스탈, 어둠의 크리스탈, 정령석 1개씩 수집
	local quest_progress_3384 = get_quest_progress(3384) -- [일일]생명의 원천-끝없는부활 : 빛의 크리스탈, 어둠의 크리스탈, 정령석 1개씩 수집
	local shout = math.random(1, 10)

	--불카누스 타임 레코드 
	local create_time = get_instance_dungeon_flag( 20000, get_value("layer"), 'date' ) -- on_create_vulcanus_instance( layer ) 함수에서 세팅한 던전 시작 시간 불러옴
	local leave_time = get_os_time() -- 던전 종료 시간 불러옴

	local country_time = get_os_date("%H",0)
	local turnaround_time = (leave_time - create_time) -- 소요시간 계산
	local print_turnaround_time	= turnaround_time +(24 -country_time) * 3600 
	
	local best_vul_record = get_global_variable( "best_vul_record" )  --현재 서버내 최고기록 불러옴
	local best_vul_record_name = get_global_variable("best_vul_record_name") --현재 서버내 최고기록 보유자 불러옴
	
	cprint(sconv("@690000084", "#@create_time@#", get_os_date("%c", create_time ))) -- 던전 시작시간 출력
	cprint(sconv("@690000085", "#@leave_time@#", get_os_date("%c", leave_time ))) -- 던전 종료시간 출력

	
	--최고기록이 갱신될 경우 기록과 기록 보유자 저장, 알림
	if best_vul_record == nil or best_vul_record == "" or best_vul_record > turnaround_time then 
		set_global_variable( "best_vul_record", turnaround_time )
		set_global_variable( "best_vul_record_name", get_value("name") )
		
		cprint(sconv("@690000086", "#@turnaround_time@#", get_os_date("%X", print_turnaround_time )))
		announce(sconv("@690000083", "#@user_name@#", get_value("name"),"#@vul_record@#",get_os_date("%X", print_turnaround_time )))
	
	--최고기록보다 저조한 기록일 경우 단순 알림
	elseif best_vul_record < turnaround_time or best_vul_record == turnaround_time then
		cprint(sconv("@690000086", "#@turnaround_time@#", get_os_date("%X", print_turnaround_time)))
	
	--소요시간이 24시간이 넘어갈 경우 알림
	elseif turnaround_time > 24*60*60 then
		cprint("@690000087")
	
	end
	
	if quest_progress_3371 == 1 then  -- 진실의 상자 : 불카누스의 심장 1개 획득
		
		local count = find_item ( 1000411 )
		if count == 0 then
		drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000411, 1 )
		
		cprint( "@90604916" ) -- <(version:7.3)><#6DD66D>그런가...... 억겁의 세월이 ...... 흘렀구나. 슈마즈의 손길을 피하기 위해.... 우리의 흔적을 남기기 위해 나의 생명을 버린 것이 성공한 것이구나.
		cprint( "@90604917" ) -- <(version:7.3)><#6DD66D>나의 기록을 되짚어 나가는 이여, 진실의 상자는 나의 심장......이다.
		
		end
	end
	
	if quest_progress_3379 == 1 or quest_progress_3380 == 1 or quest_progress_3384 == 1 then  -- 생명의 원천 : 빛의 크리스탈, 어둠의 크리스탈, 정령석 1개씩 수집 (반복퀘 포함)
		drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000415, 1 )		
		drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000416, 1 )		
		drop_item( get_target_value('x'), get_target_value('y'), get_target_value('layer'), 1000417, 1 )
	end
end




-- <(version:8.1)>새로운 게이트 조사 start
function quest_start_3337()

	local count = find_item( 1000424 )
	
		for i = 0, 10 do
			-- 남은 아이템 없애기
			delete_item( get_item_handle( 1000424 ), 1 )
		end	
		
	if count == 0 then
	insert_item( 1000424, 1 )
	cprint( "@90604745" ) -- <(version:8.1)><#6DD66D>대륙무역조합 공고문을 받았습니다.
	add_field_prop ( 126022, 180, 140398, 106190 ) 
	else
	end
end


-- <(version:8.1)>새로운 게이트 조사 end
function quest_end_3337()

	local count = find_item( 1000424)
	
	if count == 1 then
	delete_item( get_item_handle(1000424), 1 )
	else
	end
end

-- <(version:8.3)>새로운 게이트 조사 drop
function quest_drop_3337()

	local count = find_item( 1000424)
	
	if count == 1 then
	delete_item( get_item_handle(1000424), 1 )
	else
	end
end

-- <(version:8.1)>새로운 게이트 조사
--function enter_vulcanus()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	--local quest_progress_3367 = get_quest_progress(3367)

	--if quest_progress_3367 == 1 then
	--set_quest_status( 3367, 1, 1 )
	--cprint( "@90604746" ) -- <(version:8.1)><#6DD66D>새로운 게이트를 발견하였습니다.
	--end
--end


-- <(version:8.1)>진실의 상자 start
function quest_start_3338()

	local count = find_item( 1000423 )
	
		for i = 0, 10 do
			-- 남은 아이템 없애기
			delete_item( get_item_handle( 1000423 ), 1 )
		end	
	
	if count == 0 then
	insert_item( 1000423, 1 )
	cprint( "@90604748" ) -- <(version:8.1)><#6DD66D>복원된 불카누스의 마법열쇠를 받았습니다.
	else
	end
end


-- <(version:8.1)>진실의 상자 end
function quest_end_3338()

	local count = find_item( 1000423)
	
	if count == 1 then
	delete_item( get_item_handle(1000423), 1 )
	else
	end
end

-- <(version:8.3)>탄생의 비밀 - 실험용 재료 수집: 클리어 링크
function quest_end_3374()

	insert_gold( 160*get_value( "level" )*12.5 )
	--보상 Exp/Jp 일일 퀘스트 보상으로 지급될때만 가능한 수치다. 일일 퀘스트 보상에만 사용
	add_exp_jp( (get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2), ((get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2))/5 )
	cprint( '@1240' )
	reset_finished_quest( 3374 )
	
end

-- <(version:8.3)>저주받은 나무 - 실험용 재료 수집: 클리어 링크
function quest_end_3376()

	insert_gold( 160*get_value( "level" )*12.5 )
	--보상 Exp/Jp 일일 퀘스트 보상으로 지급될때만 가능한 수치다. 일일 퀘스트 보상에만 사용
	add_exp_jp( (get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2), ((get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2))/5 )
	cprint( '@1240' )
	reset_finished_quest( 3376 )

end

-- <(version:8.3)>고대인의 흔적 - 분석용 자료 수집: 클리어 링크
function quest_end_3378()	
	insert_gold( 160*get_value( "level" )*12.5 )
	--보상 Exp/Jp 일일 퀘스트 보상으로 지급될때만 가능한 수치다. 일일 퀘스트 보상에만 사용
	add_exp_jp( (get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2), ((get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2))/5 )
	cprint( '@1240' )
	reset_finished_quest( 3378 )
end

function quest_end_3384()
	--보상 Exp/Jp 일일 퀘스트 보상으로 지급될때만 가능한 수치다. 일일 퀘스트 보상에만 사용
	add_exp_jp( (get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2), ((get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2)*(get_value( "level" )/2))/5 )
	cprint( '@1240' )
	reset_finished_quest( 3384 )
end





-- <(version:8.1)>유물상자 ( 반복퀘스트용 랜덤 보상 아이템 )
function Vulcanus_random_box()

	local level = get_value( "level" )

	local item_code  -- 2013010 데바의 축복 아바타 귀속
	local item_code2 -- 2013011 공격의 루나칩 1시간 아바타 귀속
	local item_code3 -- -540027 노말 베이직 크리처 카드
	--local item_code4
	
	local v_count
	local v_count2
	local v_count3
	--local v_count4
	
	
	if level >= 30 and level < 40  then
		item_code = 2013010							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count = 1
		
		item_code2 = 2013011							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count2 = 1
		
		item_code3 = -541007							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count3 = 1
		
	elseif level >= 40 and level < 60 then
		item_code = 2013010							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count = 1
		
		item_code2 = 2013011							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count2 = 1
		
		item_code3 = -541007							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count3 = 1
		
	elseif level >= 60 and level < 80 then
		item_code = 2013010							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count = 1
		
		item_code2 = 2013011							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count2 = 1
		
		item_code3 = -540027							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count3 = 1
		
	elseif level >= 80 and level < 100 then
		item_code = 2013010							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count = 1
		
		item_code2 = 2013011							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count2 = 1
		
		item_code3 = -540027							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count3 = 1
		
	elseif level >= 100 and level < 120 then
		item_code = 2013010							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count = 1
		
		item_code2 = 2013011							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count2 = 1
		
		item_code3 = -540027							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count3 = 1
		
	elseif level >= 120 and level < 140 then
		item_code = 2013010							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count = 1
		
		item_code2 = 2013011							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count2 = 1
		
		item_code3 = -540027							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count3 = 1
		
	elseif level >= 140 and level < 160 then
		item_code = 2013010							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count = 1
		
		item_code2 = 2013011							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count2 = 1
		
		item_code3 = -540027							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count3 = 1
		
	elseif level >= 160 then
		item_code = 2013010							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count = 1
		
		item_code2 = 2013011							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count2 = 1
		
		item_code3 = -540027							-- 여기에 레벨구간별로 적정한 보상 아이템을 넣어주세요~
		v_count3 = 1
		
	end
	
	
	insert_item( item_code, v_count )
	local text = sconv( "@90604789", "#@item_name@#", "@" .. tostring( item_code + 10000000 ) )
	cprint( text )
	
	insert_item( item_code2, v_count2 )
	local text = sconv( "@90604789", "#@item_name@#", "@" .. tostring( item_code2 + 10000000 ) )
	cprint( text )
	
	item_code3 = -541007
	local item = pick_item_in_drop_group( item_code3 )
	local item_name = get_item_name_id( item[1] )
	
	insert_item( item[1], v_count3 )
	local text = sconv( "@90604789", "#@item_name@#", "@" .. tostring( item_name ) )
	--local text = sconv( "@90604789", "#@item_name@#", "@" .. tostring( 7224 ) )
	cprint( text )
end

-- 프랍 링크
------------------------------------------------------------------------------------------
function quest_prop_Vulcanus_30179()

	--<(version:8.1)>새로운 게이트 조사
	--set_quest_status( 3367, 1, 1 )
	
	cprint( "@90604913" ) --<(version:7.3)><#6DD66D>처음 발견된 불카누스 입구로 관련 정보 조사에 큰 영향을 미쳤다. 지금은 입구로서의 작동은 하지 않는다.
	
end



--============================================================
--===============================================
--===============		TP 퀘스트
--===============================================
--============================================================


-- 프랍 링크
------------------------------------------------------------------------------------------
function quest_prop_summon_creature_30171()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3352 = get_quest_progress(3352) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-1
	local quest_progress3353 = get_quest_progress(3353) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-2
	local quest_progress3354 = get_quest_progress(3354) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-3
	local quest_progress3355 = get_quest_progress(3355) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-4
	local quest_progress3356 = get_quest_progress(3356) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-5
	
	local state_level = get_state_level( 4554 ) --위협수준 증가
		
	local monster_num
	
	if state_level == 1 then
	
		cprint( "@90604778" ) --<(version:7.3)><#6DD66D>분노의 기운으로 인하여 몬스터를 소환할 수 없습니다.
		
	
	else 
		-- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-1
		if quest_progress3352 == 1 then
			
			monster_num = math.random( 1, 3 )
			
			if monster_num == 1 then
				-- 판테라 소환
				add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 110011,  1 , 55000 )
			elseif monster_num == 2 then
				-- 토르터스 소환
				add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 110012,  1 , 55000 )
			elseif monster_num == 3 then
				--	 폴트리 소환
				add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 110013,  1 , 55000 )
			end
		
		
		add_state( 4554 , 1, 55000) -- 위협수준 증가
		cprint( "@90604777" ) --<(version:7.3)><#6DD66D>크리쳐 왕들의 분노로 위협 수준이 높아 집니다.
		
		-- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-2	
		elseif quest_progress3353 == 1 then
		
			monster_num = math.random( 1, 8 )
			
				if monster_num == 1 then
					-- 오크 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 120004,  1 , 55000 )
				elseif monster_num == 2 then
					-- 예티 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 120005,  1 , 55000 )
				elseif monster_num == 3 then
					-- 스켈레톤 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 120006,  1 , 55000 )
				elseif monster_num == 4 then
					-- 레드픽시 소환
					add_npc( gv('x') 	+ math.random( 10, 50), gv('y') + math.random( 10, 50), 120007,  1 , 55000 )
				elseif monster_num == 5 then
					-- 블루픽시 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 120008,  1 , 55000 )
				elseif monster_num == 6 then
					-- 세이렌 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 120009,  1 , 55000 )
				elseif monster_num == 7 then
					-- 울프 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 120010,  1 , 55000 )
				elseif monster_num == 8 then
					-- 크라켄 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 120011,  1 , 55000 )
				end
		
		add_state( 4554 , 1, 55000) -- 위협수준 증가
		cprint( "@90604777" ) --<(version:7.3)><#6DD66D>크리쳐 왕들의 분노로 위협 수준이 높아 집니다.
		
		-- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-3	
		elseif quest_progress3354 == 1 then
		
			monster_num = math.random( 1, 5 )
			
				if monster_num == 1 then
					-- 호크맨 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 130011,  1 , 60000 )
				elseif monster_num == 2 then
					-- 샐러맨더 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 130012,  1 , 60000 )
				elseif monster_num == 3 then
					-- 윈드송 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 130013,  1 , 60000 )
				elseif monster_num == 4 then
					-- 유니콘 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 130014,  1 , 60000 )
				elseif monster_num == 5 then
					-- 다크혼 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 130015,  1 , 60000 )
								
				end
		
		add_state( 4554 , 1, 60000) -- 위협수준 증가
		cprint( "@90604777" ) --<(version:7.3)><#6DD66D>크리쳐 왕들의 분노로 위협 수준이 높아 집니다.
		
		-- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-4
		elseif quest_progress3355 == 1 then
		
			monster_num = math.random( 1, 5 )
			
				if monster_num == 1 then
					-- 켄타우로스 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 140008,  1 , 60000 )
				elseif monster_num == 2 then
					-- 아이무스 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 140009,  1 , 60000 )
				elseif monster_num == 3 then
					-- 타파리 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 140010,  1 , 60000 )
				elseif monster_num == 4 then
					-- 카이넨 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 140011,  1 , 60000 )
				elseif monster_num == 5 then
					-- 엔젤 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 140012,  1 , 60000 )
				end
				
		add_state( 4554 , 1, 60000) -- 위협수준 증가
		cprint( "@90604777" ) --<(version:7.3)><#6DD66D>크리쳐 왕들의 분노로 위협 수준이 높아 집니다.
		
		-- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-5
		elseif quest_progress3356 == 1 then
		
			monster_num = math.random( 1, 4 )
			
				if monster_num == 1 then
					-- 켈베로스 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 150021,  1 , 65000 )
				elseif monster_num == 2 then
					-- 지니 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 150022,  1 , 65000 )
				elseif monster_num == 3 then
					-- 미스틱 지니 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 150023,  1 , 65000 )
				elseif monster_num == 4 then
					-- 큐브 소환
					add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), 150025,  1 , 65000 )
				end
		
		add_state( 4554 , 1, 65000) -- 위협수준 증가
		cprint( "@90604777" ) --<(version:7.3)><#6DD66D>크리쳐 왕들의 분노로 위협 수준이 높아 집니다.
		
		else
		
			cprint( "@90604912" ) --<(version:7.3)>소환진을 사용할 수 없습니다.
		
		end
		
		
		
	end
end

function quest_prop_summon_creature_30172()

	local state_level = get_state_level( 4554 ) --위협수준 증가
		
		
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3357 = get_quest_progress(3357) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #2
	
	local monster_id
	local selected = { 110011, 110012, 110013, 
					120004, 120005, 120006, 120007, 120008, 120009, 120010, 120011,
					130011, 130012, 130013, 130014, 130015,
					140008, 140009, 140010, 140011, 140012,
					150021, 150022, 150023, 150025,
					160012, 160013, 160014 }
		
	for i = 1, 2 do
			monster_id = math.random( i, 28 )
			local temp = selected[ i ]
			selected[ i ] = selected[ monster_id ]
			selected[ monster_id ] = temp
	end
	
	if state_level == 1 then	
	
		cprint( "@90604778" ) --<(version:7.3)><#6DD66D>분노의 기운으로 인하여 몬스터를 소환할 수 없습니다.
						
	else
	
		if quest_progress3357 == 1 then
		
			monster_id = selected[ 1 ]
			
			-- 몬스터 소환 1
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 100000 )
			
			monster_id = selected[ 2 ]
			
			-- 몬스터 소환 2
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 100000 )
		
		add_state( 4554 , 1, 100000) -- 위협수준 증가
		cprint( "@90604777" ) --<(version:7.3)><#6DD66D>크리쳐 왕들의 분노로 위협 수준이 높아 집니다.
		
		else 
			cprint( "@90604776" )--<(version:7.3)><#6DD66D>해당 퀘스트를 진행 중에만 사용 가능 합니다.
		end

	end
		
end

function quest_prop_summon_creature_30173()

	local state_level = get_state_level( 4554 ) --위협수준 증가
	
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3358 = get_quest_progress(3358) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #3
	
	local monster_id
	local selected = { 110011, 110012, 110013, 
					120004, 120005, 120006, 120007, 120008, 120009, 120010, 120011,
					130011, 130012, 130013, 130014, 130015,
					140008, 140009, 140010, 140011, 140012,
					150021, 150022, 150023, 150025,
					160012, 160013, 160014 }
		
	for i = 1, 3 do
			monster_id = math.random( i, 28 )
			local temp = selected[ i ]
			selected[ i ] = selected[ monster_id ]
			selected[ monster_id ] = temp
	end
	
	if state_level == 1 then
	
		cprint( "@90604778" ) --<(version:7.3)><#6DD66D>분노의 기운으로 인하여 몬스터를 소환할 수 없습니다.
						
	else
		
		if quest_progress3358 == 1 then
		
			monster_id = selected[ 1 ]
			
			-- 몬스터 소환 1
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 100000 )
			
			monster_id = selected[ 2 ]
			
			-- 몬스터 소환 2
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 100000 )
			
			monster_id = selected[ 3 ]
			
			-- 몬스터 소환 3
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 100000 )
			
			add_state( 4554 , 1, 100000) -- 위협수준 증가
			cprint( "@90604777" ) --<(version:7.3)><#6DD66D>크리쳐 왕들의 분노로 위협 수준이 높아 집니다.
		
		else 
			cprint( "@90604776" )--<(version:7.3)><#6DD66D>해당 퀘스트를 진행 중에만 사용 가능 합니다.
			
		end
		
	end
end

function quest_prop_summon_creature_30174()

	local state_level = get_state_level( 4554 ) --위협수준 증가
	
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3359 = get_quest_progress(3359) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #5
	
	
	local monster_id
	local selected = { 110011, 110012, 110013, 
					120004, 120005, 120006, 120007, 120008, 120009, 120010, 120011,
					130011, 130012, 130013, 130014, 130015,
					140008, 140009, 140010, 140011, 140012,
					150021, 150022, 150023, 150025,
					160012, 160013, 160014 }
		
	for i = 1, 5 do
			monster_id = math.random( i, 28 )
			local temp = selected[ i ]
			selected[ i ] = selected[ monster_id ]
			selected[ monster_id ] = temp
	end
	
	if state_level == 1 then
	
		cprint( "@90604778" ) --<(version:7.3)><#6DD66D>분노의 기운으로 인하여 몬스터를 소환할 수 없습니다.
		
	else
	
		if quest_progress3359 == 1 then 
					
			monster_id = selected[ 1 ]
			
			-- 몬스터 소환 1
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 150000 )
			
			monster_id = selected[ 2 ]
			
			-- 몬스터 소환 2
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 150000 )
			
			monster_id = selected[ 3 ]
			
			-- 몬스터 소환 3
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 150000 )
			
			monster_id = selected[ 4 ]
			
			-- 몬스터 소환 4
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 150000 )
			
			monster_id = selected[ 5 ]
			
			-- 몬스터 소환 5
			add_npc( gv('x') + math.random( 10, 50), gv('y') + math.random( 10, 50), monster_id,  1 , 150000 )
			
			add_state( 4554 , 1, 150000) -- 위협수준 증가
			cprint( "@90604777" ) --<(version:7.3)><#6DD66D>크리쳐 왕들의 분노로 위협 수준이 높아 집니다.
		
		else 
			
			add_state(6006, 1, 500) -- 스턴 
			cprint( "@90604776" )--<(version:7.3)><#6DD66D>해당 퀘스트를 진행 중에만 사용 가능 합니다.
			
		end	
		
	end
	
end

function quest_prop_summon_creature_30175()

		
end

function quest_prop_summon_creature_30176()

		
end

function quest_prop_summon_creature_30177()

		
end

function quest_prop_summon_creature_30178()

		
end

------------------3350, <(version:7.3)>탐험의 왕------------------------------------------
function quest_prop_mana_stone(num) 
	
	local tp_finding = get_quest_status( 3350 , 1 ) + num
	
	set_quest_status( 3350, 1, tp_finding )
	
end
-- 퀘스트 링크
------------------------------------------------------------------------------------------
function quest_end_3350()

	local talentp = get_value( 'tp' )
	talentp = talentp + 1
	set_value( "tp", talentp )
	
	cprint( "@90604780" ) -- <(version:7.3)><#6DD66D>TP 포인트가 1 지급 되었습니다.
	
	
	del_flag( "over_bluepoint_lakcity" )
	del_flag( "over_bluepoint_horizon" )
	del_flag( "over_bluepoint_katan" )
	del_flag( "over_bluepoint_rondoh" )
	
	del_flag( "over_redpoint_lakcity" )
	del_flag( "over_redpoint_horizon" )
	del_flag( "over_redpoint_katan" )
	del_flag( "over_redpoint_rondoh" )
	
	del_flag( "over_greenpoint_lakcity" )
	del_flag( "over_greenpoint_horizon" )
	del_flag( "over_greenpoint_katan" )
	del_flag( "over_greenpoint_rondoh" )
	
end

------------------------------------------------------------------------------------------

function quest_end_3351()

	local talentp = get_value( 'tp' )
	talentp = talentp + 1
	set_value( "tp", talentp )
	
	cprint( "@90604780" ) -- <(version:7.3)><#6DD66D>TP 포인트가 1 지급 되었습니다.
		
end
------------------------------------------------------------------------------------------
function quest_end_3359()

	local talentp = get_value( 'tp' )
	talentp = talentp + 1
	set_value( "tp", talentp )
	
	cprint( "@90604780" ) -- <(version:7.3)><#6DD66D>TP 포인트가 1 지급 되었습니다.
	
end

------------------------------------------------------------------------------------------
function quest_start_3381()

	-- 대륙의 과거 엘로힘 시아페에게 워프 
	warp( 127542 + math.random(0,10) , 121326 + math.random(0,10) )
		
end

function quest_end_3381()


end

------------------------------------------------------------------------------------------
function quest_start_3382()
	
	-- 대륙의 과거 사막지기 엘레노에게 워프
	warp( 120752 + math.random(0,10) , 86799 + math.random(0,10) )
		
end

function quest_end_3382()

		
end

------------------------------------------------------------------------------------------
function quest_start_3383()
	
	-- 대륙의 과거 무녀제이나에게 워프 
	warp( 120321 + math.random(0,10) , 72102 + math.random(0,10) )
		
end

function quest_end_3383()

		
end


-- 몬스터 링크
------------------------------------------------------------------------------------------
function tp_quest_creature_adventure_1_1()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3352 = get_quest_progress(3352) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-1
	--local quest_progress3353 = get_quest_progress(3353) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-2
	--local quest_progress3354 = get_quest_progress(3354) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-3
	--local quest_progress3355 = get_quest_progress(3355) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-4
	--local quest_progress3356 = get_quest_progress(3356) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-5
	
	local quest_progress3357 = get_quest_progress(3357) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #2
	local quest_progress3358 = get_quest_progress(3358) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #3
	local quest_progress3359 = get_quest_progress(3359) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #5
	
		
	if quest_progress3352 == 1 then
	
		local tp_creature_monster = get_quest_status( 3352 , 1 ) + 1
	
		set_quest_status( 3352, 1, tp_creature_monster )
	
	end
	
	
		
	if quest_progress3357 == 1 then
	
		local tp_creature_monster = get_quest_status( 3357 , 1 ) + 1
	
		set_quest_status( 3357, 1, tp_creature_monster )
	
	end
		
	if quest_progress3358 == 1 then
	
		local tp_creature_monster = get_quest_status( 3358 , 1 ) + 1
	
		set_quest_status( 3358, 1, tp_creature_monster )
	
	end
	
	if quest_progress3359 == 1 then
	
		local tp_creature_monster = get_quest_status( 3359 , 1 ) + 1
	
		set_quest_status( 3359, 1, tp_creature_monster )
	
	end
	
end


function tp_quest_creature_adventure_1_2()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	--local quest_progress3352 = get_quest_progress(3352) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-1
	local quest_progress3353 = get_quest_progress(3353) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-2
	--local quest_progress3354 = get_quest_progress(3354) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-3
	--local quest_progress3355 = get_quest_progress(3355) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-4
	--local quest_progress3356 = get_quest_progress(3356) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-5
	
	local quest_progress3357 = get_quest_progress(3357) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #2
	local quest_progress3358 = get_quest_progress(3358) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #3
	local quest_progress3359 = get_quest_progress(3359) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #5
	
				
	if quest_progress3353 == 1 then
	
		local tp_creature_monster = get_quest_status( 3353 , 1 ) + 1
	
		set_quest_status( 3353, 1, tp_creature_monster )
	
	end
			
	if quest_progress3357 == 1 then
	
		local tp_creature_monster = get_quest_status( 3357 , 1 ) + 1
	
		set_quest_status( 3357, 1, tp_creature_monster )
	
	end
		
	if quest_progress3358 == 1 then
	
		local tp_creature_monster = get_quest_status( 3358 , 1 ) + 1
	
		set_quest_status( 3358, 1, tp_creature_monster )
	
	end
	
	if quest_progress3359 == 1 then
	
		local tp_creature_monster = get_quest_status( 3359 , 1 ) + 1
	
		set_quest_status( 3359, 1, tp_creature_monster )
	
	end
	
end


function tp_quest_creature_adventure_1_3()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	--local quest_progress3352 = get_quest_progress(3352) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-1
	--local quest_progress3353 = get_quest_progress(3353) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-2
	local quest_progress3354 = get_quest_progress(3354) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-3
	--local quest_progress3355 = get_quest_progress(3355) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-4
	--local quest_progress3356 = get_quest_progress(3356) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-5
	
	local quest_progress3357 = get_quest_progress(3357) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #2
	local quest_progress3358 = get_quest_progress(3358) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #3
	local quest_progress3359 = get_quest_progress(3359) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #5
	
				
	if quest_progress3354 == 1 then
	
		local tp_creature_monster = get_quest_status( 3354 , 1 ) + 1
	
		set_quest_status( 3354, 1, tp_creature_monster )
	
	end
			
	if quest_progress3357 == 1 then
	
		local tp_creature_monster = get_quest_status( 3357 , 1 ) + 1
	
		set_quest_status( 3357, 1, tp_creature_monster )
	
	end
		
	if quest_progress3358 == 1 then
	
		local tp_creature_monster = get_quest_status( 3358 , 1 ) + 1
	
		set_quest_status( 3358, 1, tp_creature_monster )
	
	end
	
	if quest_progress3359 == 1 then
	
		local tp_creature_monster = get_quest_status( 3359 , 1 ) + 1
	
		set_quest_status( 3359, 1, tp_creature_monster )
	
	end
	
end


function tp_quest_creature_adventure_1_4()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	--local quest_progress3352 = get_quest_progress(3352) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-1
	--local quest_progress3353 = get_quest_progress(3353) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-2
	--local quest_progress3354 = get_quest_progress(3354) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-3
	local quest_progress3355 = get_quest_progress(3355) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-4
	--local quest_progress3356 = get_quest_progress(3356) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-5
	
	local quest_progress3357 = get_quest_progress(3357) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #2
	local quest_progress3358 = get_quest_progress(3358) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #3
	local quest_progress3359 = get_quest_progress(3359) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #5
	
				
	if quest_progress3355 == 1 then
	
		local tp_creature_monster = get_quest_status( 3355 , 1 ) + 1
	
		set_quest_status( 3355, 1, tp_creature_monster )
	
	end
			
	if quest_progress3357 == 1 then
	
		local tp_creature_monster = get_quest_status( 3357 , 1 ) + 1
	
		set_quest_status( 3357, 1, tp_creature_monster )
	
	end
		
	if quest_progress3358 == 1 then
	
		local tp_creature_monster = get_quest_status( 3358 , 1 ) + 1
	
		set_quest_status( 3358, 1, tp_creature_monster )
	
	end
	
	if quest_progress3359 == 1 then
	
		local tp_creature_monster = get_quest_status( 3359 , 1 ) + 1
	
		set_quest_status( 3359, 1, tp_creature_monster )
	
	end
	
end


function tp_quest_creature_adventure_1_5()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	--local quest_progress3352 = get_quest_progress(3352) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-1
	--local quest_progress3353 = get_quest_progress(3353) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-2
	--local quest_progress3354 = get_quest_progress(3354) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-3
	--local quest_progress3355 = get_quest_progress(3355) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-4
	local quest_progress3356 = get_quest_progress(3356) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-5
	
	local quest_progress3357 = get_quest_progress(3357) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #2
	local quest_progress3358 = get_quest_progress(3358) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #3
	local quest_progress3359 = get_quest_progress(3359) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #5
	
				
	if quest_progress3356 == 1 then
	
		local tp_creature_monster = get_quest_status( 3356 , 1 ) + 1
	
		set_quest_status( 3356, 1, tp_creature_monster )
	
	end
			
	if quest_progress3357 == 1 then
	
		local tp_creature_monster = get_quest_status( 3357 , 1 ) + 1
	
		set_quest_status( 3357, 1, tp_creature_monster )
	
	end
		
	if quest_progress3358 == 1 then
	
		local tp_creature_monster = get_quest_status( 3358 , 1 ) + 1
	
		set_quest_status( 3358, 1, tp_creature_monster )
	
	end
	
	if quest_progress3359 == 1 then
	
		local tp_creature_monster = get_quest_status( 3359 , 1 ) + 1
	
		set_quest_status( 3359, 1, tp_creature_monster )
	
	end
	
end


function tp_quest_creature_adventure_1_6()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	--local quest_progress3352 = get_quest_progress(3352) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-1
	--local quest_progress3353 = get_quest_progress(3353) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-2
	--local quest_progress3354 = get_quest_progress(3354) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-3
	--local quest_progress3355 = get_quest_progress(3355) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-4
	--local quest_progress3356 = get_quest_progress(3356) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #1-5
	
	local quest_progress3357 = get_quest_progress(3357) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #2
	local quest_progress3358 = get_quest_progress(3358) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #3
	local quest_progress3359 = get_quest_progress(3359) -- <(version:7.3)><(version:7.3)>크리쳐의 왕 #5
	
	
	if quest_progress3357 == 1 then
	
		local tp_creature_monster = get_quest_status( 3357 , 1 ) + 1
	
		set_quest_status( 3357, 1, tp_creature_monster )
	
	end
		
	if quest_progress3358 == 1 then
	
		local tp_creature_monster = get_quest_status( 3358 , 1 ) + 1
	
		set_quest_status( 3358, 1, tp_creature_monster )
	
	end
	
	if quest_progress3359 == 1 then
	
		local tp_creature_monster = get_quest_status( 3359 , 1 ) + 1
	
		set_quest_status( 3359, 1, tp_creature_monster )
	
	end
	
end


---------------------------------------------------------
function quest_start_3600()

end

function quest_end_3600()

end


---------------------------------------------------------
-- <(version:7.4)>[큐브릭]로미오 구출
function quest_start_3601()

	set_flag( "romeo_finding" , math.random( 0, 2 ) )

end

function quest_end_3601()

end


---------------------------------------------------------
function quest_start_3602()

end

function quest_end_3602()

end

-- <(version:7.4)>[큐브릭]줄리엣 구출
---------------------------------------------------------
function quest_start_3603()

	set_flag( "juliet_finding" , math.random( 0, 2 ) )
	
end

function quest_end_3603()

end


---------------------------------------------------------
function quest_start_3604()

end

function quest_end_3604()

end


---------------------------------------------------------
function quest_start_3605()

end

function quest_end_3605()

end


---------------------------------------------------------
function quest_start_3606()

end

function quest_end_3606()

end




---------------------------------------------------------
function quest_start_3607()
	-- 던전 내에서 퀘스트를 새로 시작하면 퀘스트 진행하고 있는 유저 카운트 증가
	if get_instance_dungeon_id() == 30000 then

		local layer = gv( 'layer' )
		set_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount', get_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount' ) + 1 )

	end
end

function quest_end_3607()

end

function quest_drop_3607()

	-- 진행 중이었던 유저가 포기하면 던전 내에서 해당 퀘스트를 진행하고 있던 유저 카운트를 감소
	if get_instance_dungeon_id() == 30000 and get_quest_progress( 3607 ) == 1 then

		local layer = gv( 'layer' )
		set_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount', get_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount' ) - 1 )

	end
end

---------------------------------------------------------
-- function quest_start_3608()

	-- set_flag( "alram_count_01" , 0 )
	-- set_flag( "alram_count_02" , 0 )
	-- set_flag( "alram_count_03" , 0 )
	
	-- set_flag( "alram_count_04" , 0 )
	-- set_flag( "alram_count_05" , 0 )
	-- set_flag( "alram_count_06" , 0 )
	
-- end

-- function quest_end_3608()

-- end

---------------------------------------------------------
function quest_start_3609()

	-- 현재 인던 내부에서 이미 보스 몬스터가 리젠되었었다면 추가적으로 리젠하지 않게 차단
	local layer = gv( 'layer' )
	if get_instance_dungeon_id() == 30000 and get_instance_dungeon_flag( 30000, layer, 'AdditionalBossRespawned' ) == 0 then

		set_instance_dungeon_flag( 30000, layer, 'AdditionalBossRespawned', 1 )

		add_npc( 99954 + math.random( 2, 50 ), 31539 + math.random( 2, 50 ), 20170002, 1, 30000 )
		add_npc( 99954 + math.random( 2, 50 ), 31539 + math.random( 2, 50 ), 20180002, 1, 30000 )
		add_npc( 99954 + math.random( 2, 50 ), 31539 + math.random( 2, 50 ), 20185002, 1, 30000 )

	end	
end

function quest_end_3609()

	-- 출구 프랍이 중복해서 여러 개 리젠되지 않도록 체크
	local layer = gv( 'layer' )
	if get_instance_dungeon_id() == 30000 and get_instance_dungeon_flag( 30000, layer, 'ExitPropRespawned' ) == 0 then

		set_instance_dungeon_flag( 30000, layer, 'ExitPropRespawned', 1 )
		add_field_prop(60102, 0, 99954, 31539, layer, 0 )

	end	
	
	--플래그 삭제
	-- del_flag( "alram_count_01" )
	-- del_flag( "alram_count_02" )
	-- del_flag( "alram_count_03" )
	-- del_flag( "alram_count_04" )
	-- del_flag( "alram_count_05" )
	-- del_flag( "alram_count_06" )
		
	del_flag( "juliet_finding" )
	del_flag( "romeo_finding" )	
	
end





function npc_quest_cubric_mullog()

	-- 다이얼로그 출력
	dlg_title( "@90604989" )
	dlg_text( "@90604990" )
    
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function npc_quest_cubric_livedy()

	-- 다이얼로그 출력
	dlg_title( "@90604988" )
	dlg_text( "@90604990" )
    
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function npc_quest_cubric_Unidentified01()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3601 = get_quest_progress(3601) -- <(version:7.4)>[큐브릭]로미오 구출
	local quest_progress3608 = get_quest_progress(3608) -- <(version:7.4)>[큐브릭]종식
	
	local romeo = get_flag( "romeo_finding")

	-- 다이얼로그 출력
	dlg_title( "@90605012" )
	dlg_text( "@90605013" )
	
	if quest_progress3601 == 1 then	
		if romeo == 0 then
			dlg_menu( "@90605022", 'romeo_question_real()' )		
		else
			dlg_menu( "@90605022", 'romeo_question_fake()' )						
		end 
			
	end
	
	if quest_progress3608 == 1 then	
		dlg_menu( "@90605057", 'alarm()' )					
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end

function romeo_question_real()
	
	-- 다이얼로그 출력
	dlg_title( "@90605012" )
	dlg_text( "@90605025" )
	
	dlg_menu( "@90605026", 'romeo_question_real_talk1()' )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
	
end

function romeo_question_real_talk1()
	
	-- 다이얼로그 출력
	dlg_title( "@90605012" )
	dlg_text( "@90605027" )
	
	dlg_menu( "@90605028", 'romeo_question_real_talk2()' )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

function romeo_question_real_talk2()
	
	-- <(version:7.4)>[큐브릭]로미오 구출
	set_quest_status( 3601, 1, 1 )
		
end


function npc_quest_cubric_Unidentified02()
    
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3601 = get_quest_progress(3601) -- <(version:7.4)>[큐브릭]로미오 구출
	local quest_progress3608 = get_quest_progress(3608) -- <(version:7.4)>[큐브릭]종식
	
	local romeo = get_flag( "romeo_finding")

	-- 다이얼로그 출력
	dlg_title( "@90605015" )
	dlg_text( "@90605016" )
	
	if quest_progress3601 == 1 then	
		if romeo == 1 then
			dlg_menu( "@90605022", 'romeo_question_real()' )		
		else
			dlg_menu( "@90605022", 'romeo_question_fake()' )		
		end 
		
	end
	
	if quest_progress3608 == 1 then	
		dlg_menu( "@90605057", 'alarm()' )		
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function npc_quest_cubric_Unidentified03()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3601 = get_quest_progress(3601) -- <(version:7.4)>[큐브릭]로미오 구출
	local quest_progress3608 = get_quest_progress(3608) -- <(version:7.4)>[큐브릭]종식
	
	local romeo = get_flag( "romeo_finding")

	-- 다이얼로그 출력
	dlg_title( "@90605018" )
	dlg_text( "@90605019" )
	
	if quest_progress3601 == 1 then	
		if romeo == 2 then
			dlg_menu( "@90605022", 'romeo_question_real()' )		
		else
			dlg_menu( "@90605022", 'romeo_question_fake()' )								
		end 
	end
	
	if quest_progress3608 == 1 then	
		dlg_menu( "@90605057", 'alarm()' )					
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " ) 
	dlg_show()
 
end


function romeo_question_fake()
	
	local romeo_dice = math.random(0, 1)
	
	-- 다이얼로그 출력
	dlg_title( "@90605012" )
	
	if romeo_dice == 0 then
		dlg_text( "@90605023" )
	else
		dlg_text( "@90605024" )
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " ) 
	dlg_show()
end



function npc_quest_cubric_Unidentified04()
			
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3603 = get_quest_progress(3603) -- <(version:7.4)>[큐브릭]줄리엣 구출
	local quest_progress3608 = get_quest_progress(3608) -- <(version:7.4)>[큐브릭]종식
	
	local juliet = get_flag( "juliet_finding")

	-- 다이얼로그 출력
	dlg_title( "@90605030" )
	dlg_text( "@90605031" )
	
	if quest_progress3603 == 1 then	
		if juliet == 0 then
			dlg_menu( "@90605047", 'juliet_question_real()' )		
		else
			dlg_menu( "@90605047", 'juliet_question_fake()' )					
		end 
			
	end
		
	if quest_progress3608 == 1 then	
		dlg_menu( "@90605057", 'alarm()' )					
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " ) 
	dlg_show()
 
end


function juliet_question_real()
	
	-- 다이얼로그 출력
	dlg_title( "@90605012" )
	dlg_text( "@90605050" )
	
	dlg_menu( "@90605051", 'juliet_question_real_talk1()' )
	dlg_menu( "@90010002", " " ) -- 대화종료 
 
	dlg_show()
end

function juliet_question_real_talk1()
	
	-- 다이얼로그 출력
	dlg_title( "@90605012" )
	dlg_text( "@90605052" )
	
	dlg_menu( "@90605053", 'juliet_question_real_talk2()' )
	dlg_menu( "@90010002", " " )-- 대화종료
 
	dlg_show()
end

function juliet_question_real_talk2()
	
	-- <(version:7.4)>[큐브릭]줄리엣 구출
	set_quest_status( 3603, 1, 1 )
		
end

function juliet_question_fake()
	
	local juliet_dice = math.random(0, 1)
	
	-- 다이얼로그 출력
	dlg_title( "@90605012" )
	
	if juliet_dice == 0 then
		dlg_text( "@90605048" )
	else
		dlg_text( "@90605049" )
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
	
end

function npc_quest_cubric_Unidentified05()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3603 = get_quest_progress(3603) -- <(version:7.4)>[큐브릭]줄리엣 구출
	local quest_progress3608 = get_quest_progress(3608) -- <(version:7.4)>[큐브릭]종식
	
	local juliet = get_flag( "juliet_finding")

	-- 다이얼로그 출력
	dlg_title( "@90605033" )
	dlg_text( "@90605034" )
	
	if quest_progress3603 == 1 then
		if juliet == 1 then
			dlg_menu( "@90605047", 'juliet_question_real()' )
		else
			dlg_menu( "@90605047", 'juliet_question_fake()' )
		end 
	end
		
	if quest_progress3608 == 1 then
		dlg_menu( "@90605057", 'alarm()' )
	end
	
	dlg_menu( "@90010002", " " ) -- 대화종료 
	dlg_show()
 
end


function npc_quest_cubric_Unidentified06()

	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3603 = get_quest_progress(3603) -- <(version:7.4)>[큐브릭]줄리엣 구출
	local quest_progress3608 = get_quest_progress(3608) -- <(version:7.4)>[큐브릭]종식
	local juliet = get_flag( "juliet_finding")

	-- 다이얼로그 출력
	dlg_title( "@90605036" )
	dlg_text( "@90605037" )
	
	if quest_progress3603 == 1 then
		if juliet == 2 then
			dlg_menu( "@90605047", 'juliet_question_real()' )
		else
			dlg_menu( "@90605047", 'juliet_question_fake()' )
		end
	end
		
	if quest_progress3608 == 1 then
		dlg_menu( "@90605057", 'alarm()' )
	end
	 
	dlg_menu( "@90010002", " " ) -- 대화종료
	dlg_show()
 
end

function alarm()

	dlg_title( "@90605036" )
	
	local dice = math.random( 0, 2 )
	if dice == 0 then
		dlg_text( "@90605058" )
	elseif dice == 1 then
		dlg_text( "@90605059" )
	elseif dice == 2 then
		dlg_text( "@90605060" )	
	end
	
	set_quest_status( 3608, 1, 1 ) -- <(version:7.4)>[큐브릭]종식
	
	dlg_menu( "@90010002", " " ) -- 대화종료
	dlg_show()
	
end

function npc_quest_cubric_into_livedy()

	-- 다이얼로그 출력
	dlg_title( "@90605039" )
	dlg_text( "@90605040" )
    
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function npc_quest_cubric_romeo()

	-- 다이얼로그 출력
	dlg_title( "@90605042" )
	dlg_text( "@90605043" )
    
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


function npc_quest_cubric_juliet()

	-- 다이얼로그 출력
	dlg_title( "@90605045" )
	dlg_text( "@90605046" )
    
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end

   --============================================================
   --===============		전령사 메호
   --============================================================

function NPC_Quest_slaugter1_init()
	set_npc_name(  "@90605392"  )
end



function NPC_Quest_slaugter1_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605392" )
	dlg_text( "@90605393" )

		
	--dlg_menu( "@90605517", 'NPC_Quest_warp()' )
				
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

function NPC_Quest_warp()

	-- 다이얼로그 출력
	dlg_title( "@90605392" )
	dlg_text( "@90605516" )

	local quest_warp = get_value( "level" )
	
	if quest_warp >= 80 then
	
		dlg_menu( "@90605512", 'NPC_Quest_warp_1()' )
	end
	
	if quest_warp >= 100 then
	
		dlg_menu( "@90605513", 'NPC_Quest_warp_2()' )
	end
	
	if quest_warp >= 120 then
	
		dlg_menu( "@90605514", 'NPC_Quest_warp_3()' )
	end
	
	if quest_warp >= 150 then
	
		dlg_menu( "@90605515", 'NPC_Quest_warp_4()' )
	end

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

function NPC_Quest_warp_1()

	warp( 116525 + math.random(0,50) , 89736 + math.random(0,50), gv("layer") ) 
	
end

function NPC_Quest_warp_2()

	warp( 136287 + math.random(0,50) , 106956 + math.random(0,50), gv("layer") ) 
	
end

function NPC_Quest_warp_3()

	warp( 122864 + math.random(0,50) , 121769 + math.random(0,50), gv("layer") ) 
	
end

function NPC_Quest_warp_4()

	warp( 96598 + math.random(0,50) , 101299 + math.random(0,50), gv("layer") ) 
	
end
   --============================================================
   --===============		경비대장 이셀라
   --============================================================

function NPC_Quest_slaugter2_init()
	set_npc_name(  "@90605400"  )
end

function NPC_Quest_slaugter2_contact()

	dlg_title( "@90605400" )
	dlg_text( "@90605401" )

	local state_code = get_local_info() 
	local current_time = get_os_date( "%Y-%m-%d %H:%M:%S" )	
	
	if state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then
		if '2018-04-17 08:00:00' <= current_time and current_time <= '2018-05-15 08:00:00' then	
			dlg_menu( "@90610623", "NPC_Event_2018_WZ_9_Anni()" ) -- Devildom Purge event
		end
	end	
	
	dlg_menu( "@90010002", " " ) -- 대화종료 
 	dlg_show()
 
end

   --============================================================
   --===============		몬스터 연구원 크루아
   --============================================================

function NPC_Quest_slaugter3_init()
	set_npc_name(  "@90605406"  )
end



function NPC_Quest_slaugter3_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605406" )
	dlg_text( "@90605407" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		몬스터 연구원 니크
   --============================================================

function NPC_Quest_slaugter4_init()
	set_npc_name(  "@90605408"  )
end



function NPC_Quest_slaugter4_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605408" )
	
	local quest_progress3618 =  get_quest_progress(3618) -- <(version:8.1)>공포의괴물
	
	if quest_progress3618 == 0 then
		dlg_text( "@90605411" )	
		-- 퀘스트 수행 전, 수행 중 대사
	else
		dlg_text( "@90605409" )
	end
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end



   --============================================================
   --===============		고고학자 블라브
   --============================================================

function NPC_Quest_minotaur1_init()
	set_npc_name(  "@90605415"  )
end


function NPC_Quest_minotaur1_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605415" )
	dlg_text( "@90605416" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		선발대장 미엘
   --============================================================

function NPC_Quest_bapho1_init()
	set_npc_name(  "@90605426"  )
end



function NPC_Quest_bapho1_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605426" )
	dlg_text( "@90605430" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		선발대원 라호무
   --============================================================

function NPC_Quest_bapho2_init()
	set_npc_name(  "@90605427"  )
end



function NPC_Quest_bapho2_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605427" )
	dlg_text( "@90605431" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		선발대원 히라
   --============================================================

function NPC_Quest_bapho3_init()
	set_npc_name(  "@90605428"  )
end



function NPC_Quest_bapho3_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605428" )
	dlg_text( "@90605432" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		선발대원 제리크로
   --============================================================

function NPC_Quest_bapho4_init()
	set_npc_name(  "@90605429"  )
end



function NPC_Quest_bapho4_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605429" )
	dlg_text( "@90605433" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		경계하는 마녀단원
   --============================================================

function NPC_Quest_bapho5_init()
	set_npc_name(  "@90605441"  )
end



function NPC_Quest_bapho5_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605441" )
	dlg_text( "@90605442" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		숲의 주인 마히베흐
   --============================================================

function NPC_Quest_undine1_init()
	set_npc_name(  "@90605451"  )
end



function NPC_Quest_undine1_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605451" )
	dlg_text( "@90605455" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		숲의 정령  로
   --============================================================

function NPC_Quest_undine2_init()
	set_npc_name(  "@90605452"  )
end



function NPC_Quest_undine2_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605452" )
	dlg_text( "@90605456" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		숲의 정령  히르
   --============================================================

function NPC_Quest_undine3_init()
	set_npc_name(  "@90605453"  )
end



function NPC_Quest_undine3_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605453" )
	dlg_text( "@90605456" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		숲의 정령  헤이르
   --============================================================

function NPC_Quest_undine4_init()
	set_npc_name(  "@90605454"  )
end



function NPC_Quest_undine4_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605454" )
	dlg_text( "@90605456" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		현장감독 헤더
   --============================================================

function NPC_Quest_pick1_init()
	set_npc_name(  "@90605460"  )
end



function NPC_Quest_pick1_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605460" )
	dlg_text( "@90605465" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

  --============================================================
   --===============		기계공학자 셸
   --============================================================

function NPC_Quest_pick2_init()
	set_npc_name(  "@90605461"  )
end



function NPC_Quest_pick2_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605461" )
	dlg_text( "@90605466" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

  --============================================================
   --===============		기계공학자 동수
   --============================================================

function NPC_Quest_pick3_init()
	set_npc_name(  "@90605462"  )
end



function NPC_Quest_pick3_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605462" )
	dlg_text( "@90605467" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

  --============================================================
   --===============		기계공학자 머피나
   --============================================================

function NPC_Quest_pick4_init()
	set_npc_name(  "@90605463"  )
end



function NPC_Quest_pick4_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605463" )
	dlg_text( "@90605468" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

--============================================================
   --===============		기계공학자 오더크
   --============================================================

function NPC_Quest_pick5_init()
	set_npc_name(  "@90605464"  )
end



function NPC_Quest_pick5_contact()

	-- 다이얼로그 출력
	dlg_title( "@90605464" )
	dlg_text( "@90605469" )

			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end


--================================================================
------------------------- 퀘스트 링크 스크립트--------------------
--================================================================


-- 경비대장에게 보고 : 수락 
-- 퀘템 인벤토리에 넣어주기
function quest_start_3615()

		
	--퀘템 카운트 구하기
	local item_slt3615_qitem_count = find_item ( 1000426 )
	
	if item_slt3615_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000426 ), item_slt3615_qitem_count )
		end
		
	else
		 -- 퀘템 넣어주기
		insert_item( 1000426, 1 )
		cprint( "@90605479" ) -- 퀘템 받아�?
		 
	end
	
	


end


-- 경비대장에게 보고 : 클리어
-- 퀘템 인벤토리에서 삭제
function quest_end_3615()

		
	--퀘템 카운트 구하기
	local item_slt3615_qitem_count = find_item ( 1000426 )
	
	if item_slt3615_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000426 ), item_slt3615_qitem_count )
		end
		
		cprint( "@90605480" ) -- 퀘템 뺏었긔
	end
	
	
end


------------------------------------------------------------
--퀘스트 포기 아이템 삭제
------------------------------------------------------------

-- 경비대장에게 보고 : 포기
-- 퀘템 인벤토리에서 삭제
function quest_drop_3615()

		
	--퀘템 카운트 구하기
	local item_slt3615_qitem_count = find_item ( 1000426 )
	
	if item_slt3615_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000426 ), item_slt3615_qitem_count )
		end
	end
	
	
end

-- 비밀기지인가? : 포기
-- 퀘템 인벤토리에서 삭제
function quest_drop_3616()

		
	--퀘템 카운트 구하기
	local item_slt3616_qitem_count = find_item ( 1000427 )
	
	if item_slt3616_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000427 ), item_slt3616_qitem_count )
		end
	end
	
	
end

-- 사막의 수다쟁이 : 포기
-- 퀘템 인벤토리에서 삭제
function quest_drop_3619()

		
	--퀘템 카운트 구하기
	local item_slt3619_qitem_count = find_item ( 1000428 )
	
	if item_slt3619_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000428 ), item_slt3619_qitem_count )
		end
	end
	
	
end

-- 무너진 미궁탐사 : 포기
-- 퀘템 인벤토리에서 삭제
function quest_drop_3621()

		
	--퀘템 카운트 구하기
	local item_slt3621_qitem_count = find_item ( 1000429 )
	
	if item_slt3621_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000429 ), item_slt3621_qitem_count )
		end
	end
	
	
end

-- 물품상자 회수 : 포기
-- 퀘템 인벤토리에서 삭제
function quest_drop_3623()

		
	--퀘템 카운트 구하기
	local item_slt3623_qitem_count = find_item ( 1000430 )
	
	if item_slt3623_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000430 ), item_slt3623_qitem_count )
		end
	end
	
	
end

-- 어둠의힘과 숲의 주인 : 포기
-- 퀘템 인벤토리에서 삭제
function quest_drop_3629()

		
	--퀘템 카운트 구하기
	local item_slt3629_qitem_count = find_item ( 1000431 )
	
	if item_slt3629_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000431 ), item_slt3629_qitem_count )
		end
	end
	
	
end

-- 수정회수 : 포기
-- 퀘템 인벤토리에서 삭제
function quest_drop_3630()

		
	--퀘템 카운트 구하기
	local item_slt3630_qitem_count = find_item ( 1000433 )
	
	if item_slt3630_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000433 ), item_slt3630_qitem_count )
		end
	end
	
	
end

-- 수정협곡의 조사 : 포기
-- 퀘템 인벤토리에서 삭제
function quest_drop_3631()

		
	--퀘템 카운트 구하기
	local item_slt3631_qitem_count = find_item ( 1000432 )
	
	if item_slt3631_qitem_count >= 1 then

		for i = 0, 10 do
			-- 퀘템 남은 아이템 없애기
			delete_item( get_item_handle( 1000432 ), item_slt3631_qitem_count )
		end
	end
	
	
end


-----------------------------------------------------------------------------
--일퀘로 AP지급  - 슬래터20//미노타우르스:30//바포메트:40//운디네:50//픽 :100ap 지급 
--1일 총 240ap 획득 가능 

-- 슬래터 일퀘완료시 관련 퀘스트 플래그 삭제

function quest_end_3618()

	
	--해당 연퀘 초기화

	local fbossap = get_value( 'ap' )
	local questapbonus_a = 100
	if gv( "premium" ) == 1 then
		questapbonus_a = 200
	end	
	local fbossap = fbossap + questapbonus_a
	set_value( "ap", fbossap )
	cprint(sconv("@90605481","#@questapbonus_a@#", tostring(questapbonus_a) ) )
	
	reset_finished_quest( 3613 )
	--reset_finished_quest( 3614 )
	reset_finished_quest( 3615 )
	--reset_finished_quest( 3616 )
	reset_finished_quest( 3617 )
	reset_finished_quest( 3618 )	
end


-- 미노타우르스 일퀘완료시 관련 퀘스트 플래그 삭제
function quest_end_3621()

	
	--해당 연퀘 초기화
-------------------------------
	local fbossap = get_value( 'ap' )
	local questapbonus_a = 100
	if gv( "premium" ) == 1 then
		questapbonus_a = 200
	end
	
	local fbossap = fbossap + questapbonus_a
	set_value( "ap", fbossap )
	cprint(sconv("@90605481","#@questapbonus_a@#", tostring(questapbonus_a) ) )
	
	reset_finished_quest( 3619 )
	reset_finished_quest( 3620 )
	reset_finished_quest( 3621 )

end

-- 바포메트 일퀘완료시 관련 퀘스트 플래그 삭제
function quest_end_3627()

	
	--해당 연퀘 초기화
-------------------------------
	local fbossap = get_value( 'ap' )
	local questapbonus_a = 100	
	if gv( "premium" ) == 1 then
		questapbonus_a = 200
	end	
	local fbossap = fbossap + questapbonus_a
	set_value( "ap", fbossap )
	cprint(sconv("@90605481","#@questapbonus_a@#", tostring(questapbonus_a) ) )
	
	reset_finished_quest( 3622 )
	--reset_finished_quest( 3623 )
	--reset_finished_quest( 3624 )
	--reset_finished_quest( 3625 )
	reset_finished_quest( 3626 )
	reset_finished_quest( 3627 )
	
end

-- 운다인 일퀘완료시 관련 퀘스트 플래그 삭제
function quest_end_3629()
	--해당 연퀘 초기화
-------------------------------
	local fbossap = get_value( 'ap' )
	local questapbonus_a = 100
	
	if gv( "premium" ) == 1 then
		questapbonus_a = 200
	end
	
	local fbossap = fbossap + questapbonus_a
	set_value( "ap", fbossap )
	cprint(sconv("@90605481","#@questapbonus_a@#", tostring(questapbonus_a) ) )
	
	reset_finished_quest( 3628 )
	reset_finished_quest( 3629 )
	
end


-- 픽 일퀘완료시 관련 퀘스트 플래그 삭제
function quest_end_3632()

	
	--해당 연퀘 초기화
-------------------------------
	local fbossap = get_value( 'ap' )
	local questapbonus_a = 100
	if gv( "premium" ) == 1 then
		questapbonus_a = 200
	end
	local fbossap = fbossap + questapbonus_a
	set_value( "ap", fbossap )
	cprint(sconv("@90605481","#@questapbonus_a@#", tostring(questapbonus_a) ) )
	
	--reset_finished_quest( 3630 )
	--reset_finished_quest( 3631 )
	reset_finished_quest( 3632 )
end

--4대인던 일일퀘스트 수락/완료 스크립트
function quest_start_3644()


end

function quest_end_3644()

	reset_finished_quest( 3640 )
	reset_finished_quest( 3644 )

end

function quest_start_3645()


end

function quest_end_3645()

	reset_finished_quest( 3641 )
	reset_finished_quest( 3645 )

end

function quest_start_3646()


end

function quest_end_3646()

	reset_finished_quest( 3642 )
	reset_finished_quest( 3646 )

end

function quest_start_3647()


end

function quest_end_3647()

	reset_finished_quest( 3643 )
	reset_finished_quest( 3647 )

end
--4대 인던 반복 퀘스트 수락/완료 스크립트
function quest_start_3648()


end

function quest_end_3648()

	reset_finished_quest( 3648 )

end


--8.3 신규 일일퀘스트 수락/완료 스크립트


function quest_end_3712()

	reset_finished_quest( 3712 )
	
end

function quest_end_3713()

	reset_finished_quest( 3713 )
	
end

function quest_end_3714()

	reset_finished_quest( 3714 )
	
end

function quest_end_3715()

	reset_finished_quest( 3715 )
	
end


function quest_end_3716()

	reset_finished_quest( 3716 )
	
end

function quest_end_3717()

	reset_finished_quest( 3717 )
	
end

function quest_end_3718()

	reset_finished_quest( 3718 )
	
end

function quest_end_3719()

	reset_finished_quest( 3719 )
	
end

function quest_end_3720()

	reset_finished_quest( 3720 )
	
end

function quest_end_3721()

	reset_finished_quest( 3721 )
	
end

function quest_end_3722()

	reset_finished_quest( 3722 )
	
end

function quest_end_3723()

	reset_finished_quest( 3723 )
	
end

function quest_end_3724()

	reset_finished_quest( 3724 )
	
end

function quest_end_3725()

	reset_finished_quest( 3725 )
	
end

function quest_end_3726()

	reset_finished_quest( 3726 )
	
end

function quest_end_3727()

	reset_finished_quest( 3727 )
	
end

function quest_end_3728()

	reset_finished_quest( 3728 )
	
end

function quest_end_3729()

	reset_finished_quest( 3729 )
	
end

function quest_end_3730()

	reset_finished_quest( 3730 )
	
end

function quest_end_3731()

	reset_finished_quest( 3731 )
	
end

function quest_end_3732()

	reset_finished_quest( 3732 )
	
end

function quest_end_3733()

	reset_finished_quest( 3733 )
	
end

function quest_end_3734()

	reset_finished_quest( 3734 )
	
end

function quest_end_3735()

	reset_finished_quest( 3735 )
	
end

function quest_end_3736()

	reset_finished_quest( 3736 )
	
end


--9.1 신규 일일퀘스트 수락/완료 스크립트


function quest_end_4000()

	reset_finished_quest( 4000 )
	
end

function quest_end_4001()

	reset_finished_quest( 4001 )
	
end

function quest_end_4002()

	reset_finished_quest( 4002 )
	
end

function quest_end_4003()

	reset_finished_quest( 4003 )
	
end

function quest_end_4004()

	reset_finished_quest( 4004 )
	
end

function quest_end_4005()

	reset_finished_quest( 4005 )
	
end

function quest_end_4006()

	reset_finished_quest( 4006 )
	
end

function quest_end_4007()

	reset_finished_quest( 4007 )
	
end

function quest_end_4008()

	reset_finished_quest( 4008 )
	
end


-- <ep9>선발대원 소유
function NPC_circus_soyu()

	-- 다이얼로그 출력
	dlg_title("@90606169")

	dlg_text( "@90606170" )
	
	dlg_menu( "@90010002", " " )

	dlg_show()
	
end

function NPC_circus_maximus()

	-- 다이얼로그 출력
	dlg_title("@90606171")

	dlg_text( "@90606172" )
	
	dlg_menu( "@90010002", " " )

	dlg_show()
	
end

function NPC_circus_tesker()

	-- 다이얼로그 출력
	dlg_title("@90606173")

	dlg_text( "@90606174" )
	
	dlg_menu( "@90010002", " " )

	dlg_show()
	
end

function NPC_circus_teo()

	-- 다이얼로그 출력
	dlg_title("@90606175")

	dlg_text( "@90606176" )
	
	dlg_menu( "@90010002", " " )

	dlg_show()
	
end

function NPC_circus_glasse()

	-- 다이얼로그 출력
	dlg_title("@90606177")

	dlg_text( "@90606178" )
	
	dlg_menu( "@90010002", " " )

	dlg_show()
	
end

  --============================================================
   --===============		호라이즌 삐에로
   --============================================================

function NPC_pierrot_Horizon_init()
	cprint( "!호라이즌 삐에로 가동" )
	set_npc_name(  "@90610036"  )
end

function NPC_pierrot_Horizon_contact()
	local state_code = get_local_info()
	local level = get_value( "level" )
	-- 다이얼로그 출력
	dlg_title( "@90610037" )
	dlg_text( "@90610038" )
 			
	if state_code == 256 and level >= 80 then
		dlg_menu( "@90610039","Exchange_lucky_gift_1()" )
	else
		dlg_menu( "@90610039","Exchange_lucky_gift_1()" )
	end
	
	if state_code == 16 then
		dlg_menu( "@90010062", "open_market( 'Locked_Treasure_Chest' )" )
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

function Exchange_lucky_gift_1()

	-- 다이얼로그 출력
	dlg_title( "@90610037" )
	dlg_text( "@90610040" )
 			
	dlg_menu( "@90610041","Exchange_lucky_gift_2(1)" )
	dlg_menu( "@90610042","Exchange_lucky_gift_2(2)" )
	dlg_menu( "@90610043","Exchange_lucky_gift_2(3)" )
	dlg_menu( "@90610044","Exchange_lucky_gift_2(4)" )
	dlg_menu( "@90610045","Exchange_lucky_gift_2(5)" )
	dlg_menu( "@90610046","Exchange_lucky_gift_2(6)" )
	dlg_menu( "@90610047","Exchange_lucky_gift_2(7)" )
	dlg_menu( "@90610048","Exchange_lucky_gift_2(8)" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

function Exchange_lucky_gift_2(gift)

	-- 다이얼로그 출력
	dlg_title( "@90610037" )
	
	if gift == 1 then
		dlg_text( "@90610049" )
		local item_count_3 = find_item ( 1000460 )
		if item_count_3 >= 1 then
			dlg_menu( "@90610057","Exchange_lucky_gift_process(1)" )
		end		
	elseif gift == 2 then
		dlg_text( "@90610050" )		
		local item_count_5 = find_item ( 1000462 )
		if item_count_5 >= 1 then
			dlg_menu( "@90610058","Exchange_lucky_gift_process(2)" )
		end
	elseif gift == 3 then
		dlg_text( "@90610051" )
		local item_count_11 = find_item ( 1000468 )
		if item_count_11 >= 1 then
			dlg_menu( "@90610059","Exchange_lucky_gift_process(3)" )
		end		
	elseif gift == 4 then
		dlg_text( "@90610052" )		
		local item_count_8 = find_item ( 1000465 )
		local item_count_15 = find_item ( 1000472 )
		if item_count_8 >= 1 and item_count_15 >= 1 then
			dlg_menu( "@90610060","Exchange_lucky_gift_process(4)" )
		end
	elseif gift == 5 then
		dlg_text( "@90610053" )		
		local item_count_2 = find_item ( 1000459 )
		local item_count_6 = find_item ( 1000463 )
		local item_count_13 = find_item ( 1000470 )
		if item_count_2 >= 1 and item_count_6 >= 1 and item_count_13 >= 1 then
			dlg_menu( "@90610061","Exchange_lucky_gift_process(5)" )
		end
	elseif gift == 6 then
		dlg_text( "@90610054" )		
		local item_count_9 = find_item ( 1000466 )
		local item_count_12 = find_item ( 1000469 )
		local item_count_18 = find_item ( 1000475 )
		if item_count_9 >= 1 and item_count_12 >= 1 and item_count_18 >= 1 then
			dlg_menu( "@90610062","Exchange_lucky_gift_process(6)" )
		end
	elseif gift == 7 then
		dlg_text( "@90610055" )
		local item_count_4 = find_item ( 1000461 )
		local item_count_16 = find_item ( 1000473 )
		local item_count_19 = find_item ( 1000476 )
		if item_count_4 >= 1 and item_count_16 >= 1 and item_count_19 >= 1 then
			dlg_menu( "@90610063","Exchange_lucky_gift_process(7)" )
		end		
	elseif gift == 8 then
		dlg_text( "@90610056" )		
		local item_count_1 = find_item ( 1000458 )
		local item_count_7 = find_item ( 1000464 )
		local item_count_10 = find_item ( 1000467 )
		local item_count_14 = find_item ( 1000471 )
		local item_count_17 = find_item ( 1000474 )
		local item_count_20 = find_item ( 1000477 )
		if item_count_1 >= 1 and item_count_7 >= 1 and item_count_10 >= 1 and item_count_14 >= 1 and item_count_17 >= 1 and item_count_20 >= 1 then
			dlg_menu( "@90610064","Exchange_lucky_gift_process(8)" )
		end
	end
 			
	
	-- 돌아가기 
	dlg_menu( "@90010003", "Exchange_lucky_gift_1()" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

function Exchange_lucky_gift_process(gift)	
	
	if gift == 1 then
		--조건 확인해서 아이템 다 들고 있으면 교환해주고 Exchange_lucky_gift_success()로 보내주기. 선택메뉴 다시
		local item_count_3 = find_item ( 1000460 )
		if item_count_3 >= 1 then
			insert_item( 2013203, 1 )
			delete_item( get_item_handle( 1000460 ), 1 )
			Exchange_lucky_gift_success()
			return
		end		
	elseif gift == 2 then
		local item_count_5 = find_item ( 1000462 )
		if item_count_5 >= 1 then
			insert_item( 2013202, 1 )
			delete_item( get_item_handle( 1000462 ), 1 )
			Exchange_lucky_gift_success()
			return
		end	
	elseif gift == 3 then
		local item_count_11 = find_item ( 1000468 )
		if item_count_11 >= 1 then
			insert_item( 2013201, 1 )
			delete_item( get_item_handle( 1000468 ), 1 )
			Exchange_lucky_gift_success()
			return
		end
	elseif gift == 4 then
		local item_count_8 = find_item ( 1000465 )
		local item_count_15 = find_item ( 1000472 )
		if item_count_8 >= 1 and item_count_15 >= 1 then
			insert_item( 2013204, 3 )
			delete_item( get_item_handle( 1000465 ), 1 )
			delete_item( get_item_handle( 1000472 ), 1 )
			Exchange_lucky_gift_success()
			return
		end
	elseif gift == 5 then
		local item_count_2 = find_item ( 1000459 )
		local item_count_6 = find_item ( 1000463 )
		local item_count_13 = find_item ( 1000470 )
		if item_count_2 >= 1 and item_count_6 >= 1 and item_count_13 >= 1 then
			insert_item( 1001017, 1 )
			delete_item( get_item_handle( 1000459 ), 1 )
			delete_item( get_item_handle( 1000463 ), 1 )
			delete_item( get_item_handle( 1000470 ), 1 )
			Exchange_lucky_gift_success()
			return
		end
	elseif gift == 6 then
		local item_count_9 = find_item ( 1000466 )
		local item_count_12 = find_item ( 1000469 )
		local item_count_18 = find_item ( 1000475 )
		if item_count_9 >= 1 and item_count_12 >= 1 and item_count_18 >= 1 then
			insert_item( 1000482, 2 )
			delete_item( get_item_handle( 1000466 ), 1 )
			delete_item( get_item_handle( 1000469 ), 1 )
			delete_item( get_item_handle( 1000475 ), 1 )
			Exchange_lucky_gift_success()
			return
		end
	elseif gift == 7 then
		local item_count_4 = find_item ( 1000461 )
		local item_count_16 = find_item ( 1000473 )
		local item_count_19 = find_item ( 1000476 )
		if item_count_4 >= 1 and item_count_16 >= 1 and item_count_19 >= 1 then
			insert_item( 1000483, 2 )
			delete_item( get_item_handle( 1000461 ), 1 )
			delete_item( get_item_handle( 1000473 ), 1 )
			delete_item( get_item_handle( 1000476 ), 1 )
			Exchange_lucky_gift_success()
			return
		end
	elseif gift == 8 then
		local item_count_1 = find_item ( 1000458 )
		local item_count_7 = find_item ( 1000464 )
		local item_count_10 = find_item ( 1000467 )
		local item_count_14 = find_item ( 1000471 )
		local item_count_17 = find_item ( 1000474 )
		local item_count_20 = find_item ( 1000477 )
		if item_count_1 >= 1 and item_count_7 >= 1 and item_count_10 >= 1 and item_count_14 >= 1 and item_count_17 >= 1 and item_count_20 >= 1 then
			insert_item( 1000457, 1 )
			delete_item( get_item_handle( 1000458 ), 1 )
			delete_item( get_item_handle( 1000464 ), 1 )
			delete_item( get_item_handle( 1000467 ), 1 )
			delete_item( get_item_handle( 1000471 ), 1 )
			delete_item( get_item_handle( 1000474 ), 1 )
			delete_item( get_item_handle( 1000477 ), 1 )
			Exchange_lucky_gift_success()
			return
		end
	end
	
	-- 다이얼로그 출력
	dlg_title( "@90610037" )
	-- 조건 맞는거 없으면 종이 들고 오라고 하고 닫기 메뉴 출력
	dlg_text( "@90610066" )	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show() 
end

function Exchange_lucky_gift_success()

	-- 다이얼로그 출력
	dlg_title( "@90610037" )
	dlg_text( "@90610065" )
 			
	dlg_menu( "@90610041","Exchange_lucky_gift_2(1)" )
	dlg_menu( "@90610042","Exchange_lucky_gift_2(2)" )
	dlg_menu( "@90610043","Exchange_lucky_gift_2(3)" )
	dlg_menu( "@90610044","Exchange_lucky_gift_2(4)" )
	dlg_menu( "@90610045","Exchange_lucky_gift_2(5)" )
	dlg_menu( "@90610046","Exchange_lucky_gift_2(6)" )
	dlg_menu( "@90610047","Exchange_lucky_gift_2(7)" )
	dlg_menu( "@90610048","Exchange_lucky_gift_2(8)" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

  --============================================================
   --===============		유적도시 헥토르
   --============================================================
   
function NPC_Hector_RuinCity_init()
	cprint( "!무사 헥토르 가동" )
	set_npc_name(  "@90610120"  )
end

function NPC_Hector_RuinCity_contact()

	-- 다이얼로그 출력
	dlg_title( "@90610121" )
	dlg_text( "@90610122" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

  --============================================================
   --===============		유적도시 마녀의 아이 샤비
   --============================================================
   
function NPC_Xabi_RuinCity_init()
	cprint( "!마녀의 아이 샤비 가동" )
	set_npc_name(  "@90610123"  )
end

function NPC_Xabi_RuinCity_contact()

	-- 다이얼로그 출력
	dlg_title( "@90610124" )
	dlg_text( "@90610125" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

 --============================================================
   --===============		사이라그 폐허 수호군 NPC
   --============================================================
   
function NPC_Guard_Sirag_init()
	cprint( "!수호군 NPC 가동" )
	set_npc_name(  "@90610126"  )
end

function NPC_Guard_Sirag_contact()

	-- 다이얼로그 출력
	dlg_title( "@90610127" )
	dlg_text( "@90610128" )
 			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

-- 공용 퀘스트 종료후 리셋 테스트
function common_quest_end(questID)

	reset_finished_quest( questID )
	
end

--============================================================
--===============		9.4 NPC
--============================================================

-- 승선 안내원 얀
function NPC_Guide_aboard_yarn_contact()

	-- 다이얼로그 출력
	--[[ dlg_title( "@90610255" )
	
	if get_minute() >= 0 and get_minute() <= 10 then
		dlg_text("@1174") -- 탑승시 곧 배가 출항합니다.
	else
		dlg_text( sconv( "@90610256" , "#@min@#", tostring(60-get_minute()) )) -- 출항 ~분 남았습니다.
	end
	
	dlg_menu( "@90610257", "shepard_boarding()" ) -- 탑승
	dlg_menu( "@90010002", " " ) -- 대화종료
 	dlg_show() ]]
	
	dlg_title( "@90610255" )
	
	local left_min = get_minute()
	
	if left_min > 30 then							
		left_min = left_min - 30						
	end
	
	if  left_min <= 5 then							
		dlg_text("@1174")				-- 탑승시 곧 배가 출항합니다.		
	else							
		dlg_text( sconv( "@90610256" , "#@min@#", tostring(30-left_min) ))					-- 출항 ~분 남았습니다.	
	end
	
	dlg_menu( "@90610257", "shepard_boarding()" ) -- 탑승							
	dlg_menu( "@90010002", " " ) -- 대화종료							
	dlg_show()												
 
end

function shepard_boarding()

	if find_item( 2016091 ) >= 1 then
		--private_notice( "@90610259" )
		message( "@90610259" )
		warp (155347, 149512)		
	else
		-- 다이얼로그 출력
		dlg_title( "@90610255" ) -- 승선 안내원 얀
		dlg_text( "@90610258" ) -- 탑승권 필요		
		
		dlg_menu( "@90010002", " " ) -- 대화종료
		dlg_show()
	end
end

-- 유적 탐사대 샐리, 밀리
function NPC_Expedition_Sally_Milli_contact()
	
	--[[ if get_value( "x" ) >= 155211  and get_value( "x" ) <= 155483 and get_value( "y" ) >= 149451 and get_value( "y" ) <= 149574 then ]]		
	
		local npc_id = get_npc_id()
		-- 다이얼로그 출력
		if npc_id == 15040 then
			dlg_title( "@90610260" ) -- 샐리
			dlg_text( "@90610262" )
			dlg_menu( "@90610263", "Luciad_protection_buff()" ) -- 샐리 : 루시아드 보호
		elseif npc_id == 15042 then
			dlg_title( "@90610261" ) -- 밀리
			dlg_text( "@90610262" )
			dlg_menu( "@90610264", "Luciad_blessing_buff()" ) -- 밀리 : 루시아드 가호
		end
		
		dlg_menu( "@90010002", " " ) -- 대화종료
		dlg_show()
		
	--[[ end ]]
	
end

function Luciad_protection_buff()
	
	dlg_title( "@90610260" ) -- 유적 탐사대 샐리
	
	if find_item( 2016092 ) >= 1 then
		dlg_text( "@90610275" )
		add_state(1067, 1,330000) -- 55분 퀘스트 플래그 체크
		delete_item( get_item_handle( 2016092 ), 1 )		
	else		
		dlg_text( "@90610274" ) -- 신들의 흔적 필요
	end
	
	dlg_menu( "@90010002", " " ) -- 대화종료
	dlg_show()
	
end

function Luciad_blessing_buff()

	dlg_title( "@90610261" ) -- 유적 탐사대 밀리
	
	if find_item( 2016092 ) >= 1 then
		dlg_text( "@90610275" )
		add_state(1068, 1,330000) -- 55분 퀘스트 플래그 체크
		delete_item( get_item_handle( 2016092 ), 1 )		
	else
		dlg_text( "@90610274" ) -- 신들의 흔적 필요
	end
	
	dlg_menu( "@90010002", " " ) -- 대화종료
	dlg_show()
	
end

-- 선장 매클리 마을
function NPC_shepard_captain_Town_contact()	
	dlg_title( "@90610265" ) 
	
	if get_minute() >= 0 and get_minute() <= 10 then
		dlg_text("@1175") -- 곧 배가 출항합니다.
	else
		dlg_text( sconv( "@90610266" , "#@min@#", tostring(60-get_minute()) )) -- 출항 ~분 남았습니다.
	end
	
	dlg_menu( "@90010002", " " ) -- 대화종료
 	dlg_show()	
end

-- 선장 매클리 섬
function NPC_shepard_captain_Island_contact()
	dlg_title( "@90610265" ) 
	dlg_text( "@90610267") -- 많은 성과...
	
	dlg_menu( "@90010002", " " ) -- 대화종료
 	dlg_show()
end

-- 하급 선원
function NPC_Junior_crew_contact()
	dlg_title( "@90610268" ) 
	dlg_text( "@90610269")
	
	dlg_menu( "@90010002", " " ) -- 대화종료
 	dlg_show()
end

-- 상급 선원
function NPC_Senior_crew_contact()
	dlg_title( "@90610270" ) 
	dlg_text( "@90610271")
	
	dlg_menu( "@90010002", " " ) -- 대화종료
 	dlg_show()
end

-- 등대지기 노팅겔
function NPC_lighthouse_keeper_contact()
	dlg_title( "@90610272" ) 
	dlg_text( "@90610273")
	
	dlg_menu( "@90010002", " " ) -- 대화종료
 	dlg_show()
end

-- 루시아드 학회 파견관 루스
function NPC_dispatched_Ruth_contact()
	local state_code = get_local_info()
	
	dlg_title( "@90610280" ) 
	dlg_text( "@90610281")
	
	-- Edit by ham999dy -- 9:01 PM Monday, February 4, 2019
	
	-- if state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then
		dlg_menu( "@90610488", "NPC_Event_2017_dispatched_Ruth_trade()" ) -- Trade 20 Sign of Forgotten Gods
	-- end
	
	dlg_menu( "@90010002", " " ) -- 대화종료
 	dlg_show()
end

function dlg_menu_exchange ( item , dlg )
	local cnt1 = find_item( item )   -- 축복받은 변화의힘 (거래불가)
	if cnt1 > 0 then
		--dlg_menu( "<#AA99AA>|@" .. (10000000 + item) .. "| ( 보유:" .. cnt1 .. " )", dlg )
		dlg_menu( "@90610870\v#@item1@#\v@"..(10000000+item).."\v#@number1@#\v"..cnt1, dlg )
	end
end

------------------------------------------------------
function NPC_dailyquest_9_2_exchang() -- 9.4 주화 교환원
-----------------------------------------------------
	dlg_title( "@690000128" )
	dlg_text( "@90610872" ) -- 무엇을 원하시나요?	
	
	dlg_menu( "@90610871", "Coin_Exchange()" ) -- 주화로 아이템 교환
	dlg_menu( "@90610309", "NPC_9_4_OpenBox()" ) -- 스킬 카드 교환
	
	local state_code = get_local_info()
	--if state_code == 1 then
	dlg_menu( "@90610840", "NPC_9_4_Cube(0,0)" ) -- 7랭크 큐브 제작/교환
	--end
	if state_code == 1 or state_code == 256 or state_code == 8192 then
		
		-- Edit by ham999dy - 3:13 AM Monday, February 4, 2019
		
		-- dlg_menu_exchange( 705014, "NPC_9_5_Alteration(0,0)")
		-- dlg_menu_exchange( 705009, "NPC_9_5_Alteration(1,0)")
	end
	dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
	dlg_menu( "@90010002", " " )	--대화 종료
	dlg_show()
end


function Coin_Exchange()
	dlg_title( "@690000128" )
	dlg_text( "@90610082\v#@number@#\v"..find_item(1000535)) -- 주화를 다양한 아이템으로 교환해 드립니다.
	
	dlg_menu( "@90610284", "NPC_9_4_Coin_exchange_item(2016098,50)" ) -- 꾸미기 강화 스크롤 : 레어
	dlg_menu( "@90610244", "NPC_9_4_Coin_exchange_item(3800270,30)" ) -- 베이직: 로마검투사 꾸미기 랜덤상자
	dlg_menu( "@90610245", "NPC_9_4_Coin_exchange_item(3800273,30)" ) -- 베이직: 다크메이지 꾸미기 랜덤상자
	dlg_menu( "@90610246", "NPC_9_4_Coin_exchange_item(3800276,30)" ) -- 베이직: 인디언 꾸미기 랜덤상자
	dlg_menu( "@90610247", "NPC_9_4_Coin_exchange_item(1000484,20)" ) -- 숫자 상자
	dlg_menu( "@90610248", "NPC_9_4_Coin_exchange_item(2016090,10)" ) -- 모두의 축복 상자
	dlg_menu( "@90610249", "NPC_9_4_Coin_exchange_item(2013705,10)" ) -- 이상한 카드 상자
	dlg_menu( "@90610250", "NPC_9_4_Coin_exchange_item(2013011,5)" ) -- 공격의 루나칩 1시간
	
	dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
	dlg_menu( "@90010002", " " )	--대화 종료
	dlg_show()
end

-- 700112	107700112	7랭크 큐브-스트라이크
-- 700212	107700212	7랭크 큐브-디펜스
-- 700657	107700657	7랭크 축복받은 카벙클의 강화 주사위 - 스트라이크
-- 700757	107700757	7랭크 축복받은 카벙클의 강화 주사위 - 디펜스

-- 701001	10701001	큐브 스트라이크 조각
-- 702001	10702001	큐브 디펜스 조각
-- 701003   10701003	축복받은 카벙클의 강화 주사위 스트라이크 조각
-- 702003	107702001	축복받은 카벙클의 강화 주사위 디펜스 조각

function NPC_9_5_Alteration(type,sel)

        local psrc = { 705014, 705009 }
        local pitem =  { 705031,705032,705033,705034,705035, 705036,705037,705038,705039,705040, 705041,705042,705043,705044,705045,
                         705016,705017,705018,705019,705020, 705021,705022,705023,705024,705025, 705026,705027,705028,705029,705030 }
        local item_src = psrc[type+1]
        dlg_title( "@690000128" )

        if (sel==0) then
                dlg_text( "@90610865" ) -- 어떤 아이템으로 교환하겠습니까?
                for i = 1, 15 do
                        dlg_menu( "@" .. 10000000 + pitem[type*15+i] , "NPC_9_5_Alteration(".. type .. "," .. i ..")" ) --돌아가기
                end
        elseif sel >=1 and sel <=15 then
                dlg_text( "@90605296" ) -- 교환이 성공적으로 진행되었습니다.
                local item_idx = pitem[type*15+sel]
                delete_item(  get_item_handle(item_src),1 )
                insert_item(item_idx,1)
                message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@"..tostring(get_item_name_id( item_idx ))) )
        end
        dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
        dlg_menu( "@90010002", " " )        --대화 종료
        dlg_show()
end

function NPC_9_4_Cube(sel,cnt)

	dlg_title( "@690000128" )

	local pcube = {700112,700212,700657,700757}
	local ppart = {701001,702001,701003,702003}
	local piron = {28,14,112,56}
		
	if sel == 0 then
		dlg_text( "@90610841" ) -- 어떤 아이템을 만드시겠습니까?
		dlg_menu( "@10" .. pcube[1],"NPC_9_4_Cube(1,0)" )
		dlg_menu( "@10" .. pcube[2],"NPC_9_4_Cube(2,0)" )
		dlg_menu( "@10" .. pcube[3],"NPC_9_4_Cube(3,0)" )
		dlg_menu( "@10" .. pcube[4],"NPC_9_4_Cube(4,0)" )
		dlg_menu( "@90610842","NPC_9_4_Cube(5,0)" ) -- 스트라이크 큐브 10개를 축복받은 카벙클 큐브로
		dlg_menu( "@90610843","NPC_9_4_Cube(6,0)" ) -- 디펜스큐브 10개를 축복받은 카벙클 큐브로
		dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
		dlg_menu( "@90010002", " " ) -- 대화종료
		dlg_show()	
		return
	elseif sel == 5 or sel == 6 then
		local ncube = pcube[sel-4]  -- 1,2
		local nout =  pcube[sel-2]  -- 3,4
		local cnt_cube = find_item( ncube)
		if cnt == 0  then
			cnt = math.floor(cnt_cube / 10)
			dlg_text("@90610867\v#@item1@#\v@".. 10000000+ncube.."\v#@number1@#\v"..cnt_cube.."\v#@item2@#\v@".. 10000000+nout.."\v#@number2@#\v"..cnt)
			if cnt >= 100 then
				dlg_menu("@90610294\v#@number@#\v100", "NPC_9_4_Cube("..sel..",100)" )
				dlg_menu("@90610294\v#@number@#\v10", "NPC_9_4_Cube("..sel..",10)" )
			elseif cnt >10 then
				dlg_menu( "@90610294\v#@number@#\v"..cnt,"NPC_9_4_Cube("..sel..",".. cnt ..")" )
				dlg_menu("@90610294\v#@number@#\v10", "NPC_9_4_Cube("..sel..",10)" )
			elseif cnt >0 then
				dlg_menu( "@90610294\v#@number@#\v"..cnt,"NPC_9_4_Cube("..sel..",".. cnt ..")" )
			end
			dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
			dlg_menu( "@90010002", " " ) -- 대화종료
			dlg_show()	
		else
			if cnt_cube < cnt*10 then
				dlg_text( "@90610297" ) -- 재료가 부족합니다
				dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
				dlg_menu( "@90010002", " " ) -- 대화종료
				dlg_show()	
				return
			end
			
			delete_item( get_item_handle( ncube ), cnt * 10 )
			insert_item(  nout, cnt)
			dlg_text( "@90610866" ) -- 교환하였습니다.
			dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
			dlg_menu( "@90010002", " " ) -- 대화종료
			dlg_show()	
		end
		return
	end
	
	local niron  =  find_item( 1100103 ) 
	local ciron  =  math.floor(niron/piron[sel])
	local npart  =  find_item(ppart[sel])	-- 큐브 스트라이크 조각  7+14
	local cpart  =  math.floor(npart/14)	
	
	if  cnt == 0 then
		cnt = cpart
		if cpart > ciron then
			cnt = ciron
		end
		cnt = math.floor(cnt)
		--dlg_text( "연마된 강철: "..niron.."개 보유 (/"..piron[sel].."),<br> 큐브 조각: " .. npart .. "개 보유 (/14),<br>|@10" ..  pcube[sel] .. "|최대 " .. cnt .. "개 제작 가능합니다." ) 
		dlg_text("@90610868\v#@number1@#\v"..niron.."\v#@piron@#\v"..piron[sel].."\v#@number2@#\v"..npart.."\v#@item1@#\v@".. 10000000+pcube[sel].."\v#@number3@#\v"..cnt )
		if cnt >= 100 then
			dlg_menu( "@90610844\v#@number@#\v100", "NPC_9_4_Cube("..sel..",100)" )
			dlg_menu( "@90610844\v#@number@#\v10", "NPC_9_4_Cube("..sel..",10)" )
		elseif cnt > 10 then
			dlg_menu( "@90610844\v#@number@#\v"..cnt,"NPC_9_4_Cube("..sel..",".. cnt ..")" )
			dlg_menu( "@90610844\v#@number@#\v10", "NPC_9_4_Cube("..sel..",10)" )
		elseif cnt > 0 then 
			dlg_menu( "@90610844\v#@number@#\v"..cnt,"NPC_9_4_Cube("..sel..",".. cnt ..")" )
		end
		dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
		dlg_menu( "@90010002", " " ) -- 대화종료
		dlg_show()	
	elseif cnt > 0 and cnt <= 100 then
		if ciron < cnt or cpart < cnt then
			dlg_text( "@90610297" ) -- 재료가 부족합니다
			dlg_menu( "@90010002", " " ) -- 대화종료
			dlg_show()	
			return
		end
		delete_item( get_item_handle( ppart[sel] ), cnt*14 )
		delete_item( get_item_handle( 1100103 ), cnt*piron[sel] )
		insert_item(  pcube[sel], cnt)
		--dlg_text( "@10" ..  pcube[sel] .. "|" .. cnt .. "|개 제작 하였습니다." )
		dlg_text( "@90610869\v#@item1@#\v@".. 10000000+pcube[sel].."\v#@number1@#\v" .. cnt )
		dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
		dlg_menu( "@90010002", " " ) -- 대화종료
		dlg_show()	
	end
end

function NPC_9_4_OpenBox()
	dlg_title( "@690000128" )
	dlg_text( "@90610319" ) -- 어떤 스킬카드 상자를...
	
	dlg_menu( "@11100405", "NPC_9_4_OpenBox_go(1)" ) -- 기본 직업상자
	dlg_menu( "@11100406", "NPC_9_4_OpenBox_go(2)" ) -- 기본 1차 직업상자
	dlg_menu( "@11100407", "NPC_9_4_OpenBox_go(3)" ) -- 기본 2차 직업상자
	dlg_menu( "@11100408", "NPC_9_4_OpenBox_go(4)" ) -- 기본 마스터 상자
	
	dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" ) --돌아가기
	dlg_menu( "@90010002", " " ) -- 대화종료
	dlg_show()	
end

function NPC_9_4_OpenBox_go(num)
	
	dlg_title( "@690000128" )
	
	local item_id = {1100405, 1100406, 1100407, 1100408}
	local drop_group = {-3000406, -3000407, -3000408, -3000409}
	local count  = find_item( item_id[num] ) 
	
	if count == 0  then
		dlg_text( "@90610321" ) -- 상자 부족
	elseif count > 1 then
		dlg_text( "@90610320" ) -- 확인해라
		-- Edit by ham999dy - 9:48 AM Tuesday, January 15, 2019
		if count > 1000 then 
			count = 1000
		end
		
		delete_item (get_item_handle(item_id[num]), count)
	
		for i=1,count do
			local item = pick_item_in_drop_group( drop_group[num] )
			insert_item( item[1], item[2] )
		end
	end
	
	dlg_menu( "@90010003", "NPC_9_4_OpenBox()" ) --돌아가기
	dlg_menu( "@90010002", " " ) -- 대화종료
	dlg_show()
end

--주화 개수 별 아이템 교환

function NPC_9_4_Coin_exchange_item( id, count ) -- 교환메뉴

	dlg_title( "@690000128" )
	
	if id == 1 then -- 데바 to 마드리갈 주화
		local count = math.floor( find_item( 1000485 ) / 3 )
		local del_count = math.floor( find_item( 1000485 ) / 3 ) * 3
	
		if find_item( 1000485 ) >= 3 then -- 일퀘 데바의 주화
			dlg_text( "@90610254" )	-- 대사
			insert_item ( 1000535, count ) -- 마드리갈 주화 넣어줌
			delete_item( get_item_handle( 1000485 ), del_count )	-- 주화 삭제
		else 
			dlg_text( "@90610078" ) -- 주화가 없는 경우
		end	
	else	
		if find_item( 1000535 ) >= count then
			dlg_text( "@90606002" )	-- 대사
			insert_item ( id, 1 )
			delete_item( get_item_handle( 1000535 ), count )	-- 주화 삭제
		else 
			dlg_text( "@90610078" ) -- 주화가 없는 경우
		end
	end
	
	dlg_menu( "@90010003", "NPC_dailyquest_9_2_exchang()" )	--처음으로
	dlg_menu( "@90010002", " " )	--대화 종료
	dlg_show()
end

function siege_start( )

end

function siege_process(step)

end






