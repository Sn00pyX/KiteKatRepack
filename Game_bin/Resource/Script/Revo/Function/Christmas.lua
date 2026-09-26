--========================================
------------------------------------------
---------2019-NewYear-Christmas-----------
-----------12/16/2018---------------------
------------------------------------------
--========================================

--[[
function NPC_Xmas_Event_Contact()
	dlg_title("@99910001")
	dlg_text("@99910002")
	-- dlg_menu("@99910003","")
	dlg_menu("@90010002", "" )
	dlg_show()
end 

function on_use_Xmasprop()
	local progress = get_quest_progress( 21001 )
	local num = get_quest_status( 21001 , 1 )
	local pnum = num + 1
	if progress == 1 then
		if num < 5 then
			private_notice("@99910009")
			set_quest_status( 21001, 1, pnum )
		end
	end
end 

function Xmas_MonsterDead(type)
	local name = tostring(gv("name"))
	local text = sconv( "@99910005", "#@name@#", name )
	local prize_id
	local randomly
	local in_randomly
	local prize_cnt
	local in_random
	prize_id = {800000001,800000002,800000003,800000004,800000005,800000006,800000007,800000008,800000009,800000021,800000022,800000023,800000024,800000025,800000026,800000027,800000028,800000029,800000031,800000032,800000033,800000034,800000035,800000036,800000037,800000038,800000039}
	if type == 1 then
		randomly = {700000882, 800000042, 800000042, 800000042, 800000042}
		in_randomly = math.random(1,#randomly)
		notice(text)
		insert_item(randomly[in_randomly], 1)
	elseif type == 2 then
		-- prize_id = {4,5,6}
		-- prize_cnt = {1,1,1}
		-- in_random = math.random(1,#prize_cnt)
	end
	
	in_random = math.random(1,#prize_id)
	insert_item(prize_id[in_random], 1)
end 

function Xmas_MonstersCall()
	local daddy_id = 51000101
	local angry_id = 51000102
	local daddy_cnt =1
	local angry_cnt = 6
	local x = {107289,103815,98845,104618,94298}
	local y = {121978,128250,129237,131256,126116}
	local l_name = {'@99910006', '@99910006', '@99910006', '@99910006', '@99910006'}
	local call_type = 1 
	--1 : To-spawn in all locations
	--2 : To-spawn in one r-location
	local text,in_random

	if call_type == 1 then

		for i = 1 , #x do
			add_npc(x[i],y[i],daddy_id,daddy_cnt)
			add_npc(x[i],y[i],angry_id,angry_cnt)
		end
		for i = 1 , #x do
			add_npc(x[i],y[i],daddy_id,daddy_cnt)
			add_npc(x[i],y[i],angry_id,angry_cnt)
		end
		notice("@99910006")

	elseif call_type == 2 then

		in_random = math.random(1,#x)
		add_npc(x[in_random],y[in_random],daddy_id,daddy_cnt)
		add_npc(x[in_random],y[in_random],angry_id,angry_cnt)
		text = sconv( "@99910007", "#@l_name@#", tostring(l_name[in_random]) )
		notice(text)

	end 

end 

function on_use_xmas_box(type)
	local prize_id,prize_cnt,in_random
	local in_type = 1 
	local cnt_to_add = 2
	--1 : To-give all prizes , or specific cnt , by cnt_to_add
	--2 : To-give one prize

	-- جورب الثلج
	if type == 1 then
		prize_id = {2013681,2013141,2016099,705015}--حقيبة الفرح البيضاء-
		prize_cnt = {1,1,1,1}
		in_type = 2
	-- جورب الشتاء
	elseif type == 2 then
		-- prize_id = {2013204,2013141,960123} --هافانا مؤقتة-رحيق ازهار اللافندر -موريندا
		prize_id = {1000548,1000549,601100443, 601100444} -- بجع عضلي 7 ايام - بجع سحري 7 ايام - لفافة نقل تطوير 24 - لفافاة نقل تطوير 25
		prize_cnt = {1,1,1,1}
		in_type = 2
	-- جورب الحلوى
	elseif type == 3 then
		prize_id = {2013513,2013514,2013515,2013204} --كريموسا -روزماري -تروبيكال - مورياندا
		prize_cnt = {1,1,1,1}
		in_type = 2
	end

	if in_type == 1 then
		for i = 1 , #prize_id do
			insert_item(prize_id[i],prize_cnt[i])
		end 

	elseif in_type == 2 then
		in_random = math.random(1,#prize_id)
		insert_item(prize_id[in_random],prize_cnt[in_random])

	end 

end 
 
function xmas_area(in_out)
	-- notice(in_out)
	local flag = GAF("xmas_2018")
	local time_to_add = 300
	local hp = get_value("hp")
	
	-- set_account_flag("xmas_2018_where", in_out)
	
	-- if hp > 0 then
		-- if in_out == 1 then
			
			-- set_account_flag("xmas_2018", get_os_time() + time_to_add)
			-- if flag < get_os_time() then
				-- set_account_flag("xmas_2018", get_os_time() + time_to_add)
			-- else
				
			-- end
		-- else
			-- set_account_flag("xmas_2018", get_os_time() - 30000)
			-- private_notice("@99910008")
		-- end
	-- else
		-- set_account_flag("xmas_2018", get_os_time() - 30000)
	-- end
end 


function check_xmas_where()
	local prize_id = 800000043
	local prize_cnt = 1
	local time_to_add = 300
	local hp = get_value("hp")
	local current_x = get_value("x")
	local current_y = get_value("y")
	
	local time_event = GAF("xmas_2018")
	local where = GAF("xmas_2018_where")
	
	-- if current_x >= 100968 and current_x <= 101514 then
		-- if current_y >= 122640 and current_y <= 123228 then

			-- if where == 1 then
				-- if hp > 0 then
					-- if time_event < get_os_time() then
					
						-- set_account_flag("xmas_2018", get_os_time() + time_to_add)
						-- private_notice("@99910010")
						-- insert_item(prize_id,prize_cnt)
					-- end
				-- else
					-- set_account_flag("xmas_2018", get_os_time() + 30000)
				-- end
			-- else
			
			-- end
		
		-- end
	-- end
end

function GAF(flag)
	local in_flag = get_account_flag(flag)

	if in_flag == "" or in_flag == nil then
		return 0
	end 

	return in_flag
end 

function spawn_xmasprop() --server_init
	local x = {86518,107961,107584,92095,85498,85099,100042}
	local y = {128758,128005,120475,127788,135248,126948,116648}
	local id = 600001
	
	for i = 1 ,#x do
		add_field_prop( id, 0, x[i], y[i], 0, 0 )
	end
end 

function quest_start_21001()

end

]]--