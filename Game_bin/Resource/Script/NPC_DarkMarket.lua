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

	if ID == 608410 or ID == 608415 or ID == 608416 or ID == 608417 or ID == 608418 then
		open_storage()
	
	elseif ID == 608411 or ID == 608419 or ID == 608420 or ID == 608421 or ID == 608422 then
		if get_global_variable("disableAuction") == 1 then
			insert_item( 608411, 1 )
		else
			--open_market( 'beginner_equip' )
			show_auction_window()
		end
		
	elseif ID == 608412 then
		open_market( 'boost_chip' )
		
	elseif ID == 609096 then
		open_market('sec_sever_merchant_etc')
		
	elseif ID == 2016026 then -- <(version:9.2)>패러렐 월드 퀘스트용 티켓 사용 시 - [일일]틈새조사 : 타임어택
		
		--4009: 이공간 조사
		--2016026 임시 허가증
		
		if get_instance_dungeon_id() == 0 or get_instance_dungeon_id() == nil then
			local current_quest = get_quest_progress(4009)
			if current_quest == 0 or current_quest == "" or current_quest == nil or current_quest == 255 then
				dlg_special( 'confirm_window', 'force_start_quest( 4009, 90610008 )', '@9907' )
			else
				private_notice( "@90610020" ) -- 퀘스트 상태를 확인 후 다시 시도해주세요.
			end
		else
			private_notice( "@9243" ) -- 이 장소에서는 사용할 수 없습니다
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
			
		elseif quest_progress_3212 == 0 then
			force_start_quest(3212, 91000825)
			
		elseif quest_progress_3213 == 0 then
			force_start_quest(3213, 91000827)
		
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
		else		
			cprint( "@91000776" ) -- <(version:7.2)><#6DD66D>고대 마력의 방해로 아무런 작동도 하지 않습니다.
		end
	elseif ID == 608413 then -- Pet scroll

		local tbuffs =
		{
		--	Buff ID,	Max Lv
			163404,	100,	-- Blessing of Vitality
			163405,	100,	-- Blessing of Intelligence
			163406,	100,		-- Blessing of Wisdom
			163407,	100,	-- Blessing of Strength
			163433,	167,	-- Shining Armor
			2505,	322,	-- Shining Weapon
			2507,	322,	-- Shining Weapon
			13423,	65,		-- Speed of the Wind
			13424,	560,	-- Rock Energy
			13425,	577,	-- Force of Sacred Fire
			163429,	24,		-- Wind Weapon
			2506,	79,		-- Dark Might
			2508,	79,		-- Dark Might
			314049,	356,	-- Insight
			163449,	77,		-- Protector's Force
			163448,	77,		-- Angel's Force
			163440,	234,	-- Rally
			314099,	188,	-- Howl at the Moon
			314039,	10,		-- Demonic Howl
			314016,	210,	-- Divine Purpose
			314017,	360,	-- Asuran Haste
			314018,	210,	-- Gaian Strength
			314077,	2,		-- Heaven 5%
			314078,	2,		-- Heaven 10%
			314079,	2,		-- Heaven 20%
			1014,	110		-- Concentration
		}
		
		local cnt_buffs = table.getn(tbuffs)/2

		for i = 1, cnt_buffs do
			local base = 1+(i-1)*2
			
			local getBuffLv = get_state_level(tbuffs[base])

			if getBuffLv >= 1 and getBuffLv < tbuffs[base+1] then
				add_cstate ( tbuffs[base], getBuffLv, 360000 )
				
			end
			
		end
	
	elseif ID == 608414 then	
		
		local tbuffs =
		{
		--	Buff ID,	Max Lv
			163404,	100,	-- Blessing of Vitality
			163405,	100,	-- Blessing of Intelligence
			163406,	100,	-- Blessing of Wisdom
			163407,	100,	-- Blessing of Strength
			163433,	167,	-- Shining Armor
			2505,	322,	-- Shining Weapon
			2507,	322,	-- Shining Weapon
			13423,	65,		-- Speed of the Wind
			13424,	560,	-- Rock Energy
			13425,	577,	-- Force of Sacred Fire
			163429,	24,		-- Wind Weapon
			2506,	79,		-- Dark Might
			2508,	79,		-- Dark Might
			314049,	356,	-- Insight
			163449,	77,		-- Protector's Force
			163448,	77,		-- Angel's Force
			163440,	234,	-- Rally
			314099,	188,	-- Howl at the Moon
			314039,	10,		-- Demonic Howl
			314016,	210,	-- Divine Purpose
			314017,	360,	-- Asuran Haste
			314018,	210,	-- Gaian Strength
			314077,	2,		-- Heaven 5%
			314078,	2,		-- Heaven 10%
			314079,	2,		-- Heaven 20%
			1014,	110		-- Concentration
		}
		
		local cnt_buffs = table.getn(tbuffs)/2

		for i = 1, cnt_buffs do
			local base = 1+(i-1)*2
			
			local getBuffLv = get_state_level(tbuffs[base])

			if getBuffLv >= 1 and getBuffLv < tbuffs[base+1] then
				add_state ( tbuffs[base], getBuffLv, 360000 )
				
			end
				
		end
	end
	--================================================나무상자 여는 스크립트	
	-- 아이템의 속성을 나무상자와 같이 사용가능으로 변경
	-- update [Arcadia_TestQA].[dbo].[ItemResource] set type=6, script_text = 'on_use_item' 
	-- where id in  (601100232,601100233,601101001,601101002,601101003,601101004)

	-- 나무상자: 601100232
	-- 빛바렌상자: 601100233
	-- 정제된상자: 601101001
	-- 견고한상자: 601101002
	-- 평온한상자: 601101003
	-- 예리한상자: 601101004

	if ID == 601100232 or ID == 601100233 or  ID == 601101001 or ID == 601101002 or ID == 601101003 or ID == 601101004 then
		local key_item	  = 0
		local drop_group = -9007022

		if		ID == 601100232 then -- 나무상자
				key_item = 601100234
				drop_group = -9007022
		elseif	ID == 601100233 then 
				key_item = 601100235
				drop_group = -9007025
		elseif	ID == 601101001 then 
				key_item = 601101005			
				drop_group = -9007110			
		elseif	ID == 601101002	then
				key_item = 601101006
				drop_group = -9007111
		elseif	ID == 601101003	then -- 평온한 유물상자
				key_item = 601101007
			if get_local_info() == 8192 then
				drop_group = -9107311
			else
				drop_group = -9007112
			end
		elseif	ID == 601101004	then
				key_item = 601101008
				drop_group = -9007113
		else
				return
		end

		-- 열쇠체크
		if  key_item >0 then
			local key_count = find_item ( key_item )

			if  key_count < 1	then
				cprint( "@90610200")
				return
			end		
		end

		-- 키소모
		if key_item > 0 then
			delete_item( get_item_handle( key_item ), 1 ) -- 나무상자 하나 삭제하고
		end

		-- 상자소모
		local box_name = get_item_name_id( ID )
		local use_item = sconv("@251", "#@item_name@#","@" .. tostring(box_name) )
		cprint( use_item ) 
		delete_item( get_item_handle( ID ), 1 ) 

				
		-- 드랍 그룹에서 아이템 가져오는 스크립트 사용
		-- item[1]: 생성 아이템 코드 / item[2]: 개수
		local item = pick_item_in_drop_group( drop_group )
		local item_name = get_item_name_id( item[1] )
				
		-- 메시지 조합: ~아이템을 ~개 획득하였습니다.  
		local get_item_message = sconv( "@254", "#@item_name@#", "@" .. tostring(item_name), "#@item_num@#", tostring( item[2] ) )
		insert_item( item[1], item[2] )
		cprint( get_item_message )
		
	end
	--================================================비밀에 쌓인 상자 열기
	if ID >= 601100366 and ID <= 601100397  then

		local idx = ID - 601100366 ;
		if idx%2 == 0 then -- 열쇠 클릭, 상자 클릭 모두 적용
			key = ID
			box = ID+1
		else
			key = ID-1
			box = ID
		end
		
		-- 열쇠체크
		if  key > 0 then
			local key_count = find_item ( key )

			if  key_count < 1	then
				cprint( "@90610200")
				return
			end		
		end
		
		if  box > 0 then
			local box_count = find_item ( box )
			if  box_count < 1	then
				cprint( "@90610343") -- 상자가 필요합니다.
				return
			end
		end

		-- 키, 상자 소모
		if key > 0 and box > 0 then
			delete_item( get_item_handle( key ), 1 ) -- 열쇠 하나 삭제하고
			delete_item( get_item_handle( box ), 1 ) -- 상자 하나 삭제하고
		end
		
		local pdata =	{ -9007097, -9007098, -9007099, -9007100, -9007150, -9007151, -9007152, -9007153, 
						  601100398, 601100399, 601100400, 601100401, 601100402, 601100403, 601100404, 601100405,}
		pdata =	pdata[(idx/2)-(idx/2%1)+1]
		-- 드랍 그룹에서 아이템 가져오는 스크립트 사용
		-- item[1]: 생성 아이템 코드 / item[2]: 개수
		if ID <= 601100381 then
			local item = pick_item_in_drop_group( pdata )
			local item_name = get_item_name_id( item[1] )
			
			-- 메시지 조합: ~아이템을 ~개 획득하였습니다.  
			local get_item_message = sconv( "@254", "#@item_name@#", "@" .. tostring(item_name), "#@item_num@#", tostring( item[2] ) )
			insert_item( item[1], item[2] )
			cprint( get_item_message )
		else			
			local item_name = get_item_name_id( pdata )
			-- 메시지 조합: ~아이템을 ~개 획득하였습니다.  
			local get_item_message = sconv( "@254", "#@item_name@#", "@" .. tostring(item_name), "#@item_num@#", 1 )
			insert_item( pdata, 1 )
			cprint( get_item_message )
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
	--================================================마계의 천 조각 조합 아이템 복구
	if ID == 2013815 then
		insert_item(2016141, 5)
		insert_item(1000540, 1000)
		insert_item(1000541, 700)
		insert_item(1000542, 300)
		insert_item(2013816, 10)
		insert_item(602701, 1)
	end
