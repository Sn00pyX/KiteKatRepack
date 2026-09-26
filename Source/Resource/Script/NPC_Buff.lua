-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_Buff"
end

   --============================================================
   --===============		시크루트 마도사 카르트
   --============================================================

function NPC_Secroute_mage_init()
	cprint( "!마도사 카르트 가동" )
	set_npc_name(  "@90702500"  )
end



function NPC_Secroute_mage_contact()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	if is_premium() then
		dlg_text( "@90702502" )
	else
		dlg_text( "@90700118" )
	end

	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	if is_premium() then
	
	-- 일반 마법
	dlg_menu( "@90010197", "nomal_Buff()" )
	-- 중첩 마법
	dlg_menu( "@90010198", "reiterate_Buff()" )
	-- 모든 마법 취소
	dlg_menu( "@690000078", "Secroute_Buff_cancel()" )
	
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

function nomal_Buff()
	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	if is_premium() then
		dlg_text( "@90702505" )
	else
		dlg_text( "@90700118" )
	end
		
	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	if is_premium() then
	-- 일반 무료 마법
	dlg_menu( "@90010130", "nomal_no_charge_Buff()" )
	-- 일반 유료 마법
	dlg_menu( "@90010131", "nomal_charge_Buff()" )
	-- 일반 유료 마법 취소하기
	dlg_menu( "@90010212", "nomal_charge_Buff_cancel()" )
	
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end

function reiterate_Buff()
	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	if is_premium() then
		dlg_text( "@90702506" )
	else
		dlg_text( "@90700118" )
	end
		
	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	if is_premium() then
	-- 중첩 무료 마법
	dlg_menu( "@90010130", "reiterate_no_charge_Buff()" )
	-- 중첩 유료 마법
	dlg_menu( "@90010131", "reiterate_charge_Buff()" )			
	-- 중첩 유료 마법 취소
	dlg_menu( "@90010212", "reiterate_charge_Buff_cancel()" )
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
 
	dlg_show()

end



-- 일반 무료 버프
function nomal_no_charge_Buff()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	dlg_text( "@90702503" )

	-- 공속 증가 lv3
	dlg_menu( "@90010199", "nomal_no_charge_Buff_operation(1)" )
	-- 공격력 증가 lv3
	dlg_menu( "@90010200", "nomal_no_charge_Buff_operation(2)" )			
	-- 방어력 증가 lv3
	dlg_menu( "@90010201", "nomal_no_charge_Buff_operation(3)" )			
	-- 마력 증가 lv3
	dlg_menu( "@90010202", "nomal_no_charge_Buff_operation(4)" )			
	-- 마법방어력 증가 lv3
	dlg_menu( "@90010203", "nomal_no_charge_Buff_operation(5)" )			
	-- 시전속도 증가 lv3
	dlg_menu( "@90010204", "nomal_no_charge_Buff_operation(6)" )				
	--1~6번 6종 버프 한번에 받기	
	dlg_menu( "@90010249", "nomal_no_charge_Buff_operation(7)" )				
	
	--돌아가기
	dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	


-- 일반 유료 버프
function nomal_charge_Buff()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	dlg_text( "@90702503" )

	-- 공속 증가 lv6
	dlg_menu( "@90010205", "nomal_charge_Buff_operation(1)" )
	-- 공격력 증가 lv6
	dlg_menu( "@90010206", "nomal_charge_Buff_operation(2)" )			
	-- 방어력 증가 lv6
	dlg_menu( "@90010207", "nomal_charge_Buff_operation(3)" )			
	-- 마력 증가 lv6
	dlg_menu( "@90010208", "nomal_charge_Buff_operation(4)" )			
	-- 마법방어력 증가 lv6
	dlg_menu( "@90010209", "nomal_charge_Buff_operation(5)" )			
	-- 이동속도 증가 lv6
	dlg_menu( "@90010210", "nomal_charge_Buff_operation(6)" )			
	-- 시전속도 증가 lv6
	dlg_menu( "@90010211", "nomal_charge_Buff_operation(7)" )				
	--1~7번 7종 버프 한번에 받기	
	dlg_menu( "@90010250", "nomal_charge_Buff_operation(8)" )				
	
	
	--돌아가기
	dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	



