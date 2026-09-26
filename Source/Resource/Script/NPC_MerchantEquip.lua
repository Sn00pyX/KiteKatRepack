-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_MerchantEquip"
end

   -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.

   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Equip_Deva_init()
	cprint( "!상인 저스티스 가동" )
	set_npc_name( "@90100200" )
end
 

function NPC_Merchant_Equip_Deva_contact()
 
	-- 다이얼로그 출력
	local state_code = get_local_info() 
	
	dlg_title( "@90100201" )
	dlg_text( "@90100202" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90100203", "open_market( 'flat_sum_deva_equip' )" )
	else
		dlg_menu( "@90100203", "open_market( 'deva_equip' )" )		
		
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Equip_Asura_init()
	cprint( "!상인 데스티너스 가동" )
	set_npc_name( "@90200200"  )
end
 

function NPC_Merchant_Equip_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90200201" )
	dlg_text( "@90200202" )

	
--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
		dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]	

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90200203", "open_market( 'flat_sum_asura_equip' )" )
	else
		dlg_menu( "@90200203", "open_market( 'asura_equip' )" )

	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 초보자섬 NPC >>>>>>
   --============================================================

function NPC_Merchant_Equip_Beginner_init()
	cprint( "!상인 브리비안 가동" )
	set_npc_name(  "@90300200"  )
end
 

function NPC_Merchant_Equip_Beginner_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90300201" )
	dlg_text( "@90300202" )

	dlg_menu( "@90300203", "open_market( 'beginner_equip' )" )
	dlg_menu( "@90010002", '' )

 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================

function NPC_Merchant_Equip_Gaia_init()
	cprint( "!가이아 방어구상인 가동" )
	set_npc_name( "@90400200"  )
end
 

