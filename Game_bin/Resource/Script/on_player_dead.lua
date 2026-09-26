-- Lua 스크립트 암호화
function get_module_name()
             return "on_player_dead"
end

function on_player_dead( name, lost_exp )

	local level
	
	level = get_value( "level" )
	
	-- 5레벨 이상이면 경험치 하락 처리
	if level > 5 then
		--##시작
	    message(sconv("@90019004", "#@lost_exp@#",tostring(lost_exp)))
        --##끝 전투불능으로 인해  #@lost_exp@# EXP를 잃었습니다.

	else
	--##시작		
	message("@90019005")	
		
	end
	
	save()


end



-- revive_type 값에 따른 부활 유형
-- 0: 그냥 필드에서 죽어서 일반 부활
-- 1: 대련 장에서 대련 중 사망에 대한 부활(기존 대련 시스템)
-- 2: 1:1 PVP 관련 신규 대련 시스템에 의한 대련 중 사망에 부활
-- 3: 던전 시즈 중 사망에 대한 부활
function revive_in_town( revive_type )


	-- 수련자 섬에 있는 오토들은 본토로 날려 버리자
	local current_x = gv("x")
	local current_y = gv("y")
	local is_training_camp = false
	local race = get_value( "race" )
 
	if current_x >= 161280 and current_x <= 177408 then
		if current_y >= 48384 and current_y <= 64512 then
			is_training_camp = true
		end -- if current_y >= 48384 and current_y <= 64512 then
	end -- if current_x >= 161280 and current_x <= 177408 then
		
	if is_training_camp then
		local is_auto, quest_count
		quest_count, is_auto = anti_auto_quest_check()

		if is_auto then
		
	 		if race == 4 then
				RunTeleport_Auto_TO_City( 6625 , 6980 )
			elseif race == 5 then
				RunTeleport_Auto_TO_City( 116799 , 58205 )
			else
				RunTeleport_Auto_TO_City( 153506 , 77175 )
			end
		else
			warp_to_revive_position()
		end -- if is_auto then
	else
	-- 수련자의 섬이 아니라면 설정된 지역으로 날려줌.
		warp_to_revive_position()
	end
	

	-- 오토로 세팅된 캐릭터라면 저 멀리 날려 버리자~
	kick_auto_to_another_world()
	
	
	-- 부활 할 때의 회복 HP를 정하는 조건 문 값에 대한 것은 상단에 주석으로 나와 있다.
	if revive_type == 0 then
		set_value( "hp" , get_value( "max_hp" ) )			-- 일반 필드에서 몬스터에게 죽어서 마을로 복귀해서 부활 100%회복
		
	elseif revive_type == 1 then
		set_value( "hp" , get_value( "max_hp" ) * 0.1 )		-- 대련장에서 대련 후 제자리 부활 선택 시 HP 10% 만 회복.
		
	elseif revive_type == 2 then
		set_value( "hp" , get_value( "max_hp" ) * 0.1 )		-- 1:1 PVP 대련 후 제자리 부활 선택 시 HP 10% 만 회복.
		
	elseif revive_type == 3 then
		set_value( "hp" , get_value( "max_hp" ) * 0.1 )		-- 던전 시즈 중 사망 후 부활 시 가장 가까운 소유의 전략 거점에서 HP 10%만 회복해서 부활한다.
		
		else
			set_value( "hp" , get_value( "max_hp" ) )		-- 일반 사망 후 부활 선택 시 HP MAX 로 회복.
				
	end
		
	return
	
end