-- 일반 유료 버프 취소하기
function nomal_charge_Buff_cancel()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	dlg_text( "@90702507" )

	-- 공속 증가 lv6 취소
	dlg_menu( "@90010213", "nomal_charge_Buff_cancel_operation(1)" )
	-- 공격력 증가 lv6 취소
	dlg_menu( "@90010214", "nomal_charge_Buff_cancel_operation(2)" )			
	-- 방어력 증가 lv6 취소
	dlg_menu( "@90010215", "nomal_charge_Buff_cancel_operation(3)" )			
	-- 마력 증가 lv6 취소
	dlg_menu( "@90010216", "nomal_charge_Buff_cancel_operation(4)" )			
	-- 마법방어력 증가 lv6 취소
	dlg_menu( "@90010217", "nomal_charge_Buff_cancel_operation(5)" )			
	-- 이동속도 증가 lv6 취소
	dlg_menu( "@90010218", "nomal_charge_Buff_cancel_operation(6)" )			
	-- 시전속도 증가 lv6 취소
	dlg_menu( "@90010219", "nomal_charge_Buff_cancel_operation(7)" )				
	
	--돌아가기
	dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	

-- 일반 무료 버프 걸기
function nomal_no_charge_Buff_operation( magic_set )
	-- 버프 7종 받아오기
	local state_level_1 = get_state_level( 1011 )
	local state_level_2 = get_state_level( 1007 )
	local state_level_3 = get_state_level( 1009 )
	local state_level_4 = get_state_level( 1008 )
	local state_level_5 = get_state_level( 1010 )
	local state_level_6 = get_state_level( 1012 )
	
	if magic_set == 1 and state_level_1 == 0 then
		add_state( 1055, 3, 540000 ) -- 공속lv3 증가
		nomal_no_charge_Buff()
		return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702508" )
	end		
		
	if magic_set == 2 and state_level_2 == 0 then
			add_state( 1051, 3, 540000 ) -- 공격력lv3 증가
			nomal_no_charge_Buff()
			return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702508" )
	end		
	
	if magic_set == 3 and state_level_3 == 0 then
			add_state( 1053, 3, 540000 ) -- 방어력lv3 증가
			nomal_no_charge_Buff()
			return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702508" )
	end		
	if magic_set == 4 and state_level_4 == 0 then
			add_state( 1052, 3, 540000 ) -- 마력lv3 증가
			nomal_no_charge_Buff()
			return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702508" )
	end		
	if magic_set == 5 and state_level_5 == 0 then
			add_state( 1054, 3, 540000 ) -- 마법방어력lv3 증가
			nomal_no_charge_Buff()
			return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702508" )
	end		
	if magic_set == 6 and state_level_6 == 0 then
			add_state( 1056, 3, 540000 ) -- 시전속도lv3 증가
			nomal_no_charge_Buff()
			return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702508" )
	end		
	if magic_set == 7 and state_level_1 == 0 and state_level_2 == 0 and state_level_3 == 0 and state_level_4 == 0 and state_level_5 == 0 and state_level_6 == 0 then
			add_state( 1051, 3, 540000 )
			add_state( 1052, 3, 540000 )
			add_state( 1053, 3, 540000 )
			add_state( 1054, 3, 540000 )
			add_state( 1055, 3, 540000 )
			add_state( 1056, 3, 540000 )
			nomal_no_charge_Buff()
			return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702508" )
	end	
	
	--돌아가기
	dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	




