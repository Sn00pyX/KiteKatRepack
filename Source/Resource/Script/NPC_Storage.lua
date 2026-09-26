-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_Storage"
end

   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================

function NPC_Storage_Deva_init()
	cprint( "!창고지기 웨이나 가동" )
	set_npc_name(  "@90100900"  )
end
 

function NPC_Storage_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90100901" )
	dlg_text( "@90100902" )

	dlg_menu( "@90100903", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function NPC_Storage_2_Deva_init()
	cprint( "!창고지기 웨인 가동" )
	set_npc_name(  "@90101000"  )
end
 

function NPC_Storage_2_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90101001" )
	dlg_text( "@90101002" )
	
	
--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
    dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]	
	dlg_menu( "@90101003", "open_storage()" )
	dlg_menu( "@90010002", '' )
	

	dlg_show()
 
end

function NPC_Storage_3_Deva_init()
	cprint( "!창고지기 하츠 가동" )
	set_npc_name(  "@90101100"  )
end
 

function NPC_Storage_3_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90101101" )
	dlg_text( "@90101102" )

	dlg_menu( "@90101103", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================

function NPC_Storage_Asura_init()
	cprint( "!창고지기 하이언 가동" )
	set_npc_name(  "@90200900"  )
end
 

function NPC_Storage_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90200901" )
	dlg_text( "@90200902" )

	dlg_menu( "@90200903", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function NPC_Storage_2_Asura_init()
	cprint( "!창고지기 리타 가동" )
	set_npc_name(  "@90201000"  )
end
 

function NPC_Storage_2_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90201001" )
	dlg_text( "@90201002" )

	dlg_menu( "@90201003", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function NPC_Storage_3_Asura_init()
	cprint( "!창고지기 리노 가동" )
	set_npc_name(  "@90201100"  )
end
 

function NPC_Storage_3_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90201101" )
	dlg_text( "@90201102" )

	dlg_menu( "@90201103", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 수련자의 섬 NPC >>>>>>
   --============================================================

function NPC_Storage_Beginner_init()
	cprint( "!창고지기 마니엘 가동" )
	set_npc_name(  "@90300900"  )
end
 

function NPC_Storage_Beginner_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90300901" )
	dlg_text( "@90300902" )

	dlg_menu( "@90300903", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function NPC_Storage_2_Beginner_init()
	cprint( "!창고지기 그웨인 가동" )
	set_npc_name(  "@90301000"  )
end
 

function NPC_Storage_2_Beginner_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90301001" )
	dlg_text( "@90301002" )

	dlg_menu( "@90301003", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end



   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================

function NPC_Storage_Gaia_init()
	cprint( "!가이아 창고지기1 가동" ) --마리카
	set_npc_name(  "@90400900"  )
end
 

function NPC_Storage_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90400901" )
	dlg_text( "@90400902" )
	
--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
    dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]	
	
	dlg_menu( "@90400903", "open_storage()" )
	

	dlg_menu( "@90010002", '' )
	

 
	dlg_show()
 
end

function NPC_Storage_2_Gaia_init()
	cprint( "!가이아 창고지기2 가동" ) --로쉘
	set_npc_name(  "@90401000"  )
end
 

function NPC_Storage_2_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90401001" )
	dlg_text( "@90401002" )

--[[ 할로윈 사탕받기//할로윈 이벤트에만 가동 Npc_event.lua에 있음
    dlg_menu( "@90604959", 'Trick_or_treat_2011()' ) 
----------------------------------------------------------]]	

	dlg_menu( "@90401003", "open_storage()" )

	
	dlg_menu( "@90010002", '' )

 
	dlg_show()
 
end

function NPC_Storage_3_Gaia_init()
	cprint( "!가이아 창고지기3 가동" )
	set_npc_name(  "@90401100"  )
end
 

function NPC_Storage_3_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90401101" )
	dlg_text( "@90401102" )

	dlg_menu( "@90401103", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


   --============================================================
   --             <<<<<< 론도 측 NPC >>>>>>
   --============================================================

function NPC_Storage_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90600901" )
	dlg_text( "@90600902" )

	dlg_menu( "@90600903", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function NPC_Storage_2_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90601001" )
	dlg_text( "@90601002" )

	dlg_menu( "@90601003", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

function NPC_Storage_3_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90601101" )
	dlg_text( "@90601102" )

	dlg_menu( "@90601103", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================

function NPC_Storage_1_Secroute_contact()
 
	dlg_title( "@90700901" )
	
	if is_premium() then
		-- 다이얼로그 출력
		dlg_text( "@90700902" )
	
		dlg_menu( "@90700903", "open_storage()" )
	else
		dlg_text( "@90700118" )
	end
		
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end


-- 창고지기 웨이드
function NPC_Storage_2_Secroute_contact()
 
	dlg_title( "@90999710" )
	
	if is_premium() then
		-- 다이얼로그 출력
		dlg_text( "@90999711" )
	
		dlg_menu( "@90700903", "open_storage()" )
	else
		dlg_text( "@90700118" )
	end
		
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end



   --============================================================
   --             <<<<<< 마레 마을 창고지기 >>>>>>
   --============================================================
   
function NPC_Storage_1_Mare_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90702801" )
	dlg_text( "@90702802" )

	dlg_menu( "@90601103", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end      


   --============================================================
   --             <<<<<< 도시 유적 창고지기 >>>>>>
   --============================================================
-- 창고지기 베이젤드
function NPC_Storage_Ancient_relic_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90703501" )
	dlg_text( "@90703502" )

	dlg_menu( "@90601103", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end      

-- 창고지기 케냐
function NPC_Storage_2_Ancient_relic_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90703601" )
	dlg_text( "@90703602" )

	dlg_menu( "@90601103", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end      


   --============================================================
   --             <<<<<< 잃어버린 비밀의 섬 창고지기 >>>>>>
   --============================================================
   
function NPC_Storage_island_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90760201" )
	dlg_text( "@90760202" )

	dlg_menu( "@90760203", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end      


   --============================================================
   --             <<<<<< 마리캣 마켓 창고지기 >>>>>>
   --============================================================
   
function NPC_Storage_maricat_market_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90999407" )
	dlg_text( "@90999408" )

	dlg_menu( "@90760203", "open_storage()" )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end      