end

---on_use_item스크립트를 기획팀작업과 라이브 작업을 분리하기 이해 작업위치를 나누어 놓았음

function on_use_item_by_live_team( ID, p_handle, target, t_handle, target_id, target_creature )
		
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
		else 	
			local index = math.random( 1, 10 )
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

		local cash_state_level_6 = get_state_level( 41102544 )
		local cash_state_level_7 = get_state_level( 41102545 )
		local cash_state_level_8 = get_state_level( 41102546 )
		local cash_state_level_9 = get_state_level( 41102547 )
		local cash_state_level_10 = get_state_level( 41102548 )
		
		local cash_state_level_11 = get_state_level( 41102549 )
		local cash_state_level_12 = get_state_level( 41102550 )
		local cash_state_level_13 = get_state_level( 41102551 )
		local cash_state_level_14 = get_state_level( 41102552 )
		local cash_state_level_15 = get_state_level( 41102553 )
	 
		if cash_state_level_1 == 1 or cash_state_level_2 == 1 or cash_state_level_3 == 1 or cash_state_level_4 == 1 or cash_state_level_5 == 1 or cash_state_level_6 == 1 or cash_state_level_7 == 1 or cash_state_level_8 == 1 or cash_state_level_9 == 1 or cash_state_level_10 == 1 or cash_state_level_11 == 1 or cash_state_level_12 == 1 or cash_state_level_13 == 1 or cash_state_level_14 == 1 or cash_state_level_15 == 1 then
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
	
	if ID ==  2013204 then -- 금의 기운
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

		local cash_state_level_6 = get_state_level( 41102544 )
		local cash_state_level_7 = get_state_level( 41102545 )
		local cash_state_level_8 = get_state_level( 41102546 )
		local cash_state_level_9 = get_state_level( 41102547 )
		local cash_state_level_10 = get_state_level( 41102548 )
		
		local cash_state_level_11 = get_state_level( 41102549 )
		local cash_state_level_12 = get_state_level( 41102550 )
		local cash_state_level_13 = get_state_level( 41102551 )
		local cash_state_level_14 = get_state_level( 41102552 )
		local cash_state_level_15 = get_state_level( 41102553 )
	 
		if cash_state_level_1 == 1 or cash_state_level_2 == 1 or cash_state_level_3 == 1 or cash_state_level_4 == 1 or cash_state_level_5 == 1 or cash_state_level_6 == 1 or cash_state_level_7 == 1 or cash_state_level_8 == 1 or cash_state_level_9 == 1 or cash_state_level_10 == 1 or cash_state_level_11 == 1 or cash_state_level_12 == 1 or cash_state_level_13 == 1 or cash_state_level_14 == 1 or cash_state_level_15 == 1 then
			cprint("@1008")
			return
		end
		 
		if state_level_1 == 1 then
			remove_state (41103030,1, p_handle)
			add_state	(41103031, 1, 360000)
		elseif state_level_2 == 1 then 
			remove_state (41103031,1, p_handle)
			add_state	(41103032, 1, 360000)
		elseif state_level_3 == 1 then 
			remove_state (41103032, 1, p_handle)
			add_state	(41103033, 1, 360000)
		elseif state_level_4 == 1 then 
			remove_state (41103033,1,p_handle)
			add_state	(41103034, 1, 360000)
		elseif state_level_5 == 1 then 
			remove_state (41103034,1, p_handle)
			add_state	(41103034, 1, 360000)
		else 
			add_state	(41103030, 1, 360000)
		end		
		delete_item(get_item_handle(2013204), 1)		
	end
	
	if ID ==  2016072 then -- 햇토마토(2) version 9.2
		local state_level_1 = get_state_level( 41102544 )
		local state_level_2 = get_state_level( 41102545 )
		local state_level_3 = get_state_level( 41102546 )
		local state_level_4 = get_state_level( 41102547 )
		local state_level_5 = get_state_level( 41102548 )
		
		local cash_state_level_1 = get_state_level( 41103030 )
		local cash_state_level_2 = get_state_level( 41103031 )
		local cash_state_level_3 = get_state_level( 41103032 )
		local cash_state_level_4 = get_state_level( 41103033 )
		local cash_state_level_5 = get_state_level( 41103034 )
		 
		local cash_state_level_6 = get_state_level( 41102507 )
		local cash_state_level_7 = get_state_level( 41102508 )
		local cash_state_level_8 = get_state_level( 41102509 )
		local cash_state_level_9 = get_state_level( 41102510 )
		local cash_state_level_10 = get_state_level( 41102511 )
		
		local cash_state_level_11 = get_state_level( 41102549 )
		local cash_state_level_12 = get_state_level( 41102550 )
		local cash_state_level_13 = get_state_level( 41102551 )
		local cash_state_level_14 = get_state_level( 41102552 )
		local cash_state_level_15 = get_state_level( 41102553 )

		if cash_state_level_1 == 1 or cash_state_level_2 == 1 or cash_state_level_3 == 1 or cash_state_level_4 == 1 or cash_state_level_5 == 1 or cash_state_level_6 == 1 or cash_state_level_7 == 1 or cash_state_level_8 == 1 or cash_state_level_9 == 1 or cash_state_level_10 == 1 or cash_state_level_11 == 1 or cash_state_level_12 == 1 or cash_state_level_13 == 1 or cash_state_level_14 == 1 or cash_state_level_15 == 1 then
			cprint("@1008")
			return
		end
		 
		if state_level_1 == 1 then
			remove_state (41102544,1, p_handle)
			add_state	(41102545, 1, 360000)
		elseif state_level_2 == 1 then 
			remove_state (41102545,1, p_handle)
			add_state	(41102546, 1, 360000)
		elseif state_level_3 == 1 then 
			remove_state (41102546, 1, p_handle)
			add_state	(41102547, 1, 360000)
		elseif state_level_4 == 1 then 
			remove_state (41102547,1,p_handle)
			add_state	(41102548, 1, 360000)
		elseif state_level_5 == 1 then 
			remove_state (41102548,1, p_handle)
			add_state	(41102548, 1, 360000)
		else 
			add_state	(41102544, 1, 360000)
		end		
		delete_item(get_item_handle(2016072), 1)		
	end
	
	if ID ==  2016073 then -- 금의기운(2) version 9.2	
		local state_level_1 = get_state_level( 41102549 )
		local state_level_2 = get_state_level( 41102550 )
		local state_level_3 = get_state_level( 41102551 )
		local state_level_4 = get_state_level( 41102552 )
		local state_level_5 = get_state_level( 41102553 )
		
		local cash_state_level_1 = get_state_level( 41102544 )
		local cash_state_level_2 = get_state_level( 41102545 )
		local cash_state_level_3 = get_state_level( 41102546 )
		local cash_state_level_4 = get_state_level( 41102547 )
		local cash_state_level_5 = get_state_level( 41102548 )
		
		local cash_state_level_6 = get_state_level( 41103030 )
		local cash_state_level_7 = get_state_level( 41103031 )
		local cash_state_level_8 = get_state_level( 41103032 )
		local cash_state_level_9 = get_state_level( 41103033 )
		local cash_state_level_10 = get_state_level( 41103034 )
		 
		local cash_state_level_11 = get_state_level( 41102507 )
		local cash_state_level_12 = get_state_level( 41102508 )
		local cash_state_level_13 = get_state_level( 41102509 )
		local cash_state_level_14 = get_state_level( 41102510 )
		local cash_state_level_15 = get_state_level( 41102511 )

		if cash_state_level_1 == 1 or cash_state_level_2 == 1 or cash_state_level_3 == 1 or cash_state_level_4 == 1 or cash_state_level_5 == 1 or cash_state_level_6 == 1 or cash_state_level_7 == 1 or cash_state_level_8 == 1 or cash_state_level_9 == 1 or cash_state_level_10 == 1 or cash_state_level_11 == 1 or cash_state_level_12 == 1 or cash_state_level_13 == 1 or cash_state_level_14 == 1 or cash_state_level_15 == 1 then
			cprint("@1008")
			return
		end
		 
		if state_level_1 == 1 then
			remove_state (41102549,1, p_handle)
			add_state	(41102550, 1, 360000)
		elseif state_level_2 == 1 then 
			remove_state (41102550,1, p_handle)
			add_state	(41102551, 1, 360000)
		elseif state_level_3 == 1 then 
			remove_state (41102551, 1, p_handle)
			add_state	(41102552, 1, 360000)
		elseif state_level_4 == 1 then 
			remove_state (41102552,1,p_handle)
			add_state	(41102553, 1, 360000)
		elseif state_level_5 == 1 then 
			remove_state (41102553,1, p_handle)
			add_state	(41102553, 1, 360000)
		else 
			add_state	(41102549, 1, 360000)
		end
		
		delete_item(get_item_handle(2016073), 1)		
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
	
	if ID == 2013685 then -- 스타터 팩
		insert_item(910109,1) -- 시크루트 귀환권 30일
		insert_item(3630334,1) -- 야만전사의 날개 (28일)
		insert_item(540219,1) -- 라이디언 킹 (28일)
		insert_item(2012239,1) -- 펫 랜덤 박스(28일 펫)
		insert_item(950111,1) -- 카오스 스톤 강화석
		insert_item(900010,10) -- 스태미너 세이버 1시간
		insert_item(2012823,10) -- 성장의 물약 디럭스(1~150)
		insert_item(950112,5) -- 변조된 전능의 조각(2)
		insert_item(601100408,10) -- 데바의 축복 1시간
		insert_item(2013011,10) -- 공격의 루나칩 1시간
		insert_item(602060,500) -- 데바의 레드 포션
		insert_item(602260,500) -- 데바의 블루 포션		
	end
	
	if ID == 2013707 then -- 복귀자 지원 상자
		insert_item(910109,1) -- 시쿠루트 귀환권 30일 <캐릭터용>
		insert_item(540217,1) -- 홀리 유니콘<28일 낙상방지>
		insert_item(690495,1) -- 마이 아이린 <28일>
		insert_item(2013011,30) -- 공격의 루나칩 1시간
		insert_item(2013010,50) -- 데바의 축복
		insert_item(950117,50) -- 변조된 전능의 조각2
		insert_item(2902164,15) -- 스태미너 세이버
		insert_item(900018,15) -- 성장의 물약 엑설런트
		insert_item(602061,500) -- 데바의 레드포션
		insert_item(602261,500) -- 데바의 블루포션
		insert_item(3800303,3) -- 베이직: 다크메이지 꾸미기 투구 상자
		insert_item(3800304,3) -- 베이직: 다크메이지 꾸미기 신발 상자
		insert_item(3800305,3) -- 베이직: 다크메이지 꾸미기 장갑 상자
		insert_item(3800306,3) -- 베이직: 다크메이지 꾸미기 의상 상자
		insert_item(700000881,1) -- 복귀유저 망토	
	end
	
	-- 중동 요청 선물 상자
	if ID == 2013706 then
		local index = math.random( 1, 100 )
		
		if index <= 25 then
			insert_item(2010543,1)
		elseif index <= 42 then
			insert_item(2010544,1)
		elseif index <= 57 then
			insert_item(2010545,1)
		elseif index <= 69 then		
			insert_item(2010546,1)
		elseif index <= 79 then
			insert_item(2010547,1)
		elseif index <= 87 then		
			insert_item(2010548,1)
		elseif index <= 93 then
			insert_item(2010549,1)
		elseif index <= 97 then
			insert_item(2010550,1)
		elseif index <= 99 then
			insert_item(2010551,1)
		elseif index <= 100 then
			insert_item(2010552,1)
		end
	end
	
	if ID == 491014 then -- 라펠즈 보상상자
		insert_item(2012802,1) -- 완벽한 재생의 가루 <3일>
		insert_item(900018,3) -- 성장의 물약 엑설런트 1~300
		insert_item(2902042,5) -- 대모요정의 병
		insert_item(950082,5) -- 완전한 크리처 부활 주문서
		insert_item(2013513,10) -- 불의 기운
		insert_item(2013514,10) -- 바람의 기운
		insert_item(2013515,10) -- 물의 기운
	end
	
	if ID == 2016056 then -- 간담회 참석 상자
		insert_item(2012802,1) -- 완벽한 재생의 가루
		insert_item(2016055,5) -- 모두의 축복 상자
		insert_item(2013698,50) -- 스페셜 피물약 
		insert_item(2013699,50) -- 스페셜 마나물약
		insert_item(2016081,1) -- 크루샵 랜덤상자
	end
	
	--- 2018 월드컵
		
	if ID == 2013322 then -- 32강
		local rand = math.random( 1, 2)
		if rand == 1 then
			insert_item( 2013310, 1 ) -- 패
			message( "<#00ffff>|".."@91000500")			
		else
			insert_item( 2013316, 1 ) -- 승
			message( "<#00ffff>|".."@91000490")
		end
	end
	
	if ID == 2013323 then -- 16강
		local rand = math.random( 1, 2)
		if rand == 1 then
			insert_item( 2013311, 1 ) -- 패
			message( "<#00ffff>|".."@91000503")
		else
			insert_item( 2013317, 1 ) -- 승
			message( "<#00ffff>|".."@91000492")
		end
	end
	
	if ID == 2013324 then -- 8강
		local rand = math.random( 1, 2)
		if rand == 1 then
			insert_item( 2013312, 1 ) -- 패
			message( "<#00ffff>|".."@91000505")
		else
			insert_item( 2013318, 1 ) -- 승
			message( "<#00ffff>|".."@91000494")
		end
	end
	
	if ID == 2013325 then -- 4강
		local rand = math.random( 1, 2)
		if rand == 1 then
			insert_item( 2013313, 1 ) -- 패
			message( "<#00ffff>|".."@91000507")
		else
			insert_item( 2013319, 1 ) -- 승
			message( "<#00ffff>|".."@91000496")
		end
	end
	
	if ID == 2013326 then -- 4강
		local rand = math.random( 1, 2)
		if rand == 1 then
			insert_item( 2013314, 1 ) -- 패
			message( "<#00ffff>|".."@91000509")
		else
			insert_item( 2013320, 1 ) -- 승
			message( "<#00ffff>|".."@91000498")
		end
	end
	
	if ID == 2013334 then -- 3위 트로피
		local select, item_name, ritem
				
		insert_item(1000548,1)
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@11000548") )
		insert_item(1000549,1)
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@11000549") )		
		
		pboxes = {
					50,		2011569,		--	라펠즈 컵 기념 반지 7일
					80,		2011570,		--	라펠즈 컵 기념 반지 14일
					95,		2011571,		--	라펠즈 컵 기념 반지 28일
					99,		2011572,		--	라펠즈 컵 기념 반지 50일
					100,	2011573,		--	라펠즈 컵 기념 반지 100일
				}		
		select =  math.random( 0 , 99)
		ritem = select_number_value(select,pboxes)		
		insert_item(ritem,1)
		item_name = ritem + 10000000		
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@"..tostring(item_name)) )		
	end
	
	if ID == 2013335 then -- 2위 트로피
		local select, item_name, ritem
		
		insert_item(1000550,1)
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@11000550") )
		insert_item(1000551,1)
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@11000551") )	
		
		pboxes = {
					55,		2011570,		--	라펠즈 컵 기념 반지 14일
					85,		2011571,		--	라펠즈 컵 기념 반지 28일
					95,		2011572,		--	라펠즈 컵 기념 반지 50일
					100,	2011573,		--	라펠즈 컵 기념 반지 100일
				}
		select =  math.random( 0 , 99)
		ritem = select_number_value(select,pboxes)		
		insert_item(ritem,1)
		item_name = ritem + 10000000		
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@"..tostring(item_name)) )
	end
	
	if ID == 2013336 then -- 1위 트로피
		local select, item_name, ritem
		
		insert_item(1000552,1)
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@11000552") )
		insert_item(1000553,1)
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@11000553") )	
		
		pboxes = {
					50,		2011571,		--	라펠즈 컵 기념 반지 28일
					80,		2011572,		--	라펠즈 컵 기념 반지 50일
					100,	2011573,		--	라펠즈 컵 기념 반지 100일
				}
		select =  math.random( 0 , 99)
		ritem = select_number_value(select,pboxes)		
		insert_item(ritem,1)
		item_name = ritem + 10000000
		message( "<#00ffff>|" .. sconv( "@253", "#@item_name@#", "@"..tostring(item_name)) )		
	end		
