-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_JobChange"
end

function NPC_JobChange_get_resetcount()
	local reset_count = get_flag( 'reset_count' )
	
	if reset_count == '' then
		return 0
	else
		return reset_count
	end
end

   --============================================================
   --             <<<<<< 전직 NPC (공통) >>>>>>
   --============================================================
function NPC_JobChange_contact()
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 도시 유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_title( "@90703201" )
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )
	
	end -- if 끝
	
	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_text( "@90400403" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_text( "@90100403" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_text( "@90200403" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_text( "@90600403" )
	-- 도시 유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_text( "@90703203" )				
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_text( "@90700403" )
	end -- if 끝
			
	dlg_menu( "@90010060", "NPC_JobChange_change_job()" )
	dlg_menu( "@90010252", "NPC_JobChange_change_race()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

function NPC_JobChange_change_race()
	local npc_id = get_npc_id()
	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 도시 유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_title( "@90703201" )
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )
	--수련자의 섬 일 때 (3019)
	elseif npc_id == 3019 then
		dlg_title( "@90300401" )
		
	end -- if 끝
	
	if has_item(601100284) >= 1 then
		dlg_text( "@90010256" )

		local race = get_value( "race" )
	
		if race ~= 4 then
			dlg_menu( "@90010253", "NPC_JobChange_set_race(4)" )
		end
	
		if race ~= 5 then 
			dlg_menu( "@90010254", "NPC_JobChange_set_race(5)" )
		end
	
		if race ~= 3 then 
			dlg_menu( "@90010255", "NPC_JobChange_set_race(3)" )
		end
	
		dlg_show()
	else
		dlg_text( "@90010257" )
		dlg_menu( "@90010002", " " )
		dlg_show()
		
	end
	
end

function NPC_JobChange_set_race( changed_race )
	if has_item(601100284) >= 1 then -- 심장 있는지 체크 (아까 체크했지만 스크립트 열어 놓고 이것저것 할수있으므로 다시체크)
		local success = set_race( changed_race )
		local npc_id = get_npc_id()
		-- 가이아 일 때 (4004)
		if npc_id == 4004 then
			dlg_title( "@90400401" )
		-- 데바 일 때 (1004)
		elseif npc_id == 1004 then
			dlg_title( "@90100401" )
		-- 아수라 일 때 (2004)
		elseif npc_id == 2004 then
			dlg_title( "@90200401" )
		-- 론도 일 때 (6004)
		elseif npc_id == 6004 then
			dlg_title( "@90600401" )
		-- 도시 유적 일 때 (7032)
		elseif npc_id == 7032 then
			dlg_title( "@90703201" )
		-- 시크루트 일 때 (7004)
		elseif npc_id == 7004 then
			dlg_title( "@90700401" )
		--수련자의 섬 일 때 (3019)
		elseif npc_id == 3019 then
			dlg_title( "@90300401" )		
		end -- if 끝
	
		if success == 1 then
			delete_item( get_item_handle( 601100284 ), 1 )
			dlg_text( "@90010258" )
			dlg_menu( "@90010002", " " )
		else
			dlg_text( "@90010259" )
			dlg_menu( "@90010002", " " )
		end
	else
		dlg_text( "@90010257" )
		dlg_menu( "@90010002", " " )
	end

	dlg_show()
end

function NPC_JobChange_change_job()
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	-- 국가 코드 읽어오기
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_EUROPE                     = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
		
	local state_code = get_local_info()
	local reset_count = NPC_JobChange_get_resetcount()
	
	
	
	--reset_count = 0
	
	
	--================================================================================
	-- 전직 불가능한 경우 골라내기
	--================================================================================
	
	--local reset_count

	-- 기본직업일 때 Lv이나 JLv이 10 미만이면 전직불가.
	if get_value( "job_depth" ) == 0 then
		if get_value( "level" ) < 10 or get_value( "job_level" ) < 10 then

			-- 가이아 일 때 (4004)
			if npc_id == 4004 then
				dlg_title( "@90400401" )
			-- 데바 일 때 (1004)
			elseif npc_id == 1004 then
				dlg_title( "@90100401" )
			-- 아수라 일 때 (2004)
			elseif npc_id == 2004 then
				dlg_title( "@90200401" )
			-- 론도 일 때 (6004)
			elseif npc_id == 6004 then
				dlg_title( "@90600401" )
			-- 도시 유적 일 때 (7032)
			elseif npc_id == 7032 then
				dlg_title( "@90703201" )
			-- 시크루트 일 때 (7004)
			elseif npc_id == 7004 then
				dlg_title( "@90700401" )
			end -- if 끝
			
			-- 가이아 일 때 (4004)
			if npc_id == 4004 then
				dlg_text( "@90400403" )
			-- 데바 일 때 (1004)
			elseif npc_id == 1004 then
				dlg_text( "@90100403" )
			-- 아수라 일 때 (2004)
			elseif npc_id == 2004 then
				dlg_text( "@90200403" )
			-- 론도 일 때 (6004)
			elseif npc_id == 6004 then
				dlg_text( "@90600403" )
			-- 도시 유적 일 때 (7032)
			elseif npc_id == 7032 then
				dlg_text( "@90703203" )
				
			-- 시크루트 일 때 (7004)
			elseif npc_id == 7004 then
				dlg_text( "@90700403" )
			end -- if 끝
		
			dlg_menu( "@90010002", "" )
		
			dlg_show()

			return
		end

	-- 1차 직업일 때 Lv50 미만 JLv이 40 미만이면 전직불가.
	elseif get_value( "job_depth" ) == 1 then
	
		-- 중국은 2차 직업 막음. 2007.9.4. 중국 판호 관련으로 임시 해제.
	 	--if state_code == 2 then
			-- 더 이상 전직이 불가능하다는 멘트
	      --dlg_text( "@90100411" )
		
		  --dlg_menu( "@90010002", "" )
		
          	--dlg_show()

	      --return
	      --end
		
		-- 전직 불가능한 조건이면 튕김
		if get_value( "level" ) < 50 or get_value( "job_level" ) < 40 then
		
			-- 가이아 일 때 (4004)
			if npc_id == 4004 then
				dlg_title( "@90400401" )
			-- 데바 일 때 (1004)
			elseif npc_id == 1004 then
				dlg_title( "@90100401" )
			-- 아수라 일 때 (2004)
			elseif npc_id == 2004 then
				dlg_title( "@90200401" )
			-- 론도 일 때 (6004)
			elseif npc_id == 6004 then
				dlg_title( "@90600401" )
			-- 도시 유적 일 때 (7032)
			elseif npc_id == 7032 then
				dlg_title( "@90703201" )				
			-- 시크루트 일 때 (7004)
			elseif npc_id == 7004 then
				dlg_title( "@90700401" )
			end -- if 끝
			
			-- 가이아 일 때 (4004)
			if npc_id == 4004 then
				if is_able_to_jobchange() == true then
					dlg_text( "@90400409" )
				else
					dlg_text( "@90400402" )
				end
			-- 데바 일 때 (1004)
			elseif npc_id == 1004 then
				if is_able_to_jobchange() == true then
					dlg_text( "@90100409" )
				else
					dlg_text( "@90100402" )
				end
			-- 아수라 일 때 (2004)
			elseif npc_id == 2004 then
				if is_able_to_jobchange() == true then
					dlg_text( "@90200409" )
				else
					dlg_text( "@90200402" )
				end
			-- 론도 일 때 (6004)
			elseif npc_id == 6004 then
				if is_able_to_jobchange() == true then
					dlg_text( "@90600409" )
				else
					dlg_text( "@90600402" )
				end
			-- 도시 유적 일 때 (7032)
			elseif npc_id == 7032 then
				if is_able_to_jobchange() == true then
					dlg_text( "@90703209" )
				else
					dlg_text( "@90703202" )
				end
			-- 시크루트 일 때 (7004)
			elseif npc_id == 7004 then
				if is_able_to_jobchange() == true then
					dlg_text( "@90700409" )
				else
					dlg_text( "@90700402" )
				end
			end -- if 끝

			dlg_menu( "@90010002", "" )
			dlg_show()
			return

		-- 전직 가능할 때 테섭이 아니면 튕김.
	
		else
			if is_able_to_jobchange() == false then
				-- 가이아 일 때 (4004)
				if npc_id == 4004 then
					dlg_title( "@90400401" )
				-- 데바 일 때 (1004)
				elseif npc_id == 1004 then
					dlg_title( "@90100401" )
				-- 아수라 일 때 (2004)
				elseif npc_id == 2004 then
					dlg_title( "@90200401" )
				-- 론도 일 때 (6004)
				elseif npc_id == 6004 then
					dlg_title( "@90600401" )
				-- 도시 유적 일 때 (7032)
				elseif npc_id == 7032 then
					dlg_title( "@90703201" )
				-- 시크루트 일 때 (7004)
				elseif npc_id == 7004 then
					dlg_title( "@90700401" )
				end -- if 끝
				
				-- 가이아 일 때 (4004)
				if npc_id == 4004 then
					dlg_text( "@90400402" )
				-- 데바 일 때 (1004)
				elseif npc_id == 1004 then
					dlg_text( "@90100402" )
				-- 아수라 일 때 (2004)
				elseif npc_id == 2004 then
					dlg_text( "@90200402" )
				-- 론도 일 때 (6004)
				elseif npc_id == 6004 then
					dlg_text( "@90600402" )
				-- 도시 유적 일 때 (7032)
				elseif npc_id == 7032 then
					dlg_text( "@90703202" )
				-- 시크루트 일 때 (7004)
				elseif npc_id == 7004 then
					dlg_text( "@90700402" )
				end -- if 끝
				dlg_menu( "@90010002", "" )
				dlg_show()
				return
			end
		end

	-- 여기까지 오면 2차직업인 상태. 3차로의 전직은 불가능함
	elseif get_value( "job_depth" ) == 3 then
	
		-- 가이아 일 때 (4004)
		if npc_id == 4004 then
			dlg_title( "@90400401" )
		-- 데바 일 때 (1004)
		elseif npc_id == 1004 then
			dlg_title( "@90100401" )
		-- 아수라 일 때 (2004)
		elseif npc_id == 2004 then
			dlg_title( "@90200401" )
		-- 론도 일 때 (6004)
		elseif npc_id == 6004 then
			dlg_title( "@90600401" )
		-- 도시유적 일 때 (7032)
		elseif npc_id == 7032 then
			dlg_title( "@90703201" )
			
		-- 시크루트 일 때 (7004)
		elseif npc_id == 7004 then
			dlg_title( "@90700401" )

		end -- if 끝


		
		if reset_count == 0 then
			dlg_text( "@90604793" )
		elseif reset_count == 1 then
			dlg_text( "@90604794" )
		elseif reset_count == 2 then
			dlg_text( "@90604795" )
		elseif reset_count == 3 then
			dlg_text( "@90604796" )
		elseif reset_count == 4 then
			dlg_text( "@90604797" )
		elseif reset_count == 5 then
			dlg_text( "@90604798" )
		elseif reset_count == 6 then
			dlg_text( "@90604799" )
		elseif reset_count == 7 then
			dlg_text( "@90604800" )
		elseif reset_count == 8 then
			dlg_text( "@90604801" )
		elseif reset_count == 9 then
			dlg_text( "@90604802" )
		end
				

		-- 마스터 클래스 초기화 스킬 가능 멘트					
		

			
		
		-- 루피로 초기화하기		
		dlg_menu( "@90604803", "gold_skill_reset_check(npc_id)")
		-- JP로 초기화하기
		dlg_menu( "@90604804", "jp_skill_reset_check(npc_id)")
		
		-- 아이템 테스트용
		--dlg_menu( "Skill reset count Init", "Skill_reset_count_init()")		
		--dlg_menu( "Item skill reset", "item_skill_reset()")
							
		dlg_menu( "@90010002", "" )
			
		
		dlg_show()

		return
	
	else
		-- 가이아 일 때 (4004)
		if npc_id == 4004 then
			dlg_title( "@90400401" )
		-- 데바 일 때 (1004)
		elseif npc_id == 1004 then
			dlg_title( "@90100401" )
		-- 아수라 일 때 (2004)
		elseif npc_id == 2004 then
			dlg_title( "@90200401" )
		-- 론도 일 때 (6004)
		elseif npc_id == 6004 then
			dlg_title( "@90600401" )
		-- 도시유적 일 때 (7032)
		elseif npc_id == 7032 then
			dlg_title( "@90703201" )
			
		-- 시크루트 일 때 (7004)
		elseif npc_id == 7004 then
			dlg_title( "@90700401" )
		end -- if 끝

		-- 더 이상 전직이 불가능하다는 멘트
		-- dlg_text( "@90100411" )
		-- 마스터 클래스로 전직을 원한다면 영웅 헥토르를 만나라는 멘트
		dlg_text( "@91002563" )	
					
		dlg_menu( "@90010002", "" )
		
		dlg_show()

		return
	end

	

	-- 여기까지 왔으면 전직 가능
	
	local race = get_value( "race" )
	local job_name1, job1, job_name2, job2, job_name3, job3, number_job, current_job

	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 도시 유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_title( "@90703201" )
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )
	end -- if 끝

	-- 전직 가능한 직업의 개수를 초기화. 기본이 3이니 그냥 3으로...
	number_job = 3
	
	-- 현재 직업 ID를 받아 놓는다.
	current_job = get_value( "job" )

	--================================================================================
	-- 1차 전직일 경우.		(테이블에서 못읽어오므로 전직정보는 여기서 고치자)
	--================================================================================
	if get_value( "job_depth" ) == 0 then

		dlg_text( "@90100404" )
		-- 전직 가능한 직업의 개수를 설정. 1차 전직이므로 모두 3 이다.
		number_job = 3
	
		--============================================================================
		-- 얘가 데바면 파이터 201, 클레릭 202, 테이머 203
		if race == 4 then
			job_name1 =  "@10201" 
			job1 = 201
			job_name2 =  "@10202" 
			job2 = 202
			job_name3 =  "@10203" 
			job3 = 203

		--============================================================================
		-- 아수라면 스트라이더 301, 매지션 302, 서모너 303
		elseif race == 5 then
			job_name1 =  "@10301" 
			job1 = 301
			job_name2 =  "@10302" 
			job2 = 302
			job_name3 =  "@10303" 
			job3 = 303
	
		--============================================================================
		-- 가이아라면 투사 101, 주술자 102, 야수사 103
		else
			job_name1 =  "@10101" 
			job1 = 101
			job_name2 =  "@10102" 
			job2 = 102
			job_name3 =  "@10103" 
			job3 = 103
		end

	--================================================================================
	-- 2차 전직일 경우.		(테이블에서 못읽어오므로 전직정보는 여기서 고치자)
	--================================================================================
	elseif get_value( "job_depth" ) == 1 then
	
		dlg_text( "@90700410" )

		--============================================================================
		-- 데바일 때
		if race == 4 then

			-- 파이터 전직 전직		
			if current_job == 201 then
				-- 나이트 210, 워리어 211
				number_job = 2
				
				job_name1 =  "@10210"
				job1 = 210
				job_name2 =  "@10211"
				job2 = 211

			-- 클레릭의 전직
			elseif current_job == 202 then
				-- 메이지 212, 프리스트 213
				number_job = 2
				
				job_name1 =  "@10212" 
				job1 = 212
				job_name2 =  "@10213" 
				job2 = 213
				

			-- 테이머의 전직		
			elseif current_job == 203 then
				-- 브리더
				number_job = 1
				
				job_name1 =  "@10214" 
				job1 = 214
				
				
			-- 이도저도 아니면 아무것도 아니다.
			else
				number_job = 0
			end

		--============================================================================
		-- 아수라
		elseif race == 5 then

			-- 스트라이더의 전직		
			if current_job == 301 then
				-- 어쌔신 310, 레인저 311
				number_job = 2
				
				job_name1 =  "@10310" 
				job1 = 310
				job_name2 =  "@10311" 
				job2 = 311

			-- 다크매지션의 전직		
			elseif current_job == 302 then
				-- 소서러 312, 다크매지션 313
				number_job = 2
				
				job_name1 =  "@10312" 
				job1 = 312
				job_name2 =  "@10313" 
				job2 = 313

			-- 서모너 전직		
			elseif current_job == 303 then
				-- 
				number_job = 1
				
				job_name1 =  "@10314" 
				job1 = 314
				
				
			-- (무시.)
			else
				number_job = 0
			end
	
		--============================================================================
		-- 가이아
		else
			-- 투사의 전직	101	
			if current_job == 101 then
				-- 전사 110, 궁사 111
				number_job = 2
				
				job_name1 =  "@10110" 
				job1 = 110
				job_name2 =  "@10111" 
				job2 = 111

			-- 주술사의 전직
			elseif current_job == 102 then
				-- 마도사 112, 마투사 113
				number_job = 2
				
				job_name1 =  "@10112" 
				job1 = 112
				job_name2 =  "@10113" 
				job2 = 113

			-- 야수사의 전직
			elseif current_job == 103 then
				-- 마수사 114
				number_job = 1
				
				job_name1 =  "@10114" 
				job1 = 114
			-- 이도저도 아니면 아무것도 아니다.
			else
				number_job = 0
			end	
		end

	end
	
	if number_job == 1 then
		dlg_menu( job_name1 , "Run_JobChange_check_common( '" .. job_name1 .. "' , " .. job1 .. " )" )
		
	elseif number_job == 2 then
		dlg_menu( job_name1 , "Run_JobChange_check_common( '" .. job_name1 .. "' , " .. job1 .. " )" )
		dlg_menu( job_name2 , "Run_JobChange_check_common( '" .. job_name2 .. "' , " .. job2 .. " )" )
	
	elseif number_job == 3 then
		dlg_menu( job_name1 , "Run_JobChange_check_common( '" .. job_name1 .. "' , " .. job1 .. " )" )
		dlg_menu( job_name2 , "Run_JobChange_check_common( '" .. job_name2 .. "' , " .. job2 .. " )" )
		dlg_menu( job_name3 , "Run_JobChange_check_common( '" .. job_name3 .. "' , " .. job3 .. " )" )

	else
	
	end
		
	dlg_menu( "@90010001", '' )
 
	dlg_show()
	
end

----------------------------------------------------------------------------------------------------------------------
function NPC_master_partdevil_contact()
 
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	-- 국가 코드 읽어오기
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_EUROPE                     = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
	
	local state_code = get_local_info()

	local quest_progress3322 = get_quest_progress(3322)		

	--================================================================================
	-- 전직 불가능한 경우 골라내기
	--================================================================================
	-- 마스터 클래스 전직 가능 여부			
	if get_value( "job_depth" ) == 2 then
	
	
		-- 전직 불가능한 조건이면 튕김
		-- 전직 조건 변경 : 145 -> 148 2011년 4월 18일 우상욱
		if get_value( "level" ) < 147 or get_value( "job_level" ) < 49 then
			-- 마스터 사탄소녀
			dlg_title( "@91002405" )
						
				if is_able_to_jobchange() == true then
					dlg_text( "@91002407" )
					dlg_menu( "@90010003", "goto_hactor()" )
				else
					--지금은 마스터클래스로의 전직이 불가능한 상태입니다.
					dlg_text( "@91002406" )
					dlg_menu( "@90010003", "goto_hactor()" )
				end

			dlg_menu( "@90010002", "" )
			dlg_show()
			return

		-- 전직 가능할 때 테섭이 아니면 튕김.
		else
			if is_able_to_jobchange() == false then
				dlg_title( "@91002405" )
				dlg_text( "@91002406" )
						
				dlg_menu( "@90010002", "" )
				dlg_show()
				return
			end
		end

	-- 이미 3차직업인 상태. 더이상 전직은 불가능함
	elseif get_value("job_depth") == 3 then
		
		dlg_title( "@91002405" )
		-- 더 이상 전직이 불가능하다는 멘트
		dlg_text( "@91002408" )
		
		--TP 포인트란?
		dlg_menu( "@91002550", "tp_point_2()")
		
		
		dlg_menu( "@90010003", 'goto_hactor()' )
		
		dlg_menu( "@90010002", "" )
		
		dlg_show()

		return
	-- 1차 직업이 여기서 뭐하니? 누구랑 왔니? 전직 하고 와라 -_-
	else
		dlg_title( "@91002405" )

		if get_value("job") == 200 then
			job_name = "@1356"
		elseif get_value("job") == 201 then
			job_name = "@1357"
		elseif get_value("job") == 202 then
			job_name = "@1358"
		elseif get_value("job") == 203 then
			job_name = "@1359"
		elseif get_value("job") == 100 then
			job_name = "@1342"
		elseif get_value("job") == 101 then
			job_name = "@1343"
		elseif get_value("job") == 102 then
			job_name = "@1344"
		elseif get_value("job") == 103 then
			job_name = "@1345"
		elseif get_value("job") == 300 then
			job_name = "@1372"
		elseif get_value("job") == 301 then
			job_name = "@1373"
		elseif get_value("job") == 302 then
			job_name = "@1374"
		elseif get_value("job") == 303 then
			job_name = "@1375"
		end
		

		dlg_text_without_quest_menu(sconv("@91002426", "#@job_name@#",job_name))
		
		dlg_menu( "@90010002", "" )
		dlg_show()

		return
	end

	

	-- 여기까지 왔으면 전직 가능
	
	local race = get_value( "race" )
	local job_name1, job1, job_name2, job2, job_name3, job3, number_job, current_job

	dlg_title( "@91002405" )
	

	-- 전직 가능한 직업의 개수를 초기화. 기본이 3이니 그냥 3으로...
	number_job = 3
	
	-- 현재 직업 ID를 받아 놓는다.
	current_job = get_value( "job" )


	--================================================================================
	-- 마스터 클래스 전직일 경우.		(테이블에서 못읽어오므로 전직정보는 여기서 고치자)
	--================================================================================
	if get_value( "job_depth" ) == 2 then
		
		
		-- 퀘스트 상태 체크 	get_quest_progress(ID)  
		-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
		if quest_progress3322 == 255 then


			dlg_text( "@91002409" ) --마스터클래스 전직 대상자 대사
			--============================================================================
			-- 데바일 때
			if race == 4 then

				-- 나이트 전직 전직		
				if current_job == 210 then
					-- 가즈나이트 220
					number_job = 1
				
					job_name1 =  "@10220"
					job1 = 220
					

				-- 워리어 전직 전직		
				elseif current_job == 211 then
					-- 히어로 221
					number_job = 1
				
					job_name1 =  "@10221"
					job1 = 221

				-- 메이지의 전직
				elseif current_job == 212 then
					-- 아크메이지 222
					number_job = 1
				
					job_name1 =  "@10222" 
					job1 = 222
				
				-- 프리스트의 전직
				elseif current_job == 213 then
					--  세인트 223
					number_job = 1
				
					job_name1 =  "@10223" 
					job1 = 223

				-- 브리더의 전직		
				elseif current_job == 214 then
				
					-- 마에스트로 224
					number_job = 1
				
					job_name1 =  "@10224" 
					job1 = 224
				
				
				-- 이도저도 아니면 아무것도 아니다.
				else
					number_job = 0
			end

--============================================================================
			-- 아수라
			elseif race == 5 then

				-- 어쌔신의 전직		
				if current_job == 310 then
					-- 스펙터 320
					number_job = 1
				
					job_name1 =  "@10320" 
					job1 = 320


				-- 레인져의 전직		
				elseif current_job == 311 then
					--  섀도우스토커 321
					number_job = 1
				
					job_name1 =  "@10321" 
					job1 = 321

				-- 소서러의 전직		
				elseif current_job == 312 then
					-- 둠세이어 322
					number_job = 1
				
					job_name1 =  "@10322" 
					job1 = 322


				-- 다크매지션의 전직		
				elseif current_job == 313 then
					-- 소울리버 323
					number_job = 1

					job_name1 =  "@10323" 
					job1 = 323

				-- 배틀서모너 전직		
				elseif current_job == 314 then
				
				-- 길가메쉬 324
					number_job = 1
				
					job_name1 =  "@10324" 
					job1 = 324
									
				-- (무시.)
				else
					number_job = 0
				end

--============================================================================
			-- 가이아
			else
				-- 전사의 전직	
				if current_job == 110 then
					-- 투신 120
					number_job = 1
					
					job_name1 =  "@10120" 
					job1 = 120

	
				-- 궁사의 전직	
				elseif current_job == 111 then
					-- 신궁 121
					number_job = 1
					
					job_name1 =  "@10121" 
					job1 = 121
	

				-- 마도사의 전직
				elseif current_job == 112 then
					-- 현자 122
					number_job = 1
					
					job_name1 =  "@10122" 
					job1 = 122


				-- 마투사의 전직
				elseif current_job == 113 then
					-- 금강 123
					number_job = 1
					
					job_name1 =  "@10123" 
					job1 = 123

				-- 마수사의 전직
				elseif current_job == 114 then
					-- 나찰 124
					number_job = 1
				
					job_name1 =  "@10124" 
					job1 = 124

				-- 이도저도 아니면 아무것도 아니다.
				else
					number_job = 0
				end
			end

		else
			-- 모든 시련을 이겨내고 여기까지 온 것을 축하합니다.<(version:7.3)>			
			dlg_title( "@90300401" )
			dlg_text( "@91002486" )
		
		
			dlg_menu( "@90010003", 'goto_hactor()' )
		
			dlg_menu( "@90010002", " " )
		
			dlg_show()
			return
		end

	else

	end

	if number_job == 1 then
		dlg_menu( job_name1 , "Run_JobChange_check_common( '" .. job_name1 .. "' , " .. job1 .. " )" )
		
	elseif number_job == 2 then
		dlg_menu( job_name1 , "Run_JobChange_check_common( '" .. job_name1 .. "' , " .. job1 .. " )" )
		dlg_menu( job_name2 , "Run_JobChange_check_common( '" .. job_name2 .. "' , " .. job2 .. " )" )
	
	elseif number_job == 3 then
		dlg_menu( job_name1 , "Run_JobChange_check_common( '" .. job_name1 .. "' , " .. job1 .. " )" )
		dlg_menu( job_name2 , "Run_JobChange_check_common( '" .. job_name2 .. "' , " .. job2 .. " )" )
		dlg_menu( job_name3 , "Run_JobChange_check_common( '" .. job_name3 .. "' , " .. job3 .. " )" )

	else
	
	end

	dlg_menu( "@90010003", 'goto_hactor()' )
	
	dlg_menu( "@90010001", '' )
 
	dlg_show()
	
end


function goto_hactor()

	warp( 138645, 75174, gv("layer") )

end


function tp_point()

	-- 다이얼로그 출력
	dlg_title( "@90999851" )
	dlg_text( "@91002551" )


	--특성 스킬이란?
	dlg_menu( "@91002554", "tp_skill()")

	-- 대화종료 
	--dlg_menu( "@90010003","NPC_master_partdevil_contact()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end		

function tp_skill()

	-- 다이얼로그 출력
	dlg_title( "@90999851" )

	--TP 포인트 분배는 어쩌구 저쩌구
	dlg_text( "@91002553" )

	--상위 특성 스킬이란?
	dlg_menu( "@91002556", "upper_tp_skill()")
	
	-- 하위 특성 스킬이란?
	dlg_menu( "@91002558", "lower_tp_skill()")

	-- 대화종료 

	dlg_menu( "@90010003","tp_point()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end	

function 	upper_tp_skill()

	-- 다이얼로그 출력
	dlg_title( "@90999851" )

	-- 상위 특성 스킬은 어쩌구 저쩌구
	dlg_text( "@91002557" )

	-- 대화종료 

	dlg_menu( "@90010003","tp_skill()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end

function 	lower_tp_skill()

	-- 다이얼로그 출력
	dlg_title( "@90999851" )

	-- 하위 특성 스킬은 어쩌구 저쩌구
	dlg_text( "@91002559" )


	-- 대화종료 

	dlg_menu( "@90010003","tp_skill()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end


-----------------------------------------------------------
-- 사탄소녀 TP 포인트 설명
function tp_point_2()

	-- 다이얼로그 출력
	dlg_title( "@91002405" )
	dlg_text( "@91002551" )


	--특성 스킬이란?
	dlg_menu( "@91002554", "tp_skill_2()")

	-- 대화종료 
	dlg_menu( "@90010003","NPC_master_partdevil_contact()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end		

function tp_skill_2()

	-- 다이얼로그 출력
	dlg_title( "@91002405" )

	--TP 포인트 분배는 어쩌구 저쩌구
	dlg_text( "@91002553" )

	--상위 특성 스킬이란?
	dlg_menu( "@91002556", "upper_tp_skill_2()")
	
	-- 하위 특성 스킬이란?
	dlg_menu( "@91002558", "lower_tp_skill_2()")

	-- 대화종료 

	dlg_menu( "@90010003","tp_point_2()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end	

function 	upper_tp_skill_2()

	-- 다이얼로그 출력
	dlg_title( "@91002405" )

	-- 상위 특성 스킬은 어쩌구 저쩌구
	dlg_text( "@91002557" )

	-- 대화종료 

	dlg_menu( "@90010003","tp_skill_2()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end

function 	lower_tp_skill_2()

	-- 다이얼로그 출력
	dlg_title( "@91002405" )

	-- 하위 특성 스킬은 어쩌구 저쩌구
	dlg_text( "@91002559" )


	-- 대화종료 

	dlg_menu( "@90010003","tp_skill_2()" )	-- 돌아가기

	dlg_menu( "@90010002", " " )
	dlg_show()

end
------------------------------------------------------------------------------------------------------------------------
function Run_JobChange_check_common( job_name , job_id )

	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	local jobid = job_id

	-- 클릭 실수했을지도 모르므로 확실한지 확인해줌.

	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )
	-- 마스터 사탄소녀
	elseif npc_id == 11555 then
		dlg_title( "@91002405" )
	end -- if 끝

	
	
		
	if jobid == 210 then
		dlg_text_without_quest_menu("@90700411")
	elseif jobid == 211 then
		dlg_text_without_quest_menu("@90700412")
	elseif jobid == 212 then
		dlg_text_without_quest_menu("@90700413")
	elseif jobid == 213 then
		dlg_text_without_quest_menu("@90700414")
	elseif jobid == 214 then
		dlg_text_without_quest_menu("@90700415")
	elseif jobid == 310 then
		dlg_text_without_quest_menu("@90700416")
	elseif jobid == 311 then
		dlg_text_without_quest_menu("@90700417")
	elseif jobid == 312 then
		dlg_text_without_quest_menu("@90700418")
	elseif jobid == 313 then
		dlg_text_without_quest_menu("@90700419")
	elseif jobid == 314 then
		dlg_text_without_quest_menu("@90700420")
	elseif jobid == 110 then
		dlg_text_without_quest_menu("@90700421")
	elseif jobid == 111 then
		dlg_text_without_quest_menu("@90700422")
	elseif jobid == 112 then
		dlg_text_without_quest_menu("@90700423")
	elseif jobid == 113 then
		dlg_text_without_quest_menu("@90700424")
	elseif jobid == 114 then
		dlg_text_without_quest_menu("@90700425")
	elseif jobid == 101 then
	    dlg_text_without_quest_menu("@90301919")
  	elseif jobid == 102 then
	    dlg_text_without_quest_menu("@90301920")
	elseif jobid == 103 then
	    dlg_text_without_quest_menu("@90301921")
	elseif jobid == 201 then
	    dlg_text_without_quest_menu("@90301913")
	elseif jobid == 202 then
	    dlg_text_without_quest_menu("@90301914")
	elseif jobid == 203 then
	    dlg_text_without_quest_menu("@90301915")
  	elseif jobid == 301 then
	    dlg_text_without_quest_menu("@90301916")
	elseif jobid == 302 then
	    dlg_text_without_quest_menu("@90301917")
	elseif jobid == 303 then
	    dlg_text_without_quest_menu("@90301918")
	elseif jobid == 120 then --투신이란
	    dlg_text_without_quest_menu("@91002410")
	elseif jobid == 121 then --신궁이란
	    dlg_text_without_quest_menu("@91002411")
	elseif jobid == 122 then --현자란
	    dlg_text_without_quest_menu("@91002412")
	elseif jobid == 123 then --금강이란
	    dlg_text_without_quest_menu("@91002413")
	elseif jobid == 124 then --나찰이란
	    dlg_text_without_quest_menu("@91002414")
	elseif jobid == 220 then --가즈나이트란
	    dlg_text_without_quest_menu("@91002415")
	elseif jobid == 221 then --히어로란
	    dlg_text_without_quest_menu("@91002416")
	elseif jobid == 222 then --아크메이지란
	    dlg_text_without_quest_menu("@91002417")
	elseif jobid == 223 then --세인트란
	    dlg_text_without_quest_menu("@91002418")
	elseif jobid == 224 then --마에스트로란
	    dlg_text_without_quest_menu("@91002419")
	elseif jobid == 320 then --스펙터란
	    dlg_text_without_quest_menu("@91002420")
	elseif jobid == 321 then --섀도우스토커란
	    dlg_text_without_quest_menu("@91002421")
	elseif jobid == 322 then --둠세이어란
	    dlg_text_without_quest_menu("@91002422")
	elseif jobid == 323 then --소울리버란
	    dlg_text_without_quest_menu("@91002423")
	elseif jobid == 324 then --길가메쉬란
	    dlg_text_without_quest_menu("@91002424")
	end
	
	dlg_menu( "@90010007", "Run_JobChange_common( '" .. job_name .. "' , "  .. job_id ..' )' )

	dlg_menu( "@90010001", '' )
		
	dlg_show()
	
end


function Run_JobChange_common( job_name , job_id )

	
	local current_job, current_jlv, job_depth
	
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	
	-- 시작
	-- NPC 대화 최종 단계에서 마스터 클래스 전직 조건 한번 더 하기
	-- 마스터 사탄소녀
	if npc_id == 11555 then
		--================================================================================
		-- 전직 불가능한 경우 골라내기
		--================================================================================
		-- 마스터 클래스 전직 가능 여부	
		if get_value( "job_depth" ) == 2 then
		
		
			-- 전직 불가능한 조건이면 튕김
			-- 전직 조건 변경 : 145 -> 148 2011년 4월 18일 우상욱
			if get_value( "level" ) < 147 or get_value( "job_level" ) < 49 then
				-- 마스터 사탄소녀
				dlg_title( "@91002405" )
							
					if is_able_to_jobchange() == true then
						dlg_text( "@91002407" )
						dlg_menu( "@90010003", "goto_hactor()" )
					else
						--지금은 마스터클래스로의 전직이 불가능한 상태입니다.
						dlg_text( "@91002406" )
						dlg_menu( "@90010003", "goto_hactor()" )
					end

				dlg_menu( "@90010002", "" )
				dlg_show()
				return

			-- 전직 가능할 때 테섭이 아니면 튕김.
			else
				if is_able_to_jobchange() == false then
					dlg_title( "@91002405" )
					dlg_text( "@91002406" )
							
					dlg_menu( "@90010002", "" )
					dlg_show()
					return
				end
			end

		-- 이미 3차직업인 상태. 더이상 전직은 불가능함
		elseif get_value("job_depth") == 3 then
			
			dlg_title( "@91002405" )
			-- 더 이상 전직이 불가능하다는 멘트
			dlg_text( "@91002408" )
			
			--TP 포인트란?
			dlg_menu( "@91002550", "tp_point_2()")
			
			
			dlg_menu( "@90010003", 'goto_hactor()' )
			
			dlg_menu( "@90010002", "" )
			
			dlg_show()

			return
		-- 1차 직업이 여기서 뭐하니? 누구랑 왔니? 전직 하고 와라 -_-
		else
			dlg_title( "@91002405" )

			if get_value("job") == 200 then
				job_name = "@1356"
			elseif get_value("job") == 201 then
				job_name = "@1357"
			elseif get_value("job") == 202 then
				job_name = "@1358"
			elseif get_value("job") == 203 then
				job_name = "@1359"
			elseif get_value("job") == 100 then
				job_name = "@1342"
			elseif get_value("job") == 101 then
				job_name = "@1343"
			elseif get_value("job") == 102 then
				job_name = "@1344"
			elseif get_value("job") == 103 then
				job_name = "@1345"
			elseif get_value("job") == 300 then
				job_name = "@1372"
			elseif get_value("job") == 301 then
				job_name = "@1373"
			elseif get_value("job") == 302 then
				job_name = "@1374"
			elseif get_value("job") == 303 then
				job_name = "@1375"
			end
			

			dlg_text_without_quest_menu(sconv("@91002426", "#@job_name@#",job_name))
			
			dlg_menu( "@90010002", "" )
			dlg_show()

			return
		end	
	
	end -- if 끝
	-- NPC 대화 최종 단계에서 마스터 클래스 전직 조건 한번 더 하기
	--종료
	

	-- 에헤라디야~~ 전직이요~

	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 도시 유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_title( "@90703201" )
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )
	-- 마스터 사탄소녀
	elseif npc_id == 11555 then
		dlg_title( "@91002405" )
	end -- if 끝
	
	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_text_without_quest_menu(sconv("@90400406", "#@job_name@#",job_name))
		--##끝 축하하네!!<BR>지금 이순간부터 자넨 #@job_name@#(/이)라는 직업을 가지게 되었어.<BR>보다 높은 곳을 향해 정진하도록 하게.'
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_text_without_quest_menu(sconv("@90100406", "#@job_name@#",job_name))
		--##끝 축하하네!!<BR>지금 이순간부터 자넨 #@job_name@#(/이)라는 직업을 가지게 되었어.<BR>보다 높은 곳을 향해 정진하도록 하게.'
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_text_without_quest_menu(sconv("@90200406", "#@job_name@#",job_name))
		--##끝 축하하네!!<BR>지금 이순간부터 자넨 #@job_name@#(/이)라는 직업을 가지게 되었어.<BR>보다 높은 곳을 향해 정진하도록 하게.'
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_text_without_quest_menu(sconv("@90600406", "#@job_name@#",job_name))
		--##끝 축하하네!!<BR>지금 이순간부터 자넨 #@job_name@#(/이)라는 직업을 가지게 되었어.<BR>보다 높은 곳을 향해 정진하도록 하게.'
	-- 도시 유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_text_without_quest_menu(sconv("@90703206", "#@job_name@#",job_name))
		--##끝 축하하네!!<BR>지금 이순간부터 자넨 #@job_name@#(/이)라는 직업을 가지게 되었어.<BR>보다 높은 곳을 향해 정진하도록 하게.'
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_text_without_quest_menu(sconv("@90700406", "#@job_name@#",job_name))
		--##끝 축하하네!!<BR>지금 이순간부터 자넨 #@job_name@#(/이)라는 직업을 가지게 되었어.<BR>보다 높은 곳을 향해 정진하도록 하게.'
	-- 마스터 사탄소녀
	elseif npc_id == 11555 then
		dlg_text_without_quest_menu(sconv("@91002425", "#@job_name@#",job_name))
		local master_up = sconv("@246", "#@user_name@#", get_value("name"),"#@job_name@#",job_name)		
		announce( master_up )

		--##끝 축하하네!!<BR>지금 이순간부터 자넨 #@job_name@#(/이)라는 직업을 가지게 되었어.<BR>보다 높은 곳을 향해 정진하도록 하게.'
	end -- if 끝
	

	dlg_menu( "@90010002", '' )
	
	dlg_show()
	
	-- 전직 전 직업과 잡레벨을 저장한다.
	current_job = get_value( "job" )
	current_jlv = get_value( "job_level" )
	job_depth = get_value( "job_depth" )
	if job_depth == 0 then
		set_value( "job_0", current_job )
		set_value( "jlv_0", current_jlv )
		--0차에서 1차 전직할때 "신성한 불꽃의 힘" 아이템을 1개 지급해준다. 해당 아이템은 잡화상점에서도 판매한다.
		--insert_item( 2013081, 1 ) --아이템 능력치가 너무 좋아서 미적용으로 수정
		--cprint( "@90606144" )
	elseif job_depth == 1 then
		set_value( "job_1", current_job )
		set_value( "jlv_1", current_jlv )
	elseif job_depth == 2 then
		set_value( "job_2", current_job )
		set_value( "jlv_2", current_jlv )

	--마스터클래스 전직 전 직업과 잡레벨 저장
	elseif job_depth == 3 then
		set_value( "job_3", current_job )
		set_value( "jlv_3", current_jlv )
	end
	
	job_depth = job_depth + 1
	-- 전직 시키기.
	set_value( "job" , job_id )
	set_value( "job_level" , 1 )
	set_value( "job_depth" , job_depth )
	--##시작
	message( sconv("@90010017", "#@job_name@#",job_name) )
    --끝 #@job_name@# (/으)로 전직했습니다

	
end


function NPC_TestServer_Support()
 
	-- 임시 변수 선언과 동시에 NPC ID 가져오기
	local npc_id = get_npc_id()
	-- 국가 코드 읽어오기
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_EUROPE                     = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
		
	local state_code = get_local_info()

	

	-- 0차, 2차 직업일때 도우미 서비스 받을 수 없다.
	if get_value( "job_depth" ) == 0 then

		dlg_title( "@91002016" )
		dlg_text( "@91002018" )
		dlg_menu( "@90010002", "" )
		
		dlg_show()
		
		return
	
	end
	
	
	local race = get_value( "race" )
	local job_name1, job1, job_name2, job2, job_name3, job3, number_job, current_job


		
	-- 현재 직업 ID를 받아 놓는다.
	current_job = get_value( "job" )



	--================================================================================
	-- 1차 직업일때 레벨업110, 잡레벨50, 5랭크 지원 아이템을 받을 수 있다.
	--================================================================================
	if get_value( "job_depth" ) == 1 then
	
		dlg_title( "@91002016" )
		dlg_text( "@91002019" )
		dlg_menu( "@91002020", "TestServer_Support_lv_jp()" )
		dlg_menu( "@91002021", "TestServer_Support_5rank_item()" )
		dlg_menu( "@90010002", " " )
		
		dlg_show()
		
		return	
	
	end
	
	if get_value( "job_depth" ) == 2 then
	
		dlg_title( "@91002016" )
		dlg_text( "@91002019" )
		dlg_menu( "@91002020", "TestServer_Support_jp()" )
		dlg_menu( "@91002027", "TestServer_Support_rupee()" )
		dlg_menu( "@90010002", " " )
		
		dlg_show()
		
		return	
	
	end
	
	dlg_menu( "@90010002", " " )	
	 
	dlg_show()
	
end

--2차전직 후 잡 레벨 지원
function TestServer_Support_jp()
 
		
	set_value( "job_level" , 50 )

	dlg_title( "@91002016" )
	dlg_text( "@91002022" )
	dlg_menu( "@90010002", "" )
	dlg_show()
	
end

--2차전직 후 루피 지원
function TestServer_Support_rupee()
	dlg_title( "@91002016" )

	if get_flag( "designteam_bless" ) ~= 1 then
		dlg_text( "@91002025" ) -- <(version:7.2)>루피와 루나칩을 지원해준다는 내용
		
		-- 잡포인트 설정
		add_value( "jp" , 83120300 )
		add_value( "gold" , 500001 )
		
		insert_item( 690303, 4521 )
		cprint( "@91000778" ) -- <(version:7.2)><#6DD66D>아이템을 획득 하였습니다.
		
		--루피지원 플래그 설정
		set_flag( "designteam_bless", 1 )
	else
		dlg_text( "@91002026" ) -- <(version:7.2)>이미 지원을 받으셨네요~ 즐거운 하루 되세요!
		--add_state( 1013, 10, 120000) -- 이동속도 증가( 10레벨 , 20분)
	end
	
	dlg_menu( "@90010002", "" )
	dlg_show()
end

function TestServer_Support_lv_jp()
 
	-- 레벨 및 잡포인트 설정
		
	set_value( "level" , 109 )
	set_value( "job_level" , 50 )

	dlg_title( "@91002016" )
	dlg_text( "@91002022" )
	dlg_menu( "@90010002", "" )
	dlg_show()

	
end

function TestServer_Support_5rank_item()

	-- 넣어줄 아이템ID, 개수
		insert_item ( 18000040, 1 )
		insert_item ( 18000041, 1 )
		insert_item ( 18000042, 1 )
		insert_item ( 18000043, 1 )
		insert_item ( 18000044, 1 )
		insert_item ( 18000045, 1 )
		insert_item ( 18000046, 1 )
		insert_item ( 18000047, 1 )
		insert_item ( 18000048, 1 )
		insert_item ( 18000049, 1 )
		insert_item ( 18000050, 1 )
		insert_item ( 18000051, 1 )
		insert_item ( 18000052, 1 )
		insert_item ( 18000053, 1 )
		insert_item ( 18000054, 1 )
		insert_item ( 18000055, 1 )
		insert_item ( 18000056, 1 )
		insert_item ( 18000057, 1 )
		insert_item ( 18000058, 1 )
		insert_item ( 18000059, 1 )
		insert_item ( 18000060, 1 )
		insert_item ( 18000061, 1 )
		insert_item ( 18000062, 1 )
		insert_item ( 18000063, 1 )
		insert_item ( 18000064, 1 )
		insert_item ( 18000065, 1 )
		insert_item ( 18000066, 1 )
		insert_item ( 18000067, 1 )
		insert_item ( 18000068, 1 )
		insert_item ( 18000069, 1 )
		insert_item ( 18000070, 1 )
		insert_item ( 18000071, 1 )
		insert_item ( 18000072, 1 )
		insert_item ( 18000073, 1 )
		insert_item ( 18000074, 1 )
		insert_item ( 18000075, 1 )

	dlg_title( "@91002016" )
	dlg_text( "@91002022" )
	dlg_menu( "@90010002", "" )
	dlg_show()

end


--------------------------------------------------------------------------------------------------------------------------




------------------------------------------------------------------------------------------------------------------------
------------------------- 수련자의 섬 잡 서포터는 튜토리얼에 있습니다. 이건 사용하지 않습니다. -------------------------
------------------------------------------------------------------------------------------------------------------------



	--=====================================================
	-- 초보자섬 섬 용
	--=====================================================

function NPC_JobChange_Beginner_contact()
 

	--================================================================================
	-- 전직 불가능한 경우 골라내기
	--================================================================================

	-- 기본직업일 때 Lv이나 JLv이 10 미만이면 전직불가.
	if get_value( "job_depth" ) == 0 then
		if get_value( "level" ) < 10 or get_value( "job_level" ) < 10 then
			dlg_title( "@90300401" )
			dlg_text( "@90300403" )
		
			dlg_menu( "@90010002", "" )
			dlg_show()

			return
		end

	-- 1차 직업일 때 Lv50 미만 JLv이 40 미만이면 전직불가. (해제) 2차 직업 전직 모두 막기.
	elseif get_value( "job_depth" ) == 1 then
		if get_value( "level" ) < 50 or get_value( "job_level" ) < 40 then
			dlg_title( "@90300401" )
--			dlg_text( "@90300409" )
			dlg_text( "@90300402" )		--> 임시 : 2차전직 불가능하다는 안내메세지

			dlg_menu( "@90010002", "" )
			dlg_show()
	
			return
		
		else

			dlg_title( "@90300401" )
			dlg_text( "@90300402" )		--> 임시 : 2차전직 불가능하다는 안내메세지

			dlg_menu( "@90010002", "" )
		
			dlg_show()

			return
	
		end

	-- 여기까지 오면 2차직업인 상태. 3차로의 전직은 불가능함.
	else
		dlg_title( "@90300401" )
		dlg_text( "@90300402" )			--> 지금은 3차전직 안된다는 대사

		dlg_menu( "@90010002", "" )
		
		dlg_show()

		return
	end

	

	-- 여기까지 왔으면 전직 가능
	
	local race = get_value( "race" )
	local job_name1, job1, job_name2, job2, job_name3, job3, number_job, current_job

	dlg_title( "@90300401" )

	-- 전직 가능한 직업의 개수를 초기화. 기본이 3이니 그냥 3으로...
	number_job = 3
	
	-- 현재 직업 ID를 받아 놓는다.
	current_job = get_value( "job" )

	--================================================================================
	-- 1차 전직일 경우.		(테이블에서 못읽어오므로 전직정보는 여기서 고치자)
	--================================================================================
	if get_value( "job_depth" ) == 0 then

		dlg_text( "@90300404" )
		-- 전직 가능한 직업의 개수를 설정. 1차 전직이므로 모두 3 이다.
		number_job = 3
	
		--============================================================================
		-- 얘가 데바면 디바인솔져 201 또는 클레릭 203, 브리더 204가 될 수 있다.
		if race == 4 then
			job_name1 =  "@10201" 
			job1 = 201
			job_name2 =  "@10202" 
			job2 = 202
			job_name3 =  "@10203" 
			job3 = 203

		--============================================================================
		-- 아수라면 스트라이더301나 다크매지션303, 소서러304가 될 수 있다.
		elseif race == 5 then
			job_name1 =  "@10301" 
			job1 = 301
			job_name2 =  "@10302" 
			job2 = 302
			job_name3 =  "@10303" 
			job3 = 303
	
		--============================================================================
		-- 데바도 아수라도 아니면 가이아겠지. 투사 101, 주술사 103, 마수사 104 가 될 수 있다.
		else
			job_name1 =  "@10101" 
			job1 = 101
			job_name2 =  "@10102" 
			job2 = 102
			job_name3 =  "@10103" 
			job3 = 103
		end

	
	end
		
	if number_job == 1 then
		dlg_menu( job_name1 , "Run_JobChange_Beginner_check( '" .. job_name1 .. "' , " .. job1 .. " )" )
		
	elseif number_job == 2 then
		dlg_menu( job_name1 , "Run_JobChange_Beginner_check( '" .. job_name1 .. "' , " .. job1 .. " )" )
		dlg_menu( job_name2 , "Run_JobChange_Beginner_check( '" .. job_name2 .. "' , " .. job2 .. " )" )
	
	elseif number_job == 3 then
		dlg_menu( job_name1 , "Run_JobChange_Beginner_check( '" .. job_name1 .. "' , " .. job1 .. " )" )
		dlg_menu( job_name2 , "Run_JobChange_Beginner_check( '" .. job_name2 .. "' , " .. job2 .. " )" )
		dlg_menu( job_name3 , "Run_JobChange_Beginner_check( '" .. job_name3 .. "' , " .. job3 .. " )" )

	else
	
	end


	dlg_menu( "@90010001", '' )
 
	dlg_show()
	
end

function Run_JobChange_Beginner_check( job_name , job_id )

	-- 클릭 실수했을지도 모르므로 확실한지 확인해줌.

	cprint( job_name )

	dlg_title( "@90300401" )
	--##시작
	--dlg_text( job_name ..  "@90300405"  )
	dlg_text_without_quest_menu(sconv("@90300405", "#@job_name@#",job_name))
    --##끝 #@job_name@# (/으)로의 전직이라...<BR>좋은 직업을 골랐군.<BR>확실한가?
	
	dlg_menu( "@90010007", "Run_JobChange_Beginner( '" .. job_name .. "' , "  .. job_id ..' )' )
	dlg_menu( "@90010001", '' )
	
	dlg_show()
	
end


function Run_JobChange_Beginner( job_name , job_id )

	local current_job, current_jlv

	-- 에헤라디야~~ 전직이요~

	dlg_title( "@90300401" )
	--##시작
	dlg_text_without_quest_menu(sconv("@90300406", "#@job_name@#", job_name))
    --##끝 축하하네!!<BR>지금 이순간부터 자넨 #@job_name@#(/이)라는 직업을 가지게 되었어.<BR>보다 높은 곳을 향해 정진하도록 하게.'
	
	dlg_menu( "@90010002", '' )
	
	dlg_show()

	-- 전직 전 직업과 잡레벨을 저장한다.
	current_job = get_value( "job" )
	current_jlv = get_value( "job_level" )
	
	if get_value( "job_depth" ) == 0 then
		set_value( "job_0", current_job )
		set_value( "jlv_0", current_jlv )
	elseif get_value( "job_depth" ) == 1 then
		set_value( "job_1", current_job )
		set_value( "jlv_1", current_jlv )
	elseif get_value( "job_depth" ) == 2 then
		set_value( "job_2", current_job )
		set_value( "jlv_2", current_jlv )
	end
	
	-- 전직 시키기.
	set_value( "job" , job_id )
	set_value( "job_level" , 1 )
	set_value( "job_depth" , 1 )
	--##시작
	message( sconv("@90010017", "#@job_name@#",job_name) )
    --##끝 #@job_name@# (/으)로 전직했습니다.

	
end


------------------------------------------------------------------------------------------------------------------------
------------------------- 수련자의 섬 잡 서포터는 튜토리얼에 있습니다. 이건 사용하지 않습니다. -------------------------
------------------------------------------------------------------------------------------------------------------------


-------------------------------------------------------------------------------------------------------------------------

--마스터 클래스 스킬 초기화 스크립트 메인

--루피로 초기화 하기

function gold_skill_reset_check( npc_id )
 
	local reset_gold = { 10000, 2500000, 5000000, 10000000, 25000000, 50000000, 100000000, 500000000, 1000000000, 2000000000 }
	local reset_count = NPC_JobChange_get_resetcount()
	local cost_gold = reset_gold[ reset_count + 1 ]
	local gold = get_value( 'gold' )	
	
	local npc_id = get_npc_id()
	
	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 도시유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_title( "@90703201" )
		
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )

	end -- if 끝
	

	if gold >= cost_gold then
        
        set_value( 'gold', gold - cost_gold )
		reset_skill( 3 )
		
		if reset_count < 9 then
			set_flag( 'reset_count', reset_count + 1 )
		
			dlg_text( "@90604805" )

			dlg_menu( "@90010002", "" )
		
			dlg_show()
		
			return
		
		else
		
			set_flag( 'reset_count', 9 )
		
			dlg_text( "@90604805" )

			dlg_menu( "@90010002", "" )
		
			dlg_show()
		
			return
		end		
		
	else
		dlg_text( "@90604806" )

		dlg_menu( "@90010002", "" )
		
		dlg_show()
		
		return
		
	end
		
end



--JP로 초기화 하기

function jp_skill_reset_check( npc_id )
 
	local reset_jp = { 2000, 500000, 1000000, 2000000, 5000000, 10000000, 20000000, 100000000, 200000000, 400000000 }
	local reset_count = NPC_JobChange_get_resetcount()
	local cost_jp = reset_jp[ reset_count + 1 ]
	local jp = get_value( 'jp' )	
	
	local npc_id = get_npc_id()
	
	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 도시유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_title( "@90703201" )
		
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )

	end -- if 끝
	
	if jp >= cost_jp then
        
        set_value( 'jp', jp - cost_jp )
		reset_skill( 3 )
		
		if reset_count < 9 then
			set_flag( 'reset_count', reset_count + 1 )
		
			dlg_text( "@90604805" )

			dlg_menu( "@90010002", "" )
		
			dlg_show()
		
			return
		
		else
		
			set_flag( 'reset_count', 9 )
		
			dlg_text( "@90604805" )

			dlg_menu( "@90010002", "" )
		
			dlg_show()
		
			return
		end		
		
	else
		dlg_text( "@90604806" )

		dlg_menu( "@90010002", "" )
		
		dlg_show()
		
		return
		
	end
	
		
end

--마스터 클래스 스킬 초기화 카운터 증가 없이 실행

function item_skill_reset()

	
	reset_skill( 3 )
	cprint("@90604808")
	
	return

		
end

--마스터 클래스 초기화 카운터 초기화 하기

function Skill_reset_count_init()

	local reset_count = NPC_JobChange_get_resetcount()
	
		
 
	set_flag( 'reset_count', 0 )
	cprint("@90604807")
		
	return

		
end

--성별 변환 하기

function Change_Character_Gender()

	local transgender = get_value("sex")
		
	if transgender == 1 then
		set_value("sex", 2) -- 성별 바꿔주고
	
	elseif transgender == 2 then
		set_value("sex", 1)
	
	end
end


-- =============================================================

------------------- 9.2 업데이트 테섭용 전직 -------------------
---- 본섭 패치 시 삭제 // NPC_Resource도 본래대로 되돌릴 것 ----

-- =============================================================

function ChangeJob_Testserver_contact()

	local race = get_value( "race" )
	local npc_id = get_npc_id()
	
	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 도시유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_title( "@90703201" )
		
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )

	end -- if 끝
	
	dlg_text( "이번만 특별히 당신의 능력을 최고로 높여드립니다. 기회는 단 한 번뿐이니 신중하게 선택해주세요." )
	
	if race == 4 then -- 데바일 경우
	
		dlg_menu( "@2088", "ChangeJob_Testserver(21)" ) -- 가즈나이트
		dlg_menu( "@2091", "ChangeJob_Testserver(22)" ) -- 히어로
		dlg_menu( "@2191", "ChangeJob_Testserver(23)" ) -- 아크메이지
		dlg_menu( "@2094", "ChangeJob_Testserver(24)" ) -- 세인트
		dlg_menu( "@2097", "ChangeJob_Testserver(25)" ) -- 마에스트로
	
	elseif race == 5 then -- 아수라일 경우
	
		dlg_menu( "@2125", "ChangeJob_Testserver(31)" ) -- 스펙터
		dlg_menu( "@2128", "ChangeJob_Testserver(32)" ) -- 섀도우스토커
		dlg_menu( "@2131", "ChangeJob_Testserver(33)" ) -- 둠세이어
		dlg_menu( "@2134", "ChangeJob_Testserver(34)" ) -- 소울리버
		dlg_menu( "@2136", "ChangeJob_Testserver(35)" ) -- 길가메쉬
	
	else -- 가이아일 경우
	
		dlg_menu( "@2164", "ChangeJob_Testserver(11)" ) -- 투신
		dlg_menu( "@2167", "ChangeJob_Testserver(12)" ) -- 신궁
		dlg_menu( "@2170", "ChangeJob_Testserver(13)" ) -- 현자
		dlg_menu( "@2173", "ChangeJob_Testserver(14)" ) -- 금강
		dlg_menu( "@2175", "ChangeJob_Testserver(15)" ) -- 나찰
	
	end
	
	dlg_menu( "@90010002", "" )

	dlg_show()
	
end

function ChangeJob_Testserver( job_code )

	local job = get_value( "job" )
	-- local lv = get_value( "level" )
	-- local jlv = get_value( "job_level" )
	-- local j_depth = get_value( "job_depth" )
	
	-- 가이아 일 때 (4004)
	if npc_id == 4004 then
		dlg_title( "@90400401" )
	-- 데바 일 때 (1004)
	elseif npc_id == 1004 then
		dlg_title( "@90100401" )
	-- 아수라 일 때 (2004)
	elseif npc_id == 2004 then
		dlg_title( "@90200401" )
	-- 론도 일 때 (6004)
	elseif npc_id == 6004 then
		dlg_title( "@90600401" )
	-- 도시유적 일 때 (7032)
	elseif npc_id == 7032 then
		dlg_title( "@90703201" )
		
	-- 시크루트 일 때 (7004)
	elseif npc_id == 7004 then
		dlg_title( "@90700401" )

	end -- if 끝

	if job == 100 or job == 200 or job == 300 then
		
		dlg_text( "전직이 완료되었습니다." )
	
		job1( job_code )
		clear_inventory()
		
		insert_item( 602705, 10 ) -- 금괴 넣어주기(창고, 거래, 드랍 등 불가)
		
		insert_item( 540080, 1 ) -- 리뉴얼 바포메트 카드
		insert_item( 540081, 1 ) -- 리뉴얼 슬래터 카드
		insert_item( 540082, 1 ) -- 리뉴얼 운다인 카드
		insert_item( 540083, 1 ) -- 리뉴얼 미노타우르스 카드
		insert_item( 540084, 1 ) -- 리뉴얼 픽 카드

		insert_item( -8000145, 1 ) -- 랜덤 벨트
		insert_item( -4104703, 2 ) -- 랜덤 액세서리 2개
		insert_item( -8000153, 1 ) -- 랜덤 목걸이
		
		private_notice( "전직을 축하합니다. 인벤토리에 선물을 넣어두었으니 확인해보세요." )
		
	else
		dlg_text( "이미 전직이 완료되었습니다." )
	end
	
	dlg_menu( "@90010002", "" )
	
	dlg_show()
	
end

-------------------------------------------------------------------------------------------------------------------------



-------------------------------------------------------------------------------------------------------------------------