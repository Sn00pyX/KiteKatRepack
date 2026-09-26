-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_CreatureSetup"
end

   -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.

 
   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================

function NPC_CreatureSetup_Deva_init()
	cprint( "!테이머 안다리엘 가동" )
	set_npc_name(  "@90100100"  )

end
 

function NPC_CreatureSetup_Deva_contact()
 
	-- 다이얼로그 출력
 	dlg_title( "@90100101" )
	dlg_text( "@90100102" )

	-- 국가 코드 읽어옴
	local state_code = get_local_info() 	
	-- 얻어온 국가 코드가 한국(1)이라면
	if state_code == 1 or state_code == 2 then

		-- 크리처 진화에 관한 설명.
		dlg_menu( "@90010065", 'NPC_Description_Of_Evolution()' )
		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
		-- 크리처 각인치환
		dlg_menu( "@90010146", 'Creature_name_change_Menu()' )
	else
	
		-- 크리처 진화에 관한 설명.
		dlg_menu( "@90010065", 'NPC_Description_Of_Evolution()' )
		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
		-- 크리처 스킬 초기화
		-- dlg_menu( "@90100124", 'reset_Creature_skill()' )

	end	
	
	local qstart_text = get_value( "level" )
	local quest_progress1 =  get_quest_progress(2008)
	local quest_progress100 =  get_quest_progress(2000)

	-- 미래를 예언하는 소녀
	if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
	dlg_menu( "@90999617", "quest_rumor11()" )
	end
	
	-- 광신도 암살자
	if quest_progress1 == 255 then
	dlg_menu( "@90999842", "quest_witcharmy1()" )
	end
			
	dlg_menu( "@90010001", '' )
	dlg_show()
 
end

