-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_DarkMarket"
end
 
 -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.

function NPC_DarkMarket_contact()
 
	open_market( 'dark' )
	
 end

-- on_use_item은 
-- ID : 아이템 코드
-- p_handle : 사용자 핸들
-- target : 대상 타입
--			0 : 플레이어
--			1 : 소환수
--			2 : 몬스터
--			3 : NPC
--			4 : 알 수 없음
-- t_target : 대상 핸들
-- 대상의 코드값(소환수 코드/몬스터 코드/NPC 코드)
-- 소유자
-- 		대상이 소환수일 경우에만 자신의 소환수 : 1, 타인의 소환수 : 0

function on_use_item( ID, p_handle, target, t_handle, target_id, target_creature )
	on_use_item_by_design_team( ID, p_handle, target, t_handle, target_id, target_creature )
	on_use_item_by_live_team( ID, p_handle, target, t_handle, target_id, target_creature )
	on_use_item_by_program_team( ID, p_handle, target, t_handle, target_id, target_creature )
end

function on_use_item_by_design_team( ID, p_handle, target, t_handle, target_id, target_creature )

	--local item_secroute_count = find_item ( 609002 )
	--local item_secroute2_count = find_item ( 609003 )
	
	--if item_secroute_count == 1 or item_secroute2_count == 1 then
	
		if ID == 608410 then
			open_storage()
		
		elseif ID == 608411 then
			--open_market( 'beginner_equip' )
			show_auction_window()
			
		elseif ID == 608412 then
			open_market( 'boost_chip' )
			
		elseif ID == 601100231 or ID == 2012860 or ID == 2013010 then
			add_state( 314017,  70, 180000 ) -- 스피드 오브 세이지 		 7레벨
			add_state(   2506,  41, 180000 ) -- 다크 마이트 			20레벨
			add_state(   2508,  41, 180000 ) -- 다크 마이트 			20레벨
			add_state( 163429,  10, 180000 ) -- 윈드웨폰 				10레벨
			add_state(  13424, 277, 180000 ) -- 바위의 정기 			16레벨
			add_state(  13423,  36, 180000 ) -- 바람의 응원 			16레벨
			add_state(  13425, 277, 180000 ) -- 성화의 기운 			16레벨
			add_state( 314018,  70, 180000 ) -- 정신통일: 일격의 정기 	 7레벨
			add_state( 163404,  48, 180000 ) -- 블레싱 오브 바이탈리티 	16레벨
			add_state( 163405,  48, 180000 ) -- 블레싱 오브 인텔리전스 	16레벨
			add_state( 163406,  48, 180000 ) -- 블레싱 오브 멘탈 		16레벨
			add_state( 163407,  48, 180000 ) -- 블레싱 오브 스트렝스 	16레벨
			add_state( 163433,  90, 180000 ) -- 샤이닝 아머 			20레벨
			add_state(   2505, 160, 180000 ) -- 샤이닝 웨폰 			20레벨
			add_state(   2507, 160, 180000 ) -- 샤이닝 웨폰 			20레벨
			add_state( 314016,  70, 180000 ) -- 자연과 힘의 조화 		 7레벨
			
		elseif ID == 2016026 then -- <(version:9.2)>패러렐 월드 퀘스트용 티켓 사용 시 - [일일]틈새조사 : 타임어택
			
			--local quest_progress4009 = get_quest_progress(4009)
			--local count_2 = find_item( 2016026 ) -- 임시 허가증
			--if quest_progress4009 == 0 or quest_progress4009 == "" or quest_progress4009 == nil then
			
			if get_quest_progress(4009) == 0 or get_quest_progress(4009) == "" or get_quest_progress(4009) == nil then
				--show_quest_info_without_npc( 4009 )
				dlg_special( 'confirm_window', 'parallelworld_warp( 70000, x, y )', '@9907' )
			elseif get_quest_progress(4009) == 255 then
				dlg_special( 'confirm_window', 'parallelworld_warp( 70000, x, y )', '@9907' )
			else
				private_notice( "@90610020" ) -- 퀘스트 상태를 확인 후 다시 시도해주세요.
			end
			
		-- 메인 퀘스트 파트2_집착 
		-- 아이템_고대 크리스탈 
		elseif ID == 1000201 then
		
			-- 퀘스트 상태 체크 	get_quest_progress(ID)  
			-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
			local quest_progress_3210 =get_quest_progress(3210) -- <(version:7.2)>[집착]결계해지 #2
			local quest_progress_3211 =get_quest_progress(3211) -- <(version:7.2)>[집착]아공간 진입
			local quest_progress_3212 =get_quest_progress(3212) -- <(version:7.2)>[집착]주변 탐색
			local quest_progress_3213 =get_quest_progress(3213) -- <(version:7.2)>[집착]마녀정보 탐사 #1
			local quest_progress_3214 =get_quest_progress(3214) -- <(version:7.2)>[집착]마녀정보 탐사 #2
			local quest_progress_3215 =get_quest_progress(3215) -- <(version:7.2)>[집착]마녀정보 탐사 #3
			local quest_progress_3216 =get_quest_progress(3216) -- <(version:7.2)>[집착]목격자 #1
			local quest_progress_3217 =get_quest_progress(3217) -- <(version:7.2)>[집착]목격자 #2
			local quest_progress_3218 =get_quest_progress(3218) -- <(version:7.2)>[집착]목격자 #3
			local quest_progress_3219 =get_quest_progress(3219) -- <(version:7.2)>[집착]폐기물 분쇄기
			local quest_progress_3220 =get_quest_progress(3220) -- <(version:7.2)>[집착]공간이동
			--local quest_progress_3221 =get_quest_progress(3221) -- <(version:7.2)>[집착]마녀의 목소리
			--local quest_progress_3222 =get_quest_progress(3222) -- <(version:7.2)>[집착]ALONE VS ALL
			--local quest_progress_3223 =get_quest_progress(3223) -- <(version:7.2)>[집착]기억의 은총 #1
			--local quest_progress_3224 =get_quest_progress(3224) -- <(version:7.2)>[집착]기억의 은총 #2
			--local quest_progress_3225 =get_quest_progress(3225) -- <(version:7.2)>[집착]바늘호수
			--local quest_progress_3226 =get_quest_progress(3226) -- <(version:7.2)>[집착]마녀 바늘
			--local quest_progress_3227 =get_quest_progress(3227) -- <(version:7.2)>[집착]대면
			--local quest_progress_3228 =get_quest_progress(3228) -- <(version:7.2)>[집착]진실 #1
			--local quest_progress_3229 =get_quest_progress(3229) -- <(version:7.2)>[집착]진실 #2
			--local quest_progress_3230 =get_quest_progress(3230) -- <(version:7.2)>[집착]진실 #3
			--local quest_progress_3231 =get_quest_progress(3231) -- <(version:7.2)>[집착]진실 #4
			--local quest_progress_3232 =get_quest_progress(3232) -- <(version:7.2)>[집착]진실 #5
			--local quest_progress_3233 =get_quest_progress(3233) -- <(version:7.2)>[집착]패닉 
			--local quest_progress_3234 =get_quest_progress(3234) -- <(version:7.2)>[집착]역사 고증 1실
			--local quest_progress_3235 =get_quest_progress(3235) -- <(version:7.2)>[집착]헥토르 전기
			--local quest_progress_3236 =get_quest_progress(3236) -- <(version:7.2)>[집착]마녀 실험체 #1
			--local quest_progress_3237 =get_quest_progress(3237) -- <(version:7.2)>[집착]날조된 문서
			--local quest_progress_3238 =get_quest_progress(3238) -- <(version:7.2)>[집착]마녀 실험체 #2
			--local quest_progress_3239 =get_quest_progress(3239) -- <(version:7.2)>[집착]마녀 루시앙
			--local quest_progress_3240 =get_quest_progress(3240) -- <(version:7.2)>[집착]루시앙의 고뇌
			--local quest_progress_3241 =get_quest_progress(3241) -- <(version:7.2)>[집착]사탄소녀의 반대
			local quest_progress_3242 =get_quest_progress(3242) -- <(version:7.2)>[집착]부활실 입성
			local quest_progress_3243 =get_quest_progress(3243) -- <(version:7.2)>[집착]부활의 심장 탐색
			local quest_progress_3244 =get_quest_progress(3244) -- <(version:7.2)>[집착]루시앙 사념체 
			local quest_progress_3245 =get_quest_progress(3245) -- <(version:7.2)>[집착]정의의 일격
			local quest_progress_3246 =get_quest_progress(3246) -- <(version:7.2)>[집착]헥토르에게 보고
			local quest_progress_3247 =get_quest_progress(3247) -- <(version:7.2)>[집착]화형의 날
			local quest_progress_3248 =get_quest_progress(3248) -- <(version:7.2)>[집착]에르곤 크라토스
			
			if quest_progress_3210 == 0 then
				force_start_quest(3210, 91000821)
												
			elseif quest_progress_3211 == 0 then
				force_start_quest(3211, 91000823)
			
			--elseif quest_progress_3212 == 0 then
			--	force_start_quest(3212, 91000825)
				
			elseif quest_progress_3212 == 0 then
				force_start_quest(3212, 91000825)
				
			elseif quest_progress_3213 == 0 then
				force_start_quest(3213, 91000827)
				
			--elseif quest_progress_3214 == 0 then 
			--	force_start_quest(3214, 91000833)
			
			--elseif quest_progress_3215 == 0 then 
			--	force_start_quest(3215, 91000835)
			
			elseif quest_progress_3216 == 0 then 
				force_start_quest(3216, 91000833)
			
			elseif quest_progress_3217 == 0 then
				force_start_quest(3217, 91000835)
			
			elseif quest_progress_3218 == 0 then
				force_start_quest(3218, 91000837)
			
			elseif quest_progress_3219 == 0 then 
				force_start_quest(3219, 91000839)
			
			elseif quest_progress_3220 == 0 then
				force_start_quest(3220, 91000841)
			
			--elseif quest_progress_3221 == 0 then 
			--	force_start_quest(3221, 91000843)
				
			--elseif quest_progress_3222 == 0 then 
			--	force_start_quest(3222, 91000845)
			
			--elseif quest_progress_3223 == 0 then
			--	force_start_quest(3223, 91000847)
			
			--elseif quest_progress_3224 == 0 then 
			--	force_start_quest(3224, 91000849)
			
			--elseif quest_progress_3225 == 0 then 
			--	force_start_quest(3225, 91000851)
			
			--elseif quest_progress_3226 == 0 then
			--	force_start_quest(3226, 91000853)
			
			--elseif quest_progress_3227 == 0 then 
			--	force_start_quest(3227, 91000855)
				
			--elseif quest_progress_3228 == 0 then 
			--	force_start_quest(3228, 91000857)
			
			--elseif quest_progress_3229 == 0 then 
			--	force_start_quest(3229, 91000859)
			
			--elseif quest_progress_3230 == 0 then 
			--	force_start_quest(3230, 91000861)
			
			--elseif quest_progress_3231 == 0 then 
			--	force_start_quest(3231, 91000863)
			
			--elseif quest_progress_3232 == 0 then 
			--	force_start_quest(3232, 91000865)
				
			--elseif quest_progress_3233 == 0 then 
			--	force_start_quest(3233, 91000867)
			
			--elseif quest_progress_3234 == 0 then 
			--	force_start_quest(3234, 91000869)
			
			--elseif quest_progress_3235 == 0 then 
			--	force_start_quest(3235, 91000871)
			
			--elseif quest_progress_3236 == 0 then 
			--	force_start_quest(3236, 91000873)
			
			--elseif quest_progress_3237 == 0 then 
			--	force_start_quest(3237, 91000875)
			
			--elseif quest_progress_3238 == 0 then 
			--	force_start_quest(3238, 91000877)
			
			--elseif quest_progress_3239 == 0 then
			--	force_start_quest(3239, 91000879)
			
			--elseif quest_progress_3240 == 0 then 
			--	force_start_quest(3240, 91000881)
			
			--elseif quest_progress_3241 == 0 then 
			--	force_start_quest(3241, 91000883)
			
			elseif quest_progress_3242 == 0 then 
				force_start_quest(3242, 91000885)
			
			elseif quest_progress_3243 == 0 then 
				force_start_quest(3243, 91000887)
			
			elseif quest_progress_3244 == 0 then 
				force_start_quest(3244, 91000889)
			
			elseif quest_progress_3245 == 0 then 
				force_start_quest(3245, 91000891)
			
			elseif quest_progress_3246 == 0 then 
				force_start_quest(3246, 91000893)
			
			elseif quest_progress_3247 == 0 then 
				force_start_quest(3247, 91000895)
			
			elseif quest_progress_3248 == 0 then
				force_start_quest(3248, 91000897)
				
			else
			
				cprint( "@91000776" ) -- <(version:7.2)><#6DD66D>고대 마력의 방해로 아무런 작동도 하지 않습니다.
			
			end
			
		elseif ID == 1000203 then
		
			-- 퀘스트 상태 체크 	get_quest_progress(ID)  
			-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
			--local quest_progress_3210 =get_quest_progress(3210) -- <(version:7.2)>[집착]결계해지 #2
			--local quest_progress_3211 =get_quest_progress(3211) -- <(version:7.2)>[집착]아공간 진입
			--local quest_progress_3212 =get_quest_progress(3212) -- <(version:7.2)>[집착]주변 탐색
			--local quest_progress_3213 =get_quest_progress(3213) -- <(version:7.2)>[집착]마녀정보 탐사 #1
			--local quest_progress_3214 =get_quest_progress(3214) -- <(version:7.2)>[집착]마녀정보 탐사 #2
			--local quest_progress_3215 =get_quest_progress(3215) -- <(version:7.2)>[집착]마녀정보 탐사 #3
			--local quest_progress_3216 =get_quest_progress(3216) -- <(version:7.2)>[집착]목격자 #1
			--local quest_progress_3217 =get_quest_progress(3217) -- <(version:7.2)>[집착]목격자 #2
			--local quest_progress_3218 =get_quest_progress(3218) -- <(version:7.2)>[집착]목격자 #3
			--local quest_progress_3219 =get_quest_progress(3219) -- <(version:7.2)>[집착]폐기물 분쇄기
			local quest_progress_3220 =get_quest_progress(3220) -- <(version:7.2)>[집착]공간이동
			local quest_progress_3221 =get_quest_progress(3221) -- <(version:7.2)>[집착]마녀의 목소리
			local quest_progress_3222 =get_quest_progress(3222) -- <(version:7.2)>[집착]ALONE VS ALL
			local quest_progress_3223 =get_quest_progress(3223) -- <(version:7.2)>[집착]기억의 은총 #1
			local quest_progress_3224 =get_quest_progress(3224) -- <(version:7.2)>[집착]기억의 은총 #2
			local quest_progress_3225 =get_quest_progress(3225) -- <(version:7.2)>[집착]바늘호수
			local quest_progress_3226 =get_quest_progress(3226) -- <(version:7.2)>[집착]마녀 바늘
			local quest_progress_3227 =get_quest_progress(3227) -- <(version:7.2)>[집착]대면
			local quest_progress_3228 =get_quest_progress(3228) -- <(version:7.2)>[집착]진실 #1
			local quest_progress_3229 =get_quest_progress(3229) -- <(version:7.2)>[집착]진실 #2
			local quest_progress_3230 =get_quest_progress(3230) -- <(version:7.2)>[집착]진실 #3
			local quest_progress_3231 =get_quest_progress(3231) -- <(version:7.2)>[집착]진실 #4
			local quest_progress_3232 =get_quest_progress(3232) -- <(version:7.2)>[집착]진실 #5
			local quest_progress_3233 =get_quest_progress(3233) -- <(version:7.2)>[집착]패닉 
			local quest_progress_3234 =get_quest_progress(3234) -- <(version:7.2)>[집착]역사 고증 1실
			local quest_progress_3235 =get_quest_progress(3235) -- <(version:7.2)>[집착]헥토르 전기
			local quest_progress_3236 =get_quest_progress(3236) -- <(version:7.2)>[집착]마녀 실험체 #1
			local quest_progress_3237 =get_quest_progress(3237) -- <(version:7.2)>[집착]날조된 문서
			local quest_progress_3238 =get_quest_progress(3238) -- <(version:7.2)>[집착]마녀 실험체 #2
			local quest_progress_3239 =get_quest_progress(3239) -- <(version:7.2)>[집착]마녀 루시앙
			local quest_progress_3240 =get_quest_progress(3240) -- <(version:7.2)>[집착]루시앙의 고뇌
			local quest_progress_3241 =get_quest_progress(3241) -- <(version:7.2)>[집착]사탄소녀의 반대
			local quest_progress_3242 =get_quest_progress(3242) -- <(version:7.2)>[집착]부활실 입성
			--local quest_progress_3243 =get_quest_progress(3243) -- <(version:7.2)>[집착]부활의 심장 탐색
			--local quest_progress_3244 =get_quest_progress(3244) -- <(version:7.2)>[집착]루시앙 사념체 
			--local quest_progress_3245 =get_quest_progress(3245) -- <(version:7.2)>[집착]정의의 일격
			--local quest_progress_3246 =get_quest_progress(3246) -- <(version:7.2)>[집착]헥토르에게 보고
			--local quest_progress_3247 =get_quest_progress(3247) -- <(version:7.2)>[집착]화형의 날
			--local quest_progress_3248 =get_quest_progress(3248) -- <(version:7.2)>[집착]에르곤 크라토스
			
			--if quest_progress_3210 == 0 then
			--	force_start_quest(3210, 91000821)
												
			--elseif quest_progress_3211 == 0 then
			--	force_start_quest(3211, 91000823)
			
			--elseif quest_progress_3212 == 0 then
			--	force_start_quest(3212, 91000825)
				
			--elseif quest_progress_3212 == 0 then
			--	force_start_quest(3212, 91000825)
				
			--elseif quest_progress_3213 == 0 then
			--	force_start_quest(3213, 91000827)
				
			--elseif quest_progress_3214 == 0 then 
			--	force_start_quest(3214, 91000833)
			
			--elseif quest_progress_3215 == 0 then 
			--	force_start_quest(3215, 91000835)
			
			--elseif quest_progress_3216 == 0 then 
			--	force_start_quest(3216, 91000833)
			
			--elseif quest_progress_3217 == 0 then
			--	force_start_quest(3217, 91000835)
			
			--elseif quest_progress_3218 == 0 then
			--	force_start_quest(3218, 91000837)
			
			--elseif quest_progress_3219 == 0 then 
			--	force_start_quest(3219, 91000839)
			
			if quest_progress_3220 == 0 then
				force_start_quest(3220, 91000841)
			
			elseif quest_progress_3221 == 0 then 
				force_start_quest(3221, 91000843)
				
			elseif quest_progress_3222 == 0 then 
				force_start_quest(3222, 91000845)
			
			elseif quest_progress_3223 == 0 then
				force_start_quest(3223, 91000847)
			
			elseif quest_progress_3224 == 0 then 
				force_start_quest(3224, 91000849)
			
			elseif quest_progress_3225 == 0 then 
				force_start_quest(3225, 91000851)
			
			elseif quest_progress_3226 == 0 then
				force_start_quest(3226, 91000853)
			
			elseif quest_progress_3227 == 0 then 
				force_start_quest(3227, 91000855)
				
			elseif quest_progress_3228 == 0 then 
				force_start_quest(3228, 91000857)
			
			elseif quest_progress_3229 == 0 then 
				force_start_quest(3229, 91000859)
			
			elseif quest_progress_3230 == 0 then 
				force_start_quest(3230, 91000861)
			
			elseif quest_progress_3231 == 0 then 
				force_start_quest(3231, 91000863)
			
			elseif quest_progress_3232 == 0 then 
				force_start_quest(3232, 91000865)
				
			elseif quest_progress_3233 == 0 then 
				force_start_quest(3233, 91000867)
			
			elseif quest_progress_3234 == 0 then 
				force_start_quest(3234, 91000869)
			
			elseif quest_progress_3235 == 0 then 
				force_start_quest(3235, 91000871)
			
			elseif quest_progress_3236 == 0 then 
				force_start_quest(3236, 91000873)
			
			elseif quest_progress_3237 == 0 then 
				force_start_quest(3237, 91000875)
			
			elseif quest_progress_3238 == 0 then 
				force_start_quest(3238, 91000877)
			
			elseif quest_progress_3239 == 0 then
				force_start_quest(3239, 91000879)
			
			elseif quest_progress_3240 == 0 then 
				force_start_quest(3240, 91000881)
			
			elseif quest_progress_3241 == 0 then 
				force_start_quest(3241, 91000883)
			
			elseif quest_progress_3242 == 0 then 
				force_start_quest(3242, 91000885)
			
			--elseif quest_progress_3243 == 0 then 
			--	force_start_quest(3243, 91000887)
			
			--elseif quest_progress_3244 == 0 then 
			--	force_start_quest(3244, 91000889)
			
			--elseif quest_progress_3245 == 0 then 
			--	force_start_quest(3245, 91000891)
			
			--elseif quest_progress_3246 == 0 then 
			--	force_start_quest(3246, 91000893)
			
			--elseif quest_progress_3247 == 0 then 
			--	force_start_quest(3247, 91000895)
			
			--elseif quest_progress_3248 == 0 then
			--	force_start_quest(3248, 91000897)
			
			else
			
				cprint( "@91000776" ) -- <(version:7.2)><#6DD66D>고대 마력의 방해로 아무런 작동도 하지 않습니다.
			
			end
			
		end
	--else 
	
		--cprint( "@143" )
	
	--end
	--================================================나무상자 여는 스크립트	
	
	if ID == 601100232 then -- 나무상자 아이템을 사용하면
		local own_gold = get_value("gold") 
		local delete_gold = 500000 
		local index_num_1  = math.random ( 1, 10 )
		
		if is_premium() == true then
		
			delete_gold = delete_gold - (delete_gold * 0.15) 
		
		end
		
		if own_gold >= delete_gold  then -- 가진돈이 소비될 돈보다 많거나 같으면	
			local use_item = sconv("@251", "#@item_name@#",tostring("@611100232"))
			local text_gold = sconv("@263", "#@gold@#",tostring(delete_gold))
			
			delete_item( get_item_handle( ID ), 1 ) -- 나무상자 하나 삭제하고
			set_value("gold", own_gold - delete_gold) -- 루피를 소비시켜주고
			
			cprint( text_gold ) -- 얼마 소비했는지 채팅창에 알려준다
			cprint( use_item )  -- 나무 상자를 소비했다는 것도 알려준다
						
			if index_num_1 >= 3 then -- 주사위굴린 결과가 3보다 크거나 같으면(80%)
				-- 드랍 그룹에서 아이템 가져오는 스크립트 사용
				-- item[1]: 생성 아이템 코드 / item[2]: 개수
				local item = pick_item_in_drop_group( -9007022 )
				local item_name = get_item_name_id( item[1] )
					
				-- 메시지 조합: ~아이템을 ~개 획득하였습니다.  
				local get_item_message = sconv( "@254", "#@item_name@#", "@" .. tostring(item_name), "#@item_num@#", tostring( item[2] ) )
				insert_item( item[1], item[2] )
				cprint( get_item_message )
				
			else
				cprint( "@690000152" )
								
			end	
		
		else -- 애초에 가진돈이 소비될 돈보다 적으면
		
			cprint( "@90010008" ) -- 너님 가난뱅이
			
		end
			
	end
	
	--================================================초보자,1,2차 직업 10-150레벨이하 지원용 버프 스크립트
	
	if ID == 2013081 then -- 신성한 불꽃의 힘 아이템 사용했을때
		
		if get_value( "job_depth" ) < 3 and get_value( "level" ) > 9 and get_value( "level" ) < 151 then
			add_state(41102536, get_value( "level" ) / 1.5, 360000)
			add_state(41102537, get_value( "level" ) / 1.5, 360000)
			cprint( "@90606141" )
		else
			cprint( "@90606143" )
		end
	end
	
	if ID == 2013082 then -- 신성한 불꽃의 힘: 축복 크루 아이템 사용했을때(더 좋은 성능은 부여한다.)
		
		if get_value( "job_depth" ) < 3 and get_value( "level" ) > 9 and get_value( "level" ) < 151 then
			add_state(41102536, get_value( "level" ), 360000)
			add_state(41102537, get_value( "level" ), 360000)
			cprint( "@90606142" )
		else
			cprint( "@90606143" )
		end
	end
