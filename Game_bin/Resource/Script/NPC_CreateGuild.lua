-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_CreateGuild"
end

--======================================================================
-- 길드사무관 데바
--======================================================================
function NPC_CreateGuild_Deva_init()
	cprint( "!데바 길드사무관 가동" )
	set_npc_name(  "@90101200"  )
end
 

function NPC_CreateGuild_Deva_contact()

	local guild_create_level = get_env("game.guild_create_level")
	dlg_title("@90101201")
	dlg_text("@90101202\v#@level@#\v"..guild_create_level)

	dlg_menu("@90010061", "create_guild_Deva()")
	dlg_menu("@90010152", "guild_alliance()")
	
	dlg_menu( "@90010002", "" )

	dlg_show()
end


function create_guild_Deva()
	dlg_title("@90101201")
	
	local guild_create_level = get_env("game.guild_create_level")
	local guild_create_gold = get_env("game.guild_create_gold")
	
	if get_value( "level" ) < guild_create_level then -- 레벨 20 미만일 때 레벨 부족하다는 대사
		dlg_text("@90101203\v#@level@#\v"..guild_create_level)
	elseif get_value( "guild_id" ) > 0 then -- 가입된 길드가 있을 때
		dlg_text("@90101204")
	else -- 레벨 20이상이고, 가입된 길드도 없을 때 소모되는 비용에 대한 안내 대사
		dlg_text( "@90101205\v#@gold@#\v"..guild_create_gold )
		dlg_menu("@90010061", "show_guild_create()")
	end

	dlg_menu( "@90010002", "" )

	dlg_show()
end


--======================================================================
-- 길드사무관 아수라
--======================================================================
function NPC_CreateGuild_Asura_init()
	cprint( "!아수라 길드사무관 가동" )
	set_npc_name(  "@90201200"  )
end

function NPC_CreateGuild_Asura_contact()

	local guild_create_level = get_env("game.guild_create_level")
	dlg_title("@90201201")
	dlg_text("@90201202\v#@level@#\v"..guild_create_level)

	dlg_menu("@90010061", "create_guild_Asura()")
	dlg_menu("@90010152", "guild_alliance()")
	dlg_menu( "@90010002", "" )
	dlg_show()
end

function create_guild_Asura()
	dlg_title("@90201201")
	
	local guild_create_level = get_env("game.guild_create_level")
	local guild_create_gold = get_env("game.guild_create_gold")
	
	if get_value( "level" ) < guild_create_level then -- 레벨 20 미만일 때
		dlg_text("@90201203\v#@level@#\v"..guild_create_level)
	elseif get_value( "guild_id" ) > 0 then -- 가입된 길드가 있을 때
		dlg_text("@90201204")	
	else -- 그 외(레벨 20이상이고, 가입된 길드도 없을 때)
		dlg_text( "@90201205\v#@gold@#\v"..guild_create_gold )
		dlg_menu("@90010061", "show_guild_create()")
	end

	dlg_menu( "@90010002", "" )
	dlg_show()
end


--======================================================================
-- 길드사무관 가이아
--======================================================================
function NPC_CreateGuild_Gaia_init()
	cprint( "!가이아 길드사무관 가동" )
	set_npc_name(  "@90401200"  )
end

function NPC_CreateGuild_Gaia_contact()
	local guild_create_level = get_env("game.guild_create_level")
	dlg_title("@90401201")
	dlg_text("@90401202\v#@level@#\v"..guild_create_level)

	dlg_menu("@90010061", "create_guild_Gaia()")
	dlg_menu("@90010152", "guild_alliance()")
	dlg_menu( "@90010002", "" )
	dlg_show()
end

function create_guild_Gaia()
	dlg_title("@90401201")
	
	local guild_create_level = get_env("game.guild_create_level")
	local guild_create_gold = get_env("game.guild_create_gold")
	
	if get_value( "level" ) < guild_create_level then -- 레벨 20 미만일 때
		dlg_text("@90401203\v#@level@#\v"..guild_create_level)
	elseif get_value( "guild_id" ) > 0 then -- 가입된 길드가 있을 때
		dlg_text("@90401204")
	else -- 그 외(레벨 20이상이고, 가입된 길드도 없을 때)
		dlg_text( "@90401205\v#@gold@#\v"..guild_create_gold )
		dlg_menu("@90010061", "show_guild_create()")
	end

	dlg_menu( "@90010002", "" )

	dlg_show()
