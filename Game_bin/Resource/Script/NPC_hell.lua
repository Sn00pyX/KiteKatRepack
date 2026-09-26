-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_hell"
end

   --============================================================
   --             <<<<<< 무저갱 관리인 NPC >>>>>>
   --============================================================


function NPC_hell_contact()

	dlg_title("@90991001")
	
	-- 관련 안내
	if get_value("auto_user") == 1 then
		dlg_text("@90991002")
	else
		dlg_text("@90991003")
	end
	
	if get_value("auto_user") == 0 then

 		dlg_menu( "@90010127", 'RunTeleport_Auto_TO_City( 6625 , 6980 )' )
		dlg_menu( "@90010128", 'RunTeleport_Auto_TO_City( 116799 , 58205 )' )
		dlg_menu( "@90010129", 'RunTeleport_Auto_TO_City( 153506 , 77175 )' )
		dlg_menu( "@90010248", 'RunTeleport_Auto_TO_City( 172543 , 51847 )' )		
	end
	
	dlg_menu( "@90010002", '' )
 
	dlg_show()
 
end
