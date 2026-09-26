
--- Lua 스크립트 암호화
function get_module_name()
             return "ETC_dungeon_prop"
end


--	제어장치 및 코어 점령시 호출될 함수. 
-- 내부적으로 서버에서 제공하는 소유권 변경 함수 및 주변 몹 리젠 함수( respawn_near_mob( range ) )가 사용됨.
function casting_tactical_position( dungeon_id, position_id )

	-- 호출된 지점의 (제어장치 혹은 던전 코어) 소유권을 변경시키는 부분
	change_tactical_position_owner( dungeon_id, position_id )
	
	-- 변경된 지점 주변의 가디언을 리젠
	respawn_guardian_object( dungeon_id, position_id )

	
end

--	던전 출구 클릭시 필드로 텔레포트 시켜 주는 일반 함수.
function exit_dungeon( dungeon_id )

	-- 잃어버린 던전1
	if dungeon_id == 130000 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 155817 + math.random(0,60) , 103724 + math.random(0,60) )
	
	-- 잃어버린 던전2
	elseif dungeon_id == 130600 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 152309 + math.random(0,60) , 102886 + math.random(0,60) )
	
	-- 수정 계곡1
	elseif dungeon_id == 130300 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 103210 + math.random(0,60) , 100366 + math.random(0,60) )

	-- 수정 계곡2
	elseif dungeon_id == 130500 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 99757 + math.random(0,60) , 103236 + math.random(0,60) )

	-- 메마른 달빛의 유적1
	elseif dungeon_id == 130400 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 132995 + math.random(0,60) , 87096 + math.random(0,60) )
	 
	-- 메마른 달빛의 유적2
	elseif dungeon_id == 130700 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 130842 + math.random(0,60) , 79586 + math.random(0,60) )
	
    -- 팔미르 제 1 유적 
	elseif dungeon_id == 130800 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 132680 + math.random(0,60) , 128030 + math.random(0,60) )	

    -- 팔미르 제 2 유적 
	elseif dungeon_id == 130900 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 137441 + math.random(0,60) , 128115 + math.random(0,60) )		

    -- 잃어버린 비밀의 섬(백룡의 쉼터) 
	elseif dungeon_id == 121000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 91942 + math.random(0,60) , 117103 + math.random(0,60) )			

    -- 잃어버린 비밀의 섬(흑룡의 그늘) 
	elseif dungeon_id == 122000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 85752 + math.random(0,60) , 118062 + math.random(0,60) )
		
	-- 잃어버린 비밀의 섬(사룡의 심장) 
	elseif dungeon_id == 123000 then
	    -- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 91998 + math.random(0,60) , 124400 + math.random(0,60) )
			
	-- 엘 카시아 워프 프랍 (엘 카시아 -> 마르두카 폭포 필드)
	elseif dungeon_id == 120700 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 146015 + math.random(0,61) , 135591+ math.random(0,10) )

	-- 숨겨진 팔미르 던전 워프 프랍 (숨겨진 팔미르 입구 -> 팔미르 제 1 실 입구 필드)
	elseif dungeon_id == 120292 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 132680 + math.random(0,60) , 128030 + math.random(0,60) )
	
	-- 숨겨진 수정 계곡 던전 워프 프랍 (숨겨진 수정 계곡 입구 -> 수정 계곡 입구 필드)
	elseif dungeon_id == 70192 then
		-- TO DO : 던전 입구 쪽의 텔레포트 좌표 넣어야 함
		warp( 103234+ math.random(0,60) , 100310 + math.random(0,60) )

	end	
			
end