end
function on_use_item_by_program_team( ID, p_handle, target, t_handle, target_id, target_creature )
	--강철 조각 조합
	if ID == 1100101 or ID == 1100102 then
		local create_count = math.floor( find_item( ID ) / 10 )
		delete_item( get_item_handle( ID ), create_count * 10 )
		insert_item( ID + 1, create_count )
	end
	
	--고대의 강철 조합
	if ID == 1100116 then
		local create_count = math.floor( find_item( ID ) / 100 )
		delete_item( get_item_handle( ID ), create_count * 100 )
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

function Increase_Exp_Jp(ID, Exp, Jp)
	if add_exp_jp( Exp, Jp , false, false ) == 1 then
		delete_item( get_item_handle(ID), 1 )
	end
end

-- 9.4 배 탑승, 섬, 마을 귀환 스크립트
function on_forgotten_island(mod_min) -- 잊혀진 신들의 섬 내에서 스크립트
	if mod_min >= 23 and mod_min <= 27 then					
		private_notice( sconv( "@1173", "#@min@#", 28-mod_min) )				-- 1~10분 후 세퍼드호로 귀환합니다.
	elseif mod_min == 28 then 					
		warp( 174925 + math.random(0,20), 151641 + math.random(0,20) )				-- 배위로 워프
		private_notice( sconv( "@1171", "#@min@#", 1) )				-- 분 후 배가 출항합니다.
	elseif mod_min == 29 then -- 마을로 귀환					
		warp( 155396 + math.random(0,10), 149683 + math.random(0,10) )				-- 항구로 워프
	elseif mod_min >= 53 and mod_min <= 57 then					
		private_notice( sconv( "@1173", "#@min@#", 58-mod_min) )				-- 1~5분 후 세퍼드호로 귀환합니다.
	elseif mod_min == 58 then 					
		warp( 174925 + math.random(0,20), 151641 + math.random(0,20) )				-- 배위로 워프
		private_notice( sconv( "@1171", "#@min@#", 1) )				-- 분 후 배가 출항합니다.
	elseif mod_min == 59 then -- 마을로 귀환					
		warp( 155396 + math.random(0,10), 149683 + math.random(0,10) )				-- 항구로 워프
	end
	
	--[[ if mod_min >= 45 and mod_min <= 54 then				
		private_notice( sconv( "@1173", "#@min@#", 55-mod_min) ) -- 몇 분 후 세퍼드호로 귀환합니다.	
	elseif mod_min == 55 then -- 강제 배 탑승	
		warp( 174925 + math.random(0,20), 151641 + math.random(0,20) )			
		private_notice( sconv( "@1171", "#@min@#", 1) ) -- 분 후 배가 출항합니다.			
	elseif mod_min == 56 then -- 마을로 귀환				
		warp( 155396 + math.random(0,10), 149683 + math.random(0,10) )			
	end ]]				
