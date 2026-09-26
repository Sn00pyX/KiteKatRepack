function buffRevoOther()

	local str = get_string("@90702503")
	local str2 = sconv(get_string("@263"), "#@gold@#", 29000000)
	
	dlg_title( "@90702501" )
	
	dlg_text( str .. " - " .. str2 )

	dlg_menu( "@30163704", "Revo_Super_States(314078, 1)" ) -- لمسة عافية 10%قوة
	
	dlg_menu( "@30314039", "Revo_Super_States(314039, 1)" )			
	
	dlg_menu( "@30163440", "Revo_Super_States(163440, 120)" )			
	
	
		
	--돌아가기
	dlg_menu( "@90010003", "NPC_Foreign_Secroute_mage_contact()" )	

	-- 대화종료 
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function Revo_Super_States(buff_id, power)
	-- local state_id = {1,2,3,4}
	local state_power = 1 --supposed to be 1
	local state_time = 180000 --30 mintues
	local cost = 29*1000000
	local currency = tonumber(gv("gold"))

	
	if currency >= cost then
		add_state(buff_id, power, state_time)
		sv("gold", currency-cost)
	else
		message("@982")
	end

end 