end


--======================================================================
-- 길드사무관 론도
--======================================================================

 -- 길드 사무관 레샤
function NPC_CreateGuild_Rondoh_contact()

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress1 =  get_quest_progress( 2000 )	-- 쥬시 찾아가기
	local quest_progress100 =  get_quest_progress( 2008 ) -- 광신도 암살자: 론도
	
	-- 메인 퀘스트 장면 0 완료, 장면 1 조건 충족(레벨 70이상)
	--if quest_progress100 == 255 and get_value("level") > 70 then
	
	local guild_create_level = get_env("game.guild_create_level")
	local guild_create_gold = get_env("game.guild_create_gold")
	
	dlg_title("@90601201")
	dlg_text("@90601202\v#@level@#\v"..guild_create_level)

	dlg_menu("@90010061", "create_guild_Rondoh()")
	dlg_menu("@90010152", "guild_alliance()")
	
	
	if  get_value("level") < 50 then
			
			dlg_menu("@90999670", "mainquest_limitlevel50()")
				
	elseif quest_progress1 == 255 and get_value("level") < 70 then
		
			dlg_menu("@90999842", "mainquest_limitlevel70()")
		
	elseif  quest_progress1 == 0 and get_value("level") >= 50 then
		
			dlg_menu("@90999670", "mainquest_findjuicy()")
	end
	
	dlg_menu( "@90010002", "" )
	dlg_show()
		
		
end

-- 메인 퀘스트 미 시작, 미 충족(레벨 50 이하)
function mainquest_limitlevel50()
	
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress1 =  get_quest_progress( 2000 )
	
	-- 메인 퀘스트 장면 0 완료, 장면 1 조건 미 충족(레벨 50 이하)
	--if  quest_progress1 == -1 and get_value("level") < 50 then
	
	dlg_title("@90101201")
	dlg_text("@90999866")
		
	cprint( "@90999867" ) -- <#6DD66D>Lv50이상으로 성장한 후에 다시 도전하세요.
	
	--end
		
	dlg_menu( "@90010002", "" )
	dlg_show()
	
end


-- 메인 퀘스트 장면 0 완료, 장면 1 조건 미 충족(레벨 70 이하)
function mainquest_limitlevel70()
	
	dlg_title("@90601201")
	dlg_text("@90999872")
		
	cprint( "@90999873" ) -- <#6DD66D>Lv70이상으로 성장한 후에 다시 도전하세요.
		
	dlg_menu( "@90010002", "" )
	dlg_show()
	
end

-- 당신도 '예레모리'를 찾고 있나요? 모두 다 부질없는 짓이에요. 그냥 헛 소문이랍니다.
function mainquest_findjuicy()
				
	dlg_title("@90101201")
	dlg_text("@90999671")
	
	dlg_menu("@90999672", "mainquest_findjuicy_1()")
	
	dlg_menu("@90999674", "mainquest_findjuicy_2()")
	
	dlg_menu( "@90010002", "" )
	dlg_show()
	
end

-- 또다시 물어본다.
function mainquest_findjuicy_1()
		
	dlg_title("@90101201")
		
	if get_value( "level" ) < 50 then
		dlg_text("@90999866")
	
		cprint( "@90999867" ) -- <#6DD66D>Lv50이상으로 성장한 후에 다시 도전하세요.
		dlg_menu( "@90010002", "" )
		dlg_show()
	
	else
		dlg_text("@90999675")
		dlg_menu("@90999673", "mainquest_findjuicy_1_1()")
		
		dlg_menu( "@90010002", "" )
		dlg_show()
	
	end
		
end

function mainquest_findjuicy_2()
	dlg_title("@90101201")
	dlg_text("@90999677")
	
	dlg_menu( "@90010002", "" )
	dlg_show()
		
end


function mainquest_findjuicy_1_1()
	dlg_title("@90101201")
	dlg_text("@90999676")
	
	dlg_menu("@90999678", "mainquest_findjuicy_1_1_1()")
	
	dlg_menu( "@90010002", "" )
	dlg_show()
	
end

