--- Lua 스크립트 암호화
function get_module_name()
             return "NPC_CreatureEnchanter"
end

   -- "이건 빠져 있는데 이것도 DB로 넣어야 한다" 라고 생각되시는
   -- 부분들에 대해서는 연락 주세욤.

math.randomseed( get_os_time() )

vigor_list = { 693001, 693003, 693002, 693004, 693005, 693006, 693007, 693008, 693009, 693011, 693010, 693012, 693013, 693015, 693014, 
						693016, 693017, 693018, 693019, 693020, 693021, 693022, 693023, 693024, 693025, 693026, 693027, 693028, 693029,
						693030, 693031, 693032, 693033,693034,} -- 기상 아이템 추가시 여기에 추가
	
artifact_list = {   { 560001, 560002, 560003, 560004, 560005, 560006, 560007, 560008, 560009, 560010, 560011 },
					{ 560012, 560013, 560014, 560015, 560016, 560017, 560018, 560019, 560020, 560021, 560022 },
					{ 560023, 560024, 560025, 560026, 560027, 560028, 560029, 560030, 560031, 560032, 560033 }, 
					{ 560034, 560035, 560036, 560037, 560038, 560039, 560040, 560041, 560042, 560043, 560044 }, 						
					{ 560045, 560046, 560047, 560048, 560049, 560050, 560051, 560052, 560053, 560054, 560055 }, 
					{ 560056, 560057, 560058, 560059, 560060, 560061, 560062, 560063, 560064, 560065, 560066 }, 
					{ 560067, 560068, 560069, 560070, 560071, 560072, 560073, 560074, 560075, 560076, 560077 }, 
					{ 560078, 560079, 560080, 560081, 560082, 560083, 560084, 560085, 560086, 560087, 560088 }, 
					{ 560089, 560090, 560091, 560092, 560093, 560094, 560095, 560096, 560097, 560098, 560099 }, 
					{ 560100, 560101, 560102, 560103, 560104, 560105, 560106, 560107, 560108, 560109, 560110 }, 
					{ 560111, 560112, 560113, 560114, 560115, 560116, 560117, 560118, 560119, 560120, 560121 },
					{ 560122, 560123, 560124, 560125, 560126, 560127, 560128, 560129, 560130, 560131, 560132 },
					{ 560133, 560134, 560135, 560136, 560137, 560138, 560139, 560140, 560141, 560142, 560143 }, 
					{ 560144, 560145, 560146, 560147, 560148, 560149, 560150, 560151, 560152, 560153, 560154 }, 						
					{ 560155, 560156, 560157, 560158, 560159, 560160, 560161, 560162, 560163, 560164, 560165 }, 
					{ 560166, 560167, 560168, 560169, 560170, 560171, 560172, 560173, 560174, 560175, 560176 }, 
					{ 560177, 560178, 560179, 560180, 560181, 560182, 560183, 560184, 560185, 560186, 560187 }, 
					{ 560188, 560189, 560190, 560191, 560192, 560193, 560194, 560195, 560196, 560197, 560198 }, 
					{ 560199, 560200, 560201, 560202, 560203, 560204, 560205, 560206, 560207, 560208, 560209 }, 
					{ 560210, 560211, 560212, 560213, 560214, 560215, 560216, 560217, 560218, 560219, 560220 }, 
					{ 560221, 560222, 560223, 560224, 560225, 560226, 560227, 560228, 560229, 560230, 560231 },
					{ 560232, 560233, 560234, 560235, 560236, 560237, 560238, 560239, 560240, 560241, 560242 },
					{ 560243, 560244, 560245, 560246, 560247, 560248, 560249, 560250, 560251, 560252, 560253 }, 
					{ 560254, 560255, 560256, 560257, 560258, 560259, 560260, 560261, 560262, 560263, 560264 }, 						
					{ 560265, 560266, 560267, 560268, 560269, 560270, 560271, 560272, 560273, 560274, 560275 }, 
					{ 560276, 560277, 560278, 560279, 560280, 560281, 560282, 560283, 560284, 560285, 560286 },
					{ 560287, 560288, 560289, 560290, 560291, 560292, 560293, 560294, 560295, 560296, 560297 },
					{ 560298, 560299, 560300, 560301, 560302, 560303, 560304, 560305, 560306, 560307, 560308 },
					{ 560309, 560310, 560311, 560312, 560313, 560314, 560315, 560316, 560317, 560318, 560319 },
					{ 560320, 560321, 560322, 560323, 560324, 560325, 560326, 560327, 560328, 560329, 560330 },
					{ 560331, 560332, 560333, 560334, 560335, 560336, 560337, 560338, 560339, 560340, 560341 },
					{ 560342, 560343, 560344, 560345, 560346, 560347, 560348, 560349, 560350, 560351, 560352 },  
					{ 560353, 560354, 560355, 560356, 560357, 560358, 560359, 560360, 560361, 560362, 560363 },  
					{ 560364, 560365, 560366, 560367, 560368, 560369, 560370, 560371, 560372, 560373, 560374 }}  -- 아티펙트 추가시 여기에 추가
   
   --============================================================
   --             <<<<<< 데바 측 NPC >>>>>>
   --============================================================