-- 일반 유료 버프 걸기
function nomal_charge_Buff_operation( magic_set )
	local gold = get_value( "gold" )
	-- 버프 7종 받아오기
	local state_level_1 = get_state_level( 1011 )
	local state_level_2 = get_state_level( 1007 )
	local state_level_3 = get_state_level( 1009 )
	local state_level_4 = get_state_level( 1008 )
	local state_level_5 = get_state_level( 1010 )
	local state_level_6 = get_state_level( 1013 )
	local state_level_7 = get_state_level( 1012 )
	
		if gold >= 1000 then
			if magic_set == 1 and state_level_1 == 0 then
				add_state( 1055, 7, 900000 ) -- 공속lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				nomal_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )	
				dlg_text( "@90702508" )
			end
				
			if magic_set == 2 and state_level_2 == 0 then
				add_state( 1051, 7, 900000 ) -- 공격력lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				nomal_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702508" )
			end
			
			if magic_set == 3 and state_level_3 == 0 then
				add_state( 1053, 7, 900000 ) -- 방어력lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				nomal_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702508" )
			end
			
			if magic_set == 4 and state_level_4 == 0 then
				add_state( 1052, 7, 900000 ) -- 마력lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				nomal_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702508" )
			end
			
			if magic_set == 5 and state_level_5 == 0 then
				add_state( 1054, 7, 900000 ) -- 마법방어력lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				nomal_charge_Buff()		
				return
			else	
				dlg_title( "@90702501" )	
				dlg_text( "@90702508" )
			end
			
			if magic_set == 6 and state_level_6 == 0 then
				add_state( 1057, 7, 900000 ) -- 이동속도lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				nomal_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702508" )
			end
			
			if magic_set == 7 and state_level_7 == 0 then
				add_state( 1056, 7, 900000 ) -- 시전속도lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				nomal_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702508" )
			end
			if gold >= 7000 then
				if magic_set == 8 and state_level_1 == 0 and state_level_2 == 0 and state_level_3 == 0 and state_level_4 == 0 and state_level_5 == 0 and state_level_6 == 0 and state_level_7 == 0  then
					add_state( 1051, 7, 900000 )
					add_state( 1052, 7, 900000 )
					add_state( 1053, 7, 900000 )
					add_state( 1054, 7, 900000 )
					add_state( 1055, 7, 900000 )
					add_state( 1056, 7, 900000 )
					add_state( 1057, 7, 900000 )
					set_value( "gold", gold - 7000 )
					update_gold_chaos()
					nomal_charge_Buff()		
				return				
				end	
			else
				dlg_title( "@90702501" )	
				dlg_text( "@90702508" )
			end
		else
		-- 다이얼로그 출력
		dlg_title( "@90702501" )
		dlg_text( "@90702504" )
		

		end
		--돌아가기
		dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )
		-- 대화종료 
		dlg_menu( "@90010002", " " )
		dlg_show()	
end

-- 일반 유료 버프 걸기 취소하기
function nomal_charge_Buff_cancel_operation( magic_set )

		if magic_set == 1 then
			remove_state( 1055, 7 ) -- 공속lv7 취소
		elseif magic_set == 2 then
			remove_state( 1051, 7 ) -- 공격력lv7 취소
		elseif magic_set == 3 then
			remove_state( 1053, 7 ) -- 방어력lv7 취소
		elseif magic_set == 4 then
			remove_state( 1052, 7 ) -- 마력lv7 취소
		elseif magic_set == 5 then
			remove_state( 1054, 7 ) -- 마법방어력lv7 취소
		elseif magic_set == 6 then
			remove_state( 1057, 7 ) -- 이동속도lv7 취소
		else
			remove_state( 1056, 7 ) -- 시전속도lv7 취소

		end
	nomal_charge_Buff_cancel()
end


