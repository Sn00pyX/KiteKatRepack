--- Lua 스크립트 암호화
function get_module_name()
             return "NPC_Tutorial"
end

   --============================================================
   --             <<<<<< 수련자의 섬 NPC >>>>>>
   --============================================================

   --============================================================
   --===============		안내인 유시스
   --============================================================
	 function NPC_Tutorial_Guide_Deva_init()
	cprint( "!안내인 유시스 가동" )
	set_npc_name(  "@90301100"  )
end

function NPC_Tutorial_Guide_Deva_contact()


	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	-- 종족을 받아온다. 가이아 3, 데바 4, 아수라 5
	local race = get_value( "race" )
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress1 =  get_quest_progress(1005)
	local quest_progress2 =  get_quest_progress(1006)
	local quest_progress3 =  get_quest_progress(1007)

	-- 안내인의 종족이 아니면
	if race ~= 4 then
		dlg_text( "@90301102" )

	-- 첫번째 퀘스트 시작전이면(수락가능이면)
	elseif quest_progress1 == 0 then
		dlg_text( "@90301103" )
		
		
	-- 세번째 퀘스트 완료후라면
	elseif quest_progress3 == 255 then
		dlg_text( "@90301105" )

	-- 그 외의 상황(1~2 중의 한 퀘스트를 마치거나  1~3중의 하나를 수행중이거나 기타 등등...)
	else
		dlg_text( "@90301104" )
	
	end
	
	-- 레냐 페이스의 뜻은?
		dlg_menu( "@90301124", "Quest_Link_1_1d()" )		
	-- 수련자의 섬 채널 이동 안내?
		dlg_menu( "@91000463", "Quest_Link_1_1e()" )
		
	--메뉴 구성 (퀘스트가 있을 경우 자동으로 제일 위부터 퀘스트 메뉴 생성)
	-- 첫 번째 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress1 == 1 or quest_progress1 == 2 or quest_progress1 == 255 then
		-- HP란 무엇인가?
		dlg_menu( "@90010034", "Quest_Link_1_1a()" )
		-- 아이템의 장착법
		dlg_menu( "@90010035", "Quest_Link_1_1b()" )
		-- 오토 트랩, 에이전트 몬스터란?
		dlg_menu( "@90301126", "Quest_Link_1_1c()" )
		
	end
	-- 첫 번째 퀘스트 클리어.
	if quest_progress1 == 255 then
		-- 아이템의 획득 방법은?
		dlg_menu( "@90010036", "Quest_Link_1_2as()" )
	end
	-- 두 번째 퀘스트 클리어.
	if quest_progress2 == 255 then
		-- 포션을 먹는 방법은?
		dlg_menu( "@90010037", "Quest_Link_1_2bs()" )
	end
	-- 세 번째 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress3 == 1 or quest_progress3 == 2 or quest_progress3 == 255 then
		-- 미니맵을 보는 방법
		dlg_menu( "@90010038", "Quest_Link_1_3a()" )
	end
	
		-- 대화종료 
		dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end
 

