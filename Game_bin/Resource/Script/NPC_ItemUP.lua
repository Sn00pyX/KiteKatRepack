 -- Lua 스크립트 암호화
function get_module_name()
             return "NPC_ItemUP"
end

 -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.

   --============================================================
   --             <<<<<< NPC 공통 >>>>>>
   --============================================================
------------------------------------------------------------------루피를 이용한 내구도 회복 ----

function max_item_durability()
	
	local delete_gold = 0
	for index_item = 0, 198 do -- 돈 검사를 위한 LOOP
		local handle = get_wear_item_handle( index_item )--아이템 착용 부위별로 쫙 훑는다.
		
		if handle ~= 0 then-- 각 착용 부위에 아이템이 있으면
			local e_max = get_max_item_ethereal_durability( handle ) --착용 아이템의 최대 내구도를 가져온다.
			local e_durability = get_item_ethereal_durability( handle ) --착용 아이템의 현재 내구도를 가져온다.
			
			if e_durability ~= 0 then --착용 아이템의 현재 내구도가 0이 아닌 경우에만 
				local item_rank = get_item_rank( handle )
				delete_gold = delete_gold + (math.floor((e_max-e_durability)/10000))*(3^item_rank) -- 최대 내구도 값에서 현재 내구도값을 뺀 값을 10000으로 나누고 소수점을 버린다. 그 값에 3에 아이템랭크만큼의 제곱을 한 값을 곱한 값
			end
		end
	end
	
	local gold = get_value("gold")		
	local text_gold = sconv("@263", "#@gold@#",tostring(delete_gold))
	
	if delete_gold == 0 then
		cprint("@732") -- 수리할 아이템이 없습니다
	elseif gold >= delete_gold then
		set_value("gold", gold-delete_gold) 
		
		for index_item = 0, 94 do -- 수리를 위한 LOOP
			local handle = get_wear_item_handle( index_item )--아이템 착용 부위별로 쫙 훑는다. 
			
			if handle ~= 0 then-- 각 착용 부위에 아이템이 있으면
				local e_durability = get_item_ethereal_durability( handle ) --착용 아이템의 현재 내구도를 가져온다.
			
				if e_durability ~= 0 then --착용 아이템의 현재 내구도가 0이 아닌 경우에만 
					set_item_ethereal_durability( handle, get_max_item_ethereal_durability( handle ) ) -- 착용된 아이템의 내구도를 최대값으로 채워준다.
				end
			end
		end
		
		cprint(text_gold)
		cprint("@690000080")
		
	else
		cprint("@90010008") -- 소지금이 부족할 경우 소지금이 부족하다는 메세지 출력
	end
end

--====================================================================랜덤 옵션 아이템 교환
function random_item_change_menu()
	
	local count = 0
	
	-- 다이얼로그 출력
	local npc_id = get_npc_id()
	
	-- 데바 대장장이 티리엘
	if npc_id == 1003 then
		dlg_title( "@90100301" )
				
	-- 아수라 대장장이 파이론
	elseif npc_id == 2003 then
		dlg_title( "@90200301" )
		
	-- 초보자섬 대장장이 타나엘
	elseif npc_id == 3003 then
		dlg_title( "@90300301" )
	
	-- 가이아 대장장이 미르피유
	elseif npc_id == 4003 then
		dlg_title( "@90400301" )
	
	-- 론도 대장장이 라슈무
	elseif npc_id == 6003 then
		dlg_title( "@90600301" )
	
	-- 시크루트 대장장이 지온
	elseif npc_id == 7003 then
		dlg_title( "@90700301" )
	
	-- 도시유적 대장장이 하뮤
	elseif npc_id == 7031 then
		dlg_title( "@90703101" )
	
	end
	
	-- 아래 목록중에서 바꿀 아이템을 선택해 라는 대사 출력
	dlg_text( "@90606096" )
	
	-- 0번에서 7번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 7 do
		-- 입고있는 아이템 핸들 얻어옴
		handle = get_wear_item_handle( i )
		
		-- 입고 있는 아이템 코드 얻어옴
		item_code = get_item_code( handle )
		
		-- 만약 교환 대상이 되는 아이템을 입고 있다면
		if item_code == 601100300 or item_code == 601100301 or item_code == 601100302 or item_code == 601100303 
		or item_code == 601100304 or item_code == 601100305 or item_code == 601100306 or item_code == 601100307 or item_code == 700000585 then
			count = count + 1
			lv = get_item_level( handle )
			enhance =  get_item_enhance( handle )

			if lv == 0 then
				set_item_level( handle , 1 )
			end
			
			-- 해당 아이템의 강화단계, 아이템명, Lv을 텍스트로 구성
			if enhance ~= 0 then
				-- 강화되어있으면 +x 표시
				text = sconv("@90010010", "#@enhance_level@#",tostring(enhance) ,"#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010010 = + #@enhance_level@# #@item_name@# Lv #@item_Lv@#	

			else
				text = sconv("@90010011", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ), "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010011 = #@item_name@# Lv #@item_Lv@#
			end
			
			-- 아이템 레벨업 스크립트 펑션을 메뉴에 추가
			command = 'random_item_change( ' .. handle .. ' )'
			dlg_menu( text, command )
		end
	end
	if count == 0 then
		-- 바꿔줄 아이템이 없어 라는 대사 출력
		dlg_text( "@90606097" )	
	end	
	
	dlg_menu( "@90010002", '' )
	dlg_show()
		
end

function random_item_change( handle )

	-- 대장장이 메뉴에서 목록에 나온 아이템의 ID를 가져온다.
	local item_code = get_item_code( handle )
	local reward_code = 0
	
	-- 만약 아이템 ID가 item_code와 맞다면 교환 해줄 아이템 ID를 reward_code의 ID로 설정하고  
	if item_code == 601100300 then
		reward_code = 601100354
	
	elseif item_code == 601100301 then
		reward_code = 601100355
	
	elseif item_code == 601100302 then
		reward_code = 601100356
	
	elseif item_code == 601100303 then
		reward_code = 601100357
		
	elseif item_code == 601100304 then
		reward_code = 601100358
	
	elseif item_code == 601100305 then
		reward_code = 601100359
	
	elseif item_code == 601100306 then
		reward_code = 601100360
	
	elseif item_code == 601100307 then
		reward_code = 601100361
		
	elseif item_code == 700000585 then
		reward_code = 700000822
	end
	
	-- 교체된 아이템의 랜덤 옵션 첫번?와 두번? 값들을 불러오고 
	local value_1 = get_item_random_option( handle, 1 )
	local value_2 = get_item_random_option( handle, 2 )
	
	-- 불러온 아이템의 랜덤 옵션 값들이 0 또는 -1 (0 : 값없음, -1 : 시스템 오류 시)이 아니라면 설정된 대로 그 값들을 바꿔줘라.
	if value_1 ~= 0 and value_1 ~= -1 then
		if value_1[1] == 96 and value_1[2] == 512 then
			value_1[3] = value_1[3]*2
			set_item_random_option( handle, 1, value_1[1], value_1[2], value_1[3] )
		end
				
		if value_2[1] == 96 and value_2[2] == 1024 then
			value_2[3] = value_2[3]*2
			set_item_random_option( handle, 2, value_2[1], value_2[2], value_2[3] )
		end
	end	
	
	-- 위에서 설정된 reward_code의 ID로 교체하라.
	change_item_code( handle, reward_code )		
	
	-- 어떤 아이템의 설정이 변경되었는지 시스템 메세지로 알려줘라.
	text = sconv("@690000139", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ))
	cprint( text )
	
	-- 전 단계 메뉴로 돌아가라.
	random_item_change_menu()	
end
	

   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================

function NPC_ItemUP_Deva_init()
	cprint( "!대장장이 티리엘 가동" )
	set_npc_name(  "@90100300"  )
end

function NPC_ItemUP_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90100301" )

	if get_quest_progress(10173) == 1 and find_item( 1100310 ) == 0 then
	dlg_text_without_quest_menu( "@90100302" )
	
		dlg_menu( "@80010173", "Quest_Link_119_1()" )
		
	else

			dlg_text( "@90100302" )
	end
 
 
	local i, handle, lv, text, text2, command, enhance, f, price
	
 
	-- 0번에서 7번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 7 do
 
		-- 입고있는 아이템 핸들 얻어옴
		handle = get_wear_item_handle( i )
 
		-- 만약 입고 있다면
		if handle ~= 0 then
 
			lv = get_item_level( handle )
			enhance =  get_item_enhance( handle )

			if lv == 0 then
				set_item_level( handle , 1 )
			end
			
			price = get_item_price( handle )
			-- 9.4 무기 가격
			if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
				price = 674086
			elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
				price = 539270
			end

			-- ## 시작
			-- 해당 아이템의 강화단계, 아이템명, Lv을 텍스트로 구성
				if enhance ~= 0 then
					-- 강화되어있으면 +x 표시
					text = sconv("@90010010", "#@enhance_level@#",tostring(enhance) ,"#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
					-- ## 끝 @90010010 = + #@enhance_level@# #@item_name@# Lv #@item_Lv@#	

				else
					text = sconv("@90010011", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ), "#@item_Lv@#",tostring(lv))
					-- ## 끝 @90010011 = #@item_name@# Lv #@item_Lv@#
				end
			-- 아이템 레벨업 스크립트 펑션을 메뉴에 추가
			if price > 20 then
			command = 'ItemLevelUp_Deva_check( ' .. handle .. ' )'
			dlg_menu( text, command )
			end

		end
 
	end
	
	dlg_menu( "@90100703", "open_market( 'deva_itemUP_suppert' )" )	
	dlg_menu( "@690000079", "max_item_durability()" )
	dlg_menu( "@90606098", "random_item_change_menu()" )
	dlg_menu( "@90010002", '' )
	dlg_show()
 
