function creature( rank, slot, class )
	local item_codes
	local item_enhance
	local item_level
	local soul_stone_count
	local soul_stones
	
	local i, handle, lv, flag
	local low, high
	
	if rank == 2 then
		low = 20
		high = 49
		
		if class == 0 then
			item_codes = {101201,103201,105201,107201,108201,111201,221203}
		elseif class == 1 then
			item_codes = {101201,103201,105201,107201,108201,111201,222203}
		elseif class == 2 then
			item_codes = {101201,103201,105201,107201,108201,111201,223203}
		elseif class == 3 then
			item_codes = {101201,103201,105201,107201,108201,111201,224203}
		end
				
		soul_stone_count =	{2}
		soul_stones =	{805301,805601,805301,805601}
	elseif rank == 3 then
		low = 50
		high = 79
		
		if class == 0 then
			item_codes = {101301,103301,105301,107301,108301,111301,261303}
		elseif class == 1 then
			item_codes = {101301,103301,105301,107301,108301,111301,262303}
		elseif class == 2 then
			item_codes = {101301,103301,105301,107301,108301,111301,263303}
		elseif class == 3 then
			item_codes = {101301,103301,105301,107301,108301,111301,264303}
		end
			
		soul_stone_count =	{2}
		soul_stones =	{805301,805601,805301,805601}
	elseif rank == 4 then
		low = 80
		high = 99
		
		if class == 0 then
			item_codes = {101401,103402,105402,107402,108402,111402,261402}
		elseif class == 1 then
			item_codes = {101401,103402,105402,107402,108402,111402,262402}
		elseif class == 2 then
			item_codes = {101401,103402,105402,107402,108402,111402,263402}
		elseif class == 3 then
			item_codes = {101401,103402,105402,107402,108402,111402,264402}
		end
		
		soul_stone_count =	{2}
		soul_stones =	{805301,805601,805301,805601}
	elseif rank == 5 then
		low = 100
		high = 119
		
		if class == 0 then
			item_codes = {101502,103502,105502,107502,108502,111501,261501}
		elseif class == 1 then
			item_codes = {101502,103502,105502,107502,108502,111501,262501}
		elseif class == 2 then
			item_codes = {101502,103502,105502,107502,108502,111501,263501}
		elseif class == 3 then
			item_codes = {101502,103502,105502,107502,108502,111501,264501}
		end
		
		soul_stone_count =	{2}
		soul_stones =	{805301,805601,805301,805601}
	elseif rank == 6 then
		low = 120
		high = 149
		
		if class == 0 then
			item_codes = {101602,103602,105601,107601,108601,111601,221601}
		elseif class == 1 then
			item_codes = {101602,103602,105601,107601,108601,111601,222601}
		elseif class == 2 then
			item_codes = {101602,103602,105601,107601,108601,111601,223601}
		elseif class == 3 then
			item_codes = {101602,103602,105601,107601,108601,111601,224601}
		end
		
		soul_stone_count =	{2}
		soul_stones =	{805301,805601,805301,805601}
	elseif rank == 7 then
		low = 150
		high = 170
		
		if class == 0 then
			item_codes = {101701,103701,105701,107701,108701,111701,271701}
		elseif class == 1 then
			item_codes = {101701,103701,105701,107701,108701,111701,272701}
		elseif class == 2 then
			item_codes = {101701,103701,105701,107701,108701,111701,273701}
		elseif class == 3 then
			item_codes = {101701,103701,105701,107701,108701,111701,274701}
		end
		
		soul_stone_count =	{2}
		soul_stones =	{805301,805601,805301,805601}
	
	elseif rank == 8 then
		low = 150
		high = 170
		
		if class == 0 then
			item_codes = {101701,103701,105701,107701,108701,111701,271702}
		elseif class == 1 then
			item_codes = {101701,103701,105701,107701,108701,111701,272702}
		elseif class == 2 then
			item_codes = {101701,103701,105701,107701,108701,111701,273702}
		elseif class == 3 then
			item_codes = {101701,103701,105701,107701,108701,111701,274702}
		end
		
		soul_stone_count =	{2}
		soul_stones =	{805301,805601,805301,805601}
	
	elseif rank == 9 then
		low = 150
		high = 170
		
		if class == 0 then
			item_codes = {101701,103701,105701,107701,108701,111701,271703}
		elseif class == 1 then
			item_codes = {101701,103701,105701,107701,108701,111701,272703}
		elseif class == 2 then
			item_codes = {101701,103701,105701,107701,108701,111701,273703}
		elseif class == 3 then
			item_codes = {101701,103701,105701,107701,108701,111701,274703}
		end
		
		soul_stone_count =	{2}
		soul_stones =	{805301,805601,805301,805601}
	end
	
	item_enhance =	{10}
	item_level =	{10}
	
	flag = 0
		
	handle = 0
	handle = get_creature_handle(slot)
	if handle ~=0 and handle ~= nil then
		lv = get_creature_value(handle, 'level')
		if( lv >= low and lv <= high ) then
			flag = 1
		else
			return
		end 
	end

	if flag == 0 or item_codes then
		cprint( '크리처가 편성 되어 있지 않거나 입력이 잘 못 되었음' )
		return 
	end

	local item_entry_count = table.getn( item_codes )

	for i = 1, item_entry_count do
		cprint( 'Item' .. i .. ' - ' .. item_codes[ i ] .. '/' .. item_enhance[ i ] .. '/' .. item_level[ i ] .. '/' .. soul_stone_count[ i ] )

		local item_handle = insert_item( item_codes[ i ], 1 )

		if item_handle == 0 then
			cprint( 'Item could not be created: ' .. item_codes[ i ] )
			return
		end

		if get_item_enhance( item_handle ) == 0 then
			set_item_enhance( item_handle, item_enhance[ i ] )
		end
		if get_item_level( item_handle) == 1 then
			set_item_level( item_handle, item_level[ i ] )
		end

		if soul_stone_count[ i ] > 0 then
			for j = 1, soul_stone_count[ i ] do
				set_socket_info( item_handle, j - 1, soul_stones[ j ] )
			end
		end
	end
	return
end