function mainquest_findjuicy_1_1_1()
	
	
	show_quest_info_without_npc( 2000 )
	
			
end
	


function create_guild_Rondoh()
	dlg_title("@90601201")
	
	local guild_create_level = get_env("game.guild_create_level")
	local guild_create_gold = get_env("game.guild_create_gold")

	
	if get_value( "level" ) < 20 then -- 레벨 20 미만일 때
		dlg_text("@90601203\v#@level@#\v"..guild_create_level)
	elseif get_value( "guild_id" ) > 0 then -- 가입된 길드가 있을 때
		dlg_text("@90601204")	
	else -- 그 외(레벨 20이상이고, 가입된 길드도 없을 때)
		dlg_text( "@90601205\v#@gold@#\v"..guild_create_gold )
		dlg_menu("@90010061", "show_guild_create()")
	end

	dlg_menu( "@90010002", "" )

	dlg_show()
end

--======================================================================
-- 길드사무관 도시 유적
--======================================================================
function NPC_CreateGuild_Ancient_relic_init()
	cprint( "!도시유적 길드사무관 가동" )
	set_npc_name(  "@90703700"  )
end

function NPC_CreateGuild_Ancient_relic_contact()
	local guild_create_level = get_env("game.guild_create_level")
	dlg_title("@90703701")
	dlg_text("@90703702\v#@level@#\v"..guild_create_level)

	dlg_menu("@90010061", "create_guild_Ancient_relic()")
	dlg_menu("@90010152", "guild_alliance()")
	dlg_menu( "@90010002", "" )
	dlg_show()
end

function create_guild_Ancient_relic()
	dlg_title("@90703701")
	
	local guild_create_level = get_env("game.guild_create_level")
	local guild_create_gold = get_env("game.guild_create_gold")

	
	if get_value( "level" ) < 20 then -- 레벨 20 미만일 때
		dlg_text("@90703703\v#@level@#\v"..guild_create_level)
	elseif get_value( "guild_id" ) > 0 then -- 가입된 길드가 있을 때
		dlg_text("@90703704")	
	else -- 그 외(레벨 20이상이고, 가입된 길드도 없을 때)
		dlg_text( "@90703705\v#@gold@#\v"..guild_create_gold )
		dlg_menu("@90010061", "show_guild_create()")
	end

	dlg_menu( "@90010002", "" )
	dlg_show()
end


--======================================================================
-- 모든 종족 공통함수
function on_create_guild( guild_name )

	local temp_text
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title( npc_name) 
	
	-- <대사 출력> 각 NPC에 따라 적합한
		
	-- 가이아 일 때 (4012 길드 사무관 오벨리)		
	if npc_id == 4012 then
	temp_text =sconv("@90401207", "#@createguild_name@#",guild_name)
	dlg_text(temp_text)

	-- 데바 일 때 (1012 길드 사무관 세스)
	elseif npc_id == 1012 then
	temp_text =sconv("@90101207", "#@createguild_name@#",guild_name)
	dlg_text(temp_text)

	-- 아수라 일 때 (2012 길드 사무관 레비아탄)
	elseif npc_id == 2012 then
	temp_text =sconv("@90201207", "#@createguild_name@#",guild_name)
	dlg_text(temp_text)
	
	-- 론도 일 때 (6012 길드 사무관 레샤)
	elseif npc_id == 6012 then
	temp_text =sconv("@90601207", "#@createguild_name@#",guild_name)
	dlg_text(temp_text)
	
	-- 론도 일 때 (7037 길드 사무관 텐만)
	elseif npc_id == 7037 then
	temp_text =sconv("@90703707", "#@createguild_name@#",guild_name)
	dlg_text(temp_text)	
	end

	-- 길드명 변경
	dlg_menu("@90010063", "show_guild_create()")
	
	-- 확인
	dlg_menu("@90010016", "create_guild_main( '" .. guild_name .. "' )")

	-- 대화 종료
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end