end
 
function Quest_Link_119_1()
	if find_item( 1100310 ) == 0 then
		dlg_title("@90100301")

		dlg_text_without_quest_menu( "@90999590" )

		dlg_menu("@90010002", '')
		dlg_show()

		insert_item( 1100310, 1 )
	else
		dlg_title("@90100301")

		dlg_menu( "@90010002", '' ) 
		dlg_show()		
	end
end 

function ItemLevelUp_Deva_check( handle )


	local i, j, k, max_level, rank, lv, price, cost, text

	rank = get_item_rank( handle )
	if rank < 1 then
		rank = 1
	end
	lv = get_item_level( handle )

	-- 랭크가 1이면 max=5레벨, 아니면 10레벨
	if rank == 1 then
		max_level = 5
	else
		max_level = 10
	end



	if lv >= max_level then

		--업글 가능한 레벨을 벗어나므로 무효
	        
		dlg_title( "@90100301" )
		dlg_text( "@90100303" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Deva_contact()' )
		dlg_menu( "@90010002", '' )
		
		dlg_show()
			
		return
	end

	-- 업글 가능한 레벨이므로 가격 계산
	price = get_item_price( handle )
	-- 9.4 무기 가격
	if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
		price = 674086
	elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
		price = 539270
	end
	
	-- price < 20 인 아이템은 기본아이템이므로 업글 불가
	if price < 20 then
		dlg_title( "@90100301" )
		dlg_text( "@90100304" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Deva_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
	
	
	
--============================================================================================
-- 모든 랭크 업그레이드 비용 계산 통합
--============================================================================================
--	if rank == 1 then
		-- 1랭크의 업글비용 계산
--		if lv == 1 then
--			cost = 120
--		else
--			cost = (lv ^ 2) * 100

--		end
		
--	else	
		-- 업글비용 계산

		k = price
		f = {1.35, 0.2, 0.115, 0.092, 0.085, 0.1, 0.1, 0.1}		-- Lv당 증가가격용 팩터 테이블

		for i = 1, lv do

			-- 1랭크일땐 뒤에 1자리를, 2랭크일땐 뒤의 2자리, 3랭크이상일땐 뒤에 세자리를 0으로 맞춘다.
			-- lua 의 배열은 zerobase 가 아니다!!! 중요. C로 적용할 때는 배열 주소를 -1 씩 해줘야 한다.
			-- 예를 들어 아래의 배열은 C에서는 f[rank-1] 이어야 맞고 lua에서는 f[rank] 여야 맞다.		
			if rank == 1 then
				cost = math.floor(k * f[rank] * 0.1) * 10

			elseif rank == 2 then
				cost = math.floor(k * f[rank] * 0.01) * 100
				
			else
				cost = math.floor(k * f[rank] * 0.001) * 1000
			end
			
			k = k + cost
		end
--	end

    --##시작        
	-- 가격 알림
    text = sconv("@90100305", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1)),"#@upgrade_cost@#",tostring(cost))
    --##끝 #@item_name@# Lv #@item_Lv@#을 Lv#@item_up_Lv@# '(/으)로 업그레이드 하는 비용은 #@upgrade_cost@#루피 입니다.<BR>업그레이드 하시겠습니까?'
	dlg_title( "@90100301" )
	dlg_text( text )

	dlg_menu( "@90010004", 'ItemLevelUp_Deva( ' .. handle .. ' , ' .. cost .. ' )' )
	dlg_menu( "@90010003", 'NPC_ItemUP_Deva_contact()' )
	dlg_menu( "@90010002", '' )

	dlg_show()

end


function ItemLevelUp_Deva( handle , cost )
 
		-- 현재 돈 얻어옴
        local gold = get_value( "gold" )
        local result
 
        -- 돈 모자라면 KIN
        if gold < cost then

		dlg_title( "@90100301" )
		dlg_text( "@90010005" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Deva_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()

		return
        end
 
        -- 아이템 레벨 얻어옴
        local lv = get_item_level( handle )
 
        -- 레벨 증가
        result = set_item_level( handle , lv + 1 )
		if result == 0 then
		cprint("해당 무기를 소지하고 있지 않습니다.")
			return
		else
		-- 업글비용 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()
 		
	save()
		
	--##시작 
	-- 시스템 메세지
      message(sconv("@90010006", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1))))
    --##끝 #@item_name@#Lv #@item_Lv@#이  Lv "#@item_up_Lv@#" (/으)로 업그레이드 되었습니다.   
		end	
	--dlg_menu( "@90010003", 'NPC_ItemUP_Deva_contact()' )
	NPC_ItemUP_Deva_contact()	
end


   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================

function NPC_ItemUP_Asura_init()
	cprint( "!대장장이 파이론 가동" )
	set_npc_name(  "@90200300" )
end

