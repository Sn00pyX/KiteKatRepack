-- Lua 스크립트 암호화
function get_module_name()
             return "ETC_run_monster_skill"
end

-- 트리거에서 호출되어 몬스터의 스킬을 사용하게 해주는 함수
-- 서버 함수 monster_skill_cast( 스킬_index, 몬스터핸들, 타겟 핸들 ) 를 사용하여 스킬을 사용하게 한다.

-- 트리거에서 호출되는 함수 인자들.
-- trigger( monster_handle, target_handle, trigger_index, x, y, layer, is_dungeon_raid_monster )

-- index 는 모두 제로베이스이므로 몬스터 스킬은 0, 1, 2, 3 으로, 트리거는 0, 1 로 사용.


function trigger( monster_handle, target_handle, idx, x, y, layer, sub_id )
	local monster_id = get_monster_id( monster_handle )
	-- 0 : TTTTTT
	-- 1 : MTTTTT
	-- 2 : MMTTTTT
	-- 3 : TMTTTT
	-- 4 : TTMMTT
	-- 5 : TTTMMT
	-- 6 : TTTMTT
	if (sub_id==0) then
		monster_skill_cast( idx, monster_handle, target_handle )
	else
		if	(sub_id==1 and idx==0) or (sub_id==2 and (idx==0 or idx==1)) or (sub_id==3 and idx==1) or (sub_id==4 and (idx==2 or idx==3)) or (sub_id==5 and (idx==3 or idx==4)) or (sub_id==6 and idx==3) then
			monster_skill_cast( idx, monster_handle, monster_handle )
		else
			monster_skill_cast( idx, monster_handle, target_handle )
		end
	end
end

function trigger_ag( monster_handle, target_handle, idx, x, y, layer, sub_id )
	
	local monster_id = get_monster_id( monster_handle )
	if idx == 0 then
		add_state(5997,  9, 500, target_handle)
	elseif idx == 1 then
		add_state(13005, 50, 12000, target_handle)
		add_state(5997, 11, 8640000, target_handle)
		set_auto_user( 1, target_handle )
	end	
end

function trigger_02M( monster_handle, target_handle, idx, x, y, layer, sub_id )

	local monster_id = get_monster_id( monster_handle )
	if idx == 0 then																																
		monster_skill_cast( 2, monster_handle, monster_handle )
	end
end

function trigger_02T13T( monster_handle, target_handle, idx, x, y, layer, sub_id )

	local monster_id = get_monster_id( monster_handle )
	if idx == 0 then
		monster_skill_cast( 2, monster_handle, target_handle )
	elseif idx == 1 then
		monster_skill_cast( 3, monster_handle, target_handle )
	end
end

function trigger_9060016( monster_handle, target_handle, idx, x, y, layer, sub_id )

	local monster_id = get_monster_id( monster_handle )
	if idx == 0 then
		monster_skill_cast( 0, monster_handle, monster_handle )
	elseif idx == 1 then
		monster_skill_cast( 2, monster_handle, target_handle )
	elseif idx == 2 then
		monster_skill_cast( 1, monster_handle, target_handle )
	elseif idx ==  3 then
		respawn_near_monster( monster_handle, 2055001, 6 )
	end
end

function trigger_9070012( monster_handle, target_handle, idx, x, y, layer, sub_id )

	local monster_id = get_monster_id( monster_handle )
	if idx == 0 then
		monster_skill_cast( 0, monster_handle, monster_handle )
	elseif idx == 1 then
		monster_skill_cast( 1, monster_handle, target_handle )
	elseif idx == 2 then
		monster_skill_cast( 2, monster_handle, target_handle )
	elseif idx == 3 then
		respawn_near_monster( monster_handle, 2065001, 6 )
	elseif idx == 4 then
		monster_skill_cast( 4, monster_handle, target_handle )
	elseif idx == 5 then
		monster_skill_cast( 5, monster_handle, target_handle )
	end
end