function create_guild_main( guild_name )
	
	local guild_creation_state, temp_text, cost
	local gold = get_value( "gold" )
	local check_guild_name = check_valid_guild_name( guild_name ) 
	
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	
	
	--static const GUILD_CREATE_SUCCESS                    = 0;    // 성공

	--static const GUILD_CREATE_ALREADY_IN_ANOTHER_GUILD   = 1;    // 이미 다른 길드 가입중

	--static const GUILD_CREATE_INVALID_GUILD_NAME         = 2;    // 길드 이름 잘못됨

	--static const GUILD_CREATE_ALREADY_EXIST_NAME         = 3;    // 이미 있는 길드 이름

	--static const GUILD_CREATE_INVALID_ARGUMENT           = 4;    // 잘못된 인수(길드 이름을 집어넣지 않았다던가. 등등)

	--static const GUILD_CREATE_GUILD_BLOCK_TIME_LIMIT     = 5;    // 길드, 연합 탈퇴 후 7일 이전에 길드 결성 시
	
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	
	-- 길드 결성 비용은 100000000 RP
	local guild_create_gold = get_env("game.guild_create_gold")
				
	--현재 소지 금액이 길드 결성비용보다 적을 경우.
	if gold < guild_create_gold then
	
	    dlg_title( npc_name)
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text( "@90401206\v#@gold@#\v"..guild_create_gold )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text( "@90101206\v#@gold@#\v"..guild_create_gold )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text( "@90201206\v#@gold@#\v"..guild_create_gold )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text( "@90601206\v#@gold@#\v"..guild_create_gold )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text( "@90703706\v#@gold@#\v"..guild_create_gold )
		end
		
		dlg_menu( "@90010002", "" )
		dlg_show()		
		
	--소지금이 충분하면 길드 결성.
	-- 길드 만들기	
	else 
		guild_creation_state = create_guild(guild_name)

		-- 길드가 제대로 만들어 졌다면 소지금을 감소 시킨다.
		if guild_creation_state == 0 then
			set_value( "gold", gold - guild_create_gold )
			update_gold_chaos()
			temp_text = sconv("@90019002", "#@cost@#", tostring(guild_create_gold) )
			message(temp_text)
			return
		
		-- 잘못된 길드명을 사용한 경우
		else
			dlg_title( npc_name)
			if guild_creation_state == 2 or guild_creation_state == 4 then
			
			-- <대사 출력> 각 NPC에 따라 적합한 
			
				-- 가이아 일 때 (4012 길드 사무관 오벨리)		
				if npc_id == 4012 then
					dlg_text( "@90401228" )
				-- 데바 일 때 (1012 길드 사무관 세스)
				elseif npc_id == 1012 then
					dlg_text( "@90101212" )
				-- 아수라 일 때 (2012 길드 사무관 레비아탄)
				elseif npc_id == 2012 then		
					dlg_text( "@90201219" )
				-- 론도 일 때 (6012 길드 사무관 레샤)
				elseif npc_id == 6012 then		
					dlg_text( "@90601218" )
				-- 도시 유적 일 때 (7037 길드 사무관 레샤)
				elseif npc_id == 7037 then		
					dlg_text( "@90601218" )
					
				end
				dlg_menu("@90010063", "show_guild_create()")				
						
			-- 중복되는 길드명을 사용한 경우
			elseif guild_creation_state == 3 then
			-- <대사 출력> 각 NPC에 따라 적합한 
			
				-- 가이아 일 때 (4012 길드 사무관 오벨리)		
				if npc_id == 4012 then
					dlg_text( "@90401211" )
				-- 데바 일 때 (1012 길드 사무관 세스)
				elseif npc_id == 1012 then
					dlg_text( "@90101211" )
				-- 아수라 일 때 (2012 길드 사무관 레비아탄)
				elseif npc_id == 2012 then		
					dlg_text( "@90201211" )
				-- 론도 일 때 (6012 길드 사무관 레샤)
				elseif npc_id == 6012 then		
					dlg_text( "@90601211" )
				-- 도시 유적 일 때 (7037 길드 사무관 레샤)
				elseif npc_id == 7037 then		
					dlg_text( "@90703708" )
					
				end
				dlg_menu("@90010063", "show_guild_create()")
				
			-- 길드 및 연합 탈퇴 및 해산 후 7일 이전에 길드를 결성할 경우
			elseif guild_creation_state == 5 then
			-- <대사 출력> 각 NPC에 따라 적합한 
				-- 가이아 일 때 (4012 길드 사무관 오벨리)		
				if npc_id == 4012 then
					dlg_text( "@90101230" )
				-- 데바 일 때 (1012 길드 사무관 세스)
				elseif npc_id == 1012 then
					dlg_text( "@90101230" )
				-- 아수라 일 때 (2012 길드 사무관 레비아탄)
				elseif npc_id == 2012 then		
					dlg_text( "@90101230" )
				-- 론도 일 때 (6012 길드 사무관 레샤)
				elseif npc_id == 6012 then		
					dlg_text( "@90101230" )
				-- 도시 유적 일 때 (7037 길드 사무관 레샤)
				elseif npc_id == 7037 then		
					dlg_text( "@90101230" )
				end					
			end
			dlg_menu( "@90010002", "" )
			dlg_show()	
		end
	end	
