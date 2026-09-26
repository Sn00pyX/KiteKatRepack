function world_boss_spawn_system()
	local current_time,world_boss,world_boss_spawn_time,x,y,text,world_boss_id,random_id
	current_time = get_os_time()-24
	world_boss = get_global_variable("world_boss_spawn")
	world_boss_spawn_time = get_global_variable("world_boss_death_time")-24+3600 -- 3600 = 1 hours
	text = {"@9777111","@9777112","@9777113","@9777114","@9777115"} -- // Localation notice 9777111 = x y 1  / 9777112 = x y 2 / 9777113 = x y 3 / 9777114 = x y 4 / 9777115 = x y 5
	x = {153400, 131014, 121202, 155607, 139386}
	y = {78460, 104179, 57603, 149681, 140383 }
	world_boss_id = 78000175
	random_id = math.random(5)
	
	if world_boss == 0 and current_time >= world_boss_spawn_time then
		add_npc(x[random_id],y[random_id],world_boss_id,1)
		notice(text[random_id])
		set_global_variable("world_boss_spawn","1")
		set_global_variable("world_boss_death","0")
		remove_event_state(164407)
	end
end

function world_boss_server_start_spawn_check() -- // server_init add
  local world_boss,world_boss_death,x,y,world_boss_id,text
  world_boss = get_global_variable("world_boss_spawn")
  world_boss_death = get_global_variable("world_boss_death")
  text = {"@9777111","@9777112","@9777113","@9777114","@9777115"} -- // Localation notice 9777111 = x y 1  / 9777112 = x y 2 / 9777113 = x y 3 / 9777114 = x y 4 / 9777115 = x y 5
  x = {153400, 131014, 121202, 155607, 139386}
  y = {78460, 104179, 57603, 149681, 140383 }
  world_boss_id = 78000175
  random_id = math.random(5)
  if world_boss == "" and world_boss_death == "" then
    add_npc(x[random_id],y[random_id],world_boss_id,1)
    notice(text[random_id])
  elseif world_boss == 1 and world_boss_death == 0 then
    add_npc(x[random_id],y[random_id],world_boss_id,1)
    notice(text[random_id])
  end

end

function world_boss_death_system()
  local current_time,world_boss,world_boss_death_time,name,death_notice
  current_time = get_os_time()-24
  world_boss = get_global_variable("world_boss_spawn")
  world_boss_death_time = get_global_variable("world_boss_death_time")
  name = tostring(get_value("name"))
--  state_id = {1,2,3,4,5} 
--  state_value = {1,2,3,4,5} -- // state_id 1 = state_value = 1 sample /run cast_world_state(1,1,3600,0)
--  random_number = math.random(5)
  death_notice = sconv("@9994731","#@name@#",name)
  
  if world_boss == "" and world_boss_death_time == "" then
    set_global_variable("world_boss_spawn","0")
    set_global_variable("world_boss_death_time",current_time)
    set_global_variable("world_boss_death","1")
  --  cast_world_state(state_id[random_number],state_value[random_number],3600,0)
    -- world_boss_state_system(164407,75,current_time)
    notice(death_notice)
  elseif world_boss == 1 then
    set_global_variable("world_boss_spawn","0")
    set_global_variable("world_boss_death_time",current_time)
    set_global_variable("world_boss_death","1")
    -- world_boss_state_system(164407,75,current_time)
    notice(death_notice)
	set_global_variable("world_boss_state","1")
  end
  
end

function world_boss_state_add()
  local world_boss,world_boss_death
  world_boss = get_global_variable("world_boss_spawn")
  world_boss_death = get_global_variable("world_boss_death")
  world_boss_state = get_global_variable("world_boss_state")
  if world_boss == 0 and world_boss_death == 1 and world_boss_state == 1 then
    add_event_state(164407,75)
    set_global_variable("world_boss_state","0")
  end
  -- body...
end


function world_boss_state_system(state_id,state_value,death_time)
  -- save()
  add_event_state(state_id,state_value)
  set_global_variable("world_boss_state_time",death_time)
  set_global_variable("world_boss_state_id",state_id)
  
end

function world_boss_state_check()
local state_id,state_time,current_time
  state_id = get_global_variable("world_boss_state_id")
  state_time = get_global_variable("world_boss_state_time")-24+3600
  current_time = get_os_time()-24
  if current_time >= state_time then
    remove_event_state(state_id)
  end
end