end					
					
function on_forgotten_port(mod_min)	-- 유적도시 배타기 전 공지				
	--[[ if mod_min >= 50 and mod_min <= 59 then				
		private_notice( sconv( "@1171", "#@min@#", 60-mod_min) ) -- 분 후 배가 출항합니다.
	elseif mod_min >= 0 and mod_min <= 10 then
		private_notice( "@1174") -- 탑승 시 곧 배가 출항합니다..		
	end ]]
	
	if mod_min >= 20 and mod_min <= 29 then			
		private_notice( sconv( "@1171", "#@min@#", 30-mod_min) ) -- 분 후 배가 출항합니다.		
	elseif mod_min >= 30 and mod_min <= 35 then			
		private_notice( "@1174") -- 탑승 시 곧 배가 출항합니다..		
	elseif mod_min >= 50 and mod_min <= 59 then			
		private_notice( sconv( "@1171", "#@min@#", 60-mod_min) ) -- 분 후 배가 출항합니다.		
	elseif mod_min >= 0 and mod_min <= 5 then			
		private_notice( "@1174") -- 탑승 시 곧 배가 출항합니다..		
	end							
end					
					
function on_forgotten_ship(mod_min) -- 유적도시에서 잊혀진 신들의 섬으로
	--[[ if mod_min >= 0 and mod_min <= 10 and find_item( 2016091 ) >= 1 then
			set_flag( "onboard_time", get_os_time()-get_minute())				
			warp( 174816 + math.random(0,10), 151777 + math.random(0,10) )
			delete_item( get_item_handle( 2016091 ), 1 )
	end ]]
	
	if mod_min >= 0 and mod_min <= 5 and find_item( 2016091 ) >= 1 then					
		set_flag( "onboard_time", get_os_time()-get_minute())				
		warp( 174816 + math.random(0,10), 151777 + math.random(0,10) )				
		delete_item( get_item_handle( 2016091 ), 1 )				
	elseif mod_min >= 30 and mod_min <= 35 and find_item( 2016091 ) >= 1 then					
		set_flag( "onboard_time", get_os_time()-get_minute())				
		warp( 174816 + math.random(0,10), 151777 + math.random(0,10) )				
		delete_item( get_item_handle( 2016091 ), 1 )				
	end