end




















































































ALLIANCE_CREATE_SUCCESS                       = 0    -- 성공
ALLIANCE_CREATE_ALREADY_IN_ANOTHER_ALLIANCE   = 1    -- 이미 다른 연합 소속
ALLIANCE_CREATE_INVALID_ALLIANCE_NAME         = 2    -- 잘못된 연합 이름
ALLIANCE_CREATE_ALREADY_EXIST_NAME            = 3    -- 이미 존재하는 연합 이름
ALLIANCE_CREATE_INVALID_ARGUMENT              = 4    -- 잘못된 인자
ALLIANCE_CREATE_NOT_GUILD_LEADER              = 5    -- 길드장이 아님
ALLIANCE_CREATE_HAS_RAID_STARTED              = 6    -- 레이드 신청 했음
ALLIANCE_CREATE_ALLIANCE_BLOCK_TIME_LIMIT     = 7    -- 연합 해산, 탈퇴 후 7일 이전에 연합 결성 시도 시


function guild_alliance()

	local guild_leader = is_guild_leader()
	local npc_id = get_npc_id()
	local alliance_leader = is_alliance_leader()
	
	if is_alliance_leader() == 1 then
		
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101214" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101214" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101214" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101214" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101214" )
		end
		
	dlg_menu("@90010155", "question_guild_alliance()")
	dlg_menu("@90010232", "show_destroy_alliance()")
	dlg_menu("@90010154", "add_guild_alliance()")
	

	elseif is_alliance_leader() == 0 and guild_leader == 1 then
	
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101214" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101214" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101214" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101214" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101214" )
		end
		
	dlg_menu("@90010155", "question_guild_alliance()")
	dlg_menu("@90010153", "show_alliance_create()")
	dlg_menu("@90010154", "add_guild_alliance()")

	elseif guild_leader == 0 then

		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101225" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101225" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101225" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101225" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101225" )
		end

	dlg_menu("@90010155", "question_guild_alliance()")
	end
		
	dlg_menu( "@90010002", "" )
	dlg_show()
	
end	



-- 질문. 길드 연합이란?
function question_guild_alliance()
	local npc_id = get_npc_id()
		-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101213" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101213" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101213" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101213" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101213" )
		end

	--돌아가기
	dlg_menu( "@90010003", 'guild_alliance()' )	
	dlg_menu( "@90010002", "" )
	dlg_show()
	
end	

function on_create_alliance( alliance_name )
	
	local check_alliance_name = check_valid_alliance_name( alliance_name )
	local npc_id = get_npc_id()
		
	if check_alliance_name == 2 or check_alliance_name == 4 then

		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101219" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101219" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101219" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101219" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101219" )
		end
	elseif check_alliance_name == 3 then
	-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101220" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101220" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101220" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101220" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101220" )
		end

	elseif check_alliance_name == 0 then
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")

		end
	
	-- <대사 출력> 각 NPC에 따라 적합한
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
	   	temp_text =sconv("@90101218", "#@createalliance_name@#",alliance_name)
    	dlg_text_without_quest_menu(temp_text)
	
    	-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
		temp_text =sconv("@90101218", "#@createalliance_name@#",alliance_name)
		dlg_text_without_quest_menu(temp_text)
	
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
		temp_text =sconv("@90101218", "#@createalliance_name@#",alliance_name)
    	dlg_text_without_quest_menu(temp_text)
    	
    	-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
		temp_text =sconv("@90101218", "#@createalliance_name@#",alliance_name)
    	dlg_text_without_quest_menu(temp_text)
    	
    	-- 론도 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
		temp_text =sconv("@90101218", "#@createalliance_name@#",alliance_name)
    	dlg_text_without_quest_menu(temp_text)
		end
	
	dlg_menu("@90010016", "createalliance( '".. alliance_name .."' )")
	dlg_menu("@90010001", "guild_alliance()")
	end	
	
	dlg_menu( "@90010002", "" )
	dlg_show()	