-- 중첩 무료 마법 대사
function reiterate_no_charge_Buff()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	dlg_text( "@90702503" )

	-- 공속 증가 lv3
	dlg_menu( "@90010132", "reiterate_no_charge_Buff_operation(1)" )
	-- 공격력 증가 lv3
	dlg_menu( "@90010133", "reiterate_no_charge_Buff_operation(2)" )			
	-- 방어력 증가 lv3
	dlg_menu( "@90010134", "reiterate_no_charge_Buff_operation(3)" )			
	-- 마력 증가 lv3
	dlg_menu( "@90010135", "reiterate_no_charge_Buff_operation(4)" )			
	-- 마법방어력 증가 lv3
	dlg_menu( "@90010136", "reiterate_no_charge_Buff_operation(5)" )			
	-- 시전속도 증가 lv3
	dlg_menu( "@90010138", "reiterate_no_charge_Buff_operation(6)" )				
	--1~6번 6종 버프 한번에 받기	
	dlg_menu( "@90010249", "reiterate_no_charge_Buff_operation(7)" )			
	
		
	
	--돌아가기
	dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	


-- 중첩 유료 마법 대사
function reiterate_charge_Buff()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	dlg_text( "@90702503" )

	-- 공속 증가 lv6
	dlg_menu( "@90010139", "reiterate_charge_Buff_operation(1)" )
	-- 공격력 증가 lv6
	dlg_menu( "@90010140", "reiterate_charge_Buff_operation(2)" )			
	-- 방어력 증가 lv6
	dlg_menu( "@90010141", "reiterate_charge_Buff_operation(3)" )			
	-- 마력 증가 lv6
	dlg_menu( "@90010142", "reiterate_charge_Buff_operation(4)" )			
	-- 마법방어력 증가 lv6
	dlg_menu( "@90010143", "reiterate_charge_Buff_operation(5)" )			
	-- 이동속도 증가 lv6
	dlg_menu( "@90010144", "reiterate_charge_Buff_operation(6)" )			
	-- 시전속도 증가 lv6
	dlg_menu( "@90010145", "reiterate_charge_Buff_operation(7)" )				
	--1~7번 7종 버프 한번에 받기	
	dlg_menu( "@90010250", "reiterate_charge_Buff_operation(8)" )				
		
	--돌아가기
	dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	



-- 일반 유료 버프 취소하기
function reiterate_charge_Buff_cancel()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	dlg_text( "@90702507" )

	-- 공속 증가 lv6 취소
	dlg_menu( "@90010213", "reiterate_charge_Buff_cancel_operation(1)" )
	-- 공격력 증가 lv6 취소
	dlg_menu( "@90010214", "reiterate_charge_Buff_cancel_operation(2)" )			
	-- 방어력 증가 lv6 취소
	dlg_menu( "@90010215", "reiterate_charge_Buff_cancel_operation(3)" )			
	-- 마력 증가 lv6 취소
	dlg_menu( "@90010216", "reiterate_charge_Buff_cancel_operation(4)" )			
	-- 마법방어력 증가 lv6 취소
	dlg_menu( "@90010217", "reiterate_charge_Buff_cancel_operation(5)" )			
	-- 이동속도 증가 lv6 취소
	dlg_menu( "@90010218", "reiterate_charge_Buff_cancel_operation(6)" )			
	-- 시전속도 증가 lv6 취소
	dlg_menu( "@90010219", "reiterate_charge_Buff_cancel_operation(7)" )				
	
	
	--돌아가기
	dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	