end

function Ru_Newyear_Event()
	local pos_80 =	{ -- x,			y,
						120090	,	57239	,								-- 80렙 1
						120926	,	138804	,								-- 80렙 2
						156426	,	77241	,								-- 80렙 3
	}
	
	local pos_120 =	{ -- x,			y,
						141667	,	102507	,								-- 120렙 1
						142376	,	110134	,								-- 120렙 2
						130769	,	109848	,								-- 120렙 3
	}
	
	local pos_160 =	{ -- x,			y,					
						147312	,	139929	,								-- 160렙 1
						139981	,	148062	,								-- 160렙 2
						134267	,	157338	,								-- 160렙 3
	}
	
	local pos_200 =	{ -- x,			y,
						86865	,	128412	,								-- 200렙 1
						100559	,	135549	,								-- 200렙 2
						103423	,	120819	,								-- 200렙 3					
	}
	
	local event_snowman80 = 10 - get_global_variable( "event_snoman80" )
	local event_snowman120 = 10 - get_global_variable( "event_snoman120" )
	local event_snowman160 = 10 - get_global_variable( "event_snoman160" )
	local event_snowman200 = 10 - get_global_variable( "event_snoman200" )
	
	local random_pos = 2*math.random(1,3)-1
	
	for i = 1, event_snowman80 do
		add_npc( pos_80[random_pos]  + math.random( -50, 50), pos_80[random_pos+1] + math.random( -50, 50), 22000181,  1 )
		--notice( "몬스터 소환1 "..i.." "..random_pos)
	end
	
	random_pos = 2*math.random(1,3)-1
	for i = 1, event_snowman120 do
		add_npc( pos_120[random_pos]  + math.random( -50, 50), pos_120[random_pos+1] + math.random( -50, 50), 22000182,  1 )
		--notice( "몬스터 소환2 "..i.." "..random_pos)
	end
	
	random_pos = 2*math.random(1,3)-1
	for i = 1, event_snowman160 do
		add_npc( pos_160[random_pos]  + math.random( -50, 50), pos_160[random_pos+1] + math.random( -50, 50), 22000183,  1 )
		--notice( "몬스터 소환3 "..i.." "..random_pos)
	end
	
	random_pos = 2*math.random(1,3)-1
	for i = 1, event_snowman200 do
		add_npc( pos_200[random_pos]  + math.random( -50, 50), pos_200[random_pos+1] + math.random( -50, 50), 22000184,  1 )
		--notice( "몬스터 소환4 "..i.." "..random_pos)
	end
	
	set_global_variable( "event_snoman80", 10 )
	set_global_variable( "event_snoman120", 10 )
	set_global_variable( "event_snoman160", 10 )
	set_global_variable( "event_snoman200", 10 )
	
	notice( "@90610332")
	