function NPC_ItemUP_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90200301" )
	dlg_text( "@90200302" )
 
 
	local i, handle, lv, text, text2, command, enhance
 
	-- 0번에서 7번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 7 do
 
		-- 입고있는 아이템 핸들 얻어옴
		handle = get_wear_item_handle( i )
 
		-- 만약 입고 있다면
		if handle ~= 0 then
 
			lv = get_item_level( handle )
			enhance =  get_item_enhance( handle )
 
			if lv == 0 then
				set_item_level( handle , 1 )
			end
			
			price = get_item_price( handle )
			
			-- 9.4 무기 가격
			if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
				price = 674086
			elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
				price = 539270
			end
			
			-- ## 시작
			-- 해당 아이템의 강화단계, 아이템명, Lv을 텍스트로 구성
			if enhance ~= 0 then
				-- 강화되어있으면 +x 표시
				text = sconv("@90010010", "#@enhance_level@#",tostring(enhance) ,"#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010010 = + #@enhance_level@# #@item_name@# Lv #@item_Lv@#

			else
				text = sconv("@90010011", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010011 = #@item_name@# Lv #@item_Lv@#
			end
			
			-- 아이템 레벨업 스크립트 펑션을 메뉴에 추가
			if price > 20 then
			command = 'ItemLevelUp_Asura_check( ' .. handle .. ' )'
			dlg_menu( text, command )
			end

		end
 
	end
	
	dlg_menu( "@90100703", "open_market( 'Asura_itemUP_suppert' )" )
	dlg_menu( "@690000079", "max_item_durability()" )
	dlg_menu( "@90606098", "random_item_change_menu()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
 

 

function ItemLevelUp_Asura_check( handle )


	local i, j, k, max_level, rank, lv, price, cost, text

	rank = get_item_rank( handle )
	if rank < 1 then
		rank = 1
	end
	lv = get_item_level( handle )

	-- 랭크가 1이면 max=3레벨, 아니면 10레벨
	if rank == 1 then
		max_level = 5
	else
		max_level = 10
	end

	if lv >= max_level then

		--업글 가능한 레벨을 벗어나므로 무효
	        
		dlg_title( "@90200301" )
		dlg_text( "@90200303" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Asura_contact()' )
		dlg_menu( "@90010002", '' )
		
		dlg_show()
			
		return
	end

	-- 업글 가능한 레벨이므로 가격 계산
	price = get_item_price( handle )
	
	if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
		price = 674086
	elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
		price = 539270
	end

	-- price < 20 인 아이템은 기본아이템이므로 업글 불가
	if price < 20 then
		dlg_title( "@90200301" )
		dlg_text( "@90200304" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Asura_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end

	
	
			
	
	
--============================================================================================
-- 모든 랭크 업그레이드 비용 계산 통합
--============================================================================================
--	if rank == 1 then
		-- 1랭크의 업글비용 계산
--		if lv == 1 then
--			cost = 120
--		else
--			cost = (lv ^ 2) * 100

--		end
		
--	else	
		-- 업글비용 계산

		k = price
		f = {1.35, 0.2, 0.115, 0.092, 0.085, 0.1, 0.1, 0.1}		-- Lv당 증가가격용 팩터 테이블

		for i = 1, lv do

			-- 1랭크일땐 뒤에 1자리를, 2랭크일땐 뒤의 2자리, 3랭크이상일땐 뒤에 세자리를 0으로 맞춘다.
			-- lua 의 배열은 zerobase 가 아니다!!! 중요. C로 적용할 때는 배열 주소를 -1 씩 해줘야 한다.
			-- 예를 들어 아래의 배열은 C에서는 f[rank-1] 이어야 맞고 lua에서는 f[rank] 여야 맞다.		
			if rank == 1 then
				cost = math.floor(k * f[rank] * 0.1) * 10

			elseif rank == 2 then
				cost = math.floor(k * f[rank] * 0.01) * 100
				
			else
				cost = math.floor(k * f[rank] * 0.001) * 1000
			end
			
			k = k + cost
		end
--	end

    --##시작     
	-- 가격 알림
    text = sconv("@90200305", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1)),"#@upgrade_cost@#",tostring(cost))
    --##끝 #@item_name@# Lv #@item_Lv@#을 Lv #@Lv+1@# '(/으)로 업그레이드 하는 비용은 #@upgrade_cost@#루피 입니다.<BR>업그레이드 하시겠습니까?'

    
	dlg_title( "@90200301" )
	dlg_text( text )

	dlg_menu( "@90010004", 'ItemLevelUp_Asura( ' .. handle .. ' , ' .. cost .. ' )' )
	dlg_menu( "@90010003", 'NPC_ItemUP_Asura_contact()' )
	dlg_menu( "@90010002", '' )

	dlg_show()

end


function ItemLevelUp_Asura( handle , cost )
 
		-- 현재 돈 얻어옴
        local gold = get_value( "gold" )
        local result
 
        -- 돈 모자라면 KIN
        if gold < cost then

		dlg_title( "@90200301" )
		dlg_text( "@90010005" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Asura_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()

		return
        end

        -- 아이템 레벨 얻어옴
        local lv = get_item_level( handle )
 
        -- 레벨 증가
        result = set_item_level( handle , lv + 1 )
		if result == 0 then
		cprint("해당 무기를 소지하고 있지 않습니다.")
			return
		else
		-- 업글비용 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()
 
	save()
	    
	--##시작
	-- 시스템 메세지
    message(sconv("@90010006", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1))))
    --##끝 #@item_name@# Lv #@item_Lv@#이 Lv #@item_up_Lv@# (/으)로 업그레이드 되었습니다.
	end
	NPC_ItemUP_Asura_contact()
end


function Quest_Link_102_1()
		local count = find_item( 1000022 )
		if count == 0 then
		insert_item( 1000022, 1 )
		else
		end
end




   --============================================================
   --             <<<<<< 초보자섬 NPC >>>>>>
   --============================================================

function NPC_ItemUP_Beginner_init()
	cprint( "!대장장이 타나엘 가동" )
	set_npc_name(  "@90300300"  )
end

function NPC_ItemUP_Beginner_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90300301" )
	dlg_text( "@90300302" )
 
 
	local i, handle, lv, text, text2, command, enhance, f, price
 
	-- 0번에서 7번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 7 do
 
		-- 입고있는 아이템 핸들 얻어옴
		handle = get_wear_item_handle( i )
 
		-- 만약 입고 있다면
		if handle ~= 0 then
 
			lv = get_item_level( handle )
			enhance =  get_item_enhance( handle )
 
			if lv == 0 then
				set_item_level( handle , 1 )
			end
			price = get_item_price( handle )
			--9.4 무기 가격
			if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
				price = 674086
			elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
				price = 539270
			end
			
			-- ## 시작
			-- 해당 아이템의 강화단계, 아이템명, Lv을 텍스트로 구성
			if enhance ~= 0 then
				-- 강화되어있으면 +x 표시
				text = sconv("@90010010", "#@enhance_level@#",tostring(enhance) ,"#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010010 = + #@enhance_level@# #@item_name@# Lv #@item_Lv@#

			else
				text = sconv("@90010011", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010011 = #@item_name@# Lv #@item_Lv@#
			end

			-- 아이템 레벨업 스크립트 펑션을 메뉴에 추가
			if price > 20 then
			command = 'ItemLevelUp_Beginner_check( ' .. handle .. ' )'
			dlg_menu( text, command )
			end

		end
 
	end
		
	dlg_menu( "@90100703", "open_market( 'Beginner_itemUP_suppert' )" )
	dlg_menu( "@690000079", "max_item_durability()" )
	dlg_menu( "@90606098", "random_item_change_menu()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
 
 

function ItemLevelUp_Beginner_check( handle )


	local i, j, k, max_level, rank, lv, price, cost, text

	rank = get_item_rank( handle )
	if rank < 1 then
		rank = 1
	end
	lv = get_item_level( handle )

	-- 랭크가 1이면 max=3레벨, 아니면 10레벨
	if rank == 1 then
		max_level = 5
	else
		max_level = 10
	end



	if lv >= max_level then

		--업글 가능한 레벨을 벗어나므로 무효
	        
		dlg_title( "@90300301" )
		dlg_text( "@90300303" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Beginner_contact()' )
		dlg_menu( "@90010002", '' )
		
		dlg_show()
			
		return
	end

	-- 업글 가능한 레벨이므로 가격 계산
	price = get_item_price( handle )
	--9.4 무기 가격
	if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
		price = 674086
	elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
		price = 539270
	end

	-- price < 20 인 아이템은 기본아이템이므로 업글 불가
	if price < 20 then
		dlg_title( "@90300301" )
		dlg_text( "@90300304" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Beginner_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
	
	
--============================================================================================
-- 모든 랭크 업그레이드 비용 계산 통합
--============================================================================================
--	if rank == 1 then
		-- 1랭크의 업글비용 계산
--		if lv == 1 then
--			cost = 120
--		else
--			cost = (lv ^ 2) * 100

--		end
		
--	else	
		-- 업글비용 계산

		k = price
		f = {1.35, 0.2, 0.115, 0.092, 0.085, 0.1, 0.1, 0.1}		-- Lv당 증가가격용 팩터 테이블

		for i = 1, lv do

			-- 1랭크일땐 뒤에 1자리를, 2랭크일땐 뒤의 2자리, 3랭크이상일땐 뒤에 세자리를 0으로 맞춘다.
			-- lua 의 배열은 zerobase 가 아니다!!! 중요. C로 적용할 때는 배열 주소를 -1 씩 해줘야 한다.
			-- 예를 들어 아래의 배열은 C에서는 f[rank-1] 이어야 맞고 lua에서는 f[rank] 여야 맞다.		
			if rank == 1 then
				cost = math.floor(k * f[rank] * 0.1) * 10

			elseif rank == 2 then
				cost = math.floor(k * f[rank] * 0.01) * 100
				
			else
				cost = math.floor(k * f[rank] * 0.001) * 1000
			end
			
			k = k + cost
		end
--	end

    --##시작    
	-- 가격 알림
     text = sconv("@90300305", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1)),"#@upgrade_cost@#",tostring(cost))
    --##끝 #@item_name@# Lv #@item_Lv@#을 Lv#@item_up_Lv@# '(/으)로업그레이드 하는 비용은 #@upgrade_cost@#루피 입니다.<BR>업그레이드 하시겠습니까?'

	dlg_title( "@90300301" )
	dlg_text( text )

	dlg_menu( "@90010004", 'ItemLevelUp_Beginner( ' .. handle .. ' , ' .. cost .. ' )' )
	dlg_menu( "@90010003", 'NPC_ItemUP_Beginner_contact()' )
	dlg_menu( "@90010002", '' )

	dlg_show()

end


function ItemLevelUp_Beginner( handle , cost )
 
		-- 현재 돈 얻어옴
        local gold = get_value( "gold" )
        local result
 
        -- 돈 모자라면 KIN
        if gold < cost then

		dlg_title( "@90300301" )
		dlg_text( "@90010005" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Beginner_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()

		return
        end

        -- 아이템 레벨 얻어옴
        local lv = get_item_level( handle )
 
        -- 레벨 증가
        result = set_item_level( handle , lv + 1 )
		if result == 0 then
		cprint("해당 무기를 소지하고 있지 않습니다.")
			return
		else
		-- 업글비용 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()
 
	save()
     
	--시작
	-- 시스템 메세지
     message(sconv("@90010006", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1))))
    --##끝 #@item_name@#Lv #@item_Lv@#이  Lv "#@item_up_Lv@#" (/으)로 업그레이드 되었습니다.   
    end
	NPC_ItemUP_Beginner_contact()
end


   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================

function NPC_ItemUP_Gaia_init()
	cprint( "!가이아 대장장이 가동" )
	set_npc_name(  "@90400300" )
end

-- 대장장이 미르피유
function NPC_ItemUP_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90400301" )
	dlg_text( "@90400302" )
 
 
	local i, handle, lv, text, text2, command, enhance, f, price
 
	-- 0번에서 7번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 7 do
 
		-- 입고있는 아이템 핸들 얻어옴
		handle = get_wear_item_handle( i )
 
		-- 만약 입고 있다면
		if handle ~= 0 then
						
			lv = get_item_level( handle )
			enhance =  get_item_enhance( handle )
 
			if lv == 0 then
				set_item_level( handle , 1 )
			end
			price = get_item_price( handle )
			--9.4 무기 가격
			if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
				price = 674086
			elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
				price = 539270
			end
			
			-- ## 시작
			-- 해당 아이템의 강화단계, 아이템명, Lv을 텍스트로 구성
			if enhance ~= 0 then
				-- 강화되어있으면 +x 표시
				text = sconv("@90010010", "#@enhance_level@#",tostring(enhance) ,"#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010010 = + #@enhance_level@# #@item_name@# Lv #@item_Lv@#

			else
				text = sconv("@90010011", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010011 = #@item_name@# Lv #@item_Lv@#
			end

			-- 아이템 레벨업 스크립트 펑션을 메뉴에 추가
			if price > 20 then
			command = 'ItemLevelUp_Gaia_check( ' .. handle .. ' )'
			dlg_menu( text, command )
			end

		end
 
	end
	
	dlg_menu( "@90100703", "open_market( 'Gaia_itemUP_suppert' )" )	
	dlg_menu( "@690000079", "max_item_durability()" )
	dlg_menu( "@90606098", "random_item_change_menu()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
	
	local qstart_text = get_value( "level" )
	local quest_progress1 =  get_quest_progress(2008)
	local quest_progress100 =  get_quest_progress(2000)

	-- 미래를 예언하는 소녀
	if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
	dlg_menu( "@90999617", "quest_rumor10()" )
	end
	
	-- 광신도 암살자
	if quest_progress1 == 255 then
	dlg_menu( "@90999842", "quest_witcharmy1()" )
	end

 
end
 
-- 세부대화 미르피유
function quest_rumor10()
	-- 다이얼로그 출력
	dlg_title( "@90400301" )

	dlg_text( "@90999619" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_7()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 소녀에 대한 이야기를 꺼림
function quest_rumor_a_7()
	-- 다이얼로그 출력
	dlg_title( "@90400301" )

	dlg_text( "@90999630" )
	
	dlg_menu( "@90010002", '' ) 			
	dlg_show()

end


function ItemLevelUp_Gaia_check( handle )


	local i, j, k, max_level, rank, lv, price, cost, text

	rank = get_item_rank( handle )
	if rank < 1 then
		rank = 1
	end
	lv = get_item_level( handle )

	-- 랭크가 1이면 max=3레벨, 아니면 10레벨
	if rank == 1 then
		max_level = 5
	else
		max_level = 10
	end



	if lv >= max_level then

		--업글 가능한 레벨을 벗어나므로 무효
	        
		dlg_title( "@90400301" )
		dlg_text( "@90400303" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Gaia_contact()' )
		dlg_menu( "@90010002", '' )
		
		dlg_show()
			
		return
	end

	-- 업글 가능한 레벨이므로 가격 계산
	price = get_item_price( handle )
	--9.4 무기 가격
	if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
		price = 674086
	elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
		price = 539270
	end

	-- price < 20 인 아이템은 기본아이템이므로 업글 불가
	if price < 20 then
		dlg_title( "@90400301" )
		dlg_text( "@90400304" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Gaia_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
	
	
	
--============================================================================================
-- 모든 랭크 업그레이드 비용 계산 통합
--============================================================================================
--	if rank == 1 then
		-- 1랭크의 업글비용 계산
--		if lv == 1 then
--			cost = 120
--		else
--			cost = (lv ^ 2) * 100

--		end
		
--	else	
		-- 업글비용 계산
		
		k = price
		f = {1.35, 0.2, 0.115, 0.092, 0.085, 0.1, 0.1, 0.1}		-- Lv당 증가가격용 팩터 테이블

		for i = 1, lv do

			-- 1랭크일땐 뒤에 1자리를, 2랭크일땐 뒤의 2자리, 3랭크이상일땐 뒤에 세자리를 0으로 맞춘다.
			-- lua 의 배열은 zerobase 가 아니다!!! 중요. C로 적용할 때는 배열 주소를 -1 씩 해줘야 한다.
			-- 예를 들어 아래의 배열은 C에서는 f[rank-1] 이어야 맞고 lua에서는 f[rank] 여야 맞다.		
			if rank == 1 then
				cost = math.floor(k * f[rank] * 0.1) * 10

			elseif rank == 2 then
				cost = math.floor(k * f[rank] * 0.01) * 100
				
			else
				cost = math.floor(k * f[rank] * 0.001) * 1000
			end
			
			k = k + cost
		end
--	end

    --##시작    
	-- 가격 알림
     text = sconv("@90400305", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1)),"#@upgrade_cost@#",tostring(cost))
    --##끝 #@item_name@# Lv #@item_Lv@#을 Lv#@item_up_Lv@# '(/으)로업그레이드 하는 비용은 #@upgrade_cost@#루피 입니다.<BR>업그레이드 하시겠습니까?'

	dlg_title( "@90400301" )
	dlg_text( text )

	dlg_menu( "@90010004", 'ItemLevelUp_Gaia( ' .. handle .. ' , ' .. cost .. ' )' )
	dlg_menu( "@90010003", 'NPC_ItemUP_Gaia_contact()' )
	dlg_menu( "@90010002", '' )

	dlg_show()

end


function ItemLevelUp_Gaia( handle , cost )
 
		-- 현재 돈 얻어옴
        local gold = get_value( "gold" )
        local result
 
        -- 돈 모자라면 KIN
        if gold < cost then

		dlg_title( "@90400301" )
		dlg_text( "@90010005" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Gaia_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()

		return
        end

        -- 아이템 레벨 얻어옴
        local lv = get_item_level( handle )
 
        -- 레벨 증가
        result = set_item_level( handle , lv + 1 )
		if result == 0 then
		cprint("해당 무기를 소지하고 있지 않습니다.")
			return
		else
		-- 업글비용 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()
 
	save()
	
	--##시작
	-- 시스템 메세지
    message(sconv("@90010006", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1))))
    --##끝 #@item_name@#Lv #@item_Lv@#이  Lv "#@item_up_Lv@#" (/으)로 업그레이드 되었습니다.     
    end
	NPC_ItemUP_Gaia_contact()
end


   --============================================================
   --             <<<<<< 론도 측 NPC >>>>>>
   --============================================================

function NPC_ItemUP_Rondoh_init()
	cprint( "!론도 대장장이 가동" )
	set_npc_name(  "@90600300" )
end

function NPC_ItemUP_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90600301" )
	dlg_text( "@90600302" )
 
 
	local i, handle, lv, text, text2, command, enhance, f, price
 
	-- 0번에서 7번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 7 do
 
		-- 입고있는 아이템 핸들 얻어옴
		handle = get_wear_item_handle( i )
 
		-- 만약 입고 있다면
		if handle ~= 0 then
 
			lv = get_item_level( handle )
			enhance =  get_item_enhance( handle )
 
			if lv == 0 then
				set_item_level( handle , 1 )
			end
			price = get_item_price( handle )
			--9.4 무기 가격
			if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
				price = 674086
			elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
				price = 539270
			end
			
			-- ## 시작
			-- 해당 아이템의 강화단계, 아이템명, Lv을 텍스트로 구성
			if enhance ~= 0 then
				-- 강화되어있으면 +x 표시
				text = sconv("@90010010", "#@enhance_level@#",tostring(enhance) ,"#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010010 = + #@enhance_level@# #@item_name@# Lv #@item_Lv@#

			else
				text = sconv("@90010011", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010011 = #@item_name@# Lv #@item_Lv@#
			end

			-- 아이템 레벨업 스크립트 펑션을 메뉴에 추가
			if price > 20 then
			command = 'ItemLevelUp_Rondoh_check( ' .. handle .. ' )'
			dlg_menu( text, command )
			end

		end
 
	end
	
	dlg_menu( "@90100703", "open_market( 'Rondoh_itemUP_suppert' )" )
	dlg_menu( "@690000079", "max_item_durability()" )
	dlg_menu( "@90606098", "random_item_change_menu()" )
	dlg_menu( "@90010002", '' ) 
	dlg_show() 
end
 
 

function ItemLevelUp_Rondoh_check( handle )


	local i, j, k, max_level, rank, lv, price, cost, text

	rank = get_item_rank( handle )
	if rank < 1 then
		rank = 1
	end
	lv = get_item_level( handle )

	-- 랭크가 1이면 max=3레벨, 아니면 10레벨
	if rank == 1 then
		max_level = 5
	else
		max_level = 10
	end



	if lv >= max_level then

		--업글 가능한 레벨을 벗어나므로 무효
	        
		dlg_title( "@90600301" )
		dlg_text( "@90600303" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Rondoh_contact()' )
		dlg_menu( "@90010002", '' )
		
		dlg_show()
			
		return
	end

	-- 업글 가능한 레벨이므로 가격 계산
	price = get_item_price( handle )
	--9.4 무기 가격
	if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
		price = 674086
	elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
		price = 539270
	end
	-- price < 20 인 아이템은 기본아이템이므로 업글 불가
	if price < 20 then
		dlg_title( "@90600301" )
		dlg_text( "@90600304" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Rondoh_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
	
	
	
--============================================================================================
-- 모든 랭크 업그레이드 비용 계산 통합
--============================================================================================
--	if rank == 1 then
		-- 1랭크의 업글비용 계산
--		if lv == 1 then
--			cost = 120
--		else
--			cost = (lv ^ 2) * 100

--		end
		
--	else	
		-- 업글비용 계산

		k = price
		f = {1.35, 0.2, 0.115, 0.092, 0.085, 0.1, 0.1, 0.1}		-- Lv당 증가가격용 팩터 테이블

		for i = 1, lv do

			-- 1랭크일땐 뒤에 1자리를, 2랭크일땐 뒤의 2자리, 3랭크이상일땐 뒤에 세자리를 0으로 맞춘다.
			-- lua 의 배열은 zerobase 가 아니다!!! 중요. C로 적용할 때는 배열 주소를 -1 씩 해줘야 한다.
			-- 예를 들어 아래의 배열은 C에서는 f[rank-1] 이어야 맞고 lua에서는 f[rank] 여야 맞다.		
			if rank == 1 then
				cost = math.floor(k * f[rank] * 0.1) * 10

			elseif rank == 2 then
				cost = math.floor(k * f[rank] * 0.01) * 100
				
			else
				cost = math.floor(k * f[rank] * 0.001) * 1000
			end
			
			k = k + cost
		end
--	end

    --##시작    
	-- 가격 알림
     text = sconv("@90600305", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1)),"#@upgrade_cost@#",tostring(cost))
    --##끝 #@item_name@# Lv #@item_Lv@#을 Lv#@item_up_Lv@# '(/으)로업그레이드 하는 비용은 #@upgrade_cost@#루피 입니다.<BR>업그레이드 하시겠습니까?'

	dlg_title( "@90600301" )
	dlg_text( text )

	dlg_menu( "@90010004", 'ItemLevelUp_Rondoh( ' .. handle .. ' , ' .. cost .. ' )' )
	dlg_menu( "@90010003", 'NPC_ItemUP_Rondoh_contact()' )
	dlg_menu( "@90010002", '' )

	dlg_show()

end


function ItemLevelUp_Rondoh( handle , cost )
 
		-- 현재 돈 얻어옴
        local gold = get_value( "gold" )
        local result
 
        -- 돈 모자라면 KIN
        if gold < cost then

		dlg_title( "@90600301" )
		dlg_text( "@90010005" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Rondoh_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()

		return
        end

        -- 아이템 레벨 얻어옴
        local lv = get_item_level( handle )
 
        -- 레벨 증가
        result = set_item_level( handle , lv + 1 )
		if result == 0 then
		cprint("해당 무기를 소지하고 있지 않습니다.")
			return
		else
		-- 업글비용 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()
 
	save()
	
	--##시작
	-- 시스템 메세지
    message(sconv("@90010006", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1))))
    --##끝 #@item_name@#Lv #@item_Lv@#이  Lv "#@item_up_Lv@#" (/으)로 업그레이드 되었습니다.     
    end
	NPC_ItemUP_Rondoh_contact()
end



   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================

function NPC_ItemUP_Secroute_init()
	cprint( "!시크루트 대장장이 가동" )
	set_npc_name(  "@90700300" )
end

function NPC_ItemUP_Secroute_contact()

	if is_premium() == false then
		dlg_title( "@90700301" )
		dlg_text( "@90700118" )
		dlg_menu( "@90010002", '' )
		dlg_show()
	
		return
	end


	-- 다이얼로그 출력
	dlg_title( "@90700301" )
	dlg_text( "@90700302" )
 
 
	local i, handle, lv, text, text2, command, enhance, f, price
 
	-- 0번에서 7번 슬롯까지 훑어보면서 메뉴에 추가
	for i = 0, 7 do
 
		-- 입고있는 아이템 핸들 얻어옴
		handle = get_wear_item_handle( i )
 
		-- 만약 입고 있다면
		if handle ~= 0 then
 
			lv = get_item_level( handle )
			enhance =  get_item_enhance( handle )
 
			if lv == 0 then
				set_item_level( handle , 1 )
			end
			price = get_item_price( handle )
			--9.4 무기 가격
			if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
				price = 674086
			elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
				price = 539270
			end
			
			-- ## 시작
			-- 해당 아이템의 강화단계, 아이템명, Lv을 텍스트로 구성
			if enhance ~= 0 then
				-- 강화되어있으면 +x 표시
				text = sconv("@90010010", "#@enhance_level@#",tostring(enhance) ,"#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010010 = + #@enhance_level@# #@item_name@# Lv #@item_Lv@#

			else
				text = sconv("@90010011", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
				-- ## 끝 @90010011 = #@item_name@# Lv #@item_Lv@#
			end

			-- 아이템 레벨업 스크립트 펑션을 메뉴에 추가
			if price > 20 then
			command = 'ItemLevelUp_Secroute_check( ' .. handle .. ' )'
			dlg_menu( text, command )
			end

		end
 
	end

	dlg_menu( "@690000079", "max_item_durability()" )
	dlg_menu( "@90606098", "random_item_change_menu()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
 
 

function ItemLevelUp_Secroute_check( handle )


	local i, j, k, max_level, rank, lv, price, cost, text

	rank = get_item_rank( handle )
	if rank < 1 then
		rank = 1
	end
	lv = get_item_level( handle )

	-- 랭크가 1이면 max=3레벨, 아니면 10레벨
	if rank == 1 then
		max_level = 5
	else
		max_level = 10
	end



	if lv >= max_level then

		--업글 가능한 레벨을 벗어나므로 무효
	        
		dlg_title( "@90700301" )
		dlg_text( "@90700303" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Secroute_contact()' )
		dlg_menu( "@90010002", '' )
		
		dlg_show()
			
		return
	end

	-- 업글 가능한 레벨이므로 가격 계산
	price = get_item_price( handle )
	--9.4 무기 가격
	if get_item_code( handle ) >= 3100020 and get_item_code( handle ) <= 3100026 then
		price = 674086
	elseif get_item_code( handle ) >= 3100027 and get_item_code( handle ) <= 3100031 then
		price = 539270
	end
	-- price < 20 인 아이템은 기본아이템이므로 업글 불가
	if price < 20 then
		dlg_title( "@90700301" )
		dlg_text( "@90700304" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Secroute_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()
		
		return
	end
	
	
	
		-- 업글비용 계산

	k = price
	f = {1.35, 0.2, 0.115, 0.092, 0.085, 0.1, 0.1, 0.1}		-- Lv당 증가가격용 팩터 테이블

		for i = 1, lv do

			-- 1랭크일땐 뒤에 1자리를, 2랭크일땐 뒤의 2자리, 3랭크이상일땐 뒤에 세자리를 0으로 맞춘다.
			-- lua 의 배열은 zerobase 가 아니다!!! 중요. C로 적용할 때는 배열 주소를 -1 씩 해줘야 한다.
			-- 예를 들어 아래의 배열은 C에서는 f[rank-1] 이어야 맞고 lua에서는 f[rank] 여야 맞다.		
			if rank == 1 then
				cost = math.floor(k * f[rank] * 0.1) * 10

			elseif rank == 2 then
				cost = math.floor(k * f[rank] * 0.01) * 100
				
			else
				cost = math.floor(k * f[rank] * 0.001) * 1000
			end
			
			k = k + cost
		end
		
	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	-- 프리미엄 서비스 체크 주석처리
	-- 시크루트 일 때 (7022 모험 가이드 케인)
	if is_premium() then
		-- 시크루트에서는 15% 할인된 가격으로 서비스
		cost = math.floor ( cost * 0.85 )
	end

    --##시작    
	-- 가격 알림
     text = sconv("@90700305", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1)),"#@upgrade_cost@#",tostring(cost))
    --##끝 #@item_name@# Lv #@item_Lv@#을 Lv#@item_up_Lv@# '(/으)로업그레이드 하는 비용은 #@upgrade_cost@#루피 입니다.<BR>업그레이드 하시겠습니까?'

	dlg_title( "@90700301" )
	dlg_text( text )

	dlg_menu( "@90010004", 'ItemLevelUp_Secroute( ' .. handle .. ' , ' .. cost .. ' )' )
	dlg_menu( "@90010003", 'NPC_ItemUP_Secroute_contact()' )
	dlg_menu( "@90010002", '' )

	dlg_show()

end


function ItemLevelUp_Secroute( handle , cost )
 
		-- 현재 돈 얻어옴
        local gold = get_value( "gold" )
        local result
 
        -- 돈 모자라면 KIN
        if gold < cost then

		dlg_title( "@90700301" )
		dlg_text( "@90010005" )

		dlg_menu( "@90010003", 'NPC_ItemUP_Secroute_contact()' )
		dlg_menu( "@90010002", '' )

		dlg_show()

		return
        end

        -- 아이템 레벨 얻어옴
        local lv = get_item_level( handle )
 
        -- 레벨 증가
        result = set_item_level( handle , lv + 1 )
		if result == 0 then
		cprint("해당 무기를 소지하고 있지 않습니다.")
			return
		else
		-- 업글비용 차감
        set_value( "gold", gold - cost )
        update_gold_chaos()
 
	save()
	
	--##시작
	-- 시스템 메세지
    message(sconv("@90010006", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1))))
    --##끝 #@item_name@#Lv #@item_Lv@#이  Lv "#@item_up_Lv@#" (/으)로 업그레이드 되었습니다.     
    end
	NPC_ItemUP_Secroute_contact()
end





   --============================================================
   --             <<<<<< 도시 유적 측 NPC >>>>>>
   --============================================================
   
   --고대강화
   
function NPC_ItemUP_Ancient_relic_init()
	cprint( "!도시유적 대장장이 가동" )
	set_npc_name(  "@90703100" )
end

function NPC_ItemUP_Ancient_relic_contact()
 
 	dlg_title( "@90703100")  -- 고대의 대장장이 하뮤
	dlg_text(  "@90703107")  -- 고대의 강철조각과 LV10 이상의 무기나 방어구를 가져오면 강화해 주겠습니다.
 
	local i
	for i = 0, 7 do
		local handle = get_wear_item_handle(i)
		local lv =  get_item_level( handle )
		if handle ~= 0 then
 			enhance =  get_item_enhance( handle )
			local text
			if enhance ~= 0 then
				text = sconv("@90010010", "#@enhance_level@#",tostring(enhance) ,"#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
			else
				text = sconv("@90010011", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ) , "#@item_Lv@#",tostring(lv))
			end

			local lv = get_item_level( handle )
			if lv > 9 and lv < 20 then
				local command = 'ItemLevelUp_Ancient_check( ' .. handle .. ' )'
				dlg_menu( text, command )
			end
		end
	end
	
	dlg_menu( "@690000079", "max_item_durability()" )
	dlg_menu( "@90010002", '' )
	dlg_show()
end
 
 

function ItemLevelUp_Ancient_check( handle )

	local code = get_item_code( handle )
	local name = get_item_name_id( code )
	local lv = get_item_level( handle )
	local req_cost
	--					lv11	lv12	lv13	 lv14	 lv15    lv16	 lv17	 lv18	 lv19	 lv20
	if get_local_info() == 8192 then
		req_cost = {	10000000,20000000,30000000,40000000,50000000,60000000,70000000,80000000,90000000,100000000} -- 중동
	else
		req_cost = {	 1000000, 2000000, 3000000, 4000000, 5000000, 6000000, 7000000, 8000000, 9000000, 10000000}
	end
	
	local req_iron = {	1,		2,		3,		 4,		 5,		 6,		 7,		 8,		 9,		 10}		

	local cost = req_cost[lv-9]
	local iron = req_iron[lv-9]
	
	local text = sconv("@90703108", "#@item_name@#", "@" .. tostring(name ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1)),"#@upgrade_cost@#",tostring(cost), "#@count@#", tostring(iron))
	dlg_title( "@90703101" )
	dlg_text( text )
	dlg_menu( "@90010004", 'ItemLevelUp_Ancient( ' .. handle .. ' , ' .. cost .. ' , ' .. iron .. ' )' )
	dlg_menu( "@90010003", 'NPC_ItemUP_Ancient_relic_contact()' )
	dlg_menu( "@90010002", '' )
	dlg_show()
end


function ItemLevelUp_Ancient( handle , cost, iron )
 
    local gold = get_value( "gold" )
    if gold < cost then
		dlg_title( "@90703101" )
		dlg_text( "@90010005" )
		dlg_menu( "@90010003", 'NPC_ItemUP_Ancient_relic_contact()' )
		dlg_menu( "@90010002", '' )
		dlg_show()
		return
    end
		
	-- 레드포션LV1(602001~602004)로 테스트	
	local iron_code = 1100117
	local count = find_item( iron_code)
	if count < iron then
		dlg_title("@90703101")
		dlg_text( "@90703109")	-- 고대의 강철조각이 부족합니다
		dlg_menu( "@90010003", 'NPC_ItemUP_Ancient_relic_contact()' )
		dlg_menu( "@90010002", '' )
		dlg_show()
		return
	end

	local lv = get_item_level( handle )
    local result = set_item_level( handle , lv + 1 )
	if result == 0 then
		cprint("해당 무기를 소지하고 있지 않습니다.")
		return
	end


	local req_item_handle = get_item_handle(iron_code)
	delete_item( req_item_handle, iron )
	set_value( "gold", gold-cost )
	update_gold_chaos()
	save()
    message(sconv("@90010006", "#@item_name@#", "@" .. tostring( get_item_name_id( get_item_code( handle ) ) ),"#@item_Lv@#",tostring(lv),"#@item_up_Lv@#",tostring((lv+1))))
	NPC_ItemUP_Ancient_relic_contact()
end
------------------------------ 대장장이 타룬 9.5 --------------
function NPC_Smith_Tarun_contact()
	local state_code = get_local_info()  -- 국가코드
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)	
	
	dlg_text( "@90610353")
	
	dlg_menu( "@90610355", 'NPC_Tarun_Make_Armor()' )
	dlg_menu( "@90610469", 'NPC_Tarun_OpenBox()' )
	if state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then
		dlg_menu( "@90610487", "open_market( 'medusa_shaper' )" )
	end
	
	
	dlg_menu( "@90010002", '' )
	dlg_show()
end

function NPC_Tarun_OpenBox()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	dlg_text( "@90610470")
	
	dlg_menu( "@90610471", 'NPC_Tarun_OpenBox_Done()' )	
	dlg_menu( "@90010003", " NPC_Smith_Tarun_contact() " )	--돌아가기
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

function NPC_Tarun_OpenBox_Done()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	local Box_Count = find_item(2016132)
	
	-- Edit by ham999dy - 6:30 PM Friday, January 18, 2019
	if Box_Count >= 1000 then -- 성공
		dlg_text( "@90610472")
		delete_item( get_item_handle(2016132), 1000 )
		insert_item(2016128,1000)
		-- insert_item(2016128, math.random(1000, 15000))
	elseif Box_Count >= 100 then -- 성공
		dlg_text( "@90610472")
		delete_item( get_item_handle(2016132), 100 )
		insert_item(2016128,100)
		-- insert_item(2016128, math.random(100, 1500))
	else -- 실패
		dlg_text( "@90610473")
	end
	
	dlg_menu( "@90010003", " NPC_Tarun_OpenBox() " )	--돌아가기
	dlg_menu( "@90010002", '' )
	dlg_show()	
end

-----------------타룬 방어구 제작

function NPC_Tarun_Make_Armor()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	dlg_text( "@90610365")
	dlg_menu( "@90610357", 'NPC_Tarun_Make_Armor_select(601100280)' )
	dlg_menu( "@90610358", 'NPC_Tarun_Make_Armor_select(601100424)' )
	dlg_menu( "@90610359", 'NPC_Tarun_Make_3rdArmor_select()' )
	dlg_menu( "@90610360", 'NPC_Tarun_Make_4thArmor_select()' )
	dlg_menu( "@90010002", '' )
	dlg_show()
end

function NPC_Tarun_Make_Armor_select(item_id)
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	dlg_text( "@90610366") -- 제작 할 방어구는 꼭 착용을 해야지만..
	
	local text1 = sconv("@90610361","#@item@#","@"..tostring(item_id+10000000))
	local text2 = sconv("@90610361","#@item@#","@"..tostring(item_id+10000001))
	local text3 = sconv("@90610361","#@item@#","@"..tostring(item_id+10000002))
	local text4 = sconv("@90610361","#@item@#","@"..tostring(item_id+10000003))
	dlg_menu( text1, 'NPC_Tarun_Make_Armor_select_1(3,'..tostring(item_id)..')' ) -- 투구
	dlg_menu( text2, 'NPC_Tarun_Make_Armor_select_1(4,'..tostring(item_id+1)..')' ) -- 장갑
	dlg_menu( text3, 'NPC_Tarun_Make_Armor_select_1(5,'..tostring(item_id+2)..')' ) -- 부츠
	dlg_menu( text4, 'NPC_Tarun_Make_Armor_select_1(2,'..tostring(item_id+3)..')' ) -- 갑옷
	
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

function NPC_Tarun_Make_Armor_select_1(part, Want_Item)
	
	
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)	
	
	local pdata =	{ -- need_item_string, want_item, level, count,					
					710000904, 601100280, 175, 50,
					90610376, 601100281, 175, 50, -- 강화된 이공간의 글러브
					90610377, 601100282, 175, 50, -- 강화된 이공간의 부츠
					90610378, 601100283, 175, 50, -- 강화된 이공간의 갑옷
					611100280, 601100424, 181, 130, 
					611100281, 601100425, 182, 150, 
					611100282, 601100426, 183, 220, 
					611100283, 601100427, 185, 300, 
					--611100424, 601100428, 181, 300, 
					--611100425, 601100429, 182, 500, 
					--611100426, 601100430, 183, 700, 
					--611100427, 601100431, 185, 1000, 
					--611100428, 601100432, 186, 2100, 
					--611100429, 601100433, 187, 2300, 
					--611100430, 601100434, 188, 2500, 
					--611100431, 601100435, 190, 3100, 

		}
	local old_item = get_item_code( get_wear_item_handle( part ) )
	local equip_item = 0
	local length = table.getn(pdata)
	local Need_Item, Level, Count
	for i = 2, length,4 do
		if pdata[i] == Want_Item then
			New_Item = pdata[i]
			New_Item_string = pdata[i]+10000000			
			Need_Item = pdata[i-1]	
			Level = pdata[i+1]			
			Count = pdata[i+2]
			equip_item = 1
			break
		end
	end
	
	dlg_text( sconv ("@90610368", "#@item@#", "@"..tostring( New_Item_string ), "#@level@#", Level, "#@need_item@#", "@"..tostring( Need_Item ), "#@number@#", Count )) -- 레벨 재료....
	if equip_item == 1 then
		dlg_menu( "@90610362", 'NPC_Tarun_Make_Armor_done('..tostring(part)..','..tostring(New_Item )..')' ) -- 제작하기
	end
		
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

function NPC_Tarun_Make_Armor_done(part, WantItem)
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	local pdata =	{ -- old_item, new_item, level, count,					
					700000896, 601100281, 175, 50, 
					700000897, 601100281, 175, 50, 
					700000898, 601100281, 175, 50, 
					700000899, 601100281, 175, 50, 
					700000900, 601100282, 175, 50, 
					700000901, 601100282, 175, 50, 
					700000902, 601100282, 175, 50, 
					700000903, 601100282, 175, 50, 
					700000883, 601100283, 175, 50, 
					700000884, 601100283, 175, 50, 
					700000885, 601100283, 175, 50, 
					700000886, 601100283, 175, 50, 
					700000904, 601100280, 175, 50, 
					601100280, 601100424, 181, 130, 
					601100281, 601100425, 182, 150, 
					601100282, 601100426, 183, 220, 
					601100283, 601100427, 185, 300, 
					--601100424, 601100428, 181, 300, 
					--601100425, 601100429, 182, 500, 
					--601100426, 601100430, 183, 700, 
					--601100427, 601100431, 185, 1000, 
					--601100428, 601100432, 186, 2100, 
					--601100429, 601100433, 187, 2300, 
					--601100430, 601100434, 188, 2500, 
					--601100431, 601100435, 190, 3100, 

		}
	
	-- 방어구 착용 안했을 경우
	
	if get_wear_item_handle( part ) == 0 then --방어구 없
		dlg_text( "@90610367") -- 장착 후 제작 가능
	else
		local equip_item = get_item_code( get_wear_item_handle( part ) )
		local length = table.getn(pdata)
		local item_compare = 0
		local NewItem, Level, Count
		for i = 1, length,4 do
			--private_notice(i .." : "..pdata[i])			
			if pdata[i] == equip_item then
				--private_notice("WantItem : "..WantItem)
				--private_notice("Pdata : "..pdata[i+1])
				if pdata[i+1] == WantItem then
					--private_notice("들어왔음")
					Count = pdata[i+3]
					--private_notice("Count : "..Count)
					Level = pdata[i+2]
					--private_notice("Level : "..Level)
					NewItem = pdata[i+1]
					item_compare = 1
					break
				end
			end
		end
		
		if item_compare == 0 then -- 변경 가능한 아이템이 아님
			dlg_text( "@90610379") -- 제작못함
		elseif get_value( 'level' ) < Level then -- 레벨이 낮음
			dlg_text( "@90610380")
		elseif find_item ( 2016129 ) < Count then -- 재료 모자람
			dlg_text( "@90610381")
		else
			dlg_text( "@90610382") -- 제작완료
			
			delete_item( get_item_handle( 2016129 ), Count )
			change_item_code( get_wear_item_handle( part ), NewItem )
			set_item_ethereal_durability( get_wear_item_handle( part ), get_max_item_ethereal_durability( get_wear_item_handle( part ) ) ) -- 착용된 아이템의 내구도를 최대값으로 채워준다.
			update_wear() -- 방어구 변경후 외형 업데이트
		end
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
end
---3차 방어구
function NPC_Tarun_Make_3rdArmor_select()
	local Cha_Job = get_value('job')
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	dlg_text( "@90610366") -- 제작 할 방어구는 꼭 착용을 해야지만..
	
	dlg_menu( "@90610361\v#@item@#\v@611100449", 'NPC_Tarun_Make_3rdArmor_select_1(3, 601100449)' ) -- 투구
	dlg_menu( "@90610361\v#@item@#\v@611100450", 'NPC_Tarun_Make_3rdArmor_select_1(4, 601100450)' ) -- 장갑
	dlg_menu( "@90610361\v#@item@#\v@611100451", 'NPC_Tarun_Make_3rdArmor_select_1(5, 601100451)' ) -- 부츠
	
	if Cha_Job == 120 or Cha_Job == 220 or Cha_Job == 221 or Cha_Job == 320 then
		dlg_menu( "@90610361\v#@item@#\v@611100452", 'NPC_Tarun_Make_3rdArmor_select_1(2, 601100452)' ) -- 아머 기사
	elseif Cha_Job == 121 or Cha_Job == 321 then
		dlg_menu( "@90610361\v#@item@#\v@611100453", 'NPC_Tarun_Make_3rdArmor_select_1(2, 601100453)' ) -- 타이즈 헌터
	elseif Cha_Job == 122 or Cha_Job == 123 or Cha_Job == 222 or Cha_Job == 223 or Cha_Job == 322 or Cha_Job == 323 then
		dlg_menu( "@90610361\v#@item@#\v@611100454", 'NPC_Tarun_Make_3rdArmor_select_1(2, 601100454)' ) -- 가브 마법사
	elseif Cha_Job == 124 or Cha_Job == 224 or Cha_Job == 324 then
		dlg_menu( "@90610361\v#@item@#\v@611100455", 'NPC_Tarun_Make_3rdArmor_select_1(2, 601100455)' ) -- 코트 소환사
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

function NPC_Tarun_Make_3rdArmor_select_1(part, Want_Item) -- 인페르노
	
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)	
	
	local pdata =	{ -- need_item_string, want_item, level, count,
						611100424, 601100449, 186, 300, 3000, 1500, 750, 15, 
						611100425, 601100450, 187, 300, 3000, 1500, 750, 15, 
						611100426, 601100451, 188, 600, 4000, 2000, 1000, 20, 
						611100427, 601100452, 190, 1000, 5000, 3000, 1500, 50, 
						611100427, 601100453, 190, 1000, 5000, 3000, 1500, 50, 
						611100427, 601100454, 190, 1000, 5000, 3000, 1500, 50, 
						611100427, 601100455, 190, 1000, 5000, 3000, 1500, 50, 

		}
	local old_item = get_item_code( get_wear_item_handle( part ) )
	local equip_item = 0
	local length = table.getn(pdata)
	local Need_Item, Level, Count1, Count2, Count3, Count4, Count5
	for i = 2, length,8 do
		if pdata[i] == Want_Item then
			New_Item = pdata[i]
			New_Item_string = pdata[i]+10000000			
			Need_Item = pdata[i-1]	
			Level = pdata[i+1]			
			Count1 = pdata[i+2]
			Count2 = pdata[i+3]
			Count3 = pdata[i+4]
			Count4 = pdata[i+5]
			Count5 = pdata[i+6]
			equip_item = 1
			break
		end
	end
	
	dlg_text( sconv ("@90610858", "#@item@#", "@"..tostring( New_Item_string ), "#@level@#", Level, "#@need_item@#", "@"..tostring( Need_Item ), "#@number1@#", Count1, "#@number2@#", Count2, "#@number3@#", Count3, "#@number4@#", Count4, "#@number5@#", Count5 )) -- 레벨 재료....
		
	if equip_item == 1 then
		dlg_menu( "@90610362", 'NPC_Tarun_Make_3rdArmor_done('..tostring(part)..','..tostring(New_Item )..')' ) -- 제작하기
	end
		
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

function NPC_Tarun_Make_3rdArmor_done(part, WantItem)
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	local pdata =	{ -- old_item, new_item, level, count1, count2, count3, count4,	count5,			
						601100424, 601100449, 186, 300, 3000, 1500, 750, 15, 
						601100425, 601100450, 187, 300, 3000, 1500, 750, 15, 
						601100426, 601100451, 188, 600, 4000, 2000, 1000, 20, 
						601100427, 601100452, 190, 1000, 5000, 3000, 1500, 50, 
						601100427, 601100453, 190, 1000, 5000, 3000, 1500, 50, 
						601100427, 601100454, 190, 1000, 5000, 3000, 1500, 50, 
						601100427, 601100455, 190, 1000, 5000, 3000, 1500, 50,
		}
	
	-- 방어구 착용 안했을 경우
	
	if get_wear_item_handle( part ) == 0 then --방어구 없
		dlg_text( "@90610367") -- 장착 후 제작 가능
	else
		local equip_item = get_item_code( get_wear_item_handle( part ) )
		local length = table.getn(pdata)
		local item_compare = 0
		local NewItem, Level, Count1, Count2, Count3, Count4, Count5
		for i = 1, length,8 do
			--private_notice(i .." : "..pdata[i])			
			if pdata[i] == equip_item then
				--private_notice("WantItem : "..WantItem)
				--private_notice("Pdata : "..pdata[i+1])
				if pdata[i+1] == WantItem then
					--private_notice("들어왔음")
					Count1 = pdata[i+3]
					Count2 = pdata[i+4]
					Count3 = pdata[i+5]
					Count4 = pdata[i+6]
					Count5 = pdata[i+7]
					--private_notice("Count5 : "..Count5)
					Level = pdata[i+2]
					--private_notice("Level : "..Level)
					NewItem = pdata[i+1]
					item_compare = 1
					break
				end
			end
		end
		
		if item_compare == 0 then -- 변경 가능한 아이템이 아님
			dlg_text( "@90610379") -- 제작못함
		elseif get_value( 'level' ) < Level then -- 레벨이 낮음
			dlg_text( "@90610380")
		elseif find_item ( 2016129 ) < Count1 or find_item ( 1000540 ) < Count2 or find_item ( 1000541 ) < Count3 or find_item ( 1000542 ) < Count4 or find_item ( 2016158 ) < Count5 then -- 재료 모자람
			dlg_text( "@90610381")
		else
			dlg_text( "@90610382") -- 제작완료
			
			delete_item( get_item_handle( 2016129 ), Count1 )
			delete_item( get_item_handle( 1000540 ), Count2 )
			delete_item( get_item_handle( 1000541 ), Count3 )
			delete_item( get_item_handle( 1000542 ), Count4 )
			delete_item( get_item_handle( 2016158 ), Count5 )
			change_item_code( get_wear_item_handle( part ), NewItem )
			set_item_ethereal_durability( get_wear_item_handle( part ), get_max_item_ethereal_durability( get_wear_item_handle( part ) ) ) -- 착용된 아이템의 내구도를 최대값으로 채워준다.
			update_wear() -- 방어구 변경후 외형 업데이트
		end
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
end

---4차 방어구
function NPC_Tarun_Make_4thArmor_select()
	local Cha_Job = get_value('job')
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	dlg_text( "@90610366") -- 제작 할 방어구는 꼭 착용을 해야지만..
	
	dlg_menu( "@90610361\v#@item@#\v@611100456", 'NPC_Tarun_Make_4thArmor_select_1(3, 601100456)' ) -- 투구
	dlg_menu( "@90610361\v#@item@#\v@611100457", 'NPC_Tarun_Make_4thArmor_select_1(4, 601100457)' ) -- 장갑
	dlg_menu( "@90610361\v#@item@#\v@611100458", 'NPC_Tarun_Make_4thArmor_select_1(5, 601100458)' ) -- 부츠
	
	if Cha_Job == 120 or Cha_Job == 220 or Cha_Job == 221 or Cha_Job == 320 then
		dlg_menu( "@90610361\v#@item@#\v@611100459", 'NPC_Tarun_Make_4thArmor_select_1(2, 601100459)' ) -- 아머 기사
	elseif Cha_Job == 121 or Cha_Job == 321 then
		dlg_menu( "@90610361\v#@item@#\v@611100460", 'NPC_Tarun_Make_4thArmor_select_1(2, 601100460)' ) -- 타이즈 헌터
	elseif Cha_Job == 122 or Cha_Job == 123 or Cha_Job == 222 or Cha_Job == 223 or Cha_Job == 322 or Cha_Job == 323 then
		dlg_menu( "@90610361\v#@item@#\v@611100461", 'NPC_Tarun_Make_4thArmor_select_1(2, 601100461)' ) -- 가브 마법사
	elseif Cha_Job == 124 or Cha_Job == 224 or Cha_Job == 324 then
		dlg_menu( "@90610361\v#@item@#\v@611100462", 'NPC_Tarun_Make_4thArmor_select_1(2, 601100462)' ) -- 코트 소환사
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()	
end

function NPC_Tarun_Make_4thArmor_select_1(part, Want_Item) -- 루키스	
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)	
	
	local pdata =	{ -- need_item_string, want_item, level, count,
						611100449, 601100456, 191, 1500, 6000, 3000, 1500, 10, 
						611100450, 601100457, 192, 1500, 6000, 3000, 1500, 10, 
						611100451, 601100458, 193, 2000, 8000, 4000, 2000, 15, 
						611100452, 601100459, 195, 3000, 10000, 6000, 3000, 35, 
						611100453, 601100460, 195, 3000, 10000, 6000, 3000, 35, 
						611100454, 601100461, 195, 3000, 10000, 6000, 3000, 35, 
						611100455, 601100462, 195, 3000, 10000, 6000, 3000, 35,  
		}
	local old_item = get_item_code( get_wear_item_handle( part ) )
	local equip_item = 0
	local length = table.getn(pdata)
	local Need_Item, Level, Count1, Count2, Count3, Count4, Count5
	for i = 2, length,8 do
		if pdata[i] == Want_Item then
			New_Item = pdata[i]
			New_Item_string = pdata[i]+10000000			
			Need_Item = pdata[i-1]	
			Level = pdata[i+1]			
			Count1 = pdata[i+2]
			Count2 = pdata[i+3]
			Count3 = pdata[i+4]
			Count4 = pdata[i+5]
			Count5 = pdata[i+6]
			equip_item = 1
			break
		end
	end
	
	dlg_text( sconv ("@90610859", "#@item@#", "@"..tostring( New_Item_string ), "#@level@#", Level, "#@need_item@#", "@"..tostring( Need_Item ), "#@number1@#", Count1, "#@number2@#", Count2, "#@number3@#", Count3, "#@number4@#", Count4, "#@number5@#", Count5 )) -- 레벨 재료....
		
	if equip_item == 1 then
		dlg_menu( "@90610362", 'NPC_Tarun_Make_4thArmor_done('..tostring(part)..','..tostring(New_Item )..')' ) -- 제작하기
	end
		
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

function NPC_Tarun_Make_4thArmor_done(part, WantItem)
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	local pdata =	{ -- old_item, new_item, level, count1, count2, count3, count4,	count5,			
						601100449, 601100456, 191, 1500, 6000, 3000, 1500, 10, 
						601100450, 601100457, 192, 1500, 6000, 3000, 1500, 10, 
						601100451, 601100458, 193, 2000, 8000, 4000, 2000, 15, 
						601100452, 601100459, 195, 3000, 10000, 6000, 3000, 35, 
						601100453, 601100460, 195, 3000, 10000, 6000, 3000, 35, 
						601100454, 601100461, 195, 3000, 10000, 6000, 3000, 35, 
						601100455, 601100462, 195, 3000, 10000, 6000, 3000, 35, 
		}
	
	-- 방어구 착용 안했을 경우
	
	if get_wear_item_handle( part ) == 0 then --방어구 없
		dlg_text( "@90610367") -- 장착 후 제작 가능
	else
		local equip_item = get_item_code( get_wear_item_handle( part ) )
		local length = table.getn(pdata)
		local item_compare = 0
		local NewItem, Level, Count1, Count2, Count3, Count4, Count5
		for i = 1, length,8 do
			--private_notice(i .." : "..pdata[i])			
			if pdata[i] == equip_item then
				--private_notice("WantItem : "..WantItem)
				--private_notice("Pdata : "..pdata[i+1])
				if pdata[i+1] == WantItem then
					--private_notice("들어왔음")
					Count1 = pdata[i+3]
					Count2 = pdata[i+4]
					Count3 = pdata[i+5]
					Count4 = pdata[i+6]
					Count5 = pdata[i+7]
					--private_notice("Count5 : "..Count5)
					Level = pdata[i+2]
					--private_notice("Level : "..Level)
					NewItem = pdata[i+1]
					item_compare = 1
					break
				end
			end
		end
		
		if item_compare == 0 then -- 변경 가능한 아이템이 아님
			dlg_text( "@90610379") -- 제작못함
		elseif get_value( 'level' ) < Level then -- 레벨이 낮음
			dlg_text( "@90610380")
		elseif find_item ( 2016129 ) < Count1 or find_item ( 1000540 ) < Count2 or find_item ( 1000541 ) < Count3 or find_item ( 1000542 ) < Count4 or find_item ( 2016156 ) < Count5 then -- 재료 모자람
			dlg_text( "@90610381")
		else
			dlg_text( "@90610382") -- 제작완료
			
			delete_item( get_item_handle( 2016129 ), Count1 )
			delete_item( get_item_handle( 1000540 ), Count2 )
			delete_item( get_item_handle( 1000541 ), Count3 )
			delete_item( get_item_handle( 1000542 ), Count4 )
			delete_item( get_item_handle( 2016156 ), Count5 )
			change_item_code( get_wear_item_handle( part ), NewItem )
			set_item_ethereal_durability( get_wear_item_handle( part ), get_max_item_ethereal_durability( get_wear_item_handle( part ) ) ) -- 착용된 아이템의 내구도를 최대값으로 채워준다.
			update_wear() -- 방어구 변경후 외형 업데이트
		end
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
end