-- 중첩 무료 마법 걸기	
function reiterate_no_charge_Buff_operation( magic_set )

	-- 버프 7종 받아오기
	local state_level_1 = get_state_level( 1055 )
	local state_level_2 = get_state_level( 1051 )
	local state_level_3 = get_state_level( 1053 )
	local state_level_4 = get_state_level( 1052 )
	local state_level_5 = get_state_level( 1054 )
	local state_level_6 = get_state_level( 1056 )

	
	if magic_set == 1 and state_level_1 == 0 then
		add_state( 1011, 3, 540000 ) -- 공속lv3 증가
		reiterate_no_charge_Buff()	
		return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702509" )
	end		
		
		
	if magic_set == 2 and state_level_2 == 0 then
			add_state( 1007, 3, 540000 ) -- 공격력lv3 증가
		reiterate_no_charge_Buff()	
		return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702509" )
	end		
	if magic_set == 3 and state_level_3 == 0 then
			add_state( 1009, 3, 540000 ) -- 방어력lv3 증가
		reiterate_no_charge_Buff()	
		return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702509" )
	end		
	if magic_set == 4 and state_level_4 == 0 then
			add_state( 1008, 3, 540000 ) -- 마력lv3 증가
		reiterate_no_charge_Buff()	
		return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702509" )
	end		
	if magic_set == 5 and state_level_5 == 0 then
			add_state( 1010, 3, 540000 ) -- 마법방어력lv3 증가
		reiterate_no_charge_Buff()	
		return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702509" )
	end		
	if magic_set == 6 and state_level_6 == 0 then
			add_state( 1012, 3, 540000 ) -- 시전속도lv3 증가
		reiterate_no_charge_Buff()	
		return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702509" )
	end
	
	if magic_set == 7 and state_level_1 == 0 and state_level_2 == 0 and state_level_3 == 0 and state_level_4 == 0 and state_level_5 == 0 and state_level_6 == 0 then
			add_state( 1007, 3, 540000 )
			add_state( 1008, 3, 540000 )
			add_state( 1009, 3, 540000 )
			add_state( 1010, 3, 540000 )
			add_state( 1011, 3, 540000 )
			add_state( 1012, 3, 540000 )
			nomal_no_charge_Buff()
			return
	else
		dlg_title( "@90702501" )	
		dlg_text( "@90702508" )
	end	
	
	--돌아가기
	dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )	
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
	
end




