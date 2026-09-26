-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_GuideTown"
end

 
   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================

function NPC_GuideTown_Deva_init()
	cprint( "!가이드 프로메스 가동" )
	set_npc_name( "@90100800" )
end
 

function NPC_GuideTown_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90100801" )
	dlg_text( "@90100802" )

	dlg_menu( "@90010012", 'GuideTown_JobChange_Deva()' )
	dlg_menu( "@90010013", 'GuideTown_Merchant_Deva()' )
	dlg_menu( "@90010014", 'GuideTown_ItemUp_Deva()' )
	dlg_menu( "@90010015", 'GuideTown_Teleport_Deva()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

	-- 전직안내 펑션
function GuideTown_JobChange_Deva()

	-- 다이얼로그 출력
	dlg_title( "@90100801" )
	dlg_text( "@90100803" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Deva_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 상점안내 펑션
function GuideTown_Merchant_Deva()

	-- 다이얼로그 출력
	dlg_title( "@90100801" )
	dlg_text( "@90100804" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Deva_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 업글안내 펑션
function GuideTown_ItemUp_Deva()

	-- 다이얼로그 출력
	dlg_title( "@90100801" )
	dlg_text( "@90100805" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Deva_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 필드이동안내 펑션
function GuideTown_Teleport_Deva()

	-- 다이얼로그 출력
	dlg_title( "@90100801" )
	dlg_text( "@90100806" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Deva_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end



   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================

function NPC_GuideTown_Asura_init()
	cprint( "!가이드 메피스트 가동" )
	set_npc_name(  "@90200800" )
end

 
function NPC_GuideTown_Asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90200801" )
	dlg_text( "@90200802" )

	dlg_menu( "@90010012", 'GuideTown_JobChange_Asura()' )
	dlg_menu( "@90010013", 'GuideTown_Merchant_Asura()' )
	dlg_menu( "@90010014", 'GuideTown_ItemUp_Asura()' )
	dlg_menu( "@90010015", 'GuideTown_Teleport_Asura()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

	-- 전직안내 펑션
function GuideTown_JobChange_Asura()

	-- 다이얼로그 출력
	dlg_title( "@90200801" )
	dlg_text( "@90200803" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Asura_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 상점안내 펑션
function GuideTown_Merchant_Asura()

	-- 다이얼로그 출력
	dlg_title( "@90200801" )
	dlg_text( "@90200804" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Asura_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 업글안내 펑션
function GuideTown_ItemUp_Asura()

	-- 다이얼로그 출력
	dlg_title( "@90200801" )
	dlg_text( "@90200805" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Asura_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 필드이동안내 펑션
function GuideTown_Teleport_Asura()

	-- 다이얼로그 출력
	dlg_title( "@90200801" )
	dlg_text( "@90200806" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Asura_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end


   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================

function NPC_GuideTown_Gaia_init()
	cprint( "!가이아 가이드 가동" )
	set_npc_name(  "@90400800"  )
end
 

function NPC_GuideTown_Gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90400801" )
	dlg_text( "@90400802" )

	dlg_menu( "@90010012", 'GuideTown_JobChange_Gaia()' )
	dlg_menu( "@90010013", 'GuideTown_Merchant_Gaia()' )
	dlg_menu( "@90010014", 'GuideTown_ItemUp_Gaia()' )
	dlg_menu( "@90010015", 'GuideTown_Teleport_Gaia()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

	-- 전직안내 펑션
function GuideTown_JobChange_Gaia()

	-- 다이얼로그 출력
	dlg_title( "@90400801" )
	dlg_text( "@90400803" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Gaia_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 상점안내 펑션
function GuideTown_Merchant_Gaia()

	-- 다이얼로그 출력
	dlg_title( "@90400801" )
	dlg_text( "@90400804" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Gaia_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 업글안내 펑션
function GuideTown_ItemUp_Gaia()

	-- 다이얼로그 출력
	dlg_title( "@90400801" )
	dlg_text( "@90400805" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Gaia_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 필드이동안내 펑션
function GuideTown_Teleport_Gaia()

	-- 다이얼로그 출력
	dlg_title( "@90400801" )
	dlg_text( "@90400806" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Gaia_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

   --============================================================
   --             <<<<<< 론도 측 NPC >>>>>>
   --============================================================

function NPC_GuideTown_Rondoh_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90600801" )
	dlg_text( "@90600802" )

	dlg_menu( "@90010012", 'GuideTown_JobChange_Rondoh()' )
	dlg_menu( "@90010013", 'GuideTown_Merchant_Rondoh()' )
	dlg_menu( "@90010014", 'GuideTown_ItemUp_Rondoh()' )
	dlg_menu( "@90010015", 'GuideTown_Teleport_Rondoh()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

	-- 전직안내 펑션
function GuideTown_JobChange_Rondoh()

	-- 다이얼로그 출력
	dlg_title( "@90600801" )
	dlg_text( "@90600803" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Rondoh_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 상점안내 펑션
function GuideTown_Merchant_Rondoh()

	-- 다이얼로그 출력
	dlg_title( "@90600801" )
	dlg_text( "@90600804" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Rondoh_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 업글안내 펑션
function GuideTown_ItemUp_Rondoh()

	-- 다이얼로그 출력
	dlg_title( "@90600801" )
	dlg_text( "@90600805" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Rondoh_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end

	-- 필드이동안내 펑션
function GuideTown_Teleport_Rondoh()

	-- 다이얼로그 출력
	dlg_title( "@90600801" )
	dlg_text( "@90600806" )

	dlg_menu( "@90010003", 'NPC_GuideTown_Rondoh_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end


   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================

function NPC_GuideTown_Secroute_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@90700801" )
	dlg_text( "@90700802" )

	-- 시크루트에서 받을 수 있는 혜택
	dlg_menu( "@90700807", 'GuideTown_Secroute_sub( 1 )' )
	-- 크루 샵 이용 방법
	dlg_menu( "@90700809", 'GuideTown_Secroute_sub( 2 )' )
	-- 크루 박스 이용 방법
	dlg_menu( "@90700811", 'GuideTown_Secroute_sub( 3 )' )
	-- 채널 이동 해드림 (시크루트)
	dlg_menu( "@90300513", 'Teleport_channel( 130100 )' )

	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end

	-- 전직안내 펑션
function GuideTown_Secroute_sub( select )

	-- 다이얼로그 출력
	dlg_title( "@90700801" )
	
	-- 시크루트에서 받을 수 있는 혜택
	if select == 1 then
		dlg_text( "@90700808" )
	-- 크루 샵 이용 방법
	elseif select == 2 then
		dlg_text( "@90700810" )
	-- 크루 박스 이용 방법
	elseif select == 3 then
		dlg_text( "@90700812" )
	end

	dlg_menu( "@90010003", 'NPC_GuideTown_Secroute_contact()' )
	dlg_menu( "@90010002", '' )
 
	dlg_show()

end
