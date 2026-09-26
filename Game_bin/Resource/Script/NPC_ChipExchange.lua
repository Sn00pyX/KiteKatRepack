-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_ChipExchange"
end

-- 칩 교환상 클릭시 호출 함수
function NPC_ChipExchange_contact()

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	-- 라크 1 모아오기 퀘스트 상태 채크	
	local quest_progress_lak =  get_quest_progress(1032)
	
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	local qstart_text = get_value( "level" )
	local quest_progress1 =  get_quest_progress(2008)
	local quest_progress100 =  get_quest_progress(2000)
	
	-- <대사 출력> 각 NPC에 따라 적합한 
	-- 가이아 일 때 (4015 부스트칩교환상 란티스)
	if npc_id == 4015 then
		dlg_text("@90401502")
		
		-- 미래를 예언하는 소녀
		if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
		dlg_menu( "@90999617", "quest_rumor()" )
		end
		
		-- 광신도 암살자
		if quest_progress1 == 255 then
		dlg_menu( "@90999842", "quest_witcharmy2()" )
		end
		
	-- 데바 일 때 (1015	부스트칩교환상 마린느)
	elseif npc_id == 1015 then
		dlg_text("@90101502")
		
		-- 미래를 예언하는 소녀
		if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
		dlg_menu( "@90999617", "quest_rumor2()" )
		end
		
		-- 광신도 암살자
		if quest_progress1 == 255 then
		dlg_menu( "@90999842", "quest_witcharmy1()" )
		end
			
	-- 아수라 일 때 (2015 부스트칩교환상 라피스)
	elseif npc_id == 2015 then
		dlg_text("@90201502")
	-- 수련자의 섬 슈리르 일 때 (3023 부스트칩교환상 슈리르) 슈리르는 따로 처리
--	elseif npc_id == 3023 then
--		dlg_text("@90302302")
	-- 론도 일 때 (6015 라크 트레이더 치우)
	elseif npc_id == 6015 then
		dlg_text("@90601502")
	-- 도시 유적 일 때 (7039 라크 트레이더 티레)
	elseif npc_id == 7039 then
		dlg_text("@90703902")
	-- 잃어버린 비밀의 섬 일 때 (7603 라크 트레이더 그란체스터)
	elseif npc_id == 7603 then
		dlg_text("@90760302")
	-- 해안가 캠프 일 때 (7606 라크 트레이더 피카툰)
	elseif npc_id == 7606 then
		dlg_text("@90760602")
	-- 마리캣 마켓 일 때 (11130 라크 트레이더 시나)
	elseif npc_id == 11130 then
		dlg_text("@90999411")	
	-- 시크루트 일 때 (7015 라크 트레이더 루빈스키)
	elseif npc_id == 7015 then
		if is_premium() then
			dlg_text("@90701502")
		else
			dlg_text( "@90700118" )
		end

	end

	
	-- 라크 퀘스트가 종료가능할 경우(라크를 모아왔을 경우) 교환이나 환전은 퀘스트 완료하지 않으면 불가능하다는 대사 출력.	
	if quest_progress_lak == 2 then
		dlg_text("@90302324")
	else	
	
		if npc_id == 7015 and is_premium() == false then
		else
			-- <공통 메뉴>
			-- 라크를 부스트 칩으로 교환
			dlg_menu( "@90010077", 'ChipExchange_Exchange_Menu( 4, 1 )' )
			-- 라크를 루피로 환전
			dlg_menu( "@90010078", 'ChipExchange_lak_to_RP()' )
			-- 라크를 JP로 환전
			dlg_menu( "@90610513", 'ChipExchange_lak_to_JP()' )
			
			-- 시크루트 일 때 (7015 라크 트레이더 루빈스키)
			if npc_id == 7015 then
				if is_premium() then
				-- 부스트 칩을 구입한다.
				dlg_menu( "@90010079", "open_market( 'secroute_boost_chip' )" )
				end
			else
				-- 부스트 칩을 구입한다.
				dlg_menu( "@90010079", "open_market( 'boost_chip' )" )
			end
			
			-- 에테리얼 스톤량을 캐릭터 에테리얼로 전환한다.
			--dlg_menu( "@91002068", 'etherealExchange_select()' )		
			-- 빈 에테리얼 스톤과 촉진제를 루피로 환전한다.
			--dlg_menu( "@91002069", 'Exchange_ethereal_and_accelerator_to_RP()' )
		end
	end
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()

