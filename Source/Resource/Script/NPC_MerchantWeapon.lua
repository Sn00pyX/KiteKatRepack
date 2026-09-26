-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_MerchantWeapon"
end

   -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.

   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Weapon_Deva_init()
	cprint( "!데바 무기상인 가동" )
	set_npc_name(  "@90101300"  )
end

function NPC_Merchant_Weapon_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90101301" )
	dlg_text( "@90101302" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90010062", "open_market( 'flat_sum_deva_weapon' )" )
	else
		dlg_menu( "@90010062", "open_market( 'deva_weapon' )" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================ 
function NPC_Merchant_Weapon_Asura_init()
	cprint( "!아수라 무기상인 가동" )
	set_npc_name(  "@90201300"  )
end
 
 -- 무기상인 소렌
function NPC_Merchant_Weapon_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90201301" )
	dlg_text( "@90201302" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90010062", "open_market( 'flat_sum_asura_weapon' )" )
	else
		dlg_menu( "@90010062", "open_market( 'asura_weapon' )" )
	end
	
	local qstart_text = get_value( "level" )
	local quest_progress1 =  get_quest_progress(2008)
	local quest_progress100 =  get_quest_progress(2000)
	
	-- 미래를 예언하는 소녀
	if qstart_text == 50 or qstart_text > 50 and quest_progress100 == 0 then
	dlg_menu( "@90999617", "quest_rumor14()" )
	end
	
	-- 광신도 암살자
	if quest_progress1 == 255 then
	dlg_menu( "@90999842", "quest_witcharmy2()" )
	end
	
	dlg_menu( "@90010002", '' )
 	dlg_show()
 
end

-- 세부대화 소렌
function quest_rumor14()
	-- 다이얼로그 출력
	dlg_title( "@90201301" )

	dlg_text( "@90999620" )
	
	-- 세부대화 1-1, 미래를 내다보는 소녀
	dlg_menu( "@90999621", "quest_rumor_a_11()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 
function quest_rumor_a_11()
	-- 다이얼로그 출력
	dlg_title( "@90201301" )

	dlg_text( "@90999624" )
	
	-- 세부대화 1-2, 소녀에 관한 신상정보
	dlg_menu( "@90999627", "quest_rumor_b_11()" )
	
	dlg_menu( "@90010002", '' ) 	
	dlg_show()

end

-- 세부대화 레샤에 관한 정보를 준다.
function quest_rumor_b_11()
	-- 다이얼로그 출력
	dlg_title( "@90201301" )

	dlg_text( "@90999631" )
	
	dlg_menu( "@90010002", '' ) 			
	dlg_show()

end



   --============================================================
   --             <<<<<< 수련자의 섬 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Weapon_Beginner_init()
	cprint( "!수련자의 섬 무기상인 가동" )
	set_npc_name(  "@90302100"  )
end

function NPC_Merchant_Weapon_Beginner_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90302101" )
	dlg_text( "@90302102" )

	dlg_menu( "@90010062", "open_market( 'beginner_weapon' )" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Weapon_Gaia_init()
	cprint( "!가이아 무기상인 가동" )
	set_npc_name(  "@90401300"  )
end

function NPC_Merchant_Weapon_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90401301" )
	dlg_text( "@90401302" )


	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90010062", "open_market( 'flat_sum_gaia_weapon' )" )
	else
		dlg_menu( "@90010062", "open_market( 'gaia_weapon' )" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 론도 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Weapon_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90601301" )
	dlg_text( "@90601302" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90010062", "open_market( 'flat_sum_rondoh_weapon' )" )
	else
		dlg_menu( "@90010062", "open_market( 'rondoh_weapon' )" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 도시 유적 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Weapon_Ancient_relic_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90703801" )
	dlg_text( "@90703802" )

	-- game.cash_usable_server ? 1: 시크루트 관련 내용 사용 서버  /  0: 사용 안 하는 서버
	if get_env("game.cash_usable_server") == 0 then
		dlg_menu( "@90010062", "open_market( 'flat_sum_Ancient_relic_weapon' )" )
	else
		dlg_menu( "@90010062", "open_market( 'Ancient_relic_weapon' )" )
	end
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================
function NPC_Merchant_Weapon_Secroute_contact()
 
		
	-- 다이얼로그 출력
	dlg_title( "@90701301" )
	if is_premium() then
		dlg_text( "@90701302" )
	else
		dlg_text( "@90700118" )
	end


	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	if is_premium() then
		dlg_menu( "@90010062", "open_market( 'secroute_weapon' )" )
	end
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


function Quest_Link_105_1()
	local count = find_item( 1000026 )
	if count == 0 then
	insert_item( 1000026, 1 )
	else
	end
end




