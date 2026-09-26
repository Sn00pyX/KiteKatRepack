-- Lua 스크립트 암호화
function get_module_name()
             return "ETC_Script_Functions"
end

--get_server_category() 함수와 같이 사용 (서버제공 함수)
function scf_get_server_index( category )

	-- 서버 인텍스 번호
	 
	--		정식 서비스 서버	테스트/개발 서버
	--	1	레이븐				스켈파이터
	--	2	아발란체			QA서버
	--	3	세이렌				!미소X정빈서버! (--;;)
	--	4	샐러맨더
	--	5	라이디언

	-- 7과 AND 연산 시켰을 때 나오는 수가 서버 인덱스 번호
	local bin01 = math.mod( category, 2 )
	local bin02 = math.mod( math.floor( category / 2 ), 2 ) * 2
	local bin03 = math.mod( math.floor( category / 4 ), 2 ) * 4
	
	return (bin01 + bin02 + bin03)

end


function scf_is_pk_server( category )

	-- 8과 AND 연산 시켰을 때 0 이면 일반서버, 8이면 PK 서버
	if math.mod( math.floor( category / 268435456 ), 2 ) == 1 then
		return true
	else
		return false
	end

end


function scf_is_official_server( category )

	-- 16과 AND 연산 시켰을 때 0 이면 테스트서버, 16이면 정식 서버
	if math.mod( math.floor( category / 536870912 ), 2 ) == 1 then
		return true
	else
		return false
	end

end

--=========================================================================
-- 프리미엄 서비스 체크
--=========================================================================
function is_premium()
local everybody = tonumber(get_env("game.premium_for_everybody")) == 1
	if everybody then return true end

	-- 국가 코드 읽어오기
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_GERMANY                    = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_FRANCE                     = 128
		--LOCAL_INFO_RUSSIA                     = 256
		--말레이시아 512, 싱가폴 1024, 베트남 2048, 태국 4096, 중동 8192, 터키 16384	
		
	local state_code = get_local_info()	
	
	-- 한국의 경우 본섭/테섭 모두 시크루트 프리패스가 있으면 true
	if state_code == 1 and gv( "premium" ) == 1 then
		return true
	end
	-- 한국 이외 국가 시크루트 프리패스가 있으면 true
	if gv( "premium" ) == 1 and scf_is_official_server( get_server_category() ) then
		return true
	end
	
	-- 테스트 서버이면 무조건 true
	if scf_is_official_server( get_server_category() ) == false then
	--	return true	
	end
	
	return false

end

--=========================================================================
-- 2차 전직 가능 체크
--=========================================================================
function is_able_to_jobchange()

	if get_value( "job_depth" ) == 0 then
		--if get_value( "level" ) < 10 or get_value( "job_level" ) < 10 then
		if get_value( "job_level" ) < 10 then
			return false
		end
	elseif get_value( "job_depth" ) == 1 then
		--if get_value( "level" ) < 50 or get_value( "job_level" ) < 40 then
		if get_value( "job_level" ) < 40 then
			return false
		end
	elseif get_value( "job_depth" ) == 2 then
		if get_value( "level" ) < 147 or get_value( "job_level" ) < 49 then
			return false
		end
	else
		return false
	end
	
	-- 1차 직업이고 전직이 가능한 조건일 때 테스트 서버는 전직 가능하도록 true 반환
	if ( get_value( "job_depth" ) == 1 ) then
		return true
	elseif ( get_value( "job_depth" ) == 2 ) then
		return true
	-- 본섭은 불가.
	else
		return false
	end
	
	return true

end

--=========================================================================
-- 이상한 곳으로 쫓아내기 (오토를 무저갱으로)
--=========================================================================
function kick_auto_to_another_world()

	if get_value("auto_user") == 1 then
		-- 귀환지역을 외딴 곳으로 설정 (테스트용 프랍 박은 구 던전, 11-7)
    	if (gv('rx') == nil or gv('rx') == "") and (gv('ry') == nil or gv('ry') == "") then -- 해외
			set_flag( "rx", 186987 + math.random(0,10))
			set_flag( "ry", 160009 + math.random(0,10))
		else -- 국내
			sv('rx', 186987 + math.random(0,10))
			sv('ry', 160009 + math.random(0,10))
		end
    
    	-- 외딴 곳으로 날려 버리기
    	warp( 186987 + math.random(0,100) , 160009 + math.random(0,100) )
		open_popup("game.helpdesk_url", 1, 0)
    end
    