-- 세부대화 안다라엘
function quest_rumor11()
	-- 다이얼로그 출력
	dlg_title( "@90100101" )

	dlg_text( "@90999618" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_8()" )
			
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 
function quest_rumor_a_8()
	-- 다이얼로그 출력
	dlg_title( "@90100101" )

	dlg_text( "@90999622" )
	
	-- 세부대화 1-2, 소녀에 관한 신상정보
	dlg_menu( "@90999625", "quest_rumor_b_8()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 
function quest_rumor_b_8()
	-- 다이얼로그 출력
	dlg_title( "@90100101" )

	dlg_text( "@90999626" )
	
	-- 세부대화 1-3, 소녀에 관한 정보
	dlg_menu( "@90999627", "quest_rumor_c_8()" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 레샤에 관한 정보를 준다.
function quest_rumor_c_8()
	-- 다이얼로그 출력
	dlg_title( "@90100101" )

	-- 세부대화 1-4, 소녀에 관한 위치정보
	dlg_text( "@90999629" )
	
	dlg_menu( "@90010002", '' ) 			
	dlg_show()

end


--===========================================================
-- 크리처 관리 메뉴 대화 상자 - 데바
--===========================================================
function Creature_Management_Deva()

 	dlg_title( "@90100101" )
	dlg_text_without_quest_menu( "@90100108" )

	local i, handle, lv, text, command, temp_txt
 
	-- 0번에서 5번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 5 do
 
		-- 핸들 초기화
		handle = 0

		-- 장착한 소환수 핸들 얻어옴
		handle = get_creature_handle( i )
		-- 만약 장착되어 있다면
		if handle ~= 0 and handle ~= nil then
 
			-- 소환수의 레벨 얻어옴.
			lv = get_creature_value( handle, "level" )
					
			--##시작
			-- 해당 소환수의 이름, Lv을 텍스트로 구성
            text = sconv("@90010009", "#@creature_name@#",tostring(get_creature_value( handle, "name" )) ,"#@creature_level@#",tostring(lv))
            --##끝 #@creature_name@# Lv #@creature_level@#
 
			-- 소환수 이름을 메뉴에 추가
			command = 'Creature_Care_Deva( ' .. handle .. ' )'
 			
			dlg_menu( text, command )

		end
 
	end

	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

--분화된 관리메뉴
function Creature_Care_Deva( handle )
	
	local lv, text, text2, HP, MaxHP, MP, MaxMP, SP, MaxSP
	local Stemp_H, Stemp_M, Stemp_S
	local cost_R, cost_A, cost_H, cost_M, cost_S, cost_T

	dlg_title( "@90100101" )
	
	-- 크리처 상태 <BR> 크리처이름 LvXX
	lv = get_creature_value( handle, "level" )
	
	--##시작
    
	-- 상태 얻어 오기 HP, MP, SP
	HP = get_creature_value( handle, "hp" )
	MaxHP = get_creature_value( handle, "max_hp" )
	
	MP = get_creature_value( handle, "mp" )
	MaxMP = get_creature_value( handle, "max_mp" )
	
	--##시작
	-- 상태 표시하기 
	Stemp_H = math.floor( HP / MaxHP * 100 )
	Stemp_M = math.floor( MP / MaxMP * 100 )

	-- HP 가 0 이면 사망으로 표시 아니면 %로 현재상태 표시
	if HP == 0 then
		text2 = sconv("@90010019" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
       --## <BR>HP #@creature_HP@#/#@creature_MaxHP@# 사망<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %

	elseif HP ~= 0 then
		text2 = sconv("@90010018" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@percent_HP@#", tostring(Stemp_H),"#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
        --##끝 <BR>HP #@creature_HP@#/#@creature_MaxHP@# #@percent_HP@# %<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %
		
	end
    
	-- 텍스트 창 구성 -> 선택한 크리처<BR>이름 LV HP MP SP
 	dlg_text_without_quest_menu( text2 )

 	-- 치료비 계산 (100 - 현재%) * 금액상수 * (레벨 + 레벨보정) / 기울기 * 치료종목상수
 	-- 현재% 가 0 일 경우 nil이라고 에러남. 그래서 체크해서 nil일 경우 본래 의도한 값인 
 	-- 100으로 그냥 수식에 박아 넣음.
 	if Stemp_H == nil then
 		cost_H = math.floor( 100 * 1 * (lv + 4) / 10 * 1 )
 	else
 		cost_H = math.floor( (100 - Stemp_H) * 1 * (lv + 4) / 10 * 1 )
	end
 	if Stemp_M == nil then
 		cost_M = math.floor( 100 * 1 * (lv + 4) / 10 * 3 )
 	else
	 	cost_M = math.floor( (100 - Stemp_M) * 1 * (lv + 4) / 10 * 3 )
	end

	cost_A = cost_H + cost_M

 	-- 최소 부활 가격(최종가격은 HP30% 높이는 값을 포함해야함)
 	cost_R = math.floor( (100) * 1 * (lv + 4) / 10 * 3 )
 	-- 최소 부활 가격(최종가격)
 	cost_T = cost_R + math.floor( (30) * 1 * (lv + 4) / 10 * 1 )
 	
 	--##시작	
 	-- HP 가 0 이면 30% 부활과 완전 부활 메뉴
	if HP == 0 then
		text = sconv("@90010023", "#@cost@#", tostring(cost_T) )
         --## 부활 (HP30%회복)  #@cost@#RP

		dlg_menu( text, 'Creature_Care_RecoverEx_HP_Deva(' .. handle .. ',' .. cost_T .. ',' .. 30 .. ')' )
        text = sconv("@90010033", "#@cost@#", tostring(cost_R + cost_A) )
        --## 부활 (HP, MP 완전회복)  #@cost@#RP

		dlg_menu( text, 'Creature_Care_Recover_ALL_Deva(' .. handle .. ',' .. ( cost_R + cost_A ) .. ')' )
	-- HP 가 0 이 아니면 완전치 않은 것들 종류별로 체크해서 메뉴로 띄움
	elseif HP ~= 0 then

		if HP < MaxHP or MP < MaxMP then
			text = sconv("@90010024", "#@cost@#", tostring( cost_A ) )
            --##모두 회복 #@cost@#RP

			dlg_menu( text,	'Creature_Care_Recover_ALL_Deva(' .. handle .. ',' .. cost_A .. ')' )
		end
		if HP > 0 and HP < MaxHP then
			text = sconv("@90010025", "#@cost@#", tostring( cost_H ) )
            --## HP 회복 #@cost@#RP

			dlg_menu( text,	'Creature_Care_Recover_HP_Deva(' .. handle .. ',' .. cost_H .. ')' )
		end
		if MP >= 0 and MP < MaxMP then
			text = sconv("@90010026", "#@cost@#", tostring( cost_M ) )
            --##끝 MP 회복 #@cost@#RP

			dlg_menu( text,	'Creature_Care_Recover_MP_Deva(' .. handle .. ',' .. cost_M .. ')' )
		end
	
	end

	dlg_menu( "@90010003", 'Creature_Management_Deva()' )
	dlg_menu( "@90010001", '' )
 	dlg_show()

end

-- 정해진 비율 만큼만 HP를 치료하는 함수 ( 핸들, 가격, 치료할비율)
function Creature_Care_RecoverEx_HP_Deva( handle, cost, ratio )

	local Max_HP, Target_HP, temp, HP
	
	Max_HP = get_creature_value( handle, "max_hp" )
	
 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90100101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Deva()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end

 	dlg_title( "@90100101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Deva()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	Target_HP = math.floor(Max_HP * ratio / 100)
	set_creature_value( handle, "hp", Target_HP )

	message( "@90010031" )

end

function Creature_Care_Recover_HP_Deva( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90100101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Deva()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90100101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Deva()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()


	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_MP_Deva( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90100101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Deva()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90100101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Deva()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_SP_Deva( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90100101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Deva()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90100101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Deva()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
--sp관련주석처리	set_value( "gold", gold - cost )
	-- 치료시켜 주기
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_ALL_Deva( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90100101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Deva()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90100101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Deva()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end


--===========================================================
-- 크리처 카드 받기 펑션
--===========================================================
function CreatureCard_Give_Deva()

   -- 카드 받았는지 플래그 받아오기 (받았으면 1, 아니면 NULL)
   local q_flag = get_flag( "q1" )


   -- 직업 & 레벨 & 플래그 체크
   -- 20랩이 넘은 소환사계열이 아니면 KIN
   if get_value( "level" ) < 20 or get_value ( "job" ) ~= 204 then
		-- 다이얼로그 출력
		dlg_title( "@90100101" )
		dlg_text_without_quest_menu( "@90100105" )
		dlg_menu( "@90010016", '' )
		dlg_show()


	-- 20랩 넘은 소환사라도 한번 받았으면 KIN
	elseif get_value( "level" ) >= 20 and get_value ( "job" ) == 204 and q_flag == 1 then
		-- 다이얼로그 출력
		dlg_title( "@90100101" )
		dlg_text_without_quest_menu( "@90100106" )
		dlg_menu( "@90010016", '' )
		dlg_show()


   -- 한번도 안받은 20랩 소환사
   elseif get_value( "level" ) >= 20 and get_value ( "job" ) == 204 and q_flag ~= 1 then

		-- 아이템 밀어넣기(픽시카드 540005~ 540008)
		insert_item( 540005 + math.random(0,3), 1 )
		-- 플래그 세팅
		set_flag( "q1", "1")

		-- 다이얼로그 출력
		dlg_title( "@90100101" )
		dlg_text_without_quest_menu( "@90100107" )
		dlg_menu( "@90010016", '' )
		dlg_show()

   end

end


   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================

function NPC_CreatureSetup_Asura_init()
	cprint( "!테이머 슈바나츠 가동" )
	set_npc_name(  "@90200100" )

end
 

function NPC_CreatureSetup_Asura_contact()
 
	-- 다이얼로그 출력
 	dlg_title( "@90200101" )
	dlg_text( "@90200102" )

	-- 국가 코드 읽어옴
	local state_code = get_local_info() 	
	-- 얻어온 국가 코드가 한국(1)이라면
	if state_code == 1 or state_code == 2 then

		
		-- 크리처 진화에 관한 설명.
		dlg_menu( "@90010065", 'NPC_Description_Of_Evolution()' )
		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
		-- 크리처 각인치환
		dlg_menu( "@90010146", 'Creature_name_change_Menu()' )
	else
		
		-- 크리처 진화에 관한 설명.
		dlg_menu( "@90010065", 'NPC_Description_Of_Evolution()' )
		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
		-- 크리처 스킬 초기화
		-- dlg_menu( "@90100124", 'reset_Creature_skill()' )

	end	
	dlg_menu( "@90010001", '' )
	dlg_show()
 
end


--===========================================================
-- 크리처 관리 메뉴 대화 상자 - 아수라
--===========================================================
function Creature_Management_Asura()

 	dlg_title( "@90200101" )
	dlg_text_without_quest_menu( "@90200108" )

	local i, handle, lv, text, command, temp_txt
 
	-- 0번에서 5번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 5 do
 
		-- 핸들 초기화
		handle = 0

		-- 장착한 소환수 핸들 얻어옴
		handle = get_creature_handle( i )
		-- 만약 장착되어 있다면
		if handle ~= 0 and handle ~= nil then
 
			-- 소환수의 레벨 얻어옴.
			lv = get_creature_value( handle, "level" )
			
			--##시작
			-- 해당 소환수의 이름, Lv을 텍스트로 구성
			  text = sconv("@90010009", "#@creature_name@#",tostring(get_creature_value( handle, "name" )) ,"#@creature_level@#",tostring(lv))
            --##끝 #@creature_name@# Lv #@creature_level@#


			-- 소환수 이름을 메뉴에 추가
			command = 'Creature_Care_Asura( ' .. handle .. ' )'
 			
			dlg_menu( text, command )

		end
 
	end

	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

--분화된 관리메뉴
function Creature_Care_Asura( handle )
	
	local lv, text, text2, HP, MaxHP, MP, MaxMP, SP, MaxSP
	local Stemp_H, Stemp_M, Stemp_S
	local cost_R, cost_A, cost_H, cost_M, cost_S, cost_T

	dlg_title( "@90200101" )
	
	-- 크리처 상태 <BR> 크리처이름 LvXX
	lv = get_creature_value( handle, "level" )
	
	--##시작
    
	-- 상태 얻어 오기 HP, MP, SP
	HP = get_creature_value( handle, "hp" )
	MaxHP = get_creature_value( handle, "max_hp" )
	
	MP = get_creature_value( handle, "mp" )
	MaxMP = get_creature_value( handle, "max_mp" )
	
	--##시작
	-- 상태 표시하기 
	Stemp_H = math.floor( HP / MaxHP * 100 )
	Stemp_M = math.floor( MP / MaxMP * 100 )

	-- HP 가 0 이면 사망으로 표시 아니면 %로 현재상태 표시
	if HP == 0 then
		text2 = sconv("@90010019" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
       --## <BR>HP #@creature_HP@#/#@creature_MaxHP@# 사망<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %

	elseif HP ~= 0 then
		text2 = sconv("@90010018" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@percent_HP@#", tostring(Stemp_H),"#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
        --##끝 <BR>HP #@creature_HP@#/#@creature_MaxHP@# #@percent_HP@# %<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %
		
	end
    
	-- 텍스트 창 구성 -> 선택한 크리처<BR>이름 LV HP MP SP
 	dlg_text_without_quest_menu( text2 )

 	-- 치료비 계산 (100 - 현재%) * 금액상수 * (레벨 + 레벨보정) / 기울기 * 치료종목상수
 	-- 현재% 가 0 일 경우 nil이라고 에러남. 그래서 체크해서 nil일 경우 본래 의도한 값인 
 	-- 100으로 그냥 수식에 박아 넣음.
 	if Stemp_H == nil then
 		cost_H = math.floor( 100 * 1 * (lv + 4) / 10 * 1 )
 	else
 		cost_H = math.floor( (100 - Stemp_H) * 1 * (lv + 4) / 10 * 1 )
	end
 	if Stemp_M == nil then
 		cost_M = math.floor( 100 * 1 * (lv + 4) / 10 * 3 )
 	else
	 	cost_M = math.floor( (100 - Stemp_M) * 1 * (lv + 4) / 10 * 3 )
	end

	cost_A = cost_H + cost_M

 	-- 최소 부활 가격(최종가격은 HP30% 높이는 값을 포함해야함)
 	cost_R = math.floor( (100) * 1 * (lv + 4) / 10 * 3 )
 	-- 최소 부활 가격(최종가격)
 	cost_T = cost_R + math.floor( (30) * 1 * (lv + 4) / 10 * 1 )
 	
 	--##시작	
 	-- HP 가 0 이면 30% 부활과 완전 부활 메뉴
	if HP == 0 then
		text = sconv("@90010023", "#@cost@#", tostring(cost_T) )
         --## 부활 (HP30%회복)  #@cost@#RP

		dlg_menu( text, 'Creature_Care_RecoverEx_HP_Asura(' .. handle .. ',' .. cost_T .. ',' .. 30 .. ')' )
		text = sconv("@90010033", "#@cost@#", tostring(cost_R + cost_A) )
        --## 부활 (HP30%회복)  #@cost@#RP
        
		dlg_menu( text, 'Creature_Care_Recover_ALL_Asura(' .. handle .. ',' .. ( cost_R + cost_A ) .. ')' )
	-- HP 가 0 이 아니면 완전치 않은 것들 종류별로 체크해서 메뉴로 띄움
	elseif HP ~= 0 then

		if HP < MaxHP or MP < MaxMP then
			text = sconv("@90010024", "#@cost@#", tostring( cost_A ) )
            --##모두 회복 #@cost@#RP

			dlg_menu( text,	'Creature_Care_Recover_ALL_Asura(' .. handle .. ',' .. cost_A .. ')' )
		end
		if HP > 0 and HP < MaxHP then
				text = sconv("@90010025", "#@cost@#", tostring( cost_H ) )
            --## HP 회복 #@cost@#RP
            
			dlg_menu( text,	'Creature_Care_Recover_HP_Asura(' .. handle .. ',' .. cost_H .. ')' )
		end
		if MP >= 0 and MP < MaxMP then
			text = sconv("@90010026", "#@cost@#", tostring( cost_M ) )
            --##끝 MP 회복 #@cost@#RP
            
			dlg_menu( text,	'Creature_Care_Recover_MP_Asura(' .. handle .. ',' .. cost_M .. ')' )
		end
	
	end

	dlg_menu( "@90010003", 'Creature_Management_Asura()' )
	dlg_menu( "@90010001", '' )
 	dlg_show()

end

-- 정해진 비율 만큼만 HP를 치료하는 함수 ( 핸들, 가격, 치료할비율)
function Creature_Care_RecoverEx_HP_Asura( handle, cost, ratio )

	local Max_HP, Target_HP
	
	Max_HP = get_creature_value( handle, "max_hp" )
	
 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90200101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Asura()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end

 	dlg_title( "@90200101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Asura()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()


	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	Target_HP = math.floor(Max_HP * ratio / 100)
	set_creature_value( handle, "hp", Target_HP )
	
	message( "@90010031" )

end

function Creature_Care_Recover_HP_Asura( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90200101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Asura()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90200101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Asura()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()


	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_MP_Asura( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90200101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Asura()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90200101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Asura()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_SP_Asura( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90200101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Asura()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90200101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Asura()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
--sp관련주석처리	set_value( "gold", gold - cost )
	-- 치료시켜 주기
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_ALL_Asura( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90200101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Asura()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90200101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Asura()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end



--============================================================
-- 크리처 카드 받기 펑션
--============================================================
function CreatureCard_Give_Asura()

   -- 카드 받았는지 플래그 받아오기 (받았으면 1, 아니면 NULL)
   local q_flag = get_flag( "q1" )

   -- 직업 & 레벨 & 플래그 체크
   -- 20랩이 넘은 소환사계열이 아니면 KIN
   if get_value( "level" ) < 20 or get_value ( "job" ) ~= 304 then
		-- 다이얼로그 출력
		dlg_title( "@90200101" )
		dlg_text_without_quest_menu( "@90200105" )
		dlg_menu( "@90010016", '' )
		dlg_show()


   -- 20랩 넘은 소환사라도 한번 받았으면 KIN
   elseif get_value( "level" ) >= 20 and get_value ( "job" ) == 304 and q_flag == 1 then
		-- 다이얼로그 출력
		dlg_title( "@90200101" )
		dlg_text_without_quest_menu( "@90200106" )
		dlg_menu( "@90010016", '' )
		dlg_show()


   -- 한번도 안받은 20랩 소환사
	elseif get_value( "level" ) >= 20 and get_value ( "job" ) == 304 and q_flag ~= 1 then

		-- 아이템 밀어넣기(픽시카드 540005~ 540008)
		insert_item( 540005 + math.random(0,3), 1 )
		-- 플래그 세팅
		set_flag( "q1", "1")

		-- 다이얼로그 출력
		dlg_title( "@90200101" )
		dlg_text_without_quest_menu( "@90200107" )
		dlg_menu( "@90010016", '' )
		dlg_show()

	end

end


   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================

function NPC_CreatureSetup_Gaia_init()
	cprint( "!가이아 소환수 가동" )
	set_npc_name(  "@90400100"  )

end
 

function NPC_CreatureSetup_Gaia_contact()
 
	-- 다이얼로그 출력
 	dlg_title( "@90400101" )
	dlg_text( "@90400102" )

	-- 국가 코드 읽어옴
	local state_code = get_local_info() 	
	-- 얻어온 국가 코드가 한국(1)이라면
	if state_code == 1 or state_code == 2 then

		
		-- 크리처 진화에 관한 설명.
		dlg_menu( "@90010065", 'NPC_Description_Of_Evolution()' )
		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
		-- 크리처 각인치환
		dlg_menu( "@90010146", 'Creature_name_change_Menu()' )
	else
		
		-- 크리처 진화에 관한 설명.
		dlg_menu( "@90010065", 'NPC_Description_Of_Evolution()' )
		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
				-- 크리처 스킬 초기화
		-- dlg_menu( "@90100124", 'reset_Creature_skill()' )

	end	
	dlg_menu( "@90010001", '' )
	dlg_show()
 
end

--===========================================================
-- 크리처 관리 메뉴 대화 상자 - 가이아
--===========================================================
function Creature_Management_Gaia()

 	dlg_title( "@90400101" )
	dlg_text_without_quest_menu( "@90400108" )

	local i, handle, lv, text, command, temp_txt
 
	-- 0번에서 5번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 5 do
 
		-- 핸들 초기화
		handle = 0

		-- 장착한 소환수 핸들 얻어옴
		handle = get_creature_handle( i )
		-- 만약 장착되어 있다면
		if handle ~= 0 and handle ~= nil then
 
			-- 소환수의 레벨 얻어옴.
			lv = get_creature_value( handle, "level" )
			
			--##시작
			-- 해당 소환수의 이름, Lv을 텍스트로 구성
			  text = sconv("@90010009", "#@creature_name@#",tostring(get_creature_value( handle, "name" )) ,"#@creature_level@#",tostring(lv))
            --##끝 #@creature_name@# Lv #@creature_level@#


			-- 소환수 이름을 메뉴에 추가
			command = 'Creature_Care_Gaia( ' .. handle .. ' )'
 			
			dlg_menu( text, command )

		end
 
	end

	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

--분화된 관리메뉴
function Creature_Care_Gaia( handle )
	
	local lv, text, text2, HP, MaxHP, MP, MaxMP, SP, MaxSP
	local Stemp_H, Stemp_M, Stemp_S
	local cost_R, cost_A, cost_H, cost_M, cost_S, cost_T

	dlg_title( "@90400101" )
	
	-- 크리처 상태 <BR> 크리처이름 LvXX
	lv = get_creature_value( handle, "level" )
	
	--##시작
    
	-- 상태 얻어 오기 HP, MP, SP
	HP = get_creature_value( handle, "hp" )
	MaxHP = get_creature_value( handle, "max_hp" )
	
	MP = get_creature_value( handle, "mp" )
	MaxMP = get_creature_value( handle, "max_mp" )
	
	--##시작
	-- 상태 표시하기 
	Stemp_H = math.floor( HP / MaxHP * 100 )
	Stemp_M = math.floor( MP / MaxMP * 100 )

	-- HP 가 0 이면 사망으로 표시 아니면 %로 현재상태 표시
	if HP == 0 then
		text2 = sconv("@90010019" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
       --## <BR>HP #@creature_HP@#/#@creature_MaxHP@# 사망<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %

	elseif HP ~= 0 then
		text2 = sconv("@90010018" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@percent_HP@#", tostring(Stemp_H),"#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
        --##끝 <BR>HP #@creature_HP@#/#@creature_MaxHP@# #@percent_HP@# %<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %
		
	end
    
	-- 텍스트 창 구성 -> 선택한 크리처<BR>이름 LV HP MP SP
 	dlg_text_without_quest_menu( text2 )

 	-- 치료비 계산 (100 - 현재%) * 금액상수 * (레벨 + 레벨보정) / 기울기 * 치료종목상수
 	-- 현재% 가 0 일 경우 nil이라고 에러남. 그래서 체크해서 nil일 경우 본래 의도한 값인 
 	-- 100으로 그냥 수식에 박아 넣음.
 	if Stemp_H == nil then
 		cost_H = math.floor( 100 * 1 * (lv + 4) / 10 * 1 )
 	else
 		cost_H = math.floor( (100 - Stemp_H) * 1 * (lv + 4) / 10 * 1 )
	end
 	if Stemp_M == nil then
 		cost_M = math.floor( 100 * 1 * (lv + 4) / 10 * 3 )
 	else
	 	cost_M = math.floor( (100 - Stemp_M) * 1 * (lv + 4) / 10 * 3 )
	end

	cost_A = cost_H + cost_M

 	-- 최소 부활 가격(최종가격은 HP30% 높이는 값을 포함해야함)
 	cost_R = math.floor( (100) * 1 * (lv + 4) / 10 * 3 )
 	-- 최소 부활 가격(최종가격)
 	cost_T = cost_R + math.floor( (30) * 1 * (lv + 4) / 10 * 1 )
 	
 	--##시작	
 	-- HP 가 0 이면 30% 부활과 완전 부활 메뉴
	if HP == 0 then
		text = sconv("@90010023", "#@cost@#", tostring(cost_T) )
         --## 부활 (HP30%회복)  #@cost@#RP

		dlg_menu( text, 'Creature_Care_RecoverEx_HP_Gaia(' .. handle .. ',' .. cost_T .. ',' .. 30 .. ')' )
		  text = sconv("@90010033", "#@cost@#", tostring(cost_R + cost_A) )
        --## 부활 (HP30%회복)  #@cost@#RP
        
		dlg_menu( text, 'Creature_Care_Recover_ALL_Gaia(' .. handle .. ',' .. ( cost_R + cost_A ) .. ')' )
	-- HP 가 0 이 아니면 완전치 않은 것들 종류별로 체크해서 메뉴로 띄움
	elseif HP ~= 0 then

		if HP < MaxHP or MP < MaxMP then
			text = sconv("@90010024", "#@cost@#", tostring( cost_A ) )
            --##모두 회복 #@cost@#RP
            
			dlg_menu( text,	'Creature_Care_Recover_ALL_Gaia(' .. handle .. ',' .. cost_A .. ')' )
		end
		if HP > 0 and HP < MaxHP then
			text = sconv("@90010025", "#@cost@#", tostring( cost_H ) )
            --## HP 회복 #@cost@#RP

			dlg_menu( text,	'Creature_Care_Recover_HP_Gaia(' .. handle .. ',' .. cost_H .. ')' )
		end
		if MP >= 0 and MP < MaxMP then
				text = sconv("@90010026", "#@cost@#", tostring( cost_M ) )
            --##끝 MP 회복 #@cost@#RP
            
			dlg_menu( text,	'Creature_Care_Recover_MP_Gaia(' .. handle .. ',' .. cost_M .. ')' )
		end
	
	end

	dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
	dlg_menu( "@90010001", '' )
 	dlg_show()

end

-- 정해진 비율 만큼만 HP를 치료하는 함수 ( 핸들, 가격, 치료할비율)
function Creature_Care_RecoverEx_HP_Gaia( handle, cost, ratio )

	local Max_HP, Target_HP, temp, HP
	
	Max_HP = get_creature_value( handle, "max_hp" )
	
 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90400101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end

 	dlg_title( "@90400101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	Target_HP = math.floor(Max_HP * ratio / 100)
	set_creature_value( handle, "hp", Target_HP )

	message( "@90010031" )

end

function Creature_Care_Recover_HP_Gaia( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90400101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90400101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()


	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_MP_Gaia( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90400101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90400101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_SP_Gaia( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90400101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90400101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
--sp관련주석처리	set_value( "gold", gold - cost )
	-- 치료시켜 주기
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_ALL_Gaia( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90400101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90400101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Gaia()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end


--===========================================================
-- 크리처 카드 받기 펑션
--===========================================================
function CreatureCard_Give_Gaia()

   -- 카드 받았는지 플래그 받아오기 (받았으면 1, 아니면 NULL)
   local q_flag = get_flag( "q1" )


   -- 직업 & 레벨 & 플래그 체크
   -- 20랩이 넘은 소환사계열이 아니면 KIN
   if get_value( "level" ) < 20 or get_value ( "job" ) ~= 204 then
		-- 다이얼로그 출력
		dlg_title( "@90400101" )
		dlg_text_without_quest_menu( "@90400105" )
		dlg_menu( "@90010016", '' )
		dlg_show()


	-- 20랩 넘은 소환사라도 한번 받았으면 KIN
	elseif get_value( "level" ) >= 20 and get_value ( "job" ) == 204 and q_flag == 1 then
		-- 다이얼로그 출력
		dlg_title( "@90400101" )
		dlg_text_without_quest_menu( "@90400106" )
		dlg_menu( "@90010016", '' )
		dlg_show()


   -- 한번도 안받은 20랩 소환사
   elseif get_value( "level" ) >= 20 and get_value ( "job" ) == 204 and q_flag ~= 1 then

		-- 아이템 밀어넣기(픽시카드 540005~ 540008)
		insert_item( 540005 + math.random(0,3), 1 )
		-- 플래그 세팅
		set_flag( "q1", "1")

		-- 다이얼로그 출력
		dlg_title( "@90400101" )
		dlg_text_without_quest_menu( "@90400107" )
		dlg_menu( "@90010016", '' )
		dlg_show()

   end

end

   --============================================================
   --             <<<<<< 도시 유적 측 NPC >>>>>>
   --============================================================

function NPC_CreatureSetup_Ancient_relic_contact()
 
	-- 다이얼로그 출력
 	dlg_title( "@90702901" )
	dlg_text( "@90702902" )

	-- 국가 코드 읽어옴
	local state_code = get_local_info() 	
	-- 얻어온 국가 코드가 한국(1)이라면
	if state_code == 1 or state_code == 2 then

		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
		-- 크리처 각인치환
		dlg_menu( "@90010146", 'Creature_name_change_Menu()' )
	else

		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
				-- 크리처 스킬 초기화
		-- dlg_menu( "@90100124", 'reset_Creature_skill()' )

	end	
	dlg_menu( "@90010001", '' )
	dlg_show()
 
end



   --============================================================
   --             <<<<<< 론도 측 NPC >>>>>>
   --============================================================

function NPC_CreatureSetup_Rondoh_contact()
 
	-- 다이얼로그 출력
 	dlg_title( "@90600101" )
	dlg_text( "@90600102" )

	-- 국가 코드 읽어옴
	local state_code = get_local_info() 	
	-- 얻어온 국가 코드가 한국(1)이라면

	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	if get_quest_progress(3262) == 1 then
			dlg_text_without_quest_menu( "@90600102" )
			dlg_menu( "@80003262", "Quest_Link_3262()" )
			
		if state_code == 1 or state_code == 2 then

		-- 크리처 진화 메뉴
			dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
			dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
		-- 크리처 각인치환
			dlg_menu( "@90010146", 'Creature_name_change_Menu()' )
		else

		-- 크리처 진화 메뉴
			dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
			dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
				-- 크리처 스킬 초기화
		-- dlg_menu( "@90100124", 'reset_Creature_skill()' )

		end
 	
	elseif state_code == 1 or state_code == 2 then

		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
		-- 크리처 각인치환
		dlg_menu( "@90010146", 'Creature_name_change_Menu()' )
	else

		-- 크리처 진화 메뉴
		dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
		
		-- 크리처 관리 메뉴 출력
		dlg_menu( "@90010032", 'Creature_Management_Deva()' )
		
				-- 크리처 스킬 초기화
		-- dlg_menu( "@90100124", 'reset_Creature_skill()' )

	end
			
	dlg_menu( "@90010001", '' )
	dlg_show()
end

function Quest_Link_3262()
	if find_item( 1000113 ) == 0 then
		dlg_title("@90600101")
		dlg_text_without_quest_menu( "@91000976" )
		dlg_menu("@90010002", '')
		dlg_show()

		insert_item( 1000113, 1 )
	else
		dlg_title("@90600102")
		
		dlg_menu( "@90010002", '' ) 
		dlg_show()		
	end
end

--===========================================================
-- 크리처 관리 메뉴 대화 상자 - 론도
--===========================================================
function Creature_Management_Rondoh()

 	dlg_title( "@90600101" )
	dlg_text_without_quest_menu( "@90600108" )

	local i, handle, lv, text, command, temp_txt
 
	-- 0번에서 5번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 5 do
 
		-- 핸들 초기화
		handle = 0

		-- 장착한 소환수 핸들 얻어옴
		handle = get_creature_handle( i )
		-- 만약 장착되어 있다면
		if handle ~= 0 and handle ~= nil then
 
			-- 소환수의 레벨 얻어옴.
			lv = get_creature_value( handle, "level" )
			
			--##시작
			-- 해당 소환수의 이름, Lv을 텍스트로 구성
			  text = sconv("@90010009", "#@creature_name@#",tostring(get_creature_value( handle, "name" )) ,"#@creature_level@#",tostring(lv))
            --##끝 #@creature_name@# Lv #@creature_level@#


			-- 소환수 이름을 메뉴에 추가
			command = 'Creature_Care_Rondoh( ' .. handle .. ' )'
 			
			dlg_menu( text, command )

		end
 
	end

	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

--분화된 관리메뉴
function Creature_Care_Rondoh( handle )
	
	local lv, text, text2, HP, MaxHP, MP, MaxMP, SP, MaxSP
	local Stemp_H, Stemp_M, Stemp_S
	local cost_R, cost_A, cost_H, cost_M, cost_S, cost_T

	dlg_title( "@90600101" )
	
	-- 크리처 상태 <BR> 크리처이름 LvXX
	lv = get_creature_value( handle, "level" )
	
	--##시작
    
	-- 상태 얻어 오기 HP, MP, SP
	HP = get_creature_value( handle, "hp" )
	MaxHP = get_creature_value( handle, "max_hp" )
	
	MP = get_creature_value( handle, "mp" )
	MaxMP = get_creature_value( handle, "max_mp" )
	
	--##시작
	-- 상태 표시하기 
	Stemp_H = math.floor( HP / MaxHP * 100 )
	Stemp_M = math.floor( MP / MaxMP * 100 )

	-- HP 가 0 이면 사망으로 표시 아니면 %로 현재상태 표시
	if HP == 0 then
		text2 = sconv("@90010019" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
       --## <BR>HP #@creature_HP@#/#@creature_MaxHP@# 사망<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %

	elseif HP ~= 0 then
		text2 = sconv("@90010018" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@percent_HP@#", tostring(Stemp_H),"#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
        --##끝 <BR>HP #@creature_HP@#/#@creature_MaxHP@# #@percent_HP@# %<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %
		
	end
    
	-- 텍스트 창 구성 -> 선택한 크리처<BR>이름 LV HP MP SP
 	dlg_text_without_quest_menu( text2 )

 	-- 치료비 계산 (100 - 현재%) * 금액상수 * (레벨 + 레벨보정) / 기울기 * 치료종목상수
 	-- 현재% 가 0 일 경우 nil이라고 에러남. 그래서 체크해서 nil일 경우 본래 의도한 값인 
 	-- 100으로 그냥 수식에 박아 넣음.
 	if Stemp_H == nil then
 		cost_H = math.floor( 100 * 1 * (lv + 4) / 10 * 1 )
 	else
 		cost_H = math.floor( (100 - Stemp_H) * 1 * (lv + 4) / 10 * 1 )
	end
 	if Stemp_M == nil then
 		cost_M = math.floor( 100 * 1 * (lv + 4) / 10 * 3 )
 	else
	 	cost_M = math.floor( (100 - Stemp_M) * 1 * (lv + 4) / 10 * 3 )
	end

	cost_A = cost_H + cost_M

 	-- 최소 부활 가격(최종가격은 HP30% 높이는 값을 포함해야함)
 	cost_R = math.floor( (100) * 1 * (lv + 4) / 10 * 3 )
 	-- 최소 부활 가격(최종가격)
 	cost_T = cost_R + math.floor( (30) * 1 * (lv + 4) / 10 * 1 )
 	
 	--##시작	
 	-- HP 가 0 이면 30% 부활과 완전 부활 메뉴
	if HP == 0 then
		text = sconv("@90010023", "#@cost@#", tostring(cost_T) )
         --## 부활 (HP30%회복)  #@cost@#RP

		dlg_menu( text, 'Creature_Care_RecoverEx_HP_Rondoh(' .. handle .. ',' .. cost_T .. ',' .. 30 .. ')' )
		  text = sconv("@90010033", "#@cost@#", tostring(cost_R + cost_A) )
        --## 부활 (HP30%회복)  #@cost@#RP
        
		dlg_menu( text, 'Creature_Care_Recover_ALL_Rondoh(' .. handle .. ',' .. ( cost_R + cost_A ) .. ')' )
	-- HP 가 0 이 아니면 완전치 않은 것들 종류별로 체크해서 메뉴로 띄움
	elseif HP ~= 0 then

		if HP < MaxHP or MP < MaxMP then
			text = sconv("@90010024", "#@cost@#", tostring( cost_A ) )
            --##모두 회복 #@cost@#RP
            
			dlg_menu( text,	'Creature_Care_Recover_ALL_Rondoh(' .. handle .. ',' .. cost_A .. ')' )
		end
		if HP > 0 and HP < MaxHP then
			text = sconv("@90010025", "#@cost@#", tostring( cost_H ) )
            --## HP 회복 #@cost@#RP

			dlg_menu( text,	'Creature_Care_Recover_HP_Rondoh(' .. handle .. ',' .. cost_H .. ')' )
		end
		if MP >= 0 and MP < MaxMP then
				text = sconv("@90010026", "#@cost@#", tostring( cost_M ) )
            --##끝 MP 회복 #@cost@#RP
            
			dlg_menu( text,	'Creature_Care_Recover_MP_Rondoh(' .. handle .. ',' .. cost_M .. ')' )
		end
	
	end

	dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
	dlg_menu( "@90010001", '' )
 	dlg_show()

end

-- 정해진 비율 만큼만 HP를 치료하는 함수 ( 핸들, 가격, 치료할비율)
function Creature_Care_RecoverEx_HP_Rondoh( handle, cost, ratio )

	local Max_HP, Target_HP, temp, HP
	
	Max_HP = get_creature_value( handle, "max_hp" )
	
 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90600101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end

 	dlg_title( "@90600101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	Target_HP = math.floor(Max_HP * ratio / 100)
	set_creature_value( handle, "hp", Target_HP )

	message( "@90010031" )

end

function Creature_Care_Recover_HP_Rondoh( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90600101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90600101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()


	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_MP_Rondoh( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90600101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90600101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_SP_Rondoh( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90600101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90600101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
--sp관련주석처리	set_value( "gold", gold - cost )
	-- 치료시켜 주기
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_ALL_Rondoh( handle, cost )

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90600101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90600101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Rondoh()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end




   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================

function NPC_CreatureSetup_Secroute_contact()
 
	-- 다이얼로그 출력
 	dlg_title( "@90700101" )
 	
	
	if is_premium() then
		dlg_text_without_quest_menu( "@90700102" )

		-- 국가 코드 읽어옴
		local state_code = get_local_info() 	
		-- 얻어온 국가 코드가 한국(1)이라면
		if state_code == 1 or state_code == 2 then
			-- 크리처 진화 메뉴
			dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
			-- 크리처 관리 메뉴 출력
			dlg_menu( "@90010032", 'Creature_Management_Deva()' )
			-- 크리처 각인치환
			dlg_menu( "@90010146", 'Creature_name_change_Menu()' )
		else
			-- 크리처 진화 메뉴
			dlg_menu( "@90010064", 'NPC_Creature_Evolution_Menu()' )
			-- 크리처 관리 메뉴 출력
			dlg_menu( "@90010032", 'Creature_Management_Deva()' )
			-- 크리처 스킬 초기화. 대충 기능 추가해놓은 것이므로 추후 텍스트ID 확인
			-- dlg_menu( "@90100124", 'reset_Creature_skill()' )
		
		end	
	else
		dlg_text_without_quest_menu( "@90700118" )
		 
	end
	dlg_menu( "@90010001", '' )
	dlg_show()
end
		

--===========================================================
-- 크리처 관리 메뉴 대화 상자 - 시크루트
--===========================================================
function Creature_Management_Secroute()

	if is_premium() == false then
		return false
	end
	
 	dlg_title( "@90700101" )
	dlg_text_without_quest_menu( "@90700108" )

	local i, handle, lv, text, command, temp_txt
 
	-- 0번에서 5번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 5 do
 
		-- 핸들 초기화
		handle = 0

		-- 장착한 소환수 핸들 얻어옴
		handle = get_creature_handle( i )
		-- 만약 장착되어 있다면
		if handle ~= 0 and handle ~= nil then
 
			-- 소환수의 레벨 얻어옴.
			lv = get_creature_value( handle, "level" )
			
			--##시작
			-- 해당 소환수의 이름, Lv을 텍스트로 구성
			  text = sconv("@90010009", "#@creature_name@#",tostring(get_creature_value( handle, "name" )) ,"#@creature_level@#",tostring(lv))
            --##끝 #@creature_name@# Lv #@creature_level@#


			-- 소환수 이름을 메뉴에 추가
			command = 'Creature_Care_Secroute( ' .. handle .. ' )'
 			
			dlg_menu( text, command )

		end
 
	end

	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

--분화된 관리메뉴
function Creature_Care_Secroute( handle )
	
	local lv, text, text2, HP, MaxHP, MP, MaxMP, SP, MaxSP
	local Stemp_H, Stemp_M, Stemp_S
	local cost_R, cost_A, cost_H, cost_M, cost_S, cost_T

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if is_premium() == false then
		return false
	end

	
	dlg_title( "@90700101" )
	
	-- 크리처 상태 <BR> 크리처이름 LvXX
	lv = get_creature_value( handle, "level" )
	
	--##시작
    
	-- 상태 얻어 오기 HP, MP, SP
	HP = get_creature_value( handle, "hp" )
	MaxHP = get_creature_value( handle, "max_hp" )
	
	MP = get_creature_value( handle, "mp" )
	MaxMP = get_creature_value( handle, "max_mp" )
	
	--##시작
	-- 상태 표시하기 
	Stemp_H = math.floor( HP / MaxHP * 100 )
	Stemp_M = math.floor( MP / MaxMP * 100 )

	-- HP 가 0 이면 사망으로 표시 아니면 %로 현재상태 표시
	if HP == 0 then
		text2 = sconv("@90010019" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
       --## <BR>HP #@creature_HP@#/#@creature_MaxHP@# 사망<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %

	elseif HP ~= 0 then
		text2 = sconv("@90010018" ,"#@creature_name@#",get_creature_value( handle, "name" ) ,"#@creature_level@#",tostring(lv), "#@creature_HP@#",tostring(HP),"#@creature_MaxHP@#",tostring(MaxHP), "#@percent_HP@#", tostring(Stemp_H),"#@creature_MP@#",tostring(MP),"#@creature_MaxMP@#",tostring(MaxMP), "#@percent_MP@#",tostring(Stemp_M) )
        --##끝 <BR>HP #@creature_HP@#/#@creature_MaxHP@# #@percent_HP@# %<BR>MP#@creature_MP@# / #@creature_MaxMP@# #@percent_MP@# %
		
	end
    
	-- 텍스트 창 구성 -> 선택한 크리처<BR>이름 LV HP MP SP
 	dlg_text_without_quest_menu( text2 )

 	-- 치료비 계산 (100 - 현재%) * 금액상수 * (레벨 + 레벨보정) / 기울기 * 치료종목상수
 	-- 현재% 가 0 일 경우 nil이라고 에러남. 그래서 체크해서 nil일 경우 본래 의도한 값인 
 	-- 100으로 그냥 수식에 박아 넣음.
 	if Stemp_H == nil then
 		cost_H = math.floor( 100 * 0.85 * (lv + 4) / 10 * 1 )
 	else
 		cost_H = math.floor( (100 - Stemp_H) * 0.85 * (lv + 4) / 10 * 1 )
	end
 	if Stemp_M == nil then
 		cost_M = math.floor( 100 * 0.85 * (lv + 4) / 10 * 3 )
 	else
	 	cost_M = math.floor( (100 - Stemp_M) * 0.85 * (lv + 4) / 10 * 3 )
	end

	cost_A = cost_H + cost_M

 	-- 최소 부활 가격(최종가격은 HP30% 높이는 값을 포함해야함)
 	cost_R = math.floor( (100) * 0.85 * (lv + 4) / 10 * 3 )
 	-- 최소 부활 가격(최종가격)
 	cost_T = cost_R + math.floor( (30) * 0.85 * (lv + 4) / 10 * 1 )
 	
 	--##시작	
 	-- HP 가 0 이면 30% 부활과 완전 부활 메뉴
	if HP == 0 then
		text = sconv("@90010023", "#@cost@#", tostring(cost_T) )
         --## 부활 (HP30%회복)  #@cost@#RP

		dlg_menu( text, 'Creature_Care_RecoverEx_HP_Secroute(' .. handle .. ',' .. cost_T .. ',' .. 30 .. ')' )
		  text = sconv("@90010033", "#@cost@#", tostring(cost_R + cost_A) )
        --## 부활 (HP30%회복)  #@cost@#RP
        
		dlg_menu( text, 'Creature_Care_Recover_ALL_Secroute(' .. handle .. ',' .. ( cost_R + cost_A ) .. ')' )
	-- HP 가 0 이 아니면 완전치 않은 것들 종류별로 체크해서 메뉴로 띄움
	elseif HP ~= 0 then

		if HP < MaxHP or MP < MaxMP then
			text = sconv("@90010024", "#@cost@#", tostring( cost_A ) )
            --##모두 회복 #@cost@#RP
            
			dlg_menu( text,	'Creature_Care_Recover_ALL_Secroute(' .. handle .. ',' .. cost_A .. ')' )
		end
		if HP > 0 and HP < MaxHP then
			text = sconv("@90010025", "#@cost@#", tostring( cost_H ) )
            --## HP 회복 #@cost@#RP

			dlg_menu( text,	'Creature_Care_Recover_HP_Secroute(' .. handle .. ',' .. cost_H .. ')' )
		end
		if MP >= 0 and MP < MaxMP then
				text = sconv("@90010026", "#@cost@#", tostring( cost_M ) )
            --##끝 MP 회복 #@cost@#RP
            
			dlg_menu( text,	'Creature_Care_Recover_MP_Secroute(' .. handle .. ',' .. cost_M .. ')' )
		end
	
	end

	dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
	dlg_menu( "@90010001", '' )
 	dlg_show()

end

-- 정해진 비율 만큼만 HP를 치료하는 함수 ( 핸들, 가격, 치료할비율)
function Creature_Care_RecoverEx_HP_Secroute( handle, cost, ratio )

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if is_premium() == false then
		return false
	end
	
	local Max_HP, Target_HP, temp, HP
	
	Max_HP = get_creature_value( handle, "max_hp" )
	
 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90700101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end

 	dlg_title( "@90700101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	Target_HP = math.floor(Max_HP * ratio / 100)
	set_creature_value( handle, "hp", Target_HP )

	message( "@90010031" )

end

function Creature_Care_Recover_HP_Secroute( handle, cost )

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if is_premium() == false then
		return false
	end

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90700101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90700101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()


	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_MP_Secroute( handle, cost )

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if is_premium() == false then
		return false
	end
 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90700101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90700101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_SP_Secroute( handle, cost )

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if is_premium() == false then
		return false
	end

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90700101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90700101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
--sp관련주석처리	set_value( "gold", gold - cost )
	-- 치료시켜 주기
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end

function Creature_Care_Recover_ALL_Secroute( handle, cost )

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if is_premium() == false then
		return false
	end

 	-- 현재 돈 얻어옴
	local gold = get_value( "gold" )
 
	-- 돈 모자라면 KIN
	if gold < cost then

		dlg_title( "@90700101" )
		dlg_text_without_quest_menu( "@90010005" )

		dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
 	dlg_title( "@90700101" )
	dlg_text_without_quest_menu( "@90010031" )

	dlg_menu( "@90010003", 'Creature_Management_Secroute()' )
	dlg_menu( "@90010002", '' )
 	dlg_show()

	-- 치료비용 차감
	set_value( "gold", gold - cost )
	update_gold_chaos()
	-- 치료시켜 주기
	set_creature_value( handle, "hp", get_creature_value( handle, "max_hp" ) )
	set_creature_value( handle, "mp", get_creature_value( handle, "max_mp" ) )
--sp관련주석처리	set_creature_value( handle, "sp", get_creature_value( handle, "max_sp" ) )
	
	message( "@90010031" )

end

















--=========================================================
-- 종족 공통 함수
--=========================================================










-- 크리처 진화에 대한 설명
function NPC_Description_Of_Evolution()

	--NPC ID를 받아온다.
	local npc_id = get_npc_id()
	
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
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_title("@90600100")
	-- 도시 유적 일 때 (7029	테이머 게스)
	elseif npc_id == 7029 then
		dlg_title("@90702900")	
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_title("@90700100")
	end
	
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_text_without_quest_menu("@90400112")
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_text_without_quest_menu("@90100112")
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_text_without_quest_menu("@90200112")
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_text_without_quest_menu("@90600112")
	-- 도시 유적 일 때 (7029	테이머 게스)
	elseif npc_id == 7029 then
		dlg_text_without_quest_menu("@90702912")
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_text_without_quest_menu("@90700112")
	end

	-- 대화 종료
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end





--==========================================================================
-- 크리처 진화 시키기
--==========================================================================





function NPC_Creature_Evolution_Menu()

	local npc_id, evolution_1_level, evolution_2_level

	--NPC ID를 받아온다.
	npc_id = get_npc_id()
	

	-- 각 종족에 따라 NPC 이름 출력
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_title("@90400100")
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_title("@90100100")
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_title("@90200100")
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_title("@90600100")
	-- 도시유적 일 때 (7029	테이머 게스)
	elseif npc_id == 7029 then
		dlg_title("@90702900")
		
		
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		if is_premium() == false then
			return false
		end

		dlg_title("@90700100")
	end
	
	-- 진화 가능한 크리처를 가지고 있을 때의 대사.(기본적으로 진화 가능한 크리처가 있다고 가정하고 대사를 구성한 뒤 
	-- 진화 가능한 크리처가 없을 경우 아래에서 덮어쓴다.
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_text_without_quest_menu("@90400113")
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_text_without_quest_menu("@90100113")
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_text_without_quest_menu("@90200113")
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_text_without_quest_menu("@90600113")
	-- 도시 유적 일 때 (7029 테이머 게스)
	elseif npc_id == 7029 then
		dlg_text_without_quest_menu("@90702913")		
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_text_without_quest_menu("@90700113")
	end


	-- 1차 진화(기본형 -> 성장형) 가능 레벨은 50레벨 이상
	evolution_1_level = 50

	-- 2차 진화(성장형 -> 진화형) 가능 레벨은 100레벨 이상
	evolution_2_level = 100
	
	local i, handle, lv, text, command, bEvolutionable, NumberOfEvolutionableCreature, creature_evolution_depth
 
	-- 진화 가능 크리처의 수.
	NumberOfEvolutionableCreature = 0
	
	-- 0번에서 5번 슬롯까지 훑어보면서 진화 가능한 소환수를 메뉴에 추가
	for i = 0, 5 do
 
		-- 핸들 초기화
		handle = 0

		-- 장착한 소환수 핸들 얻어옴
		handle = get_creature_handle( i )
		-- 진화 가능 플래그 초기화
		bEvolutionable = 0
		
		-- 만약 장착되어 있다면
		if handle ~= 0 and handle ~= nil then
 
			-- 소환수의 진화형태.
			creature_evolution_depth = get_creature_value( handle, "evolution_depth" )
			
						
			-- 소환수의 레벨 얻어옴.
			lv = get_creature_value( handle, "level" )
			
			-- 기본형일 때,
			if creature_evolution_depth == 1 then
				-- 크리처의 레벨이 진화 가능 레벨 이상이면.
				if lv >= evolution_1_level then
					bEvolutionable = 1
				end
			-- 성장형 일 때,
			elseif creature_evolution_depth == 2 then
				-- 크리처의 레벨이 진화 가능 레벨 이상이면.
				if lv >= evolution_2_level then
					bEvolutionable = 1
				end
			-- 진화형일 때.
			else
				bEvolutionable = 0
			end
					
			-- 진화 가능하면 해당 소환수의 이름, Lv을 텍스트로 구성
			if bEvolutionable == 1 then
			
				text = sconv("@90010009", "#@creature_name@#",tostring(get_creature_value( handle, "name" )) ,"#@creature_level@#",tostring(lv))

				-- 소환수 이름을 메뉴에 추가
				command = 'Creature_Evolution_sub( ' .. handle .. ' )'
 			
				dlg_menu( text, command )
				
				-- 진화 가능 소환수 카운트 증가
				NumberOfEvolutionableCreature = NumberOfEvolutionableCreature + 1
			end

		end
 
	end

	-- 진화 가능한 크리처가 없을 때의 대사.
	if NumberOfEvolutionableCreature ==0 then
		-- 가이아 일 때 (4001	테이머 알레이아)
		if npc_id == 4001 then
			dlg_text_without_quest_menu("@90400114")
		-- 데바 일 때 (1001	테이머 안다리엘)
		elseif npc_id == 1001 then
			dlg_text_without_quest_menu("@90100114")
		-- 아수라 일 때 (2001	서머너 슈바나츠)
		elseif npc_id == 2001 then
			dlg_text_without_quest_menu("@90200114")
		-- 론도 일 때 (6001	테이머 마르스)
		elseif npc_id == 6001 then
			dlg_text_without_quest_menu("@90600114")
		-- 도시 유적 일 때 (7029 테이머 게스)
		elseif npc_id == 7029 then
			dlg_text_without_quest_menu("@90702914")	
		
		-- 시크루트 일 때 (7001	테이머 네벨)
		elseif npc_id == 7001 then
			dlg_text_without_quest_menu("@90700114")
		end
	end

	
	
	-- 대화 종료
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end

function Creature_Evolution_sub( handle )
	
	local npc_id, summon_state, temp_text

	-- NPC ID를 받아온다.
	npc_id = get_npc_id()
	-- 크리처의 소환 상태를 받아 온다.
	summon_state = get_creature_value( handle, "summon_state" )
	-- 소환수의 레벨 얻어옴.
	lv = get_creature_value( handle, "level" )

	-- NPC 이름 출력
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_title("@90400100")
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_title("@90100100")
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_title("@90200100")
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_title("@90600100")
	-- 도시 유적 일 때 (7029 테이머 게스)
	elseif npc_id == 7029 then
		dlg_title("@90702900")
		
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_title("@90700100")
	end

	-- start of "소환 상태에 따른 대사 출력"
	-- 대상 크리처가 역소환 상태일 경우,  대상 크리처가 진화 시킬 크리처가 맞는지 한번 더 확인하는 대사.
	if summon_state ==   0  then

		-- 대사에 집어 넣을 크리처 이름 세팅
		text = sconv("@90010009", "#@creature_name@#",tostring(get_creature_value( handle, "name" )) ,"#@creature_level@#",tostring(lv))

		-- start of "종족에 따른 NPC 대사"
		-- 가이아 일 때 (4001	테이머 알레이아)
		if npc_id == 4001 then
			temp_text = sconv("@90400116", "#@creature_name@#", text)
			dlg_text_without_quest_menu(temp_text)
		-- 데바 일 때 (1001	테이머 안다리엘)
		elseif npc_id == 1001 then
			temp_text = sconv("@90100116", "#@creature_name@#", text)
			dlg_text_without_quest_menu(temp_text)
		-- 아수라 일 때 (2001	서머너 슈바나츠)
		elseif npc_id == 2001 then
			temp_text = sconv("@90200116", "#@creature_name@#", text)
			dlg_text_without_quest_menu(temp_text)
		-- 론도 일 때 (6001	테이머 마르스)
		elseif npc_id == 6001 then
			temp_text = sconv("@90600116", "#@creature_name@#", text)
			dlg_text_without_quest_menu(temp_text)
		-- 도시 유적 일 때 (7029	테이머 마르스)
		elseif npc_id == 7029 then
			temp_text = sconv("@90702916", "#@creature_name@#", text)
			dlg_text_without_quest_menu(temp_text)	
		-- 시크루트 일 때 (7001	테이머 네벨)
		elseif npc_id == 7001 then
			temp_text = sconv("@90700116", "#@creature_name@#", text)
			dlg_text_without_quest_menu(temp_text)
		end -- end of "종족에 따른 NPC 대사"
		

		-- 핸들을 포함한 크리처 진화 함수. (서버에서 제공하는 기능)
		command = 'Creature_Evolution_exe( ' .. handle .. ' )'
 			
		-- 크리처 진화 (실행) 메뉴.
		dlg_menu( "@90010064", command )
		
	-- 대상 크리처가 소환된 상태일 경우. 진화 대상 크리처를 역소환 시켜야 된다는 대사 출력.
	else 

		-- start of "종족에 따른 NPC 대사"
		-- 가이아 일 때 (4001	테이머 알레이아)
		if npc_id == 4001 then
			dlg_text_without_quest_menu("@90400115")
		-- 데바 일 때 (1001	테이머 안다리엘)
		elseif npc_id == 1001 then
			dlg_text_without_quest_menu("@90100115")
		-- 아수라 일 때 (2001	서머너 슈바나츠)
		elseif npc_id == 2001 then
			dlg_text_without_quest_menu("@90200115")
		-- 론도 일 때 (6001	테이머 마르스)
		elseif npc_id == 6001 then
			dlg_text_without_quest_menu("@90600115")
		-- 도시 유적 일 때 (7029	테이머 마르스)
		elseif npc_id == 7029 then
			dlg_text_without_quest_menu("@90702915")
		-- 시크루트 일 때 (7001	테이머 네벨)
		elseif npc_id == 7001 then
			dlg_text_without_quest_menu("@90700115")
		end -- end of "종족에 따른 NPC 대사"
	end -- end of "소환 상태에 따른 대사 출력"
	
	-- 돌아가기 메뉴
	dlg_menu( "@90010003", "NPC_Creature_Evolution_Menu()" )
	-- 대화 종료. 
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end

function Creature_Evolution_exe( handle )

	local npc_id, creature_evolution_depth, lv, id, temp

	-- 소환수의 진화형태를 받아옴.
	creature_evolution_depth = get_creature_value( handle, "evolution_depth" )
	-- 소환수의 ID 얻어옴.
	id = get_creature_value( handle, "job" )
	-- 소환수의 레벨 얻어옴.
	lv = get_creature_value( handle, "level" )
	
	-- 진화 형태에 따라 이전 형태의 ID 및 레벨 저장.
	-- 기본형 일 때.
	if creature_evolution_depth == 1 then
		set_creature_value(handle, "ev_1_ID", id)
		set_creature_value(handle, "ev_1_level", lv)
	-- 성장형 일 때.
	elseif creature_evolution_depth == 2 then
		set_creature_value(handle, "ev_2_ID", id)
		set_creature_value(handle, "ev_2_level", lv)
	-- 여기까지 넘어 오면 뭔가 이상한 상태.
	else
		temp = "Error:CS1497:" .. handle ..":".. id ..":".. creature_evolution_depth ..":".. lv
		message( temp )
	end

	-- 크리처를 진화 시킨다.
	creature_evolution(handle)
	
	-- 소환수의 진화형태에 따라 이전
	-- NPC ID를 받아온다.
	npc_id = get_npc_id()

	-- NPC 이름 출력
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_title("@90400100")
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_title("@90100100")
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_title("@90200100")
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_title("@90600100")
	-- 도시 유적 일 때 (7029	테이머 마르스)
	elseif npc_id == 7029 then
		dlg_title("@90702900")
		
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_title("@90700100")
	end

	-- start of "종족에 따른 NPC 대사"
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_text_without_quest_menu("@90400117")
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_text_without_quest_menu("@90100117")
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_text_without_quest_menu("@90200117")
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_text_without_quest_menu("@90600117")
	-- 도시 유적 일 때 (7029	테이머 마르스)
	elseif npc_id == 7029 then
		dlg_text_without_quest_menu("@90702917")
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_text_without_quest_menu("@90700117")
	end -- end of "종족에 따른 NPC 대사"

	-- 대화 종료. 
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end




--===========================================================
-- 크리처 각인치환 대화 상자 -
--===========================================================

function Creature_name_change_Menu()
 	
	local npc_id, i, handle, lv, text, command, gold, name_change_gold
	
	-- npc id 얻어옴
	npc_id = get_npc_id()
	
	-- NPC 이름 출력
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_title("@90400101")
		dlg_text_without_quest_menu( "@90100120" )  -- 각인치환 설명
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_title("@90100101")
		dlg_text_without_quest_menu( "@90100120" )  -- 각인치환 설명
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_title("@90200101")
		dlg_text_without_quest_menu( "@90100120" )  -- 각인치환 설명
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_title("@90600101")
		dlg_text_without_quest_menu( "@90100120" )  -- 각인치환 설명
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_title("@90700101")
		dlg_text_without_quest_menu( "@90100120" )  -- 각인치환 설명
	-- 도시유적 일 때 (7029	테이머 게스)
	elseif npc_id == 7029 then
		dlg_title("@90702901")
		dlg_text_without_quest_menu( "@90100120" )  -- 각인치환 설명
		
	end

				
	-- 0번에서 5번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 5 do
 		-- 핸들 초기화
		handle = 0

		-- 장착한 소환수 핸들 얻어옴
		handle = get_creature_handle( i )
		-- 만약 장착되어 있다면
		if handle ~= 0 and handle ~= nil then
 
			-- 소환수의 레벨 얻어옴.
			lv = get_creature_value( handle, "level" )
			
			-- 소환수 각인치환 가격
			name_change_gold = lv * 1000
														
			--##시작
			-- 해당 소환수의 종족, Lv, 가격을 텍스트로 구성
            text = sconv("@90010147", "#@creature_name@#", "@"..tostring(get_creature_name_id( handle )), "#@creature_level@#",tostring(lv), "#@name_change_gold@#", tostring(name_change_gold))
            --##끝 #@creature_name@# Lv #@creature_level@#
 
			-- 소환수 이름을 바꿈
			command = 'Creature_name_change_gold( ' .. handle .. ' )'			
			dlg_menu( text, command )
		end
	end
	dlg_menu( "@90010003", 'NPC_CreatureSetup_Deva_contact()' )
	dlg_menu( "@90010002", '' )
	dlg_show()
end


-- 소지금 확인 후 각인치환 시스템 박스 띄움.
function Creature_name_change_gold(handle)
	local npc_id, lv, gold, name_change_gold
	-- npc id 얻어옴
	npc_id = get_npc_id()
	-- 플레이어 소지금 얻어옴
	gold = get_value( "gold" )
	-- 소환수의 레벨 얻어옴.
	lv = get_creature_value( handle, "level" )
	-- 소환수 각인치환 가격
	name_change_gold = lv * 1000

	if gold >= name_change_gold then
		creature_name_change_box(handle)
	else 
		-- NPC 이름 출력
		-- 가이아 일 때 (4001	테이머 알레이아)
		if npc_id == 4001 then
			dlg_title("@90400101")
			dlg_text_without_quest_menu( "@90010005" )  -- 돈이 모자람
		-- 데바 일 때 (1001	테이머 안다리엘)
		elseif npc_id == 1001 then
			dlg_title("@90100101")
			dlg_text_without_quest_menu( "@90010005" )  -- 돈이 모자람
		-- 아수라 일 때 (2001	서머너 슈바나츠)
		elseif npc_id == 2001 then
			dlg_title("@90200101")
			dlg_text_without_quest_menu( "@90010005" )  -- 돈이 모자람
		-- 론도 일 때 (6001	테이머 마르스)
		elseif npc_id == 6001 then
			dlg_title("@90600101")
			dlg_text_without_quest_menu( "@90010005" )  -- 돈이 모자람
		-- 시크루트 일 때 (7001	테이머 네벨)
		elseif npc_id == 7001 then
			dlg_title("@90700101")
			dlg_text_without_quest_menu( "@90010005" )  -- 돈이 모자람
		-- 도시유적 일 때 (7029	테이머 게스)
		elseif npc_id == 7029 then
			dlg_title("@90702901")
			dlg_text_without_quest_menu( "@90010005" )  -- 돈이 모자람
			
			
		end		
	end
	dlg_menu( "@90010002", '' )
	dlg_show()
end


--크리처 이름 바꾸기 성공 한 뒤 저장
function change_summon_name( handle, new_Creature_name)
	local lv, gold, name_change_gold
	
	-- 플레이어 소지금 읽어옴
	gold = get_value( "gold" )
	-- 소환수의 레벨 얻어옴.
	lv = get_creature_value( handle, "level" )
	-- 소환수 각인치환 가격
	name_change_gold = lv * 1000
	-- 플레이어 소지금 저장
	set_value( "gold", gold - name_change_gold )
	update_gold_chaos()
	-- 이름 변경된 크리처 저장함
	change_creature_name( handle, new_Creature_name )
end





--===========================================================
-- 크리처 스킬 초기화 대화 상자. 
--===========================================================

-- 현재 기능 막아둠
	function reset_Creature_skill()

	local npc_id, i, handle, lv, text, command, temp_txt
	
	-- npc id 얻어옴
	npc_id = get_npc_id()
	
	-- NPC 이름 출력
	-- 가이아 일 때 (4001	테이머 알레이아)
	if npc_id == 4001 then
		dlg_title("@90400101")
		dlg_text_without_quest_menu( "@90100125" )  
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_title("@90100101")
		dlg_text_without_quest_menu( "@90100125" )  
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_title("@90200101")
		dlg_text_without_quest_menu( "@90100125" )  
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_title("@90600101")
		dlg_text_without_quest_menu( "@90100125" )  
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_title("@90700101")
		dlg_text_without_quest_menu( "@90100125" )  
	-- 도시유적 일 때 (7029	테이머 게스)
	elseif npc_id == 7029 then
		dlg_title("@90702901")
		dlg_text_without_quest_menu( "@90100125" )  
		
	end


 
	-- 0번에서 5번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 5 do
 
		-- 핸들 초기화
		handle = 0

		-- 장착한 소환수 핸들 얻어옴
		handle = get_creature_handle( i )
		-- 만약 장착되어 있다면
		if handle ~= 0 and handle ~= nil then
 
			-- 소환수의 레벨 얻어옴.
			lv = get_creature_value( handle, "level" )
					
			--##시작
			-- 해당 소환수의 이름, Lv을 텍스트로 구성
            text = sconv("@90010009", "#@creature_name@#",tostring(get_creature_value( handle, "name" )) ,"#@creature_level@#",tostring(lv))
            --##끝 #@creature_name@# Lv #@creature_level@#
 
			-- 소환수 이름을 메뉴에 추가
			command = 'question_skill_reset( ' .. handle .. ' )'
 			
			dlg_menu( text, command )

		end
 
	end
	
	
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

-- 크리처 스킬 초기화할 것인지 문의
	 function question_skill_reset(handle)

	local npc_id, lv, gold, skill_reset_gold
	-- npc id 얻어옴
	npc_id = get_npc_id()
	-- 플레이어 소지금 얻어옴
	gold = get_value( "gold" )
	-- 소환수의 레벨 얻어옴.
	lv = get_creature_value( handle, "level" )
	-- 소환수 스킬 취소 가격
	skill_reset_gold = lv * 5000

			
	if npc_id == 4001 then
		dlg_title("@90400101")
		text = sconv("@90100127", "#@skill_reset_gold_text@#",tostring( skill_reset_gold )) -- 스킬 초기화에는 xx 루피가 필요함.
		dlg_text_without_quest_menu(text)
		
	-- 데바 일 때 (1001	테이머 안다리엘)
	elseif npc_id == 1001 then
		dlg_title("@90100101")
		text = sconv("@90100127", "#@skill_reset_gold_text@#",tostring( skill_reset_gold )) -- 스킬 초기화에는 xx 루피가 필요함.
		dlg_text_without_quest_menu(text)
	-- 아수라 일 때 (2001	서머너 슈바나츠)
	elseif npc_id == 2001 then
		dlg_title("@90200101")
		text = sconv("@90100127", "#@skill_reset_gold_text@#",tostring( skill_reset_gold )) -- 스킬 초기화에는 xx 루피가 필요함.
		dlg_text_without_quest_menu(text)
	-- 론도 일 때 (6001	테이머 마르스)
	elseif npc_id == 6001 then
		dlg_title("@90600101")
		text = sconv("@90100127", "#@skill_reset_gold_text@#",tostring( skill_reset_gold )) -- 스킬 초기화에는 xx 루피가 필요함.
		dlg_text_without_quest_menu(text)
	-- 시크루트 일 때 (7001	테이머 네벨)
	elseif npc_id == 7001 then
		dlg_title("@90700101")
		text = sconv("@90100127", "#@skill_reset_gold_text@#",tostring( skill_reset_gold )) -- 스킬 초기화에는 xx 루피가 필요함.
		dlg_text_without_quest_menu(text)
	-- 도시유적 일 때 (7029	테이머 게스)
	elseif npc_id == 7029 then
		dlg_title("@90702901")
		text = sconv("@90100127", "#@skill_reset_gold_text@#",tostring( skill_reset_gold )) -- 스킬 초기화에는 xx 루피가 필요함.
		dlg_text_without_quest_menu(text)
	end
	
	dlg_menu( "@90010016", 'Creature_skill_reset_success('..handle..')' )
	dlg_menu( "@90010001", 'reset_Creature_skill()' )		

	dlg_show()
	end


	
-- 크리처 스킬 리셋 성공	
	function Creature_skill_reset_success(handle)

	local npc_id, lv, gold, skill_reset_gold
	-- npc id 얻어옴
	npc_id = get_npc_id()
	-- 플레이어 소지금 얻어옴
	gold = get_value( "gold" )
	-- 소환수의 레벨 얻어옴.
	lv = get_creature_value( handle, "level" )
	-- 소환수 스킬 취소 가격
	skill_reset_gold = lv * 5000
	
	if gold >= skill_reset_gold then
	
		-- NPC 이름 출력
		-- 가이아 일 때 (4001 테이머 알레이아)
		if npc_id == 4001 then
			dlg_title("@90400101")
			dlg_text_without_quest_menu( "@90100128" )  -- 성공했습니다.
			-- 플레이어 소지금 저장
			set_value( "gold", gold - skill_reset_gold )
			update_gold_chaos()
			-- 크리처 스킬 취소(!!!!!!!!!!!)
			reset_summon_skill( handle ) 
		-- 데바 일 때 (1001	테이머 안다리엘)
		elseif npc_id == 1001 then
			dlg_title("@90100101")
			dlg_text_without_quest_menu( "@90100128" )  -- 성공했습니다.
			-- 플레이어 소지금 저장
			set_value( "gold", gold - skill_reset_gold )
			update_gold_chaos()
			-- 크리처 스킬 취소(!!!!!!!!!!!)
			reset_summon_skill( handle ) 
		-- 아수라 일 때 (2001	서머너 슈바나츠)
		elseif npc_id == 2001 then
			dlg_title("@90200101")
			dlg_text_without_quest_menu( "@90100128" )  -- 성공했습니다.
			-- 플레이어 소지금 저장
			set_value( "gold", gold - skill_reset_gold )
			update_gold_chaos()
			-- 크리처 스킬 취소(!!!!!!!!!!!)
			reset_summon_skill( handle ) 
		-- 론도 일 때 (6001	테이머 마르스)
		elseif npc_id == 6001 then
			dlg_title("@90600101")
			dlg_text_without_quest_menu( "@90100128" )  -- 성공했습니다.
			-- 플레이어 소지금 저장
			set_value( "gold", gold - skill_reset_gold )
			update_gold_chaos()
			-- 크리처 스킬 취소(!!!!!!!!!!!)
			reset_summon_skill( handle ) 
		-- 시크루트 일 때 (7001	테이머 네벨)
		elseif npc_id == 7001 then
			dlg_title("@90700101")
			dlg_text_without_quest_menu( "@90100128" )  -- 성공했습니다.
			-- 플레이어 소지금 저장
			set_value( "gold", gold - skill_reset_gold )
			update_gold_chaos()
			-- 크리처 스킬 취소(!!!!!!!!!!!)
			reset_summon_skill( handle ) 
		-- 도시유적 일 때 (7029	테이머 게스)
		elseif npc_id == 7029 then
			dlg_title("@90702901")
			dlg_text_without_quest_menu( "@90100128" )  -- 성공했습니다.
	
		-- 플레이어 소지금 저장
		set_value( "gold", gold - skill_reset_gold )
		update_gold_chaos()
			
		-- 크리처 스킬 취소(!!!!!!!!!!!)
		reset_summon_skill( handle ) 
		

		end
				
	else 
		-- NPC 이름 출력
		-- 가이아 일 때 (4001	테이머 알레이아)
		if npc_id == 4001 then
			dlg_title("@90400101")
			dlg_text_without_quest_menu( "@90100126" )  -- 돈이 모자람
		-- 데바 일 때 (1001	테이머 안다리엘)
		elseif npc_id == 1001 then
			dlg_title("@90100101")
			dlg_text_without_quest_menu( "@90100126" )  -- 돈이 모자람
		-- 아수라 일 때 (2001	서머너 슈바나츠)
		elseif npc_id == 2001 then
			dlg_title("@90200101")
			dlg_text_without_quest_menu( "@90100126" )  -- 돈이 모자람
		-- 론도 일 때 (6001	테이머 마르스)
		elseif npc_id == 6001 then
			dlg_title("@90600101")
			dlg_text_without_quest_menu( "@90100126" )  -- 돈이 모자람
		-- 시크루트 일 때 (7001	테이머 네벨)
		elseif npc_id == 7001 then
			dlg_title("@90700101")
			dlg_text_without_quest_menu( "@90100126" )  -- 돈이 모자람
		-- 도시유적 일 때 (7029	테이머 게스)
		elseif npc_id == 7029 then
			dlg_title("@90702901")
			dlg_text_without_quest_menu( "@90100126" )  -- 돈이 모자람
			
			
		end		
	end
	dlg_menu( "@90010003", 'reset_Creature_skill()' )
	dlg_menu( "@90010002", '' )
	dlg_show()
end