end
	
	
	
	
function createalliance( alliance_name )
	
	local check_create_alliance = create_alliance( alliance_name )
	local npc_id = get_npc_id()

		
	if 	check_create_alliance == 1 then
	
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101222" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101222" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101222" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101222" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101222" )
		end
		
	elseif check_create_alliance == 2 or check_create_alliance == 4 then

		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101219" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101219" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101219" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101219" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101219" )
		end

	elseif check_create_alliance == 3 then
	-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101220" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101220" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101220" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101220" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101220" )
		end


	elseif check_create_alliance == 5 then
	-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101223" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101223" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101223" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101223" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101223" )
		end

	elseif check_create_alliance == 6 then
	-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101215" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101215" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101215" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101215" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101215" )
		end
	elseif check_create_alliance == 7 then
	-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101229" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101229" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101229" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101229" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101229" )
		end	
	elseif check_create_alliance == 0 then
		
	-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
	   	temp_text =sconv("@90101221", "#@createalliance_name@#",alliance_name)
    	dlg_text_without_quest_menu(temp_text)
	
    	-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
		temp_text =sconv("@90101221", "#@createalliance_name@#",alliance_name)
		dlg_text_without_quest_menu(temp_text)
	
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
		temp_text =sconv("@90101221", "#@createalliance_name@#",alliance_name)
    	dlg_text_without_quest_menu(temp_text)
    	
    	-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
		temp_text =sconv("@90101221", "#@createalliance_name@#",alliance_name)
    	dlg_text_without_quest_menu(temp_text)
    	
    	-- 론도 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
		temp_text =sconv("@90101221", "#@createalliance_name@#",alliance_name)
    	dlg_text_without_quest_menu(temp_text)
		end
	end
	
	dlg_menu( "@90010003", "guild_alliance()" )
	dlg_menu( "@90010002", "" )
	dlg_show()	
end




-- 길드 추가

function add_guild_alliance()


	local max_alliance_member = get_max_alliance_member_count()
	local alliance_leader = is_alliance_leader()
	local npc_id = get_npc_id()


	
	if alliance_leader == 1 then
		
		-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)
		if npc_id == 4012 then
			temp_text =sconv( "@90101217" , "#@add_alliance@#", max_alliance_member)
			dlg_text_without_quest_menu(temp_text)
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	temp_text =sconv( "@90101217" , "#@add_alliance@#", max_alliance_member)
        	dlg_text_without_quest_menu(temp_text)
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	temp_text =sconv( "@90101217" , "#@add_alliance@#", max_alliance_member)
        	dlg_text_without_quest_menu(temp_text)
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	temp_text =sconv( "@90101217" , "#@add_alliance@#", max_alliance_member)
        	dlg_text_without_quest_menu(temp_text)
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	temp_text =sconv( "@90101217" , "#@add_alliance@#", max_alliance_member)
        	dlg_text_without_quest_menu(temp_text)
		end

	dlg_menu("@90010016", "pay_add_alliance()")
	dlg_menu("@90010001", "guild_alliance()")
	
	elseif alliance_leader == 0 then
		-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101226" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101226" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101226" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101226" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101226" )
		end
	
	dlg_menu( "@90010003", "guild_alliance()" )
	end
		
	dlg_menu( "@90010002", "" )
	dlg_show()
	
end	