end

function RU_Newyear_monster(level)
	if level == 80 then
		set_global_variable("event_snoman80", get_global_variable("event_snoman80")-1)
	elseif level == 120 then
		set_global_variable("event_snoman120", get_global_variable("event_snoman120")-1)
	elseif level == 160 then
		set_global_variable("event_snoman160", get_global_variable("event_snoman160")-1)
	elseif level == 200 then
		set_global_variable("event_snoman200", get_global_variable("event_snoman200")-1)
	end
end




function add_ap(point)

	local new_ap = get_value( 'ap' ) + point
	set_value( "ap", new_ap )
	cprint( "@2356 | + " ..  tostring(point)  )  --2356	아레나 포인트 + 1000
	
end


function inc_item(pitem, enhance, popt)
	local cnt = table.getn(pitem)
	for i = 1, cnt do
	    local item_handle = insert_item( pitem[i], 1 )
		identify_item( item_handle, popt[i] )

		if get_item_enhance( item_handle ) == 0 then
			set_item_enhance( item_handle, enhance )
		end
	
		if get_item_level( item_handle) == 1 then
			set_item_level( item_handle, 10 )
		end
	end
	return
end 

function inc_item2(pitem, enhance)

    local item_handle = insert_item( pitem, 1 )
	--identify_item( item_handle, 100 )
	if get_item_enhance( item_handle ) == 0 then
		set_item_enhance( item_handle, enhance )
	end
	
	if get_item_level( item_handle) == 1 then
		set_item_level( item_handle, 10 )
	end
	return