-- 중첩 유료 마법 걸기	
function reiterate_charge_Buff_operation( magic_set )
	local gold = get_value( "gold" )
	-- 버프 7종 받아오기
	local state_level_1 = get_state_level( 1055 )
	local state_level_2 = get_state_level( 1051 )
	local state_level_3 = get_state_level( 1053 )
	local state_level_4 = get_state_level( 1052 )
	local state_level_5 = get_state_level( 1054 )
	local state_level_6 = get_state_level( 1057 )
	local state_level_7 = get_state_level( 1056 )

	
		if gold >= 1000 then
			if magic_set == 1 and state_level_1 == 0 then
				add_state( 1011, 7, 900000 ) -- 공속lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				reiterate_charge_Buff()
				return
			else
				dlg_title( "@90702501" )	
				dlg_text( "@90702509" )
			end
				
			if magic_set == 2 and state_level_2 == 0 then
				add_state( 1007, 7, 900000 ) -- 공격력lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				reiterate_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
			
			if magic_set == 3 and state_level_3 == 0 then
				add_state( 1009, 7, 900000 ) -- 방어력lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				reiterate_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
			
			if magic_set == 4 and state_level_4 == 0 then
				add_state( 1008, 7, 900000 ) -- 마력lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				reiterate_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
			
			if magic_set == 5 and state_level_5 == 0 then
				add_state( 1010, 7, 900000 ) -- 마법방어력lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				reiterate_charge_Buff()		
				return
			else	
				dlg_title( "@90702501" )	
				dlg_text( "@90702509" )
			end
			
			if magic_set == 6 and state_level_6 == 0 then
				add_state( 1013, 7, 900000 ) -- 이동속도lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				reiterate_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
			
			if magic_set == 7 and state_level_7 == 0 then
				add_state( 1012, 7, 900000 ) -- 시전속도lv7 증가
				set_value( "gold", gold - 1000 )
				update_gold_chaos()
				reiterate_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
						
			if gold >= 7000 then
				if magic_set == 8 and state_level_1 == 0 and state_level_2 == 0 and state_level_3 == 0 and state_level_4 == 0 and state_level_5 == 0 and state_level_6 == 0 and state_level_7 == 0  then
					add_state( 1007, 7, 900000 )
					add_state( 1008, 7, 900000 )
					add_state( 1009, 7, 900000 )
					add_state( 1010, 7, 900000 )
					add_state( 1011, 7, 900000 )
					add_state( 1012, 7, 900000 )
					add_state( 1013, 7, 900000 )
					set_value( "gold", gold - 7000 )
					update_gold_chaos()
					nomal_charge_Buff()		
				return
				end	
			else
				dlg_title( "@90702501" )	
				dlg_text( "@90702508" )
			end
		else
		-- 다이얼로그 출력
		dlg_title( "@90702501" )
		dlg_text( "@90702504" )
		
		end
		--돌아가기
		dlg_menu( "@90010003", "NPC_Secroute_mage_contact()" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end


-- 일반 유료 버프 걸기 취소하기
function reiterate_charge_Buff_cancel_operation( magic_set )

		if magic_set == 1 then
			remove_state( 1011, 7 ) -- 공속lv7 취소
		elseif magic_set == 2 then
			remove_state( 1007, 7 ) -- 공격력lv7 취소
		elseif magic_set == 3 then
			remove_state( 1009, 7 ) -- 방어력lv7 취소
		elseif magic_set == 4 then
			remove_state( 1008, 7 ) -- 마력lv7 취소
		elseif magic_set == 5 then
			remove_state( 1010, 7 ) -- 마법방어력lv7 취소
		elseif magic_set == 6 then
			remove_state( 1013, 7 ) -- 이동속도lv7 취소
		else
			remove_state( 1012, 7 ) -- 시전속도lv7 취소

		end
	reiterate_charge_Buff_cancel()
end

-- 모든 시크루트 버프(일반/중첩) 한번에 취소하기
function Secroute_Buff_cancel()

		-- 시크루트 일반 버프 취소하기
		remove_state( 1011, 7 ) -- 공속lv7 취소
		remove_state( 1007, 7 ) -- 공격력lv7 취소
		remove_state( 1009, 7 ) -- 방어력lv7 취소
		remove_state( 1008, 7 ) -- 마력lv7 취소
		remove_state( 1010, 7 ) -- 마법방어력lv7 취소
		remove_state( 1013, 7 ) -- 이동속도lv7 취소
		remove_state( 1012, 7 ) -- 시전속도lv7 취소

		-- 시크루트 중첩 버프 취소하기
		remove_state( 1055, 7 ) -- 공속lv7 취소
		remove_state( 1051, 7 ) -- 공격력lv7 취소
		remove_state( 1053, 7 ) -- 방어력lv7 취소
		remove_state( 1052, 7 ) -- 마력lv7 취소
		remove_state( 1054, 7 ) -- 마법방어력lv7 취소
		remove_state( 1057, 7 ) -- 이동속도lv7 취소
		remove_state( 1056, 7 ) -- 시전속도lv7 취소

end

---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------
-------------------------------- 해외 시크루트 버프 ------------------------------------
---------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------

function NPC_Foreign_Secroute_mage_contact()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	if is_premium() then
		dlg_text( "@90999607" )	--시크루트가 있는 사람
	else
		dlg_text( "@90700118" )	--시크루트가 없는 사람
	end

	-- 시크루트 프리패스가 활성화된 상태(프리미엄 회원)
	if is_premium() then
	-- 중첩 마법
	dlg_menu( "@90010198", "Foreign_reiterate_charge_Buff()" )
	-- 중첩 유료 마법 취소
	dlg_menu( "@90010212", "Foreign_reiterate_charge_Buff_cancel()" )
	
	end
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end

-- 중첩 유료 마법 대사
function Foreign_reiterate_charge_Buff()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	dlg_text( "@90702503" )

	-- 공속 증가 lv6
	dlg_menu( "@90999609", "Foreign_reiterate_charge_Buff_operation(1)" )
	-- 공격력 증가 lv6
	dlg_menu( "@90999610", "Foreign_reiterate_charge_Buff_operation(2)" )			
	-- 방어력 증가 lv6
	dlg_menu( "@90999611", "Foreign_reiterate_charge_Buff_operation(3)" )			
	-- 마력 증가 lv6
	dlg_menu( "@90999612", "Foreign_reiterate_charge_Buff_operation(4)" )			
	-- 마법방어력 증가 lv6
	dlg_menu( "@90999613", "Foreign_reiterate_charge_Buff_operation(5)" )			
	-- 이동속도 증가 lv6
	dlg_menu( "@90999614", "Foreign_reiterate_charge_Buff_operation(6)" )			
	-- 시전속도 증가 lv6
	dlg_menu( "@90999615", "Foreign_reiterate_charge_Buff_operation(7)" )				
	--1~7번 7종 버프 한번에 받기	
	dlg_menu( "@90999616", "Foreign_reiterate_charge_Buff_operation(8)" )				
		
	--돌아가기
	dlg_menu( "@90010003", "NPC_Foreign_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	

-- 중첩 유료 마법 걸기	
function Foreign_reiterate_charge_Buff_operation( magic_set )
	local gold = get_value( "gold" )
	-- 버프 7종 받아오기
	local state_level_1 = get_state_level( 1055 )
	local state_level_2 = get_state_level( 1051 )
	local state_level_3 = get_state_level( 1053 )
	local state_level_4 = get_state_level( 1052 )
	local state_level_5 = get_state_level( 1054 )
	local state_level_6 = get_state_level( 1057 )
	local state_level_7 = get_state_level( 1056 )

	
		if gold >= 3000 then
			if magic_set == 1 and state_level_1 == 0 then
				add_state( 1011, 7, 360000 ) -- 공속 lv7 증가
				set_value( "gold", gold - 3000 )
				update_gold_chaos()
				Foreign_reiterate_charge_Buff()
				return
			else
				dlg_title( "@90702501" )	
				dlg_text( "@90702509" )
			end
				
			if magic_set == 2 and state_level_2 == 0 then
				add_state( 1007, 7, 360000 ) -- 공격력 lv7 증가
				set_value( "gold", gold - 3000 )
				update_gold_chaos()
				Foreign_reiterate_charge_Buff()		
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
			
			if magic_set == 3 and state_level_3 == 0 then
				add_state( 1009, 7, 360000 ) -- 방어력lv7 증가
				set_value( "gold", gold - 3000 )
				update_gold_chaos()
				Foreign_reiterate_charge_Buff()
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
			
			if magic_set == 4 and state_level_4 == 0 then
				add_state( 1008, 7, 360000 ) -- 마력lv7 증가
				set_value( "gold", gold - 3000 )
				update_gold_chaos()
				Foreign_reiterate_charge_Buff()
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
			
			if magic_set == 5 and state_level_5 == 0 then
				add_state( 1010, 7, 360000 ) -- 마법방어력lv7 증가
				set_value( "gold", gold - 3000 )
				update_gold_chaos()
				Foreign_reiterate_charge_Buff()
				return
			else	
				dlg_title( "@90702501" )	
				dlg_text( "@90702509" )
			end
			
			if magic_set == 6 and state_level_6 == 0 then
				add_state( 1013, 7, 360000 ) -- 이동속도lv7 증가
				set_value( "gold", gold - 3000 )
				update_gold_chaos()
				Foreign_reiterate_charge_Buff()
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
			
			if magic_set == 7 and state_level_7 == 0 then
				add_state( 1012, 7, 360000 ) -- 시전속도lv7 증가
				set_value( "gold", gold - 3000 )
				update_gold_chaos()
				Foreign_reiterate_charge_Buff()
				return
			else
				dlg_title( "@90702501" )		
				dlg_text( "@90702509" )
			end
						
			if gold >= 21000 then
				if magic_set == 8 and state_level_1 == 0 and state_level_2 == 0 and state_level_3 == 0 and state_level_4 == 0 and state_level_5 == 0 and state_level_6 == 0 and state_level_7 == 0  then
					add_state( 1007, 7, 360000 )
					add_state( 1008, 7, 360000 )
					add_state( 1009, 7, 360000 )
					add_state( 1010, 7, 360000 )
					add_state( 1011, 7, 360000 )
					add_state( 1012, 7, 360000 )
					add_state( 1013, 7, 360000 )
					set_value( "gold", gold - 21000 )
					update_gold_chaos()
					nomal_charge_Buff()		
				return
				end	
			else
				dlg_title( "@90702501" )	
				dlg_text( "@90702508" )
			end
		else
		-- 다이얼로그 출력
		dlg_title( "@90702501" )
		dlg_text( "@90702504" )
		
		end
		--돌아가기
		dlg_menu( "@90010003", "NPC_Foreign_Secroute_mage_contact()" )
	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end

-- 일반 유료 버프 취소하기
function Foreign_reiterate_charge_Buff_cancel()

	-- 다이얼로그 출력
	dlg_title( "@90702501" )
	dlg_text( "@90702507" )

	-- 공속 증가 lv6 취소
	dlg_menu( "@90010213", "Foreign_reiterate_charge_Buff_cancel_operation(1)" )
	-- 공격력 증가 lv6 취소
	dlg_menu( "@90010214", "Foreign_reiterate_charge_Buff_cancel_operation(2)" )			
	-- 방어력 증가 lv6 취소
	dlg_menu( "@90010215", "Foreign_reiterate_charge_Buff_cancel_operation(3)" )			
	-- 마력 증가 lv6 취소
	dlg_menu( "@90010216", "Foreign_reiterate_charge_Buff_cancel_operation(4)" )			
	-- 마법방어력 증가 lv6 취소
	dlg_menu( "@90010217", "Foreign_reiterate_charge_Buff_cancel_operation(5)" )			
	-- 이동속도 증가 lv6 취소
	dlg_menu( "@90010218", "Foreign_reiterate_charge_Buff_cancel_operation(6)" )			
	-- 시전속도 증가 lv6 취소
	dlg_menu( "@90010219", "Foreign_reiterate_charge_Buff_cancel_operation(7)" )				
	-- 모든 버프 취소
	dlg_menu( "@690000078", "Foreign_reiterate_charge_Buff_cancel_operation(8)" )	
	
	--돌아가기
	dlg_menu( "@90010003", "NPC_Foreign_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()

end	

-- 일반 유료 버프 걸기 취소하기
function Foreign_reiterate_charge_Buff_cancel_operation( magic_set )

		if magic_set == 1 then
			remove_state( 1011, 7 ) -- 공속lv7 취소
		elseif magic_set == 2 then
			remove_state( 1007, 7 ) -- 공격력lv7 취소
		elseif magic_set == 3 then
			remove_state( 1009, 7 ) -- 방어력lv7 취소
		elseif magic_set == 4 then
			remove_state( 1008, 7 ) -- 마력lv7 취소
		elseif magic_set == 5 then
			remove_state( 1010, 7 ) -- 마법방어력lv7 취소
		elseif magic_set == 6 then
			remove_state( 1013, 7 ) -- 이동속도lv7 취소
		elseif magic_set == 7 then
			remove_state( 1012, 7 ) -- 시전속도lv7 취소
		else
			remove_state( 1011, 7 ) -- 공속lv7 취소
			remove_state( 1007, 7 ) -- 공격력lv7 취소
			remove_state( 1009, 7 ) -- 방어력lv7 취소
			remove_state( 1008, 7 ) -- 마력lv7 취소
			remove_state( 1010, 7 ) -- 마법방어력lv7 취소
			remove_state( 1013, 7 ) -- 이동속도lv7 취소
			remove_state( 1012, 7 ) -- 시전속도lv7 취소
			
		end
	Foreign_reiterate_charge_Buff_cancel()
end

