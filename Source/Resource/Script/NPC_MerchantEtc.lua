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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90100703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90100703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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



	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90200703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90200703", "open_market( 'sec_sever_merchant_etc' )" )
	end
	
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90100703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90400703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90400703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90100703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90600703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90600703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90600703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90600703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90100703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90400703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90400703", "open_market( 'sec_sever_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90200703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90200703", "open_market( 'sec_sever_merchant_etc' )" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end



   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Etc_Secroute_contact()

 
	-- 다이얼로그 출력
	dlg_title( "@90700701" )
	if is_premium() then
		dlg_text( "@90700702" )
	else
		dlg_text( "@90700118" )
	end

	-- 국가 코드 읽어옴
	local state_code = get_local_info() 	
	
	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	if is_premium() then
		-- 얻어온 국가 코드가 한국(1)이라면
		if state_code == 1 then
--			dlg_menu( "@90700703", "open_market( 'secroute_etc_KR' )" )
			dlg_menu( "@90700703", "open_market( 'sec_sever_secmerchant_etc' )" )
		else
		-- 얻어온 국가 코드가 한국이 아니라면
			dlg_menu( "@90700703", "open_market( 'sec_sever_secmerchant_etc' )" )
		end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90100703", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90100703", "open_market( 'sec_sever_merchant_etc' )" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end




   --============================================================
   --             <<<<<< 마레 마을 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Etc_Mare_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90702601" )
	dlg_text( "@90702602" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90600703", "open_market( 'nosec_sever_mare_merchant_etc' )" )
	else
		dlg_menu( "@90600703", "open_market( 'sec_sever_mare_merchant_etc' )" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end



--============================================================
   --             <<<<<< 도시 유적 마을 측 NPC >>>>>>
 --============================================================

function NPC_Merchant_Etc_Ancient_relic_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90703401" )
	dlg_text( "@90703402" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90600703", "open_market( 'nosec_sever_mare_merchant_etc' )" )
	else
		dlg_menu( "@90600703", "open_market( 'sec_sever_mare_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90760103", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90760103", "open_market( 'sec_sever_merchant_etc' )" )
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90760503", "open_market( 'nosec_sever_merchant_etc' )" )
	else
		dlg_menu( "@90760503", "open_market( 'sec_sever_merchant_etc' )" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 크리쳐 카드판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_Creature_card_init()
	cprint( "!크리쳐 카드 상인 가동" )
	set_npc_name(  "@90996972"  )
end
 

function NPC_Merchant_Creature_card_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90996973" )
	dlg_text( "@90996974" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'flat_sum_Creature_card' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'Creature_card' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90998001" )
	dlg_text( "@90998002" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'flat_sum_deco_effect' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'deco_effect' )" )				--부분유료화 서버
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 테섭 크루 장비 아이템 판매  NPC >>>>>>
   --============================================================
function NPC_Merchant_crushop_equip_init()
	cprint( "!테스트 써버 크루 장비 아이템 상인 가동" )
	set_npc_name(  "@90998003"  )
end
 

function NPC_Merchant_crushop_equip_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90998004" )
	dlg_text( "@90998005" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'flat_sum_crushop_equip' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'crushop_equip' )" )			--부분유료화 서버
	end
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

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'flat_sum_crushop_etc' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'crushop_etc' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90998010" )
	dlg_text( "@90998011" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'flat_sum_crushop_etc' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'crushop_hair' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90999285" )
	dlg_text( "@90999286" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'flat_sum_5rank_expansion_acc_ring' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( '5rank_expansion_acc_ring' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90999287" )
	dlg_text( "@90999288" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'flat_sum_6rank_expansion_acc_ring' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( '6rank_expansion_acc_ring' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90999287" )
	dlg_text( "@90999288" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'flat_sum_7rank_expansion_acc_ring' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( '7rank_expansion_acc_ring' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90999431" )
	dlg_text( "@90999432" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90999434" )
	dlg_text( "@90999435" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90999437" )
	dlg_text( "@90999438" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90999440" )
	dlg_text( "@90999441" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )		--정액제 서버
	else
		dlg_menu( "@90996975", "open_market( 'expansion_acc_bag' )" )				--부분유료화 서버
	end
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
 
	-- 다이얼로그 출력
	dlg_title( "@90999541" )
	
	if is_premium() then
		dlg_text( "@90999542" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	--if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90100703", "open_market( 'secroute_tickets' )" )
	--else
		--dlg_menu( "@90100703", "open_market( 'deva_etc' )" )
	--end
	else
		dlg_text( "@90700118" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
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

   --============================================================
   --             <<<<<< 2013년 epic 8.2 update 버전 불카누스 입장 아이템 판매 NPC >>>>>>
   --============================================================

function NPC_bulcanus_joinitem_sell()
 
	-- 다이얼로그 출력
	dlg_title( "@90605268" )
	dlg_text( "@90605790" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90600703", "open_market( 'nosec_sever_bulcajoin' )" )
	else
		dlg_menu( "@90600703", "open_market( 'sec_sever_bulcajoin' )" )
	end
	dlg_menu( "@690000088", 'NPC_bulcanus_best_record()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function NPC_bulcanus_joinitem_sell_sec()

 
	-- 다이얼로그 출력
	dlg_title( "@90605268" )
	if is_premium() then
		dlg_text( "@90605790" )
	else
		dlg_text( "@90605790" )
	end

	-- 국가 코드 읽어옴
	local state_code = get_local_info() 	
	
	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	if is_premium() then
		-- 얻어온 국가 코드가 한국(1)이라면
		if state_code == 1 then
--			dlg_menu( "@90700703", "open_market( 'secroute_etc_KR' )" )
			dlg_menu( "@90700703", "open_market( 'sec_sever_bulcajoin_sec' )" )
		else
		-- 얻어온 국가 코드가 한국이 아니라면
			dlg_menu( "@90700703", "open_market( 'sec_sever_bulcajoin_sec' )" )
		end
	end
	dlg_menu( "@690000088", 'NPC_bulcanus_best_record()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
	
end
   
function NPC_bulcanus_best_record()

	local best_vul_record = get_global_variable( "best_vul_record" )
	local best_vul_record_name = get_global_variable("best_vul_record_name")

	dlg_title( "@90605268" )
	
	if best_vul_record == nil or best_vul_record == "" then
		dlg_text( "@90605863" )
	else
		local country_time = get_os_date("%H",0)
		local print_best_vul_record = best_vul_record +(24 -country_time) * 3600 
		dlg_text( sconv("@90605862", "#@user_name@#", get_global_variable( "best_vul_record_name" ), "#@vul_record@#", get_os_date("%X",print_best_vul_record)) )
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end

function parallelworld_merchant_etc()

	dlg_title( "@90610014" )
	dlg_text( "@90610015" )
	
	dlg_menu( "@90610016", "open_market( 'parallelworld_merchant_etc' )" )
	
	dlg_menu( "@90010002", '' )
	dlg_show()
	
end
