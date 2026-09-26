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
		-- dlg_text("@90991002") 기존 NPC 대사, 임시 NPC 세운 뒤 변경
		dlg_text("@90991008") -- 임시 NPC를 소개하는 대사
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


------------- function NPC_hell_remove_state()		오토 유저에 대한 천벌 디버프가 풀리지 않는 버그가 수정됨으로써 임시 NPC는 삭제

-- dlg_title( "@90991004" ) -- NPC 자비로운 관리인
-- dlg_text( "@90991005" )
	
-- dlg_menu( "@90991007", 'NPC_hell_remove_state_now()' ) -- 천벌효과 풀기 메뉴
	
-- dlg_menu( "@90010002", '' )
 
-- dlg_show()
 
-- end

------------- function NPC_hell_remove_state_now() -- 천벌 효과 풀기 메뉴 선택 시

-- dlg_title( "@90991004" ) -- NPC 자비로운 관리인
-- dlg_text( "@90991006" ) -- 이제 천벌은 풀렸으니 옆에 있는 관리인들을 통해 나가세요

-- if get_value( "auto_user" ) == 1 then
	
-- set_auto_user( 0 )
		
--remove_state( 5997, 0 ) -- 천벌 없애기
	
-- end
	
-- dlg_menu( "@90010002", '' )
	
-- dlg_show()

-- end


