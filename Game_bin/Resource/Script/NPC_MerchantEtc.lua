-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_MerchantEtc"
end

   -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.


   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Etc_Deva_init()
	cprint( "!잡화상인 크리스테 가동" )
	set_npc_name(  "@90100700"  )
end
 

function NPC_Merchant_Etc_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90100701" )
	dlg_text( "@90100702" )

	dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

-- 프리미엄 테스트 서버용 특수판매상 루스
function NPC_Merchant_Etc_astarot_init()
	cprint( "!특수판매상 루스 가동" )
	set_npc_name(  "@90704600"  )
end
 

function NPC_Merchant_Etc_astarot_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90704601" )
	dlg_text( "@90704602" )

	dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end



   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Etc_Asura_init()
	cprint( "!잡화상인 루너티온 가동" )
	set_npc_name(  "@90200700"  )
end
 

function NPC_Merchant_Etc_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90200701" )

	if get_quest_progress(10173) == 1 and find_item( 1100311 ) == 0 then
			dlg_text_without_quest_menu( "@90200702" )
			dlg_menu( "@80010173", "Quest_Link_118_1()" )
	else
			dlg_text( "@90200702" )
	end
	
	dlg_menu( "@90200703", "open_market( 'sec_sever_merchant_etc' )" )
	
	local qstart_text = get_value( "level" )
	local quest_progress1 =  get_quest_progress(2008)
	local quest_progress100 =  get_quest_progress(2000)
	
	-- 미래를 예언하는 소녀
	if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
	dlg_menu( "@90999617", "quest_rumor15()" )
	end
	
	-- 광신도 암살자
	if quest_progress1 == 255 then
	dlg_menu( "@90999842", "quest_witcharmy2()" )
	end
			
	dlg_menu( "@90010002", '' )
 	dlg_show()
 
end

function Quest_Link_118_1()
	if find_item( 1100311 ) == 0 then
		dlg_title("@90200701")
		dlg_text_without_quest_menu( "@90999591" )
		dlg_menu("@90010002", '')
		dlg_show()

		insert_item( 1100311, 1 )
	else
		dlg_title("@90200701")
		
		dlg_menu( "@90010002", '' ) 
		dlg_show()		
	end
end