end


	
-- 세부대화
function quest_rumor()
	-- 다이얼로그 출력
	dlg_title("@90401501")

	dlg_text( "@90999618" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화
function quest_rumor_a()
	-- 다이얼로그 출력
	dlg_title("@90401501")

	dlg_text( "@90999622" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999625", "quest_rumor_b()" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화
function quest_rumor_b()
	-- 다이얼로그 출력
	dlg_title("@90401501")

	dlg_text( "@90999626" )
	
	-- 세부대화 1-1b, 미래를 내다보는 소녀
	dlg_menu( "@90999627", "quest_rumor_c()" )
		
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 결국 레샤에 관한 정보는 없음.
function quest_rumor_c()
	-- 다이얼로그 출력
	dlg_title("@90401501")

	dlg_text( "@90999628" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 마린느
function quest_rumor2()
	-- 다이얼로그 출력
	dlg_title("@90101501")

	dlg_text( "@90999619" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 라크를 부스트 칩으로 교환한다.
function ChipExchange_lak_to_chip()

-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)	
	
	-- <대사 출력> 각 NPC에 따라 적합한 
	-- 가이아 일 때 (4015 부스트칩교환상 란티스)
	if npc_id == 4015 then
		dlg_text_without_quest_menu("@90401503")
	-- 데바 일 때 (1015	부스트칩교환상 마린느)
	elseif npc_id == 1015 then
		dlg_text_without_quest_menu("@90101503")
	-- 아수라 일 때 (2015 부스트칩교환상 라피스)
	elseif npc_id == 2015 then
		dlg_text_without_quest_menu("@90201503")
	-- 수련자의 섬 슈리르 일 때 (3023 부스트칩교환상 슈리르)
	elseif npc_id == 3023 then
		dlg_text_without_quest_menu("@90302303")		
	-- 론도 일 때 (6015 라크 트레이더 치우)
	elseif npc_id == 6015 then
		dlg_text_without_quest_menu("@90601503")
	-- 도시 유적 일 때 (7039 라크 트레이더 티레)
	elseif npc_id == 7039 then
		dlg_text_without_quest_menu("@90703903")
	-- 잃어버린 비밀의 섬 일 때 (7603 라크 트레이더 그란체스터)
	elseif npc_id == 7603 then
		dlg_text_without_quest_menu("@90760303")
	-- 해안가 캠프 일 때 (7606 라크 트레이더 피카툰)
	elseif npc_id == 7606 then
		dlg_text_without_quest_menu("@90760603")
	-- 시크루트 일 때 (7015 라크 트레이더 루빈스키)
	elseif npc_id == 7015 then
		dlg_text_without_quest_menu("@90701503")
	-- 마리캣 마켓 일 때 (11130 라크 트레이더 시나)
	elseif npc_id == 11130 then
		dlg_text_without_quest_menu("@90999412")
	end
	
	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end


	-- <공통 메뉴>
	-- ChipExchange_chip_choice ( 칩종류 ) // 1 : 포스칩, 2 : 소울칩, 3 : 루나칩, 4 스킬: 루나칩
	-- 포스칩.
	--dlg_menu( "@90010081", 'ChipExchange_chip_choice( 1 )' )
	-- 소울칩.
	--dlg_menu( "@90010080", 'ChipExchange_chip_choice( 2 )' )
	-- 루나칩.
	--dlg_menu( "@90010241", 'ChipExchange_chip_choice( 3 )' )
	
	-- 루나칩.
	dlg_menu( "@90010241", 'ChipExchange_chip_choice( 4 )' )
	
		
	-- 수련자의 섬 슈리르 일 때 (3023 부스트칩교환상 슈리르)
	if npc_id == 3023 then
		--돌아가기
		dlg_menu( "@90010003", 'NPC_ChipExchange_beginner_contact()' )
	else
		--돌아가기
		dlg_menu( "@90010003", 'NPC_ChipExchange_contact()' )
	end
		
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()

end

-- 칩의 랭크를 선택한다.
function ChipExchange_chip_choice( select )

-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)	
	
	-- <대사 출력> 각 NPC에 따라 적합한 
	-- 가이아 일 때 (4015 부스트칩교환상 란티스)
	if npc_id == 4015 then
		dlg_text_without_quest_menu("@90401507")
	-- 데바 일 때 (1015	부스트칩교환상 마린느)
	elseif npc_id == 1015 then
		dlg_text_without_quest_menu("@90101507")
	-- 아수라 일 때 (2015 부스트칩교환상 라피스)
	elseif npc_id == 2015 then
		dlg_text_without_quest_menu("@90201507")
	-- 수련자의 섬 슈리르 일 때 (3023 부스트칩교환상 슈리르)
	elseif npc_id == 3023 then
		dlg_text_without_quest_menu("@90302322")		
	-- 론도 일 때 (6015 라크 트레이더 치우)
	elseif npc_id == 6015 then
		dlg_text_without_quest_menu("@90601507")
	-- 도시 유적 일 때 (7039 라크 트레이더 티레)
	elseif npc_id == 7039 then
		dlg_text_without_quest_menu("@90703907")
	-- 잃어버린 비밀의 섬 일 때 (7603 라크 트레이더 그란체스터)
	elseif npc_id == 7603 then
		dlg_text_without_quest_menu("@90760307")
	-- 해안가 캠프 일 때 (7606 라크 트레이더 피카툰)
	elseif npc_id == 7606 then
		dlg_text_without_quest_menu("@90760607")
	-- 시크루트 일 때 (7015 라크 트레이더 루빈스키)
	elseif npc_id == 7015 then
		dlg_text_without_quest_menu("@90701507")
	-- 마리캣 마켓 일 때 (11130 라크 트레이더 시나)
	elseif npc_id == 11130 then
		dlg_text_without_quest_menu("@90999413")
	end

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end
	

	-- 포스칩 일 때
	if select == 1 then
		-- 수련자의 섬 메뉴 (1랭크 포스칩만..)
		-- ChipExchange_Select_Menu ( 칩종류, 칩랭크 )
		dlg_menu( "@90010095", 'ChipExchange_Exchange_Menu( ' .. select .. ', 1 )' )
		
		if npc_id ~= 3023 then
			-- <공통 메뉴>
			-- 2랭크 포스칩.
			dlg_menu( "@90010096", 'ChipExchange_Exchange_Menu( ' .. select .. ', 2 )' )
			-- 3랭크 포스칩.
			dlg_menu( "@90010097", 'ChipExchange_Exchange_Menu( ' .. select .. ', 3 )' )
			-- 4랭크 포스칩.
			dlg_menu( "@10690004", 'ChipExchange_Exchange_Menu( ' .. select .. ', 4 )' )
			-- 5랭크 포스칩.
			dlg_menu( "@10690005", 'ChipExchange_Exchange_Menu( ' .. select .. ', 5 )' )
			-- 6랭크 포스칩.
			dlg_menu( "@10690006", 'ChipExchange_Exchange_Menu( ' .. select .. ', 6 )' )
			-- 7랭크 포스칩.
			dlg_menu( "@10690007", 'ChipExchange_Exchange_Menu( ' .. select .. ', 7 )' )
		end
					
	-- 소울칩 일 때
	elseif select == 2 then
	-- 수련자의 섬 메뉴 (1랭크 소울칩만..)
		dlg_menu( "@90010101", 'ChipExchange_Exchange_Menu( ' .. select .. ', 1 )' )
		
		if npc_id ~= 3023 then
		    -- 2랭크 소울칩.
			dlg_menu( "@90010102", 'ChipExchange_Exchange_Menu( ' .. select .. ', 2 )' )
			-- 3랭크 소울칩.
			dlg_menu( "@90010103", 'ChipExchange_Exchange_Menu( ' .. select .. ', 3 )' )
			-- 4랭크 포스칩.
			dlg_menu( "@10690104", 'ChipExchange_Exchange_Menu( ' .. select .. ', 4 )' )
			-- 5랭크 포스칩.
			dlg_menu( "@10690105", 'ChipExchange_Exchange_Menu( ' .. select .. ', 5 )' )
			-- 6랭크 포스칩.
			dlg_menu( "@10690106", 'ChipExchange_Exchange_Menu( ' .. select .. ', 6 )' )
			-- 7랭크 포스칩.
			dlg_menu( "@10690107", 'ChipExchange_Exchange_Menu( ' .. select .. ', 7 )' )
		end
		
	-- 루나칩 일 때
	elseif select == 3 then
	-- 수련자의 섬 메뉴 (1랭크 루나칩만..)
		dlg_menu( "@90010242", 'ChipExchange_Exchange_Menu( ' .. select .. ', 1 )' )
		
		if npc_id ~= 3023 then
		    -- 2랭크 루나칩.
			dlg_menu( "@90010243", 'ChipExchange_Exchange_Menu( ' .. select .. ', 2 )' )
			-- 3랭크 루나칩.
			dlg_menu( "@90010244", 'ChipExchange_Exchange_Menu( ' .. select .. ', 3 )' )
			-- 4랭크 포스칩.
			dlg_menu( "@10690204", 'ChipExchange_Exchange_Menu( ' .. select .. ', 4 )' )
			-- 5랭크 포스칩.
			dlg_menu( "@10690205", 'ChipExchange_Exchange_Menu( ' .. select .. ', 5 )' )
			-- 6랭크 포스칩.
			dlg_menu( "@10690206", 'ChipExchange_Exchange_Menu( ' .. select .. ', 6 )' )
			-- 7랭크 포스칩.
			dlg_menu( "@10690207", 'ChipExchange_Exchange_Menu( ' .. select .. ', 7 )' )
		end

	-- 스킬: 루나칩 일 때
	elseif select == 4 then
	-- 수련자의 섬 메뉴 (1랭크 루나칩만..)
		dlg_menu( "@90010242", 'ChipExchange_Exchange_Menu( ' .. select .. ', 1 )' )
		
		--if npc_id ~= 3023 then
		    -- 2랭크 루나칩.
			--dlg_menu( "@90010243", 'ChipExchange_Exchange_Menu( ' .. select .. ', 2 )' )
			-- 3랭크 루나칩.
			--dlg_menu( "@90010244", 'ChipExchange_Exchange_Menu( ' .. select .. ', 3 )' )
			-- 4랭크 포스칩.
			--dlg_menu( "@10690204", 'ChipExchange_Exchange_Menu( ' .. select .. ', 4 )' )
			-- 5랭크 포스칩.
			--dlg_menu( "@10690205", 'ChipExchange_Exchange_Menu( ' .. select .. ', 5 )' )
			-- 6랭크 포스칩.
			--dlg_menu( "@10690206", 'ChipExchange_Exchange_Menu( ' .. select .. ', 6 )' )
			-- 7랭크 포스칩.
			--dlg_menu( "@10690207", 'ChipExchange_Exchange_Menu( ' .. select .. ', 7 )' )
		--end	
		
	end

	--돌아가기
	dlg_menu( "@90010003", 'ChipExchange_lak_to_chip()' )
			
	--대화 종료
	dlg_menu( "@90010002", "" )
		
		
	-- 다이얼로그 출력하기
	dlg_show()

end


function ChipExchange_Exchange_Menu( chip_type, chip_rank )

-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end

	local numrac, numofchip, text, price, handle_1_rank_force,price_1_rank_force
	
	handle_1_rank_force = get_item_handle( 690001 )	
	
	-- 포스칩 가격 가져 오기
	price_1_rank_force = get_item_price( handle_1_rank_force)
	local price_2_rank_force = get_item_price( 690002)
	local price_3_rank_force = get_item_price( 690003)
	local price_4_rank_force = get_item_price( 690004)
	local price_5_rank_force = get_item_price( 690005)
	local price_6_rank_force = get_item_price( 690006)
	local price_7_rank_force = get_item_price( 690007)
	
	-- 소울칩 가격 가져 오기
	local price_1_rank_soul = get_item_price( 690101)
	local price_2_rank_soul = get_item_price( 690102)
	local price_3_rank_soul = get_item_price( 690103)
	local price_4_rank_soul = get_item_price( 690104)
	local price_5_rank_soul = get_item_price( 690105)
	local price_6_rank_soul = get_item_price( 690106)
	local price_7_rank_soul = get_item_price( 690107)
	
	-- 루나칩 가격 가져 오기
	local price_1_rank_luna = get_item_price( 690201)
	local price_2_rank_luna = get_item_price( 690202)
	local price_3_rank_luna = get_item_price( 690203)
	local price_4_rank_luna = get_item_price( 690204)
	local price_5_rank_luna = get_item_price( 690205)
	local price_6_rank_luna = get_item_price( 690206)
	local price_7_rank_luna = get_item_price( 690207)
	
	-- 루나칩 가격 가져 오기
	local price_1_rank_skill_luna = get_item_price( 690303)

	
	-- 일반 환율은 250
	local exchange_rate = 250

	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)이고 시크루트일 때,
	-- 프리미엄 서비스 체크 주석처리
	
	if is_premium() then
		if npc_id == 7015 then
			exchange_rate = 375
		end
	end
	
	-- 랭크에 따라 칩 가격 결정.
	-- 1랭크 포스, 소울칩의 가격은 100, 루나칩의 가격은 120 (라크를 칩으로 교환시에는 20% 할인 판매)
	if chip_rank == 1 then
	    if chip_type == 1 then
	       price = 80
	    elseif chip_type == 2 then
	       price = 80
	    elseif chip_type == 3 then
		   price = 104
		elseif chip_type == 4 then
		   price = 104
	    end
	-- 2랭크 포스, 소울칩의 가격은 200, 루나칩의 가격은 240 (라크를 칩으로 교환시에는 20% 할인 판매)
	elseif chip_rank == 2 then
	    if chip_type == 1 then
	       price = 160
	    elseif chip_type == 2 then
	       price = 160
	    elseif chip_type == 3 then 
		   price = 208
		elseif chip_type == 4 then 
		   price = 208
	    end
	-- 3랭크 포스, 소울칩의 가격은 400, 루나칩의 가격은 480 (라크를 칩으로 교환시에는 20% 할인 판매)
	elseif chip_rank == 3 then
	    if chip_type == 1 then
	       price = 320   
	    elseif chip_type == 2 then
	       price = 320
	    elseif chip_type == 3 then
		   price = 416
		elseif chip_type == 4 then
		   price = 416
	    end
	-- 4랭크 포스, 소울칩의 가격은 400, 루나칩의 가격은 480 (라크를 칩으로 교환시에는 20% 할인 판매)
	elseif chip_rank == 4 then
	    if chip_type == 1 then
	       price = 560   
	    elseif chip_type == 2 then
	       price = 560
	    elseif chip_type == 3 then
		   price = 728
		elseif chip_type == 4 then
		   price = 728
	    end
	-- 5랭크 포스, 소울칩의 가격은 400, 루나칩의 가격은 480 (라크를 칩으로 교환시에는 20% 할인 판매)
	elseif chip_rank == 5 then
	    if chip_type == 1 then
	       price = 880   
	    elseif chip_type == 2 then
	       price = 880
	    elseif chip_type == 3 then
		   price = 1144
		elseif chip_type == 4 then
		   price = 1144
	    end
	-- 6랭크 포스, 소울칩의 가격은 400, 루나칩의 가격은 480 (라크를 칩으로 교환시에는 20% 할인 판매)
	elseif chip_rank == 6 then
	    if chip_type == 1 then
	       price = 1280   
	    elseif chip_type == 2 then
	       price = 1280
	    elseif chip_type == 3 then
		   price = 1664
		elseif chip_type == 4 then
		   price = 1664
	    end
	-- 7랭크 포스, 소울칩의 가격은 400, 루나칩의 가격은 480 (라크를 칩으로 교환시에는 20% 할인 판매)
	elseif chip_rank == 7 then
	    if chip_type == 1 then
	       price = 1760 
	    elseif chip_type == 2 then
	       price = 1760
	    elseif chip_type == 3 then
		   price = 2288
		elseif chip_type == 4 then
		   price = 2288
	    end
    end
	
	-- 변경된 기준환율 250, 
	numrac = gv("chaos")    -- 현재 수집한 라크의 양을 가져온다.
	numofchip = math.floor(numrac * exchange_rate / price)     -- 적당한 계산으로 판매 가능한 최대 칩의 개수를 계산한다.

	local default_num, num1, num2, num3, num4

	-- 현재 가진 라크량에 따라메뉴 구성
	-- 10 미만일 때
	if numrac < 100 then	
		-- 최소 교환 개수 계산
		num1 = math.floor ( exchange_rate / price )
		-- 환율보다 칩 가격이 높을 때, 그냥 무시하고 2로 통일.
		if num1 == 0 then
			num1 = 2
		end
		num2 = num1 * 2
		num3 = num1 * 4
		num4 = num1 * 8
	-- 10 이상일 때
	else
		num1 = 10
			
		if numrac >= 5000 then
			default_num = 5000

		elseif numrac >= 3000 then
			default_num = 3000
		else
			default_num = math.floor( numrac * 0.01 ) * 100
		end

		num2 = default_num / 4
		num3 = default_num / 2
		num4 = default_num
	end
	
	-- <대사 출력> 각 NPC에 따라 적합한  
	-- 가이아 일 때 (4015 부스트칩교환상 란티스).
	if npc_id == 4015 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.	
		end
			
	-- 데바 일 때 (1015	부스트칩교환상 마린느)
	elseif npc_id == 1015 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
			
		end
			
	-- 아수라 일 때 (2015 부스트칩교환상 라피스)
	elseif npc_id == 2015 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		end
		
	-- 수련자의 섬 슈리르 일 때 (3023 부스트칩교환상 슈리르)
	elseif npc_id == 3023 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		end

	-- 론도 일 때 (6015 라크 트레이더 치우)
	elseif npc_id == 6015 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.	
		end

	-- 도시 유적 일 때 (7039 라크 트레이더 티레)
	elseif npc_id == 7039 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		end
		
	-- 잃어버린 비밀의 섬 일 때 (7603 라크 트레이더 그란체스터)
	elseif npc_id == 7603 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		end
		
	-- 해안가 캠프 일 때 (7606 라크 트레이더 피카툰)
	elseif npc_id == 7606 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		end
		
	-- 시크루트 일 때 (7015 라크 트레이더 루빈스키)
	elseif npc_id == 7015 then
		if chip_type	== 1 then
			text = sconv("@90401504", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90401505", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90401509", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		end
		
	-- 마리캣 마켓 일 때 (11130 라크 트레이더 시나)
	elseif npc_id == 11130 then
		if chip_type	== 1 then
			text = sconv("@90999414", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ) ,  "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 2 then
			text = sconv("@90999415", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 3 then
			text = sconv("@90999416", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		elseif chip_type	== 4 then
			text = sconv("@90999416", "#@rac@#", tostring( numrac ) , "#@ranktype@#", tostring( chip_rank ), "#@number@#",tostring(numofchip))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		end		
	end 
		
	dlg_text_without_quest_menu( text )       -- 대사창에 텍스트 삽입
		
	-- <공통 메뉴>
	-- 전부 교환.
	if numofchip > 0 then
		dlg_menu( "@90010083", 'ChipExchange_Do_Exchange(' .. chip_type.. ', ' .. chip_rank .. ', ' .. numrac .. ', ' ..  exchange_rate.. ', ' .. price .. ')' )
	end
		

	-- function ChipExchange_Do_Exchange( 칩타입, 칩랭크, 교환할라크량, 기준환율, 칩가격 )  // 칩타입  1 : 포스칩, 2 : 소울칩, 3 : 루나칩
	if numrac >= num1 then
		-- #@number@#라크만 교환. num1
		text = sconv("@90010082", "#@number@#", tostring( num1 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_Do_Exchange(' .. chip_type.. ', ' .. chip_rank .. ', ' .. num1 .. ', ' ..  exchange_rate.. ', ' .. price .. ')' )
	end
	if numrac >= num2 then
		-- #@number@#라크만 교환. num2
		text = sconv("@90010082", "#@number@#", tostring( num2 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_Do_Exchange(' .. chip_type.. ', ' .. chip_rank .. ', ' .. num2 .. ', ' ..  exchange_rate.. ', ' .. price .. ')' )
	end
	if numrac >= num3 then
		-- #@number@#라크만 교환. num3
		text = sconv("@90010082", "#@number@#", tostring( num3 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_Do_Exchange(' .. chip_type.. ', ' .. chip_rank .. ', ' .. num3 .. ', ' ..  exchange_rate.. ', ' .. price .. ')' )
	end
	if numrac >= num4 then
		-- #@number@#라크만 교환. num4
		text = sconv("@90010082", "#@number@#", tostring( num4 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_Do_Exchange(' .. chip_type.. ', ' .. chip_rank .. ', ' .. num4 .. ', ' ..  exchange_rate.. ', ' .. price .. ')' )
	end
	
	--돌아가기
	--dlg_menu( "@90010003", 'ChipExchange_chip_choice( ' .. chip_type.. ' )' )
	
	
	dlg_menu( "@90010003", 'NPC_ChipExchange_contact()' )
		
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()

end

function ChipExchange_Do_Exchange( chip_type, chip_rank, lak, exchange_rate, price )

	local temp_lak, i, numofchip, player_lak, item_id, item_id_500, item_id_1000, numofchip_bag
	-- 라크 1 모아오기 퀘스트 상태 채크	
	local quest_progress_lak =  get_quest_progress(1032)	
	
	player_lak = gv("chaos")
	
	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end

	
	-- 포스칩
	if chip_type == 1 then
		if chip_rank == 1 then
			item_id = 690001
		elseif chip_rank == 2 then
			item_id = 690002
		elseif chip_rank == 3 then
			item_id = 690003
		elseif chip_rank == 4 then
			item_id = 690004
		elseif chip_rank == 5 then
			item_id = 690005
		elseif chip_rank == 6 then
			item_id = 690006
		elseif chip_rank == 7 then
			item_id = 690007
		end
	-- 소울칩
	elseif chip_type == 2 then
		if chip_rank == 1 then
			item_id = 690101
		elseif chip_rank == 2 then
			item_id = 690102
		elseif chip_rank == 3 then
			item_id = 690103
		elseif chip_rank == 4 then
			item_id = 690104
		elseif chip_rank == 5 then
			item_id = 690105
		elseif chip_rank == 6 then
			item_id = 690106
		elseif chip_rank == 7 then
			item_id = 690107
		end
	-- 루나칩
	elseif chip_type == 3 then
		if chip_rank == 1 then
			item_id = 690201
		elseif chip_rank == 2 then
			item_id = 690202
		elseif chip_rank == 3 then
			item_id = 690203
		elseif chip_rank == 4 then
			item_id = 690204
		elseif chip_rank == 5 then
			item_id = 690205
		elseif chip_rank == 6 then
			item_id = 690206
		elseif chip_rank == 7 then
			item_id = 690207
		end
		
	-- 스킬: 루나칩
	elseif chip_type == 4 then
		if chip_rank == 1 then
			item_id = 690303
			item_id_500 =690321
			item_id_1000 =690322
		elseif chip_rank == 2 then
			item_id = 690303
			item_id_500 =690321
			item_id_1000 =690322
		elseif chip_rank == 3 then
			item_id = 690303
			item_id_500 =690321
			item_id_1000 =690322
		elseif chip_rank == 4 then
			item_id = 690303
			item_id_500 =690321
			item_id_1000 =690322
		elseif chip_rank == 5 then
			item_id = 690303
			item_id_500 =690321
			item_id_1000 =690322
		elseif chip_rank == 6 then
			item_id = 690303
			item_id_500 =690321
			item_id_1000 =690322
		elseif chip_rank == 7 then
			item_id = 690303
			item_id_500 =690321
			item_id_1000 =690322
		end
	end
	
	
	-- 라크 퀘스트가 종료가능할 경우(라크를 모아왔을 경우) 교환이나 환전은 퀘스트 완료하지 않으면 불가능하다는 대사 출력.	
	if quest_progress_lak == 2 then
		set_auto_user( 1 )	
		add_state( 5997, 10, 8640000 )
		kick_auto_to_another_world()				
		
		elseif player_lak >= lak then
	
			temp_lak = player_lak - lak
		
			-- 라크를 칩으로 교환시에는 80%로 판매
			numofchip = math.floor(lak * exchange_rate / price)     -- 적당한 계산으로 판매 가능한 최대 칩의 개수를 계산한다.
			
			-- 라크를 까고
			sv("chaos", temp_lak )
			update_gold_chaos()
			
			-- 칩 개수가 1000이 넘는 경우
			if numofchip >= 1000 then
			
				numofchip_bag = math.floor( numofchip/1000 )
				numofchip = ((numofchip/1000) - numofchip_bag)*1000
				
				insert_item( item_id_1000, numofchip_bag )
				insert_item( item_id, numofchip )
			
			else
				-- 칩을 넣어준다.
				insert_item( item_id, numofchip )
				
			end
	end

end

-- 라크를 루피로 환전한다.
function ChipExchange_lak_to_RP()

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end


-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	local numrac, exchange_rupy, text, nummoney, player_money
	
	-- 일반 환율은 250
	--local exchange_rate = 250
	local exchange_rate = 125

	-- 프리미엄 서비스 체크 주석처리	
	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)이고 시크루트일 때에는,
	-- 일반 환율의 50%(250 * 0.5)인 125를 추가 지급한다.
	if is_premium() then
		if npc_id == 7015 then
			--exchange_rate = exchange_rate + 125
			exchange_rate = exchange_rate + 63
		end
	end

	-- 억만장자(호칭 ID : 69)를 주 호칭으로 사용 시,
	-- 일반 환율의 10%(250 * 0.1)인 25를 추가 지급한다.
	if get_main_title() == 69 then
		--exchange_rate = exchange_rate + 25
		exchange_rate = exchange_rate + 13

	-- 끝없는 부를 가진 자(호칭 ID : 70)를 주 호칭으로 사용 시,
	-- 일반 환율의 20%(250 * 0.2)인 50를 추가 지급한다.
	elseif get_main_title() == 70 then
		--exchange_rate = exchange_rate + 50
		exchange_rate = exchange_rate + 25
	end

	numrac = gv("chaos")    -- 현재 수집한 라크의 양을 가져온다.
	exchange_rupy = math.floor(numrac * exchange_rate)     -- 환전 가능한 최대 루피의 량을 계산한다.
	player_money = gv("gold")	--	유저 현재 소지금을 가져온다.
	nummoney = exchange_rupy + player_money	--	유저 현재 소지금과 환전금액을 합친다.
	if nummoney > 100000000000 then	--	환전가능한 최대 루피 량과 유저 현재 소지금을 합쳐서 1000억이 넘는지 체크 한다.
		dlg_text("@90760310")	-- 	소지금이 초과되었다는 메시지출력
		dlg_menu( "@90010003", 'NPC_ChipExchange_contact()' )	-- 돌아가기
		dlg_show()	-- 보이기
		return
	end	
	
	-- <대사 출력> 각 NPC에 따라 적합한 
	-- 가이아 일 때 (4015 부스트칩교환상 란티스).
	if npc_id == 4015 then			
		text = sconv("@90401506", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 데바 일 때 (1015	부스트칩교환상 마린느)
	elseif npc_id == 1015 then
		text = sconv("@90101506", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 아수라 일 때 (2015 부스트칩교환상 라피스)
	elseif npc_id == 2015 then
		text = sconv("@90201506", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 수련자의 섬 슈리르 일 때 (3023 부스트칩교환상 슈리르)
	elseif npc_id == 3023 then
		text = sconv("@90302306", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 론도 일 때 (6015 라크 트레이더 치우)
	elseif npc_id == 6015 then
		text = sconv("@90601506", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 도시 유적 일 때 (7039 라크 트레이더 티레)
	elseif npc_id == 7039 then
		text = sconv("@90703906", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 잃어버린 비밀의 섬 일 때 (7603 라크 트레이더 그란체스터)
	elseif npc_id == 7603 then
		text = sconv("@90760306", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 해안가 캠프 일 때 (7606 라크 트레이더 피카툰)
	elseif npc_id == 7606 then
		text = sconv("@90760606", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 시크루트 일 때 (7015 라크 트레이더 루빈스키)
	elseif npc_id == 7015 then
		text = sconv("@90701506", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	-- 마리캣 마켓 일 때 (11130 라크 트레이더 시나)
	elseif npc_id == 11130 then
		text = sconv("@90999417", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_rupy))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
	end 
	dlg_text_without_quest_menu( text )       -- 대사창에 텍스트 삽입
		
		-- <공통 메뉴>
		-- 전부 교환.
	if numrac > 0 then
		dlg_menu( "@90010083", 'ChipExchange_RP_num(' .. numrac .. ', ' .. exchange_rate .. ')' )
	end
		
	local num1, num2, num3, num4

	num1 = 10

	if numrac <10 then
		num1 = 1
		num2 = 5
		num3 = 10
		num4 = 20
	
	elseif numrac < 200 then
		num2 = 30
		num3 = 50
		num4 = 100
		
	elseif numrac < 500 then
		num2 = 50
		num3 = 100
		num4 = 300
	
	elseif numrac < 5000 then
		num2 = 50
		num3 = 200
		num4 = 500

	else
		num1 = 5000
		num2 = 7000
		num3 = 8000
		num4 = 9000

	end
		
	if numrac >= num1 then
		-- #@number@#라크만 교환. num1
		text = sconv("@90010082", "#@number@#", tostring( num1 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_RP_num(' .. num1 .. ', ' .. exchange_rate .. ')' )
	end
	if numrac >= num2 then
		-- #@number@#라크만 교환. num2

		text = sconv("@90010082", "#@number@#", tostring( num2 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_RP_num(' .. num2 .. ', ' .. exchange_rate .. ')' )
	end
	if numrac >= num3 then
		-- #@number@#라크만 교환. num3

		text = sconv("@90010082", "#@number@#", tostring( num3 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_RP_num(' .. num3 .. ', ' .. exchange_rate .. ')' )
	end
	if numrac >= num4 then
		-- #@number@#라크만 교환. num4

		text = sconv("@90010082", "#@number@#", tostring( num4 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_RP_num(' .. num4 .. ', ' .. exchange_rate .. ')' )
	end

	-- 수련자의 섬 슈리르 일 때 (3023 부스트칩교환상 슈리르)
	if npc_id == 3023 then
		--돌아가기
		dlg_menu( "@90010003", 'NPC_ChipExchange_beginner_contact()' )
	else
		--돌아가기
		dlg_menu( "@90010003", 'NPC_ChipExchange_contact()' )
	end
						
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()

end

-- 루피 환전 (특정값)
function ChipExchange_RP_num( lak, exchange_rate )

	-- 라크 1 모아오기 퀘스트 상태 채크	
	local quest_progress_lak =  get_quest_progress(1032)
	local temp_money, temp_lak
	local player_money, player_lak
		
	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end
	
	player_money = gv("gold")
	player_lak = gv("chaos")
	
	-- 라크 퀘스트가 종료가능할 경우(라크를 모아왔을 경우) 교환이나 환전은 퀘스트 완료하지 않으면 불가능하다는 대사 출력.	
	if quest_progress_lak == 2 then
		set_auto_user( 1 )
		add_state( 5997, 10, 8640000 )
		kick_auto_to_another_world()		
		
		elseif player_lak >= lak then
	
			-- 변경된 기준환율 250, 1랭크 칩의 가격은 100 (라크를 칩으로 교환시에는 20프로 할인 판매)
			temp_money = player_money + ( lak * exchange_rate )
			temp_lak = player_lak - lak
			
			-- 돈을 넣어주고
			sv("gold", temp_money )
			update_gold_chaos()
			-- 라크를 깐다.
			sv("chaos", temp_lak )
			update_gold_chaos()
	end

	--환전시 호칭 부여
	update_title_condition( 9002003, lak )			
end
---------------라크를 JP로 교환---------------
function ChipExchange_lak_to_JP()

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end


-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)
	
	local exchange_rate = 10
	
	local numrac, exchange_jp, player_jp, text, numjp

	numrac = gv("chaos")    -- 현재 수집한 라크의 양을 가져온다.
	exchange_jp = math.floor(numrac * exchange_rate)     -- 환전 가능한 최대 JP량을 계산한다.
	player_jp = gv("jp")	--	유저 현재 JP를 가져온다.
	numjp = exchange_jp + player_jp	--	유저 현재 소지 jp과 환전 jp를 합친다.
	if numjp > 99999000000 then	--	환전가능한 최대 JP 량과 유저 현재 JP를 합쳐서 99,999,000,000이 넘는지 체크 한다.
		dlg_text("@90610514")	-- 	최대 JP가 초과되었다는 메시지출력
		dlg_menu( "@90010003", 'NPC_ChipExchange_contact()' )	-- 돌아가기
		dlg_show()	-- 보이기
		return
	end
	
	text = sconv("@90610515", "#@rac@#", tostring( numrac ) , "#@number@#",tostring(exchange_jp))   -- 변수를 실제 값(스트링)으로 치환 시킨다.	
	dlg_text_without_quest_menu( text )       -- 대사창에 텍스트 삽입
		
		-- <공통 메뉴>
		-- 전부 교환.
	if numrac > 0 then
		dlg_menu( "@90010083", 'ChipExchange_JP_num(' .. numrac .. ')' )
	end
		
	local num1, num2, num3, num4

	num1 = 10

	if numrac <10 then
		num1 = 1
		num2 = 5
		num3 = 10
		num4 = 20
	
	elseif numrac < 200 then
		num2 = 30
		num3 = 50
		num4 = 100
		
	elseif numrac < 500 then
		num2 = 50
		num3 = 100
		num4 = 300
	
	elseif numrac < 5000 then
		num2 = 50
		num3 = 200
		num4 = 500

	else
		num1 = 5000
		num2 = 7000
		num3 = 8000
		num4 = 9000

	end
		
	if numrac >= num1 then
		-- #@number@#라크만 교환. num1
		text = sconv("@90010082", "#@number@#", tostring( num1 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_JP_num(' .. num1 ..')' )
	end
	if numrac >= num2 then
		-- #@number@#라크만 교환. num2

		text = sconv("@90010082", "#@number@#", tostring( num2 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_JP_num(' .. num2 ..')' )
	end
	if numrac >= num3 then
		-- #@number@#라크만 교환. num3

		text = sconv("@90010082", "#@number@#", tostring( num3 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_JP_num(' .. num3 ..')' )
	end
	if numrac >= num4 then
		-- #@number@#라크만 교환. num4

		text = sconv("@90010082", "#@number@#", tostring( num4 ) )   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_menu( text, 'ChipExchange_JP_num(' .. num4 .. ')' )
	end

	-- 수련자의 섬 슈리르 일 때 (3023 부스트칩교환상 슈리르)
	if npc_id == 3023 then
		--돌아가기
		dlg_menu( "@90010003", 'NPC_ChipExchange_beginner_contact()' )
	else
		--돌아가기
		dlg_menu( "@90010003", 'NPC_ChipExchange_contact()' )
	end
						
	--대화 종료
	dlg_menu( "@90010002", "" )

	-- 다이얼로그 출력하기
	dlg_show()

end

-- JP 환전
function ChipExchange_JP_num( lak )

	-- 라크 1 모아오기 퀘스트 상태 채크	
	local quest_progress_lak =  get_quest_progress(1032)
	local temp_lak, player_lak
		
	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end
	
	player_lak = gv("chaos")
	
	-- 라크 퀘스트가 종료가능할 경우(라크를 모아왔을 경우) 교환이나 환전은 퀘스트 완료하지 않으면 불가능하다는 대사 출력.	
	if quest_progress_lak == 2 then
		set_auto_user( 1 )
		add_state( 5997, 10, 8640000 )
		kick_auto_to_another_world()
		
	elseif player_lak >= lak then
		temp_lak = player_lak - lak
		
		-- JP 넣어주고
		add_exp_jp( 0, lak * 10 )
		-- 라크를 깐다.
		sv("chaos", temp_lak )
		update_gold_chaos()
		
		message( sconv( "@90610516", "#@jp_num@#", lak * 10) )
	end
	
	
end
-----------라크를 JP로 교환 끝--------------

function etherealExchange_select()

-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end

	dlg_text("@91002070")	-- 	캐릭터 에테리얼로 전환할 에테리얼 스톤을 선택해주세요
	local checked_item_id = {1100501, 1100502, 1100503, 1100504, 1100505, 1100506, 1100507, 1100508}
	local existed_item_id = {}
	local existed_item_count = 0

	local i
	for i = 1, 8 do
		local count = find_item( checked_item_id[i] )
		if count ~= 0 then
			existed_item_count = existed_item_count + 1
			existed_item_id[existed_item_count] = checked_item_id[i]
		end
	end

	for i = 1, existed_item_count do
		dlg_menu( tostring( get_item_name_by_code( existed_item_id[i] ) ), "etherealExchange_stone_to_character(" .. existed_item_id[i] .. ")" )
	end

	dlg_menu( "@90010003", "NPC_ChipExchange_contact()" )	-- 돌아가기
	dlg_show()	-- 보이기
end

function etherealExchange_stone_to_character( item_code )
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end		


	dlg_text("@91002071")	-- 	캐릭터 에테리얼로 전환할 에테리얼 스톤을 선택해주세요	

	local found_item_handle = get_item_handle_list( item_code )
	
	local i
	for i = 1, table.getn( found_item_handle ) do
		dlg_menu( tostring( get_item_name( found_item_handle[i] ) ) .. "(" .. math.ceil( get_item_ethereal_durability( found_item_handle[i] ) / 10000 ) .. ")", "etherealExchange_calculator(" .. found_item_handle[i] .. ")" )
	end

	dlg_menu( "@90010003", "NPC_ChipExchange_contact()" )	-- 돌아가기
	dlg_show()	-- 보이기
end

function etherealExchange_calculator( item_handle )
	if is_erasable_item( item_handle ) == 0 then
		return
	end

	local item_ethereal = get_item_ethereal_durability( item_handle )
	local character_ethereal = gv( 'ethereal_stone' )
	local charge_ethereal = 100000000 - character_ethereal

	if charge_ethereal > item_ethereal then
		charge_ethereal = item_ethereal
	end

	set_item_ethereal_durability( item_handle, item_ethereal - charge_ethereal )
	sv( 'ethereal_stone', character_ethereal + charge_ethereal )

	item_ethereal = get_item_ethereal_durability( item_handle )
	if item_ethereal < 10000 then
		ethereal_and_accelertor_delete( item_handle, 1 ) 
	end
end

function Exchange_ethereal_and_accelerator_to_RP()
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()	
	dlg_title(npc_name)

	-- 시크루트 NPC 일 때 시크루트가 아니면
	if get_npc_id() == 7015 then
		if is_premium() == false then
			return false
		end
	end	


	dlg_text("@91002074")	-- 	빈 에테리얼 스톤과 촉진제 모두 루피로 교환	
	-- 에테리얼 스톤 8종
	local checked_item_id = {1100501, 1100502, 1100503, 1100504, 1100505, 1100506, 1100507, 1100508}

	local found_item_handle = {}
	local i

	for i = 1, 8 do
		found_item_handle = get_item_handle_list( checked_item_id[i] )
		
		for j = 1, table.getn( found_item_handle ) do
			if get_item_ethereal_durability( found_item_handle[j] ) < 10000 then
				ethereal_and_accelertor_delete( found_item_handle[j], 1 )
			end
		end
	end

	-- 촉매 3종
	checked_item_id = {1100601, 1100602, 1100603}
	for i = 1, 3 do
		local count = find_item( checked_item_id[i] )
		ethereal_and_accelertor_delete( get_item_handle( checked_item_id[i] ), count )
	end

	dlg_menu( "@90010003", "NPC_ChipExchange_contact()" )	-- 돌아가기
	dlg_show()	-- 보이기
end

function ethereal_and_accelertor_delete( item_handle, count )
	if is_erasable_item( item_handle ) == 0 or count == 0 then 
		return
	end

	local price_table = {}
	price_table[ 1100501 ] = 100000 
	price_table[ 1100502 ] = 5000000 
	price_table[ 1100503 ] = 10000000 
	price_table[ 1100504 ] = 10
	price_table[ 1100505 ] = 10
	price_table[ 1100506 ] = 10
	price_table[ 1100507 ] = 10
	price_table[ 1100508 ] = 10
	price_table[ 1100601 ] = 1000
	price_table[ 1100602 ] = 500
	price_table[ 1100603 ] = 200000

	sv( 'gold', gv( 'gold' ) + price_table[ get_item_code( item_handle ) ] * count )
	update_gold_chaos()

	delete_item( item_handle, count )
end