end

function RunTeleport_Auto_TO_City( x_pos , y_pos )

	-- 귀환지역을 대상도시로 설정
    set_flag( "rx", x_pos + math.random(0,100))
    set_flag( "ry", y_pos + math.random(0,100))

	save()

	-- 상위 펑션에서 호출한 x, y좌표에 +100을 랜덤값으로 텔레포트함
	warp( x_pos + math.random(0,100) , y_pos + math.random(0,100) )

end


--=========================================================================
-- 오토 방지용 퀘스트 체크 함수
--=========================================================================
function anti_auto_quest_check()

	-- 국가 코드 읽어오기
	-- 국가 코드 읽어오기
		-- get_local_info()의 반환값들
		--LOCAL_INFO_KOREA                      = 1
		--LOCAL_INFO_HONGKONG                   = 2
		--LOCAL_INFO_AMERICA                    = 4
		--LOCAL_INFO_GERMANY                    = 8
		--LOCAL_INFO_JAPAN                      = 16
		--LOCAL_INFO_TAIWAN                     = 32
		--LOCAL_INFO_CHINA                      = 64
		--LOCAL_INFO_FRANCE                     = 128
		--LOCAL_INFO_RUSSIA                     = 256
		--말레이시아 512, 싱가폴 1024, 베트남 2048, 태국 4096, 중동 8192, 터키 16384	
		
	local state_code = get_local_info()	
	
	local is_auto_char = false
	local is_tranning_camp = false
	local quest_clear_count = 0
	
	local current_level = gv("level")

	
	-- 테섭이면 그냥 무시
	if scf_is_official_server( get_server_category() ) then
	else
		return quest_clear_count, is_auto_char
	end


	-- 미국이면 다 무시.
	if state_code == 4 or state_code == 512 or state_code == 4096 then
		return quest_clear_count, is_auto_char
	end

	--현재 게임 내에 존재하는 40레벨 미만의 퀘스트들에 대한 수행 여부 체크
	if ( get_quest_progress(1005) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1006) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1007) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1008) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1009) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1010) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1011) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1012) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1013) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1014) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1015) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1016) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1017) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1018) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1019) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1020) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1021) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1022) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1023) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1024) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1033) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1037) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1038) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1030) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1031) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1032) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1043) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1044) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1025) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1026) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1027) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1028) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1029) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1034) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1035) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1036) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1039) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1040) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1041) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1042) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1045) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1049) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1063) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1053) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1102) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1103) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1104) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1149) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1129) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1131) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1132) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1139) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1140) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1150) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1151) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1152) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1153) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1154) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1155) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1156) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1157) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1158) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1046) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1054) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1050) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1130) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1047) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1051) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1055) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1142) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1143) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1141) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1159) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1100) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1094) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1101) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1146) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1147) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1148) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1109) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1048) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1052) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1056) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1057) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1061) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1095) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1136) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1137) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1138) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1144) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1145) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1133) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1064) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1065) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1068) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1071) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1072) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1073) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1081) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1116) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1117) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1058) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1059) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1096) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1069) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1110) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1111) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1112) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1134) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
		
	if ( get_quest_progress(1135) ) == 255 then	
		quest_clear_count = quest_clear_count + 1
	end	
 
	-- 수련자의 섬 안에 있을 때
	local current_x = gv("x")
	local current_y = gv("y")
 
	if current_x >= 161280 and current_x <= 177408 then
		if current_y >= 48384 and current_y <= 64512 then
			is_tranning_camp = true
		end
	end
 
	-- 수련자의 섬에서 오토 쫓아내기
	if is_tranning_camp then
	
		-- 레벨에 대한 퀘스트 체크
  
	  
		-- 레벨이 8~11 일 때 클리어한 퀘스트가 3개 미만이면
		if current_level >= 8 and current_level <= 11 then
			if quest_clear_count < 3 then
				is_auto_char = true
			end
		end
		  
		-- 레벨이 12~17 일 때 클리어한 퀘스트가 6개 미만이면
		if current_level >= 12 and current_level <= 17 then
			if quest_clear_count < 6 then
				is_auto_char = true
			end
		end
		  
		-- 레벨이 18 이상이면
		if current_level >= 18 then
			if quest_clear_count < 7 then
			    is_auto_char = true
		   end
		end
 
	-- 수련자의 섬에서 오토 쫓아내기 if 완료
	end

	return quest_clear_count, is_auto_char
 
end