function NPC_Creature_Enchanter_Deva_init()
	cprint( "!크리쳐 강화학자 라미 가동" )
	set_npc_name(  "@91000300"  )
end
 

function NPC_Creature_Enchanter_Deva_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@91000304" ) --크리쳐 강화학자 라미
	dlg_text( "@91000308" ) -- npc 대사
	dlg_menu( "@91000312", "NPC_Creature_Enchanter_Deva_enchant_explain()" ) -- 크리쳐 카드 강화 방법
	dlg_menu( "@91000313", "NPC_Creature_Enchanter_Deva_enchant_merits()" ) -- 강화 크리쳐의 좋은 점
	dlg_menu( "@91000314", "NPC_Creature_Enchanter_Deva_enchant_demerits()" ) -- 강화에 실패할 경우
	dlg_menu( "@91000315", "NPC_Creature_Enchanter_Deva_enchant_durablity()" ) -- 강화 크리쳐 카드의 내구도
	dlg_menu( "@91000316", "NPC_Creature_Enchanter_Deva_enchant_artifact()" ) -- 아티펙트란?
	dlg_menu( "@91000317", "open_market( 'creature_enchant' )" )
	dlg_menu( "@91000318", "Deva_show(1)" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function NPC_Creature_Enchanter_Deva_enchant_explain()
	dlg_title( "@91000304" ) --크리쳐 강화학자 라미
	dlg_text( "@91000319" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Deva_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_Deva_enchant_merits()
	dlg_title( "@91000304" ) --크리쳐 강화학자 라미
	dlg_text( "@91000320" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Deva_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_Deva_enchant_demerits()
	dlg_title( "@91000304" ) --크리쳐 강화학자 라미
	dlg_text( "@91000321" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Deva_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_Deva_enchant_durablity()
	dlg_title( "@91000304" ) --크리쳐 강화학자 라미
	dlg_text( "@91000322" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Deva_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_Deva_enchant_artifact()
	dlg_title( "@91000304" ) --크리쳐 강화학자 라미
	dlg_text( "@91000323" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Deva_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function Deva_show( start )
	-- 다이얼로그 출력
	dlg_title( "@91000304" ) --크리쳐 강화학자 라미	
	dlg_text( "@91000343" )
	
	local vigor_list_count = table.getn( vigor_list )
	local handle_list = {}
	local handle_list_count = 0
	local handle = 0
	local idx = 0

	for i=start, vigor_list_count do
		handle = get_item_handle( vigor_list[i] )

		if handle ~= 0 then
			handle_list_count = handle_list_count + 1
			handle_list[ handle_list_count ] = handle
		end

		if handle_list_count == 10 then
			idx = i
			break
		end
	end

	for i=1, handle_list_count do
		name = "@" .. tostring( get_item_name_id( get_item_code( handle_list[i] ) ) )		
		command = "Deva_artifact_exchange(" .. handle_list[i] .. ")"
		dlg_menu( name, command )
	end

	if handle_list_count == 10 then
		name = "@90010251"
		command = "Deva_show(" .. idx+1 .. ")"
		dlg_menu( name, command )
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
end

function Deva_artifact_exchange(handle)
	dlg_text( "@91000334" ) -- 해당 기상 아이템을 아티펙트로 교환 하시겠습니까?
	dlg_menu( "@90010195", "Deva_artifact_insert(" .. handle .. ")" ) -- 예
	dlg_menu( "@90010196", "NPC_Creature_Enchanter_Deva_contact()" ) -- 아니오
	dlg_show()
end

function Deva_artifact_insert(handle)
	-- 다이얼로그 출력
	dlg_title( "@91000304" ) --크리쳐 강화학자 라미
	dlg_text( "@91000308" ) -- npc 대사

	local soulcatcher_count = find_item(710002)
	local vigor_list_count = table.getn( vigor_list )

	if soulcatcher_count < 1 then
		dlg_text( "@91000337" ) -- 소울 캐쳐가 없습니다.

	else
		for i=1, vigor_list_count do
			if get_item_code(handle) == vigor_list[i] then

				local artifact = artifact_list[i]
				local artifact_count = table.getn( artifact )
				local select_one = math.random(1, artifact_count)
				
				delete_item( handle, 1 )
				delete_item( get_item_handle( 710002 ), 1 )
				insert_item( artifact[select_one], 1 )	
			end
		
		end
		dlg_text( "@91000340" )	
	end

	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Deva_contact()" )
	dlg_menu( "@90010002", '' )
 	dlg_show()
end

   --============================================================
   --             <<<<<< 가이아 측 NPC >>>>>>
   --============================================================
function NPC_Creature_Enchanter_gaia_init()
	cprint( "!크리쳐 강화학자 섭 가동" )
	set_npc_name(  "@91000301"  )
end
 

function NPC_Creature_Enchanter_gaia_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@91000305" ) --크리쳐 강화학자 섭
	dlg_text( "@91000309" ) -- npc 대사
	dlg_menu( "@91000312", "NPC_Creature_Enchanter_gaia_enchant_explain()" ) -- 크리쳐 카드 강화 방법
	dlg_menu( "@91000313", "NPC_Creature_Enchanter_gaia_enchant_merits()" ) -- 강화 크리쳐의 좋은 점
	dlg_menu( "@91000314", "NPC_Creature_Enchanter_gaia_enchant_demerits()" ) -- 강화에 실패할 경우
	dlg_menu( "@91000315", "NPC_Creature_Enchanter_gaia_enchant_durablity()" ) -- 강화 크리쳐 카드의 내구도
	dlg_menu( "@91000316", "NPC_Creature_Enchanter_gaia_enchant_artifact()" ) -- 아티펙트란?
	dlg_menu( "@91000317", "open_market( 'creature_enchant' )" )
	dlg_menu( "@91000318", "Gaia_show(1)" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function NPC_Creature_Enchanter_gaia_enchant_explain()
	dlg_title( "@91000305" ) --크리쳐 강화학자 섭
	dlg_text( "@91000324" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_gaia_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_gaia_enchant_merits()
	dlg_title( "@91000305" ) --크리쳐 강화학자 섭
	dlg_text( "@91000325" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_gaia_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_gaia_enchant_demerits()
	dlg_title( "@91000305" ) --크리쳐 강화학자 섭
	dlg_text( "@91000326" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_gaia_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_gaia_enchant_durablity()
	dlg_title( "@91000305" ) --크리쳐 강화학자 섭
	dlg_text( "@91000327" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_gaia_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_gaia_enchant_artifact()
	dlg_title( "@91000305" ) --크리쳐 강화학자 섭
	dlg_text( "@91000328" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_gaia_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function Gaia_show( start )
	-- 다이얼로그 출력
	dlg_title( "@91000305" ) --크리쳐 강화학자 섭
	dlg_text( "@91000344" ) -- npc 대사
	
	local vigor_list_count = table.getn( vigor_list )
	local handle_list = {}
	local handle_list_count = 0
	local handle = 0
	local idx = 0

	for i=start, vigor_list_count do
		handle = get_item_handle( vigor_list[i] )

		if handle ~= 0 then
			handle_list_count = handle_list_count + 1
			handle_list[ handle_list_count ] = handle
		end

		if handle_list_count == 10 then
			idx = i
			break
		end
	end

	for i=1, handle_list_count do
		name = "@" .. tostring( get_item_name_id( get_item_code( handle_list[i] ) ) )		
		command = "Gaia_artifact_exchange(" .. handle_list[i] .. ")"
		dlg_menu( name, command )
	end

	if handle_list_count == 10 then
		name = "@90010251"
		command = "Gaia_show(" .. idx+1 .. ")"
		dlg_menu( name, command )
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
end

function Gaia_artifact_exchange(handle)
	dlg_text( "@91000335" ) -- 해당 기상 아이템을 아티펙트로 교환 하시겠습니까?
	dlg_menu( "@90010195", "Gaia_artifact_insert(" .. handle .. ")" ) -- 예
	dlg_menu( "@90010196", "NPC_Creature_Enchanter_gaia_contact()" ) -- 아니오
	dlg_show()
end

function Gaia_artifact_insert(handle)
	-- 다이얼로그 출력
	dlg_title( "@91000305" ) --크리쳐 강화학자 섭
	dlg_text( "@91000309" ) -- npc 대사

	local soulcatcher_count = find_item(710002)
	local vigor_list_count = table.getn( vigor_list )

	if soulcatcher_count < 1 then
		dlg_text( "@91000338" ) -- 소울 캐쳐가 없습니다.

	else
		for i=1, vigor_list_count do
			if get_item_code(handle) == vigor_list[i] then

				local artifact = artifact_list[i]
				local artifact_count = table.getn( artifact )
				local select_one = math.random(1, artifact_count)
				
				delete_item( handle, 1 )
				delete_item( get_item_handle( 710002 ), 1 )
				insert_item( artifact[select_one], 1 )	
			end
		
		end
		dlg_text( "@91000341" )	
	end

	dlg_menu( "@90010003", "NPC_Creature_Enchanter_gaia_contact()" )
	dlg_menu( "@90010002", '' )
 	dlg_show()
end

   --============================================================
   --             <<<<<< 아수라 측 NPC >>>>>>
   --============================================================
function NPC_Creature_Enchanter_asura_init()
	cprint( "!크리쳐 강화학자 칼리크 가동" )
	set_npc_name(  "@91000302"  )
end
 

function NPC_Creature_Enchanter_asura_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@91000306" ) --크리쳐 강화학자 칼리크
	dlg_text( "@91000310" ) -- npc 대사
	dlg_menu( "@91000312", "NPC_Creature_Enchanter_asura_enchant_explain()" ) -- 크리쳐 카드 강화 방법
	dlg_menu( "@91000313", "NPC_Creature_Enchanter_asura_enchant_merits()" ) -- 강화 크리쳐의 좋은 점
	dlg_menu( "@91000314", "NPC_Creature_Enchanter_asura_enchant_demerits()" ) -- 강화에 실패할 경우
	dlg_menu( "@91000315", "NPC_Creature_Enchanter_asura_enchant_durablity()" ) -- 강화 크리쳐 카드의 내구도
	dlg_menu( "@91000316", "NPC_Creature_Enchanter_asura_enchant_artifact()" ) -- 아티펙트란?
	dlg_menu( "@91000317", "open_market( 'creature_enchant' )" )
	dlg_menu( "@91000318", "Asura_show(1)" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function NPC_Creature_Enchanter_asura_enchant_explain()
	dlg_title( "@91000306" ) --크리쳐 강화학자 칼리크
	dlg_text( "@91000329" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_asura_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_asura_enchant_merits()
	dlg_title( "@91000306" ) --크리쳐 강화학자 칼리크
	dlg_text( "@91000330" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_asura_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_asura_enchant_demerits()
	dlg_title( "@91000306" ) --크리쳐 강화학자 칼리크
	dlg_text( "@91000331" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_asura_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_asura_enchant_durablity()
	dlg_title( "@91000306" ) --크리쳐 강화학자 칼리크
	dlg_text( "@91000332" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_asura_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_asura_enchant_artifact()
	dlg_title( "@91000306" ) --크리쳐 강화학자 칼리크
	dlg_text( "@91000333" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_asura_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function Asura_show( start )
	-- 다이얼로그 출력
	dlg_title( "@91000306" ) --크리쳐 강화학자 칼리크
	dlg_text( "@91000345" ) -- npc 대사
	
	local vigor_list_count = table.getn( vigor_list )
	local handle_list = {}
	local handle_list_count = 0
	local handle = 0
	local idx = 0

	for i=start, vigor_list_count do
		handle = get_item_handle( vigor_list[i] )

		if handle ~= 0 then
			handle_list_count = handle_list_count + 1
			handle_list[ handle_list_count ] = handle
		end

		if handle_list_count == 10 then
			idx = i
			break
		end
	end

	for i=1, handle_list_count do
		name = "@" .. tostring( get_item_name_id( get_item_code( handle_list[i] ) ) )		
		command = "Asura_artifact_exchange(" .. handle_list[i] .. ")"
		dlg_menu( name, command )
	end

	if handle_list_count == 10 then
		name = "@90010251"
		command = "Asura_show(" .. idx+1 .. ")"
		dlg_menu( name, command )
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
end

function Asura_artifact_exchange(handle)
	dlg_text( "@91000336" ) -- 해당 기상 아이템을 아티펙트로 교환 하시겠습니까?
	dlg_menu( "@90010195", "Asura_artifact_insert(" .. handle .. ")" ) -- 예
	dlg_menu( "@90010196", "NPC_Creature_Enchanter_asura_contact()" ) -- 아니오
	dlg_show()
end

function Asura_artifact_insert(handle)
	-- 다이얼로그 출력
	dlg_title( "@91000306" ) --크리쳐 강화학자 칼리크
	dlg_text( "@91000310" ) -- npc 대사

	local soulcatcher_count = find_item(710002)
	local vigor_list_count = table.getn( vigor_list )

	if soulcatcher_count < 1 then
		dlg_text( "@91000339" ) -- 소울 캐쳐가 없습니다.

	else
		for i=1, vigor_list_count do
			if get_item_code(handle) == vigor_list[i] then

				local artifact = artifact_list[i]
				local artifact_count = table.getn( artifact )
				local select_one = math.random(1, artifact_count)
				
				delete_item( handle, 1 )
				delete_item( get_item_handle( 710002 ), 1 )
				insert_item( artifact[select_one], 1 )	
			end
		
		end
		dlg_text( "@91000342" )	
	end

	dlg_menu( "@90010003", "NPC_Creature_Enchanter_asura_contact()" )
	dlg_menu( "@90010002", '' )
 	dlg_show()
end

   --============================================================
   --             <<<<<< 시크루트 측 NPC >>>>>>
   --============================================================
function NPC_Creature_Enchanter_Secroute_init()
	cprint( "!크리쳐 강화학자 브라우니 가동" )
	set_npc_name(  "@91000303"  )
end
 

function NPC_Creature_Enchanter_Secroute_contact()
 
	-- 다이얼로그 출력
	dlg_title( "@91000307" ) --크리쳐 강화학자 브라우니
	if is_premium() then
		dlg_text( "@91000311" )
	else
		dlg_text( "@90700118" )
	end

		if is_premium() then
			dlg_menu( "@91000312", "NPC_Creature_Enchanter_Secroute_enchant_explain()" ) -- 크리쳐 카드 강화 방법
			dlg_menu( "@91000313", "NPC_Creature_Enchanter_Secroute_enchant_merits()" ) -- 강화 크리쳐의 좋은 점
			dlg_menu( "@91000314", "NPC_Creature_Enchanter_Secroute_enchant_demerits()" ) -- 강화에 실패할 경우
			dlg_menu( "@91000315", "NPC_Creature_Enchanter_Secroute_enchant_durablity()" ) -- 강화 크리쳐 카드의 내구도
			dlg_menu( "@91000316", "NPC_Creature_Enchanter_Secroute_enchant_artifact()" ) -- 아티펙트란?
			dlg_menu( "@91000317", "open_market( 'Secroute_creature_enchant' )" )
			dlg_menu( "@91000318", "Secroute_show(1)" )
			dlg_menu( "@90010002", " " )
		else
			dlg_text( "@90700118" )
			dlg_menu( "@90010002", " " )
	end	
			
			dlg_show()
end

function NPC_Creature_Enchanter_Secroute_enchant_explain()
	dlg_title( "@91000307" ) --크리쳐 강화학자 브라우니
	dlg_text( "@91000319" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Secroute_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_Secroute_enchant_merits()
	dlg_title( "@91000307" ) --크리쳐 강화학자 브라우니
	dlg_text( "@91000320" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Secroute_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_Secroute_enchant_demerits()
	dlg_title( "@91000307" ) --크리쳐 강화학자 브라우니
	dlg_text( "@91000321" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Secroute_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_Secroute_enchant_durablity()
	dlg_title( "@91000307" ) --크리쳐 강화학자 브라우니
	dlg_text( "@91000322" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Secroute_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function NPC_Creature_Enchanter_Secroute_enchant_artifact()
	dlg_title( "@91000307" ) --크리쳐 강화학자 브라우니
	dlg_text( "@91000323" ) -- npc 대사
	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Secroute_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function Secroute_show( start )
	-- 다이얼로그 출력
	dlg_title( "@91000307" ) --크리쳐 강화학자 브라우니
	dlg_text( "@91000343" ) -- npc 대사
	
	local vigor_list_count = table.getn( vigor_list )
	local handle_list = {}
	local handle_list_count = 0
	local handle = 0
	local idx = 0

	for i=start, vigor_list_count do
		handle = get_item_handle( vigor_list[i] )

		if handle ~= 0 then
			handle_list_count = handle_list_count + 1
			handle_list[ handle_list_count ] = handle
		end

		if handle_list_count == 10 then
			idx = i
			break
		end
	end

	for i=1, handle_list_count do
		name = "@" .. tostring( get_item_name_id( get_item_code( handle_list[i] ) ) )		
		command = "Secroute_artifact_exchange(" .. handle_list[i] .. ")"
		dlg_menu( name, command )
	end

	if handle_list_count == 10 then
		name = "@90010251"
		command = "Secroute_show(" .. idx+1 .. ")"
		dlg_menu( name, command )
	end
	
	dlg_menu( "@90010002", '' )
	dlg_show()
end

function Secroute_artifact_exchange(handle)
	dlg_text( "@91000334" ) -- 해당 기상 아이템을 아티펙트로 교환 하시겠습니까?
	dlg_menu( "@90010195", "Secroute_artifact_insert(" .. handle .. ")" ) -- 예
	dlg_menu( "@90010196", "NPC_Creature_Enchanter_Secroute_contact()" ) -- 아니오
	dlg_show()
end

function Secroute_artifact_insert(handle)
	-- 다이얼로그 출력
	dlg_title( "@91000307" ) --크리쳐 강화학자 브라우니
	dlg_text( "@91000308" ) -- npc 대사

	local soulcatcher_count = find_item(710002)
	local vigor_list_count = table.getn( vigor_list )

	if soulcatcher_count < 1 then
		dlg_text( "@91000337" ) -- 소울 캐쳐가 없습니다.

	else
		for i=1, vigor_list_count do
			if get_item_code(handle) == vigor_list[i] then

				local artifact = artifact_list[i]
				local artifact_count = table.getn( artifact )
				local select_one = math.random(1, artifact_count)
				
				delete_item( handle, 1 )
				delete_item( get_item_handle( 710002 ), 1 )
				insert_item( artifact[select_one], 1 )	
			end
		
		end
		dlg_text( "@91000340" )	
	end

	dlg_menu( "@90010003", "NPC_Creature_Enchanter_Secroute_contact()" )
	dlg_menu( "@90010002", '' )
 	dlg_show()
end