function pay_add_alliance()

	local max_alliance_member = get_max_alliance_member_count()
	local gold = get_value( "gold" )
	local cost = 1000000
	local npc_id = get_npc_id()
		
	if max_alliance_member >= 5 then
		-- <이름 출력> 각 NPC에 따라 적합한 
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101227" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90101227" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90101227" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90101227" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90101227" )
		end
	elseif max_alliance_member == 1 or max_alliance_member == 2 or max_alliance_member == 3 or max_alliance_member == 4 then
		if cost > gold then
			-- <이름 출력> 각 NPC에 따라 적합한 
			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
			if npc_id == 4012 then
				dlg_title("@90401201")
			-- 데바 일 때 (1012 길드 사무관 세스)
			elseif npc_id == 1012 then
				dlg_title("@90101201")
			-- 아수라 일 때 (2012 길드 사무관 레비아탄)
			elseif npc_id == 2012 then
				dlg_title("@90201201")
			-- 론도 일 때 (6012 길드 사무관 레샤)
			elseif npc_id == 6012 then
				dlg_title("@90601201")
			-- 도시 유적 일 때 (7037 길드 사무관 텐만)
			elseif npc_id == 7037 then
				dlg_title("@90703701")
			
			end
		
		    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
			if npc_id == 4012 then
				dlg_text_without_quest_menu( "@90101224" )
			-- 데바 일 때 (1012 길드 사무관 세스)
			elseif npc_id == 1012 then
	        	dlg_text_without_quest_menu( "@90101224" )
			-- 아수라 일 때 (2012 길드 사무관 레비아탄)
			elseif npc_id == 2012 then
	        	dlg_text_without_quest_menu( "@90101224" )
	        -- 론도 일 때 (6012 길드 사무관 레샤)
			elseif npc_id == 6012 then
	        	dlg_text_without_quest_menu( "@90101224" )
	        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
			elseif npc_id == 7037 then
	        	dlg_text_without_quest_menu( "@90101224" )
			end
		
		elseif cost <= gold then
		
			-- <이름 출력> 각 NPC에 따라 적합한 
			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
			if npc_id == 4012 then
				dlg_title("@90401201")
			-- 데바 일 때 (1012 길드 사무관 세스)
			elseif npc_id == 1012 then
				dlg_title("@90101201")
			-- 아수라 일 때 (2012 길드 사무관 레비아탄)
			elseif npc_id == 2012 then
				dlg_title("@90201201")
			-- 론도 일 때 (6012 길드 사무관 레샤)
			elseif npc_id == 6012 then
				dlg_title("@90601201")
			-- 도시 유적 일 때 (7037 길드 사무관 텐만)
			elseif npc_id == 7037 then
				dlg_title("@90703701")
			
			end
		
		    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
			if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90101228" )
			-- 데바 일 때 (1012 길드 사무관 세스)
			elseif npc_id == 1012 then
       	 	dlg_text_without_quest_menu( "@90101228" )
			-- 아수라 일 때 (2012 길드 사무관 레비아탄)
			elseif npc_id == 2012 then
       	 	dlg_text_without_quest_menu( "@90101228" )
    	   	 -- 론도 일 때 (6012 길드 사무관 레샤)
			elseif npc_id == 6012 then
       	 	dlg_text_without_quest_menu( "@90101228" )
    	   	 -- 도시 유적 일 때 (7037 길드 사무관 레샤)
			elseif npc_id == 7037 then
       	 	dlg_text_without_quest_menu( "@90101228" )
			end	

		set_value( "gold", gold - cost )
		update_gold_chaos()
		increase_max_alliance_member_count()
				
		end
	
	end
	
	dlg_menu( "@90010002", "" )
	dlg_show()	
end





function show_destroy_alliance()


	local guild_leader = is_guild_leader()
	local npc_id = get_npc_id()
	local alliance_leader = is_alliance_leader()
		
		-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010233" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010233" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010233" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010233" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010233" )
		end	
				
	dlg_menu("@90010016", "on_destroy_alliance()")
	dlg_menu("@90010001", "guild_alliance()")
end	
	
	
	