end



---on_use_item스크립트를 기획팀작업과 라이브 작업을 분리하기 이해 작업위치를 나누어 놓았음

function on_use_item_by_live_team( ID, p_handle, target, t_handle, target_id, target_creature )



		if ID == 2012825 then  --데바의 축복 사내테스트용
			add_state( 314017,  70, 180000 ) -- 스피드 오브 세이지 		 7레벨
			add_state(   2506,  41, 180000 ) -- 다크 마이트 			20레벨
			add_state(   2508,  41, 180000 ) -- 다크 마이트 			20레벨
			add_state( 163429,  10, 180000 ) -- 윈드웨폰 				10레벨
			add_state(  13424, 277, 180000 ) -- 바위의 정기 			16레벨
			add_state(  13423,  36, 180000 ) -- 바람의 응원 			16레벨
			add_state(  13425, 277, 180000 ) -- 성화의 기운 			16레벨
			add_state( 314018,  70, 180000 ) -- 정신통일: 일격의 정기 	 7레벨
			add_state( 163404,  48, 180000 ) -- 블레싱 오브 바이탈리티 	16레벨
			add_state( 163405,  48, 180000 ) -- 블레싱 오브 인텔리전스 	16레벨
			add_state( 163406,  48, 180000 ) -- 블레싱 오브 멘탈 		16레벨
			add_state( 163407,  48, 180000 ) -- 블레싱 오브 스트렝스 	16레벨
			add_state( 163433,  90, 180000 ) -- 샤이닝 아머 			20레벨
			add_state(   2505, 160, 180000 ) -- 샤이닝 웨폰 			20레벨
			add_state(   2507, 160, 180000 ) -- 샤이닝 웨폰 			20레벨
			add_state( 314016,  70, 180000 ) -- 자연과 힘의 조화 		 7레벨
		end
		

		
		if ID == 2012818 or ID == 2013058 then  --축복받은 카벙클 상자<2013년 구정 이벤트>
		
			local level = get_value( "level" )
			
			if level < 50 then
				local index = math.random( 1, 10 )	
				if index <= 7 then
					insert_item(2012769,1)
				elseif index <= 10 then
					insert_item(2012775,1)
				end
				
			elseif	level < 80 then
				local index = math.random( 1, 10 )	
				if index <= 7 then
					insert_item(2012770,1)
				elseif index <= 10 then
					insert_item(2012776,1)
				end
		
			elseif	level < 100 then
				local index = math.random( 1, 10 )	
				if index <= 7 then
					insert_item(2012771,1)
				elseif index <= 10 then
					insert_item(2012777,1)
				end		
		
		elseif	level < 120 then
				local index = math.random( 1, 10 )	
				if index <= 7 then
					insert_item(2012772,1)
				elseif index <= 10 then
					insert_item(2012778,1)
				end				
		
		elseif	level < 150 then
				local index = math.random( 1, 10 )	
				if index <= 7 then
					insert_item(2012773,1)
				elseif index <= 10 then
					insert_item(2012779,1)
				end					
		
		else 	local index = math.random( 1, 10 )	
				if index <= 7 then
					insert_item(2012774,1)
				elseif index <= 10 then
					insert_item(2012780,1)
				end				
		
			end
		
		
		
		end

	if ID ==  2012832 then -- 햇 토마토
		local state_level_1 = get_state_level( 41102507 )
		local state_level_2 = get_state_level( 41102508 )
		local state_level_3 = get_state_level( 41102509 )
		local state_level_4 = get_state_level( 41102510 )
		local state_level_5 = get_state_level( 41102511 )
		
		local cash_state_level_1 = get_state_level( 41103030 )
		local cash_state_level_2 = get_state_level( 41103031 )
		local cash_state_level_3 = get_state_level( 41103032 )
		local cash_state_level_4 = get_state_level( 41103033 )
		local cash_state_level_5 = get_state_level( 41103034 )		
	 
		if cash_state_level_1 == 2 or cash_state_level_2 == 2 or cash_state_level_3 == 2 or cash_state_level_4 == 2 or cash_state_level_5 == 2 then
			cprint("@1008")
			return
		end

		if state_level_1 == 1 then
			remove_state (41102507,1, p_handle)
			add_state	(41102508, 1, 360000) 
			
			
		elseif state_level_2 == 1 then 
			remove_state (41102508,1, p_handle)
			add_state	(41102509, 1, 360000) 		
			
			
		elseif state_level_3 == 1 then 
			remove_state (41102509, 1, p_handle)
			add_state	(41102510, 1, 360000) 				
			
			
		elseif state_level_4 == 1 then 
			remove_state (41102510,1,p_handle)
			add_state	(41102511, 1, 360000) 		

			
		elseif state_level_5 == 1 then 
			remove_state (41102511,1, p_handle)
			add_state	(41102511, 1, 360000) 			
			

		else 
			add_state	(41102507, 1, 360000)
			
		end
		
		delete_item(get_item_handle(2012832), 1)
		
	end
	
	if ID ==  2013204 then -- 중동 요청 햇 토마토류 아이템
		local state_level_1 = get_state_level( 41103030 )
		local state_level_2 = get_state_level( 41103031 )
		local state_level_3 = get_state_level( 41103032 )
		local state_level_4 = get_state_level( 41103033 )
		local state_level_5 = get_state_level( 41103034 )
		 
		local cash_state_level_1 = get_state_level( 41102507 )
		local cash_state_level_2 = get_state_level( 41102508 )
		local cash_state_level_3 = get_state_level( 41102509 )
		local cash_state_level_4 = get_state_level( 41102510 )
		local cash_state_level_5 = get_state_level( 41102511 )		
	 
		if cash_state_level_1 == 1 or cash_state_level_2 == 1 or cash_state_level_3 == 1 or cash_state_level_4 == 1 or cash_state_level_5 == 1 then
			cprint("@1008")
			return
		end

		 
		if state_level_1 == 2 then
			remove_state (41103030,2, p_handle)
			add_state	(41103031, 2, 360000) 
			
			
		elseif state_level_2 == 2 then 
			remove_state (41103031,2, p_handle)
			add_state	(41103032, 2, 360000) 		
			
			
		elseif state_level_3 == 2 then 
			remove_state (41103032, 2, p_handle)
			add_state	(41103033, 2, 360000) 				
			
			
		elseif state_level_4 == 2 then 
			remove_state (41103033,2,p_handle)
			add_state	(41103034, 2, 360000) 		

			
		elseif state_level_5 == 2 then 
			remove_state (41103034,2, p_handle)
			add_state	(41103034, 2, 360000) 			
			

		else 
			add_state	(41103030, 2, 360000)
			
		end
		
		delete_item(get_item_handle(2013204), 1)
		
	end
	
	if ID == 2013060 then
		insert_item(2013023,1)
		insert_item(2013024,1)
		insert_item(2013025,1)
		insert_item(2013026,1)
		insert_item(2013043,1)
	end
	
	if ID == 2013061 then
		insert_item(2013031,1)
		insert_item(2013032,1)
		insert_item(2013033,1)
		insert_item(2013034,1)
		insert_item(2013045,1)
	end
		
