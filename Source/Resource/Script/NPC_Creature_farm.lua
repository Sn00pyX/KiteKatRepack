---- Lua 스크립트 암호화
function get_module_name()
             return "NPC_Creature_farm"
end

   -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.

math.randomseed( get_os_time() )

   --============================================================
   --             <<<<<< 크리쳐 농장 NPC >>>>>>
   --============================================================
function NPC_Creature_Farm_init()
	cprint( "!크리쳐 농장 NPC 가동" )
	set_npc_name(  "@91000349"  )
end
 
ticket_table = 	{
					{{1,1,1,2,2,3},
					{3,3,3,6,6,9}},
								
					{{2,2,2,3,3,4},
					{6,6,6,9,9,12}},
							
					{{2,2,2,3,3,4},
					{6,6,6,9,9,12}},
							
					{{3,3,3,4,4,5},
					{9,9,9,12,12,15}},
					
					{{3,3,3,4,4,5},
					{9,9,9,12,12,15}},	
							
					{{4,4,4,5,5,6},
					{12,12,12,15,15,18}}
				}
				
--ticket_code_table = {}
--ticket_code_table[ "259200" ] = {}
--ticket_code_table[ "259200" ][ "0" ] = { 710005 } -- , 추가된 아이디 ... }
--ticket_code_table[ "259200" ][ "1" ] = { 710007 } -- , 추가된 아이디 ... }
--ticket_code_table[ "604800" ] = {}
--ticket_code_table[ "604800" ][ "0" ] = { 710006 } -- , 추가된 아이디 ... }
--ticket_code_table[ "604800" ][ "1" ] = { 710008 } -- , 추가된 아이디 ... }

--cracker_code_table = { 2902163, 2902162, 2902167, 2902166, 2902161 }

slot_usable = {}
slot_usable = { 0, 0, 0 }

function NPC_Creature_Farm_nurse_handler()
           local rate = math.random(1, 10)
           if rate < 6 then
                      insert_item( 710009, 1 )
                     return 1
           end
           return 0
end

function NPC_Creature_Farm_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@91000350" ) --크리쳐 농장 주인 소냐
	dlg_text( "@91000351" ) -- npc 대사
	dlg_menu( "@91000352", "NPC_Creature_Farm_explain()" ) -- 크리쳐 농장 소개
	dlg_menu( "@91000353", "show_creature_farm_window()" ) -- 크리쳐 관리

	dlg_menu( "@90010002", " " )
	dlg_show()
end

function NPC_Creature_Farm_explain()
	dlg_title( "@91000350" ) --크리쳐 농장 주인 소냐
	dlg_text( "@91000358" ) -- 어떤 게 궁금해?
	dlg_menu( "@91000355", "NPC_Creature_Farm_Whatis()" ) -- 크리쳐 농장이란
	dlg_menu( "@91000356", "NPC_Creature_Farm_Creature_Growup()" ) -- 크리쳐 성장
	dlg_menu( "@91000357", "NPC_Creature_Farm_Whatis_Creature_Care()" ) -- 돌보기
	dlg_menu( "@90010003", "NPC_Creature_Farm_contact()" )	
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Farm_Whatis() -- 크리쳐 농장이란
	dlg_title( "@91000350" ) 
	dlg_text( "@91000359" )
	dlg_menu( "@90010003", "NPC_Creature_Farm_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Farm_Creature_Growup() -- 크리쳐 성장
	dlg_title( "@91000350" )
	dlg_text( "@91000360" ) 
	dlg_menu( "@90010003", "NPC_Creature_Farm_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Farm_Whatis_Creature_Care() -- 돌보기
	dlg_title( "@91000350" ) 
	dlg_text( "@91000361" ) 
	dlg_menu( "@90010003", "NPC_Creature_Farm_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