function on_destroy_alliance()

	local state_code = destroy_alliance()
	local npc_id = get_npc_id()
	
	--ALLIANCE_DESTROY_SUCCESS                         = 0;    // 연합 해산이 성공적으로 이루어 졌습니다.
	--ALLIANCE_DESTROY_NOT_IN_GUILD                    = 1;    // 해산을 시도하는 유저는 어떠한 길드에도 소속되어 있지 않습니다.	
	--ALLIANCE_DESTROY_NOT_IN_ALLIANCE                 = 2;    // 해산을 시도하는 유저의 길드는 어떠한 길드 연합에도 소속되어 있지 않습니다.
	--ALLIANCE_DESTROY_INVALID_ARGUMENT                = 3;    // 해산 명령어에 파라미터가 잘못 주어졌습니다. 파라미터는 반드시 문자열타입으로 연합 이름을 의미해야 합니다.
	--ALLIANCE_DESTROY_NOT_GUILD_LEADER                = 4;    // 해산을 시도하는 유저는 소속 길드의 길드장이 아닙니다.
	--ALLIANCE_DESTROY_NOT_ALLIANCE_LEADER             = 5;    // 해산을 시도하는 유저는 소속 길드 연합의 마스터 길드 소속이 아닙니다.	
	--ALLIANCE_DESTROY_OTHER_MEMBER_GUILD_EXIST        = 6;    // 해산을 시도하는 길드 연합에 마스터 길드 이외의 멤버 길드가 존재합니다.
	--ALLIANCE_DESTROY_DUNGEON_OWNER                   = 7;    // 해산을 시도하는 길드 연합이 던전을 소유 중입니다.
	--ALLIANCE_DESTROY_DUNGEON_RAID_OR_SIEGE_REQUESTED = 8;    // 해산을 시도하는 길드 연합이 던전 시즈 또는 레이드 신청 상태(참여 중)입니다.
	--ALLIANCE_DESTROY_UNKNOWN                         = 99;   // 위에 나열된 이유 외에 알 수 없는 이유로 인하여 해산이 실패하였습니다.
	
	if state_code == 0 then
	
			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010234" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010234" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010234" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010234" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010234" )
		end	
		
	dlg_menu( "@90010002", "" )
	dlg_show()	

	elseif state_code == 1 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010235" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010235" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010235" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010235" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010235" )
		end			
		
	dlg_menu( "@90010002", "" )
	dlg_show()	

	elseif state_code == 2 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010235" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010235" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010235" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010235" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010235" )
		end	
		
	dlg_menu( "@90010002", "" )
	dlg_show()	

	elseif state_code == 3 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010236" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010236" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010236" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010236" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010236" )
		end			
	
	dlg_menu( "@90010002", "" )
	dlg_show()	
	
	elseif state_code == 4 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010237" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010237" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010237" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010237" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010237" )
		end		
		
	dlg_menu( "@90010002", "" )
	dlg_show()	

	elseif state_code == 5 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010237" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010237" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010237" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010237" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010237" )
		end			
		
	dlg_menu( "@90010002", "" )
	dlg_show()	

	elseif state_code == 6 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010238" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010238" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010238" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010238" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010238" )
		end	
		
	dlg_menu( "@90010002", "" )
	dlg_show()	

	elseif state_code == 7 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010239" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010239" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010239" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010239" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010239" )
		end		
		
	dlg_menu( "@90010002", "" )
	dlg_show()	

	elseif state_code == 8 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010236" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010236" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010236" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010236" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010236" )
		end		
		
	dlg_menu( "@90010002", "" )
	dlg_show()	

	elseif state_code == 99 then

			-- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_title("@90401201")
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
			dlg_title("@90101201")
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
			dlg_title("@90201201")
		-- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
			dlg_title("@90601201")
		-- 도시 유적 일 때 (7037 길드 사무관 텐만)
		elseif npc_id == 7037 then
			dlg_title("@90703701")
		end
	
	    -- 가이아 일 때 (4012 길드 사무관 오벨리)		
		if npc_id == 4012 then
			dlg_text_without_quest_menu( "@90010240" )
		-- 데바 일 때 (1012 길드 사무관 세스)
		elseif npc_id == 1012 then
        	dlg_text_without_quest_menu( "@90010240" )
		-- 아수라 일 때 (2012 길드 사무관 레비아탄)
		elseif npc_id == 2012 then
        	dlg_text_without_quest_menu( "@90010240" )
        -- 론도 일 때 (6012 길드 사무관 레샤)
		elseif npc_id == 6012 then
        	dlg_text_without_quest_menu( "@90010240" )
        -- 도시 유적 일 때 (7037 길드 사무관 레샤)
		elseif npc_id == 7037 then
        	dlg_text_without_quest_menu( "@90010240" )
		end	
	end						
	dlg_menu( "@90010002", "" )
	dlg_show()	
 
end