function NPC_Merchant_Equip_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90400201" )
	dlg_text( "@90400202" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90400203", "open_market( 'flat_sum_gaia_equip' )" )
	else
		dlg_menu( "@90400203", "open_market( 'gaia_equip' )" )

	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 론도 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Equip_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90600201" )
	if get_quest_progress(10173) == 1 and find_item( 1100312 ) == 0 then
			dlg_text_without_quest_menu( "@90600202" )
			dlg_menu( "@80010173", "Quest_Link_114_1()" )
		else
			dlg_text( "@90600202" )
	end

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90600203", "open_market( 'flat_sum_rondoh_equip' )" )
	else
		dlg_menu( "@90600203", "open_market( 'rondoh_equip' )" )

	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function Quest_Link_114_1()
	if find_item( 1100312 ) == 0 then
		dlg_title("@90600201")
		dlg_text_without_quest_menu( "@90999592" )
		dlg_menu("@90010002", '')
		dlg_show()

		insert_item( 1100312, 1 )
	else
		dlg_title("@90600201")
		dlg_menu( "@90010002", '' ) 
		dlg_show()		
	end
end

   --============================================================
   --             <<<<<< 도시 유적 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Equip_Ancient_relic_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90703001" )
	dlg_text( "@90703002" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90600203", "open_market( 'flat_sum_Ancient_relic_equip' )" )
	else
		dlg_menu( "@90600203", "open_market( 'Ancient_relic_equip' )" )

	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================
 function NPC_Merchant_Equip_Secroute_contact()
 
		
	-- 다이얼로그 출력
	dlg_title( "@90700201" )
	if is_premium() then
		dlg_text( "@90700202" )
	else
		dlg_text( "@90700118" )
	end

	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	if is_premium() then
		dlg_menu( "@90700203", "open_market( 'secroute_equip' )" )

	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
 
    
function Quest_Link_103_1()
	local count = find_item( 1000024 )
	if count == 0 then
	insert_item( 1000024, 1 )
	else
	end
end

   --============================================================
   --             <<<<<< 시크루트 측 꾸미기상인 NPC >>>>>>
   --============================================================
 function NPC_Merchant_Deco_Secroute_contact()
 
		
	-- 다이얼로그 출력
	dlg_title( "@90606234" )
	if is_premium() then
		dlg_text( "@90606235" )
	else
		dlg_text( "@90606236" )
	end

	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	-- 중동의 경우에는 꾸미기 해적 세트 판매를 못하므로 중동 전용 상점 데이터(secroute_deco_me)를 사용한다
	if is_premium() then
		local flag = get_local_info() -- 국가값 얻어오기
		if flag == 8192 then -- 중동이면
			dlg_menu( "@90606237", "open_market( 'secroute_deco_me' )" )
		else
			dlg_menu( "@90606237", "open_market( 'secroute_deco' )" )
		end
	end
	dlg_menu( "@90606238", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 론도 배틀 아레나 보상 아이템 판매 NPC >>>>>>
   --============================================================
   
   -- 소모품 및 기타 아이템 판매 NPC
function NPC_Merchant_Battlearena_Etc()
 
	-- 다이얼로그 출력
	dlg_title( "@90605155" )
	dlg_text( "@90605156" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605149", "open_market( 'Rondoh_Battlearena_Etc' )" )
	dlg_menu( "@90605150", "open_market( 'Rondoh_Battlearena_consumables' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   -- <고구려 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 1
function NPC_Merchant_Battlearena_DecoEquip_goguryeo()
 
	-- 다이얼로그 출력
	dlg_title( "@90605158" )
	dlg_text( "@90605159" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버

	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_goguryeo_Lv1' )" )
	dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_goguryeo_Lv100' )" )
	dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_goguryeo_Lv150' )" )

	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

-- <방한용 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 2
function NPC_Merchant_Battlearena_DecoEquip_russia()
 
	-- 다이얼로그 출력
	dlg_title( "@90605161" )
	dlg_text( "@90605162" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_russia_Lv1' )" )
	dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_russia_Lv100' )" )
	dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_russia_Lv150' )" )

	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
-- <금장식 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 3
function NPC_Merchant_Battlearena_DecoEquip_phasin()
 
	-- 다이얼로그 출력
	dlg_title( "@90605164" )
	dlg_text( "@90605165" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_phasin_Lv1' )" )
	dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_phasin_Lv100' )" )
	dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_phasin_Lv150' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
-- <닌자 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 4
function NPC_Merchant_Battlearena_DecoEquip_ninja()
 
	-- 다이얼로그 출력
	dlg_title( "@90605167" )
	dlg_text( "@90605168" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_ninja_Lv1' )" )
	dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_ninja_Lv100' )" )
	dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_ninja_Lv150' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
-- <해적 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 5
function NPC_Merchant_Battlearena_DecoEquip_pirate()
 
	-- 다이얼로그 출력
	dlg_title( "@90605170" )
	dlg_text( "@90605171" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_pirate_Lv1' )" )
	dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_pirate_Lv100' )" )
	dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_pirate_Lv150' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
-- <치파오 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 6
function NPC_Merchant_Battlearena_DecoEquip_china()
 
	-- 다이얼로그 출력
	dlg_title( "@90605173" )
	dlg_text( "@90605174" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_china_Lv1' )" )
	dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_china_Lv100' )" )
	dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_china_Lv150' )" )

	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
-- <로마 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 7
function NPC_Merchant_Battlearena_DecoEquip_roma()
 
	-- 다이얼로그 출력
	dlg_title( "@90605176" )
	dlg_text( "@90605177" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_roma_Lv1' )" )
	--dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_roma_Lv100' )" )
	--dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_roma_Lv150' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
-- <고구려 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 8
function NPC_Merchant_Battlearena_DecoEquip_darkmage()
 
	-- 다이얼로그 출력
	dlg_title( "@90605179" )
	dlg_text( "@90605180" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_darkmage_Lv1' )" )
	--dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_darkmage_Lv100' )" )
	--dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_darkmage_Lv150' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
-- <고구려 꾸미기 세트> 꾸미기 보상 아이템 판매 NPC 9
function NPC_Merchant_Battlearena_DecoEquip_indian()
 
	-- 다이얼로그 출력
	dlg_title( "@90605182" )
	dlg_text( "@90605183" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	dlg_menu( "@90605151", "open_market( 'Rondoh_Battlearena_indian_Lv1' )" )
	--dlg_menu( "@90605152", "open_market( 'Rondoh_Battlearena_indian_Lv100' )" )
	--dlg_menu( "@90605153", "open_market( 'Rondoh_Battlearena_indian_Lv150' )" )
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