function warp_gate( prop_id )

		
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능  /  255 : 이미종료
	local quest_progress1 =  get_quest_progress(1244) -- 유령선 나비스라미아: 진행 1
	local quest_progress2 =  get_quest_progress(1245) -- 유령선 나비스라미아: 진행 2
	local quest_progress3 =  get_quest_progress(1247) -- 유령선 나비스라미아: 진행 3
	
	-- 고리수정
	local item_lamia_sujung_1 = find_item ( 1000077 )
	local item_lamia_sujung_handle = get_item_handle ( 1000077 )
   	

	local currentposition = get_value('layer')
	
	local party_id = get_value( 'party_id' )		

	-- 나비스 라미아 갑판  
	if prop_id == 130881 then
		
		if item_lamia_sujung_1 == 1 then
		
		-- 퀘스트 도중 다시 처음부터 시작 할때 진행 불가
		--if quest_progress1 == 2 then	
	
		-- 첫번째 퀘스트 시작전이면(수행중이면)
		--if quest_progress2 == 1 then
		
		-- 텔레포트 좌표
		warp( 190072 + math.random(0,10) , 34171+ math.random(0,10), gv("layer") )
					
		else 
		cprint( "@1233" ) -- '워프게이트 사용을 위한 허가를 받지 못했습니다.'
		end
	end
	
	-- 나비스 라미아 조타실  
	if prop_id == 130882 then
		
		-- 퀘스트 도중 다시 처음부터 시작 할때 진행 불가
		--if quest_progress2 == 2 then	
	
		-- 첫번째 퀘스트 시작전이면(수행중이면)
		--if quest_progress2 == 1 then
		
		-- 텔레포트 좌표
		warp( 189645 + math.random(0,10) , 36119+ math.random(0,10), gv("layer") )
					
		--else 
		--cprint( "@1233" )
		--end
	end
		
	-- 나비스 라미아 주방  
	if prop_id == 130883 then
		
		-- 퀘스트 도중 다시 처음부터 시작 할때 진행 불가
		if quest_progress3 == 2 or quest_progress3 == 255 then	
	
		-- 첫번째 퀘스트 시작전이면(수행중이면)
		--if quest_progress3 == 1 then
		
		-- 텔레포트 좌표
		warp( 190018 + math.random(0,10) , 37865+ math.random(0,10), gv("layer") )
					
		else 
		cprint( "@1233" )
		end
	end
			
	-- 수련자의 섬 선착장 
	if prop_id == 130880 then	
		-- 텔레포트 좌표
		warp(175274 + math.random(0,10) , 62743+ math.random(0,10), gv("layer") )
					
	-- 나비스 라미아 노선실 
	elseif prop_id == 130884 then	
		-- 텔레포트 좌표
		warp( 190008 + math.random(0,10) , 40519+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 노선실 
	elseif prop_id == 130885 then	
		-- 텔레포트 좌표
		warp( 187541 + math.random(0,10) , 36391+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 노선실  
	elseif prop_id == 130886 then	
		-- 텔레포트 좌표
		warp( 187541 + math.random(0,10) , 37946+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 노선실  
	elseif prop_id == 130887 then	
		-- 텔레포트 좌표
		warp( 187541 + math.random(0,10) , 39622+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 노선실 
	elseif prop_id == 130888 then	
		-- 텔레포트 좌표
		warp( 192531 + math.random(0,10) , 39596+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 노선실 
	elseif prop_id == 130889 then	
		-- 텔레포트 좌표
		warp( 192531 + math.random(0,10) , 37943+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 노선실 
	elseif prop_id == 130890 then	
		-- 텔레포트 좌표
		warp( 192531 + math.random(0,10) , 36385+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 식료품창고 
	elseif prop_id == 130874 then	
		-- 텔레포트 좌표
		warp( 189811 + math.random(0,10) , 37968+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 객실1 
	elseif prop_id == 130875 then	
		-- 텔레포트 좌표
		warp( 189811 + math.random(0,10) , 38128+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 잡화창고 
	elseif prop_id == 130876 then	
		-- 텔레포트 좌표
		warp( 189811 + math.random(0,10) , 38323+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 라미아의 눈물 
	elseif prop_id == 130877 then	
		-- 텔레포트 좌표
		warp( 190232 + math.random(0,10) , 38325+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 객실2 
	elseif prop_id == 130878 then	
		-- 텔레포트 좌표
		warp( 190232 + math.random(0,10) , 38127+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 생체실험 준비실 
	elseif prop_id == 130879 then	
		-- 텔레포트 좌표
		warp( 190232 + math.random(0,10) , 37967+ math.random(0,10), gv("layer") )
		
	-- 나비스 라미아 선장실 
	elseif prop_id == 130974 then	
		-- 텔레포트 좌표
		warp( 175249 + math.random(0,10) , 60932+ math.random(0,10), gv("layer") )
	
	-------------------------------------------------------------------------------
								--	팔미르 유적 --
	-------------------------------------------------------------------------------
	
	-- 팔미르 제 1 유적 워프 프랍 1 번	
	elseif prop_id == 130801 then	
		-- 텔레포트 좌표
		warp( 210380 + math.random(0,10) , 136234+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 2 번	
	elseif prop_id == 130802 then	
		-- 텔레포트 좌표
		warp( 219449 + math.random(0,10) , 129851+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 3 번	
	elseif prop_id == 130803 then	
		-- 텔레포트 좌표
		warp( 219913 + math.random(0,10) , 133705+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 4 번	
	elseif prop_id == 130804 then	
		-- 텔레포트 좌표
		warp( 219712 + math.random(0,10) , 133084+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 5 번	
	elseif prop_id == 130805 then	
		-- 텔레포트 좌표
		warp( 214456 + math.random(0,10) , 136681+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 6 번	
	elseif prop_id == 130806 then	
		-- 텔레포트 좌표
		warp( 213631 + math.random(0,10) , 136637+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 7 번	
	elseif prop_id == 130807 then	
		-- 텔레포트 좌표
		warp( 214263 + math.random(0,10) , 137450+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 8 번	
	elseif prop_id == 130808 then	
		-- 텔레포트 좌표
		warp( 220659 + math.random(0,10) , 133638+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 9 번	
	elseif prop_id == 130809 then	
		-- 텔레포트 좌표
		warp( 210201 + math.random(0,10) , 139440+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 10 번	
	elseif prop_id == 130810 then	
		-- 텔레포트 좌표
		warp( 218003 + math.random(0,10) , 129314+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 11 번	
	elseif prop_id == 130811 then	
		-- 텔레포트 좌표
		warp( 217804 + math.random(0,10) , 133780+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 12 번	
	elseif prop_id == 130812 then	
		-- 텔레포트 좌표
		warp( 218185 + math.random(0,10) , 134275+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 13 번	
	elseif prop_id == 130813 then	
		-- 텔레포트 좌표
		warp( 213922 + math.random(0,10) , 134885+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 14 번	
	elseif prop_id == 130814 then	
		-- 텔레포트 좌표
		warp( 214533 + math.random(0,10) , 135565+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 15 번	
	elseif prop_id == 130815 then	
		-- 텔레포트 좌표
		warp( 224491 + math.random(0,10) , 134710+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 16 번	
	elseif prop_id == 130816 then	
		-- 텔레포트 좌표
		warp( 223407 + math.random(0,10) , 134701+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 17 번	
	elseif prop_id == 130817 then	
		-- 텔레포트 좌표
		warp( 217010 + math.random(0,10) , 140208+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 18 번	
	elseif prop_id == 130818 then	
		-- 텔레포트 좌표
		warp( 216860 + math.random(0,10) , 139575+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 19 번	
	elseif prop_id == 130819 then	
		-- 텔레포트 좌표
		warp( 216533 + math.random(0,10) , 140560+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 20 번	
	elseif prop_id == 130820 then	
		-- 텔레포트 좌표
		warp( 223831 + math.random(0,10) , 134943+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 21 번	
	elseif prop_id == 130821 then	
		-- 텔레포트 좌표
		warp( 224101 + math.random(0,10) , 142035+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 22 번	
	elseif prop_id == 130822 then	
		-- 텔레포트 좌표
		warp( 224474 + math.random(0,10) , 141600+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 23 번	
	elseif prop_id == 130823 then	
		-- 텔레포트 좌표
		warp( 223466 + math.random(0,10) , 141318+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 24 번	
	elseif prop_id == 130824 then	
		-- 텔레포트 좌표
		warp( 221410 + math.random(0,10) , 144106+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 25 번	
	elseif prop_id == 130825 then	
		-- 텔레포트 좌표
		warp( 220847 + math.random(0,10) , 143458+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 26 번	
	elseif prop_id == 130826 then	
		-- 텔레포트 좌표
		warp( 220942 + math.random(0,10) , 144434+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 27 번	
	elseif prop_id == 130827 then	
		-- 텔레포트 좌표
		warp( 219150 + math.random(0,10) , 139636+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 28 번	
	elseif prop_id == 130828 then	
		-- 텔레포트 좌표
		warp( 219520 + math.random(0,10) , 140193+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 29 번	
	elseif prop_id == 130829 then	
		-- 텔레포트 좌표
		warp( 220115 + math.random(0,10) , 139207+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 30 번	
	elseif prop_id == 130830 then	
		-- 텔레포트 좌표
		warp( 219486 + math.random(0,10) , 139264+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 31 번	
	elseif prop_id == 130831 then	
		-- 텔레포트 좌표
		warp( 213211 + math.random(0,10) , 142969+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 32 번	
	elseif prop_id == 130832 then	
		-- 텔레포트 좌표
		warp( 217128 + math.random(0,10) , 136129+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 33 번	
	elseif prop_id == 130833 then	
		-- 텔레포트 좌표
		warp( 218883 + math.random(0,10) , 137899+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 3-1 번	
	elseif prop_id == 130836 then	
		-- 텔레포트 좌표
		warp( 219973 + math.random(0,10) , 133090+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 4-1 번	
	elseif prop_id == 130837 then	
		-- 텔레포트 좌표
		warp( 220079 + math.random(0,10) , 133531+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 6-1 번	
	elseif prop_id == 130839 then	
		-- 텔레포트 좌표
		warp( 214272 + math.random(0,10) , 137193+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 7-1 번	
	elseif prop_id == 130840 then	
		-- 텔레포트 좌표
		warp( 213847 + math.random(0,10) , 136674+ math.random(0,10), gv("layer") )
				
	-- 팔미르 제 1 유적 워프 프랍 11-1 번	
	elseif prop_id == 130844 then	
		-- 텔레포트 좌표
		warp( 217946 + math.random(0,10) , 134270+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 12-1 번	
	elseif prop_id == 130845 then	
		-- 텔레포트 좌표
		warp( 217667 + math.random(0,10) , 133968+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 13-1 번	
	elseif prop_id == 130846 then	
		-- 텔레포트 좌표
		warp( 214559 + math.random(0,10) , 135314+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 14-1 번	
	elseif prop_id == 130847 then	
		-- 텔레포트 좌표
		warp( 214191 + math.random(0,10) , 134901+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 15-1 번	
	elseif prop_id == 130848 then	
		-- 텔레포트 좌표
		warp( 223595 + math.random(0,10) , 134708+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 16-1 번	
	elseif prop_id == 130849 then	
		-- 텔레포트 좌표
		warp( 224289 + math.random(0,10) , 134749+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 18-1 번	
	elseif prop_id == 130858 then	
		-- 텔레포트 좌표
		warp( 216570 + math.random(0,10) , 140372+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 19-1 번	
	elseif prop_id == 130859 then	
		-- 텔레포트 좌표
		warp( 216838 + math.random(0,10) , 139733+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 20-1 번	
	elseif prop_id == 130860 then	
		-- 텔레포트 좌표
		warp( 217010 + math.random(0,10) , 140208+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 21-1 번	
	elseif prop_id == 130861 then	
		-- 텔레포트 좌표
		warp( 223629 + math.random(0,10) , 141357+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 22-1 번	
	elseif prop_id == 130862 then	
		-- 텔레포트 좌표
		warp( 224048 + math.random(0,10) , 141882+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 23-1 번	
	elseif prop_id == 130863 then	
		-- 텔레포트 좌표
		warp( 224313 + math.random(0,10) , 141529+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 24-1 번	
	elseif prop_id == 130864 then	
		-- 텔레포트 좌표
		warp( 220995 + math.random(0,10) , 144279+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 25-1 번	
	elseif prop_id == 130865 then	
		-- 텔레포트 좌표
		warp( 221241 + math.random(0,10) , 144075+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 26-1 번	
	elseif prop_id == 130866 then	
		-- 텔레포트 좌표
		warp( 220867 + math.random(0,10) , 143631+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 1 유적 워프 프랍 33-1 번	
	elseif prop_id == 130873 then	
		-- 텔레포트 좌표
		warp( 220001 + math.random(0,10) , 138279+ math.random(0,10), gv("layer") )
		
		
	-- 팔미르 제 2 유적 워프 프랍 1 번	
	elseif prop_id == 130901 then	
		-- 텔레포트 좌표
		warp( 210380 + math.random(0,10) , 152362+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 2 번	
	elseif prop_id == 130902 then	
		-- 텔레포트 좌표
		warp( 219449 + math.random(0,10) , 145979+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 3 번	
	elseif prop_id == 130903 then	
		-- 텔레포트 좌표
		warp( 219913 + math.random(0,10) , 149833+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 4 번	
	elseif prop_id == 130904 then	
		-- 텔레포트 좌표
		warp( 219712 + math.random(0,10) , 149212+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 5 번	
	elseif prop_id == 130905 then	
		-- 텔레포트 좌표
		warp( 214456 + math.random(0,10) , 152809+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 6 번	
	elseif prop_id == 130906 then	
		-- 텔레포트 좌표
		warp( 213631 + math.random(0,10) , 152765+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 7 번	
	elseif prop_id == 130907 then	
		-- 텔레포트 좌표
		warp( 214263 + math.random(0,10) , 153578+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 8 번	
	elseif prop_id == 130908 then	
		-- 텔레포트 좌표
		warp( 220659 + math.random(0,10) , 149766+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 9 번	
	elseif prop_id == 130909 then	
		-- 텔레포트 좌표
		warp( 210201 + math.random(0,10) , 155568+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 10 번	
	elseif prop_id == 130910 then	
		-- 텔레포트 좌표
		warp( 218003 + math.random(0,10) , 145442+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 11 번	
	elseif prop_id == 130911 then	
		-- 텔레포트 좌표
		warp( 217803 + math.random(0,10) , 149850+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 12 번	
	elseif prop_id == 130912 then	
		-- 텔레포트 좌표
		warp( 218185 + math.random(0,10) , 150403+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 13 번	
	elseif prop_id == 130913 then	
		-- 텔레포트 좌표
		warp( 213922 + math.random(0,10) , 151013+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 14 번	
	elseif prop_id == 130914 then	
		-- 텔레포트 좌표
		warp( 214533 + math.random(0,10) , 151693+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 15 번	
	elseif prop_id == 130915 then	
		-- 텔레포트 좌표
		warp( 224491 + math.random(0,10) , 150838+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 16 번	
	elseif prop_id == 130916 then	
		-- 텔레포트 좌표
		warp( 223407 + math.random(0,10) , 150829+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 17 번	
	elseif prop_id == 130917 then	
		-- 텔레포트 좌표
		warp( 217010 + math.random(0,10) , 156336+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 18 번	
	elseif prop_id == 130918 then	
		-- 텔레포트 좌표
		warp( 216860 + math.random(0,10) , 155703+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 19 번	
	elseif prop_id == 130919 then	
		-- 텔레포트 좌표
		warp( 216533 + math.random(0,10) , 156688+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 20 번	
	elseif prop_id == 130920 then	
		-- 텔레포트 좌표
		warp( 223831 + math.random(0,10) , 151071+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 21 번	
	elseif prop_id == 130921 then	
		-- 텔레포트 좌표
		warp( 224101 + math.random(0,10) , 158163+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 22 번	
	elseif prop_id == 130922 then	
		-- 텔레포트 좌표
		warp( 224474 + math.random(0,10) , 157728+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 23 번	
	elseif prop_id == 130923 then	
		-- 텔레포트 좌표
		warp( 223466 + math.random(0,10) , 157446+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 24 번	
	elseif prop_id == 130924 then	
		-- 텔레포트 좌표
		warp( 221410 + math.random(0,10) , 160234+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 25 번	
	elseif prop_id == 130925 then	
		-- 텔레포트 좌표
		warp( 220847 + math.random(0,10) , 159586+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 26 번	
	elseif prop_id == 130926 then	
		-- 텔레포트 좌표
		warp( 220942 + math.random(0,10) , 160562+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 27 번	
	elseif prop_id == 130927 then	
		-- 텔레포트 좌표
		warp( 219150 + math.random(0,10) , 155764+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 28 번	
	elseif prop_id == 130928 then	
		-- 텔레포트 좌표
		warp( 219520 + math.random(0,10) , 156321+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 29 번	
	elseif prop_id == 130929 then	
		-- 텔레포트 좌표
		warp( 220115 + math.random(0,10) , 155335+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 30 번	
	elseif prop_id == 130930 then	
		-- 텔레포트 좌표
		warp( 219486 + math.random(0,10) , 155392+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 31 번	
	elseif prop_id == 130931 then	
		-- 텔레포트 좌표
		warp( 213211 + math.random(0,10) , 159097+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 32 번	
	elseif prop_id == 130932 then	
		-- 텔레포트 좌표
		warp( 217128 + math.random(0,10) , 152257+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 33 번	
	elseif prop_id == 130933 then	
		-- 텔레포트 좌표
		warp( 218883 + math.random(0,10) , 154027+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 3-1 번	
	elseif prop_id == 130936 then	
		-- 텔레포트 좌표
		warp( 219973 + math.random(0,10) , 149218+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 4-1 번	
	elseif prop_id == 130937 then	
		-- 텔레포트 좌표
		warp( 220079 + math.random(0,10) , 149659+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 6-1 번	
	elseif prop_id == 130939 then	
		-- 텔레포트 좌표
		warp( 214272 + math.random(0,10) , 153321+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 7-1 번	
	elseif prop_id == 130940 then	
		-- 텔레포트 좌표
		warp( 213847 + math.random(0,10) , 152802+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 11-1 번	
	elseif prop_id == 130944 then	
		-- 텔레포트 좌표
		warp( 217946 + math.random(0,10) , 150398+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 12-1 번	
	elseif prop_id == 130945 then	
		-- 텔레포트 좌표
		warp( 217725 + math.random(0,10) , 150104+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 13-1 번	
	elseif prop_id == 130946 then	
		-- 텔레포트 좌표
		warp( 214559 + math.random(0,10) , 151442+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 14-1 번	
	elseif prop_id == 130947 then	
		-- 텔레포트 좌표
		warp( 214191 + math.random(0,10) , 151029+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 15-1 번	
	elseif prop_id == 130948 then	
		-- 텔레포트 좌표
		warp( 223595 + math.random(0,10) , 150836+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 16-1 번	
	elseif prop_id == 130949 then	
		-- 텔레포트 좌표
		warp( 224289 + math.random(0,10) , 150877+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 18-1 번	
	elseif prop_id == 130958 then	
		-- 텔레포트 좌표
		warp( 216570 + math.random(0,10) , 156500+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 19-1 번	
	elseif prop_id == 130959 then	
		-- 텔레포트 좌표
		warp( 216838 + math.random(0,10) , 155861+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 20-1 번	
	elseif prop_id == 130960 then	
		-- 텔레포트 좌표
		warp( 217010 + math.random(0,10) , 156336+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 21-1 번	
	elseif prop_id == 130961 then	
		-- 텔레포트 좌표
		warp( 223629 + math.random(0,10) , 157485+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 22-1 번	
	elseif prop_id == 130962 then	
		-- 텔레포트 좌표
		warp( 224048 + math.random(0,10) , 158010+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 23-1 번	
	elseif prop_id == 130963 then	
		-- 텔레포트 좌표
		warp( 224313 + math.random(0,10) , 157657+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 24-1 번	
	elseif prop_id == 130964 then	
		-- 텔레포트 좌표
		warp( 220995 + math.random(0,10) , 160407+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 25-1 번	
	elseif prop_id == 130965 then	
		-- 텔레포트 좌표
		warp( 221241 + math.random(0,10) , 160203+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 26-1 번	
	elseif prop_id == 130966 then	
		-- 텔레포트 좌표
		warp( 220867 + math.random(0,10) , 159759+ math.random(0,10), gv("layer") )
		
	-- 팔미르 제 2 유적 워프 프랍 33-1 번	
	elseif prop_id == 130973 then	
		-- 텔레포트 좌표
		warp( 220001 + math.random(0,10) , 154407+ math.random(0,10), gv("layer") )

	-- 잃어버린 비밀의 섬 워프 프랍 1-1 번(설원분지 -> 잃어버린 비밀의 섬 갈림길)
	-- [에픽 7 파트 1 잃어버린 섬 확장] '설원분지 출구' -> '드래곤의 둥지 입구'로 수정
	elseif prop_id == 50750 then	
		-- 텔레포트 좌표
		--warp( 84828 + math.random(0,10) , 117225+ math.random(0,10), gv("layer") )
		
		-- 드래곤의 둥지로 
		warp( 105093 + math.random(0,10) , 137583+ math.random(0,10), gv("layer") )

	-- 백룡의 쉼터 워프 프랍 2-1 번(해안가 -> 잃어버린 비밀의 섬)
	elseif prop_id == 50752 then	
		-- 텔레포트 좌표
		warp( 83286 + math.random(0,10) , 115722+ math.random(0,10), gv("layer") )

	-- 백룡의 쉼터 워프 프랍 3-1 번(잃어버린 비밀의 섬 -> 해안가)
	elseif prop_id == 50753 then	
		-- 텔레포트 좌표
		warp( 122759 + math.random(0,10) , 121596+ math.random(0,10), gv("layer") )

	-- 백룡의 쉼터 1-1 번(백색마력의 방 -> 설원분지)
	elseif prop_id == 121052 then	
		-- 텔레포트 좌표
		warp( 88817 + math.random(0,10) , 119671+ math.random(0,10), gv("layer") )
	
	-------------------------------------------------------------------------------
								--숨겨진 팔미르 유적 --
	-------------------------------------------------------------------------------

	-- 숨겨진 팔미르 제 2 유적 워프 프랍 1 번			
	elseif prop_id == 120201 then	
		-- 텔레포트 좌표
		warp( 194248 + math.random(0,10) , 39475+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 2 번	
	elseif prop_id == 120202 then	
		-- 텔레포트 좌표
		warp( 203311 + math.random(0,10) , 33102+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 3 번(4-1로)	
	elseif prop_id == 120203 then	
		-- 텔레포트 좌표
		warp( 203764 + math.random(0,10) , 36956+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 4 번(3-1로)	
	elseif prop_id == 120204 then	
		-- 텔레포트 좌표
		warp( 203595 + math.random(0,10) , 36319+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 5 번(8로)		
	elseif prop_id == 120205 then	
		-- 텔레포트 좌표
		warp( 198345 + math.random(0,10) , 39913+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 6 번(7-1로)	
	elseif prop_id == 120206 then	
		-- 텔레포트 좌표
		warp( 197500 + math.random(0,10) , 39866+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 7 번(6-1로)		
	elseif prop_id == 120207 then	
		-- 텔레포트 좌표
		warp( 198129 + math.random(0,10) , 40672+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 8 번(5로)	
	elseif prop_id == 120208 then	
		-- 텔레포트 좌표
		warp( 204551 + math.random(0,10) , 36884+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 9 번(x)	
	elseif prop_id == 120209 then	
		-- 텔레포트 좌표
		warp( 206586 + math.random(0,10) , 37253+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 10 번(x)	
	elseif prop_id == 120210 then	
		-- 텔레포트 좌표
		warp( 201082 + math.random(0,10) , 43561+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 11 번(12-1로)	
	elseif prop_id == 120211 then	
		-- 텔레포트 좌표
		warp( 201646 + math.random(0,10) , 37038+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 12 번(11-1로)	
	elseif prop_id == 120212 then	
		-- 텔레포트 좌표
		warp( 202047 + math.random(0,10) , 37505+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 13 번(14-1로)	
	elseif prop_id == 120213 then	
		-- 텔레포트 좌표
		warp( 197827 + math.random(0,10) , 38102+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 14 번(13-1로)	
	elseif prop_id == 120214 then	
		-- 텔레포트 좌표
		warp( 198409 + math.random(0,10) , 38745+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 15 번(16-1로)	
	elseif prop_id == 120215 then	
		-- 텔레포트 좌표
		warp( 208402 + math.random(0,10) , 37935+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 16 번(15-1로)	
	elseif prop_id == 120216 then	
		-- 텔레포트 좌표
		warp( 207232 + math.random(0,10) , 37941+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 17 번(20로)	
	elseif prop_id == 120217 then	
		-- 텔레포트 좌표
		warp( 200869 + math.random(0,10) , 43459+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 18 번(19-1로)	
	elseif prop_id == 120218 then	
		-- 텔레포트 좌표
		warp( 200732 + math.random(0,10) , 42791+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 19 번(18-1로)	
	elseif prop_id == 120219 then	
		-- 텔레포트 좌표
		warp( 200396 + math.random(0,10) , 43797+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 20 번(17로) 	
	elseif prop_id == 120220 then	
		-- 텔레포트 좌표
		warp( 207712 + math.random(0,10) , 38200+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 21 번(22-1로)		
	elseif prop_id == 120221 then	
		-- 텔레포트 좌표
		warp( 208041 + math.random(0,10) , 45458+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 22 번(23-1로)	
	elseif prop_id == 120222 then	
		-- 텔레포트 좌표
		warp( 208358 + math.random(0,10) , 44855+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 23 번(21-1로)	
	elseif prop_id == 120223 then	
		-- 텔레포트 좌표
		warp( 207265 + math.random(0,10) , 44540+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 24 번(25-1로)	

	elseif prop_id == 120224 then	
		-- 텔레포트 좌표
		warp( 205305 + math.random(0,10) , 47347+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 25 번(26-1로)		
	elseif prop_id == 120225 then	
		-- 텔레포트 좌표
		warp( 204730 + math.random(0,10) ,  46691+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 26 번(24-1로)	
	elseif prop_id == 120226 then	
		-- 텔레포트 좌표
		warp( 204780 + math.random(0,10) , 47705+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 27 번(29로)	
	elseif prop_id == 120227 then	
		-- 텔레포트 좌표
		warp( 203033 + math.random(0,10) , 42943+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 28 번(27로)	
	elseif prop_id == 120228 then	
		-- 텔레포트 좌표
		warp( 203371 + math.random(0,10) , 43424+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 29 번(30로)	
	elseif prop_id == 120229 then	
		-- 텔레포트 좌표
		warp( 203974 + math.random(0,10) , 42449+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 30 번(28로)	
	elseif prop_id == 120230 then	
		-- 텔레포트 좌표
		warp( 203422 + math.random(0,10) , 42484+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 31 번(32로)	
	elseif prop_id == 120231 then	
		-- 텔레포트 좌표
		warp( 197086 + math.random(0,10) , 46169+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 32 번(31로)	
	elseif prop_id == 120232 then	
		-- 텔레포트 좌표
		warp( 201015 + math.random(0,10) , 39371+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 33 번(33-1로)	
	elseif prop_id == 120233 then	
		-- 텔레포트 좌표
		warp( 202800 + math.random(0,10) , 41174+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 3-1 번(3로)	
	elseif prop_id == 120236 then	
		-- 텔레포트 좌표
		warp( 203845+ math.random(0,10) , 36344+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 4-1 번(4로)	
	elseif prop_id == 120237 then	
		-- 텔레포트 좌표
		warp( 203936 + math.random(0,10) , 36797+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 6-1 번(6로)	
	elseif prop_id == 120239 then	
		-- 텔레포트 좌표
		warp( 198151 + math.random(0,10) , 40464+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 7-1 번(7로)	
	elseif prop_id == 120240 then	
		-- 텔레포트 좌표
		warp( 197719 + math.random(0,10) , 39907+ math.random(0,10), gv("layer") )
	
	-- [QA]_버그 #20919
	-- [워프] 숨겨진 팔미르 유적 제 2 실 - 워프게이트 입구9 문제
	-- 팔미르 제 1 유적 워프 프랍 9-1 번	
	elseif prop_id == 120242 then	
		-- 텔레포트 좌표
		warp( 206439 + math.random(0,10) , 37087 + math.random(0,10), gv("layer") )	
	
		
	-- [QA]_버그 #19867 
	-- [워프] 숨겨진 팔미르 유적의 워프게이트 10관련
	-- 팔미르 제 1 유적 워프 프랍 10-1 번	
	elseif prop_id == 120243 then	
		-- 텔레포트 좌표
		warp( 194878 + math.random(0,10) , 46716 + math.random(0,10), gv("layer") )	

	
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 11-1 번(11로)	
	elseif prop_id == 120244 then	
		-- 텔레포트 좌표
		warp( 201871 + math.random(0,10) , 37498+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 12-1 번(12로)	
	elseif prop_id == 120245 then	
		-- 텔레포트 좌표
		warp( 201541 + math.random(0,10) , 37193+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 13-1 번(13로)	
	elseif prop_id == 120246 then	
		-- 텔레포트 좌표
		warp( 198436 + math.random(0,10) , 38550+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 14-1 번(14로)	
	elseif prop_id == 120247 then	
		-- 텔레포트 좌표
		warp( 198050 + math.random(0,10) , 38129+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 15-1 번(15로)	
	elseif prop_id == 120248 then	
		-- 텔레포트 좌표
		warp( 207457 + math.random(0,10) , 37948+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 16-1 번(16로)	
	elseif prop_id == 120249 then	
		-- 텔레포트 좌표
		warp( 208166 + math.random(0,10) , 37970+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 18-1 번(18로)	
	elseif prop_id == 120258 then	
		-- 텔레포트 좌표
		warp( 200446 + math.random(0,10) , 43608+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 2 유적 워프 프랍 19-1 번(19로)	
	elseif prop_id == 120259 then	
		-- 텔레포트 좌표
		warp( 200703 + math.random(0,10) , 42984+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 1 유적 워프 프랍 20-1 번 xxxxx	
	elseif prop_id == 120260 then	
		-- 텔레포트 좌표
		warp( 217010 + math.random(0,10) , 156336+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 1 유적 워프 프랍 21-1 번(21로)
	elseif prop_id == 120261 then	
		-- 텔레포트 좌표
		warp( 207498 + math.random(0,10) , 44595+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 1 유적 워프 프랍 22-1 번(22로)	
	elseif prop_id == 120262 then	
		-- 텔레포트 좌표
		warp( 207910 + math.random(0,10) , 45102+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 1 유적 워프 프랍 23-1 번(23로)	
	elseif prop_id == 120263 then	
		-- 텔레포트 좌표
		warp( 208177 + math.random(0,10) , 44755+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 1 유적 워프 프랍 24-1 번(24로)	
	elseif prop_id == 120264 then	
		-- 텔레포트 좌표
		warp( 204864 + math.random(0,10) , 47496+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 1 유적 워프 프랍 25-1 번(25로)	
	elseif prop_id == 120265 then	
		-- 텔레포트 좌표
		warp( 205107 + math.random(0,10) , 47309+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 1 유적 워프 프랍 26-1 번(26로)	
	elseif prop_id == 120266 then	
		-- 텔레포트 좌표
		warp( 204732 + math.random(0,10) , 46881+ math.random(0,10), gv("layer") )
		
	-- 숨겨진 팔미르 제 1 유적 워프 프랍 33-1 번(33로)	
	elseif prop_id == 120273 then	
		-- 텔레포트 좌표
		warp( 203883 + math.random(0,10) , 41510+ math.random(0,10), gv("layer") )

	-- 숨겨진 팔미르 제 1 유적 입구 워프 프랍 
	elseif prop_id == 120291 then	
		-- 텔레포트 좌표
		warp( 195147 + math.random(0,10) , 33220+ math.random(0,10), gv("layer") )

	-- 숨겨진 수정계곡 입구 워프 프랍 
	elseif prop_id == 70191 then	
		-- 텔레포트 좌표
		warp( 121934 + math.random(0,10) , 24410+ math.random(0,10), gv("layer") )

	-- 숨겨진 엘카시아 입구 워프 프랍 
	elseif prop_id == 110191 then	
		-- 텔레포트 좌표
		warp( 189968 + math.random(0,10) , 23636+ math.random(0,10), gv("layer") )

	-- 숨겨진 백룡의 쉼터 입구 워프 프랍 
	elseif prop_id == 100191 then	
		-- 텔레포트 좌표
		warp( 169145 + math.random(0,10) , 22619+ math.random(0,10), gv("layer") )

	-- 숨겨진 흑룡의 그늘 입구 워프 프랍 
	elseif prop_id == 90191 then	
		-- 텔레포트 좌표
		warp( 153016 + math.random(0,10) , 22621+ math.random(0,10), gv("layer") )

	-- 숨겨진 사룡의 심장 입구 워프 프랍 
	elseif prop_id == 80191 then	
		-- 텔레포트 좌표
		warp( 136890 + math.random(0,10) , 22626+ math.random(0,10), gv("layer") )

	-------------------------------------------------------------------------------
								       --큐브릭 던전 --
	-------------------------------------------------------------------------------

	-- 큐브릭 던전 입구 프랍, 던전 입장 
	elseif prop_id == 60101 then
		
		local level = get_value( 'level' )

		if party_id == 0 or level < 150 then
			--레벨 150이상, 파티를 결성해야 입장 가능 (1인이상, 사실상 의미 인원은 의미 없음)
			cprint( "@9831" )
			return		
				
		else 

			-- 같은 파티원만 입장할 수 있습니다.
			cprint( "@9830" )

			--set_flag( 'green_ticket', 0)
			--set_flag( 'cyan_ticket', 0 )
			
			-- 던전 입장
			dlg_special( 'confirm_window', 'warp_to_instance_dungeon(30000)', '@9825\v#@dungeon_name@#\v@80030000' )
		end



	-- 2번방 입장
	elseif prop_id == 60103 then

		-- 입구1
		warp(97636 , 29721, currentposition )

	
	-- 3번방 입장
	elseif prop_id == 60104 then
		-- 입구2
		warp(98246 , 30030, currentposition )

	-- 4번방 입장
	elseif prop_id == 60105 then
		-- 입구3
		warp(98259 , 30744, currentposition )

	-- 5번방 입장
	elseif prop_id == 60106 then
		-- 입구4
		warp(98246 , 31311, currentposition )

	-- 6번방 입장(중간보스1)
	elseif prop_id == 60108 then
		-- 입구6
		warp(97806 , 31540, currentposition )
	
	-- 7번방 입장
	elseif prop_id == 60109 then
		-- 입구7
		warp(97640 , 30964, currentposition )

	-- 8번방 입장
	elseif prop_id == 60107 then
		-- 입구5
		warp(98672 , 31728, currentposition )

	-- 9번방 입장
	elseif prop_id == 60110 then
		-- 입구8
		warp_to_cubric_branch_room( 1 )

	-- 10번방 입장(최종보스)
	elseif prop_id == 60111 then

		local layer = gv('layer')
		local sub_boss1_alive = get_alive_instance_respawn_group_monster_count( 30000, layer, 1 )
		local sub_boss2_alive = get_alive_instance_respawn_group_monster_count( 30000, layer, 2 )

		if sub_boss1_alive == 0 and sub_boss2_alive == 0 then

			--모두 제거했는데 입장할래? 다이얼로그                                    
			dlg_special( 'confirm_window', 'warp_to_cubric_boss_room()', '@9828' )

		else 
			--모두 제거해야만 들어갈 수 있다는 다이얼로그
			dlg_general( '@9827' )
			return
		end

	-- 11번방 입장
	elseif prop_id == 60112 then
		-- 입구10
		warp(99866 , 30628, currentposition )
		
	-- 12번방 입장
	elseif prop_id == 60113 then
		-- 입구11
		warp(99969 , 30079, currentposition )

	-- 13번방 입장
	elseif prop_id == 60114 then
		-- 입구12
		warp(100455 , 29927, currentposition )

	-- 14번방 입장
	elseif prop_id == 60115 then
		-- 입구13
		warp(100566 , 30354, currentposition )

	-- 15번방 입장
	elseif prop_id == 60116 then
		-- 입구14
		warp(100945 , 30479, currentposition )

	-- 16번방 입장
	elseif prop_id == 60117 then
		-- 입구15
		warp(101069 , 30053, currentposition )

	-- 13번방 입장
	elseif prop_id == 60118 then
		-- 입구16
		warp(100682 , 29926, currentposition )

	-- 17번방 입장
	elseif prop_id == 60119 then
		-- 입구17
		warp(101517 , 30243, currentposition )

	-- 18번방 입장(중보스입장2)
	elseif prop_id == 60120 then
		-- 입구18
		warp(101511 , 31012, currentposition )

	-- 15번방 입장
	elseif prop_id == 60153 then
		-- 입구18-1
		warp(101067 , 30597, currentposition )

	-- 15번방 입장
	elseif prop_id == 60152 then
		-- 입구17-1
		warp(101195 , 30477, currentposition )

	-- 15번방 입장
	elseif prop_id == 60150 then
		-- 입구15-1
		warp(101069 , 30363, currentposition )

	-- 16번방 입장
	elseif prop_id == 60151 then
		-- 입구16-1
		warp(100950 , 29928, currentposition )

	-- 14번방 입장
	elseif prop_id == 60149 then
		-- 입구14-1
		warp(100687 , 30476, currentposition )

	-- 13번방 입장
	elseif prop_id == 60148 then
		-- 입구13-1
		warp(100567 , 30053, currentposition )

	-- 12번방 입장
	elseif prop_id == 60147 then
		-- 입구12-1
		warp(100144 , 29929, currentposition )

	-- 11번방 입장
	elseif prop_id == 60146 then
		-- 입구11-1
		warp(99967 , 30507, currentposition )

	-- 9번방 입장
	elseif prop_id == 60145 then
		-- 입구10-1
		warp_to_cubric_branch_room( 2 )

	-- 9번방 입장
	elseif prop_id == 60144 then
	
	
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress3606 = get_quest_progress(3606) -- <(version:7.4)>[큐브릭]탈출 시도 #2
	
		if quest_progress3606 == 1 then
		
			set_quest_status( 3606, 1, 1 )
			cprint( "@90605055" ) -- <(version:7.4)><#6DD66D>바닥이 진동합니다.
			
		else
			-- 입구9-1
			warp(99018 , 30995, currentposition )
		end
		
	-- 4번방 입장
	elseif prop_id == 60143 then
		-- 입구8-1
		warp(98386 , 30851, currentposition )

	-- 4번방 입장
	elseif prop_id == 60139 then
		-- 입구4-1
		warp(98259 , 30978, currentposition )

	-- 5번방 입장
	elseif prop_id == 60140 then
		-- 입구5-1
		warp(98388 , 31423, currentposition )

	-- 5번방 입장(중보스 방에서 입장)
	elseif prop_id == 60141 then
		-- 입구6-1
		warp(98147 , 31425, currentposition )

	-- 6번방 입장(중보스 방으로 입장)
	elseif prop_id == 60142 then
		-- 입구7-1
		warp(97623 , 31352, currentposition )

	-- 3번방 입장
	elseif prop_id == 60138 then
		-- 입구3-1
		warp(98248 , 30374, currentposition )

	-- 2번방 입장
	elseif prop_id == 60137 then
		-- 입구2-1
		warp(97880 , 29729, currentposition )

	-- 1번방 입장
	elseif prop_id == 60136 then
		-- 입구1-1
		warp(97307 , 29997, currentposition )

	-- 중간보스(아카샤그린) 제거후 나타나는 포탈
	elseif prop_id == 60155 then
		
		-- 9번방으로 입장
		--warp(98858 , 30827, currentposition )
		--제거했는데 9번방으로 입장할래? 다이얼로그			
		dlg_special( 'confirm_window', 'warp_to_cubric_branch_room( 1 )', '@9829' )

	-- 중간보스(아카샤사이안) 제거후 나타나는 포탈
	elseif prop_id == 60156 then
		
		-- 9번방으로 입장
		--warp(99180 , 30825, currentposition )
		dlg_special( 'confirm_window', 'warp_to_cubric_branch_room( 2 )', '@9829' )

--==========================================에픽 8.2 신규 인스턴스 던전 4종 숨겨진 방 입구 프랍

		
	elseif prop_id == 60170 then -- 메마른 달빛 지하기지 숨은 밤
		
		warp(38499 , 22933, currentposition )
		
	elseif prop_id == 60171 then -- 발모어 탄광 지하기지 숨은 방
		
		warp(39682 , 9575, currentposition )
	
	elseif prop_id == 60172 then -- 수정계곡 지하기지 숨은 방
		
		warp(62154 , 31194, currentposition )
	
	elseif prop_id == 60173 then  -- 팔미르 지하기지 숨은 방
		
		warp(54047 , 3774, currentposition )
	
--==========================================에픽 9.1 붉은 거미 서커스단 포털

	
	elseif prop_id == 60186 then -- 1번방 입장
		
		warp(38918 , 137152, currentposition )
		
	elseif prop_id == 60187 then -- 1번방 퇴장
		
		warp(38980 , 137258, currentposition )	
		
	
	elseif prop_id == 60188 then -- 2번방 입장
		
		warp(40292 , 137131, currentposition )
		
	elseif prop_id == 60189 then -- 2번방 퇴장
		
		warp(40124 , 137058, currentposition )	
		
		
	elseif prop_id == 60190 then -- 보스방 입장
		
		warp(39555 , 136445, currentposition )
		
--	elseif prop_id == 60191 then -- 보스방 퇴장
		
--		warp(38561 , 118114, currentposition )	

	elseif prop_id == 60192 then -- 붉은거미 서커스

		local level = get_value( 'level' )

		if level >= 160 then

			dlg_special( 'confirm_window', 'warp(35692 , 120963 )', '@9852' )  -- 붉은거미 서커스 지역으로 입장하시겠습니까?
		
		elseif level < 160 then

			cprint( "@9254" )		-- 160레벨 이하는 입장할 수 없습니다.

		end
	
	elseif prop_id == 60193 then -- 붉은거미 서커스 퇴장
		
		warp(162950 , 116349, currentposition )

	
	-------------------------------------------------------------------------------
							-- 패러렐 월드<1> (version:9.2) ★--
	-------------------------------------------------------------------------------
	
		
	elseif prop_id == 60194 then -- 패러렐 월드 가디언 처치 시 다음 구역으로 이동
	
		warp( 151786 + math.random(0,10), 10288 + math.random(0,10), currentposition )
	
	elseif prop_id == 60195 then
	
		warp( 146956 + math.random(0,10), 11215 + math.random(0,10), currentposition )
		
	elseif prop_id == 60196 then
	
		warp( 147227 + math.random(0,10), 15039 + math.random(0,10), currentposition )
		
	elseif prop_id == 60197 then
	
		warp( 151846 + math.random(0,10), 14286 + math.random(0,10), currentposition )
		
	elseif prop_id == 60198 then
	
		warp( 149640 + math.random(0,10), 12564 + math.random(0,10), currentposition )
		
	elseif prop_id == 60199 then
	
		dlg_special( 'confirm_window', 'warp_parallelworld_to_city( x, y )', '@9906' )
		
	end
end

function warp_parallelworld_to_city( x, y )

	local hx = get_flag( 'hx' )
	local hy = get_flag( 'hy' )
	
	warp( hx + math.random(0,20), hy + math.random(0,20) )

		del_flag( "hx" )
		del_flag( "hy" )
end

--[[퀘스트 상태 체크 	get_quest_progress(ID)  
	반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress4009 = get_quest_progress(4009) -- [일일]틈새조사 : 타임어택]]--

function quest_start_4009()

	local quest_progress4009 = get_quest_progress(4009)
	
	local x = gv( "x" )
	local y = gv( "y" )

	if quest_progress4009 == 0 then
		parallelworld_warp( 70000, x, y )
		set_flag( "quest_progress4009", 1 )
	end
end

function quest_drop_4009()

	local x = gv( "x" )
	local y = gv( "y" )

		if 178000 < x and x < 185000 and 9400 < y and y < 15200 then
			warp_parallelworld_to_city( x, y )
		end
end

function quest_end_4009()

end

function on_create_parallelworld_instance( dungeon_id, layer )

	if dungeon_id == 60000 then
		add_instance_dungeon_monster( 60001, 60000, layer )
		add_instance_dungeon_monster( 60002, 60000, layer )
		add_instance_dungeon_monster( 60003, 60000, layer )
		add_instance_dungeon_monster( 60004, 60000, layer )
		add_instance_dungeon_monster( 60005, 60000, layer )
		
	elseif dungeon_id == 70000 then
		force_start_quest( 4009, 90610008 ) -- <(version:9.2)>패러렐 월드 퀘스트용 티켓 사용 시 - [일일]틈새조사 : 타임어택
		broadcast_notice( 1, "@9915", 70000, layer )

		add_instance_dungeon_monster( 70001, 70000, layer )
		add_instance_dungeon_monster( 70002, 70000, layer )
		add_instance_dungeon_monster( 70003, 70000, layer )
		add_instance_dungeon_monster( 70004, 70000, layer )
		add_instance_dungeon_monster( 70005, 70000, layer )
	end
end

function parallelworld_check_respawn_group_clear_1( respawn_group, x, y, layer )
	
	local left_monster_count = get_alive_instance_respawn_group_monster_count( 60000, layer, respawn_group ) -- 패러렐 월드<1>
	
-- 카운트 처리
		if left_monster_count == 0 then
			if respawn_group == 60001 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 1 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60006, 60000, layer )
				add_instance_dungeon_monster( 60007, 60000, layer )
				add_instance_dungeon_monster( 60008, 60000, layer )
				add_instance_dungeon_monster( 60009, 60000, layer )
				add_instance_dungeon_monster( 60010, 60000, layer )
				
			elseif respawn_group == 60006 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 2 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60011, 60000, layer )
				add_instance_dungeon_monster( 60012, 60000, layer )
				add_instance_dungeon_monster( 60013, 60000, layer )
				add_instance_dungeon_monster( 60014, 60000, layer )
				add_instance_dungeon_monster( 60015, 60000, layer )
				
			elseif respawn_group == 60011 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 3 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60016, 60000, layer )
				add_instance_dungeon_monster( 60017, 60000, layer )
				add_instance_dungeon_monster( 60018, 60000, layer )
				add_instance_dungeon_monster( 60019, 60000, layer )
				add_instance_dungeon_monster( 60020, 60000, layer )
				
			elseif respawn_group == 60016 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 4 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60021, 60000, layer )
				add_instance_dungeon_monster( 60022, 60000, layer )
				add_instance_dungeon_monster( 60023, 60000, layer )
				add_instance_dungeon_monster( 60024, 60000, layer )
				add_instance_dungeon_monster( 60025, 60000, layer )
				
			elseif respawn_group == 60021 then
				local text = sconv( "@9910", "#@boss_no@#", tostring( 5 ) )
				broadcast_notice( 1, text, 60000, layer )
				
				add_instance_dungeon_monster( 60026, 60000, layer )
				add_instance_dungeon_monster( 60027, 60000, layer )
				add_instance_dungeon_monster( 60028, 60000, layer )
				add_instance_dungeon_monster( 60029, 60000, layer )
				add_instance_dungeon_monster( 60030, 60000, layer )
				
			elseif respawn_group == 60026 then
				broadcast_notice( 1, "@9913", 60000, layer )
				insert_item( 2016029, 1 )
			end
		end
end

function parallelworld_check_respawn_group_clear_2( respawn_group, x, y, layer )
	
	--local quest_progress4009 = get_quest_progress(4009)
	local left_monster_count = get_alive_instance_respawn_group_monster_count( 70000, layer, respawn_group ) -- 패러렐 월드<2>
	
-- 카운트 처리
		if left_monster_count == 0 then
			if respawn_group == 70001 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 1 ) )
				broadcast_notice( 1, text, 70000, layer )
				set_quest_status( 4009, 1, 1 )
				
				add_instance_dungeon_monster( 70006, 70000, layer )
				add_instance_dungeon_monster( 70007, 70000, layer )
				add_instance_dungeon_monster( 70008, 70000, layer )
				add_instance_dungeon_monster( 70009, 70000, layer )
				add_instance_dungeon_monster( 70010, 70000, layer )
				
			elseif respawn_group == 70006 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 2 ) )
				broadcast_notice( 1, text, 70000, layer )
				set_quest_status( 4009, 2, 1 )
				
				add_instance_dungeon_monster( 70011, 70000, layer )
				add_instance_dungeon_monster( 70012, 70000, layer )
				add_instance_dungeon_monster( 70013, 70000, layer )
				add_instance_dungeon_monster( 70014, 70000, layer )
				add_instance_dungeon_monster( 70015, 70000, layer )
				
			elseif respawn_group == 70011 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 3 ) )
				broadcast_notice( 1, text, 70000, layer )
				set_quest_status( 4009, 3, 1 )
				
				add_instance_dungeon_monster( 70016, 70000, layer )
				add_instance_dungeon_monster( 70017, 70000, layer )
				add_instance_dungeon_monster( 70018, 70000, layer )
				add_instance_dungeon_monster( 70019, 70000, layer )
				add_instance_dungeon_monster( 70020, 70000, layer )
				
			elseif respawn_group == 70016 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 4 ) )
				broadcast_notice( 1, text, 70000, layer )
				set_quest_status( 4009, 4, 1 )
				
				add_instance_dungeon_monster( 70021, 70000, layer )
				add_instance_dungeon_monster( 70022, 70000, layer )
				add_instance_dungeon_monster( 70023, 70000, layer )
				add_instance_dungeon_monster( 70024, 70000, layer )
				add_instance_dungeon_monster( 70025, 70000, layer )
				
			elseif respawn_group == 70021 then
				local text = sconv( "@9905", "#@boss_no@#", tostring( 5 ) )
				broadcast_notice( 1, text, 70000, layer )
				set_quest_status( 4009, 5, 1 )
				
				add_instance_dungeon_monster( 70026, 70000, layer )
				add_instance_dungeon_monster( 70027, 70000, layer )
				add_instance_dungeon_monster( 70028, 70000, layer )
				add_instance_dungeon_monster( 70029, 70000, layer )
				add_instance_dungeon_monster( 70030, 70000, layer )
				
			elseif respawn_group == 70026 then
				broadcast_notice( 1, "@9914", 70000, layer )
				set_quest_status( 4009, 6, 1 )
			end
		end
end

function parallelworld_exit()

	local quest_progress4009 = get_quest_progress(4009) -- [일일]틈새조사 : 타임어택
	
	if quest_progress4009 == 1 then
		dlg_special( 'confirm_window', 'warp_parallelworld_to_city( x, y )', '@9911' )
	else
		warp_gate( 60199 )
	end

end

function on_leave_vulcanus( layer )

	
end

--=================================================================================

function enter_to_secret_dungeon( prop_id )

	if prop_id == 120291 then

		-- 숨겨진 팔미르 유적 ID 70120201
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(120201)', '@728\v#@dungeon_name@#\v@70120201' )
	
	elseif prop_id == 100191 then

		-- 숨겨진 백룡의 쉼터 ID 70100101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(100101)', '@728\v#@dungeon_name@#\v@70100101' )

	elseif prop_id == 90191 then

		-- 숨겨진 흑룡의 그늘 ID 70090101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(90101)', '@728\v#@dungeon_name@#\v@70090101' )

	elseif prop_id == 80191 then

		-- 숨겨진 사룡의 심장 ID 70080101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(80101)', '@728\v#@dungeon_name@#\v@70080101' )

	elseif prop_id == 110191 then
 
		-- 숨겨진 엘카시아 ID 70110101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(110101)', '@728\v#@dungeon_name@#\v@70110101' )

	elseif prop_id == 70191 then

		-- 숨겨진 수정계곡 ID 70070101
		dlg_special( 'confirm_window', 'warp_to_secret_dungeon(70101)', '@728\v#@dungeon_name@#\v@70070101' )

	else

	end

end

function enter_vulcanus()

	local level = get_value( 'level' )
	local count = find_item( 1000401 )
	local party_id = get_value('party_id')
	
	-- 퀘스트 상태 체크 	get_quest_progress(ID)  
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	local quest_progress_3367 = get_quest_progress(3367)
	
	if quest_progress_3367 == 1 then
		set_quest_status( 3367, 1, 1 )
		cprint( "@90604746" ) -- <(version:8.1)><#6DD66D>새로운 게이트를 발견하였습니다.
		return
	end

	if level >= 30 then

		if count == 0 then

			--열쇠가 없어 입장할 수 없습니다
			cprint( "@9811" )

		elseif count >= 20 then
		
			if party_id == 0 then
				-- 던전 보상이 캐릭터의 한도를 초과할 경우 게임 진행이 원활하지 않을 수 있습니다. 
				cprint( "@9812" )

				dlg_special('confirm_window', 'partycheck()', '@9805\v#@dungeon_name@#\v@80020000' )

			else 
				--파티 상태에서는 입장 불가함
				cprint( "@9247" )
				return
			end

		elseif count >= 1 and count < 20 then
			--열쇠가 충분하지 않아 입장할 수 없습니다
			cprint( "@9810" )
			return
		else
			cprint("에러")
		end

	else

	--30레벨 이상부터 입장 가능합니다.
	cprint( "@9251" )

	end

end

function partycheck()

	local party_id = get_value('party_id')
	
	if party_id == 0 then

		warp_to_instance_dungeon(20000)
		
	else 
		--파티 상태에서는 입장 불가함
		cprint( "@9247" )
		return
	end	
end
	

-- 인스턴스 던전 : 불카누스 프랍 호출 시 다이얼로그 생성
function enter_other_indun( dungeon_id, current_floor, next_floor, current_item_count, next_item_count )
	if dungeon_id == 20000 then

		local floor = current_floor
		local flag = 'Vul' .. tostring( current_floor )
		local floor_flag = get_instance_dungeon_flag( dungeon_id, gv( 'layer' ), flag )
		
			if floor_flag == 15 then
				floor = -1
			end

		-- 던전 보상이 캐릭터의 한도를 초과할 경우 게임 진행이 원활하지 않을 수 있습니다. 
		cprint( "@9812" )
		
		enter_other_instance_dungeon( dungeon_id, floor, next_floor, current_item_count, next_item_count )

	end
end

function warp_indun( instance_dungeon, floor )

	if instance_dungeon == 20000 then

		local flag = 'Vul' .. tostring( floor )
		local floor_flag  = get_instance_dungeon_flag( instance_dungeon, gv( 'layer' ), flag )
		local gate_num = 1
		local room_list = {}
		local room_count = 0

		if floor_flag == 15 then
			return
		end
		
		if floor == 4 then

			warp_floor( floor, gate_num )

		else
 
			-- 이전의 while문을 통하여 난수를 반복생성하여 서버다운 문제가 발생하였던 문제를 수정
			-- 현재 방의 입장 상태를 확인하여 들어간 방과 들어가지 않은 방을 구분하여 들어가지 않은 방의 테이블을 생성
			-- 후에 들어가지 않은 방의 테이블의 수 만큼 난수를 생성하여 생성된 난수로 방을 입장 시키는 방식으로 반복문을 제거한 방식으로 수정
			-- 각 방에 들어가지 않았을 때 테이블을 늘려주고 테이블에 해당 방 번호를 넣어줌으로써 해당 루프를 한번 거치게 되면 
			-- 들어가지 않은 방의 수 만큼 테이블이 생성되고 테이블에 방 번호가 순차적으로 작성됩니다.
			
			
			if floor_flag ~= nil and floor_flag ~= "" then
				-- 1번 방에 안 들어갔을 때
				if floor_flag == 0 or floor_flag == 2 or floor_flag == 4 or floor_flag == 8 or floor_flag == 6 or floor_flag == 10 or floor_flag == 12 or floor_flag == 14 then
					room_list[ room_count + 1 ] = 1
					room_count = room_count + 1
				end
					
				-- 2번 방에 안 들어갔을 때
				if floor_flag == 0 or floor_flag == 1 or floor_flag == 4 or floor_flag == 8 or floor_flag == 5 or floor_flag == 9 or floor_flag == 12 or floor_flag == 13 then
					room_list[ room_count + 1 ] = 2
					room_count = room_count + 1
				end
				
				-- 3번 방에 안 들어갔을 때
				if floor_flag == 0 or floor_flag == 1 or floor_flag == 2 or floor_flag == 8 or floor_flag == 3 or floor_flag == 9 or floor_flag == 10 or floor_flag == 11 then
					room_list[ room_count + 1 ] = 3
					room_count = room_count + 1
				end
				
				-- 4번 방에 안 들어갔을 때
				if floor_flag == 0 or floor_flag == 1 or floor_flag == 2 or floor_flag == 4 or floor_flag == 3 or floor_flag == 5 or floor_flag == 6 or floor_flag == 7 then
					room_list[ room_count + 1 ] = 4
					room_count = room_count + 1
				end
			end

			gate_num = math.random( 1, room_count ) 
						
			warp_floor( floor, room_list[ gate_num ] )
					
		end
		
	end	
	
end


function warp_floor( floor, gate_num )

	local x, y

	local currentposition = get_value('layer')

	if floor == 1 then

		-- 1층 열쇠 조건 체크,

		local count = find_item( 1000401 )	

		if count >= 20 then
			-- 만족하면 열쇠 수거하고 
			delete_item( get_item_handle( 1000401 ), 20 )

			-- gate_num에 맞게 워프해주자	
			if gate_num == 1 then
				x = 197393
				y = 28580
			elseif gate_num == 2 then
				x = 198814
				y = 28457
			elseif gate_num == 3 then
				x = 200543
				y = 28458
			elseif gate_num == 4 then
				x = 202065
				y = 28458
			end
		else
			--열쇠가 충분하지 않아 입장할 수 없습니다
			cprint( "@9810" )
			return
		end

	elseif floor == 2 then

		-- 2층 열쇠 조건 체크,
		
		local count = find_item( 1000402 )

		if count >= 10 then
			-- 만족하면 열쇠 수거하고 
			delete_item( get_item_handle( 1000402 ), 10 )

			if gate_num == 1 then
				x = 197047
				y = 25954
			elseif gate_num == 2 then
				x = 198661
				y = 25929
			elseif gate_num == 3 then
				x = 200362
				y = 25962
			elseif gate_num == 4 then
				x = 202169
				y = 26121
			end

		else
			--열쇠가 충분하지 않아 입장할 수 없습니다
			cprint( "@9810" )
			return
		end
		
	elseif floor == 3 then

		-- 3층 열쇠 조건 체크,
	
		local count = find_item( 1000403 )

		if count >= 5 then
			-- 만족하면 열쇠 수거하고 
			delete_item( get_item_handle( 1000403 ), 5 )

			if gate_num == 1 then
				x = 197303
				y = 23750
			elseif gate_num == 2 then
				x = 198586
				y = 23765
			elseif gate_num == 3 then
				x = 200390
				y = 23677
			elseif gate_num == 4 then
				x = 202114
				y = 23701
			end

		else
			--열쇠가 충분하지 않아 입장할 수 없습니다
			cprint( "@9810" )
			return
		end

		
	elseif floor == 4 then
		
		-- 4층 열쇠 조건 체크,

		local count = find_item( 1000404 )
			
		if count >= 1 then
			-- 만족하면 열쇠 수거하고 
			delete_item( get_item_handle( 1000404 ), 1 )

			x = 206474
			y = 27625

		else
			--열쇠가 충분하지 않아 입장할 수 없습니다
			cprint( "@9810" )
			return
		end
	end

	warp( x, y, currentposition )
	
	-- 미션 UI 관련 정보 세팅(새로 들어간 방에 대한 미션)
	-- 미션 제목은 항상 '모든 몬스터를 섬멸하라'
	send_mission_title( '@1224' )
	-- 미션 보상은 on_create_vulcanus_instance 에서 세팅된 difficulty 값에 따라 달라지므로 함수에 의해 얻어 옴
	local exp, jp, gold = vulcanus_clear_reward( currentposition, floor )
	-- string.char( 11 ) == '\v'
	local reward_string = '@1225'
	reward_string = reward_string .. '\v#@reward_exp@#\v' .. exp
	reward_string = reward_string .. '\v#@reward_jp@#\v' .. jp
	reward_string = reward_string .. '\v#@reward_gold@#\v' .. gold
	send_mission_reward( reward_string )
	-- 미션 목표도 항상 '모든 몬스터를 섬멸하라'에 최대 몬스터 수는 6(4층인 경우만 1이 됨)
	if floor == 4 then
		send_mission_objective( 0, 1, '@1226' )
		
		-- 4층 불카누스 방 입장 시 인던 내에 불카누스 멘트 방송
		--<(version:7.3)><#6DD66D>나는.. 불카누스다. 나의 의지를 깨운 너는....너는 누구.... 인가. 열쇠를 소지한 자.. 인가?
		cprint( '@90604914' )
		--<(version:7.3)><#6DD66D>그렇다면, 내가 숨겨 논 기록을...... 되찾은 자로군. 시간이 없으니...... 너의 지식을 보겠다.
		cprint( '@90604915' )
	else
		send_mission_objective( 0, 6, '@1224' )
	end
	-- 불카누스는 1인 던전이라 누가 들어가면 몬스터는 다 살아 있으므로 진행률 무조건 0
	send_mission_objective_progress( 0, 0 )
end

-- 인스턴스 던전 : 불카누스 던전 퇴장
-- 인스턴스 던전 : 큐브릭 던전 퇴장
-- 마을로 돌려 보내주는 워프게이트
function exit_instance_dungeon( prop_id )

	if prop_id == 126027 then
		-- 20000 인던 불카누스 ID
		dlg_special( 'confirm_window', 'exit_indun(100101)', '@9807\v#@dungeon_name@#\v@80020000' )

	elseif prop_id == 126023 then
		-- 20000 인던 불카누스 ID
		dlg_special( 'confirm_window', 'exit_indun(100101)', '@9807\v#@dungeon_name@#\v@80020000' )

	elseif prop_id == 60154 or prop_id == 60102 then

		-- 30000 인던 큐브릭 ID
		dlg_special( 'confirm_window', 'exit_indun( 30000 )', '@9832\v#@dungeon_name@#\v@80030000' )

	elseif prop_id == 60166 then -- 메마른 달빛 지하기지 출구
		
		dlg_special( 'confirm_window', 'exit_indun( 40000 )', '@9832\v#@dungeon_name@#\v@80040000' )
		--warp(133066 , 87015 )
		
	elseif prop_id == 60167 then -- 발모어 탄광 지하기지 출구
		
		dlg_special( 'confirm_window', 'exit_indun( 41001 )', '@9832\v#@dungeon_name@#\v@80041001' )
		--warp(155868 , 103644 )
	
	elseif prop_id == 60168 then -- 수정계곡 지하기지 출구
		
		dlg_special( 'confirm_window', 'exit_indun( 42001 )', '@9832\v#@dungeon_name@#\v@80042001' )
		--warp(103282 , 100401 )
	
	elseif prop_id == 60169 then  -- 팔미르 지하기지 출구
		
		dlg_special( 'confirm_window', 'exit_indun( 43001 )', '@9832\v#@dungeon_name@#\v@80043001' )
		--warp(132695 , 128105 )
		
	elseif prop_id == 60191 then  -- 서커스 출구
		
		dlg_special( 'confirm_window', 'exit_indun( 50000 )', '@9853\v#@dungeon_name@#\v@80050000' )
		--warp(132695 , 128105 )

	else


	end
	
end

function exit_indun( dungeon_id )

	local hx = get_flag( 'hx' )
	local hy = get_flag( 'hy' )
	
	warp( hx, hy, 0 )
end




-- 용도: 불카누스 인스턴스 던전이 생성될 때마다 호출되는 함수
-- 역할: 불카누스 던전의 진행상황 정보 초기값 세팅
function on_create_vulcanus_instance( layer )

	-- 불카누스 인스턴스 던전 ID: 20000
	set_instance_dungeon_flag( 20000, layer, 'Vul1', 0 )
	set_instance_dungeon_flag( 20000, layer, 'Vul2', 0 )
	set_instance_dungeon_flag( 20000, layer, 'Vul3', 0 )

	-- 입장 시 레벨에 따른 난이도 값 보관(각 방 완료 시 보상 차등화를 위해 필요함)
	set_instance_dungeon_flag( 20000, layer, 'difficulty', get_instance_dungeon_type_id( 20000, layer ) )
	set_instance_dungeon_flag( 20000, layer, 'date', get_os_time() )
	
end


-- 용도: 불카누스 인스턴스 던전에 유저가 참여할 때마다 호출되는 함수
-- 역할: 미션 UI 세팅용 정보를 방금 참여한 유저에게 송신해 줌
function on_join_vulcanus( layer )

	-- 불카누스 인스턴스 던전 ID: 20000

	-- 미션 제목은 항상 '모든 몬스터를 섬멸하라'
	send_mission_title( '@1224' )
	-- 미션 보상은 on_create_vulcanus_instance 에서 세팅된 difficulty 값에 따라 달라지므로 함수에 의해 얻어 옴
	local exp, jp, gold = vulcanus_clear_reward( layer, 1 )
	-- string.char( 11 ) == '\v'
	local reward_string = '@1225'
	reward_string = reward_string .. '\v#@reward_exp@#\v' .. exp
	reward_string = reward_string .. '\v#@reward_jp@#\v' .. jp
	reward_string = reward_string .. '\v#@reward_gold@#\v' .. gold
	send_mission_reward( reward_string )
	-- 미션 목표도 항상 '모든 몬스터를 섬멸하라'에 최대 몬스터 수는 6(4층인 경우만 1이 됨)
	send_mission_objective( 0, 6, '@1224' )
	-- 1층 1번 방의 남은 몬스터 수를 기준으로 진행률을 체크(불카누스는 1인 던전이라 누가 들어가면 몬스터는 다 살아 있으므로 진행률 무조건 0)
	send_mission_objective_progress( 0, 0 )

	do_each_player_in_instance_dungeon( 20000, layer, "open_title(1)" )
end


-- 용도: 불카누스 인스턴스 던전에서 유저가 이탈할 때마다 호출되는 함수
-- 역할: 미션 UI 관련 정보를 방금 이탈한 유저에게서 제거시켜 줌(UI 숨겨짐)
function on_leave_vulcanus( layer )

	-- 불카누스 인스턴스 던전 ID: 20000
	-- 미션 UI 제거
	send_mission_title( '' )
	
end


-- 용도: 몬스터 죽을 때마다 호출되는 함수
-- 역할: 죽은 몬스터의 리스폰 그룹에 남은 몬스터 수를 체크하여 모두 죽었을 경우 방 별 클리어 정보를 세팅하고 다른 곳으로 이동을 위한 프랍을 리젠시켜 줌
function vulcanus_check_respawn_group_clear( respawn_group, x, y, layer )

	-- 죽은 몬스터와 같은 그룹에 소속된 몬스터 중 살아있는 녀석이 남아 있다면 아무것도 안 함
	local left_monster_count = get_alive_instance_respawn_group_monster_count( 20000, layer, respawn_group )

	-- 해당 인던 내에 있는 모든 유저의 미션 UI의 진행률 갱신(4층이면 총 몬스터 수가 1이고 그 외에는 모두 6)
	-- 죽인 몬스터의 수를 표시해야 하므로 (전체 몬스터 수) - (남은 몬스터의 수) 를 통해 계산된 값을 방송함
	if respawn_group == 20013 then
		-- 불카누스 4층(보스방)
		broadcast_mission_objective_progress( 1, 0, 1 - left_monster_count, 20000, layer )
	else
		broadcast_mission_objective_progress( 1, 0, 6 - left_monster_count, 20000, layer )
	end

	if left_monster_count ~= 0 then
		return
	end

	-- 리스폰 그룹별로 차별화되어야 하는 부분 처리(리스폰 그룹에 따라 적절한 위치 선정)
	local prop_id = 0
	local prop_z_offset = 10
	local exp = 0
	local jp = 0
	local gold = 0

	-- 불카누스 1층
	if		respawn_group == 20001 or respawn_group == 20002 or respawn_group == 20003 or respawn_group == 20004 then
		prop_id = 126024

		-- 각 방별 클리어 정보 갱신
		local floor_clear_flag = get_instance_dungeon_flag( 20000, layer, 'Vul1' )
		if		respawn_group == 20001 then
			floor_clear_flag = floor_clear_flag + 1
		elseif	respawn_group == 20002 then
			floor_clear_flag = floor_clear_flag + 2
		elseif	respawn_group == 20003 then
			floor_clear_flag = floor_clear_flag + 4
		elseif	respawn_group == 20004 then
			floor_clear_flag = floor_clear_flag + 8
		end
		set_instance_dungeon_flag( 20000, layer, 'Vul1', floor_clear_flag )
		exp, jp, gold = vulcanus_clear_reward( layer, 1 )

	-- 불카누스 2층
	elseif	respawn_group == 20005 or respawn_group == 20006 or respawn_group == 20007 or respawn_group == 20008 then
		prop_id = 126025

		-- 각 방별 클리어 정보 갱신
		local floor_clear_flag = get_instance_dungeon_flag( 20000, layer, 'Vul2' )
		if		respawn_group == 20005 then
			floor_clear_flag = floor_clear_flag + 1
		elseif	respawn_group == 20006 then
			floor_clear_flag = floor_clear_flag + 2
		elseif	respawn_group == 20007 then
			floor_clear_flag = floor_clear_flag + 4
		elseif	respawn_group == 20008 then
			floor_clear_flag = floor_clear_flag + 8
		end
		set_instance_dungeon_flag( 20000, layer, 'Vul2', floor_clear_flag )
		exp, jp, gold = vulcanus_clear_reward( layer, 2 )

	-- 불카누스 3층
	elseif	respawn_group == 20009 or respawn_group == 20010 or respawn_group == 20011 or respawn_group == 20012 then
		prop_id = 126026

		-- 각 방별 클리어 정보 갱신
		local floor_clear_flag = get_instance_dungeon_flag( 20000, layer, 'Vul3' )
		if		respawn_group == 20009 then
			floor_clear_flag = floor_clear_flag + 1
		elseif	respawn_group == 20010 then
			floor_clear_flag = floor_clear_flag + 2
		elseif	respawn_group == 20011 then
			floor_clear_flag = floor_clear_flag + 4
		elseif	respawn_group == 20012 then
			floor_clear_flag = floor_clear_flag + 8
		end
		set_instance_dungeon_flag( 20000, layer, 'Vul3', floor_clear_flag )
		exp, jp, gold = vulcanus_clear_reward( layer, 3 )

	-- 불카누스 4층(보스방)
	elseif	respawn_group == 20013 then
		prop_id = 126027
		-- 불카누스 방 퇴장 프랍만 모양이 다르고 z_offset도 달라야 함
		prop_z_offset = 35
		exp, jp, gold = vulcanus_clear_reward( layer, 4 )

		-- 불카누스 사냥 시 호칭 부여
		do_each_player_in_instance_dungeon( 20000, layer, "update_title_condition( 9002002, 1 )" )
		
		--[[
		-- 블랙 프라이 데이 이벤트 기간에만 몬스터 리젠
		local difficulty = get_instance_dungeon_flag( 20000, layer, 'difficulty' )
		local monster_code
		-- 난이도에 따라 몬스터 코드가 달라진다.
		
		if difficulty == 0 then 
			monster_code = 9400022
		elseif difficulty == 1 then
			monster_code = 9400122
		elseif difficulty == 2 then
			monster_code = 9400222
		elseif difficulty == 3 then
			monster_code = 9400322
		elseif difficulty == 4 then
			monster_code = 9400422
		elseif difficulty == 5 then
			monster_code = 9400522 
		elseif difficulty == 6 then
			monster_code = 9400622
		elseif difficulty == 7 then
			monster_code = 9400722
		elseif difficulty == 8 then
			monster_code = 9400822
		elseif difficulty == 9 then
			monster_code = 9400922
		elseif difficulty == 10 then
			monster_code = 9401022
		elseif difficulty == 11 then
			monster_code = 9401122
		elseif difficulty == 12 then
			monster_code = 9401222
		elseif difficulty == 13 then
			monster_code = 9401322
		end 
				
		-- 해당 위치에 이벤트 몬스터 리젠
		add_npc( 206048, 27430, monster_code, 1, 0, layer )		
		
		-- 블랙 프라이데이 이벤트 스크립트 종료
		
		]]
		
		
		
		
	else
		do_each_player_in_instance_dungeon( 20000, layer, "cprint( 'vulcanus_check_respawn_group_clear - No respawn group' )" )
		return
	end

	-- 안내 메시지 출력: 모든 몬스터를 쓰러뜨렸습니다. 불카누스 지하 입구를 더블 클릭하면 다음 던전으로 입장할 수 있는 메뉴가 나타납니다.
	-- 4층이면 다른 메시지 출력 안 함
	if respawn_group ~= 20013 then
		do_each_player_in_instance_dungeon( 20000, layer, "cprint('@9813');cprint('@9250')" )
	end

	-- 보상 지급 스크립트
	local reward_handler = 'add_exp_jp( ' .. exp .. ', ' .. jp .. ', false, false ); insert_gold( ' .. gold .. ', true )'
	do_each_player_in_instance_dungeon( 20000, layer, reward_handler )

	-- 미션 목표를 모두 달성했으므로 미션 UI 닫음(다른 방으로 이동하면 다시 새로 부여됨)
	broadcast_mission_title( 1, '', 20000, layer )

	-- 프랍 생성
	add_field_prop( prop_id, 0, x, y, layer, prop_z_offset )
end


-- 용도: 각 방 클리어 완료 보상 지급 시 보상량을 계산하기 위해 호출됨
-- 역할: 경험치/잡포인트/루피를 난이도에 따라 각각 지급해줄 만큼 얻어서 반환함
function vulcanus_clear_reward( layer, floor )

	-- 보상은 각 방의 번호는 무관하며 던전의 난이도(입장 시 유저 레벨에 의해 결정됨)와 클리어 층 수에 따라 결정됨
	local difficulty = get_instance_dungeon_flag( 20000, layer, 'difficulty' )

	local exp = 0
	local jp = 0
	local gold = 0

	-- difficulty 값 관련 레벨 제한은 InstanceDungeonTypeResource 테이블에 기입된 값이므로
	-- 해당 테이블에서 데이터가 변경되면 아래의 내용도 수정되어야 함
	-- 수식화하거나 자동화하면 좋겠지만 보상의 수식을 모르겠어서 조건문에 따라 아래와 같이 처리함
	-- 기획팀에서 쓴 수식을 알려면 퀘스트Data 엑셀 파일을 참조하면 되는데,
	-- 거기서 참조한 각종 데이터를 스크립트에서 참조하도록 만드는 것도 무리가 있으므로 일단 수작업 처리 ㄷㄷ;
	if		difficulty == 0 then
		-- 30 ~ 39 레벨
		if		floor == 1 then
			-- 1층
			exp		= 19956
			jp		= 3991
			gold	= 31200
		elseif	floor == 2 then
			-- 2층
			exp		= 24454
			jp		= 4890
			gold	= 33300
		elseif	floor == 3 then
			-- 3층
			exp		= 29555
			jp		= 5910
			gold	= 35400
		elseif	floor == 4 then
			-- 4층
			exp		= 58378
			jp		= 11675
			gold	= 72000
		end
	elseif	difficulty == 1 then
		-- 40 ~ 49 레벨
		if		floor == 1 then
			-- 1층
			exp		= 38548
			jp		= 4890
			gold	= 41600
		elseif	floor == 2 then
			-- 2층
			exp		= 44904
			jp		= 7709
			gold	= 43700
		elseif	floor == 3 then
			-- 3층
			exp		= 51743
			jp		= 10348
			gold	= 45800
		elseif	floor == 4 then
			-- 4층
			exp		= 132405
			jp		= 26481
			gold	= 92000
		end
	elseif	difficulty == 2 then
		-- 50 ~ 59 레벨
		if		floor == 1 then
			-- 1층
			exp		= 57408
			jp		= 11481
			gold	= 52000
		elseif	floor == 2 then
			-- 2층
			exp		= 64815
			jp		= 12002
			gold	= 54100
		elseif	floor == 3 then
			-- 3층
			exp		= 72956
			jp		= 13510
			gold	= 56200
		elseif	floor == 4 then
			-- 4층
			exp		= 214137
			jp		= 39655
			gold	= 112000
		end
	elseif	difficulty == 3 then
		-- 60 ~ 69 레벨
		if		floor == 1 then
			-- 1층
			exp		= 76854
			jp		= 14232
			gold	= 62400
		elseif	floor == 2 then
			-- 2층
			exp		= 85198
			jp		= 15777
			gold	= 64500
		elseif	floor == 3 then
			-- 3층
			exp		= 94077
			jp		= 17421
			gold	= 66600
		elseif	floor == 4 then
			-- 4층
			exp		= 269109
			jp		= 49834
			gold	= 132000
		end
	elseif	difficulty == 4 then
		-- 70 ~ 79 레벨
		if		floor == 1 then
			-- 1층
			exp		= 91139
			jp		= 16877
			gold	= 72800
		elseif	floor == 2 then
			-- 2층
			exp		= 99583
			jp		= 18441
			gold	= 74900
		elseif	floor == 3 then
			-- 3층
			exp		= 108495
			jp		= 20091
			gold	= 77000
		elseif	floor == 4 then
			-- 4층
			exp		= 314787
			jp		= 58293
			gold	= 152000
		end
	elseif	difficulty == 5 then
		-- 80 ~ 89 레벨
		if		floor == 1 then
			-- 1층
			exp		= 113358
			jp		= 20992
			gold	= 83200
		elseif	floor == 2 then
			-- 2층
			exp		= 122679
			jp		= 22718
			gold	= 85300
		elseif	floor == 3 then
			-- 3층
			exp		= 132349
			jp		= 24509
			gold	= 87400
		elseif	floor == 4 then
			-- 4층
			exp		= 427830
			jp		= 79227
			gold	= 172000
		end
	elseif	difficulty == 6 then
		-- 90 ~ 99 레벨
		if		floor == 1 then
			-- 1층
			exp		= 131787
			jp		= 24405
			gold	= 93600
		elseif	floor == 2 then
			-- 2층
			exp		= 141445
			jp		= 26193
			gold	= 95700
		elseif	floor == 3 then
			-- 3층
			exp		= 151556
			jp		= 28065
			gold	= 97800
		elseif	floor == 4 then
			-- 4층
			exp		= 561177
			jp		= 103921
			gold	= 192000
		end
	elseif	difficulty == 7 then
		-- 100 ~ 109 레벨
		if		floor == 1 then
			-- 1층
			exp		= 175584
			jp		= 32515
			gold	= 104000
		elseif	floor == 2 then
			-- 2층
			exp		= 187230
			jp		= 32280
			gold	= 106100
		elseif	floor == 3 then
			-- 3층
			exp		= 199491
			jp		= 34395
			gold	= 108200
		elseif	floor == 4 then
			-- 4층
			exp		= 700102
			jp		= 120707
			gold	= 212000
		end
	elseif	difficulty == 8 then
		-- 110 ~ 119 레벨
		if		floor == 1 then
			-- 1층
			exp		= 237120
			jp		= 40882
			gold	= 114400
		elseif	floor == 2 then
			-- 2층
			exp		= 251671
			jp		= 43391
			gold	= 116500
		elseif	floor == 3 then
			-- 3층
			exp		= 266785
			jp		= 45997
			gold	= 118600
		elseif	floor == 4 then
			-- 4층
			exp		= 988754
			jp		= 170474
			gold	= 232000
		end
	elseif	difficulty == 9 then
		-- 120 ~ 129 레벨
		if		floor == 1 then
			-- 1층
			exp		= 292792
			jp		= 50481
			gold	= 124800
		elseif	floor == 2 then
			-- 2층
			exp		= 306274
			jp		= 52805
			gold	= 126900
		elseif	floor == 3 then
			-- 3층
			exp		= 320012
			jp		= 55174
			gold	= 129000
		elseif	floor == 4 then
			-- 4층
			exp		= 1169043
			jp		= 201559
			gold	= 252000
		end
	elseif	difficulty == 10 then
		-- 130 ~ 139 레벨
		if		floor == 1 then
			-- 1층
			exp		= 377019
			jp		= 65003
			gold	= 135200
		elseif	floor == 2 then
			-- 2층
			exp		= 392723
			jp		= 67710
			gold	= 137300
		elseif	floor == 3 then
			-- 3층
			exp		= 408832
			jp		= 70488
			gold	= 139400
		elseif	floor == 4 then
			-- 4층
			exp		= 1785194
			jp		= 307792
			gold	= 272000
		end
	elseif	difficulty == 11 then
		-- 140 ~ 149 레벨
		if		floor == 1 then
			-- 1층
			exp		= 470027
			jp		= 81039
			gold	= 145600
		elseif	floor == 2 then
			-- 2층
			exp		= 527619
			jp		= 90968
			gold	= 147700
		elseif	floor == 3 then
			-- 3층
			exp		= 620635
			jp		= 107006
			gold	= 149800
		elseif	floor == 4 then
			-- 4층
			exp		= 3387588
			jp		= 584066
			gold	= 292000
		end
	elseif	difficulty == 12 then
		-- 150 ~ 159 레벨
		if		floor == 1 then
			-- 1층
			exp		= 769704
			jp		= 132707
			gold	= 156000
		elseif	floor == 2 then
			-- 2층
			exp		= 1541102
			jp		= 248564
			gold	= 158100
		elseif	floor == 3 then
			-- 3층
			exp		= 3173438
			jp		= 511844
			gold	= 160200
		elseif	floor == 4 then
			-- 4층
			exp		= 6718169
			jp		= 1083575
			gold	= 312000
		end
	elseif	difficulty == 13 then
		-- 160 ~ 최대 레벨
		if		floor == 1 then
			-- 1층
			exp		= 1219563
			jp		= 196703
			gold	= 166400
		elseif	floor == 2 then
			-- 2층
			exp		= 1800319
			jp		= 290374
			gold	= 168500
		elseif	floor == 3 then
			-- 3층
			exp		= 3922638
			jp		= 632683
			gold	= 170600
		elseif	floor == 4 then
			-- 4층
			exp		= 12356309
			jp		= 1992953
			gold	= 332000
		end
	end

	return exp, jp, gold
end

-- 용도: 큐브릭 인스턴스 던전이 생성될 때마다 호출되는 함수
-- 역할: 큐브릭 던전의 진행상황 정보 초기값 세팅
function on_create_cubric_instance( layer )

	-- 큐브릭 인스턴스 던전 ID: 30000
	set_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount', 0 )	-- 인던 진행 퀘스트 체크 대상인 3607 퀘스트를 진행 중인 유저 수 체크용
	set_instance_dungeon_flag( 30000, layer, 'AdditionalBossRespawned', 0 )	-- 인던 진행 퀘스트로 인해 추가 보스몹들의 리젠 여부 체크용(중첩 리젠 방지용)
	set_instance_dungeon_flag( 30000, layer, 'ExitPropRespawned', 0 )		-- 인던 최종 출구 프랍 리젠 여부 체크용(중첩 리젠 방지용)
	set_instance_dungeon_flag( 30000, layer, 'MissionRevealed', 0 )			-- 아크샤 두 마리 잡으라는 미션을 유저들에게 보여줬는지 체크용(0:활성화 전, 1:활성화 중, 2:미션 완료)

end


-- 용도: 큐브릭 인스턴스 던전에 유저가 참여할 때마다 호출되는 함수
-- 역할: 방금 참여한 유저가 체크 대상인 퀘스트를 보유하고 있었을 경우 던전 내부 체크용 변수 세팅
function on_join_cubric( layer )

	-- 방금 진입한 유저가 3607 퀘스트를 진행 중이라면 던전 상태 변수에 반영
	-- * 이전에 퀘 받아놓고 완료 조건 못 채우고 나갔던 유저면 반영하지만
	--   완료 조건이 달성되어 있는 유저라면 퀘스트를 진행하고 있는 상태라고 볼 수 없으므로 반영하지 않음
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	if get_quest_progress( 3607 ) == 1 then

		-- 큐브릭 인스턴스 던전 ID: 30000
		set_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount', get_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount' ) + 1 )

	end

	-- 미션 UI가 아직 활성화된 이후에 입장하는 거라면 미션 UI 표시
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) == 1 then

		-- 미션 제목
		send_mission_title( '@1234' )

		-- 0번 미션 목표(서쪽의 아크샤)
		send_mission_objective( 0, 1, '@1235' )
		-- 1번 미션 목표(동쪽의 아크샤)
		send_mission_objective( 1, 1, '@1236' )
		-- 2번 미션 목표(보스방 드가세요)
		send_mission_objective( 2, 1, '@1237' )

		-- 미션 보상 안내
		send_mission_reward( '@1238' )

		-- 미션 목표 진행 상태
		update_cubric_mission( false, layer )

	end

end


-- 용도: 큐브릭 인스턴스 던전에서 유저가 이탈할 때마다 호출되는 함수
-- 역할: 방금 이탈한 유저가 체크 대상인 퀘스트를 보유하고 있었을 경우 던전 내부 체크용 변수 세팅
function on_leave_cubric( layer )

	-- 방금 이탈한 유저가 3607 퀘스트를 진행 중이라면 던전 상태 변수에 반영
	-- * 퀘 받아놓고 완료 조건 못 채우고 나가는 유저면 반영하지만
	--   완료 조건이 달성되어 있는 유저라면 퀘스트를 진행 중인 상태라고 볼 수 없으므로 반영하지 않음
	--   (예. 퀘 받고 몹 죽여서 보상 받으면 되지만 보상만 안 받고 나간 경우)
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	if get_quest_progress( 3607 ) == 1 then

		-- 큐브릭 인스턴스 던전 ID: 30000
		set_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount', get_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount' ) - 1 )

	end

	-- 미션 UI가 활성화 된 이후였으면 미션 UI 닫아주기
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) == 1 then

		send_mission_title( '' )

	end

end


-- 용도: 몬스터 죽을 때마다 호출되는 함수
-- 역할: 죽은 몬스터의 리스폰 그룹에 남은 몬스터 수를 체크하여 모두 죽었을 프랍을 열어 주거나 아무 것도 하지 않거나 함
function cubric_check_respawn_group_clear( respawn_group, x, y, layer )

	-- 부 보스 그룹으로 리젠된 몬스터가 죽은 경우
	if respawn_group == 1 or respawn_group == 2 then

		-- 부 보스 몬스터는 죽을 때마다 갈림길 방으로 돌아가는 프랍 열어 줌
		add_field_prop( 60156, 0, x, y, layer, 0 )

		-- 필요한 경우 미션 UI 업데이트 처리
		update_cubric_mission( true, layer )

	-- 보스 그룹으로 리젠된 몬스터가 죽은 경우
	elseif respawn_group == 3 then

		-- 보스는 한 마리밖에 리젠 안 되기 때문에 모두 죽었는지 체크할 필요 없음
		-- 큐브릭 던전 진행 퀘스트가 진행 중일 경우에는 밖으로 출구 프랍을 생성하지 않고,
		-- 퀘스트가 진행 중인 유저가 하나도 없다면 그냥 밖으로 나갈 수 있도록 포탈 열어 줌
		-- 진행 중인지 판별하는 퀘스트는 [ID:3607 <(version:7.4)>[큐브릭]악마 큐브]를 사용(QuestResource.script_start_text, script_end_text, script_drop_text를 이용하여 세팅함)
		if get_instance_dungeon_flag( 30000, layer, 'QuestInProgressCount' ) == 0 then
			
			add_field_prop( 60102, 0, 99954, 31539, layer, 0 )
			set_instance_dungeon_flag( 30000, layer, 'ExitPropRespawned', 1 )

		end

	end

end


-- 용도: 아크샤 잡고 생성된 포탈 클릭 시 호출되는 함수
-- 역할: 중앙의 갈림길 방으로 플레이어를 워프시키면서 아크샤를 두 마리 잡아야 한다는 미션 UI를 활성화시켜 줌
function warp_to_cubric_branch_room( side )

	local layer = gv( 'layer' )

	-- 중앙의 방으로 워프시켜 줌
	if side == 1 then
		warp( 98858, 30827, layer )
	elseif side == 2 then
		warp( 99180, 30825, layer )
	end

	-- 미션 UI가 아직 활성화 안 되어 있으면 미션 UI 활성화
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) == 0 then

		set_instance_dungeon_flag( 30000, layer, 'MissionRevealed', 1 )

		-- 미션 제목
		broadcast_mission_title( 1, '@1234', 30000, layer )

		-- 0번 미션 목표(서쪽의 아크샤)
		broadcast_mission_objective( 1, 0, 1, '@1235', 30000, layer )
		-- 1번 미션 목표(동쪽의 아크샤)
		broadcast_mission_objective( 1, 1, 1, '@1236', 30000, layer )
		-- 2번 미션 목표(보스방 드가세요)
		broadcast_mission_objective( 1, 2, 1, '@1237', 30000, layer )

		-- 미션 보상 안내
		broadcast_mission_reward( 1, '@1238', 30000, layer )

		-- 미션 목표 진행 상태
		update_cubric_mission( true, layer )

	end

end


-- 용도: 아크샤 두 마리 다 잡고 보스 방으로 들어가는 문 클릭
-- 역할: 보스 방 내부로 플레이어를 워프시키면서 미션 UI를 제거시켜 줌
function warp_to_cubric_boss_room()

	local layer = gv( 'layer' )

	-- 보스 방 내부로 워프
	warp( 99711, 31535, layer )

	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) == 1 then

		-- 미션 완료
		broadcast_mission_objective_progress( 1, 2, 1, 30000, layer )

		-- 미션 보상 지급
		do_each_player_in_instance_dungeon( 30000, layer, 'cubric_reward_distributer()' )

		-- 미션 UI 더 이상 활성화되지 않게 플래그 값 변경
		set_instance_dungeon_flag( 30000, layer, 'MissionRevealed', 2 )

		-- 미션 UI 비활성화
		broadcast_mission_title( 1, '', 30000, layer )

	end

end

function cubric_reward_distributer()

	--퀘스트 보상 Lv 150, 1% 수준
	insert_gold( 300000 )
	add_exp_jp( 5131363, 884717 )
	cprint( '@1239' )

end


-- 용도: 미션 UI를 업데이트 해야 하는 몇몇 경우에 호출(부보스 사망, 유저 입장 등)
-- 역할: 미션 UI를 업데이트해야 하는 이벤트 발생 시 방 안의 모든 유저 또는 현재 스크립트 실행 주체 유저에게 미션 내용 알려주기
function update_cubric_mission( broadcast, layer )

	
	-- 미션이 진행 중이지 않다면 딱히 알려줄 것도 없음
	if get_instance_dungeon_flag( 30000, layer, 'MissionRevealed' ) ~= 1 then
		return
	end

	if broadcast == true then

		broadcast_mission_objective_progress( 1, 0, 1 - get_alive_instance_respawn_group_monster_count( 30000, layer, 1 ), 30000, layer )
		broadcast_mission_objective_progress( 1, 1, 1 - get_alive_instance_respawn_group_monster_count( 30000, layer, 2 ), 30000, layer)
		broadcast_mission_objective_progress( 1, 2, 0, 30000, layer )

	else

		send_mission_objective_progress( 0, 1 - get_alive_instance_respawn_group_monster_count( 30000, layer, 1 ) )
		send_mission_objective_progress( 1, 1 - get_alive_instance_respawn_group_monster_count( 30000, layer, 2 ) )
		send_mission_objective_progress( 2, 0 )

	end

end

function NPC_bulcanus_joinitem_change()

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605269" )	--대사 ( 기본 대사 )
	
	if is_premium() then
		dlg_text( "@90605269" )
	else
		dlg_text( "@90700118" )
	end
	
	if is_premium() then
	dlg_menu( "@90605271", " NPC_bulcanus_joinitem_change_want()" ) -- 사자의 혼 교환
	end

	dlg_menu( "@90010002", " " )	--대화 종료
	dlg_show()
end

function NPC_bulcanus_joinitem_change_want()

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605285" )	--대사 ( 몇 층의 혼을 원하냐는 대화 )
	
	dlg_menu( "@90605272", " NPC_bulcanus_joinitem_change_want_choice1( 1 ) " )
	dlg_menu( "@90605273", " NPC_bulcanus_joinitem_change_want_choice2() " )
	dlg_menu( "@90605274", " NPC_bulcanus_joinitem_change_want_choice3() " )
	dlg_menu( "@90605275", " NPC_bulcanus_joinitem_change_want_choice4( 6 ) " )
	dlg_menu( "@90010003", " NPC_bulcanus_joinitem_change() " )											--돌아가기
	dlg_show()
end

function NPC_bulcanus_joinitem_change_want_choice1( case )

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605286" )	--대사 ( 1층의 혼을 원하는 것이 맞는지 묻는 대화 )
	
	dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_number( 1 ) " )  					--예 ( 갯수 진행 )
	dlg_menu( "@90010196", " NPC_bulcanus_joinitem_change_want() " ) 									--아니오 ( 돌아가기 )
	dlg_show()
end

function NPC_bulcanus_joinitem_change_want_choice2()

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605287" )	--대사 ( 2층의 혼을 원하는 것이 맞는지와 어떤 층의 혼을 사용할 것인지 대화 )
	
	dlg_menu( "@90605276", " NPC_bulcanus_joinitem_change_want_choice_1f2f( 2 ) " )  					--1층 사자의 혼 사용 ( 맞는지 진행 )
	dlg_menu( "@90605277", " NPC_bulcanus_joinitem_change_want_choice_3f2f( 3 ) " )  					--2층 사자의 혼 사용 ( 맞는지 진행 )
	dlg_menu( "@90010003", " NPC_bulcanus_joinitem_change_want() " ) 									-- 돌아가기 
	dlg_show()
end

function NPC_bulcanus_joinitem_change_want_choice3()

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605288" )	--대사 ( 3층의 혼을 원하는 것이 맞는지와 어떤 층의 혼을 사용할 것인지 대화 )
	
	dlg_menu( "@90605278", " NPC_bulcanus_joinitem_change_want_choice_2f3f( 4 ) " )  					--2층 사자의 혼 사용 ( 맞는지 진행 )
	dlg_menu( "@90605279", " NPC_bulcanus_joinitem_change_want_choice_4f3f( 5 ) " )  					--4층 사자의 혼 사용 ( 맞는지 진행 )
	dlg_menu( "@90010003", " NPC_bulcanus_joinitem_change_want() " ) 									-- 돌아가기 
	dlg_show()
end

function NPC_bulcanus_joinitem_change_want_choice4( case )

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605289" )	--대사 ( 1층의 혼을 원하는 것이 맞는지 묻는 대화 )
	
	dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_number(  6  ) " )  				--예 ( 갯수 진행 )
	dlg_menu( "@90010196", " NPC_bulcanus_joinitem_change_want() " ) 									--아니오 ( 돌아가기 )
	dlg_show()
end



function NPC_bulcanus_joinitem_change_want_choice_1f2f( case )

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605290" )	--대사 ( 1층 사자의 혼으로 2층 사자의 혼을 원하는 것인지 왁인해주는 대화 )
	
	dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_number(  2  )" )  				-- 예( 갯수 진행 )
	dlg_menu( "@90010003", " NPC_bulcanus_joinitem_change_want_choice2() " ) 							-- 돌아가기 
	dlg_show()
end

function NPC_bulcanus_joinitem_change_want_choice_3f2f( case )

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605291" )	--대사 (  3층 사자의 혼으로 2층 사자의 혼을 워너하는 것인지 확인해주는 대화 )
	
	dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_number(  3  )" )  				-- 예 ( 갯수 진행 )
	dlg_menu( "@90010003", " NPC_bulcanus_joinitem_change_want_choice2() " ) 							-- 돌아가기 
	dlg_show()
end

function NPC_bulcanus_joinitem_change_want_choice_2f3f( case )

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605292" )	--대사 ( 2층 사자의 혼으로 3층 사자의 혼을 원하는 것인지 확인해주는 대화 )
	
	dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_number(  4  )" )  				-- 예 ( 갯수 진행 )
	dlg_menu( "@90010003", " NPC_bulcanus_joinitem_change_want_choice3() " ) 							-- 돌아가기 
	dlg_show()
end


function NPC_bulcanus_joinitem_change_want_choice_4f3f( case )

	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605293" )	--대사 ( 4층 사자의 혼으로 3층 사자의 혼을 원하는 것인지 확인해주는 대화 )
		
	dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_number(  5  )" )  				-- 예 ( 갯수 진행 )
	dlg_menu( "@90010003", " NPC_bulcanus_joinitem_change_want_choice3() " ) 							-- 돌아가기 
	dlg_show()
end

function NPC_bulcanus_joinitem_change_want_choice_number( case )	
	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )
	dlg_text( "@90605294" )	--대사 ( 몇개의 사자의 혼을 바꿀지를 묻는 대화 )
	
	dlg_menu( "@90605280", " NPC_bulcanus_joinitem_change_lastcheck( " .. case .. ", 10 ) " )  			-- 10개
	dlg_menu( "@90605281", " NPC_bulcanus_joinitem_change_lastcheck( " .. case .. ", 50 ) " )  			-- 50개
	dlg_menu( "@90605282", " NPC_bulcanus_joinitem_change_lastcheck( " .. case .. " , 100 ) " )  		-- 100개
	dlg_menu( "@90605283", " NPC_bulcanus_joinitem_change_lastcheck( " .. case .. " , 500 ) " )  		-- 500개
	dlg_menu( "@90010003", " NPC_bulcanus_joinitem_change_want() " ) 	-- 돌아가기 
	dlg_show()
end

function NPC_bulcanus_joinitem_change_lastcheck( case , number )
	dlg_title( "@90605268" )	--NPC 이름 ( 귀부인 라니에 )

	if case == 1 then 
		
		dlg_text( sconv ( "@90605295" , "#@want_num@#", tostring(  number  ),"#@use_floor@#", tostring(2), "#@use_num@#", tostring( ( number ) / 2 ), "#@want_floor@#", tostring(1) ) )
		
		dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_savecheck(" .. case .. ", " .. number .. ")" )  		-- 예
		dlg_menu( "@90010196", " NPC_bulcanus_joinitem_change_want_choice_number(" .. case .. ")" ) 							-- 아니오
		dlg_show()
		
	elseif case == 2 then 
		dlg_text( sconv ( "@90605295" , "#@want_num@#", tostring(  number  ),"#@use_floor@#", tostring(1), "#@use_num@#", tostring( ( number ) * 2 ), "#@want_floor@#", tostring(2) ) )
		
		dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_savecheck(" .. case .. ", " .. number .. ")" )  		-- 예
		dlg_menu( "@90010196", " NPC_bulcanus_joinitem_change_want_choice_number(   case   ) " ) 								-- 아니오
		dlg_show()
			
	elseif case == 3 then 
		dlg_text( sconv ( "@90605295" , "#@want_num@#", tostring(  number  ),"#@use_floor@#", tostring(3), "#@use_num@#", tostring( ( number ) / 2 ), "#@want_floor@#", tostring(2) ) )
		
		dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_savecheck(" .. case .. ", " .. number .. ")" )  		-- 예
		dlg_menu( "@90010196", " NPC_bulcanus_joinitem_change_want_choice_number(" .. case .. ")" ) 							-- 아니오
		dlg_show()
			
	elseif case == 4 then 	
		dlg_text( sconv ( "@90605295" , "#@want_num@#", tostring(  number  ),"#@use_floor@#", tostring(2), "#@use_num@#", tostring( ( number ) * 2 ), "#@want_floor@#", tostring(3) ) )
		
		dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_savecheck(" .. case .. ", " .. number .. ")" )  		-- 예
		dlg_menu( "@90010196", " NPC_bulcanus_joinitem_change_want_choice_number(" .. case .. ")" ) 							-- 아니오
		dlg_show()
		
	elseif case == 5 then 	
		dlg_text( sconv ( "@90605295" , "#@want_num@#", tostring(  number  ),"#@use_floor@#", tostring(4), "#@use_num@#", tostring( ( number ) / 2 ), "#@want_floor@#", tostring(3) ) )
		
		dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_savecheck(" .. case .. ", " .. number .. ")" )  		-- 예
		dlg_menu( "@90010196", " NPC_bulcanus_joinitem_change_want_choice_number(" .. case .. ")" ) 							-- 아니오
		dlg_show()

	elseif case == 6 then 	
		dlg_text( sconv ( "@90605295" , "#@want_num@#", tostring(  number  ),"#@use_floor@#", tostring(3), "#@use_num@#", tostring( ( number ) * 2 ), "#@want_floor@#", tostring(4) ) )
		
		dlg_menu( "@90010195", " NPC_bulcanus_joinitem_change_want_choice_savecheck(" .. case .. ", " .. number .. ")" )  		-- 예
		dlg_menu( "@90010196", " NPC_bulcanus_joinitem_change_want_choice_number(" .. case .. ")" ) 							-- 아니오	
		dlg_show()		
	end 
	
end

function NPC_bulcanus_joinitem_change_want_choice_savecheck( save_case , save_number ) 

	local count 
	
	if  save_case == 1 then
	
		count = find_item( 1000402 )
		 
		if count >= ( save_number ) / 2 then
			delete_item( get_item_handle( 1000402 ), ( save_number ) / 2 )
			insert_item( 1000401, save_number  )
			cprint( "@90605296" )
		else
			cprint( "@90605297" )
			return
		end	
	
	elseif save_case == 2 then
	
		count = find_item( 1000401 )
		
		if count >= ( save_number ) * 2 then
			delete_item( get_item_handle( 1000401 ),  ( save_number ) * 2 )
			insert_item( 1000402, save_number )
			cprint( "@90605296" )
		else
			cprint( "@90605297" )
			return
		end	
	
	elseif save_case == 3 then	
	
		count = find_item( 1000403 )
		
		if count >= ( save_number ) / 2 then
			delete_item( get_item_handle( 1000403 ),  ( save_number ) / 2 )
			insert_item( 1000402, save_number )
			cprint( "@90605296" )
		else
			cprint( "@90605297" )
			return
		end	
	
	elseif save_case == 4 then
	
		count = find_item( 1000402 )
		
		if count >= ( save_number ) * 2 then
			delete_item( get_item_handle( 1000402 ),  ( save_number ) * 2 )
			insert_item( 1000403, save_number )
			cprint( "@90605296" )
		else
			cprint( "@90605297" )
			return
		end	
	
	elseif save_case == 5 then
	
		count = find_item( 1000404 )
		
		if count >= ( save_number ) / 2 then
			delete_item( get_item_handle( 1000404 ),  ( save_number ) / 2 )
			insert_item( 1000403, save_number )
			cprint( "@90605296" )
		else
			cprint( "@90605297" )
			return
		end	
	
	elseif save_case == 6 then
	
		count = find_item( 1000403 )
		
		if count >= ( save_number ) * 2 then
			delete_item( get_item_handle( 1000403 ),  ( save_number ) * 2 )
			insert_item( 1000404, save_number )
			cprint( "@90605296" )
		else
			cprint( "@90605297" )
			return
		end	
	
	end
	
end

function Call_BossMob( monster_handle )

	local monster_id = get_monster_id( monster_handle )
	local layer = ghv( monster_handle, 'layer' )

	local quest_progress3644 = get_quest_progress(3644) -- [일일]던전 토벌: 메두사 사냥
	local quest_progress3645 = get_quest_progress(3645) -- [일일]던전 토벌: 블랙 위도우 사냥
	local quest_progress3646 = get_quest_progress(3646) -- [일일]던전 토벌: 본 드래곤 사냥
	local quest_progress3647 = get_quest_progress(3647) -- [일일]던전 토벌: 미크로랍토르 사냥
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	
	
--==========================================================================1단계
	if monster_id == 20190005 then
		
		add_npc( 38871, 22943, 20190006, 1, 0, layer )
		add_npc( 38906, 23013, 20190001, 1, 0, layer )
		add_npc( 39215, 22959, 20190003, 1, 0, layer )
		add_npc( 39138, 22881, 20190003, 1, 0, layer )
		add_npc( 39087, 23021, 20190003, 1, 0, layer )
		add_npc( 39067, 22964, 20190003, 1, 0, layer )
		add_npc( 38980, 22920, 20190003, 1, 0, layer )
		add_npc( 28980, 23005, 20190003, 1, 0, layer )
		
		if quest_progress3644 == 1 then
			
			if get_quest_status( 3644 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 3 ) + 1
				set_quest_status( 3644, 3, A_day_A_quest )
			end
		end
	        
	elseif monster_id == 20190007 then
		
		add_npc( 40715, 9563, 20190008, 1, 0, layer )
		add_npc( 40497, 9586, 20190025, 1, 0, layer )
		add_npc( 40763, 9623, 20190004, 1, 0, layer )
		add_npc( 40761, 9591, 20190004, 1, 0, layer )
		add_npc( 40765, 9526, 20190004, 1, 0, layer )
		add_npc( 40697, 9492, 20190004, 1, 0, layer )
		add_npc( 40692, 9547, 20190004, 1, 0, layer )
		add_npc( 40709, 9596, 20190004, 1, 0, layer )
		add_npc( 40715, 9618, 20190004, 1, 0, layer )
		add_npc( 40718, 9660, 20190004, 1, 0, layer )
		add_npc( 40642, 9655, 20190004, 1, 0, layer )
		add_npc( 40623, 9589, 20190004, 1, 0, layer )
		add_npc( 40632, 9543, 20190004, 1, 0, layer )
		add_npc( 40639, 9498, 20190004, 1, 0, layer )
		
		if quest_progress3645 == 1 then
			
			if get_quest_status( 3645 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 3 ) + 1
				set_quest_status( 3645, 3, A_day_A_quest )
			end
		end
	
	elseif monster_id == 20190009 then
		
		add_npc( 61203, 30845, 20190012, 1, 0, layer )
		add_npc( 61360, 30953, 20190026, 1, 0, layer )
		add_npc( 61254, 30888, 20190002, 1, 0, layer )
		add_npc( 61107, 30905, 20190002, 1, 0, layer )
		add_npc( 60838, 30876, 20190002, 1, 0, layer )
		add_npc( 60725, 30763, 20190002, 1, 0, layer )
		add_npc( 60794, 30659, 20190002, 1, 0, layer )
		add_npc( 61050, 30627, 20190002, 1, 0, layer )
		add_npc( 60866, 31022, 20190002, 1, 0, layer )
		add_npc( 60770, 30926, 20190002, 1, 0, layer )
		add_npc( 60640, 30799, 20190002, 1, 0, layer )
		add_npc( 60654, 30696, 20190002, 1, 0, layer )
		add_npc( 60517, 30887, 20190002, 1, 0, layer )
		add_npc( 61129, 30766, 20190002, 1, 0, layer )
		
		if quest_progress3646 == 1 then
			
			if get_quest_status( 3646 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 3 ) + 1
				set_quest_status( 3646, 3, A_day_A_quest )
			end
		end

		
	elseif monster_id == 20190011 then

		add_npc( 53668, 4378, 20190010, 1, 0, layer )
		add_npc( 53568, 4384, 20190027, 1, 0, layer )
		add_npc( 54092, 4663, 20190003, 1, 0, layer )
		add_npc( 54073, 4648, 20190003, 1, 0, layer )
		add_npc( 54074, 4534, 20190003, 1, 0, layer )
		add_npc( 54019, 4622, 20190003, 1, 0, layer )
		add_npc( 53969, 4714, 20190003, 1, 0, layer )
		add_npc( 53931, 4595, 20190003, 1, 0, layer )
		add_npc( 53831, 4524, 20190004, 1, 0, layer )
		add_npc( 53757, 4558, 20190004, 1, 0, layer )
		add_npc( 53661, 4569, 20190004, 1, 0, layer )
		add_npc( 53583, 4490, 20190004, 1, 0, layer )
		add_npc( 53638, 4391, 20190004, 1, 0, layer )
		add_npc( 53700, 4573, 20190004, 1, 0, layer )
		add_npc( 53766, 4572, 20190002, 1, 0, layer )
		add_npc( 54062, 4425, 20190002, 1, 0, layer )
		add_npc( 54172, 4572, 20190002, 1, 0, layer )
		
		if quest_progress3647 == 1 then
			
			if get_quest_status( 3647 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 3 ) + 1
				set_quest_status( 3647, 3, A_day_A_quest )
			end
		end
	end	

--==========================================================================2단계	
	if monster_id == 20190032 then
		
		add_npc( 38871, 22943, 20190033, 1, 0, layer )
		add_npc( 38906, 23013, 20190028, 1, 0, layer )
		add_npc( 39215, 22959, 20190030, 1, 0, layer )
		add_npc( 39138, 22881, 20190030, 1, 0, layer )
		add_npc( 39087, 23021, 20190030, 1, 0, layer )
		add_npc( 39067, 22964, 20190030, 1, 0, layer )
		add_npc( 38980, 22920, 20190030, 1, 0, layer )
		add_npc( 28980, 23005, 20190030, 1, 0, layer )
		
		if quest_progress3644 == 1 then
			
			if get_quest_status( 3644 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 3 ) + 1
				set_quest_status( 3644, 3, A_day_A_quest )
			end
		end
	        
	elseif monster_id == 20190034 then
		
		add_npc( 40715, 9563, 20190035, 1, 0, layer )
		add_npc( 40497, 9586, 20190052, 1, 0, layer )
		add_npc( 40763, 9623, 20190031, 1, 0, layer )
		add_npc( 40761, 9591, 20190031, 1, 0, layer )
		add_npc( 40765, 9526, 20190031, 1, 0, layer )
		add_npc( 40697, 9492, 20190031, 1, 0, layer )
		add_npc( 40692, 9547, 20190031, 1, 0, layer )
		add_npc( 40709, 9596, 20190031, 1, 0, layer )
		add_npc( 40715, 9618, 20190031, 1, 0, layer )
		add_npc( 40718, 9660, 20190031, 1, 0, layer )
		add_npc( 40642, 9655, 20190031, 1, 0, layer )
		add_npc( 40623, 9589, 20190031, 1, 0, layer )
		add_npc( 40632, 9543, 20190031, 1, 0, layer )
		add_npc( 40639, 9498, 20190031, 1, 0, layer )
		
		if quest_progress3645 == 1 then
			
			if get_quest_status( 3645 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 3 ) + 1
				set_quest_status( 3645, 3, A_day_A_quest )
			end
		end
	elseif monster_id == 20190036 then
		
		add_npc( 61203, 30845, 20190039, 1, 0, layer )
		add_npc( 61360, 30953, 20190053, 1, 0, layer )
		add_npc( 61254, 30888, 20190029, 1, 0, layer )
		add_npc( 61107, 30905, 20190029, 1, 0, layer )
		add_npc( 60838, 30876, 20190029, 1, 0, layer )
		add_npc( 60725, 30763, 20190029, 1, 0, layer )
		add_npc( 60794, 30659, 20190029, 1, 0, layer )
		add_npc( 61050, 30627, 20190029, 1, 0, layer )
		add_npc( 60866, 31022, 20190029, 1, 0, layer )
		add_npc( 60770, 30926, 20190029, 1, 0, layer )
		add_npc( 60640, 30799, 20190029, 1, 0, layer )
		add_npc( 60654, 30696, 20190029, 1, 0, layer )
		add_npc( 60517, 30887, 20190029, 1, 0, layer )
		add_npc( 61129, 30766, 20190029, 1, 0, layer )
		
		if quest_progress3646 == 1 then
			
			if get_quest_status( 3646 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 3 ) + 1
				set_quest_status( 3646, 3, A_day_A_quest )
			end
		end

		
	elseif monster_id == 20190038 then

		add_npc( 53668, 4378, 20190037, 1, 0, layer )
		add_npc( 53568, 4384, 20190054, 1, 0, layer )
		add_npc( 54092, 4663, 20190030, 1, 0, layer )
		add_npc( 54073, 4648, 20190030, 1, 0, layer )
		add_npc( 54074, 4534, 20190030, 1, 0, layer )
		add_npc( 54019, 4622, 20190030, 1, 0, layer )
		add_npc( 53969, 4714, 20190030, 1, 0, layer )
		add_npc( 53931, 4595, 20190030, 1, 0, layer )
		add_npc( 53831, 4524, 20190031, 1, 0, layer )
		add_npc( 53757, 4558, 20190031, 1, 0, layer )
		add_npc( 53661, 4569, 20190031, 1, 0, layer )
		add_npc( 53583, 4490, 20190031, 1, 0, layer )
		add_npc( 53638, 4391, 20190031, 1, 0, layer )
		add_npc( 53700, 4573, 20190031, 1, 0, layer )
		add_npc( 53766, 4572, 20190029, 1, 0, layer )
		add_npc( 54062, 4425, 20190029, 1, 0, layer )
		add_npc( 54172, 4572, 20190029, 1, 0, layer )
		
		if quest_progress3647 == 1 then
			
			if get_quest_status( 3647 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 3 ) + 1
				set_quest_status( 3647, 3, A_day_A_quest )
			end
		end
	end
	
--==========================================================================3단계	
	if monster_id == 20190059 then
		
		add_npc( 38871, 22943, 20190060, 1, 0, layer )
		add_npc( 38906, 23013, 20190055, 1, 0, layer )
		add_npc( 39215, 22959, 20190057, 1, 0, layer )
		add_npc( 39138, 22881, 20190057, 1, 0, layer )
		add_npc( 39087, 23021, 20190057, 1, 0, layer )
		add_npc( 39067, 22964, 20190057, 1, 0, layer )
		add_npc( 38980, 22920, 20190057, 1, 0, layer )
		add_npc( 28980, 23005, 20190057, 1, 0, layer )
		
		if quest_progress3644 == 1 then
			
			if get_quest_status( 3644 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 3 ) + 1
				set_quest_status( 3644, 3, A_day_A_quest )
			end
		end
	        
	elseif monster_id == 20190061 then
		
		add_npc( 40715, 9563, 20190062, 1, 0, layer )
		add_npc( 40497, 9586, 20190079, 1, 0, layer )
		add_npc( 40763, 9623, 20190058, 1, 0, layer )
		add_npc( 40761, 9591, 20190058, 1, 0, layer )
		add_npc( 40765, 9526, 20190058, 1, 0, layer )
		add_npc( 40697, 9492, 20190058, 1, 0, layer )
		add_npc( 40692, 9547, 20190058, 1, 0, layer )
		add_npc( 40709, 9596, 20190058, 1, 0, layer )
		add_npc( 40715, 9618, 20190058, 1, 0, layer )
		add_npc( 40718, 9660, 20190058, 1, 0, layer )
		add_npc( 40642, 9655, 20190058, 1, 0, layer )
		add_npc( 40623, 9589, 20190058, 1, 0, layer )
		add_npc( 40632, 9543, 20190058, 1, 0, layer )
		add_npc( 40639, 9498, 20190058, 1, 0, layer )
		
		if quest_progress3645 == 1 then
			
			if get_quest_status( 3645 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 3 ) + 1
				set_quest_status( 3645, 3, A_day_A_quest )
			end
		end
	
	elseif monster_id == 20190063 then
		
		add_npc( 61203, 30845, 20190066, 1, 0, layer )
		add_npc( 61360, 30953, 20190080, 1, 0, layer )
		add_npc( 61254, 30888, 20190056, 1, 0, layer )
		add_npc( 61107, 30905, 20190056, 1, 0, layer )
		add_npc( 60838, 30876, 20190056, 1, 0, layer )
		add_npc( 60725, 30763, 20190056, 1, 0, layer )
		add_npc( 60794, 30659, 20190056, 1, 0, layer )
		add_npc( 61050, 30627, 20190056, 1, 0, layer )
		add_npc( 60866, 31022, 20190056, 1, 0, layer )
		add_npc( 60770, 30926, 20190056, 1, 0, layer )
		add_npc( 60640, 30799, 20190056, 1, 0, layer )
		add_npc( 60654, 30696, 20190056, 1, 0, layer )
		add_npc( 60517, 30887, 20190056, 1, 0, layer )
		add_npc( 61129, 30766, 20190056, 1, 0, layer )
		
		if quest_progress3646 == 1 then
			
			if get_quest_status( 3646 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 3 ) + 1
				set_quest_status( 3646, 3, A_day_A_quest )
			end
		end

		
	elseif monster_id == 20190065 then

		add_npc( 53668, 4378, 20190064, 1, 0, layer )
		add_npc( 53568, 4384, 20190081, 1, 0, layer )
		add_npc( 54092, 4663, 20190057, 1, 0, layer )
		add_npc( 54073, 4648, 20190057, 1, 0, layer )
		add_npc( 54074, 4534, 20190057, 1, 0, layer )
		add_npc( 54019, 4622, 20190057, 1, 0, layer )
		add_npc( 53969, 4714, 20190057, 1, 0, layer )
		add_npc( 53931, 4595, 20190057, 1, 0, layer )
		add_npc( 53831, 4524, 20190058, 1, 0, layer )
		add_npc( 53757, 4558, 20190058, 1, 0, layer )
		add_npc( 53661, 4569, 20190058, 1, 0, layer )
		add_npc( 53583, 4490, 20190058, 1, 0, layer )
		add_npc( 53638, 4391, 20190058, 1, 0, layer )
		add_npc( 53700, 4573, 20190058, 1, 0, layer )
		add_npc( 53766, 4572, 20190056, 1, 0, layer )
		add_npc( 54062, 4425, 20190056, 1, 0, layer )
		add_npc( 54172, 4572, 20190056, 1, 0, layer )
		
		
		if quest_progress3647 == 1 then
			
			if get_quest_status( 3647 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 3 ) + 1
				set_quest_status( 3647, 3, A_day_A_quest )
			end
		end
	end
	

--==========================================================================4단계	
	if monster_id == 20190086 then
		
		add_npc( 38871, 22943, 20190087, 1, 0, layer )
		add_npc( 38906, 23013, 20190082, 1, 0, layer )
		add_npc( 39215, 22959, 20190084, 1, 0, layer )
		add_npc( 39138, 22881, 20190084, 1, 0, layer )
		add_npc( 39087, 23021, 20190084, 1, 0, layer )
		add_npc( 39067, 22964, 20190084, 1, 0, layer )
		add_npc( 38980, 22920, 20190084, 1, 0, layer )
		add_npc( 28980, 23005, 20190084, 1, 0, layer )
		
		if quest_progress3644 == 1 then
			
			if get_quest_status( 3644 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 3 ) + 1
				set_quest_status( 3644, 3, A_day_A_quest )
			end
		end
	        
	elseif monster_id == 20190088 then
		
		add_npc( 40715, 9563, 20190089, 1, 0, layer )
		add_npc( 40497, 9586, 21190106, 1, 0, layer )
		add_npc( 40763, 9623, 20190085, 1, 0, layer )
		add_npc( 40761, 9591, 20190085, 1, 0, layer )
		add_npc( 40765, 9526, 20190085, 1, 0, layer )
		add_npc( 40697, 9492, 20190085, 1, 0, layer )
		add_npc( 40692, 9547, 20190085, 1, 0, layer )
		add_npc( 40709, 9596, 20190085, 1, 0, layer )
		add_npc( 40715, 9618, 20190085, 1, 0, layer )
		add_npc( 40718, 9660, 20190085, 1, 0, layer )
		add_npc( 40642, 9655, 20190085, 1, 0, layer )
		add_npc( 40623, 9589, 20190085, 1, 0, layer )
		add_npc( 40632, 9543, 20190085, 1, 0, layer )
		add_npc( 40639, 9498, 20190085, 1, 0, layer )
		
		if quest_progress3645 == 1 then
			
			if get_quest_status( 3645 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 3 ) + 1
				set_quest_status( 3645, 3, A_day_A_quest )
			end
		end
	
	elseif monster_id == 20190090 then
		
		add_npc( 61203, 30845, 20190093, 1, 0, layer )
		add_npc( 61360, 30953, 21190107, 1, 0, layer )
		add_npc( 61254, 30888, 20190083, 1, 0, layer )
		add_npc( 61107, 30905, 20190083, 1, 0, layer )
		add_npc( 60838, 30876, 20190083, 1, 0, layer )
		add_npc( 60725, 30763, 20190083, 1, 0, layer )
		add_npc( 60794, 30659, 20190083, 1, 0, layer )
		add_npc( 61050, 30627, 20190083, 1, 0, layer )
		add_npc( 60866, 31022, 20190083, 1, 0, layer )
		add_npc( 60770, 30926, 20190083, 1, 0, layer )
		add_npc( 60640, 30799, 20190083, 1, 0, layer )
		add_npc( 60654, 30696, 20190083, 1, 0, layer )
		add_npc( 60517, 30887, 20190083, 1, 0, layer )
		add_npc( 61129, 30766, 20190083, 1, 0, layer )
		
		if quest_progress3646 == 1 then
			
			if get_quest_status( 3646 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 3 ) + 1
				set_quest_status( 3646, 3, A_day_A_quest )
			end
		end

		
	elseif monster_id == 20190092 then

		add_npc( 53668, 4378, 20190091, 1, 0, layer )
		add_npc( 53568, 4384, 21190108, 1, 0, layer )
		add_npc( 54092, 4663, 20190084, 1, 0, layer )
		add_npc( 54073, 4648, 20190084, 1, 0, layer )
		add_npc( 54074, 4534, 20190084, 1, 0, layer )
		add_npc( 54019, 4622, 20190084, 1, 0, layer )
		add_npc( 53969, 4714, 20190084, 1, 0, layer )
		add_npc( 53931, 4595, 20190084, 1, 0, layer )
		add_npc( 53831, 4524, 20190085, 1, 0, layer )
		add_npc( 53757, 4558, 20190085, 1, 0, layer )
		add_npc( 53661, 4569, 20190085, 1, 0, layer )
		add_npc( 53583, 4490, 20190085, 1, 0, layer )
		add_npc( 53638, 4391, 20190085, 1, 0, layer )
		add_npc( 53700, 4573, 20190085, 1, 0, layer )
		add_npc( 53766, 4572, 20190083, 1, 0, layer )
		add_npc( 54062, 4425, 20190083, 1, 0, layer )
		add_npc( 54172, 4572, 20190083, 1, 0, layer )
		
		if quest_progress3647 == 1 then
			
			if get_quest_status( 3647 , 3 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 3 ) + 1
				set_quest_status( 3647, 3, A_day_A_quest )
			end
		end
	
	end	

end

function Call_Main_Chest( monster_handle )

	local monster_id = get_monster_id( monster_handle )
	local layer = ghv( monster_handle, 'layer' )
	
	local quest_progress3644 = get_quest_progress(3644) -- [일일]던전 토벌: 메두사 사냥
	local quest_progress3645 = get_quest_progress(3645) -- [일일]던전 토벌: 블랙 위도우 사냥
	local quest_progress3646 = get_quest_progress(3646) -- [일일]던전 토벌: 본 드래곤 사냥
	local quest_progress3647 = get_quest_progress(3647) -- [일일]던전 토벌: 미크로랍토르 사냥
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	
	
	if monster_id == 20190001 or monster_id == 20190006 --메던 지하기지 1단계
	or monster_id == 20190028 or monster_id == 20190033 --메던 지하기지 2단계
	or monster_id == 20190055 or monster_id == 20190060 --메던 지하기지 3단계
	or monster_id == 20190082 or monster_id == 20190087 then --메던 지하기지 4단계
	
		local monster_count = get_instance_dungeon_flag( 40000, layer, 'check_open_prop')
		if monster_count == 2 then
			set_instance_dungeon_flag( 40000, layer, 'check_open_prop', 1)
		elseif monster_count == 1 then
			add_field_prop ( 60170, 60000, 38719, 22938, layer )
			set_instance_dungeon_flag( 40000, layer, 'check_open_prop', 0)
		end
		
		if quest_progress3644 == 1 and monster_id == 20190001 then
			if get_quest_status( 3644 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 1 ) + 1
				set_quest_status( 3644, 1, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190028 then
			if get_quest_status( 3644 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 1 ) + 1
				set_quest_status( 3644, 1, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190055 then
			if get_quest_status( 3644 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 1 ) + 1
				set_quest_status( 3644, 1, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190082 then
			if get_quest_status( 3644 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 1 ) + 1
				set_quest_status( 3644, 1, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190006 then
			if get_quest_status( 3644 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 2 ) + 1
				set_quest_status( 3644, 2, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190033 then
			if get_quest_status( 3644 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 2 ) + 1
				set_quest_status( 3644, 2, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190060 then
			if get_quest_status( 3644 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 2 ) + 1
				set_quest_status( 3644, 2, A_day_A_quest )
			end
		elseif quest_progress3644 == 1 and monster_id == 20190087 then
			if get_quest_status( 3644 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3644 , 2 ) + 1
				set_quest_status( 3644, 2, A_day_A_quest )
			end
		end
		
	elseif monster_id == 20190025 or monster_id == 20190008 --탄광 지하기지 1단계
	or monster_id == 20190052 or monster_id == 20190035 --탄광 지하기지 2단계
	or monster_id == 20190079 or monster_id == 20190062 --탄광 지하기지 3단계
	or monster_id == 21190106 or monster_id == 20190089 then --탄광 지하기지 4단계
	
		local monster_count = get_instance_dungeon_flag( 41001, layer, 'check_open_prop')
		if monster_count == 2 then
			set_instance_dungeon_flag( 41001, layer, 'check_open_prop', 1)
		elseif monster_count == 1 then
			add_field_prop ( 60171, 60000, 40379, 9573, layer )
			set_instance_dungeon_flag( 41001, layer, 'check_open_prop', 0)
		end
		
		if quest_progress3645 == 1 and monster_id == 20190025 then
			if get_quest_status( 3645 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 1 ) + 1
				set_quest_status( 3645, 1, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190052 then
			if get_quest_status( 3645 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 1 ) + 1
				set_quest_status( 3645, 1, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190079 then
			if get_quest_status( 3645 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 1 ) + 1
				set_quest_status( 3645, 1, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 21190106 then
			if get_quest_status( 3645 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 1 ) + 1
				set_quest_status( 3645, 1, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190008 then
			if get_quest_status( 3645 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 2 ) + 1
				set_quest_status( 3645, 2, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190035 then
			if get_quest_status( 3645 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 2 ) + 1
				set_quest_status( 3645, 2, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190062 then
			if get_quest_status( 3645 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 2 ) + 1
				set_quest_status( 3645, 2, A_day_A_quest )
			end
		elseif quest_progress3645 == 1 and monster_id == 20190089 then
			if get_quest_status( 3645 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3645 , 2 ) + 1
				set_quest_status( 3645, 2, A_day_A_quest )
			end
		end
		
	elseif monster_id == 20190026 or monster_id == 20190012 
	or monster_id == 20190053 or monster_id == 20190039 
	or monster_id == 20190080 or monster_id == 20190066 
	or monster_id == 21190107 or monster_id == 20190093 then
		
		local monster_count = get_instance_dungeon_flag( 42001, layer, 'check_open_prop')
		if monster_count == 2 then
			set_instance_dungeon_flag( 42001, layer, 'check_open_prop', 1)
		elseif monster_count == 1 then
			add_field_prop ( 60172, 60000, 61784, 31019, layer )
			set_instance_dungeon_flag( 42001, layer, 'check_open_prop', 0)
		end
		
		if quest_progress3646 == 1 and monster_id == 20190026 then
			if get_quest_status( 3646 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 1 ) + 1
				set_quest_status( 3646, 1, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190053 then
			if get_quest_status( 3646 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 1 ) + 1
				set_quest_status( 3646, 1, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190080 then
			if get_quest_status( 3646 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 1 ) + 1
				set_quest_status( 3646, 1, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 21190107 then
			if get_quest_status( 3646 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 1 ) + 1
				set_quest_status( 3646, 1, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190012 then
			if get_quest_status( 3646 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 2 ) + 1
				set_quest_status( 3646, 2, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190039 then
			if get_quest_status( 3646 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 2 ) + 1
				set_quest_status( 3646, 2, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190066 then
			if get_quest_status( 3646 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 2 ) + 1
				set_quest_status( 3646, 2, A_day_A_quest )
			end
		elseif quest_progress3646 == 1 and monster_id == 20190093 then
			if get_quest_status( 3646 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3646 , 2 ) + 1
				set_quest_status( 3646, 2, A_day_A_quest )
			end
		end
		
	elseif monster_id == 20190027 or monster_id == 20190010 
	or monster_id == 20190054 or monster_id == 20190037 
	or monster_id == 20190081 or monster_id == 20190064 
	or monster_id == 21190108 or monster_id == 20190091 then
	
		local monster_count = get_instance_dungeon_flag( 43001, layer, 'check_open_prop')
		if monster_count == 2 then
			set_instance_dungeon_flag( 43001, layer, 'check_open_prop', 1)
		elseif monster_count == 1 then
			add_field_prop ( 60173, 60000, 53659, 4147, layer )
			set_instance_dungeon_flag( 43001, layer, 'check_open_prop', 0)
		end
		
		if quest_progress3647 == 1 and monster_id == 20190027 then
			if get_quest_status( 3647 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 1 ) + 1
				set_quest_status( 3647, 1, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190054 then
			if get_quest_status( 3647 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 1 ) + 1
				set_quest_status( 3647, 1, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190081 then
			if get_quest_status( 3647 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 1 ) + 1
				set_quest_status( 3647, 1, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 21190108 then
			if get_quest_status( 3647 , 1 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 1 ) + 1
				set_quest_status( 3647, 1, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190010 then
			if get_quest_status( 3647 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 2 ) + 1
				set_quest_status( 3647, 2, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190037 then
			if get_quest_status( 3647 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 2 ) + 1
				set_quest_status( 3647, 2, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190064 then
			if get_quest_status( 3647 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 2 ) + 1
				set_quest_status( 3647, 2, A_day_A_quest )
			end
		elseif quest_progress3647 == 1 and monster_id == 20190091 then
			if get_quest_status( 3647 , 2 ) == 0 then
				local A_day_A_quest = get_quest_status( 3647 , 2 ) + 1
				set_quest_status( 3647, 2, A_day_A_quest )
			end
		end
		
	end
	
end

function Open_Chest( prop_id )

	local count = find_item( 601100310 )
	
	if prop_id == 60157 or prop_id == 60159 or prop_id == 60161 or prop_id == 60162 or prop_id == 60163 then
				
		if count >= 1 then
			-- 만족하면 열쇠 수거하고 
			delete_item( get_item_handle( 601100310 ), 1 )
		end
	end
	

end

function Open_main_Chest( prop_id )

	local index_num_1  = math.random ( 0, 2 )
		
	if prop_id == 60176 then -- 메두사의 투구가 나오는 프랍
		
		if index_num_1 == 0 then

			insert_item(601100355, 1)
		
		elseif index_num_1 == 1 then
		
			insert_item(601100355, 1)
			insert_item(601100355, 1)
		
		elseif index_num_1 == 2 then
		
			insert_item(601100355, 1)
			insert_item(601100355, 1)
			insert_item(601100355, 1)
		
		end
	
	elseif prop_id == 60179 then -- 블랙 위도우의 부츠가 나오는 프랍
	
		if index_num_1 == 0 then

			insert_item(601100357, 1)
		
		elseif index_num_1 == 1 then
		
			insert_item(601100357, 1)
			insert_item(601100357, 1)
		
		elseif index_num_1 == 2 then
		
			insert_item(601100357, 1)
			insert_item(601100357, 1)
			insert_item(601100357, 1)
		
		end
	
	elseif prop_id == 60182 then -- 본 드래곤의 아머가 나오는 프랍
	
		if index_num_1 == 0 then

			insert_item(math.random ( 601100358, 601100361 ), 1)
		
		elseif index_num_1 == 1 then
		
			insert_item(math.random ( 601100358, 601100361 ), 1)
			insert_item(math.random ( 601100358, 601100361 ), 1)
		
		elseif index_num_1 == 2 then
		
			insert_item(math.random ( 601100358, 601100361 ), 1)
			insert_item(math.random ( 601100358, 601100361 ), 1)
			insert_item(math.random ( 601100358, 601100361 ), 1)
		
		end

	elseif prop_id == 60185 then  -- 미크로랍토르의 글러브가 나오는 프랍
	
		if index_num_1 == 0 then

			insert_item(601100356, 1)
		
		elseif index_num_1 == 1 then
		
			insert_item(601100356, 1)
			insert_item(601100356, 1)
		
		elseif index_num_1 == 2 then
		
			insert_item(601100356, 1)
			insert_item(601100356, 1)
			insert_item(601100356, 1)
		
		end
	
	end
	
	cprint( "@690000089" )
	
end

function Call_Script_Witch_Death( monster_handle )

	local monster_id = get_monster_id( monster_handle )
	local quest_progress3648 = get_quest_progress(3648) -- [반복]지하 기지 소탕
	-- 반환값 -1 : 아무것도 아님  /  0 : 수락가  /  1 : 수행중  /  2 : 종료가능   / 100 : 실패  /  255 : 이미종료
	
	if monster_id == 20190002 or monster_id == 20190029 or monster_id == 20190056 or monster_id == 20190083 then  
	
		if quest_progress3648 == 1 then
			if get_quest_status( 3648 , 1 ) < 50 then
				local A_day_A_quest = get_quest_status( 3648 , 1 ) + 1
				set_quest_status( 3648, 1, A_day_A_quest )
			end
		end
	
	elseif monster_id == 20190003 or monster_id == 20190030 or monster_id == 20190057 or monster_id == 20190084 then
	
		if quest_progress3648 == 1 then
			if get_quest_status( 3648 , 2 ) < 50 then
				local A_day_A_quest = get_quest_status( 3648 , 2 ) + 1
				set_quest_status( 3648, 2, A_day_A_quest )
			end
		end
	
	elseif monster_id == 20190004 or monster_id == 20190031 or monster_id == 20190058 or monster_id == 20190085 then
	
		if quest_progress3648 == 1 then
			if get_quest_status( 3648 , 3 ) < 50 then
				local A_day_A_quest = get_quest_status( 3648 , 3 ) + 1
				set_quest_status( 3648, 3, A_day_A_quest )
			end
		end
		
	end
end

-- *add_instance_dungeon_monster( 몬스터리젠ID, 인던ID, 레이어번호): 해당 레이어 인던에 해당 몬스터 리젠 ID의 몬스터 그룹 생성*
-- 서커스 던전 내에 특정 몬스터 그룹이 전멸 했을 때 다음 던전 진행을 처리하기 위해 사용되는 스크립트
function circus_check_respawn_group_clear( monster_group, layer )
	-- monster_group 번호의 숫자를 세서 전멸이면 다음 던전 진행 처리를 한다. 
	local cnt = get_alive_instance_respawn_group_monster_count( 50000, layer, monster_group )
	
	-- 카운트 처리
	if monster_group == 1 then
		broadcast_mission_objective_progress( 1, 0, 10 - cnt, 50000, layer )
	elseif monster_group == 2 then
		broadcast_mission_objective_progress( 1, 0, 13 - cnt, 50000, layer )
	elseif monster_group == 3 then
		broadcast_mission_objective_progress( 1, 0, 13 - cnt, 50000, layer )
	elseif monster_group == 4 then
		broadcast_mission_objective_progress( 1, 0, 23 - cnt, 50000, layer )
	elseif monster_group == 5 then
		broadcast_mission_objective_progress( 1, 0, 19 - cnt, 50000, layer )
	end
	
	-- 전멸 처리
	if cnt == 0 then
				
		
		if monster_group == 1 then -- 전멸 그룹이 1번이면
			add_field_prop( 60186, 0, 38958, 137159, layer, 0, 0, 0, 1.5, 0.3, 0.3, 0.3 )	--작은방 입구 프랍 생성
			
			-- private_notice("@90606228")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606228", 50000, layer )
			-- 2번 그룹 리젠
			add_instance_dungeon_monster( 50023, 50000, layer )
			add_instance_dungeon_monster( 50024, 50000, layer )
			add_instance_dungeon_monster( 50025, 50000, layer )
			add_instance_dungeon_monster( 50026, 50000, layer )
			add_instance_dungeon_monster( 50027, 50000, layer )
			add_instance_dungeon_monster( 50028, 50000, layer )
			add_instance_dungeon_monster( 50029, 50000, layer )
			
			
			broadcast_mission_title( 1, '@9901', 50000, layer ) 
			broadcast_mission_objective( 1, 0, 13, '@1224', 50000, layer )

						
		elseif monster_group == 2 then -- 전멸 그룹이 2번이면
			add_field_prop( 60187, 0, 38836, 137150, layer, 0, 0, 0, -1.5, 0.3, 0.3, 0.3 )	--작은방 출구 프랍 생성
			
			-- private_notice("@90606229")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606229", 50000, layer )
			-- 3번 그룹 리젠
			add_instance_dungeon_monster( 50030, 50000, layer)
			add_instance_dungeon_monster( 50031, 50000, layer)
			add_instance_dungeon_monster( 50032, 50000, layer)
			add_instance_dungeon_monster( 50033, 50000, layer)
			add_instance_dungeon_monster( 50034, 50000, layer)
			add_instance_dungeon_monster( 50035, 50000, layer)
			
			broadcast_mission_title( 1, '@9902', 50000, layer ) 
			broadcast_mission_objective( 1, 0, 13, '@1224', 50000, layer )
						
		elseif monster_group == 3 then -- 전멸 그룹이 3번이면
			add_field_prop( 60188, 0, 40155, 137132, layer, 0, 0, 0, -1.5, 0.3, 0.3, 0.3 )	--중보스방 입구 프랍 생성
			
			-- private_notice("@90606230")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606230", 50000, layer )
			-- 4번 그룹 리젠
			add_instance_dungeon_monster( 50036, 50000, layer)
			add_instance_dungeon_monster( 50037, 50000, layer)
			add_instance_dungeon_monster( 50038, 50000, layer)
			add_instance_dungeon_monster( 50039, 50000, layer)
			add_instance_dungeon_monster( 50040, 50000, layer)
			add_instance_dungeon_monster( 50041, 50000, layer)
			add_instance_dungeon_monster( 50042, 50000, layer)
			add_instance_dungeon_monster( 50043, 50000, layer)
			add_instance_dungeon_monster( 50044, 50000, layer)
			add_instance_dungeon_monster( 50045, 50000, layer)
			add_instance_dungeon_monster( 50046, 50000, layer)
			add_instance_dungeon_monster( 50047, 50000, layer)
			add_instance_dungeon_monster( 50048, 50000, layer)
		
			broadcast_mission_title( 1, '@9903', 50000, layer ) 
			broadcast_mission_objective( 1, 0, 23, '@1224', 50000, layer )
		
		elseif monster_group == 4 then -- 전멸 그룹이 4번이면 		
			add_field_prop( 60189, 0, 40271, 136657, layer, 0, 0, 0, 1.5, 0.3, 0.3, 0.3 )	--중보스방 출구
			add_field_prop( 60190, 0, 39554, 136281, layer, 0, 0, 0, 0, 0.3, 0.3, 0.3 )		--보스방 입구
			
			-- private_notice("@90606231")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606231", 50000, layer )
			-- 5번 그룹 리젠
			add_instance_dungeon_monster( 50049, 50000, layer)
			add_instance_dungeon_monster( 50050, 50000, layer)
			add_instance_dungeon_monster( 50051, 50000, layer)
			add_instance_dungeon_monster( 50052, 50000, layer)
			add_instance_dungeon_monster( 50053, 50000, layer)
			add_instance_dungeon_monster( 50054, 50000, layer)
			add_instance_dungeon_monster( 50055, 50000, layer)
			add_instance_dungeon_monster( 50056, 50000, layer)
			add_instance_dungeon_monster( 50057, 50000, layer)
			
			
			broadcast_mission_title( 1, '@9904', 50000, layer ) 
			broadcast_mission_objective( 1, 0, 19, '@1224', 50000, layer )
			
		elseif monster_group == 5 then -- 전멸 그룹이 5번이면 (던전 클리어)
			add_field_prop( 60191, 0, 39552, 136341, layer, 0, 0, 0, 3, 0.3, 0.3, 0.3 )		--보스방 출구
			
			broadcast_mission_title( 1, '', 50000, layer )
			-- private_notice("@90606232")			-- 안내 메시지 출력
			broadcast_notice( 1, "@90606232", 50000, layer )
		end
	end
end

function on_join_circus( layer )
	
	local cnt_1 = get_alive_instance_respawn_group_monster_count( 50000, layer, 1 )
	local cnt_2 = get_alive_instance_respawn_group_monster_count( 50000, layer, 2 )
	local cnt_3 = get_alive_instance_respawn_group_monster_count( 50000, layer, 3 )
	local cnt_4 = get_alive_instance_respawn_group_monster_count( 50000, layer, 4 )
	local cnt_5 = get_alive_instance_respawn_group_monster_count( 50000, layer, 5 )

	-- 현재 미션 1 진행중
	if( cnt_1 > 0 ) then
		send_mission_title( '@9900' ) 
		send_mission_objective( 0, 10, '@1224' )
		send_mission_objective_progress( 0, 10 - cnt_1 )
	-- 현재 미션 2 진행중
	elseif( cnt_2 > 0 ) then
		send_mission_title( '@9901' ) 
		send_mission_objective( 0, 13, '@1224' )
		send_mission_objective_progress( 0, 13 - cnt_2 )
	-- 현재 미션 3 진행중
	elseif( cnt_3 > 0 ) then
		send_mission_title( '@9902' ) 
		send_mission_objective( 0, 13, '@1224' )
		send_mission_objective_progress( 0, 13 - cnt_3 )
	-- 현재 미션 4 진행중
	elseif( cnt_4 > 0 ) then
		send_mission_title( '@9903' ) 
		send_mission_objective( 0, 23, '@1224' )
		send_mission_objective_progress( 0, 23 - cnt_4 )
	-- 현재 미션 5 진행중
	elseif( cnt_5 > 0 ) then
		send_mission_title( '@9904' ) 
		send_mission_objective( 0, 19, '@1224' )
		send_mission_objective_progress( 0, 19 - cnt_5 )
	end
end

-- 서버 시작할 때 스크립트를 통해 추가 할 프랍들이 있으면 여기에 추가한다.
function add_global_prop()

	-- 칠흑의숲 -> 붉은 거미 서커스 워프 프랍
	add_field_prop( 60192, 0, 163100, 116110, 0, 0, 0 ,0, 0.45 , 1, 1, 1)
	add_field_prop( 60193, 0, 35527, 121011, 0, 0, 0 ,0, 1.5 , 1, 1, 1)


end