-- 세부대화 루너티온
function quest_rumor15()
	-- 다이얼로그 출력
	dlg_title( "@90200701" )

	dlg_text( "@90999618" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_12()" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end

-- 세부대화 
function quest_rumor_a_12()
	-- 다이얼로그 출력
	dlg_title( "@90200701" )

	dlg_text( "@90999623" )
	
	-- 세부대화 1-2, 소녀에 관한 신상정보
	dlg_menu( "@90999627", "quest_rumor_b_12()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 레샤에 관한 정보는 없음.
function quest_rumor_b_12()
	-- 다이얼로그 출력
	dlg_title( "@90200701" )

	-- 세부대화 1-3, 소녀에 관한 위치정보
	dlg_text( "@90999628" )
	
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end


-- 프리미엄 테스트 서버용 특수판매상 루스
function NPC_Merchant_Etc_astarot_init()
	cprint( "!특수판매상 루스 가동" )
	set_npc_name(  "@90704700"  )
end
 

function NPC_Merchant_Etc_astarot_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90704701" )
	dlg_text( "@90704702" )

	dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 초보자섬 NPC >>>>>>
   --============================================================
function NPC_Merchant_Etc_Beginner_init()
	cprint( "!잡화상인 모니크 가동" )
	set_npc_name(  "@90300700"  )
end
 

function NPC_Merchant_Etc_Beginner_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90300701" )
	dlg_text( "@90300702" )

	dlg_menu( "@90300703", "open_market( 'beginner_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Etc_Gaia_init()
	cprint( "!가이아 잡화상인 가동" )
	set_npc_name(  "@90400700"  )
end
 

function NPC_Merchant_Etc_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90400701" )
	dlg_text( "@90400702" )

	dlg_menu( "@90400703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

-- 프리미엄 테스트 서버용 특수판매상 루스
function NPC_Merchant_Etc_astarot_init()
	cprint( "!특수판매상 루스 가동" )
	set_npc_name(  "@90704800"  )
end
 

function NPC_Merchant_Etc_astarot_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90704801" )
	dlg_text( "@90704802" )

	dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
   --============================================================
   --             <<<<<< 론도 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Etc_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90600701" )
	dlg_text( "@90600702" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90600703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 론도 대련장 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Etc2_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90604401" )
	dlg_text( "@90604402" )

	
	dlg_menu( "@90600703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end



   --============================================================
   --             <<<<<< 데바 대련장 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Etc2_lakcity_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90108901" )
	dlg_text( "@90108902" )

	dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 가이아 대련장 측 NPC >>>>>>
   --============================================================
 

function NPC_Merchant_Etc2_gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90409401" )
	dlg_text( "@90409402" )

	dlg_menu( "@90400703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end




   --============================================================
   --             <<<<<< 아수라 대련장 측 NPC >>>>>>
   --============================================================


function NPC_Merchant_Etc2_asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90208701" )
	dlg_text( "@90208702" )

	dlg_menu( "@90200703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end



   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Etc_Secroute_contact()	
	dlg_title( "@90700701" ) -- 다이얼로그 출력
	
	if is_premium() then -- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
		dlg_text( "@90700702" )
		dlg_menu( "@90700703", "open_market( 'sec_sever_secmerchant_etc' )" )
	else
		dlg_text( "@90700118" )
	end

	dlg_menu( "@90010002", '' )
	dlg_show()
end

-- 프리미엄 테스트 서버용 특수판매상 루스
function NPC_Merchant_Etc_astarot_init()
	cprint( "!특수판매상 루스 가동" )
	set_npc_name(  "@90704900"  )
end
 

function NPC_Merchant_Etc_astarot_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90704901" )
	dlg_text( "@90704902" )

	dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end




   --============================================================
   --             <<<<<< 마레 마을 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Etc_Mare_contact()
	dlg_title( "@90702601" )
	dlg_text( "@90702602" )

	dlg_menu( "@90600703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' ) 
	dlg_show() 
end



--============================================================
   --             <<<<<< 도시 유적 마을 측 NPC >>>>>>
 --============================================================

function NPC_Merchant_Etc_Ancient_relic_contact()
	dlg_title( "@90703401" )
	dlg_text( "@90703402" )

	dlg_menu( "@90600703", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' ) 
	dlg_show()
end



   --============================================================
   --             <<<<<< 잃어버린 비밀의 섬 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Etc_island_init()
	cprint( "!잃어버린 비밀의 섬 잡화상인 가동" )
	set_npc_name(  "@90760100"  )
end
 

function NPC_Merchant_Etc_island_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90760101" )
	dlg_text( "@90760102" )

	dlg_menu( "@90760103", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 해안가 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Etc_sealine_init()
	cprint( "!해안가 잡화상인 가동" )
	set_npc_name(  "@90760500"  )
end
 

function NPC_Merchant_Etc_sealine_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90760501" )
	dlg_text( "@90760502" )

	dlg_menu( "@90760503", "open_market( 'sec_sever_merchant_etc' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 꾸미기 이펙트 아이템 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_deco_effect_init()
	cprint( "!꾸미기 이펙트 아이템 상인 가동" )
	set_npc_name(  "@90998000"  )
end
 

function NPC_Merchant_deco_effect_contact()
	dlg_title( "@90998001" )
	dlg_text( "@90998002" )

	dlg_menu( "@90996975", "open_market( 'deco_effect' )" )	
	dlg_menu( "@90010002", '' ) 
	dlg_show() 
end

   --============================================================
   --             <<<<<< 테섭 크루 장비 아이템 판매  NPC >>>>>> -- 이거 안쓰고 있다
   --============================================================
function NPC_Merchant_crushop_equip_init()
	cprint( "!테스트 써버 크루 장비 아이템 상인 가동" )
	set_npc_name(  "@90998003"  )
end
 

function NPC_Merchant_crushop_equip_contact()
	dlg_title( "@90998004" )
	dlg_text( "@90998005" )

	dlg_menu( "@90996975", "open_market( 'crushop_equip' )" )
	dlg_menu( "@90010002", '' ) 
	dlg_show()
end


   --============================================================
   --             <<<<<< 테섭 크루 잡화 아이템 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_crushop_etc_init()
	cprint( "!테스트 써버 크루 잡화 아이템 상인 가동" )
	set_npc_name(  "@90998006"  )
end
 

function NPC_Merchant_crushop_etc_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90998007" )
	dlg_text( "@90998008" )
	
	dlg_menu( "크리쳐 상점", "open_market( 'crushop_creature' )" )				--크리쳐 상점
	dlg_menu( "소환 주문서 상점", "open_market( 'crushop_summon' )" )				--소환 주문서 상점
	dlg_menu( "버프 아이템 상점", "open_market( 'crushop_buff' )" )				--버프 아이템 상점
	dlg_menu( "강화 아이템 상점", "open_market( 'crushop_enhancement' )" )				--강화 아이템 상점
	dlg_menu( "초기화 및 아이템 상점", "open_market( 'crushop_reset' )" )				--초기화 및 아이템 상점
	dlg_menu( "꾸미기 아이템 상점", "open_market( 'crushop_costume' )" )				--꾸미기 아이템 상점
	dlg_menu( "탈것/줍기펫 상점", "open_market( 'crushop_pet' )" )				--탈것/줍기펫 상점
	dlg_menu( "엠블럼 아이템 상점", "open_market( 'crushop_emblem' )" )				--엠블럼 아이템 상점
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

   --============================================================
   --             <<<<<< 테섭 크루 헤어 아이템 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_crushop_hair_init()
	cprint( "!테스트 써버 크루 헤어 아이템 상인 가동" )
	set_npc_name(  "@90998009"  )
end
 

function NPC_Merchant_crushop_hair_contact()
	dlg_title( "@90998010" )
	dlg_text( "@90998011" )

	dlg_menu( "@90996975", "open_market( 'crushop_hair' )" )
	dlg_menu( "@90010002", '' )
	dlg_show()
end

   --============================================================
   --             <<<<<< 테섭 5랭크 액세서리 아이템 확장 아이템 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_5rank_expansion_acc_init()
	cprint( "!테스트 써버 5랭크 액세서리 판매 상인 가동" )
	set_npc_name(  "@90998006"  )
end
 

function NPC_Merchant_5rank_expansion_acc_contact()
	dlg_title( "@90999285" )
	dlg_text( "@90999286" )

	dlg_menu( "@90996975", "open_market( '5rank_expansion_acc_ring' )" )
	dlg_menu( "@90010002", '' )
	dlg_show()
end

--============================================================
   --             <<<<<< 테섭 6랭크 액세서리 아이템 확장 아이템 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_6rank_expansion_acc_init()
	cprint( "!테스트 써버 6랭크 액세서리 판매 상인 가동" )
	set_npc_name(  "@90998006"  )
end
 

function NPC_Merchant_6rank_expansion_acc_contact()
	dlg_title( "@90999287" )
	dlg_text( "@90999288" )

	dlg_menu( "@90996975", "open_market( '6rank_expansion_acc_ring' )" )
	dlg_menu( "@90010002", '' ) 
	dlg_show()
end

--============================================================
   --             <<<<<< 테섭 7랭크 액세서리 아이템 확장 아이템 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_7rank_expansion_acc_init()
	cprint( "!테스트 써버 7랭크 액세서리 판매 상인 가동" )
	set_npc_name(  "@90998006"  )
end
 

function NPC_Merchant_7rank_expansion_acc_contact()
	dlg_title( "@90999287" )
	dlg_text( "@90999288" )

	dlg_menu( "@90996975", "open_market( '7rank_expansion_acc_ring' )" )
	dlg_menu( "@90010002", '' ) 
	dlg_show()
end

--===============================================================
   --             <<<<<< 테섭 라크시 가방 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_expansion_bag_deva_init()
	cprint( "!테스트 써버 가방 제봉사 가동" )
	set_npc_name(  "@90999430"  )
end
 

function NPC_Merchant_expansion_bag_deva_contact()
	dlg_title( "@90999431" )
	dlg_text( "@90999432" )

	dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )
	dlg_menu( "@90010002", '' ) 
	dlg_show()

end


--===============================================================
   --             <<<<<< 테섭 호라이즌 가방 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_expansion_bag_gaia_init()
	cprint( "!테스트 써버 가방 제봉사 가동" )
	set_npc_name(  "@90999433"  )
end
 

function NPC_Merchant_expansion_bag_gaia_contact()
	dlg_title( "@90999434" )
	dlg_text( "@90999435" )

	dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )
	dlg_menu( "@90010002", '' ) 
	dlg_show()
end


--===============================================================
   --             <<<<<< 테섭 카탄 가방 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_expansion_bag_asura_init()
	cprint( "!테스트 써버 가방 제봉사 가동" )
	set_npc_name(  "@90999436"  )
end
 

function NPC_Merchant_expansion_bag_asura_contact()
	dlg_title( "@90999437" )
	dlg_text( "@90999438" )

	dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )
	dlg_menu( "@90010002", '' ) 
	dlg_show()
end


--===============================================================
   --             <<<<<< 테섭 론도 가방 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_expansion_bag_rondoh_init()
	cprint( "!테스트 써버 가방 제봉사 가동" )
	set_npc_name(  "@90999439"  )
end
 

function NPC_Merchant_expansion_bag_rondoh_contact()
	dlg_title( "@90999440" )
	dlg_text( "@90999441" )

	dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )
	dlg_menu( "@90010002", '' )
	dlg_show()

end

   --============================================================
   --             <<<<<< 시크루트 이용권 판매 NPC >>>>>>
   --============================================================
function NPC_Secroute_tickets_init()
	cprint( "!이용권 판매인 데포이나 가동" )
	set_npc_name(  "@90999540"  )
end
 

function NPC_Secroute_tickets_contact()
	local state_code = get_local_info()
	if get_global_variable("disableAuction") == 1 then
		return
	else
		-- 다이얼로그 출력
		dlg_title( "@90999541" )
		
		if is_premium() then
			dlg_text( "@90999542" )
			if state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then --미국유럽
				dlg_menu( "@90100703", "open_market( 'secroute_tickets_US' )" )
			else
				dlg_menu( "@90100703", "open_market( 'secroute_tickets' )" )
			end
		else
			dlg_text( "@90700118" )
		end
		dlg_menu( "@90010002", '' )	 
		dlg_show()
	end
end


   --============================================================
   --             <<<<<< 강화 도우미 테스트 서버 전용 NPC >>>>>>
   --============================================================
function NPC_Merchant_inhance_helper_init()
	cprint( "!이용권 판매인 데포이나 가동" )
	set_npc_name(  "@90999551"  )
end
 

function NPC_Merchant_inhance_helper_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90999552" )
	dlg_text( "@90999553" )

	dlg_menu( "@90100703", "open_market( 'inhance_helper_randombox' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 2010년 하반기 테썹 전용 신규 캐시 아이템 옵션 다양화 판매 NPC >>>>>>
   --============================================================
function NPC_Merchant_crushop_Newequip_Item_init()
	cprint( "!크루샵 상인" )
	set_npc_name(  "@90999551"  )
end
 

function NPC_Merchant_crushop_Newequip_Item_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90999552" )
	dlg_text( "@90999553" )

		dlg_menu( "@91002028", "open_market( 'crushop_Newequip_armors' )" )
		dlg_menu( "@91002029", "open_market( 'crushop_Newequip_helm_2rank' )" )
		dlg_menu( "@91002030", "open_market( 'crushop_Newequip_helm_3rank' )" )
		dlg_menu( "@91002031", "open_market( 'crushop_Newequip_helm_4rank' )" )
		dlg_menu( "@91002032", "open_market( 'crushop_Newequip_helm_5rank' )" )
		dlg_menu( "@91002033", "open_market( 'crushop_Newequip_helm_6rank' )" )
		dlg_menu( "@91002034", "open_market( 'crushop_Newequip_helm_7rank' )" )
		dlg_menu( "@91002035", "open_market( 'crushop_Newequip_weapon_mantle' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function parallelworld_merchant_etc()
	
	dlg_title( "@90610014" )
	dlg_text( "@90610015" )
	
	local state_code = get_local_info()
	
	if state_code == 8192 then -- 중동용
		dlg_menu( "@90610016", "open_market( 'parallelworld_merchant_etc_ME' )" )
	elseif state_code == 1 then -- 한국용
		dlg_menu( "@90610016", "open_market( 'parallelworld_merchant_etc_KR' )" )
	else
		dlg_menu( "@90610016", "open_market( 'parallelworld_merchant_etc' )" )
	end
	
	-- 한국 조각난 보석 교환	
	if state_code == 1 then
		dlg_menu( "@90610201", "HW_merchant_jewel_exchange()" ) -- 보석교환
	end
	
	dlg_menu( "<#00A0F0>|@90610390" , "collector_menu(1,0)" ) 	--  @90610390 : 노말 컬렉트북교환
	dlg_menu( "<#00A0F0>|@90610391" , "collector_menu(2,0)" ) 	--  @90610391 : 레어 컬렉트북교환
	dlg_menu( "<#00A0F0>|@90610392" , "collector_menu(3,0)" ) 	--  @90610392 : 스페셜 컬렉트북교환 
	dlg_menu( "<#00A0F0>|@90610393" , "collector_menu(4,0)" ) 	--  @90610393 : 유니크 컬렉트북교환 
	
	dlg_menu( "@90010002", '' )	
	dlg_show()
	
end

function collector_menu( page, exchange)
	local pitem	 =	
	{	-- 컬렉션,  보스카드,  필요갯수. 1,2,3,4
		307201,	307001,	10,	307202,	307002,	10,	307203,	307003,	10,	307204,	307004,	10,	307205,	307005,	10,	307206,	307006,	10,	307207,	307007,	10,
		307208,	307008,	10,	307209,	307009,	10,	307210,	307010,	10,	307211,	307011,	10,	307212,	307012,	10,	307213,	307013,	10,	307214,	307014,	10,

		307215,	307015,	7,	307216,	307016,	7,	307217,	307017,	7,	307218,	307018,	7,	307219,	307019,	7,	307220,	307020,	7,	307221,	307021,	7,
		307222,	307022,	7,	307223,	307023,	7,	307224,	307024,	7,	307225,	307025,	7,	307226,	307026,	7,	0,	0,	0,	0,	0,	0,

		307229,	307029,	5,	307230,	307030,	5,	307231,	307031,	5,	307232,	307032,	5,	307233,	307033,	5,	307234,	307034,	5,	307235,	307035,	5,
		307236,	307036,	5,	307245,	307045,	5,	307246,	307046,	5,	307247,	307047,	5,	307248,	307048,	5,	307249,	307049,	5,	0,	0,	0,

		307240,	307040,	3,	307241,	307041,	3,	307242,	307042,	3,	307243,	307043,	3,	307244,	307044,	3,	307250,	307050,	3,	307251,	307051,	3,
		307252,	307052,	3,	307253,	307053,	3,	307254,	307054,	3,	307255,	307055,	3,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	}

	dlg_title( "@90610014" )
	if exchange == 0 then
		dlg_text(  "@90610394" ) 			--  교환할 컬렉션을 선택하시오
	else
		local idx = exchange-1
		local collect 	= pitem[ idx * 3 + 1]
		local card	= pitem[ idx * 3 + 2]
		local count	= pitem[ idx * 3 + 3]
		local num_collect = find_item( collect )
		local num_card  = find_item( card )

		-- 벨트 장착 검사 장착 검사
		--local is_weard = get_item_in_belt(card)

		--if  is_weard ==1 then
		--	dlg_text(  "90610398")    -- 장착중인 아이템은 교환할수 없습니다.
		--else
		if  num_collect > 0 then
			dlg_text(  "@90610395")    -- 이미 수집한 컬렉션입니다
		elseif num_card < count then
			dlg_text(  "@90610396")    -- 카드가 부족합니다.
		else
			dlg_text(  "@" .. collect .. "|@90610397")    --  을 획득하였습니다
			delete_item( get_item_handle( card ), count )
			insert_item( collect, 1 )
		end
	end

	for i = 0, 13 do
		local idx	= (page-1)*14 + i
		local collect 	= pitem[ idx * 3 + 1]
		local card	= pitem[ idx * 3 + 2]
		local count	= pitem[ idx * 3 + 3]
		local num_collect = find_item( collect )
		local num_card  = find_item( card )
		if collect == 0 then
			break
		end
		if num_collect > 0 then
			dlg_menu("<#00FF00>|@" .. collect , "collector_menu(" .. page .. "," .. idx + 1 .. ")")  
		elseif num_card < count then
			dlg_menu("<#F28F90>|@" .. card .. " | " .. num_card .. "/" ..  count , "collector_menu(" .. page .. "," .. idx + 1 .. ")" )  
		else
			dlg_menu("<#F28F90>|@" .. card .. " | " .. num_card .. "/" ..  count , "collector_menu(" .. page .. "," .. idx + 1 .. ")" )  

		end
	end				
		
	dlg_menu( "@90605284", "parallelworld_merchant_etc()" ) -- 돌아가기	
	dlg_show()

end
----------------------------------컬렉션 끝------------------------------------
function HW_merchant_jewel_exchange()
	dlg_title( "@90610014" )
	dlg_text( "@90610202" ) -- 보석상자 교환가능..
	
	dlg_menu( "@90610207", "HW_merchant_jewel_exchange_select(1)" ) -- 조각난  수정 보석 교환
	dlg_menu( "@90610208", "HW_merchant_jewel_exchange_select(2)" ) -- 조각난  달빛  보석 교환
	dlg_menu( "@90610209", "HW_merchant_jewel_exchange_select(3)" ) -- 조각난 크리스탈 보석 교환
	dlg_menu( "@90610210", "HW_merchant_jewel_exchange_select(4)" ) -- 조각난 팔미르 보석 교환
	dlg_menu( "@90610211", "HW_merchant_jewel_exchange_select(5)" ) -- 조각난 검은 보석 교환
	dlg_menu( "@90610212", "HW_merchant_jewel_exchange_select(6)" ) -- 조각난 사념체 결정 교환
	
	dlg_menu( "@90605284", "parallelworld_merchant_etc()" ) -- 돌아가기	
	dlg_menu( "@90010002", '' )	-- 대화 종료
	
	dlg_show()
end

function HW_merchant_jewel_exchange_select(num) -- 조각난 보석 교환
	dlg_title( "@90610014" )
	dlg_text( "@90610203" ) -- 조각 선택하시오
	if num == 1 then
		dlg_menu( "@11000492", "HW_merchant_jewel_exchange_done(1000528,1000492)" ) -- 조각난 수정 보석 1
		dlg_menu( "@11000493", "HW_merchant_jewel_exchange_done(1000528,1000493)" ) -- 조각난 수정 보석 2
		dlg_menu( "@11000494", "HW_merchant_jewel_exchange_done(1000528,1000494)" ) -- 조각난 수정 보석 3
		dlg_menu( "@11000495", "HW_merchant_jewel_exchange_done(1000528,1000495)" ) -- 조각난 수정 보석 4
	elseif num == 2 then
		dlg_menu( "@11000502", "HW_merchant_jewel_exchange_done(1000529,1000502)" ) -- 조각난 달빛 보석 1
		dlg_menu( "@11000503", "HW_merchant_jewel_exchange_done(1000529,1000503)" ) -- 조각난 달빛 보석 2
		dlg_menu( "@11000504", "HW_merchant_jewel_exchange_done(1000529,1000504)" ) -- 조각난 달빛 보석 3
		dlg_menu( "@11000505", "HW_merchant_jewel_exchange_done(1000529,1000505)" ) -- 조각난 달빛 보석 4
	elseif num == 3 then
		dlg_menu( "@11000512", "HW_merchant_jewel_exchange_done(1000530,1000512)" ) -- 조각난 크리스탈 보석 1
		dlg_menu( "@11000513", "HW_merchant_jewel_exchange_done(1000530,1000513)" ) -- 조각난 크리스탈 보석 2
		dlg_menu( "@11000514", "HW_merchant_jewel_exchange_done(1000530,1000514)" ) -- 조각난 크리스탈 보석 3
		dlg_menu( "@11000515", "HW_merchant_jewel_exchange_done(1000530,1000515)" ) -- 조각난 크리스탈 보석 4
	elseif num == 4 then
		dlg_menu( "@11000497", "HW_merchant_jewel_exchange_done(1000531,1000497)" ) -- 조각난 팔미르 보석 1
		dlg_menu( "@11000498", "HW_merchant_jewel_exchange_done(1000531,1000498)" ) -- 조각난 팔미르 보석 2
		dlg_menu( "@11000499", "HW_merchant_jewel_exchange_done(1000531,1000499)" ) -- 조각난 팔미르 보석 3
		dlg_menu( "@11000500", "HW_merchant_jewel_exchange_done(1000531,1000500)" ) -- 조각난 팔미르 보석 4
	elseif num == 5 then
		dlg_menu( "@11000507", "HW_merchant_jewel_exchange_done(1000532,1000507)" ) -- 조각난 검은 보석 1
		dlg_menu( "@11000508", "HW_merchant_jewel_exchange_done(1000532,1000508)" ) -- 조각난 검은 보석 2
		dlg_menu( "@11000509", "HW_merchant_jewel_exchange_done(1000532,1000509)" ) -- 조각난 검은 보석 3
		dlg_menu( "@11000510", "HW_merchant_jewel_exchange_done(1000532,1000510)" ) -- 조각난 검은 보석 4
	elseif num == 6 then
		dlg_menu( "@11000517", "HW_merchant_jewel_exchange_done(1000533,1000517)" ) -- 조각난 사념체 결정 1
		dlg_menu( "@11000518", "HW_merchant_jewel_exchange_done(1000533,1000518)" ) -- 조각난 사념체 결정 2
		dlg_menu( "@11000519", "HW_merchant_jewel_exchange_done(1000533,1000519)" ) -- 조각난 사념체 결정 3
		dlg_menu( "@11000520", "HW_merchant_jewel_exchange_done(1000533,1000520)" ) -- 조각난 사념체 결정 4
	end
	
	dlg_menu( "@90605284", "parallelworld_merchant_etc()" ) -- 돌아가기	
	dlg_menu( "@90010002", '' )	-- 대화 종료
	
	dlg_show()
	
end

function HW_merchant_jewel_exchange_done(box, item) -- 교환 완료
	local Jewel_Count = find_item( item )
	local gold = get_value( "gold" )
	
	dlg_title( "@90610014" )
	
	if Jewel_Count < 1 then
		dlg_text( "@90610205" ) -- 조각난 보석 부족	
		
	elseif gold < 10000000 then
		dlg_text( "@90610206" ) --루피 부족
		
	elseif Jewel_Count >= 1 and gold >= 10000000 then		
		dlg_text( "@90610204" ) --조건 만족..
		
		delete_item( get_item_handle( item ), 1 )
		set_value( "gold", gold - 10000000 )
        update_gold_chaos()
		insert_item( box, 1  )
	end
	
	dlg_menu( "@90605284", "parallelworld_merchant_etc()" ) -- 돌아가기	
	dlg_menu( "@90010002", '' )	-- 대화 종료	
end

function NPC_Test_Item_contact()
 
	-- 다이얼로그 출력
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title( npc_name )
	dlg_text( "@90704602" )

	dlg_menu( "@90100703", "open_market( 'TestServer_Item_Shop' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end