end 


function inc_armor1(enhance)
	local popt1  = {	7,5,6,	1,2,3,4	}
	local pitem  = {	601100355,					-- helm	
				601100356,					-- gloves	
				601100357,					-- boots
				601100358, 601100359, 601100360, 601100361 }	-- body
	inc_item(pitem,enhance,popt1)
end

function inc_armor2( enhance)
	local popt1  = {	54,54,54,54,	55,55,55,55,	50,51,52,53	}
	local pitem  = {	700000850, 700000851, 700000852, 700000853,	-- gloves
				700000854, 700000855, 700000856, 700000857,	-- boots
				700000846, 700000847, 700000848, 700000849 }	-- body
				
	inc_item(pitem,enhance,popt1)
end


function inc_armor3( enhance)
	local popt1  = {	56,56,56,56,	54,54,54,54,  55,55,55,55,	50,51,52,52,52,52,53,53,53,53,}	-- need to make new option for all job	
	--local popt1  = {	56,56,56,56,	54,54,54,54,  55,55,55,55,	50,51,52,53,}	-- need to make new option for all job
	-- local pitem  = {	601100280,	601100424,	601100428, 	601100432,	-- helm	
				-- 601100281,	601100425,	601100429,	601100433,	-- gloves
				-- 601100282,	601100426,	601100430,	601100434,	-- boots
				-- 601100283,	601100427,	601100431,	601100435 }	-- body				
	local pitem  = {	601100280,	601100424,	601100449, 	601100456,	-- helm	
				 601100281,	601100425,	601100450,	601100457,	-- gloves
				 601100282,	601100426,	601100451,	601100458,	-- boots
				 601100283,	601100427,	601100452,	601100453,	601100454,	601100455,	601100459,	601100460,	601100461,	601100462 }	-- body				


	inc_item(pitem,enhance,popt1)