end
function on_use_item_by_program_team( ID, p_handle, target, t_handle, target_id, target_creature )
	--강철 조각 조합
	if ID == 1100101 or ID == 1100102 then
		local create_count = math.floor( find_item( ID ) / 10 )
		delete_item( get_item_handle( ID ), create_count * 10 )
		insert_item( ID + 1, create_count )
	end
	
	--행운이 빗나간 빈 종이 조합
	if ID == 3800290 then
		local create_count = math.floor( find_item( ID ) / 10 )
		delete_item( get_item_handle( ID ), create_count * 10 )
		insert_item( 3800280 , create_count )
	end
	
	-- 바포메트 크리처 카드 획득 알림
	if ID == 3800285 then	
		local ap_creaturecard_get = sconv("@90605913", "#@user_name@#", get_value("name"))		
		announce( ap_creaturecard_get )	
	end
	
	-- 슬래터 크리처 카드 획득 알림
	if ID == 3800286 then	
		local ap_creaturecard_get = sconv("@90605914", "#@user_name@#", get_value("name"))		
		announce( ap_creaturecard_get )	
	end
	
	-- 운다인 크리처 카드 획득 알림
	if ID == 3800287 then	
		local ap_creaturecard_get = sconv("@90605915", "#@user_name@#", get_value("name"))		
		announce( ap_creaturecard_get )	
	end
	
	-- 미노타우르스 크리처 카드 획득 알림
	if ID == 3800288 then	
		local ap_creaturecard_get = sconv("@90605916", "#@user_name@#", get_value("name"))		
		announce( ap_creaturecard_get )	
	end
	
	-- 픽 크리처 카드 획득 알림
	if ID == 3800289 then	
		local ap_creaturecard_get = sconv("@90605917", "#@user_name@#", get_value("name"))		
		announce( ap_creaturecard_get )	
	end
	
	-- 시크루트 1시간
	if ID == 910084 or ID == 910089 then	
	
		set_account_authority( 910000, 3600 )
				
		if is_premium() == false then
			cprint( "@90605918" )
		elseif is_premium() == true then
			cprint( "@90605923" )
		end
	end
	
	-- 시크루트 1일
	if ID == 910085 or ID == 910090 then	
	
		set_account_authority( 910000, 86400 )
				
		if is_premium() == false then
			cprint( "@90605919" )
		elseif is_premium() == true then
			cprint( "@90605924" )
		end
	end
	
	-- 시크루트 7일
	if ID == 910086 or ID == 910091 then	
	
		set_account_authority( 910000, 604800 )
				
		if is_premium() == false then
			cprint( "@90605920" )
		elseif is_premium() == true then
			cprint( "@90605925" )
		end
	end
	
	-- 시크루트 30일
	if ID == 910087 or ID == 910092 then	
	
		set_account_authority( 910000, 2592000 )
				
		if is_premium() == false then
			cprint( "@90605921" )
		elseif is_premium() == true then
			cprint( "@90605926" )
		end
	end
	
	-- 시크루트 90일
	if ID == 910088 or ID == 910093 then	
	
		set_account_authority( 910000, 7776000 )
				
		if is_premium() == false then
			cprint( "@90605922" )
		elseif is_premium() == true then
			cprint( "@90605927" )
		end
	end
		
end