-- 퀘스트 1-1 수락 부연설명
function Quest_Link_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text_without_quest_menu( "@90301114" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_1_1a()" )
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_1_1b()" )
	-- 오토 트랩, 에이전트 몬스터란?
	dlg_menu( "@90301126", "Quest_Link_1_1c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


-- 부가정보 1-1a 	HP,LV의 개념
function Quest_Link_1_1a()

	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text_without_quest_menu( "@90301106" )
	
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_1_1b()" )
	-- 오토 트랩, 에이전트 몬스터란?
	dlg_menu( "@90301126", "Quest_Link_1_1c()" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

 
-- 부가정보 1-1b 	아이템 장착법
function Quest_Link_1_1b()

	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text_without_quest_menu( "@90301107" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_1_1a()" )
	-- 오토 트랩, 에이전트 몬스터란?
	dlg_menu( "@90301126", "Quest_Link_1_1c()" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 1-1c 	오토 트랩, 에이전트 몬스터
function Quest_Link_1_1c()

	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text_without_quest_menu( "@90301127" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_1_1a()" )
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_1_1b()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 1-1d 	수련자의 섬 - 레냐 페이스
function Quest_Link_1_1d()

	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text_without_quest_menu( "@90301125" )
	
	-- 돌아가기
	--dlg_menu( "@90010003", "NPC_Tutorial_Guide_Deva_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 1-1e 	수련자의 섬 채널 이동 안내
function Quest_Link_1_1e()

	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text_without_quest_menu( "@91000464" )
	
	-- 돌아가기
	--dlg_menu( "@90010003", "NPC_Tutorial_Guide_Deva_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 버프 걸고 대사로 넘김. (공속 3레벨 10분, ID 1011)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_1_2a()
	add_state( 1011, 3, 60000 )
	Quest_Link_1_2as()
end
-- 부가정보 1-2a 아이템 획득
function Quest_Link_1_2as()

	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text( "@90301108" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end


-- 버프 걸고 대사로 넘김. (이속 3레벨 20분, ID 1013)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_1_2b()
	add_state( 1013, 3, 120000 )
	Quest_Link_1_2bs()
end
-- 부가정보 1-2b 포션 사용법
function Quest_Link_1_2bs()

	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text( "@90301109" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 부가정보 1-3a 미니맵 보는법
function Quest_Link_1_3a()
	-- 다이얼로그 출력
	dlg_title( "@90301101" )

	dlg_text_without_quest_menu( "@90301110" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 부가정보 1-4 기본무기숙련 부연설명
function Quest_Link_1_4()

	local npc_id

	-- NPC ID를 받아온다.
	npc_id = get_npc_id()

	-- NPC 이름 출력
	-- 가이아 일 때 (3013	안내인 카노프스)
	if npc_id == 3013 then
		dlg_title("@90301301")
	-- 데바 일 때 (	3011 안내인 유시스)
	elseif npc_id == 3011 then
		dlg_title("@90301101")
	-- 아수라 일 때 (3012 안내인 아로셀)
	elseif npc_id == 3012 then
		dlg_title("@90301201")
	end

	-- start of "종족에 따른 NPC 대사"
	-- 가이아 일 때 (3013	안내인 카노프스)
	if npc_id == 3013 then
		dlg_text_without_quest_menu("@90301322")
	-- 데바 일 때 (	3011 안내인 유시스)
	elseif npc_id == 3011 then
		dlg_text_without_quest_menu("@90301122")
	-- 아수라 일 때 (3012 안내인 아로셀)
	elseif npc_id == 3012 then
		dlg_text_without_quest_menu("@90301222")
	end -- end of "종족에 따른 NPC 대사"

	-- 대화 종료.
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end


   --============================================================
   --===============		안내인 아로셀
   --============================================================
function NPC_Tutorial_Guide_Asura_init()
	cprint( "!안내인 아로셀 가동" )
	set_npc_name(  "@90301200"  )
end

function NPC_Tutorial_Guide_Asura_contact()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	-- 종족을 받아온다. 가이아 3, 데바 4, 아수라 5
	local race = get_value( "race" )
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress1 =  get_quest_progress(1008)
	local quest_progress2 =  get_quest_progress(1009)
	local quest_progress3 =  get_quest_progress(1007)

	-- 안내인의 종족이 아니면
	if race ~= 5 then
		dlg_text( "@90301202" )

	-- 첫번째 퀘스트 시작전이면(수락가능이면)
	elseif quest_progress1 == 0 then
		dlg_text( "@90301203" )
		
		

	-- 세번째 퀘스트 완료후라면
	elseif quest_progress3 == 255 then
		dlg_text( "@90301205" )

	-- 그 외의 상황(1~2 중의 한 퀘스트를 마치거나  1~3중의 하나를 수행중이거나 기타 등등...)
	else
		dlg_text( "@90301204" )
	
	end
	
	-- 레냐 페이스의 뜻은?
		dlg_menu( "@90301124", "Quest_Link_2_1d()" )	
	-- 수련자의 섬 채널 이동 안내?
		dlg_menu( "@91000463", "Quest_Link_2_1e()" )
		
	--메뉴 구성 (퀘스트가 있을 경우 자동으로 제일 위부터 퀘스트 메뉴 생성)
	-- 첫 번째 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress1 == 1 or quest_progress1 == 2 or quest_progress1 == 255 then
		-- HP란 무엇인가?
		dlg_menu( "@90010034", "Quest_Link_2_1a()" )
		-- 아이템의 장착법
		dlg_menu( "@90010035", "Quest_Link_2_1b()" )
		-- 오토 트랩, 에이전트 몬스터란?
		dlg_menu( "@90301126", "Quest_Link_2_1c()" )
	end
	-- 첫 번째 퀘스트 클리어.
	if quest_progress1 == 255 then
		-- 아이템의 획득 방법은?
		dlg_menu( "@90010036", "Quest_Link_2_2as()" )
	end
	-- 두 번째 퀘스트 클리어.
	if quest_progress2 == 255 then
		-- 포션을 먹는 방법은?
		dlg_menu( "@90010037", "Quest_Link_2_2bs()" )
	end
	-- 세 번째 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress3 == 1 or quest_progress3 == 2 or quest_progress3 == 255 then
		-- 미니맵을 보는 방법
		dlg_menu( "@90010038", "Quest_Link_2_3a()" )
	end
	
		-- 대화종료 
		dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end
 

-- 퀘스트 2-1 수락 부연설명
function Quest_Link_2_1()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text_without_quest_menu( "@90301214" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_2_1a()" )
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_2_1b()" )
	-- 오토 트랩, 에이전트 몬스터란?
	dlg_menu( "@90301126", "Quest_Link_2_1c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


-- 부가정보 2-1a 	HP,LV의 개념
function Quest_Link_2_1a()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text_without_quest_menu( "@90301206" )
	
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_2_1b()" )
	-- 퀘스트란?
	dlg_menu( "@90301126", "Quest_Link_2_1c()" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

 
-- 부가정보 2-1b 	아이템 장착법
function Quest_Link_2_1b()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text_without_quest_menu( "@90301207" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_2_1a()" )
	-- 퀘스트란?
	dlg_menu( "@90301126", "Quest_Link_2_1c()" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 2-1c 	퀘스트
function Quest_Link_2_1c()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text_without_quest_menu( "@90301226" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_2_1a()" )
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_2_1b()" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 2-1d 	수련자의 섬 - 레냐 페이스
function Quest_Link_2_1d()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text_without_quest_menu( "@90301225" )
	
	-- 돌아가기
	--dlg_menu( "@90010003", "NPC_Tutorial_Guide_Asura_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 2-1e 	수련자의 섬 채널 이동 안내
function Quest_Link_2_1e()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text_without_quest_menu( "@91000464" )
	
	-- 돌아가기
	--dlg_menu( "@90010003", "NPC_Tutorial_Guide_Deva_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 버프 걸고 대사로 넘김. (공속 3레벨 10분, ID 1011)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_2_2a()
	add_state( 1011, 3, 60000 )
	Quest_Link_2_2as()
end
-- 부가정보 2-2a 아이템 획득
function Quest_Link_2_2as()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text( "@90301208" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end


-- 버프 걸고 대사로 넘김. (이속 3레벨 20분, ID 1013)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_2_2b()
	add_state( 1013, 3, 120000 )
	Quest_Link_2_2bs()
end
-- 부가정보 2-2b 포션 사용법
function Quest_Link_2_2bs()

	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text( "@90301209" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 부가정보 2-3a 미니맵 보는법
function Quest_Link_2_3a()
	-- 다이얼로그 출력
	dlg_title( "@90301201" )

	dlg_text_without_quest_menu( "@90301210" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end



   --============================================================
   --===============		안내인 카노프스
   --============================================================
function NPC_Tutorial_Guide_Gaia_init()
	cprint( "!안내인 카노프스 가동" )
	set_npc_name(  "@90301300"  )
end

function NPC_Tutorial_Guide_Gaia_contact()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	-- 종족을 받아온다. 가이아 3, 데바 4, 아수라 5
	local race = get_value( "race" )
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress1 =  get_quest_progress(1010)
	local quest_progress2 =  get_quest_progress(1011)
	local quest_progress3 =  get_quest_progress(1007)

	-- 안내인의 종족이 아니면
	if race ~= 3 then
		dlg_text( "@90301302" )

	-- 첫번째 퀘스트 시작전이면(수락가능이면)
	elseif quest_progress1 == 0 then
		dlg_text( "@90301303" )
		
		

	-- 세번째 퀘스트 완료후라면
	elseif quest_progress3 == 255 then
		dlg_text( "@90301305" )

	-- 그 외의 상황(1~2 중의 한 퀘스트를 마치거나  1~3중의 하나를 수행중이거나 기타 등등...)
	else
		dlg_text( "@90301304" )
	
	end
	
	-- 레냐 페이스의 뜻은?
		dlg_menu( "@90301124", "Quest_Link_3_1d()" )	
	-- 수련자의 섬 채널 이동 안내?
		dlg_menu( "@91000463", "Quest_Link_3_1e()" )
		
	--메뉴 구성 (퀘스트가 있을 경우 자동으로 제일 위부터 퀘스트 메뉴 생성)
	-- 첫 번째 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress1 == 1 or quest_progress1 == 2 or quest_progress1 == 255 then
		-- HP란 무엇인가?
		dlg_menu( "@90010034", "Quest_Link_3_1a()" )
		-- 아이템의 장착법
		dlg_menu( "@90010035", "Quest_Link_3_1b()" )
		-- 오토 트랩, 에이전트 몬스터란?
		dlg_menu( "@90301126", "Quest_Link_3_1c()" )
		end
	-- 첫 번째 퀘스트 클리어.
	if quest_progress1 == 255 then
		-- 아이템의 획득 방법은?
		dlg_menu( "@90010036", "Quest_Link_3_2as()" )
	end
	-- 두 번째 퀘스트 클리어.
	if quest_progress2 == 255 then
		-- 포션을 먹는 방법은?
		dlg_menu( "@90010037", "Quest_Link_3_2bs()" )
	end
	-- 세 번째 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress3 == 1 or quest_progress3 == 2 or quest_progress3 == 255 then
		-- 미니맵을 보는 방법
		dlg_menu( "@90010038", "Quest_Link_3_3a()" )
	end
	
		-- 대화종료 
		dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end
 

-- 퀘스트 3-1 수락 부연설명
function Quest_Link_3_1()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text_without_quest_menu( "@90301314" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_3_1a()" )
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_3_1b()" )
	-- 오토 트랩, 에이전트 몬스터란?
	dlg_menu( "@90301126", "Quest_Link_3_1c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end


-- 부가정보 3-1a 	HP,LV의 개념
function Quest_Link_3_1a()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text_without_quest_menu( "@90301306" )
	
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_3_1b()" )
	-- 오토 트랩, 에이전트 몬스터란?
	dlg_menu( "@90301126", "Quest_Link_3_1c()" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

 
-- 부가정보 3-1b 	아이템 장착법
function Quest_Link_3_1b()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text_without_quest_menu( "@90301307" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_3_1a()" )
	-- 오토 트랩, 에이전트 몬스터란?
	dlg_menu( "@90301126", "Quest_Link_3_1c()" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 3-1c 	오토 트랩, 에이전트 몬스터 설명
function Quest_Link_3_1c()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text_without_quest_menu( "@90301326" )
	
	-- HP란 무엇인가?
	dlg_menu( "@90010034", "Quest_Link_3_1a()" )
	-- 아이템의 장착법
	dlg_menu( "@90010035", "Quest_Link_3_1b()" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 3-1d 	수련자의 섬 - 레냐 페이스
function Quest_Link_3_1d()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text_without_quest_menu( "@90301325" )
	
	-- 돌아가기
	--dlg_menu( "@90010003", "NPC_Tutorial_Guide_Gaia_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 3-1e 	수련자의 섬 - 레냐 페이스
function Quest_Link_3_1e()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text_without_quest_menu( "@91000464" )
	
	-- 돌아가기
	--dlg_menu( "@90010003", "NPC_Tutorial_Guide_Asura_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 버프 걸고 대사로 넘김. (공속 3레벨 10분, ID 1011)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_3_2a()
	add_state( 1011, 3, 60000 )
	Quest_Link_3_2as()
end
-- 부가정보 3-2a 아이템 획득
function Quest_Link_3_2as()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text( "@90301308" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end


-- 버프 걸고 대사로 넘김. (이속 3레벨 20분, ID 1013)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_3_2b()
	add_state( 1013, 3, 120000 )
	Quest_Link_3_2bs()
end
-- 부가정보 3-2b 포션 사용법
function Quest_Link_3_2bs()

	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text( "@90301309" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 부가정보 3-3a 미니맵 보는법
function Quest_Link_3_3a()
	-- 다이얼로그 출력
	dlg_title( "@90301301" )

	dlg_text_without_quest_menu( "@90301310" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end



   --============================================================
   --===============		초급교관 시스티나
   --============================================================
function NPC_Tutorial_Instructor_1_init()
	cprint( "!초급교관 시스티나 가동" )
	set_npc_name(  "@90301400"  )
end
function NPC_Tutorial_Instructor_1_contact()

	local currnet_text = 0
	
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	-- 종족을 받아온다. 가이아 3, 데바 4, 아수라 5
	local race = get_value( "race" )
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress3 =  get_quest_progress(1007)
	local quest_progress4_1 =  get_quest_progress(1012)
	local quest_progress4_2a =  get_quest_progress(1013)
	local quest_progress4_2b =  get_quest_progress(1014)
	local quest_progress4_2c =  get_quest_progress(1015)
	local quest_progress4_3 =  get_quest_progress(1016)

	
		
	-- 퀘스트 1-3, 2-3, 3-3 받기 전
	if quest_progress3 == -1 or quest_progress3 == 0 then
		dlg_text( "@90301402" )
		currnet_text =  "@90301402" 

	-- 퀘스트 1-3, 2-3, 3-3 진행중
	elseif quest_progress3 == 1 or quest_progress3 == 2 then
		dlg_text( "@90301403" )
		currnet_text =  "@90301403" 

	-- 퀘스트 4-3 진행중
	elseif quest_progress4_3 == 1 or quest_progress3 == 2 then
		dlg_text( "@90301405" )
		currnet_text =  "@90301405" 

	-- 퀘스트 4-3 완료 이후
	elseif quest_progress4_3 == 255 then
		dlg_text( "@90301406" )
		currnet_text =  "@90301406" 

	-- 퀘스트 1-3, 2-3, 3-3 완료 이후
	elseif quest_progress3 == 255 then
		dlg_text( "@90301404" )
		currnet_text =  "@90301404" 

	-- 그 외의 상황(퀘스트 1-3, 2-3, 3-3 진행중과 같이 처리)
	else
		dlg_text( "@90301403" )
		currnet_text =  "@90301402" 
	
	end

	-- 레냐 페이스의 뜻은2?
		dlg_menu( "@90301432", "Quest_Link_4_4a()" )
	
	--메뉴 구성 (퀘스트가 있을 경우 자동으로 제일 위부터 퀘스트 메뉴 생성)
	-- 4-1a 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress4_1 == 1 or quest_progress4_1 == 2 or quest_progress4_1 == 255 then
		-- JP, JLv 에 대하여 (서브메뉴 호출)
		dlg_menu( "@90010045", "sub_menu_4_1()" )
	end
	
	-- 4-2 퀘스트, 종족에 따라 처리(데바)
	if race == 4 then
		-- 4-2b 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
		if quest_progress4_2a == 1 or quest_progress4_2a == 2 or quest_progress4_2a == 255 then
			-- 스킬이란 무엇인가?
			dlg_menu( "@90010041", "Quest_Link_4_2a()" )
		end
	-- (아수라)
	elseif race == 5 then
		-- 4-2b 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
		if quest_progress4_2b == 1 or quest_progress4_2b == 2 or quest_progress4_2b == 255 then
			-- 스킬이란 무엇인가?
			dlg_menu( "@90010041", "Quest_Link_4_2a()" )
		end
	-- (가이아)
	elseif race == 3 then
		-- 4-2c 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
		if quest_progress4_2c == 1 or quest_progress4_2c == 2 or quest_progress4_2c == 255 then
			-- 스킬이란 무엇인가?
			dlg_menu( "@90010041", "Quest_Link_4_2a()" )
		end
	-- 그 외의 종족(없음.)
	else
	end
	
	-- 4-2a 또는 4-2b 또는 4-2c 퀘스트를 완료했으면
	if quest_progress4_2a == 255 or quest_progress4_2b == 255 or quest_progress4_2c == 255 then
		-- 스킬에 대한 팁 (서브메뉴 호출)
		dlg_menu( "@90010046", "sub_menu_4_2()" )
	end

	-- 4-3 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress4_3 == 1 or quest_progress4_3 == 2 or quest_progress4_3 == 255 then
		-- 퀵슬롯이란 무엇인가?
		dlg_menu( "@90010044", "Quest_Link_4_3a()" )
	end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end

--메인메뉴에서 대사 유지한 체로 서브메뉴 버튼 호출
function sub_menu_4_1()

	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	-- 서브 메뉴가 나타날 때 서두 대사
	dlg_text_without_quest_menu("@90301430")
	
	-- JP란 무엇인가?
	dlg_menu( "@90010039", "Quest_Link_4_1a()" )
	-- 잡레벨은 무엇인가?
	dlg_menu( "@90010040", "Quest_Link_4_1b()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 	
end
--메인메뉴에서 대사 유지한 체로 서브메뉴 버튼 호출
function sub_menu_4_2()

	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	-- 서브 메뉴가 나타날 때 서두 대사
	dlg_text_without_quest_menu("@90301431")
	
	-- 스킬레벨이란 무엇인가?
	dlg_menu( "@90010042", "Quest_Link_4_2b()" )
	-- 스킬 투자의 팁
	dlg_menu( "@90010043", "Quest_Link_4_2c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end

-- 퀘스트 4-1 수락 링크
function Quest_Link_4_0a()

	-- 현재 위치 받아오기	
	local current_x = get_value( "x" )
	local current_y = get_value( "y" )
	
    -- 귀환지역을 수련자 섬의 현재 위치로 설정
    if (gv('rx') == nil or gv('rx') == "") and (gv('ry') == nil or gv('ry') == "") then -- 해외
		set_flag( "rx", current_x + math.random(0,10))
		set_flag( "ry", current_y + math.random(0,10))
	else -- 국내
		sv( "rx", current_x + math.random(0,10))
		sv( "ry", current_y + math.random(0,10))
	end
	

    -- 퀘스트 4-1 수락 부연 설명 호출
	Quest_Link_4_1()
end
-- 추가대사 4-1, 퀘스트 4-1 수락 부연설명
function Quest_Link_4_1()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301415" )
	
	-- JP란 무엇인가?
	dlg_menu( "@90010039", "Quest_Link_4_1a()" )
	-- 잡레벨은 무엇인가?
	dlg_menu( "@90010040", "Quest_Link_4_1b()" )
	-- 스킬이란 무엇인가?
	dlg_menu( "@90010041", "Quest_Link_4_2a()" )
	-- 퀵슬롯이란 무엇인가?
	dlg_menu( "@90010044", "Quest_Link_4_3a()" )


		-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 퀘스트 4-1 클리어 링크
function Quest_Link_4_0b()

	-- 종족을 받아온다. 가이아 3, 데바 4, 아수라 5
	local race = get_value( "race" )

	-- 종족에 따라 퀘스트 시작시킴. 퀘스트 시작 함수 => start_quest( 퀘스트ID, 시작대사ID )
	-- 버그로 인한 긴급 삭제
--	if race == 4 then
--		start_quest(1013, 301417)
--	elseif race == 5 then
--		start_quest(1014, 301420)
--	elseif race == 3 then
--		start_quest(1015, 301423)
--	end
end


-- 부가정보 4-1a, JP의 개념
function Quest_Link_4_1a()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301407" )
	
	-- 잡레벨은 무엇인가?
	dlg_menu( "@90010040", "Quest_Link_4_1b()" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 4-1b, 잡레벨의 개념
function Quest_Link_4_1b()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301408" )
	
	-- JP란 무엇인가?
	dlg_menu( "@90010039", "Quest_Link_4_1a()" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 퀘스트 4-2 수락 링크
function Quest_Link_4_2()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301428" )

	-- 스킬이란 무엇인가?
	dlg_menu( "@90010041", "Quest_Link_4_2a()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 4-2a, 스킬이란 무엇인가
function Quest_Link_4_2a()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301409" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 4-2b, 스킬레벨은 무엇인가
function Quest_Link_4_2b()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301410" )
	
	-- 스킬 투자의 팁
	dlg_menu( "@90010043", "Quest_Link_4_2c()" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 4-2c, 스킬투자의 팁
function Quest_Link_4_2c()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301411" )
	
	-- 스킬레벨이란 무엇인가?
	dlg_menu( "@90010042", "Quest_Link_4_2b()" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 퀘스트 4-3 수락 링크
function Quest_Link_4_3()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )
	
	-- 국가 코드 읽어오기
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_EUROPE                     = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
	
	local state_code = get_local_info() 
	
		
	-- 중국일 경우
	if state_code == 64 then
		
		--dlg_title("@90301401")
		
		-- 워프시켜주겠다는 설명
		-- dlg_text( "@90301806" )
		
		-- 중급 교관 길다스의 위치한 곳으로 워프 시켜줌.
		warp( 172711 + math.random(0,10) , 52362 + math.random(0,10) )

		-- 대화종료
		--dlg_menu( "@90010002", " " )
 		--dlg_show()
	end
	dlg_text_without_quest_menu( "@90301427" )
	
	-- 퀵슬롯이란 무엇인가?
	dlg_menu( "@90010044", "Quest_Link_4_3a()" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 4-3a, 퀵슬롯이란 무엇인가
function Quest_Link_4_3a()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301429" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 부가정보 4-4a, 레냐 페이스란?
function Quest_Link_4_4a()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301433" )
	
	-- 수련과 재활?
	dlg_menu( "@90301434", "Quest_Link_4_4a_1()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 부가정보 4-4a_1, 수련과 재활?
function Quest_Link_4_4a_1()
	-- 다이얼로그 출력
	dlg_title( "@90301401" )

	dlg_text_without_quest_menu( "@90301435" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

   --============================================================
   --===============		중급교관 길다스
   --============================================================
function NPC_Tutorial_Instructor_2_init()
	cprint( "!중급교관 길다스 가동" )
	set_npc_name(  "@90301500"  )
end

function NPC_Tutorial_Instructor_2_contact()

	local currnet_text = 0
	
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress4_3 =  get_quest_progress(1016)
	local quest_progress5_1 =  get_quest_progress(1017)
	local quest_progress5_3 =  get_quest_progress(1019)
	local quest_progress5_5 =  get_quest_progress(1021)
	local quest_progress5_9 =  get_quest_progress(1024)

		
	-- 퀘스트 4-3 받기 전
	if quest_progress4_3 == -1 or quest_progress4_3 == 0 then
		dlg_text( "@90301502" )
		currnet_text =  "@90301502" 

	-- 퀘스트 4-3 진행중
	elseif quest_progress4_3 == 1 or quest_progress4_3 == 2 then
		dlg_text( "@90301503" )
		currnet_text =  "@90301503" 

	-- 퀘스트 5-9 완료 이후 (수료후, 수료후가 없으므로 상급교관 만나기 퀘스트가 끝났다면 수료후로 간주.)
	elseif quest_progress5_9 == 255 then
		dlg_text( "@90301506" )
		currnet_text =  "@90301506" 

	-- 퀘스트 5-5 완료 이후
	elseif quest_progress5_5 == 255 then
		dlg_text( "@90301505" )
		currnet_text =  "@90301505" 

	-- 퀘스트 4-3 완료 이후
	elseif quest_progress4_3 == 255 then
		dlg_text( "@90301540" )
		currnet_text =  "@90301540" 
	
	-- 그 외의 상황(4-3 받기 전과 같이 처리)
	else
		dlg_text( "@90301502" )
		currnet_text =  "@90301502" 
	
	end
	
	-- 수련자의 섬 레냐 페이스3?
	dlg_menu( "@90301547", "Quest_Link_5_11a()" )
		
	--메뉴 구성 (퀘스트가 있을 경우 자동으로 제일 위부터 퀘스트 메뉴 생성)
	-- 5-1 퀘스트를 받은 적이 있으면...(수행, 종료여부에 관계없이)
	if quest_progress5_1 == 1 or quest_progress5_1 == 2 or quest_progress5_1 == 255 then
		-- 아이템 레벨에 대하여 
		dlg_menu( "@90010047", "sub_menu_5_1()" )
	end
	-- 퀘스트 5-3을 클리어
	if quest_progress5_3 == 255 then
		-- JP관리에 관한 팁 (서브메뉴 호출)
		dlg_menu( "@90010048", "sub_menu_5_3()" )
	end
	-- 퀘스트 5-5을 클리어
	if quest_progress5_5 == 255 then
		-- 파티를 하려면
		dlg_menu( "@90010049", "Quest_Link_5_5a()" )
		-- 아이템 트레이드 방법
		dlg_menu( "@90010050", "Quest_Link_5_5b()" )
	end
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
 
end

-- 추가대사 5-1, 퀘스트 5-1 수락 부연설명
function Quest_Link_5_1()
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301516" )
	
	-- 아이템 업그레이드란?
	dlg_menu( "@90010054", "Quest_Link_5_1a()" )
	-- 아이템 랭크란?
	dlg_menu( "@90010055", "Quest_Link_5_1b()" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
--메인메뉴에서 대사 유지한 체로 서브메뉴 버튼 호출
function sub_menu_5_1()

	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	-- 서브 메뉴가 나타날 때 서두 대사
	dlg_text_without_quest_menu("@90301527")

	-- 아이템 업그레이드란?
	dlg_menu( "@90010054", "Quest_Link_5_1a()" )
	-- 아이템 랭크란?
	dlg_menu( "@90010055", "Quest_Link_5_1b()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 	
end

-- 부가정보 5-1a, 아이템 업그레이드란?
function Quest_Link_5_1a()
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301507" )
	
	-- 아이템 랭크란?
	dlg_menu( "@90010055", "Quest_Link_5_1b()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 부가정보 5-1b, 아이템 랭크란?
function Quest_Link_5_1b()
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301508" )
	
	-- 아이템 업그레이드란?
	dlg_menu( "@90010054", "Quest_Link_5_1a()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

--메인메뉴에서 대사 유지한 체로 서브메뉴 버튼 호출
function sub_menu_5_3()

	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	-- 서브 메뉴가 나타날 때 서두 대사
	dlg_text_without_quest_menu("@90301528")

	-- 초반 스킬의 중요성
	dlg_menu( "@90010051", "Quest_Link_5_3a()" )
	-- JP를 빨리 모으려면
	dlg_menu( "@90010052", "Quest_Link_5_3b()" )
	-- 전직을 늦게 해도 되나?
	dlg_menu( "@90010053", "Quest_Link_5_3c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 	
end
 
-- 부가정보 5-3a, 초반 스킬의 중요성
function Quest_Link_5_3a()
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301509" )
	
	-- JP를 빨리 모으려면
	dlg_menu( "@90010052", "Quest_Link_5_3b()" )
	-- 전직을 늦게 해도 되나?
	dlg_menu( "@90010053", "Quest_Link_5_3c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 5-3b, JP를 빨리 모으려면
function Quest_Link_5_3b()
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301510" )
	
	-- 초반 스킬의 중요성
	dlg_menu( "@90010051", "Quest_Link_5_3a()" )
	-- 전직을 늦게 해도 되나?
	dlg_menu( "@90010053", "Quest_Link_5_3c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 5-3c, 전직을 늦게 해도 되나?
function Quest_Link_5_3c()
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301511" )
	
	-- 초반 스킬의 중요성
	dlg_menu( "@90010051", "Quest_Link_5_3a()" )
	-- JP를 빨리 모으려면
	dlg_menu( "@90010052", "Quest_Link_5_3b()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 5-5a, 파티를 하려면
function Quest_Link_5_5a()
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301512" )

	-- 아이템 트레이드 방법
	dlg_menu( "@90010050", "Quest_Link_5_5b()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 5-5b, 아이템 트레이드의 방법
function Quest_Link_5_5b()
	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301513" )

	-- 파티를 하려면
	dlg_menu( "@90010049", "Quest_Link_5_5a()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 5-6, 부스트칩에 대한 설명
function Quest_Link_5_6()

    -- 귀환지역을 캠프 중앙으로 설정.
	
	if (gv('rx') == nil or gv('rx') == "") and (gv('ry') == nil or gv('ry') == "") then -- 해외
		set_flag( "rx", 172543 + math.random(0,100))
		set_flag( "ry", 51847 + math.random(0,100))
	else -- 국내
		sv( "rx", 172543 + math.random(0,100))
		sv( "ry", 51847 + math.random(0,100))
	end

	-- 다이얼로그 출력
	dlg_title( "@90301501" )

	dlg_text_without_quest_menu( "@90301504" )
	dlg_text_without_quest_menu( "@90301532" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 5-10a, 크리처와 카드에 대한 간략한 설명.
function Quest_Link_5_10a()
	-- 다이얼로그 출력
	dlg_title( "@90302301" )

	dlg_text_without_quest_menu( "@90301530" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 5-10b, 크리처와 카드에 대한 좀 더 자세한
function Quest_Link_5_10b()

	local temp_text, npc_id
	-- 플레이어의 종족값을 받아옴 3 가이아, 4 데바, 5 아수라
	local race = get_value( "race" )
	--NPC ID를 받아온다.
	npc_id = get_npc_id()
	
	-- 각 종족에 따라 이름 출력
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_title("@90400100")
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_title("@90100100")
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_title("@90200100")
	end
	
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_text_without_quest_menu("@90400110")
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_text_without_quest_menu("@90100110")
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_text_without_quest_menu("@90200110")
	end

	-- 대화 종료
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end

-- 부가정보 5-11a, 수련자의 섬 레냐 페이스란3?
function Quest_Link_5_11a()
	-- 다이얼로그 출력
	dlg_title( "@90302301" )

	dlg_text_without_quest_menu( "@90301548" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

   --============================================================
   --===============		상급교관 란슬롯
   --============================================================
function NPC_Tutorial_Instructor_3_init()
	cprint( "!상급교관 란슬롯 가동" )
	set_npc_name(  "@90301600"  )
end
function NPC_Tutorial_Instructor_3_contact()

	-- 다이얼로그 출력

	dlg_title( "@90301601" )
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	
	local quest_progress5_9 =  get_quest_progress(1024)
	local stamina, text
	
		stamina = gv("stamina")    -- 현재 스태미너의 량을 받아와 컨디션을 체크한다.
	
	
	-- 퀘스트 5-9 받기 전
	if quest_progress5_9 == -1 or quest_progress5_9 == 0 then
		dlg_text( "@90301602" )

	-- 퀘스트 5-9 진행중
	elseif quest_progress5_9 == 1 or quest_progress5_9 == 2 then
		dlg_text( "@90301603" )

	-- 퀘스트 5-9 완료 이후
	elseif quest_progress5_9 == 255 then
		-- 스태미너가 보통이거나 나쁠때
		if stamina < (158400*0.85) then
			dlg_text( "@90301609" )
		-- 스태미너가 좋을때
		else
			dlg_text( "@90301604" )
		end

	-- 그 외의 상황(5-9 받기 전과 같이 처리)
	else
		dlg_text( "@90301602" )
	
	end
		
	--메뉴 구성 (퀘스트가 있을 경우 자동으로 제일 위부터 퀘스트 메뉴 생성)
	
	-- 퀘스트 5-9 완료 이후
	if quest_progress5_9 == 255 then
		-- 스킬과 JLv의 조율
		dlg_menu( "@90010057", "Quest_Link_7_1b()" )
		-- 스킬 투자의 팁
		dlg_menu( "@90010058", "Quest_Link_7_1c()" )
		-- 퀵슬롯 페이지의 확장
		dlg_menu( "@90010059", "Quest_Link_7_1d()" )
		-- 컨디션이란 무엇인가
		dlg_menu( "@90010071", "Quest_Link_7_2b()" )
		
	end
	
	-- 다른 수련자 캠프로 이동
	dlg_menu( "@90300513", 'Teleport_channel( 1000 )')

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 버프 걸고 대사로 넘김. (공격력,방어력,공속 3레벨 20분, ID 1007, 1009, 1011)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_7_1()
	add_state( 1007, 3, 120000 )
	add_state( 1009, 3, 120000 )
	add_state( 1011, 3, 120000 )
	Quest_Link_7_1a()
end


-- 보상으로 스태미너 1000 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_7_2()

	local stamina

	stamina = gv("stamina")
	sv("stamina",stamina + 100000 )
	
end

-- 부가정보 7-1a, 차회예고
function Quest_Link_7_1a()
	-- 다이얼로그 출력
	dlg_title( "@90301601" )

	dlg_text_without_quest_menu( "@90301605" )

	-- 스킬과 JLv의 조율
	dlg_menu( "@90010057", "Quest_Link_7_1b()" )
	-- 스킬 투자의 팁
	dlg_menu( "@90010058", "Quest_Link_7_1c()" )
	-- 퀵슬롯 페이지의 확장
	dlg_menu( "@90010059", "Quest_Link_7_1d()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 7-1b, 스킬과 JLv의 조율
function Quest_Link_7_1b()
	-- 다이얼로그 출력
	dlg_title( "@90301601" )

	dlg_text_without_quest_menu( "@90301606" )

	-- 스킬 투자의 팁
	dlg_menu( "@90010058", "Quest_Link_7_1c()" )
	-- 퀵슬롯 페이지의 확장
	dlg_menu( "@90010059", "Quest_Link_7_1d()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 7-1c, 스킬 투자의 팁
function Quest_Link_7_1c()
	-- 다이얼로그 출력
	dlg_title( "@90301601" )

	dlg_text_without_quest_menu( "@90301607" )

	-- 스킬과 JLv의 조율
	dlg_menu( "@90010057", "Quest_Link_7_1b()" )
	-- 퀵슬롯 페이지의 확장
	dlg_menu( "@90010059", "Quest_Link_7_1d()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
-- 부가정보 7-1d, 퀵슬롯 페이지의 확장
function Quest_Link_7_1d()
	-- 다이얼로그 출력
	dlg_title( "@90301601" )

	dlg_text_without_quest_menu( "@90301608" )

	-- 스킬과 JLv의 조율
	dlg_menu( "@90010057", "Quest_Link_7_1b()" )
	-- 스킬 투자의 팁
	dlg_menu( "@90010058", "Quest_Link_7_1c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 부가정보 7-2b, 컨디션이란 무엇인가?
function Quest_Link_7_2b()
	-- 다이얼로그 출력
	dlg_title( "@90301601" )

	dlg_text_without_quest_menu( "@90301610" )
	
	-- 스테미너 회복 방법은?
	dlg_menu( "@90010072", "Quest_Link_7_2c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 부가정보 7-2c, 스테미너 회복 방법은?
function Quest_Link_7_2c()
	-- 다이얼로그 출력
	dlg_title( "@90301601" )

	dlg_text_without_quest_menu( "@90301611" )
	
	-- 컨디션이란 무엇인가?
	dlg_menu( "@90010071", "Quest_Link_7_2b()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end


   --============================================================
   --===============		잡화상인 쟈팡
   --============================================================
function NPC_Tutorial_Merchant_Etc_init()
	cprint( "!잡화상인 쟈팡 가동" )
	set_npc_name(  "@90301700"  )
end
function NPC_Tutorial_Merchant_Etc_contact()
	-- 다이얼로그 출력
	dlg_title( "@90301701" )
	dlg_text( "@90301706" )

	dlg_menu( "@90300703", "open_market( 'beginner_etc' )" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end
 
   --============================================================
   --===============		방어구상인 카츄아
   --============================================================
function NPC_Tutorial_Merchant_Equip_init()
	cprint( "!방어구상인 카츄아 가동" )
	set_npc_name(  "@90301800"  )
end
function NPC_Tutorial_Merchant_Equip_contact()
	-- 다이얼로그 출력
	dlg_title( "@90301801" )
	dlg_text( "@90301806" )

	dlg_menu( "@90300203", "open_market( 'beginner_equip' )" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
 
end
	
   --============================================================
   --===============		잡서포터 데네브
   --============================================================
function	NPC_Tutorial_Job_Change_init()
	cprint( "!잡서포터 데네브 가동" )
	set_npc_name(  "@90301900"  )
end

function	NPC_Tutorial_Job_Change_contact()
	
	dlg_title( "@90301901" )
	
	dlg_text( "@90301910" )
	
	dlg_menu( "@90010060", "NPC_Tutorial_Change_job()" )
	dlg_menu( "@90010252", "NPC_JobChange_change_race()" )
	dlg_menu( "@90010002", " " )
	
	dlg_show()
	
end

function	NPC_Tutorial_Change_job()
	-- 다이얼로그 출력
	dlg_title( "@90301901" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress5_8 =  get_quest_progress(1023)
	local quest_progress6_2 =  get_quest_progress(1025)
	local quest_progress6_3 =  get_quest_progress(1027)

	-- 퀘스트 6_2 진행중
	if quest_progress6_2 == 0 then --1 or quest_progress6_2 == 2 then
		if get_value( "level" ) < 10 or get_value( "job_level" ) < 10 then
			dlg_text_without_quest_menu( "@90301911" )
		else
			dlg_text( "@90301903" )
		end
	-- 이미 전직을 했다면.
	elseif get_value( "job_depth" ) == 1 then
		dlg_text( "@90301910" )
	--레벨과 잡레벨이 모자라거나 퀘스트 5-8을 받기 전이면...
--	elseif get_value( "level" ) < 10 or get_value( "job_level" ) < 10 or quest_progress5_8 ~= 1 or quest_progress5_8 ~= 2 or quest_progress5_8 ~= 255 then
	elseif get_value( "level" ) < 10 or get_value( "job_level" ) < 10 then
		dlg_text( "@90301902" )
	elseif get_value( "level" ) >= 10 and get_value( "job_level" ) >= 10 and ( quest_progress5_8 == -1 or quest_progress5_8 == 0 ) then
		dlg_text( "@90301922" )
	-- 그 외의 상황
	else
		dlg_text( "@90301910" )
	
	end
		
	--메뉴 구성 (퀘스트가 있을 경우 자동으로 제일 위부터 퀘스트 메뉴 생성)

	-- 퀘스트 6-3 완료했고, 아직 전직하지 않았을 때.(레벨 10이상, 잡레벨 10이상)
	if get_value( "job_depth" ) == 0 and quest_progress6_3 == 255 and get_value( "level" ) >= 10 and get_value( "job_level" ) >= 10 then
		-- 전직하기
		dlg_menu( "@90010060", "Quest_Link_6_3()" )	
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end
-- 부가정보 6-3, 전직하기
function Quest_Link_6_3()


	-- 0차 직업 상태가 아니면 쫓아낸다. 본래는 1차 직업은 쫓아낸다에서 0차 아니면 쫓아낸다로 변경
	-- 대만에서 왠 미친 2차 직업이 수련자의 섬으로 들어가서 2차에서 1차로 전직하는 사태 발생
	if get_value( "job_depth" ) ~= 0 then
		return
	end
	
	-- 여기까지 왔으면 전직 가능
	
	local race = get_value( "race" )
	local job_name1, job1, job_name2, job2, job_name3, job3, current_job

	dlg_title( "@90301901" )

	dlg_text_without_quest_menu( "@90301906" )

	-- 현재 직업 ID를 받아 놓는다.
	current_job = get_value( "job" )

	--================================================================================
	-- 1차 전직만 취급함.
	--================================================================================
	
		--============================================================================
		-- 얘가 데바면 디바인솔져 또는 클레릭, 브리더가 될 수 있다.
		if race == 4 then
			job_name1 =  "@10201" 
			job1 = 201
			job_name2 =  "@10202"
			job2 = 202
			job_name3 =  "@10203" 
			job3 = 203

		--============================================================================
		-- 아수라면 스트라이더나 다크매지션, 소서러가 될 수 있다.
		elseif race == 5 then
			job_name1 =  "@10301" 
			job1 = 301
			job_name2 =  "@10302" 
			job2 = 302
			job_name3 =  "@10303" 
			job3 = 303
	
		--============================================================================
		-- 데바도 아수라도 아니면 가이아겠지. 투사 101, 주술사 103, 마수사 104 가 될 수 있다.
		else
			job_name1 =  "@10101" 
			job1 = 101
			job_name2 =  "@10102" 
			job2 = 102
			job_name3 =  "@10103" 
			job3 = 103
		end
	
		
	dlg_menu( job_name1 , "Run_JobChange_Tutorial_check( '" .. job_name1 .. "' , " .. job1 .. " )" )
	dlg_menu( job_name2 , "Run_JobChange_Tutorial_check( '" .. job_name2 .. "' , " .. job2 .. " )" )
	dlg_menu( job_name3 , "Run_JobChange_Tutorial_check( '" .. job_name3 .. "' , " .. job3 .. " )" )

		
	-- 대화종료 
	dlg_menu( "@90010002", " " )

 
	dlg_show()
	

end


function Run_JobChange_Tutorial_check( job_name , job_id )

	local jobid = job_id
	-- 클릭 실수했을지도 모르므로 확실한지 확인해줌.

	cprint( job_name )
	
	dlg_title( "@90301901" )
	if jobid == 101 then
	    dlg_text_without_quest_menu("@90301919")
    elseif jobid == 102 then
	    dlg_text_without_quest_menu("@90301920")
    elseif jobid == 103 then
	    dlg_text_without_quest_menu("@90301921")
	elseif jobid == 201 then
	    dlg_text_without_quest_menu("@90301913")
    elseif jobid == 202 then
	    dlg_text_without_quest_menu("@90301914")
    elseif jobid == 203 then
	    dlg_text_without_quest_menu("@90301915")
  	elseif jobid == 301 then
	    dlg_text_without_quest_menu("@90301916")
    elseif jobid == 302 then
	    dlg_text_without_quest_menu("@90301917")
    elseif jobid == 303 then
	    dlg_text_without_quest_menu("@90301918")
	end
    
	dlg_menu( "@90010195", "Run_JobChange_Tutorial( '" .. job_name .. "' , "  .. job_id ..' )' )
	dlg_menu( "@90010196", 'Quest_Link_6_3()' )
	
	dlg_show()
	
end


function Run_JobChange_Tutorial( job_name , job_id )

	local current_job, current_jlv, job_depth

	-- 에헤라디야~~ 전직이요~

	dlg_title( "@90301901" )
	--##시작
	dlg_text_without_quest_menu(sconv("@90301908", "#@job_name@#",job_name))
    --##끝 축하합니다!!<BR>지금 이순간부터 #@job_name@#(/이)라는 직업을 가지게 되셨습니다.<BR>어엿한 직업인이 되셨군요.'
	
	dlg_menu( "@90010002", '' )
	
	dlg_show()

	-- 전직 전 직업과 잡레벨을 저장한다.
	current_job = get_value( "job" )
	current_jlv = get_value( "job_level" )
	job_depth = get_value( "job_depth" )
	if job_depth == 0 then
		set_value( "job_0", current_job )
		set_value( "jlv_0", current_jlv )
		--0차에서 1차 전직할때 "신성한 불꽃의 힘" 아이템을 1개 지급해준다. 해당 아이템은 잡화상점에서도 판매한다.
		--insert_item( 2013081, 1 ) --아이템 능력치가 너무 좋아서 미적용으로 수정
		--cprint( "@90606144" )
	elseif job_depth == 1 then
		set_value( "job_1", current_job )
		set_value( "jlv_1", current_jlv )
	elseif job_depth == 2 then
		set_value( "job_2", current_job )
		set_value( "jlv_2", current_jlv )
	end
	
	job_depth = job_depth + 1
	-- 전직 시키기.
	set_value( "job" , job_id )
	set_value( "job_level" , 1 )
	set_value( "job_depth" , job_depth )
	--##시작
	--message( job_name ..  "@90300408"  )
    message(sconv("@90300408", "#@job_name@#",job_name))
    --##끝 #@job_name@#(/으)로 전직했습니다.

	
end

	
   --============================================================
   --===============		수련생OB 트리스탄
   --============================================================
function	NPC_Tutorial_Old_Boy_init()
	cprint( "!수련생OB 트리스탄 가동" )
	set_npc_name(  "@90302000"  )
end
function	NPC_Tutorial_Old_Boy_contact()
	-- 다이얼로그 출력
	dlg_title( "@90302001" )
	dlg_text( "@90302008" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

-- 전직 퀘 워프 지원
function Quest_Link_6_1()
	-- 트리스탄 위치에서 동쪽으로 2미터 떨어진 곳으로 워프 시켜줌.
	warp( 175172 + math.random(0,10) , 58576 + math.random(0,10) )
end

-- 버프 걸고 퀘스트 시작 (이동속도 3레벨 10분, ID 1013)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
-- 버그로 인한 긴급 삭제
function Quest_Link_6_1s()
	add_state( 1013, 3, 60000 )
--	start_quest(1026,302003)
end
-- 추가대사 6-2, 퀘스트 6-2 수락 부연설명
function Quest_Link_6_2()
	-- 다이얼로그 출력
	dlg_title( "@90302001" )

	dlg_text_without_quest_menu( "@90302005" )

	-- 오렌지 포션 5를 넣어 준다.
	local q_flag = get_flag( "q1" )
	-- 받은 적이 없으면 준다. 아니면 안준다.
	if q_flag ~= 1 then
		insert_item( 602502, 5 )
		set_flag( "q1", "1")
	end
		
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 버프 걸고 퀘스트 시작 (공격속도 3레벨 10분, ID 1013)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_18_1()
	add_state( 1011, 3, 60000 )
end

-- 퀘스트 완료 후 버프  (이동속도 3레벨 10분, ID 1013)  지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
function Quest_Link_18_3()
	add_state( 1013, 3, 60000 )
end

-- 나비스 라미아<반복> 퀘스트시 진행 아이템 넣어주기
function Quest_Link_19_1()
	insert_item( 1000077, 1 )
	insert_item( 1000076, 1 )
	insert_item( 1000075, 1 )
	insert_item( 1000068, 1 )
end


-- 유령선 나비스 라미아 완료시 아이템 삭제
function Quest_Link_20_1()
--마녀의 팬던트 카운트 구하기
	local item_1_count = find_item ( 1000068 )
--마녀의 팬던트 조각 1~6 카운트 구하기
	local item_2_1_count = find_item ( 1000069 )	
	local item_2_2_count = find_item ( 1000070 )	
	local item_2_3_count = find_item ( 1000071 )
	local item_2_4_count = find_item ( 1000072 )
	local item_2_5_count = find_item ( 1000073 )
	local item_2_6_count = find_item ( 1000074 )
--마녀의 귀걸이, 마력수정, 고리수정 카운트 구하기
	local item_3_1_count = find_item ( 1000075 )
	local item_3_2_count = find_item ( 1000076 )
	local item_3_3_count = find_item ( 1000077 )
	
	
	for i = 0, 14 do
	
		if item_1_count == 0 or item_2_1_count == 0 or item_2_2_count == 0 or item_2_3_count == 0 or item_2_4_count == 0 or item_2_5_count == 0 or item_2_6_count == 0 or item_3_1_count == 0 or item_3_2_count == 0 or item_3_3_count == 0 then

		--마녀의 팬던트 삭제하기
		delete_item( get_item_handle( 1000068 ), item_1_count )
		--마녀의 팬던트 조각 1~6 삭제하기
		delete_item( get_item_handle( 1000069 ), item_2_1_count )
		delete_item( get_item_handle( 1000070 ), item_2_2_count )
		delete_item( get_item_handle( 1000071 ), item_2_3_count )
		delete_item( get_item_handle( 1000072 ), item_2_4_count )
		delete_item( get_item_handle( 1000073 ), item_2_5_count )
		delete_item( get_item_handle( 1000074 ), item_2_6_count )
		--마녀의 귀걸이, 마력수정, 고리수정 삭제하기
		delete_item( get_item_handle( 1000075 ), item_3_1_count )
		delete_item( get_item_handle( 1000076 ), item_3_2_count )
		delete_item( get_item_handle( 1000077 ), item_3_3_count )
		
		end
	
	end
	
end

   --============================================================
   --===============		모험가이드 크레아
   --============================================================
function NPC_Adventure_Guide_beginner_contact()
	-- 다이얼로그 출력
	dlg_title( "@90302201" )
	dlg_text("@90704102")

	dlg_menu( "@90704103", "open_market( 'normal_riding' )" )	
	
	dlg_menu( "@90302203", "Quest_Link_8_1c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
end

	
	
-- 부가정보 8-1a, 탑승용 크리처란?(완료 링크)
function Quest_Link_8_1a()
	-- 다이얼로그 출력
	dlg_title( "@90302201" )

	dlg_text_without_quest_menu( "@90302204" )
	
	-- 부가정보 8-1b, 수련자용 탑승 크리처 지급.
	dlg_menu( "@90010076", "Quest_Link_8_1b()" )
			
	dlg_show()

end

-- 부가정보 8-1b, 수련자용 탑승 크리처 지급.
function Quest_Link_8_1b()
	-- 다이얼로그 출력
	dlg_title( "@90302201" )

	dlg_text_without_quest_menu( "@90302209" )
		
	-- 아이템을 지급한다.
--	insert_item(540099, 1)
		
	-- 인벤에서 해당 카드를 찾아 편성 가능하면 편성한다.
--	equip_summon_card(540099)
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 부가정보 8-1c, 탑승용 크리처란?(서브 메뉴)
function Quest_Link_8_1c()
	-- 다이얼로그 출력
	dlg_title( "@90302201" )

	dlg_text_without_quest_menu( "@90302204" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
			
	dlg_show()

end

-- 크리처 편성

function card_formation()
	-- 다이얼로그 출력
	dlg_title( "@90302201" )

	if has_item(540017) == 1 then
	
	dlg_text( "@90302211")
	
	equip_summon_card(540017)
	
	else
	
	dlg_text( "@90302210")
	
	end
				
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

-- 대여 조건 설명하는 함수
function NPC_Adventure_Guide_rental_info()
	
	dlg_title("@90302201")
	dlg_text_without_quest_menu("@90302213")
	-- 대화 종료
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end-- 펑션 끝

	--============================================================
   --===============		부스트칩교환상 슈리르
   --============================================================
function NPC_ChipExchange_beginner_contact()

	-- 수련자의 섬 슈리르 (3023 부스트칩교환상 슈리르)
	dlg_title( "@90302301" )
	
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress5_6 =  get_quest_progress(1030)
	local quest_progress_lak =  get_quest_progress(1032)

	-- <대사 출력>
	-- 라크 퀘스트가 종료가능할 경우(라크를 모아왔을 경우) 교환이나 환전은 퀘스트 완료하지 않으면 불가능하다는 대사 출력.
	if quest_progress_lak == 2 then
		dlg_text( "@90302324" )
		
	-- 보통의 경우에는 일반 대사 출력 및 교환, 환전 메뉴 출력
	else
		dlg_text( "@90302302" )
		-- 라크를 부스트 칩으로 교환한다.
		dlg_menu( "@90010077", 'ChipExchange_lak_to_chip()' )
		-- 라크를 루피로 환전한다.
		dlg_menu( "@90010078", 'ChipExchange_lak_to_RP()' )
	end
	
	-- 메뉴 구성 (퀘스트가 있을 경우 자동으로 제일 위부터 퀘스트 메뉴 생성)
				
		-- 부스트 칩을 구입한다.
		--dlg_menu( "@90010079", "open_market( 'beginner_boost_chip' )" )
		dlg_menu( "@90010079", "open_market( 'boost_chip' )" )
		
		-- 퀘스트 5-6을 클리어
	if quest_progress5_6 == 255 then
		-- 목걸이란?
		dlg_menu( "@90010067", "Quest_Link_5_7a()" )	
	end
		
		-- 부스트 칩의 종류를 물어본다.
		dlg_menu( "@90010073", 'ChipExchange_kind_of_chips()' )
		--대화 종료
		dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()
 
end
		 
	-- 부스트 칩의 종류를 물어본다.
function ChipExchange_kind_of_chips()

	-- 수련자의 섬 슈리르 (3023 부스트칩교환상 슈리르)
		dlg_title("@90302301")
		-- <대사 출력>
		dlg_text("@90302307")
	-- 부스트칩을 싸게 사려면?
		dlg_menu( "@90010074", "ChipExchange_wholesale_chips()" )			
	-- 대화 종료
		dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()
end

-- 부스트 칩을 싸게 사려면?
function ChipExchange_wholesale_chips()

	-- 수련자의 섬 슈리르 (3023 부스트칩교환상 슈리르)
		dlg_title("@90302301")
	-- <대사 출력>
		dlg_text("@90302308")
	-- 대화 종료
		dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()

end

-- 부가정보 5-7a, 목걸이란?
function Quest_Link_5_7a()
	-- 다이얼로그 출력
	dlg_title( "@90302301" )

	dlg_text_without_quest_menu( "@90302316" )
	
	-- 카오스 스톤이란?
	dlg_menu( "@90010068", "Quest_Link_5_7b()" )
	
	-- 라크란?
	dlg_menu( "@90010069", "Quest_Link_5_7c()" )
		
	-- 라크를 모으는 방법은?
	dlg_menu( "@90010070", "Quest_Link_5_7d()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()	

end

-- 부가정보 5-7b, 카오스 스톤이란?
function Quest_Link_5_7b()
	-- 다이얼로그 출력
	dlg_title( "@90302301" )

	dlg_text_without_quest_menu( "@90302317" )
			
	-- 라크란?
	dlg_menu( "@90010069", "Quest_Link_5_7c()" )
		
	-- 라크를 모으는 방법은?
	dlg_menu( "@90010070", "Quest_Link_5_7d()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()	

end

-- 부가정보 5-7c, 라크란?
function Quest_Link_5_7c()
	-- 다이얼로그 출력
	dlg_title( "@90302301" )

	dlg_text_without_quest_menu( "@90302318" )
			
	-- 카오스 스톤이란?
	dlg_menu( "@90010068", "Quest_Link_5_7b()" )
		
	-- 라크를 모으는 방법은?
	dlg_menu( "@90010070", "Quest_Link_5_7d()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()	

end

-- 부가정보 5-7d, 라크를 모으는 방법은?
function Quest_Link_5_7d()
	-- 다이얼로그 출력
	dlg_title( "@90302301" )

	dlg_text_without_quest_menu( "@90302319" )
			
	-- 라크란?
	dlg_menu( "@90010069", "Quest_Link_5_7c()" )
	
	-- 카오스 스톤이란?
	dlg_menu( "@90010068", "Quest_Link_5_7b()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()	

end


   --============================================================
   --===============		소환사 루드미유
   --============================================================
function NPC_Tutorial_CreatureSetup_init()
	cprint( "!소환사 루드미유 가동" )
	set_npc_name(  "@90302500"  )
end

function NPC_Tutorial_CreatureSetup_contact()

	-- 다이얼로그 출력
	dlg_title( "@90302501" )
	dlg_text( "@90302502" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

--========================================================================================================================
-- 수련자의 섬 리뉴얼
--========================================================================================================================

   
   --============================================================
   --===============		탈것 오르니토
   --============================================================
function NPC_Tutorial_rapter_lv1_init()
	cprint( "!탈것 오르니토 가동" )
	set_npc_name(  "@90999158"  )
end

function NPC_Tutorial_rapter_lv1_contact()

	-- 다이얼로그 출력
	dlg_title( "@90999159" )
	dlg_text( "@90999160" )

	-- 대화종료 
	dlg_menu( "@90010002", " " )
 	dlg_show()
 
end

   --============================================================
   --===============		하위 사제 루빈 
   --============================================================
function	NPC_Tutorial_Quest_anatema_init()
	cprint( "!하위 사제 루빈 가동" )
	set_npc_name(  "@90999006"  )
end
function	NPC_Tutorial_Quest_anatema_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999007" )
	dlg_text( "@90999008" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	--local quest_progress10 =  get_quest_progress(1236)

	-- 퀘스트 종료시 대사	
	--if quest_progress10 == 255 then
	--	dlg_text( "@90999011" )	
			
	-- 퀘스트 수행 전, 수행 중 대사
	--else dlg_text( "@90999008" )

	--end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


   --============================================================
   --===============		매지션 슐
   --============================================================
function	NPC_Tutorial_Quest_kenta_init()
	cprint( "!매지션 슐 가동" )
	set_npc_name(  "@90999012"  )
end
function	NPC_Tutorial_Quest_kenta_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999013" )
	dlg_text( "@90999014" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	--local quest_progress11 =  get_quest_progress(1237)

	-- 퀘스트 종료시 대사	
	--if quest_progress11 == 255 then
	--	dlg_text( "90999017" )	
			
	-- 퀘스트 수행 전, 수행 중 대사
	--else dlg_text( "@90999014" )

	--end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


   --============================================================
   --===============		사제수련생 이마퀼라트
   --============================================================
function	NPC_Tutorial_Quest_pretamming_init()
	cprint( "!사제수련생 이마퀼라트 가동" )
	set_npc_name(  "@90999018"  )
end
function	NPC_Tutorial_Quest_pretamming_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999019" )
	dlg_text( "@90999020" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	--local quest_progress11 =  get_quest_progress(1238)

	-- 퀘스트 종료시 대사	
	--if quest_progress11 == 255 then
	--	dlg_text( "90999026" )	
			
	-- 퀘스트 수행 전, 수행 중 대사
	--else dlg_text( "@90999020" )

	--end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


   --============================================================
   --===============		수련생 이시스
   --============================================================
function	NPC_Tutorial_Quest_tamming_init()
	cprint( "!수련생 이시스 가동" )
	set_npc_name(  "@90999012"  )
end
function	NPC_Tutorial_Quest_tamming_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999027" )
	dlg_text( "@90999029" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	--local quest_progress11 =  get_quest_progress(1240)

	-- 퀘스트 종료시 대사	
	--if quest_progress11 == 255 then
	--	dlg_text( "90999032" )	
			
	-- 퀘스트 수행 전, 수행 중 대사
	--else dlg_text( "@90999029" )

	--end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


   --============================================================
   --===============		선장 페도라
   --============================================================
function	NPC_Tutorial_Quest_navislamia01_init()
	cprint( "!선장 페도라 가동" )
	set_npc_name(  "@90999033"  )
end
function	NPC_Tutorial_Quest_navislamia01_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999034" )
	dlg_text( "@90999035" )

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	-- local quest_progress11 =  get_quest_progress(1241)

	-- 퀘스트 종료시 대사	
	-- if quest_progress11 == 255 then
	--	dlg_text( "90999038" )	
			
	-- 퀘스트 수행 전, 수행 중 대사
	-- else dlg_text( "@90999035" )

	-- end
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


   --============================================================
   --===============		선착장 경비병<좌>
   --============================================================
function	NPC_Tutorial_Quest_defender01_init()
	cprint( "!선착장 경비병<좌> 가동" )
	set_npc_name(  "@90999039"  )
end
function	NPC_Tutorial_Quest_defender01_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999040" )
	dlg_text( "@90999041" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


   --============================================================
   --===============		선착장 경비병<우>
   --============================================================
function	NPC_Tutorial_Quest_defender02_init()
	cprint( "!선착장 경비병<우> 가동" )
	set_npc_name(  "@90999042"  )
end
function	NPC_Tutorial_Quest_defender02_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999043" )
	dlg_text( "@90999044" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

   --============================================================
   --===============		파란 눈 소녀
   --============================================================
function	NPC_Tutorial_Quest_lamiaGirl01_init()
	cprint( "!파란 눈 소녀 가동" )
	set_npc_name(  "@90999045"  )
end
function	NPC_Tutorial_Quest_lamiaGirl01_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999046" )
	dlg_text( "@90999047" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end
   --============================================================
   --===============		갑판 경비병
   --============================================================
function	NPC_Tutorial_Quest_navislamia02_init()
	cprint( "!갑판 경비병 가동" )
	set_npc_name(  "@90999051"  )
end
function	NPC_Tutorial_Quest_navislamia02_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999052" )
	dlg_text( "@90999053" )

	-- 소녀에대한 물음
	dlg_menu( "@90999126", "blueeyegirl_link_a()" )
	
	--선착작으로 텔레포트
	dlg_menu( "@90999125", 'RunTeleport( 0 ,175249 , 60932 )' )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

-- 부가정보 1 	하위사제 아델 - 신상정보
function blueeyegirl_link_a()

	-- 다이얼로그 출력
	dlg_title( "@90999052" )

	dlg_text_without_quest_menu( "@90999127" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_Tutorial_Quest_navislamia02_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

   --============================================================
   --===============		하위 사제 조디에
   --============================================================
function	NPC_Tutorial_Quest_navislamia_priest01_init()
	cprint( "!하위 사제 조디에 가동" )
	set_npc_name(  "@90999054"  )
end
function	NPC_Tutorial_Quest_navislamia_priest01_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999055" )
	dlg_text( "@90999056" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

   --============================================================
   --===============		템플나이트 클라우스
   --============================================================
function	NPC_Tutorial_Quest_navislamia03_init()
	cprint( "!템플나이트 클라우스 가동" )
	set_npc_name(  "@90999057"  )
end
function	NPC_Tutorial_Quest_navislamia03_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999058" )
	dlg_text( "@90999059" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

   --============================================================
   --===============		하위사제 아델
   --============================================================
function	NPC_Tutorial_Quest_navislamia_priest02_init()
	cprint( "!하위사제 아델 가동" )
	set_npc_name(  "@90999060"  )
end
function	NPC_Tutorial_Quest_navislamia_priest02_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999061" )
	dlg_text( "@90999062" )

	-- 아델의 신상?
	dlg_menu( "@90999063", "lamia_talkLink__1()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

-- 부가정보 1 	하위사제 아델 - 신상정보
function lamia_talkLink__1()

	-- 다이얼로그 출력
	dlg_title( "@90999061" )

	dlg_text_without_quest_menu( "@90999064" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_Tutorial_Quest_navislamia_priest02_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


   --============================================================
   --===============		어쌔신 맥귀네스
   --============================================================
function	NPC_Tutorial_Quest_navislamia04_b1_init()
	cprint( "!어쌔신 맥귀네스 가동" )
	set_npc_name(  "@90999065"  )
end
function	NPC_Tutorial_Quest_navislamia04_b1_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999066" )
	dlg_text( "@90999067" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

   --============================================================
   --===============		템플나이트 빈센트
   --============================================================
function	NPC_Tutorial_Quest_navislamia04_init()
	cprint( "!템플나이트 빈센트 가동" )
	set_npc_name(  "@90999068"  )
end

function	NPC_Tutorial_Quest_navislamia04_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999069" )
	dlg_text( "@90999070" )

	-- 마녀의 펜던트?
	dlg_menu( "@90999071", "lamia_talkLink_2a()" )
	
	-- 현재 상황?
	dlg_menu( "@90999073", "lamia_talkLink_2b()" )
	
	-- 조합?
	dlg_menu( "@90999075", "lamia_talkLink_2c()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


-- 부가정보 2a 	템플나이트 빈센트 - 마녀의 펜던트
function lamia_talkLink_2a()

	-- 다이얼로그 출력
	dlg_title( "@90999069" )

	dlg_text_without_quest_menu( "@90999072" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_Tutorial_Quest_navislamia04_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 2b 	템플나이트 빈센트 - 현재 상황
function lamia_talkLink_2b()

	-- 다이얼로그 출력
	dlg_title( "@90999069" )

	dlg_text_without_quest_menu( "@90999074" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_Tutorial_Quest_navislamia04_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 부가정보 2c 	템플나이트 빈센트 - 조합
function lamia_talkLink_2c()

	-- 다이얼로그 출력
	dlg_title( "@90999069" )

	dlg_text_without_quest_menu( "@90999076" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_Tutorial_Quest_navislamia04_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


   --============================================================
   --===============		하위사제 그라셀
   --============================================================
function	NPC_Tutorial_Quest_navislamia_priest03_init()
	cprint( "!하위사제 그라셀 가동" )
	set_npc_name(  "@90999077"  )
end
function	NPC_Tutorial_Quest_navislamia_priest03_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999078" )
	dlg_text( "@90999079" )

	-- 조합에 대한 설명?
	dlg_menu( "@90999080", "lamia_talkLink_3()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

-- 부가정보 3 	하위사제 그라셀 - 조합
function lamia_talkLink_3()

	-- 다이얼로그 출력
	dlg_title( "@90999078" )

	dlg_text_without_quest_menu( "@90999081" )
	
	-- 돌아가기
	dlg_menu( "@90010003", "NPC_Tutorial_Quest_navislamia_priest03_contact()" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

   --============================================================
   --===============		어쌔신 힝기보탐
   --============================================================
function	NPC_Tutorial_Quest_navislamia03_b2_init()
	cprint( "!어쌔신 힝기보탐 가동" )
	set_npc_name(  "@90999082"  )
end
function	NPC_Tutorial_Quest_navislamia04_b2_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999083" )
	dlg_text( "@90999084" )

	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end

   --============================================================
   --===============		정체불명 소녀
   --============================================================
function	NPC_Tutorial_Quest_lamiaGirl02_init()
	cprint( "!정체불명 소녀 가동" )
	set_npc_name(  "@90999085"  )
end
function	NPC_Tutorial_Quest_lamiaGirl02_contact()
	-- 다이얼로그 출력
	dlg_title( "@90999086" )
	dlg_text( "@90999128" )

	-- 대화 1?
	dlg_menu( "@90999129", "lamiagirl_talkLink_1()" )
	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )

	dlg_show()
 
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1
function lamiagirl_talkLink_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999130" )
	
	-- 기본 1_1?
	dlg_menu( "@90999131", "lamiagirl_talkLink_1_1()" )
	
	-- 기본 1_2?
	dlg_menu( "@90999132", "lamiagirl_talkLink_1_2()" )
	
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_1
function lamiagirl_talkLink_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999133" )
	
	-- 기본 1_1_1?
	dlg_menu( "@90999134", "lamiagirl_talkLink_1_1_1()" )
	
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_1_1
function lamiagirl_talkLink_1_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999135" )
		
	-- 기본 1_1_1_1?
	dlg_menu( "@90999136", "lamiagirl_talkLink_1_1_1_1()" )
	
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_1_1_1
function lamiagirl_talkLink_1_1_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999137" )
		
	-- 기본 1_1_1_1_1?
	dlg_menu( "@90999138", "lamiagirl_talkLink_1_1_1_1_1()" )
	
	-- 기본 1_1_1_1_2?
	dlg_menu( "@90999140", "lamiagirl_talkLink_1_1_1_1_2()" )
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_1_1_1_1 중독 부여
function lamiagirl_talkLink_1_1_1_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999139" )
		
	-- 지속효과 부여 => add_state( 지속효과ID, 레벨, 시간 )
	add_state( 6003, 1, 6000 )
			
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_1_1_1_2
function lamiagirl_talkLink_1_1_1_1_2()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999141" )
		
	-- 기본 1_1_1_1_2_1?
	dlg_menu( "@90999142", "lamiagirl_talkLink_1_1_1_1_2_1()" )
	
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_1_1_1_2_1
function lamiagirl_talkLink_1_1_1_1_2_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999143" )
		
	-- 기본 1_1_1_1_2_1?
	dlg_menu( "@90999144", "lamiagirl_talkLink_1_1_1_1_2_1_1()" )
	
		
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_1_1_1_2_1_1
function lamiagirl_talkLink_1_1_1_1_2_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	local item_lamia_pendant_4 = find_item ( 1000072 )
	
	local item_lamia_pendant_handle = get_item_handle ( 1000072 )

   	
	if item_lamia_pendant_4 == 0 or is_erasable_item( item_lamia_pendant_handle ) == 0 then
	
	dlg_text_without_quest_menu( "@90999145" )
		
		insert_item ( 1000072, 1 )		-- 마녀의 펜던트 조각4 지급
		cprint( "@1230" )
		
		-- 대화종료
		dlg_menu( "@90010002", " " )
		
		dlg_show()
	return
	
	elseif item_lamia_pendant_4 >= 1 then				-- 조각4가 있으면 그냥 가라고 텍스트 출력
		dlg_text_without_quest_menu( "@90999104" )
	
	
		-- 대화종료 
		dlg_menu( "@90010002", " " )
 
		dlg_show()
	end

end

-- ========================================================================= 
-- 분기
-- ========================================================================= 

-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_2
function lamiagirl_talkLink_1_2()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999146" )
		
	-- 기본 1_2_1?
	dlg_menu( "@90999147", "lamiagirl_talkLink_1_2_1()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_2_1
function lamiagirl_talkLink_1_2_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999148" )
	
	-- 기본 2_2_1?
	dlg_menu( "@90999149", "lamiagirl_talkLink_1_2_1_1()" )
	
	-- 기본 2_2_2?
	dlg_menu( "@90999150", "lamiagirl_talkLink_1_2_1_2()" )
		
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_2_1_1
function lamiagirl_talkLink_1_2_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999151" )
		
	-- 기본 1_2_1_1_1?
	dlg_menu( "@90999152", "lamiagirl_talkLink_1_2_1_1_1()" )	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end

-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_2_1_1_1
function lamiagirl_talkLink_1_2_1_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999153" )
		
	-- 기본 1_2_1_1_1_1?
	dlg_menu( "@90999154", "lamiagirl_talkLink_1_2_1_1_1_1()" )	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_2_1_1_1_1
function lamiagirl_talkLink_1_2_1_1_1_1()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )
	
	dlg_text_without_quest_menu( "@90999155" )
		
	-- 기본 1_2_1_1_1_1?
	dlg_menu( "@90999144", "lamiagirl_talkLink_1_1_1_1_2_1_1()" )	
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end


-- ========================================================================= 
-- 분기
-- ========================================================================= 

-- 펜던트조각 얻기 	정체 불명 소녀 - 대사1_2_1_2
function lamiagirl_talkLink_1_2_1_2()

	-- 다이얼로그 출력
	dlg_title( "@90999086" )

	dlg_text_without_quest_menu( "@90999156" )
		
	-- 기본 1_2_1_1_1_1?
	dlg_menu( "@90999157", "lamiagirl_talkLink_1_1()" )
	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()
end


-- 나비스 라미아 진행을 위한 아이템 지금
function Quest_Link_21_1()
		
	local item_lamia_sujung_1 = find_item ( 1000077 )
	
	local item_lamia_sujung_handle = get_item_handle ( 1000077 )
   	
	if item_lamia_sujung_1 == 0 or is_erasable_item( item_lamia_sujung_handle ) == 0 then
	
		cprint( "@1231" )
		insert_item ( 1000077, 1 )		-- 고리수정 지급
		
	return
	
	elseif item_lamia_sujung_1 >= 1 then				-- 고리수정이 있다고 출력
		cprint( "@1232" )
	
	end
end