end

function inc_weapon1( enhance)
	local pitem = {3100001,3100002,3100003,3100004,3100005,3100006,3100007,3100008,3100009,3100010,3100011,3100012,3100013,3100014}
	local popt1  = {101,101, 101,106,101,101,104,102,107,107,105,102,103,105}
	inc_item(pitem,enhance,popt1)
end

function inc_weapon2( enhance)
	local pitem = {3100020,3100021,3100022,3100023,3100024,3100025,3100026,3100027,3100028,3100029,3100030,3100031 }
	local popt1  = {101,101, 101,106,101,101,104,102,107,107,105,102}
	inc_item(pitem,enhance,popt1)
end

function inc_acc1( enhance_value)
	local popt1 = {108,109,110,111,111}
	local pitem = {3100015,3100016,3100017,3100018,3100019}
	inc_item(pitem,enhance,popt1)
end

function inc_acc2( enhance_value, opt)
	local popt1 = {114,115}
	local pitem = {3100032,3100033}
	inc_item(pitem,enhance,popt1)
end

function inc_cash()

	insert_item(910110,10)		-- 시크루트 90일 (사용형)
	insert_item(601100231,10)	-- 데바
	insert_item(950117,10)		-- 전능의 조각
	insert_item(900017,10)		-- 성장의 포션 엑셀런트
	insert_item(900000,10)		-- 스태미너

	insert_item(960123,10)		-- 성별변경	
	insert_item(601100284,10)	-- 호문쿨루스의 심장
	insert_item(910005,10)		-- 대모요정의병
	insert_item(960124,10)		-- 축복받은 재생의 가루
	insert_item(710003,10)		-- 소울 오비터
	insert_item(930040,10)		-- 이름변경
	insert_item(601100287,10)	-- 줍기펫
	insert_item(960019,10)		-- 테이밍 거울
	insert_item(491008,10)		-- 테이밍 주문서
	insert_item(930070,10)		-- 깃털
	insert_item(910077,10)		-- 모두의 축복
	insert_item(910078,10)		-- 모두의 축복
	insert_item(910079,10)		-- 모두의 축복

	insert_item(910079,10)		-- 고대큐브
	insert_item(700256,10)		-- 고대큐브
	insert_item(700402,10)		-- 고대큐브
	insert_item(491005,10)		-- 장인의 망치

	insert_item(491003,10)		-- 벨트강화
	insert_item(950111,10)		-- 목걸이 강화
	insert_item(3620137,10)		-- 목걸이 강화

	insert_item(490002,1)		-- 무한가방 1개
	insert_item(3800102,1)		-- 달인의 배낭 1개

	insert_item(800004,1)		-- 펫진화석
	insert_item(2902161,10)		-- 크래커
	insert_item(710006,10)		-- 농장
	insert_item(1000535,100)	-- 마드리갈 주화

	insert_item(2012787,100)	-- 피물약
	insert_item(2012788,100)	-- 마나물약
end