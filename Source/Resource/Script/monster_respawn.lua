-- Lua 스크립트 암호화.
function get_module_name()
             return "monster_respawn"
end

function mob( ID, left, top, right, bottom )
	-- 이벤트 박스에서 리스폰 ID를 호출해준다.

	local monster_ID = { 0, 0, 0, 0, 0, 0 }   	-- 표준몹 ID  (디폴트로 1번몹이 나옴)
	local density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }	-- 표준몹 밀도
	local interval = { 200, 300, 400, 500, 1000, 1500 }	-- 표준 리스폰 체크 간격 (1/100초)
	
	local Raremob_ID = { 0, 0, 0, 0 }   	-- 레어몹 ID
	local Raremob_count = { 1, 1, 1, 1 }   	-- 레어몹 개체수 (기본 1마리)
	local Raremob_interval = { 30000, 60000, 180000, 360000 }  	-- 레어몹 출현 체크 빈도 (5분, 10분, 30분, 60분 )

	local Raidmob_ID = { 0, 0, 0, 0, 0, 0 }   	-- 표준몹 ID  (디폴트로 1번몹이 나옴)
	local Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }	-- 표준몹 밀도
	local Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	-- 표준 리스폰 체크 간격 (1/100초)

		
	local Raid_Raremob_ID = { 0, 0, 0, 0 }   	-- 레어몹 ID
	local Raid_Raremob_count = { 1, 1, 1, 1 }   	-- 레어몹 개체수 (기본 1마리)
	local Raid_Raremob_interval = { 30000, 60000, 180000, 360000 }  	-- 레어몹 출현 체크 빈도 (5분, 10분, 30분, 60분 )

	local i, size, max_num, max_respawn_once, respawn_type_count


	-- 일본/한국에서 할로윈 던전에서 젖소와 팬더옷 드랍 안하는 몬스터로 바꿔치기.
	
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
	local other_drop = 0
	
	-- 한국과 일본일 때만 다른 드랍 몬스터로 세팅하도록 변수 설정.
	if state_code == 1 or state_code == 16 then
		-- 전국가 공통 적용으로 0으로 세팅함 2011.10.11
		-- 기본은 1(일본 한국은 다른 몬스터 세팅)
		other_drop = 0
	end
	
	-- 할로윈 이벤트 몬스터 적용 여부 결정
	-- 몬스터 적용 시 1/제거 시 0로 비트셋처럼 적용
	local halloween_on = 1
	
	-- 크리스마스 이벤트 몬스터 적용 여부 결정
	-- 이벤트 적용 시 1/제거 시 0로 비트셋처럼 적용
	local rangifer_on = 0
	
	
	-- 리스폰 ID에 따라 어떤 몬스터를 리스폰시킬지를 정의한다. (최대 6종)
	-- 밀도와 인터벌은 위에 정의된 것을 디폴트로 쓴다. 다른 밀도를 쓰려면 테이블에 새 값을 할당해주면 된다.
	-- 레어몹 출현이 있을경우도 새 값을 할당해주면 된다.

	
	--=======================================================================
	-- 초보자섬
	--=======================================================================
	
	if ID == 11 then	-- 가이아 시작지점1
		monster_ID = { 1002, 2002, 2002, 0, 0, 0 }
		density = { 1.2, 0.7, 0.4, 0.15, 0.1, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }

		Raremob_ID = { 5044, 5046, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 21 then-- 가이아 시작지점2 (중간공터 & 진행루트)
		monster_ID = { 2002, 3003, 1002, 0, 0, 0 }
		density = { 1.2, 0.7, 0.4, 0.15, 0.1, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }

		Raremob_ID = { 5044, 5046, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 12 then-- 데바 시작지점1
		monster_ID = { 1003, 2003, 2003, 0, 0, 0 }
		density = { 1.2, 0.7, 0.4, 0.15, 0.1, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }

		Raremob_ID = { 5049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 22 then-- 데바 시작지점2 (다리건너편)
		monster_ID = { 2003, 3004, 1003, 0, 0, 0 }
		density = { 1.2, 0.7, 0.4, 0.15, 0.1, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }

		Raremob_ID = { 5049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 13 then-- 아수라 시작지점1
		monster_ID = { 1004, 2004, 2004, 0, 0, 0 }
		density = { 1.2, 0.7, 0.4, 0.15, 0.1, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }
				
	elseif ID == 23 then-- 아수라 시작지점2 (중간공터)
		monster_ID = { 2004, 3005, 1004, 0, 0, 0 }
		density = { 1.2, 0.7, 0.4, 0.15, 0.1, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }
			
		Raremob_ID = { 5041, 5043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 42 then-- 초급교관 주변지역
		monster_ID = { 4002, 3006, 2002, 3003, 0, 0 }
		density = { 1.0, 0.5, 0.4, 0.4, 0.1, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }

		Raremob_ID = { 5049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	------------------------------------------------------------------------------------
	
	elseif ID == 52 then-- 캠프 남쪽지역
		monster_ID = { 5005, 5004, 6004, 4002, 0, 0 }
		density = { 0.8, 0.5, 0.4, 0.3, 0.2, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }
		
		Raremob_ID = { 5049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 72 then-- 캠프 동쪽지역
		monster_ID = { 6005, 7005, 7004, 6003, 5004, 7007 }
		density = { 0.8, 0.5, 0.4, 0.3, 0.2, 0.4 }
		interval = { 100, 100, 100, 100, 100, 100 }

		Raremob_ID = { 5049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 82 then-- 캠프 북쪽지역
		monster_ID = { 8002, 7007, 7005, 9002, 6003, 7006 }
		density = { 0.8, 0.6, 0.4, 0.3, 0.2, 0.4 }
		interval = { 100, 100, 100, 100, 100, 100 }

		Raremob_ID = { 5049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	------------------------------------------------------------------------------------
		
	elseif ID == 102 then-- 행자목림
		monster_ID = { 10006, 9003, 11005, 12007, 11007, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.2, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }

		Raremob_ID = { 10049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 112 then-- 행자목림 안쪽
		monster_ID = { 11004, 14004, 11005, 12006,  13006, 12007 }
		density = { 0.6, 0.6, 0.6, 0.5, 0.3, 0.2 }
		interval = { 100, 100, 100, 100, 100, 100 }
		
		Raremob_ID = { 10049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 113 then-- 바위산 아나테마 출몰지
		monster_ID = { 0, 0, 14004, 0,  0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.2, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }
		
		Raremob_ID = { 10003, 10049, 0, 0 }
		Raremob_count = { 4, 1, 1, 1 }
		Raremob_interval = { 500, 100, 100, 0 }

	elseif ID == 114 then-- 바위산 아나테마 출몰지2
		monster_ID = { 0, 0, 14004, 0,  0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.2, 0.07 }
		interval = { 100, 100, 100, 100, 100, 100 }
		
		Raremob_ID = { 10003, 10049, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 30000, 100, 100, 0 }
		
		
	elseif ID == 142 then-- 열사의 해안 
		monster_ID = { 14005, 15008, 0, 0, 0, 0 }
        density = { 0.5, 0.3, 0.3, 0.15, 0.1, 0.07 }
        interval = { 100, 100, 100, 100, 100, 100 }
		
        Raremob_ID = { 15049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 0, 0, 0 }
		
	elseif ID == 143 then-- 열사의 해안 (우두머리 출몰)
		monster_ID = { 14005, 15008, 0, 0, 0, 0 }
        density = { 0.5, 0.3, 0.3, 0.15, 0.1, 0.07 }
        interval = { 100, 100, 100, 100, 100, 100 }
		
        Raremob_ID = { 15009, 15049, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 6000, 100, 0, 0 }
		        	
	------------------------------------------------------------------------------------
	-- 켄타지역
	
	elseif ID == 122 then-- 켄타 계곡 입구
		monster_ID = { 12006, 11006, 0, 0, 12007, 0 }
		density = { 0.5, 0.5, 0.5, 0.4, 0.2, 0.4 }
		interval = { 100, 100, 100, 100, 100, 100 }
		
		Raremob_ID = { 10049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
    elseif ID == 132 then-- 검은 천국
		monster_ID = { 13010, 14007, 15010, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 300, 300, 300, 300, 300, 300 }

		Raremob_ID = { 10049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 152 then-- 켄타계곡 입구2
		monster_ID = { 11006, 13007, 13008, 0, 0, 0 }
		density = { 0.5, 0.4, 0.3, 0.2, 0.3, 0.2 }
		interval = { 300, 300, 300, 300, 300, 300 }

		Raremob_ID = { 10049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 153 then-- 켄타계곡 입구3
		monster_ID = { 13007, 14004, 13006, 0, 0, 0 }
		density = { 0.5, 0.4, 0.3, 0.2, 0.3, 0.2 }
		interval = { 300, 300, 300, 300, 300, 300 }

		Raremob_ID = { 10049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 154 then-- 켄타계곡
		monster_ID = { 14004, 15002, 0, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.1, 0.2 }
		interval = { 300, 300, 300, 300, 300, 300 }

		Raremob_ID = { 10049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 155 then-- 데스메이트 출몰지
		monster_ID = { 14004, 0, 0, 0, 0, 0 }
		density = { 0.5, 0.4, 0.4, 0.3, 0.3, 0.2 }
		interval = { 300, 300, 300, 300, 300, 300 }

		Raremob_ID = { 15011, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 6000, 400, 0, 0 }

	------------------------------------------------------------------------------------
	-- 나비스라미아 
	
	elseif ID == 162 then-- 갑판
		monster_ID = { 14008, 14008, 0, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1000, 1000, 1000, 1000, 1000, 1000 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 163 then-- 조타실
		monster_ID = { 15013, 14008, 13011, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 164 then-- 주방
		monster_ID = { 14009, 13012, 15013, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 165 then-- 주방 보스
		monster_ID = { 0, 0, 0, 0, 14009, 13012 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500 }

		Raremob_ID = { 15012, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 2000, 100, 0, 0 }
		
	elseif ID == 172 then-- 잡화 창고
		monster_ID = { 15015, 14010, 13011, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1000, 1000, 1000, 1000, 1000, 1000 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 173 then-- 객실1
		monster_ID = { 16007, 13011, 13012, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1000, 1000, 1000, 1000, 1000, 1000 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 174 then-- 식료품 창고
		monster_ID = { 16009, 15014, 16007, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1000, 1000, 1000, 1000, 1000, 1000 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 175 then-- 객실2
		monster_ID = { 17011, 16008, 14009, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1000, 1000, 1000, 1000, 1000, 1000 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 176 then-- 생체실험 준비실
		monster_ID = { 17012, 15013, 17013, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 1000, 1000, 1000, 1000, 1000, 1000 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 182 then-- 선장실 입구
		monster_ID = { 0, 0, 0, 0, 16008, 14009 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.1, 0.1 }
		interval = { 2000, 2000, 2000, 2000, 2000, 2000 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }
		
	elseif ID == 183 then-- 선장실
		monster_ID = { 0, 0, 0, 17011, 17013, 15013 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 2000, 2000, 2000, 2000, 2000, 2000 }

		Raremob_ID = { 0, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 400, 400, 0, 0 }

	elseif ID == 184 then-- 선장실 보스
		monster_ID = { 0, 0, 0, 0, 0, 0 }
		density = { 0.6, 0.5, 0.4, 0.3, 0.3, 0.2 }
		interval = { 2000, 2000, 2000, 2000, 2000, 2000 }

		Raremob_ID = { 17010, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 6000, 400, 0, 0 }
		        	
	--크리처 아이무스 배치를 위해 추가
	elseif ID == 35034 then		
		monster_ID = {35034,36027,0,0,0,0}
		density = { 0.7, 0.6, 0.1, 0.1, 0.1, 0.1 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = {37029,0,0,0,0,0}
		Raidmob_density = { 0.1, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	--=======================================================================
	-- 라크시 필드	
	--=======================================================================
	elseif ID == 101013	 then
		monster_ID = { 13009, 	14001, 	15001, 	16001, 	17001, 	18001 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 400, 600, 400, 500, 600, 700 }

        Raremob_ID = { 15049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 102019	 then
		monster_ID = { 19001, 	20001, 	21001, 	22001, 	23001, 	24001 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 400, 600, 800, 1000, 1200, 1500 }
		
        Raremob_ID = { 20049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
	
	elseif ID == 103024	 then
		monster_ID = {24002, 	25001, 	26001, 	27001, 	28001, 	29001 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 400, 600, 800, 1000, 1200, 1500 }
		
		Raremob_ID = { 29002, 25049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1500, 100, 100, 0 }
		
	elseif ID == 104022	 then
		monster_ID = {22002, 	23002, 	24003, 	25002, 	26002, 	27002 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 400, 600, 800, 1000, 1200, 1500 }

		Raremob_ID = { 25049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 105025	 then
		monster_ID = {25003, 	26003, 	27003, 	28002, 	29003, 	30001 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 400, 600, 800, 1000, 1200, 1500 }

		Raremob_ID = { 30049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 106015	 then
		monster_ID = {15002, 	16002, 	17002, 	18002,	19002,	20002 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 400, 600, 400, 500, 600, 700 }

        Raremob_ID = { 20049, 0, 0, 0 }
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 100, 100, 0, 0 }
	
	elseif ID == 107028	 then
		monster_ID = {28003, 	29004, 	30002, 	31001, 	32001, 0 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 400, 600, 800, 1000, 1200, 1500 }

		Raremob_ID = { 30049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 108032	 then
		monster_ID = {32002, 	33001, 	34001, 	35001, 	36001, 	37001 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 800, 1000, 1200, 1400, 1600, 2000 }

		Raremob_ID = { 37002, 35049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 0 }
		
	elseif ID == 109035	 then
		monster_ID = {35002, 	36002, 	37003, 	38001, 	39001, 	40001 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 800, 1000, 1200, 1400, 1600, 2000 }

		Raremob_ID = { 40049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 110038	 then
		monster_ID = {38002, 	39002, 	40002, 	41001, 	42001, 	43002 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 800, 1000, 1200, 1400, 1600, 2000 }

		Raremob_ID = { 40049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 111028	 then
		monster_ID = {28004, 	29005,	30003, 	31002, 	32003, 	33002 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 800, 1000, 600, 700, 800, 1000 }

		Raremob_ID = { 30049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 112030	 then
		monster_ID = {30004, 	31003,	32004, 	33003, 	34002, 	35003 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 800, 1000, 600, 700, 800, 2000 }

		Raremob_ID = { 35004, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 0, 0 }
		
	elseif ID == 113031	 then
		monster_ID = {31004, 	32005, 	33004, 	34003, 	35005, 	36003 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 800, 1000, 1200, 1400, 1600, 2000 }

		Raremob_ID = { 35049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 114035	 then
		monster_ID = {35006, 	36004, 	37004, 	38003, 	39003, 	40003 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 800, 1000, 1200, 1400, 1600, 1000 }

		Raremob_ID = { 40049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 115060	 then
		monster_ID = {60007, 	61007, 	62007, 	63006, 	64005,	65005 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1400, 1500, 1600 }
		
		Raremob_ID = { 65049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 116041	 then
		monster_ID = {41002, 	42002, 	43003, 	44004, 	45005, 	46007 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 600, 700, 700, 800 }
		
		Raremob_ID = { 46008, 45049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 0 }
		
	elseif ID == 117045	 then
		monster_ID = {45006, 	46009, 	47006, 	48005, 	49005, 	50005 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1400, 700, 800 }
		
		Raremob_ID = { 50049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 118040	 then
		monster_ID = {40004, 	41003, 	42003, 	43004, 	44005, 	45007 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1400, 700, 800 }
		
		Raremob_ID = { 45049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 119035	 then
		monster_ID = {35007, 	36005,	37005,	38004, 	39004, 0 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 800, 1000, 600, 700, 800, 2000 }

		Raremob_ID = { 39005, 35049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 0 }
		
	elseif ID == 120039	 then
		monster_ID = {39006, 	40005, 	41004, 	42004, 	43005, 	44006 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 600, 700, 700, 800 }
		
		Raremob_ID = { 40049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 121040	 then
		monster_ID = {40006, 41005, 42005, 43006, 41005, 42005, 43006 }
		density = { 0.50, 0.50, 0.50, 0.50, 0.50, 0.50, 0.50} 
		interval = { 500, 500, 500, 500, 500, 500, 500 }
		
		Raremob_ID = { 40049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 122043	 then
		monster_ID = {43007, 	44007, 	45008, 	46010, 	47007, 	48006 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 700, 700, 800 }
		
		Raremob_ID = { 45049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 123040	 then
		monster_ID = {40007, 	41006, 	42006, 	43008, 	44008, 	45009 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1400, 1500, 1600 }
		
		Raremob_ID = { 45049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 124042	 then
		monster_ID = {42007, 	43009, 	44009, 	45010, 	46011, 	47008 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 700, 1500, 800 }
		
		Raremob_ID = { 45049, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 0, 0 }
		
	elseif ID == 125045	 then
		monster_ID = {45011, 46012, 47009, 48007, 49006, 50006, 49006, 50006 }
		density = { 0.50, 0.30, 0.20, 0.20, 0.20, 0.07, 0.20, 0.07 } 
		interval = { 1000, 1000, 1200, 1200, 600, 800, 600, 800 }
		
		Raremob_ID = { 50007, 50049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1600, 100, 100, 0 }
		
	elseif ID == 126050	 then
		monster_ID = {50008, 	51005, 	52004, 	53006, 	54005, 	55006 }
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1400, 1500, 1600 }
		
		Raremob_ID = { 55007, 55049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1600, 100, 100, 0 }
				
	--=======================================================================
	-- 카탄 필드	
	--=======================================================================
	elseif ID == 201013	 then
		monster_ID = { 13001, 14002, 15004, 16003, 	17004, 0 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 400, 600, 800, 1000, 1200, 1500 }
				
		Raremob_ID = { 15041, 15043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 202019	 then
		monster_ID = { 19003, 20003, 21002,	22003, 	23004, 	24004 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 400, 600, 800, 1000, 1200, 700 }
				
		Raremob_ID = { 20041, 20043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 203024	 then
		monster_ID = { 24005, 25004, 26004, 27004, 	28005, 0 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 400, 600, 800, 1000, 600, 1500 }
				
		Raremob_ID = { 25041, 25043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 227013	 then
		monster_ID = { 13002, 14003, 15005,	16004, 	17006, 	18003, 16004, 17006 , 18003}
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 , 0.2, 0.15, 0.1}
		interval = { 400, 300, 400, 500, 600, 700 , 500, 600, 700}
				
		Raremob_ID = { 15041, 15043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 204019	 then
		monster_ID = { 19004, 20004, 21003,	22004, 	23005, 	24006 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 400, 600, 800, 1000, 600, 700 }
				
		Raremob_ID = { 20041, 20043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 205028	 then
		monster_ID = { 28006, 29006, 30005,	31005, 	32006, 	33005 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 400, 600, 800, 1000, 1200, 1500 }
				
		Raremob_ID = { 30041, 30043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 206025	 then
		monster_ID = { 25005, 26005, 27005,	28007, 	29007, 	30006, 	29007, 	30006 }
		density = { 0.54, 0.36, 0.27, 0.27, 0.27, 0.1, 0.27, 0.1 }
		interval = { 400, 600, 400, 400, 400, 400, 400, 400 }
		
		Raremob_ID = { 30007, 30041, 30043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 207025	 then
		monster_ID = { 25006, 26006, 27006,	28008, 	29008, 	30009 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 400, 600, 800, 1000, 1200, 1500 }
		
		Raremob_ID = { 28009, 30041, 30043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1000, 100, 100, 100 }
		
	elseif ID == 208020	 then
		monster_ID = { 20005, 21004, 22005, 23006, 	24007, 	25007 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 400, 600, 800, 1000, 1200, 1500 }
				
		Raremob_ID = { 25041, 25043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 209031	 then
		monster_ID = { 31006, 32007, 33006,	34004, 	35008, 	36006 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 500, 600, 800, 1000, 1000, 600 }
				
		Raremob_ID = { 35041, 35043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 210035	 then
		monster_ID = { 35009, 36007, 37006,  38005,  39007, 40008, 40008 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1, 0.1 }
		interval = { 500, 600, 800, 500, 500, 600, 600 }
		
		Raremob_ID = { 40009, 40041, 40043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 500, 100, 100, 100 }
		
	elseif ID == 211029	 then
		monster_ID = { 29009, 30010, 31007, 32008, 	33007, 	34005 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 500, 600, 800, 1000, 1000, 1200 }
				
		Raremob_ID = { 30041, 30043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 212032	 then
		monster_ID = { 32009, 33008, 34006,  35010,	36008, 	37007 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 500, 600, 800, 1000, 1000, 1200 }
				
		Raremob_ID = { 35041, 35043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 214024	 then
		monster_ID = { 24008, 25008, 26007, 27007, 	28010, 	29010 }
		density = { 0.54, 0.36, 0.27, 0.27, 0.15, 0.1 }
		interval = { 400, 600, 400, 400, 600, 700 }
				
		Raremob_ID = { 35041, 35043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 215036	 then
		monster_ID = { 36009, 37008, 38006,	39008, 	40010, 	41007, 	40010, 	41007 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1, 0.15, 0.1 }
		interval = { 500, 600, 400, 500, 500, 600, 500, 600 }
				
		Raremob_ID = { 40041, 40043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 218030	 then
		monster_ID = { 30011, 31008, 32010,	33009, 	34007, 	35011, 32010,	33009, 	34007, 	35011 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1, 0.27, 0.2, 0.15, 0.1 }
		interval = { 200, 300, 400, 500, 500, 600, 400, 500, 500, 600 }
				
		Raremob_ID = { 35041, 35043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 216040	 then
		monster_ID = { 40011, 41012, 42013,	43010, 	44010, 	45012 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 1000, 500 }
				
		Raremob_ID = { 45041, 45043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 217035	 then
		monster_ID = { 35012, 36011, 37009,	38007, 	39009, 	40012 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 500, 600, 800, 500, 500, 600 }
		
		Raremob_ID = { 37010, 35041, 40043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 219045	 then
		monster_ID = { 45013, 46013, 47010,	48008, 	49007, 	50009 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 500, 500 }
				
		Raremob_ID = { 50041, 50043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 220040	 then
		monster_ID = { 40013, 41013, 42014,	43011, 	44011, 	45014 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 1000, 1000 }
				
		Raremob_ID = { 45041, 45043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 221036	 then
		monster_ID = { 36012, 37011, 38008,	39010, 	40014, 	41014 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 500, 600, 800, 1000, 1000, 1200 }
				
		Raremob_ID = { 40041, 40043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 222032	 then
		monster_ID = { 32011, 33010, 34008,	35013, 	36013, 	37012 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 500, 600, 800, 1000, 1000, 1200 }
		
		Raremob_ID = { 37013, 35041, 35043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 223038	 then
		monster_ID = {38009, 39011, 40015, 41015,	42015, 	43012}
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 500, 600, 800, 1000, 1000, 600 }
				
		Raremob_ID = { 40041, 40043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 224040	 then
		monster_ID = {40016, 41016,	42016, 	43013, 	44012, 	45015, 	44012, 	45015 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1, 0.15, 0.1 }
		interval = { 600, 400, 500, 500, 500, 500, 500, 500 }
		
		Raremob_ID = { 45016, 45041, 45043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1000, 100, 100, 100 }
		
	elseif ID == 225044	 then
		monster_ID = {44013, 45017,	46014,	47011, 	48009, 	49008 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 500, 500 }
				
		Raremob_ID = { 45041, 45043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 226049	 then
		monster_ID = {49009, 50010,	51006, 	52005, 	53007, 	54006, 	54006 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1, 0.1 }
		interval = { 600, 800, 1000, 500, 500, 500, 500 }
				
		Raremob_ID = { 50041, 50043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 228026	 then
		monster_ID = {26015, 27014, 28017, 29016, 30018, 31013 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 1000, 1000 }
				
		Raremob_ID = { 50041, 50043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 229029	 then
		monster_ID = {29017, 30019, 31014, 32016, 33014, 34013 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 1000, 1000 }
				
		Raremob_ID = { 30041, 30043, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 230031	 then
		monster_ID = {31015, 32017, 33015, 34014, 35019, 36022, 34014, 35019, 36022 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1, 0.2, 0.15, 0.1 }
		interval = { 300, 400, 500, 500, 500, 500, 500, 500, 500 }
				
		Raremob_ID = { 37013, 35041, 35043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }

	elseif ID == 231034	 then
		monster_ID = {34015, 35020, 36023, 37025, 38024, 39024 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 1000, 1000 }
				
		Raremob_ID = { 39025, 37013, 35041, 35043 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }

	elseif ID == 232038	 then
		monster_ID = {39026, 40028, 41022, 42021, 43017, 44018 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 1000, 1000 }
				
		Raremob_ID = { 45016, 45041, 45043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1000, 100, 100, 100 }

	elseif ID == 233032	 then
		monster_ID = {32033, 35035, 0, 0, 0, 0 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 1000, 1000 }
				
		Raremob_ID = { 37016, 35041, 35043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1000, 100, 100, 100 }	
		
	elseif ID == 234038	 then
		monster_ID = {38026, 43018, 0, 0, 0, 0 }
		density = { 0.54, 0.36, 0.27, 0.2, 0.15, 0.1 }
		interval = { 600, 800, 1000, 1000, 1000, 1000 }
				
		Raremob_ID = { 45016, 45041, 45043, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1000, 100, 100, 100 }

	--=======================================================================
	-- 호라이즌 필드	
	--=======================================================================
	elseif ID == 301013	 then
		monster_ID = { 13003, 	14006, 	15006, 	16005, 	17007, 	18004, 16005 , 17007, 18004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 , 0.1, 0.07, 0.01} 
		interval = { 500, 700, 500, 700, 900, 1100 , 700, 900, 1100}
		
		Raremob_ID = { 15044, 15046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 302017	 then
		monster_ID = { 17008, 	18005, 	19005, 	20006, 	21005, 	22006 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 500, 700, 1000, 1500, 1800, 2200 }
		
		Raremob_ID = { 19006, 20044, 20046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 303015	 then
		monster_ID = { 15007, 	16006, 	17009, 	18006, 	19007, 	20007, 19007, 20007 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.07, 0.01 } 
		interval = { 500, 700, 1000, 700, 900, 1100, 900, 1100 }
		
		Raremob_ID = { 20044, 20046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 304025	 then
		monster_ID = { 25009, 	26008, 	27008, 	28011, 	29011, 	30012, 	28011, 	29011, 	30012 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.1, 0.07, 0.01 } 
		interval = { 500, 700, 500, 700, 900, 1100, 700, 900, 1100 }
		
		Raremob_ID = { 30044, 30046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 305027	 then
		monster_ID = { 27009, 	28012, 	29012, 	30013, 	31009, 	32012, 	29012, 	30013, 	31009 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.17, 0.1, 0.07 } 
		interval = { 500, 300, 500, 700, 900, 2200, 500, 700, 900 }
		
		Raremob_ID = { 32013, 30044, 30046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 2200, 100, 100, 100 }
		
	elseif ID == 306020	 then
		monster_ID = { 20008, 	21006, 	22007, 	23007, 	24009, 	25010 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 500, 700, 1000, 1500, 1800, 2200 }
		
		Raremob_ID = { 25044, 25046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 307025	 then
		monster_ID = { 25011, 	26009, 	27010, 	28013, 	29013, 	30014 }
		density = { 0.47, 0.26, 0.17, 0.17, 0.17, 0.01 } 
		interval = { 500, 700, 1000, 500, 500, 500 }
		
		Raremob_ID = { 25044, 30046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 308018	 then
		monster_ID = { 18007, 	19008, 	20009, 	21007, 	22008, 	23008, 	21007, 	22008, 	23008 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.1, 0.07, 0.01 } 
		interval = { 500, 700, 500, 700, 900, 1100, 700, 900, 1100 }
		
		Raremob_ID = { 20044, 20046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 309023	 then
		monster_ID = { 23009, 	24010, 	25012, 	26010, 	27011, 	28014, 	27011, 	28014 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.07, 0.01 } 
		interval = { 500, 700, 500, 1500, 900, 1100, 900, 1100 }
		
		Raremob_ID = { 25044, 25046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 310026	 then
		monster_ID = { 26011, 	27012, 	28015, 	29014, 	30015, 	31010 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 500, 700, 1000, 1500, 1800, 2200 }
		
		Raremob_ID = { 30044, 30046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 311030	 then
		monster_ID = { 30016, 	31012, 	32014, 	33011, 	34009, 	35014, 	33011, 	34009, 	35014 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.1, 0.07, 0.01 } 
		interval = { 1000, 1200, 700, 1100, 1200, 1500, 1100, 1200, 1500 }
		
		Raremob_ID = { 30044, 35046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 312036	 then
		monster_ID = { 36014, 	37014, 	38010, 	39012, 	40021, 	41017, 	39012, 	40021, 	41017 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.1, 0.07, 0.01 } 
		interval = { 1000, 1200, 1500, 1100, 1200, 1500, 1100, 1200, 1500 }
		
		Raremob_ID = { 40044, 40046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 313033	 then
		monster_ID = { 33012, 	34010, 	35015, 	36015, 	37015, 	38011 }
		density = { 0.47, 0.26, 0.17, 0.17, 0.07, 0.01 } 
		interval = { 1000, 1200, 1500, 700, 700, 700 }
		
		Raremob_ID = { 36016, 35044, 35046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 314037	 then
		monster_ID = { 37019, 	38012, 	39013, 	40022, 	41018, 	42017, 	40022, 	41018, 	42017 }
		density = { 0.47, 0.26, 0.17, 0.17, 0.17, 0.17, 0.17, 0.17, 0.17 } 
		interval = { 1000, 600, 700, 700, 700, 700, 700, 700, 700 }
		
		Raremob_ID = { 40044, 40046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 315021	 then
		monster_ID = { 21008, 	22009, 	23010, 	24011, 	25013, 	26012 , 23010, 	24011, 	25013, 	26012}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 500, 300, 500, 700, 900, 1100, 500, 700, 900, 1100 }

		Raremob_ID = { 26013, 25044, 25046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 316040	 then
		monster_ID = { 40023, 	41019, 	42018, 	43014, 	44014, 	45018 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 1500, 1700, 2500 }
		
		Raremob_ID = { 40044, 45046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 317044	 then
		monster_ID = { 44015, 	45019, 	46015, 	47012, 	48010, 	49010, 	47012, 	48010, 	49010 }
		density = { 0.47, 0.26, 0.17, 0.17, 0.17, 0.01, 0.17, 0.17, 0.01 } 
		interval = { 1500, 2000, 1200, 1200, 1200, 1200, 1200, 1200, 1200 }
		
		Raremob_ID = { 49011, 45044, 45046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 318047	 then
		monster_ID = { 47013, 	48011, 	49012, 	50011, 	51007, 	52006, 	50011, 	51007, 	52006 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 1200, 1500, 1700, 2500, 1500, 1700, 2500 }
		
		Raremob_ID = { 50044, 50046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
				
	elseif ID == 319050	 then
		monster_ID = {	50012, 	51008, 	52007, 	53008, 	54007, 	55008 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 3500, 5000 }
		
		Raremob_ID = { 50044, 55046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 320050	 then
		monster_ID = { 50013, 	51009, 	52008, 	53009, 	54008, 	55009 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 1200, 1500, 1700, 2500 }
		
		Raremob_ID = { 55010, 50044, 55046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 321054	 then
		monster_ID = { 54009, 	55011, 	56006, 	57005, 	58007, 	59006 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 1000, 1200, 1500, 1400, 2500 }
		
		Raremob_ID = { 55044, 55046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 322025	 then
		monster_ID = { 25014, 	26014, 	27013, 	28016, 	29015, 	30017 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.1, 0.1 } 
		interval = { 500, 700,  700,  300,  300,  300 }
		
		Raremob_ID = { 25044, 30046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 323032	 then
		monster_ID = { 32015, 	33013, 	34011, 	35016, 	36017, 	37020, 	34011, 	35016, 	36017, 	37020 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1000, 600, 700, 1100, 1200, 1500, 700, 1100, 1200, 1500 }
		
		Raremob_ID = { 35044, 35046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 324035	 then
		monster_ID = { 35017, 	36018, 	37021, 	38013, 	39014, 	40024, 	37021, 	38013, 	39014, 	40024 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1000, 600, 700, 1100, 1200, 1500, 700, 1100, 1200, 1500 }
		
		Raremob_ID = { 37022, 35044, 40046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 325039	 then
		monster_ID = { 39015, 	40025, 	41020, 	42019, 	43015, 	44016, 	43015, 	44016 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.07, 0.01 } 
		interval = { 1500, 2000, 1200, 1500, 1700, 2500, 1700, 2500 }
		
		Raremob_ID = { 40044, 40046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 326035	 then
		monster_ID = { 35018, 	36019, 	37023, 	38014, 	39016, 	40026 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1000, 1200, 1500, 2200, 2500, 3000 }
		
		Raremob_ID = { 35044, 40046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 327040	 then
		monster_ID = { 40027, 	41021, 	42020, 	43016, 	44017, 	45020 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 3500, 5000 }
		
		Raremob_ID = { 40044, 45046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 328045	 then
		monster_ID = { 45021, 	46016, 	47014, 	48012, 	49013, 	50014, 	50014 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.01 } 
		interval = { 1500, 2000, 2500, 1500, 1700, 2500, 2500 }
		
		Raremob_ID = { 50015, 45044, 50046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 329048	 then
		monster_ID = { 48013, 	49014, 	50016, 	51010, 	52009, 	53010, 	53010 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 1700, 2500, 2500 }
		
		Raremob_ID = { 50044, 50046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 330051	 then
		monster_ID = { 51011, 	52010, 	53011, 	54010, 	55012, 	56007, 	56007 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01, 0.01 } 
		interval = { 1500, 2000, 2500, 1500, 3500, 2500, 2500 }
		
		Raremob_ID = { 55044, 55046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 310000	 then
		monster_ID = { 1000001, 1000002, 1000003, 0, 0, 0 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 3500, 5000 }
		
		Raremob_ID = { 55044, 55046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 331029	 then
		monster_ID = {  29018, 30020, 31016, 32029, 33016, 34016 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 3500, 5000 }
		
		Raremob_ID = { 30046, 30044, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 332032	 then
		monster_ID = {  32030, 33017, 34017, 35030, 36024, 37026 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 3500, 5000 }
		
		Raremob_ID = { 35044, 35046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 332031 	then
		monster_ID = { 32031, 33018, 34019, 35031, 36025, 37027 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 3500, 5000 }
		
		Raremob_ID = { 35044, 35046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 333030	 then
		monster_ID = {  30021, 31017, 32032, 33019, 34020, 35032 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 3500, 5000 }
		
		Raremob_ID = { 35044, 35046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 333033	 then
		monster_ID = {  33020, 34021, 35033, 36026, 37028, 38025 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 } 
		interval = { 1500, 2000, 2500, 3000, 3500, 5000 }
		
		Raremob_ID = { 35044, 35046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

		
	--=======================================================================
	-- 세리우 사막
	--=======================================================================
	elseif ID == 401060 then
		monster_ID = {60011, 61010, 62009, 63009, 64007, 65007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 60044, 65046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 402058 then
		monster_ID = {58009, 59008, 60012, 61011, 62010, 63010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 63015, 60044, 60046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 403062 then
		monster_ID = {62011, 63011, 64008, 65008, 66003, 67003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
			
		Raremob_ID = { 65044, 65046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 404058 then
		monster_ID = {58010, 59009, 60013, 61012, 62012, 63012}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 63016, 60044, 60046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 405062 then
		monster_ID = {66004, 67004, 68003, 69003, 70004, 71003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 70044, 70046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 406068 then
		monster_ID = {68004, 69004, 70005, 71004, 72003, 73002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 70044, 70046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 407070 then
		monster_ID = {70006, 71005, 72004, 73003, 74002, 75004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 73004, 70044, 70046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 408074 then
		monster_ID = {74004, 75006, 77004, 78005, 79007, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 75044, 75046, 76005, 77005 }   
		Raremob_count = { 1, 1, 2, 2 }  
		Raremob_interval = { 100, 100, 100, 100 }
		
	elseif ID == 409065 then
		monster_ID = {65009, 66005, 67005, 68005, 69005, 70007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 65044, 70046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 410060 then
		monster_ID = {60014, 61013, 62013, 63013, 64009, 65010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 60044, 65046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 411062 then
		monster_ID = {62014, 63014, 64004, 65011, 63007, 67006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 65044, 65046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 412055 then
		monster_ID = {55015, 56008, 57006, 58011, 59010, 60015}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 55044, 60046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 413070 then
		monster_ID = {70009, 72005, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 70044, 75046, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

		
	--=======================================================================
	-- 론도 
	--=======================================================================
	
	elseif ID == 501048 then
		monster_ID = {48001, 49001, 50001, 51001, 52001, 53001}
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 50044, 50049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 502058 then
		monster_ID = {58001, 59001, 60001, 61001, 62001, 63001}
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 60044, 60049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 503060 then
		monster_ID = {60002, 61002, 62002, 63002, 64001, 65001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 60044, 65049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 504044 then
		monster_ID = {44001, 45003, 46001, 47001, 48002, 49002}
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 45044, 45049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 505050 then
		monster_ID = {50002, 51002, 52002, 53002, 54001, 55001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 50044, 55049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 506056 then
		monster_ID = {56001, 57001, 58002, 59002, 60003, 61003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 60044, 60049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 507060 then
		monster_ID = {60004, 61004, 62003, 63003, 64002, 65002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 60044, 65049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 508047 then
		monster_ID = {47002, 48003, 49003, 50003, 51003, 52003}
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 50020, 50044, 50049, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 509050 then
		monster_ID = {50004, 51004, 52011, 53003, 54002, 55002}
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 50044, 55049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 510053 then
		monster_ID = {53004, 54003, 55003, 56002, 57002, 58003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 55044, 55049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 511059 then
		monster_ID = {59003, 60005, 61005, 62004, 63004, 64003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 64011, 60044, 60049, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 501045 then
		monster_ID = {45004, 46002, 47003, 48004, 49004, 50018}
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 45044, 50049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 512051 then
		monster_ID = {51012, 52012, 53005, 54004, 55004, 56003}
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 55044, 55049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 513055 then
		monster_ID = {55005, 56004, 57003, 58004, 59004, 60006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 57007, 55044, 60049, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 514058 then
		monster_ID = {58005, 59005, 60008, 61006, 62005, 63005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 60044, 60049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 515060 then
		monster_ID = {60009, 61008, 62006, 63007, 64010, 65003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 60044, 65049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 516065 then
		monster_ID = {65004, 66001, 67001, 68001, 69001, 70001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 70008, 65044, 70049, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 517067 then
		monster_ID = {67002, 68002, 69002, 70002, 71001, 72001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 70044, 70049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 518061 then
		monster_ID = {61009, 62008, 63008, 64006, 65006, 66002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 65044, 65049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 519070 then
		monster_ID = {70003, 71002, 72002, 73001, 74001, 75001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 70044, 75049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 520076 then
		monster_ID = {76001, 77001, 78001, 79001, 80001, 81001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 79008, 80044, 80046, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 521075 then
		monster_ID = {75002, 76002, 77002, 78002, 79002, 80002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 75044, 80049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 522078 then
		monster_ID = {78003, 79003, 80003, 81002, 82001, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 82005, 80044, 80049, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 523079 then --팔미르 고원 바포메트 주변 엔젤몹들을 광신도들로 교체<8.1>
		monster_ID = {79004, 80004, 81003, 81011, 81012, 85012}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 85013, 80044, 80049, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 524075 then
		monster_ID = {75003, 76003, 77003, 78004, 79005, 80005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 75044, 80049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 525079 then
		monster_ID = {79006, 80006, 81004, 82003, 83002, 84002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 80044, 80049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 526084 then
		monster_ID = {84003, 85001, 86001, 87001, 88001, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 85044, 85049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 527086 then
		monster_ID = {86002, 87002, 88002, 89001, 90001, 91001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 87004, 90044, 90049, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 528080 then
		monster_ID = {80007, 81005, 82004, 83003, 84004, 85002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 80044, 85049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 529085 then
		monster_ID = {85003, 86003, 87003, 88003, 89002, 90002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 85044, 90049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 530090 then
		monster_ID = {90003, 91002, 92001, 93001, 94001, 95001}
		density = { 0.47, 0.26, 0.17, 0.17, 0.17, 0.01 }
		interval = { 1000, 1000, 1200, 1200, 1200, 1200 }
		
		Raremob_ID = { 90044, 95049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 531095 then
		monster_ID = {95002, 96001, 97001, 98001, 99001, 100001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 100002, 95044, 100049, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 532055 then
		monster_ID = {55013, 56005, 57004, 58006, 59007, 60010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 55044, 60049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 533050 then
		monster_ID = {50019, 51013, 52013, 53012, 54011, 55014}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 55044, 55049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 534085 then
		monster_ID = {85004, 86004, 87005, 88004, 89003, 90004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 85044, 90049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 535094 then
		monster_ID = {94002, 95003, 96002, 97002, 98002, 99002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 95044, 95049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
			
	elseif ID == 536090 then
		monster_ID = {90005, 91003, 92002, 93002, 94003, 95004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 500, 600, 700, 1000, 3000 }
		
		Raremob_ID = { 90044, 95049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 537096 then
		monster_ID = {96003, 97003, 98003, 99003, 100003, 101001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 100044, 100049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 538100 then
		monster_ID = {100004, 101002, 102001, 103001, 104001, 105001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 600, 700, 1000, 1500 }
		
		Raremob_ID = { 105044, 105049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 539102 then --151~160 챔피온 몬스터
		
		monster_ID = { 151001, 152001, 153001, 154001, 155001, 156001, 157001, 158001, 159001}
		density = { 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}
		
		Raremob_ID = {160001,0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1440000, 0, 0, 0 } --챔피온 필드 중보스의 특징으로 적용
		
	elseif ID == 540106 then
		monster_ID = {106002, 107002, 108001, 109001, 110001, 111001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 110044, 110049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	 elseif ID == 541108 then --151~160 챔피온 몬스터
		
		monster_ID = {161001, 162001, 163001, 164001, 165001, 166001, 167001, 168001, 169001}
		density = { 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500 }
		
		Raremob_ID = { 170001, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1440000, 0, 0, 0 } --챔피온 필드 중보스의 특징으로 적용
		
	elseif ID == 542110 then
		monster_ID = {110003, 111003, 112002, 113002, 114001, 115001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 600, 700, 1000, 1500 }
		
		Raremob_ID = { 115044, 115049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 543114 then
		monster_ID = {114002, 115002, 116001, 117001, 118001, 119001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 120044, 120049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 544093 then
		Raremob_ID = {116002, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 545110 then
		Raremob_ID = {117002, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 546120 then
			
		monster_ID = {108002, 109002, 110002, 111002, 112001, 113001, 102002, 103002, 104002, 105002, 106001, 107001}
		density = { 0.23, 0.13, 0.08, 0.05, 0.03, 0.01, 0.23, 0.13, 0.08, 0.05, 0.03, 0.01} -- 밀도 50%감소
		interval = { 2000, 2000, 2400, 3000, 4000, 6000, 2000, 2000, 2400, 3000, 4000, 6000  } -- 리스폰 주기 200% 증가
	
		Raremob_ID = {119002, 110044, 110049, 105044, 110049 }   
		Raremob_count = { 1, 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100, 100 }
		
		        	
				        	
	--=======================================================================	
	-- 마르두카 군락지
	--=======================================================================	
	
	elseif ID == 601090  then
		monster_ID = {92003, 93003, 94004, 90007, 91004, 95008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 95044, 95049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 602090  then
		monster_ID = {92004, 93004, 95005, 90006, 91005, 94006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 95044, 95049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 603090  then
		monster_ID = {92005, 93005, 94005, 95006, 91006, 90008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 95044, 95049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 604100  then
		monster_ID = {103004, 104003, 105003, 100006, 101004, 102004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 105044, 105049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 605106  then
		monster_ID = {110004, 111004, 108005, 109006, 106009, 107009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 110044, 110049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 606105  then
		monster_ID = {108004, 109004, 110008, 105005, 106005, 107006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 110044, 110049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 607096  then
		monster_ID = {100005, 101003, 98004, 99004, 96005, 97005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 100044, 100049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 608097  then
		monster_ID = {97004, 102003, 98005, 99005, 100007, 101005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 100044, 100049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 609102  then
		monster_ID = {103006, 106010, 107010, 104005, 105007, 102007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 610103  then
		monster_ID = {108003, 105004, 106004, 107007, 103005, 104006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 105044, 105049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 611105  then
		monster_ID = {107003, 109003, 110005, 105008, 106007, 108006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 110044, 110049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 612104  then
		monster_ID = {104004, 107005, 105009, 106008, 108008, 109007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 105044, 105049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 612105  then
		monster_ID = {109005, 110006, 105006, 106006, 107008, 108007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 110044, 110049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 613110  then
		monster_ID = {118002, 119003, 114003, 116005, 115006, 117004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 115044, 115049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 614114  then
		monster_ID = {115003, 116003, 119005, 114005, 117005, 118003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 115044, 115049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 615120  then
		monster_ID = {122001, 123001, 125001, 120003, 121004, 124003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 125044, 125049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 616110  then
		monster_ID = {110007, 111005, 112005, 113005, 114006, 0 }
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 115044, 115049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 619119  then
		monster_ID = {120001, 121001, 119004, 123004, 124001, 122003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 120044, 120049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 620122  then 
		monster_ID = {123002, 126002, 122002, 125003, 127003, 124005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 125044, 125049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 618112  then--101~110 챔피온
		monster_ID = {7101001, 7102001, 7103001, 7104001, 7105001, 7106001, 7107001, 7108001, 7108001}
		density = { 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}
		
		Raremob_ID = { 7110001, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1440000, 0, 0, 0 }
	
	elseif ID == 617110  then --111~120 챔피온
	    
		monster_ID = {7111001, 7112001, 7113001, 7114001, 7115001, 7116001, 7117001, 7118001, 7119001}
		density = { 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}
		
		Raremob_ID = { 7120001, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1440000, 0, 0, 0 }
		
	elseif ID == 633130  then --121~130 챔피온 3
		
		monster_ID = {7121001, 7122001, 7123001, 7124001, 7125001, 7126001, 7127001, 7128001, 7129001}
		density = { 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}
		
		Raremob_ID = { 7130001, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1440000, 0, 0, 0 }
	
	elseif ID == 629137  then --131~140 챔피온
		
		monster_ID = {7131001, 7132001, 7133001, 7134001, 7135001, 7136001, 7137001, 7138001, 7139001}
		density = { 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}
		
		Raremob_ID = { 7140001, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1440000, 0, 0, 0 }
		
	elseif ID == 643145   then --141~150 챔피온
		
		monster_ID = {7141001, 7142001, 7143001, 7144001, 7145001, 7146001, 7147001, 7148001, 7149001}
		density = { 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02 }
		interval = { 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500}
		
		Raremob_ID = { 7150001, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 1440000, 0, 0, 0 }
		
	elseif ID == 621120  then
		monster_ID = {123003, 120002, 121002, 125004, 124004, 122004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		elseif ID == 624129  then
		monster_ID = {129001, 130002, 133002, 132003, 134003, 131004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 130044, 130049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 625124  then
		monster_ID = {126001, 127001, 128001, 125002, 124002, 129003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 125044, 125049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 626132  then
		monster_ID = {132001, 136001, 137001, 133004, 134001, 135004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 135044, 135049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 627134  then
		monster_ID = {139001, 135001, 136002, 138001, 134002, 137003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 135044, 135049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 628139  then
		monster_ID = {139002, 140001, 144002, 142001, 141002, 143003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 140044, 140049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 630140  then
		monster_ID = {142002, 145002, 141003, 143002, 140002, 144003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 145044, 145049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 631143  then
		monster_ID = {143001, 145001, 144001, 147001, 148001, 146002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 145044, 145049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 632145  then
		monster_ID = {150001, 146001, 147002, 145003, 149001, 148002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 150044, 150049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 634110 then
		Raremob_ID = {111008, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 635120  then
		Raremob_ID = {123005, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 6000, 100, 100 }
		
	elseif ID == 636130  then
		Raremob_ID = {132005, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 6000, 100, 100 }
		
	elseif ID == 637130  then
		Raremob_ID = {133005, 0, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }
		
	elseif ID == 638150  then
		Raremob_ID = {150002, 0, 0, 0  }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 6000, 100, 100 }
		
	elseif ID == 639095   then
		monster_ID = {96004, 95007, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 640110   then
		monster_ID = {111007, 110009, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 641121   then
		monster_ID = {121003, 122005, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 642064   then
		monster_ID = {64010, 65003, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 639142   then
		monster_ID = {142003 ,143004 ,144004 ,145004 ,146003 ,147002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 145044, 145049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 640100   then
		monster_ID = {100008 ,101006 ,102006 ,103006 ,0 ,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 105044, 105049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 641145   then
		monster_ID = {145005 ,146004 ,147004 ,148002 ,149001 ,150002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 150044, 150049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 642143   then
		monster_ID = {143005 ,144005 ,145006 ,150003 ,147004 ,148003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 145044, 145049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 644145   then
		monster_ID = {145008 ,146006 ,147006 ,148005 ,149003 ,150005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 150044, 150049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 645142   then
		monster_ID = {142004 ,143006 ,144006 ,145009 ,0 ,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 145044, 145049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }
		
	elseif ID == 646150  then
		Raremob_ID = {150006 , 0 ,0 ,0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 6000, 100, 100 }
		
	elseif ID == 647150  then
		Raremob_ID = {150007 ,0 ,0 ,0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 60000, 100, 100, 100 }

	elseif ID == 648132   then
		monster_ID = {132006,135005, 0, 0, 0 ,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = { 135044, 135049, 0, 0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 100, 100, 100, 0 }

	elseif ID == 649108  then
		Raremob_ID = {108009 ,0 ,0 ,0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 6000, 100, 100, 100 }
		
	elseif ID == 650118  then
		Raremob_ID = {118004 ,0 ,0 ,0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 6000, 100, 100, 100 }

	elseif ID == 651126  then
		Raremob_ID = {126004 ,0 ,0 ,0 }   
		Raremob_count = { 1, 1, 1, 1 }  
		Raremob_interval = { 6000, 100, 100, 100 }

	--=======================================================================	
	-- 잃어버린 갱도	
	--=======================================================================	
	elseif ID == 1001050 then
		monster_ID = {9050001, 9051001, 9052001, 9053001, 9054001, 9055001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9050001, 9051001, 9052001, 9053001, 9054001, 9055001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1002052 then
		monster_ID = {9052002, 9053002, 9054002, 9055002, 9056001, 9057001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9052002, 9053002, 9054002, 9055002, 9056001, 9057001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1003054 then
		monster_ID = {9054003, 9055003, 9056002, 9057002, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9054003, 9055003, 9056002, 9057002, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1004056 then
		monster_ID = {9056003, 9057003, 9058001, 9059001, 9060001, 9061001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9056003, 9057003, 9058001, 9059001, 9060001, 9061001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1005058 then
		monster_ID = {9058002, 9059002, 9060002, 9061002, 9062001, 9063001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9058002, 9059002, 9060002, 9061002, 9062001, 9063001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1006051 then
		monster_ID = {9051002, 9052003, 9053003, 9054004, 9055004, 9056004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9051002, 9052003, 9053003, 9054004, 9055004, 9056004}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1007052 then
		monster_ID = {9052004, 9053004, 9054005, 9055005, 9056005, 9057004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9052004, 9053004, 9054005, 9055005, 9056005, 9057004}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1008052 then
		monster_ID = {9052005, 9053005, 9054006, 9055006, 9056006, 9057005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9052005, 9053005, 9054006, 9055006, 9056006, 9057005}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1009053 then
		monster_ID = {9053006, 9054007, 9055007, 9056007, 9057006, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9053006, 9054007, 9055007, 9056007, 9057006, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1010054 then
		monster_ID = {9054008, 9055008, 9056008, 9057007, 9058003, 9059003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9054008, 9055008, 9056008, 9057007, 9058003, 9059003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1011054 then
		monster_ID = {9054009, 9055009, 9056009, 9057008, 9058004, 9059004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9054009, 9055009, 9056009, 9057008, 9058004, 9059004}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1012056 then
		monster_ID = {9056010, 9057009, 9058005, 9059005, 9060003, 9061003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9056010, 9057009, 9058005, 9059005, 9060003, 9061003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1013057 then
		monster_ID = {9057010, 9058006, 9059006, 9060004, 9061004, 9062002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9057010, 9058006, 9059006, 9060004, 9061004, 9062002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1014058 then
		monster_ID = {9058007, 9059007, 9060005, 9061005, 9062003, 9063002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9058007, 9059007, 9060005, 9061005, 9062003, 9063002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1015060 then
		monster_ID = {9060006, 9061006, 9062004, 9063003, 9064001, 9065001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9060006, 9061006, 9062004, 9063003, 9064001, 9065001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1016061 then
		monster_ID = {9061007, 9062005, 9063004, 9064001, 9065002, 9066001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9061007, 9062005, 9063004, 9064001, 9065002, 9066001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1017064 then
		monster_ID = {9064003, 9065003, 9066002, 9067001, 9068001, 9069001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9064003, 9065003, 9066002, 9067001, 9068001, 9069001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1018060 then
		monster_ID = {9060007, 9061008, 9062006, 9063005, 9064004, 9065004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9060007, 9061008, 9062006, 9063005, 9064004, 9065004}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1019061 then
		monster_ID = {9061009, 9062007, 9063006, 9064005, 9065005, 9066003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9061009, 9062007, 9063006, 9064005, 9065005, 9066003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1020063 then
		monster_ID = {9063007, 9064006, 9065006, 9066004, 9067002, 9068002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9063007, 9064006, 9065006, 9066004, 9067002, 9068002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1021065 then
		monster_ID = {9065007, 9066005, 9067003, 9068003, 9069002, 9070001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9065007, 9066005, 9067003, 9068003, 9069002, 9070001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1022057 then
		monster_ID = {9057011, 9058008, 9059008, 9060008, 9061010, 9062008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9057011, 9058008, 9059008, 9060008, 9061010, 9062008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1023058 then
		monster_ID = {9058009, 9059009, 9060009, 9061011, 9062009, 9063008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9058009, 9059009, 9060009, 9061011, 9062009, 9063008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1024059 then
		monster_ID = {9059010, 9060010, 9061012, 9062010, 9063009, 9064007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9059010, 9060010, 9061012, 9062010, 9063009, 9064007}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1025057 then
		monster_ID = {9057012, 9058010, 9059011, 9060011, 9061013, 9062011}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9057012, 9058010, 9059011, 9060011, 9061013, 9062011}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1026057 then
		monster_ID = {9057013, 9058011, 9059012, 9060012, 9061014, 9062012}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9057013, 9058011, 9059012, 9060012, 9061014, 9062012}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1027058 then
		monster_ID = {9058012, 9059013, 9060013, 9061015, 9062013, 9063010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9058012, 9059013, 9060013, 9061015, 9062013, 9063010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1028065 then
		monster_ID = {9065008, 9066006, 9067004, 9068004, 9069003, 9070002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9065008, 9066006, 9067004, 9068004, 9069003, 9070002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1029068 then
		monster_ID = {9068005, 9069004, 9070003, 9070004, 9070005, 9070006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9068005, 9069004, 9070003, 9070004, 9070005, 9070006}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1030066 then
		monster_ID = {9066007, 9067005, 9068006, 9069005, 9070007, 9070008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9066007, 9067005, 9068006, 9069005, 9070007, 9070008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1031067 then
		monster_ID = {9067006, 9068007, 9069006, 9070009, 9070010, 9070011}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9067006, 9068007, 9069006, 9070009, 9070010, 9070011}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1032050 then
		monster_ID = {9050002, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9050002, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1033053 then
		monster_ID = {9053007, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9053007, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1034053 then
		monster_ID = {9053008, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9053008, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1035055 then
		monster_ID = {9055010, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9055010, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1036060 then
		monster_ID = {9060014, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9060014, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1037060 then
		monster_ID = {9069007, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9069007, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1038063 then
		monster_ID = {9063011, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9063011, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1039064 then
		monster_ID = {9064008, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9064008, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1040065 then
		monster_ID = {9065009, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9065009, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1041066 then
		monster_ID = {9066008, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9066008, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1042053 then
		monster_ID = {9053009, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9053009, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1043054 then
		monster_ID = {9054010, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9054010, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1044056 then
		monster_ID = {9056011, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9056011, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1045053 then
		monster_ID = {9053010, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9053010, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1046065 then
		monster_ID = {9065010, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9065010, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1047065 then
		monster_ID = {9065011, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9065011, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1048055 then
		Raremob_ID = {9055011, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9055011, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1049061 then
		Raremob_ID = {9061016, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9061016, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1050060  then
		Raremob_ID = {9060015, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9060015, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1051069  then
		Raremob_ID = {9069008, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9069008, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1052060  then
		Raremob_ID = {9060016, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 270000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9060016, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 270000, 0, 0, 0 }
		
	elseif ID == 1053070  then
		Raremob_ID = {9070012, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9070012, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }
		
	--=======================================================================	
	-- 수정 계곡	
	--=======================================================================	
	elseif ID == 1101070  then
		monster_ID = {9070013, 9071001, 9072001, 9073001, 9074001, 9075001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9070013, 9071001, 9072001, 9073001, 9074001, 9075001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1103072   then
		monster_ID = {9070014, 9071002, 9072002, 9073002, 9074002, 9075002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9070014, 9071002, 9072002, 9073002, 9074002, 9075002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1104073   then
		monster_ID = {9073003, 9074003, 9075003, 9076001, 9077001, 9078001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9073003, 9074003, 9075003, 9076001, 9077001, 9078001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1105075   then
		monster_ID = {9075004, 9076002, 9077002, 9078002, 9079001, 9080001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9075004, 9076002, 9077002, 9078002, 9079001, 9080001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1106076   then
		monster_ID = {9076003, 9077003, 9078003, 9079002, 9080002, 9081001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9076003, 9077003, 9078003, 9079002, 9080002, 9081001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
	
	--[[ 영역 아이디 오류로 인해 잘못 리스폰 되고있음	
	elseif ID == 1107078   then
		monster_ID = {9078004, 9079003, 9080003, 9081002, 9082001, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9078004, 9079003, 9080003, 9081002, 9082001, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
	--]]
		
	elseif ID == 1108076   then
		monster_ID = {9076004, 9077004, 9078005, 9079004, 9080004, 9081003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9076004, 9077004, 9078005, 9079004, 9080004, 9081003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1109078   then
		monster_ID = {9078006, 9079005, 9080005, 9081004, 9082002, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9078006, 9079005, 9080005, 9081004, 9082002, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1110080   then
		monster_ID = {9080006, 9081005, 9082003, 9083001, 9084001, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9080006, 9081005, 9082003, 9083001, 9084001, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1111080   then
		monster_ID = {9080007, 9081006, 9082004, 9083002, 9084002, 9084012}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9080007, 9081006, 9082004, 9083002, 9084002, 9084012}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1112070   then
		monster_ID = {9070015, 9071003, 9072003, 9073004, 9074004, 9075005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9070015, 9071003, 9072003, 9073004, 9074004, 9075005}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1114071   then
		monster_ID = {9071004, 9072004, 9073005, 9074005, 9075006, 9076005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9071004, 9072004, 9073005, 9074005, 9075006, 9076005}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1115073   then
		monster_ID = {9073006, 9073007, 9074006, 9074007, 9075007, 9076006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9073006, 9073007, 9074006, 9074007, 9075007, 9076006}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1116074   then
		monster_ID = {9074008, 9074009, 9075008, 9076007, 9077005, 9078007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9074008, 9074009, 9075008, 9076007, 9077005, 9078007}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1117076   then
		monster_ID = {9076008, 9077006, 9078008, 9079006, 9080008, 9081007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9076008, 9077006, 9078008, 9079006, 9080008, 9081007}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1118077   then
		monster_ID = {9077007, 9078009, 9079007, 9080009, 9081008, 9082005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9077007, 9078009, 9079007, 9080009, 9081008, 9082005}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1119075   then
		monster_ID = {9075009, 9076009, 9077008, 9078010, 9079008, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9075009, 9076009, 9077008, 9078010, 9079008, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1120080   then
		monster_ID = {9080010, 9081009, 9082006, 9083003, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9080010, 9081009, 9082006, 9083003, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1121076   then
		monster_ID = {9076010, 9077009, 9078011, 9079009, 9080011, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9076010, 9077009, 9078011, 9079009, 9080011, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1122080   then
		monster_ID = {9080012, 9081010, 9082007, 9083004, 9084003, 9084004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9080012, 9081010, 9082007, 9083004, 9084003, 9084004}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1124082   then
		monster_ID = {9082008, 9082009, 9083005, 9084005, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9082008, 9082009, 9083005, 9084005, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1125084   then
		monster_ID = {9084006, 9084007, 9085001, 9086001, 9087001, 9088001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9084006, 9084007, 9085001, 9086001, 9087001, 9088001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1126084   then
		monster_ID = {9084008, 9084009, 9085002, 9085003, 9086002, 9086003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9084008, 9084009, 9085002, 9085003, 9086002, 9086003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1127085   then
		monster_ID = {9085004, 9085005, 9085006, 9086004, 9087002, 9088002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9085004, 9085005, 9085006, 9086004, 9087002, 9088002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1129086   then
		monster_ID = {9086005, 9087003, 9087004, 9088003, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9086005, 9087003, 9087004, 9088003, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1131087   then
		monster_ID = {9087005, 9088004, 9088005, 9089001, 9090001, 9090002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9087005, 9088004, 9088005, 9089001, 9090001, 9090002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1132087   then
		monster_ID = {9087006, 9088006, 9089002, 9090003, 9090004, 9090005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9087006, 9088006, 9089002, 9090003, 9090004, 9090005}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1133088   then
		monster_ID = {9088007, 9089003, 9089004, 9090006, 9090007, 9090008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9088007, 9089003, 9089004, 9090006, 9090007, 9090008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1134084   then
		monster_ID = {9084010, 9085007, 9086006, 9087007, 9088008, 9089005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9084010, 9085007, 9086006, 9087007, 9088008, 9089005}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1135082   then
		monster_ID = {9082010, 9083006, 9084011, 9085008, 9086007, 9087008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9082010, 9083006, 9084011, 9085008, 9086007, 9087008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1136085   then
		monster_ID = {9085009, 9086008, 9086009, 9087009, 9088009, 9089006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9085009, 9086008, 9086009, 9087009, 9088009, 9089006}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1137087   then
		monster_ID = {9087010, 9088010, 9088011, 9089007, 9090009, 9090010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9087010, 9088010, 9088011, 9089007, 9090009, 9090010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1138086   then
		monster_ID = {9086010, 9087011, 9088012, 9090011, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9086010, 9087011, 9088012, 9090011, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1139088   then
		monster_ID = {9088013, 9088014, 9089008, 9089009, 9090012, 9090013}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9088013, 9088014, 9089008, 9089009, 9090012, 9090013}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1140086   then
		monster_ID = {9086011, 9086012, 9087012, 9088015, 9089010, 9089011}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9086011, 9086012, 9087012, 9088015, 9089010, 9089011}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1141085   then
		monster_ID = {9085010, 9086013, 9087013, 9088016, 9089012, 9090014}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9085010, 9086013, 9087013, 9088016, 9089012, 9090014}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1142087   then
		monster_ID = {9087014, 9088017, 9088018, 9089013, 9089014, 9090015}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9087014, 9088017, 9088018, 9089013, 9089014, 9090015}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1143088   then
		monster_ID = {9088019, 9089015, 9089016, 9090016, 9090017, 9090018}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9088019, 9089015, 9089016, 9090016, 9090017, 9090018}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1144072   then
		monster_ID = {9072005, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9072005, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1145088  then
		monster_ID = {9088020, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9088020, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1146089  then
		monster_ID = {9089017, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9089017, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1147088  then
		monster_ID = {9088021, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9088021, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1148086  then
		monster_ID = {9086014, 0, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9086014, 0, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1149075  then
		Raremob_ID = {9075010, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9075010, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1150080  then
		Raremob_ID = {9080013, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9080013, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1151085  then
		Raremob_ID = {9085011, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9085011, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1152089  then
		Raremob_ID = {9089018, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9089018, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1153080  then
		Raremob_ID = {9080014, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9080014, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }
		
	elseif ID == 1154090  then
		Raremob_ID = {9090019, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9090019, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }
		
		       	
	--=======================================================================	
	-- 메마른 달빛의 유적	
	--=======================================================================	
	elseif ID == 1201030  then
		monster_ID = {9030001, 9031001, 9032001, 9033001, 9034001, 9035001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9030001, 9031001, 9032001, 9033001, 9034001, 9035001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1202031  then
		monster_ID = {9031002, 9032002, 9033002, 9034002, 9035002, 9036001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9031002, 9032002, 9033002, 9034002, 9035002, 9036001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1203033  then
		monster_ID = {9033003, 9034003, 9035003, 9036002, 9037001, 9038001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9033003, 9034003, 9035003, 9036002, 9037001, 9038001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1204034  then
		monster_ID = {9034004, 9035004, 9036003, 9037002, 9038002, 9039001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9034004, 9035004, 9036003, 9037002, 9038002, 9039001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1205035  then
		monster_ID = {9035005, 9036004, 9037003, 9038003, 9039002, 9040001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9035005, 9036004, 9037003, 9038003, 9039002, 9040001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1206036  then
		monster_ID = {9036005, 9037004, 9038004, 9039003, 9040002, 9041001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9036005, 9037004, 9038004, 9039003, 9040002, 9041001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1207037  then
		monster_ID = {9037005, 9038005, 9039004, 9040003, 9041002, 9042001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9037005, 9038005, 9039004, 9040003, 9041002, 9042001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1208037  then
		monster_ID = {9037006, 9038006, 9039005, 9040004, 9041003, 9042002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9037006, 9038006, 9039005, 9040004, 9041003, 9042002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1209039  then
		monster_ID = {9039006, 9040005, 9041004, 9042003, 9043001, 9044001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9039006, 9040005, 9041004, 9042003, 9043001, 9044001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1210039  then
		monster_ID = {9039007, 9040006, 9041005, 9042004, 9043002, 9044002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9039007, 9040006, 9041005, 9042004, 9043002, 9044002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1210040  then
		monster_ID = {9040007, 9041006, 9042005, 9043003, 9044002, 9045001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9040007, 9041006, 9042005, 9043003, 9044002, 9045001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1211041  then
		monster_ID = {9041007, 9042006, 9043004, 9044004, 9045002, 9046001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9041007, 9042006, 9043004, 9044004, 9045002, 9046001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1212040  then
		monster_ID = {9040008, 9041008, 9042007, 9043005, 9044005, 9045003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9040008, 9041008, 9042007, 9043005, 9044005, 9045003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1213030  then
		monster_ID = {9030002, 9031003, 9032003, 9033004, 9034005, 9035006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9030002, 9031003, 9032003, 9033004, 9034005, 9035006}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1214030  then
		monster_ID = {9030003, 9031004, 9032004, 9033005, 9034006, 9035007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9030003, 9031004, 9032004, 9033005, 9034006, 9035007}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1215032  then
		monster_ID = {9032005, 9033006, 9034007, 9035008, 9036006, 9037007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9032005, 9033006, 9034007, 9035008, 9036006, 9037007}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1216033  then
		monster_ID = {9033007, 9034008, 9035009, 9036007, 9037008, 9038007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9033007, 9034008, 9035009, 9036007, 9037008, 9038007}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1217035  then
		monster_ID = {9035010, 9036008, 9037009, 9038008, 9039008, 9040009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9035010, 9036008, 9037009, 9038008, 9039008, 9040009}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1218035  then
		monster_ID = {9035011, 9036009, 9037010, 9038009, 9039009, 9040010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9035011, 9036009, 9037010, 9038009, 9039009, 9040010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1219037  then
		monster_ID = {9037011, 9038010, 9039010, 9040011, 9041009, 9042008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9037011, 9038010, 9039010, 9040011, 9041009, 9042008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1220036  then
		monster_ID = {9036010, 9037012, 9038010, 9039011, 9040012, 9041010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9036010, 9037012, 9038010, 9039011, 9040012, 9041010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1221037  then
		monster_ID = {9037013, 9038012, 9039012, 9040013, 9041011, 9042009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9037013, 9038012, 9039012, 9040013, 9041011, 9042009}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1222038  then
		monster_ID = {9038013, 9039013, 9040014, 9041012, 9042010, 9043006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9038013, 9039013, 9040014, 9041012, 9042010, 9043006}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1223038  then
		monster_ID = {9038014, 9039014, 9040015, 9041013, 9042011, 9043007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9038014, 9039014, 9040015, 9041013, 9042011, 9043007}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1224040  then
		monster_ID = {9040016, 9041014, 9042012, 9043008, 9044006, 9045004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9040016, 9041014, 9042012, 9043008, 9044006, 9045004}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1225041  then
		monster_ID = {9041015, 9042013, 9043009, 9044007, 9045005, 9046002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9041015, 9042013, 9043009, 9044007, 9045005, 9046002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1225043  then
		monster_ID = {9043010, 9044008, 9045006, 9046003, 9047001, 9048001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9043010, 9044008, 9045006, 9046003, 9047001, 9048001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1226042  then
		monster_ID = {9042014, 9043011, 9044009, 9045007, 9046004, 9047002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9042014, 9043011, 9044009, 9045007, 9046004, 9047002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1227044  then
		monster_ID = {9044010, 9045008, 9046005, 9047003, 9048002, 9049001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9044010, 9045008, 9046005, 9047003, 9048002, 9049001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1229041  then
		monster_ID = {9041016, 9042015, 9043012, 9044011, 9045009, 9046006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9041016, 9042015, 9043012, 9044011, 9045009, 9046006}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1230042  then
		monster_ID = {9042016, 9043013, 9044012, 9045010, 9046007, 9047004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9042016, 9043013, 9044012, 9045010, 9046007, 9047004}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1231043  then
		monster_ID = {9043014, 9044013, 9045011, 9046008, 9047005, 9048003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9043014, 9044013, 9045011, 9046008, 9047005, 9048003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1232044  then
		monster_ID = {9044014, 9045012, 9046009, 9047006, 9048004, 9049002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9044014, 9045012, 9046009, 9047006, 9048004, 9049002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1233042  then
		monster_ID = {9042017, 9043015, 9044015, 9045013, 9046010, 9047007}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9042017, 9043015, 9044015, 9045013, 9046010, 9047007}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1234044  then
		monster_ID = {9044016, 9045014, 9046011, 9047008, 9048005, 9049003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9044016, 9045014, 9046011, 9047008, 9048005, 9049003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1235045  then
		monster_ID = {9045015, 9046012, 9047009, 9048006, 9049004, 9050003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9045015, 9046012, 9047009, 9048006, 9049004, 9050003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1236046  then
		monster_ID = {9046013, 9046014, 9047010, 9048007, 9049005, 9050004}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9046013, 9046014, 9047010, 9048007, 9049005, 9050004}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1237046  then
		monster_ID = {9046015, 9046016, 9047011, 9048008, 9049006, 9050005}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9046015, 9046016, 9047011, 9048008, 9049006, 9050005}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1238047  then
		monster_ID = {9047012, 9047013, 9048009, 9048010, 9049007, 9050006}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9047012, 9047013, 9048009, 9048010, 9049007, 9050006}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1239047  then
		monster_ID = {9047014, 9048011, 9049008, 9050007, 9049009, 9048012}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9047014, 9048011, 9049008, 9050007, 9049009, 9048012}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1240048  then
		monster_ID = {9048013, 9048014, 9049010, 9049011, 9050008, 9050009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9048013, 9048014, 9049010, 9049011, 9050008, 9050009}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1241048  then
		monster_ID = {9048015, 9048016, 9049012, 9049013, 9050010, 9050011}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9048015, 9048016, 9049012, 9049013, 9050010, 9050011}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1242045  then
		monster_ID = {9045016, 9046017, 9047015, 9048017, 9049014, 9050012}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9045016, 9046017, 9047015, 9048017, 9049014, 9050012}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1248043  then
		monster_ID = {9043016, 9044017, 0,  0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
			
		Raidmob_ID = {9043016, 9044017, 0, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1249040  then
		monster_ID = {9040017, 9041017, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9040017, 9041017, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1250043  then
		monster_ID = {9043017, 9044018, 0, 0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9043017, 9044018, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1251038  then
		monster_ID = {9038015, 9039015, 0,  0, 0, 0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9038015, 9039015, 0, 0, 0, 0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1242037  then
		Raremob_ID = {9037014, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9037014, 0, 0, 0 }
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1243037  then
		Raremob_ID = {9037015, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9037015, 0, 0, 0 }
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1244046  then
		Raremob_ID = {9046018, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9046018, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1245046  then
		Raremob_ID = {9046019, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9046019, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1246040  then
		Raremob_ID = {9040018, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 270000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9040018, 0, 0, 0 }
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 270000, 0, 0, 0 }
		
	elseif ID == 1247050  then
		Raremob_ID = {9050013, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9050013, 0, 0, 0 }
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }

	--=======================================================================	
	-- 팔미르 유적
	--=======================================================================				
		
	elseif ID == 1301090 then
		monster_ID = {9090021,9091001,9092001,9093001,9094001,9095001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9090021,9091001,9092001,9093001,9094001,9095001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1302091 then
		monster_ID = {9091002,9092002,9093001,9094001,9095002,9096001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9091002,9092002,9093001,9094001,9095002,9096001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1303092 then
		monster_ID = {9092003,9093003,9094003,9095001,9096002,9097001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9092003,9093003,9094003,9095001,9096002,9097001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1304093 then
		monster_ID = {9093003,9094004,9095004,9096002,9097002,9098001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9093003,9094004,9095004,9096002,9097002,9098001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1305093 then
		monster_ID = {9093001,9094001,9095005,9096004,9097001,9098001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9093001,9094001,9095005,9096004,9097001,9098001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1306094 then
		monster_ID = {9094001,9095005,9096004,9097002,9098003,9099001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9094001,9095005,9096004,9097002,9098003,9099001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1307095 then
		monster_ID = {9095001,9096002,9097001,9098001,9099002,9100001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9095001,9096002,9097001,9098001,9099002,9100001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1308095 then
		monster_ID = {9095001,9096001,9097002,9098005,9099001,9100001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9095001,9096001,9097002,9098005,9099001,9100001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1309096 then
		monster_ID = {9096002,9097002,9098001,9099001,9100001,9101001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9096002,9097002,9098001,9099001,9100001,9101001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1310097 then
		monster_ID = {9097008,9098007,9099005,9100004,9101002,9102001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9097008,9098007,9099005,9100004,9101002,9102001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1311098 then
		monster_ID = {9098008,9099002,9100004,9101003,9102001,9103001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9098008,9099002,9100004,9101003,9102001,9103001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1312099 then
		monster_ID = {9099002,9100004,9101004,9102001,9103002,9104001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9099002,9100004,9101004,9102001,9103002,9104001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1313100 then
		monster_ID = {9100004,9101002,9102001,9103003,9104002,9105001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9100004,9101002,9102001,9103003,9104002,9105001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1314101 then
		monster_ID = {9101002,9102005,9103003,9104003,9105001,9106001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9101002,9102005,9103003,9104003,9105001,9106001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1315101 then
		monster_ID = {9101004,9102005,9103001,9104001,9105003,9106001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9101004,9102005,9103001,9104001,9105003,9106001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1316102 then
		monster_ID = {9102007,9103003,9104003,9105004,9106001,9107001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9102007,9103003,9104003,9105004,9106001,9107001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1317103 then
		monster_ID = {9103007,9104002,9105005,9106004,9107002,9108001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9103007,9104002,9105005,9106004,9107002,9108001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1318104 then
		monster_ID = {9104002,9105003,9106001,9107002,9108002,9109001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9104002,9105003,9106001,9107002,9108002,9109001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1319105 then
		monster_ID = {9105005,9106006,9107002,9108003,9109001,9110001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9105005,9106006,9107002,9108003,9109001,9110001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1320104 then
		monster_ID = {9104008,9105005,9106007,9107001,9108004,9109003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9104008,9105005,9106007,9107001,9108004,9109003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1321106 then
		monster_ID = {9106001,9107006,9108005,9109004,9110002,9111001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9106001,9107006,9108005,9109004,9110002,9111001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1322106 then
		monster_ID = {9106009,9107001,9108001,9109005,9110002,9111001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9106009,9107001,9108001,9109005,9110002,9111001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1323107 then
		monster_ID = {9107008,9108001,9109005,9110004,9111003,9112001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9107008,9108001,9109005,9110004,9111003,9112001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1324107 then
		monster_ID = {9107009,9108001,9109005,9110002,9111003,9112002}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9107009,9108001,9109005,9110002,9111003,9112002}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1325108 then
		monster_ID = {9108001,9109001,9110004,9111001,9112002,9113001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9108001,9109001,9110004,9111001,9112002,9113001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1326108 then
		monster_ID = {9108001,9109005,9110004,9111003,9112001,9113001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9108001,9109005,9110004,9111003,9112001,9113001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1327108 then
		monster_ID = {9108001,9109005,9110002,9111001,9112005,9113003}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9108001,9109005,9110002,9111001,9112005,9113003}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1328108 then
		monster_ID = {9108001,9109005,9110001,9111003,9112001,9113001}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9108001,9109005,9110001,9111003,9112001,9113001}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1329092 then
		monster_ID = {9092004,9093006,9094007,9095009,9096009,9097009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9092004,9093006,9094007,9095009,9096009,9097009}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1330093 then
		monster_ID = {9093006,9094008,9095010,9096009,9097010,9098009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9093006,9094008,9095010,9096009,9097010,9098009}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1331093 then
		monster_ID = {9093008,9094007,9095009,9096011,9097010,9098010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9093008,9094007,9095009,9096011,9097010,9098010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1332094 then
		monster_ID = {9093008,9094008,9095009,9096009,9097010,9098011}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9093008,9094008,9095009,9096009,9097010,9098011}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1333095 then
		monster_ID = {9095009,9096009,9097013,9098010,9099008,9100008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9095009,9096009,9097013,9098010,9099008,9100008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1334095 then
		monster_ID = {9095009,9096011,9097010,9098010,9099009,9100008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9095009,9096011,9097010,9098010,9099009,9100008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1335096 then
		monster_ID = {9096009,9097013,9098010,9099010,9100010,9101008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9096009,9097013,9098010,9099010,9100010,9101008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1336097 then
		monster_ID = {9097016,9098015,9099011,9100011,9101009,9102008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9097016,9098015,9099011,9100011,9101009,9102008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1337098 then
		monster_ID = {9098016,9099011,9100012,9101010,9102009,9103008}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9098016,9099011,9100012,9101010,9102009,9103008}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1338099 then
		monster_ID = {9099013,9100013,9101011,9102009,9103008,9104009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9099013,9100013,9101011,9102009,9103008,9104009}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1339100 then
		monster_ID = {9100012,9101012,9102011,9103010,9104009,9105009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9100012,9101012,9102011,9103010,9104009,9105009}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1340101 then
		monster_ID = {9101013,9102012,9103011,9104011,9105009,9106010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9101013,9102012,9103011,9104011,9105009,9106010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1341101 then
		monster_ID = {9101013,9102009,9103012,9104011,9105009,9106010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9101013,9102009,9103012,9104011,9105009,9106010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1342102 then
		monster_ID = {9102008,9103013,9104013,9105012,9106012,9107010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9102008,9103013,9104013,9105012,9106012,9107010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1343103 then
		monster_ID = {9103011,9104014,9105012,9106013,9107011,9108013}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9103011,9104014,9105012,9106013,9107011,9108013}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1344104 then
		monster_ID = {9104013,9105014,9106013,9107012,9108014,9109012}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9104013,9105014,9106013,9107012,9108014,9109012}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1345105 then
		monster_ID = {9105015,9106012,9107013,9108013,9109012,9110010}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9105015,9106012,9107013,9108013,9109012,9110010}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1345106 then
		monster_ID = {9106016,9107011,9108014,9109012,9110011,9111009}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9106016,9107011,9108014,9109012,9110011,9111009}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1367105 then
		monster_ID = {9105026,9105027,9106024,9107021,9108023,9109019}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9105026,9105027,9106024,9107021,9108023,9109019}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }

	elseif ID == 1368106 then
		monster_ID = {9106025,9107022,9107023,9108024,9109020,9110016}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9106025,9107022,9107023,9108024,9109020,9110016}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }

	elseif ID == 1394104 then
		monster_ID = {9104022,9105028,9106026,9107024,9108025,9109021}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9104022,9105028,9106026,9107024,9108025,9109021}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }

	elseif ID == 1395105 then
		monster_ID = {9105029,9106027,9107025,9108026,9109022,9110017}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9105029,9106027,9107025,9108026,9109022,9110017}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }

	elseif ID == 1396106 then
		monster_ID = {9106028,9106029,9107026,9107027,9108027,9109023}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		Raidmob_ID = {9106028,9106029,9107026,9107027,9108027,9109023}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
				
	elseif ID == 1346090 then
		monster_ID = {9090022,9091003,9092005,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9090022,9091003,9092005,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1347092 then
		monster_ID = {9092006,9093010,9094011,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9092006,9093010,9094011,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1349092 then
		monster_ID = {9092006,9093010,9094011,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9092006,9093010,9094011,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1350093 then
		monster_ID = {9093012,9094013,9095015,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9093012,9094013,9095015,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1351093 then
		monster_ID = {9093010,9094014,9095016,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9093010,9094014,9095016,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1352094 then
		monster_ID = {9094014,9095017,9096016,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9094014,9095017,9096016,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1353095 then
		monster_ID = {9095015,9096017,9097017,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9095015,9096017,9097017,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1354099 then
		monster_ID = {9099014,9100015,9101015,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9099014,9100015,9101015,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1355099 then
		monster_ID = {9099014,9100016,9101016,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9099014,9100016,9101016,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1356100 then
		monster_ID = {9100015,9101015,9102015,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9100015,9101015,9102015,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1357101 then
		monster_ID = {9101015,9102016,9103015,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9101015,9102016,9103015,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1358102 then
		monster_ID = {9102017,9103015,9104016,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9102017,9103015,9104016,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1359103 then
		monster_ID = {9103017,9104017,9105016,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9103017,9104017,9105016,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1360104 then
		monster_ID = {9104017,9105016,9106017,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9104017,9105016,9106017,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1362105 then
		monster_ID = {9105018,9106017,9107015,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9105018,9106017,9107015,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1363106 then
		monster_ID = {9106017,9107016,9108017,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9106017,9107016,9108017,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1364108 then
		monster_ID = {9108018,9109015,9110012,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9108018,9109015,9110012,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1365109 then
		monster_ID = {9109016,9110013,9111010,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9109016,9110013,9111010,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1366109 then
		monster_ID = {9109016,9110012,9111010,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9109016,9110012,9111010,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1369092 then
		monster_ID = {9092008,9093014,9094016,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9092008,9093014,9094016,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1370092 then
		monster_ID = {9092008,9093014,9094017,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9092008,9093014,9094017,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1371092 then
		monster_ID = {9092010,9093014,9094017,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9092010,9093014,9094017,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1372093 then
		monster_ID = {9093014,9094016,9095019,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9093014,9094016,9095019,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1373093 then
		monster_ID = {9093014,9094016,9095019,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9093014,9094016,9095019,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1374095 then
		monster_ID = {9095021,9096018,9097018,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9095021,9096018,9097018,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1375096 then
		monster_ID = {9096018,9097019,9098017,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9096018,9097019,9098017,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1376097 then
		monster_ID = {9097019,9098018,9099016,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9097019,9098018,9099016,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1377098 then
		monster_ID = {9098018,9099017,9100018,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9098018,9099017,9100018,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1379102 then
		monster_ID = {9102018,9103018,9104019,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9102018,9103018,9104019,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1380103 then
		monster_ID = {9103018,9104019,9105019,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9103018,9104019,9105019,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1381104 then
		monster_ID = {9104019,9105020,9106010,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9104019,9105020,9106010,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1382105 then
		monster_ID = {9105020,9106010,9107010,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9105020,9106010,9107010,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1383105 then
		monster_ID = {9105020,9106010,9107010,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9105020,9106010,9107010,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1384106 then
		monster_ID = {9106023,9107019,9108014,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9106023,9107019,9108014,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1385107 then
		monster_ID = {9107019,9108014,9109012,0,0,0}
		density = { 0.47, 0.26, 0.17, 0.1, 0.07, 0.01 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
				
		Raidmob_ID = {9107019,9108014,9109012,0,0,0}
		Raidmob_density = { 0.94, 0.52, 0.34, 0.2, 0.17, 0.02 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1395101  then
		Raremob_ID = {9101019, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9101019, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1396095  then
		Raremob_ID = {9095022, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9095022, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1397105  then
		Raremob_ID = {9105023, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9105023, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1386095  then
		Raremob_ID = {9095023, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9095023, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1387100  then
		Raremob_ID = {9100019, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9100019, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1388105  then
		Raremob_ID = {9105024, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9105024, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1389095  then
		Raremob_ID = {9095024, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9095024, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1390100  then
		Raremob_ID = {9100020, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9100020, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1391105  then
		Raremob_ID = {9105025, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9105025, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1392108  then
		Raremob_ID = {9108021, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9108021, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1393108  then
		Raremob_ID = {9108022, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9108022, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
	
	elseif ID == 1394110  then
		Raremob_ID = {9110015, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9110015, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
		
		

	--=======================================================================	
	-- 잃어버린 비밀의 섬
	--=======================================================================				
		
	elseif ID == 701001 then
		monster_ID = {9121001,9122001,9123001,9124001,9125001,9125001}
		density = { 0.06, 0.07, 0.09, 0.06, 0.04, 0.03 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 701011 then
		monster_ID = {9121001,9122001,9123001,9124001,9125001,9125001}
		density = { 0.08, 0.1, 0.12, 0.1, 0.1, 0.1 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 701021 then
		monster_ID = {9121001,9122001,9123001,9124001,9125001,9125001}
		density = { 0.08, 0.1, 0.12, 0.08, 0.06, 0.04 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 701031 then
		monster_ID = {9121001,9122001,9123001,9124001,9125001,9125001}
		density = { 0.08, 0.1, 0.12, 0.1, 0.1, 0.1 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
		
	elseif ID == 701041 then
		monster_ID = {9121001,9122001,9123001,9124001,9125001,9125001}
		density = { 0.08, 0.1, 0.12, 0.08, 0.06, 0.08 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
		
	elseif ID == 701051 then
		monster_ID = {9121001,9122001,9123001,9124001,9125001,9125001}
		density = { 0.06, 0.07, 0.09, 0.06, 0.04, 0.03 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 701061 then
		monster_ID = {9121001,9122001,9123001,9124001,9125001,9125001}
		density = { 0.08, 0.1, 0.12, 0.1, 0.1, 0.1 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 704001 then
		monster_ID = {9125008,0,0,0,0,0}
		density = { 0.04, 0.05, 0.06, 0.04, 0.03, 0.02 }
		interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	

	elseif ID == 704011 then
		monster_ID = {9125008,0,0,0,0,0}
		density = { 0.04, 0.05, 0.06, 0.04, 0.03, 0.02 }
		interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 704021 then
		monster_ID = {9125008,0,0,0,0,0}
		density = { 0.04, 0.05, 0.06, 0.04, 0.03, 0.02 }
		interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
		
	
	--=======================================================================	
	-- 챔피온 지역 확장 ( Lv 30 ~ Lv100 )
	--=======================================================================				
		
	elseif ID == 801001 then									-- 라크시 필드 (124526 135823)
		Raremob_ID = {7032002,7034001,0,0,0,0}
		Raremob_count = { 5, 5, 10, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
	
	
		--monster_ID = {7032002,7034001,0,0,0,0}		챔피온 몬스터를 비율로 배치안하고 개수를 정해서 배치했음 (20100219 김동민)
		--density = { 0.8, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801002 then									-- 라크시 필드 (125007 132969)
		Raremob_ID = {7034001,7035001,0,0,0,0}
		Raremob_count = { 5, 5, 10, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
	
		
		--monster_ID = {7034001,7035001,0,0,0,0}
		--density = { 0.8, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 801003 then									-- 그루터기 벌목장 (123133 131894)
		Raremob_ID = {7036001,7037001,0,0,0,0}
		Raremob_count = { 5, 5, 10, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }


		--monster_ID = {7036001,7037001,0,0,0,0}
		--density = { 0.8, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 801004 then									-- 라크시 필드 (120369 132034)
		Raremob_ID = {7036002,7037003,0,0,0,0}
		Raremob_count = { 5, 5, 10, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }


		--monster_ID = {7036002,7037003,0,0,0,0}
		--density = { 0.8, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
		
	elseif ID == 801005 then									-- 라크시 필드 (117246 138074)
		Raremob_ID = {7039001,7040001,7041022,7043007,0,0}
		Raremob_count = { 5, 5, 5, 5, 1, 1 }
		Raremob_interval = { 1000, 1000, 1000, 1000, 2000, 3000 }
	
	
		--monster_ID = {7039001,7040001,0,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
		
	elseif ID == 801006 then									-- 카탄 필드(123394 60604)
		Raremob_ID = {7030005,7032006,7033005,0,0,0}
		Raremob_count = { 5, 5, 5, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
	
	
		--monster_ID = {7030005,7032006,7033005,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 801007 then									-- 카탄 필드 (122944 56430)
		Raremob_ID = {7030005,7032006,7033005,0,0,0}
		Raremob_count = { 5, 5, 5, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
	
	
		--monster_ID = {7030005,7032006,7033005,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
	
	elseif ID == 801008 then									-- 카탄 필드 (115678 50745)
		Raremob_ID = {7035019,7036017,7030017,0,0,0}
		Raremob_count = { 5, 5, 5, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
	
	
		--monster_ID = {7035019,7036017,7030017,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801009 then									-- 카탄 서쪽 (110317 62734)
		Raremob_ID = {7033007,7034005,7035010,0,0,0}
		Raremob_count = { 5, 5, 5, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }		
	
	
		--monster_ID = {7033007,7034005,7035010,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801010 then									-- 침몰의 늪지 (101849 62940)
		Raremob_ID = {7037007,7037006,7040008,7042021,0,0}
		Raremob_count = { 5, 5, 5, 5, 5, 1 }
		Raremob_interval = { 1000, 1000, 1000, 1000, 1000, 3000 }			
	
	
		--monster_ID = {7037007,7037006,7040008,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801011 then									-- 호라이즌 필드 (157643 77796)
		Raremob_ID = {7031009,7035018,7037023,0,0,0}
		Raremob_count = { 5, 5, 5, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }				
	
	
		--monster_ID = {7031009,7035018,7037023,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801012 then									-- 호라이즌 필드 (154784 71845)
		Raremob_ID = {7030013,7030012,7036019,0,0,0}
		Raremob_count = { 5, 5, 5, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }					
	
	
		--monster_ID = {7030013,7030012,7036019,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801013 then									-- 호라이즌 필드 (147461 72765)
		Raremob_ID = {7030014,7037023,7032012,0,0,0}
		Raremob_count = { 5, 5, 5, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }						
	
	
		--monster_ID = {7030014,7037023,7032012,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801014 then									-- 호라이즌 북서쪽 (142876 96571)							
		Raremob_ID = {7053009,7055009,7059006,0,0,0}
		Raremob_count = { 5, 5, 5, 1, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }							
	
	
		--monster_ID = {7053009,7055009,7059006,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 801015 then									-- 메마른 달빛의 유적 부근 (137165 92502)
		Raremob_ID = {7050013,7051009,7055011,7058007,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }								
	
	
		--monster_ID = {7050013,7051009,7055011,7058007,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 801016 then									-- 제1 발모어 탄광 (152167 91843)
		Raremob_ID = {7063003,7064002,7065004,0,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }									
	
	
		--monster_ID = {7063003,7064002,7065004,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
		
	elseif ID == 801017 then									-- 호라이즌 북서쪽 (150416 94792)		
		Raremob_ID = {7066001,7068001,7067003,0,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }										
	
	
		--monster_ID = {7066001,7068001,7067003,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
		
	elseif ID == 801018 then									-- 론도 동문 앞 필드 (141406 108919)
		Raremob_ID = {7076004,7078005,7079001,0,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }											
	
	
		--monster_ID = {7076004,7078005,7079001,0,0,0}		
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 801019 then									-- 론도 필드 (135624 97800)
		Raremob_ID = {7071003,7073001,7075004,0,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }												
	
	
		--monster_ID = {7071003,7073001,7075004,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
	
	elseif ID == 801020 then									-- 하르라이 목장 (142314 118100)
		Raremob_ID = {7078003,7079002,7077004,0,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }				
	
	
		--monster_ID = {7078003,7079002,7077004,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801021 then									-- 팔미르 고원 (134246 116584)
		Raremob_ID = {7073001,7075001,7072003,0,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }					
	
	
		--monster_ID = {7073001,7075001,7072003,0,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801022 then									-- 라크시 동쪽 (141334 138751)
		Raremob_ID = {7082003,7084004,7086004,7089002,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }						
	
	
		--monster_ID = {7082003,7084004,7086004,7089002,0,0}
		--density = { 0.4, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 801023 then									-- 마레 마을 인근 (168084 133585)
		Raremob_ID = {7092002,7098003,7100008,7096005,0,0}
		Raremob_count = { 5, 5, 5, 4, 1, 1 }
		Raremob_interval = { 1000, 1000, 1200, 1500, 2000, 3000 }							
	
	
		--monster_ID = {7092002,7098003,7100008,7096005,0,0}
		--density = { 0.8, 0.6, 0.6, 0.06, 0.04, 0.03 }
		--interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
	
	
	--=======================================================================	
	-- 베어로드 몬스터 그리고 테스트
	--=======================================================================			
		
	-- 베어로드 몬스터 테스트용
	elseif ID == 3001010 then
		monster_ID = {40,0,0,0,0,0}
		density = { 1.0, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 100, 1000, 1200, 1500, 2000, 3000 }
	
	-- 몬스터 리젠 테스트 11월11일
	elseif ID == 3001011 then
		monster_ID = {8000001,0,0,0,0,0}
		density = { 1.0, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 100, 1000, 1200, 1500, 2000, 3000 }
		
	--=======================================================================	
	-- 잃어버린 비밀의 섬 던전 ( 백룡 )
	--=======================================================================			
		
				
	elseif ID == 1401001 then
		monster_ID = {9146008,9148008,9149008,9150004,9147004,9148007}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9146008,9148008,9149008,9150004,9147004,9148007}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }

	elseif ID == 1401011 then
		monster_ID = {9150004,9151003,9152003,9152009,9151008,9152008}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9150004,9151003,9152003,9152009,9151008,9152008}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1401021 then
		monster_ID = {9150004,9151003,9152003,9152009,9151008,9152008}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9150004,9151003,9152003,9152009,9151008,9152008}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1401031 then
		monster_ID = {9148005,9149005,9150005,0,0,9150006}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9148005,9149005,9150005,9127001,9129001,9150006}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1401041 then
		monster_ID = {9148005,9149005,9150005,0,9129001,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9148005,9149005,9150005,0,9129001,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1401051 then
		monster_ID = {9148005,9149005,9150005,0,0,9150006}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9148005,9149005,9150005,0,0,9150006}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
		
	elseif ID == 1401061 then
		monster_ID = {9148005,9149005,9150005,0,9129001,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9148005,9149005,9150005,0,9129001,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			

	elseif ID == 1401071 then
		monster_ID = {9155006,9156006,9157006,9155011,9156010,9157010}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9155006,9156006,9157006,9155011,9156010,9157010}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1401081 then
		monster_ID = {9155006,9156006,9157006,9155011,9156010,9157010}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9155006,9156006,9157006,9155011,9156010,9157010}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1401091 then
		monster_ID = {9155013,9156012,9157006,9157012,0,9157013}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9155013,9156012,9157006,9157012,0,9157013}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1401101 then
		monster_ID = {9156006,9157006,0,0,0,9157013}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9156006,9157006,0,0,0,9157013}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1401111 then
		monster_ID = {9156006,9157006,0,0,0,9157013}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9156006,9157006,0,0,0,9157013}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
				
	elseif ID == 1401121 then
		monster_ID = {9155013,9156012,9157006,9157012,0,9157013}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9155013,9156012,9157006,9157012,0,9157013}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1401131 then
		monster_ID = {9155013,9156012,9157006,9157012,0,9157013}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9155013,9156012,9157006,9157012,0,9157013}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1401141 then
		monster_ID = {9156006,9157006,0,0,9157007,9157013}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9156006,9157006,0,0,9157007,9157013}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1401151 then
		monster_ID = {9156008,9157002,9157008,0,9157013,9157012}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9156008,9157002,9157008,0,9157013,9157012}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1401161 then
		monster_ID = {9156008,9157002,9157008,0,9157013,9156012}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9156008,9157002,9157008,0,9157013,9156012}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	

	elseif ID == 1401171 then
		monster_ID = {9156008,9157002,9157008,9157007,0,9157006}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9156008,9157002,9157008,9157007,0,9157006}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1403001 then
		monster_ID = {9148006,9148006,0,0,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 6000, 9000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9148006,9148006,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1403011 then
		monster_ID = {9148006,9148006,0,0,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 6000, 9000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9148006,9148006,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1402001 then
		monster_ID = {9153001,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9153001,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1402001 then
		monster_ID = {9150008,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9150008,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1402011 then
		monster_ID = {9150008,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9150008,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1402021 then
		monster_ID = {9157007,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9157007,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
			
	elseif ID == 1402031 then
		monster_ID = {9157007,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9157007,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1402041 then
		monster_ID = {9157007,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9157007,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	

	elseif ID == 1404001 then
		monster_ID = {9157014,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9157014,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1404011 then
		monster_ID = {9157015,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9157015,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1404021 then
		monster_ID = {9158001,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9158001,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1404031  then
		Raremob_ID = {9158002, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9158002, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }
		
		
	--=======================================================================	
	-- 잃어버린 비밀의 섬 던전 (흑룡)
	--=======================================================================			
		
				
	elseif ID == 1501001 then
		monster_ID = {9158003,9159003,9161002,9161003,9158004,9160002}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9158003,9159003,9161002,9161003,9158004,9160002}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }

	elseif ID == 1501011 then
		monster_ID = {9161003,9162003,9162005,9163004,9162004,9163004}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9161003,9162003,9162005,9163004,9162004,9163004}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1501021 then
		monster_ID = {9161003,9162003,9162005,9163004,9162004,9163004}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9161003,9162003,9162005,9163004,9162004,9163004}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1501031 then
		monster_ID = {9159002,9160004,9162002,0,0,9161005}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9159002,9160004,9162002,0,0,9161005}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1501041 then
		monster_ID = {9159002,9160004,9162002,0,9160003,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9159002,9160004,9162002,0,9160003,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1501051 then
		monster_ID = {9159002,9160004,9162002,0,0,9161005}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9159002,9160004,9162002,0,0,9161005}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
		
	elseif ID == 1501061 then
		monster_ID = {9159002,9160004,9162002,0,9160003,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9159002,9160004,9162002,0,9160003,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			

	elseif ID == 1501071 then
		monster_ID = {9164001,9166002,9168002,9165003,9167004,9169004}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9164001,9166002,9168002,9165003,9167004,9169004}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1501081 then
		monster_ID = {9164001,9166002,9168002,9165003,9167004,9169004}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9164001,9166002,9168002,9165003,9167004,9169004}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1501091 then
		monster_ID = {9165002,9167002,9168002,9169002,0,9169003}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9165002,9167002,9168002,9169002,0,9169003}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1501101 then
		monster_ID = {9166002,9168002,0,0,0,9169003}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9166002,9168002,0,0,0,9169003}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1501111 then
		monster_ID = {9166002,9168002,0,0,0,9169003}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9166002,9168002,0,0,0,9169003}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
				
	elseif ID == 1501121 then
		monster_ID = {9165002,9167002,9168002,9169002,0,9169003}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9165002,9167002,9168002,9169002,0,9169003}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1501131 then
		monster_ID = {9165002,9167002,9168002,9169002,0,9169003}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9165002,9167002,9168002,9169002,0,9169003}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1501141 then
		monster_ID = {9166002,9168002,0,0,9168003,9169003}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9166002,9168002,0,0,9168003,9169003}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1501151 then
		monster_ID = {9167005,9168004,9169005,0,9169003,9169002}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9167005,9168004,9169005,0,9169003,9169002}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1501161 then
		monster_ID = {9167005,9168004,9169005,0,9169003,9167002}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9167005,9168004,9169005,0,9169003,9167002}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	

	elseif ID == 1501171 then
		monster_ID = {9167005,9168004,9169005,9168003,0,9168002}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9167005,9168004,9169005,9168003,0,9168002}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1503001 then
		monster_ID = {9158005,9159004,0,0,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 6000, 9000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9158005,9159004,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1503011 then
		monster_ID = {9158005,9159004,0,0,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 6000, 9000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9158005,9159004,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1502001 then
		monster_ID = {9161004,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9161004,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1502011 then
		monster_ID = {9161004,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9161004,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1502021 then
		monster_ID = {9168003,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9168003,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
			
	elseif ID == 1502031 then
		monster_ID = {9169003,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9169003,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1504001 then
		monster_ID = {9169005,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9169005,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1504011 then
		monster_ID = {9169005,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9169005,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1504021 then
		monster_ID = {9169005,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9169005,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1504031  then
		Raremob_ID = {9170001,0,0,0,0,0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9170001,0,0,0,0,0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }
		
		
		
		
	--=======================================================================	
	-- 잃어버린 비밀의 섬 던전 (사룡)
	--=======================================================================			
		
				
	elseif ID == 1601001 then
		monster_ID = {9165005,9165004,9166004,9166004,9166004,9166004}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9165005,9165004,9166004,9166004,9166004,9166004}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }

	elseif ID == 1601011 then
		monster_ID = {9166004,9166004,9166005,9166005,9166005,9166005}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9166004,9166004,9166005,9166005,9166005,9166005}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }
		
	elseif ID == 1601021 then
		monster_ID = {9167008,9167006,9167007,9167007,9168005,9168005}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9167008,9167006,9167007,9167007,9168005,9168005}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1601031 then
		monster_ID = {9168005,9168005,9168006,0,0,9168006}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9168005,9168005,9168006,0,0,9168006}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1601041 then
		monster_ID = {9170002,9170002,9170002,0,9170003,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9170002,9170002,9170002,0,9170003,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1601051 then
		monster_ID = {9170004,9170004,9170004,0,0,9170005}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9170004,9170004,9170004,0,0,9170005}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
		
	elseif ID == 1601061 then
		monster_ID = {9170005,9170005,9172001,0,9173001,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9170005,9170005,9172001,0,9173001,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			

	elseif ID == 1601071 then
		monster_ID = {9174001,9174001,9175003,9175003,9175001,9175001}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9174001,9174001,9175003,9175003,9175001,9175001}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1601081 then
		monster_ID = {9175002,9180001,9180001,9180002,9180002,9180002}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9175002,9180001,9180001,9180002,9180002,9180002}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1601091 then
		monster_ID = {9181001,9181001,9181001,9181002,0,9182001}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9181001,9181001,9181001,9181002,0,9182001}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1601101 then
		monster_ID = {9182001,9182001,0,0,0,9182002}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9182001,9182001,0,0,0,9182002}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1601111 then
		monster_ID = {9182002,9183001,0,0,0,9183004}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9182002,9183001,0,0,0,9183004}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
				
	elseif ID == 1601121 then
		monster_ID = {9183004,9183004,9183002,9183002,0,9183003}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9183004,9183004,9183002,9183002,0,9183003}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1601131 then
		monster_ID = {9183003,9184001,9184001,9184001,0,9184001}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9183003,9184001,9184001,9184001,0,9184001}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1601141 then
		monster_ID = {9184001,9184001,0,0,9184003,9184003}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9184001,9184001,0,0,9184003,9184003}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1601151 then
		monster_ID = {9184003,9184002,9184002,0,9184002,9184002}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9184003,9184002,9184002,0,9184002,9184002}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1601161 then
		monster_ID = {9184002,9184002,9185001,0,9185001,9185001}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9184002,9184002,9185001,0,9185001,9185001}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	

	elseif ID == 1601171 then
		monster_ID = {9185004,9185004,9185004,9185004,0,9185004}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185004,9185004,9185004,9185004,0,9185004}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1603001 then
		monster_ID = {9185002,9185002,0,0,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 6000, 9000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185002,9185002,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1603011 then
		monster_ID = {9185002,9185002,0,0,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.2, 0.2 }
		interval = { 6000, 9000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185002,9185002,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
	

	elseif ID == 1602001 then
		monster_ID = {9185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1602011 then
		monster_ID = {9185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1602021 then
		monster_ID = {9185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
			
	elseif ID == 1602031 then
		monster_ID = {9185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1602041 then
		monster_ID = {9185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	

	elseif ID == 1604001 then
		monster_ID = {9185002,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1604011 then
		monster_ID = {9185003,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185003,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1604021 then
		monster_ID = {9185003,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9185003,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1604031  then
		Raremob_ID = {9190001,0,0,0,0,0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {9190001,0,0,0,0,0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }	
	
	
	
	
		
		
				
	--=======================================================================	
	-- 월드 환경 몬스터(메갈로케로스는 크리스마스 이벤트에 영향)
	--=======================================================================			
		
	elseif ID == 1001 then -- 수련자의 섬 문래빗		
			monster_ID = {0, 0, 0, 0, 1000004, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1002 then -- 수련자의 섬 타미		
			monster_ID = {0, 0, 0, 0, 1000007, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1003 then -- 수련자의 섬 오비스		
			monster_ID = {0, 0, 0, 0, 1000005, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1004  then -- 수련자의 섬 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000006, 0}
		else	
			monster_ID = {0, 0, 0, 0, 1100006, 0}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1005  then -- 수련자의 섬 문래빗, 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000004, 1000006}
		else	
			monster_ID = {0, 0, 0, 0, 1100004, 1100006}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1006 then -- 호라이즌 타미		
			monster_ID = {0, 0, 0, 0, 1000009, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1007 then -- 호라이즌 문래빗		
			monster_ID = {0, 0, 0, 0, 1000008, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1008 then -- 호라이즌 문래빗2		
			monster_ID = {0, 0, 0, 0, 1000011, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1009  then -- 호라이즌 문래빗2, 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000011, 1000010}
		else	
			monster_ID = {0, 0, 0, 0, 1100011, 1100010}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }	

	elseif ID == 1011 then -- 라크시 문래빗		
			monster_ID = {0, 0, 0, 0, 1000013, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1012 then -- 라크시 쿠쿠리		
			monster_ID = {0, 0, 0, 0, 1000014, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1013  then -- 라크시 문래빗, 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000015, 1000013}
		else	
			monster_ID = {0, 0, 0, 0, 1100015, 1100013}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1014  then -- 라크시 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000015, 0}
		else	
			monster_ID = {0, 0, 0, 0, 1100015, 0}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
			
	elseif ID == 1016  then -- 라크시 오비스, 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000015, 0}
		else	
			monster_ID = {0, 0, 0, 0, 1100015, 0}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1017 then -- 사막 타미		
			monster_ID = {0, 0, 0, 0, 1000018, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1018 then -- 사막 문래빗, 타미		
			monster_ID = {0, 0, 0, 0, 1000017, 1000018}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1019  then -- 침엽수림 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000019, 0}
		else	
			monster_ID = {0, 0, 0, 0, 1100019, 0}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1020 then -- 론도 문래빗		
			monster_ID = {0, 0, 0, 0, 1000020, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1021 then -- 론도 꾸꾸리1		
			monster_ID = {0, 0, 0, 0, 1000021, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1022 then -- 론도 꾸꾸리2		
			monster_ID = {0, 0, 0, 0, 1000022, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1023 then -- 론도 꾸꾸리3		
			monster_ID = {0, 0, 0, 0, 1000023, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1024 then -- 론도 꾸꾸리1, 문래빗		
			monster_ID = {0, 0, 0, 0, 1000021, 1000020}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1025  then -- 론도 문래빗, 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000025, 1000020}
		else	
			monster_ID = {0, 0, 0, 0, 1100025, 1100020}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1026 then -- 론도 꾸꾸리4		
			monster_ID = {0, 0, 0, 0, 1000024, 0}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1027  then -- 해안가 메갈로, 오비스		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 0, 1000026}
		else	
			monster_ID = {0, 0, 0, 0, 1100027, 1100026}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1028  then -- 하르라이 목장 문래빗, 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000029, 1000028}
		else	
			monster_ID = {0, 0, 0, 0, 1100029, 1100028}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1029 then -- 하르라이 목장 에쿠르스		
			monster_ID = {1000030, 1000031, 1000032, 1000033, 0, 0}
			density = { 0.03, 0.03, 0.03, 0.03, 0.03, 0.03 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
		
	elseif ID == 1031 then -- 칠흑의 숲 타미, 문래빗		
			monster_ID = {0, 0, 0, 0, 1000036, 1000035}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1032  then -- 칠흑의 숲 오비스, 메갈로, 타미		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 1000036, 1000037, 1000038}
		else	
			monster_ID = {0, 0, 0, 1100036, 1100037, 1000038}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1033 then -- 마르두카군락지 꾸꾸리		
			monster_ID = {0, 0, 0, 0, 0, 1000039}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1034 then -- 마르두카군락지 문래빗, 타미		
			monster_ID = {0, 0, 0, 0, 1000040, 1000041}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1035  then -- 마르두카군락지 메갈로, 타미		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000041, 1000042}
		else	
			monster_ID = {0, 0, 0, 0, 1100041, 1100042}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1036 then -- 도시 유적 꾸꾸리		
			monster_ID = {0, 0, 0, 0, 0, 1000043}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1037 then -- 도시 유적 문래빗		
			monster_ID = {0, 0, 0, 0, 0, 1000044}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1038 and rangifer_on == 1 then -- 카탄 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 0, 1000045}
		else	
			monster_ID = {0, 0, 0, 0, 0, 1100045}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
	elseif ID == 1039 then -- 카탄 타미		
			monster_ID = {0, 0, 0, 0, 0, 1000046}
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
			
	elseif ID == 1041 and rangifer_on == 1 then -- 카탄 타미, 메갈로		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 0, 1000045, 1000046}
		else	
			monster_ID = {0, 0, 0, 0, 1100045, 1100046}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }
			
			
	elseif ID == 1042 and rangifer_on == 1 then -- 카탄 타미, 메갈로, 오비스		
		if rangifer_on == 0 then	
			monster_ID = {0, 0, 0, 1000045, 1000046, 0}
		else	
			monster_ID = {0, 0, 0, 1100045, 1100046, 1100047}
		end	
			
			density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
			interval = { 1000, 1000, 1000, 1000, 1000, 1000 }

--=======================================================================		
	--[[ 2014 부활절 이벤트 (할로윈 맵)
	--=======================================================================		
			
	elseif ID == 1200001 then		
			
			monster_ID = { 21190169, 21190170, 21190173, 21190173, 21190169, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200007 then		
			
			monster_ID = { 21190170, 21190173, 21190169, 0, 0, 21190170 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200013 then		
			
			monster_ID = { 21190171, 21190173, 21190174, 21190174, 21190175, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200019 then		
			
			monster_ID = { 21190177, 21190179, 0, 21190177, 21190179, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200025 then		
			
			monster_ID = { 21190181, 21190182, 21190183, 21190184, 21190185, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200031 then		
			
			monster_ID = { 21190187, 21190192, 21190188, 0, 0, 21190192 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200037 then		
			
			monster_ID = { 21190194, 21190194, 21190195, 21190195, 0, 21190194 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200043 then		
			
			monster_ID = { 21190202, 21190196, 21190198, 21190199, 21190201, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200049 then		
			
			monster_ID = { 21190203, 21190206, 21190208, 21190208, 21190210, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200055 then		
			
			monster_ID = { 21190211, 21190212, 0, 21190214, 21190215, 21190218 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200061 then		
			
			monster_ID = { 21190219, 21190224, 21190222, 21190223, 0, 21190224 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200067 then		
			
			monster_ID = { 21190226, 21190227, 21190230, 21190230, 21190232, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200073 then		
			
			monster_ID = { 21190233, 21190234, 21190234, 0, 21190236, 21190237 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200079 then		
			
			monster_ID = { 21190238, 21190238, 21190239, 21190241, 21190242, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200085 then		
			
			monster_ID = { 21190243, 21190243, 21190244, 21190245, 21190246, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100091 then		
			
			monster_ID = { 0, 21190250, 21190250, 21190252, 21190254, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100097 then		
			
			monster_ID = { 21190253, 21190255, 21190256, 21190258, 21190260, 21190262 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100103 then		
			
			monster_ID = { 21190264, 21190266, 21190267, 21190268, 21190268, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100109 then		
			
			monster_ID = { 21190272, 21190273, 21190273, 21190274, 21190275, 21190276 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100115 then		
			
			monster_ID = { 21190277, 21190278, 21190280, 21190281, 0, 21190284 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100121 then		
			
			monster_ID = { 21190285, 21190286, 21190291, 21190292, 21190292, 21190294 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100127 then		
			
			monster_ID = { 21190291, 21190297, 21190298, 21190300, 21190294, 21190303 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100133 then		
			
			monster_ID = { 21190307, 0, 21190308, 21190311, 21190312, 21190313 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100139 then		
			
			monster_ID = { 21190314, 21190315, 21190316, 21190317, 21190318, 21190319 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100145 then		
			
			monster_ID = { 21190320, 21190321, 21190322, 21190323, 21190324, 21190325 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200002 then		
			
			monster_ID = { 21190169, 21190169, 21190170, 21190170, 21190171, 21190171 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200008 then		
			
			monster_ID = { 21190172, 21190172, 21190173, 21190173, 21190174, 21190174 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200014 then		
			
			monster_ID = { 21190172, 21190172, 21190175, 21190175, 21190176, 21190176 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200020 then		
			
			monster_ID = { 21190177, 21190177, 21190178, 21190178, 21190180, 21190181 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200026 then		
			
			monster_ID = { 21190182, 21190183, 21190184, 21190186, 21190185, 21190187 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200032 then		
			
			monster_ID = { 21190187, 21190188, 21190189, 21190190, 21190191, 21190192 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200038 then		
			
			monster_ID = { 21190192, 21190193, 21190193, 0, 21190194, 21190195 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200044 then		
			
			monster_ID = { 21190197, 21190195, 21190200, 21190201, 21190202, 3049002 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 400, 600, 800, 1000, 1200, 1500 } 	
			
	elseif ID == 1200050 then		
			
			monster_ID = { 3050001, 21190207, 21190209, 21190210, 21190210, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200056 then		
			
			monster_ID = { 21190212, 21190213, 21190220, 21190216, 21190217, 21190220 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200062 then		
			
			monster_ID = { 21190221, 21190221, 21190222, 21190223, 21190225, 21190225 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200068 then		
			
			monster_ID = { 21190228, 21190229, 21190231, 21190232, 0, 21190233 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200074 then		
			
			monster_ID = { 21190233, 21190234, 21190235, 21190235, 21190236, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200080 then		
			
			monster_ID = { 21190238, 21190240, 21190239, 21190241, 21190243, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200086 then		
			
			monster_ID = { 21190244, 21190245, 21190244, 21190246, 21190247, 21190248 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100092 then		
			
			monster_ID = { 21190249, 21190251, 21190253, 21190253, 21190255, 0 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100098 then		
			
			monster_ID = { 21190265, 21190257, 21190259, 21190261, 21190263, 21190265 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100104 then		
			
			monster_ID = { 21190266, 21190267, 21190269, 21190270, 21190271, 21190273 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100110 then		
			
			monster_ID = { 0, 21190273, 21190274, 21190275, 21190276, 21190277 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100116 then		
			
			monster_ID = { 21190278, 21190279, 21190282, 21190283, 0, 21190285 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100122 then		
			
			monster_ID = { 21190286, 21190287, 21190288, 21190292, 21190293, 21190294 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100128 then		
			
			monster_ID = { 21190301, 21190302, 21190304, 21190294, 21190296, 21190296 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100134 then		
			
			monster_ID = { 21190309, 21190309, 21190311, 21190309, 21190312, 21190313 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100140 then		
			
			monster_ID = { 21190314, 21190315, 21190316, 21190317, 21190318, 21190319 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100146 then		
			
			monster_ID = { 21190320, 21190321, 21190322, 21190323, 21190324, 21190325 }
			density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
			interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
	]]	
		
--[[=======================================================================		
	-- 할로윈 원더랜드(할로윈 이벤트 월드)		2011 리뉴얼 코스튬 리스폰 제외버전
	--=======================================================================		
			
	elseif ID == 1200001 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3011002, 3012002, 3014002, 3014002, 3011002, 0 }
		else	
			monster_ID = { 4001001, 4002001, 4003002, 4004001, 4005001, 4006001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200007 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3012002, 3014002, 3011002, 0, 0, 3012002 }
		else	
			monster_ID = { 4007001, 4008002, 4009002, 4010002, 4011001, 4012002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200013 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3013001, 3014002, 3016002, 3016002, 3017002, 0 }
		else	
			monster_ID = { 4013001, 4014002, 4015001, 4016002, 4017002, 4018001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200019 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3022001, 3023002, 0, 3022001, 3023002, 0 }
		else	
			monster_ID = { 4019001, 4020001, 4021002, 4022001, 4023002, 4024001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200025 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3025001, 3026002, 3027002, 3028002, 3029001, 0 }
		else	
			monster_ID = { 4025001, 4026002, 4027002, 4028002, 4029001, 4030002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200031 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3031001, 3036002, 3033002, 0, 0, 3036002 }
		else	
			monster_ID = { 4031001, 4032001, 4033002, 4034001, 4035001, 4036002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200037 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3042002, 3042002, 3043002, 3043002, 0, 3042002 }
		else	
			monster_ID = { 4037001, 4038001, 4039001, 4040001, 4041001, 4042002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200043 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3048002, 3044001, 3045002, 3046001, 3047002, 0 }
		else	
			monster_ID = { 4043001, 4044001, 4045002, 4046001, 4047002, 4048002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200049 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3049001, 3050002, 3051002, 3051002, 3053002, 0 }
		else	
			monster_ID = { 4049001, 4050002, 4051002, 4052001, 4053002, 4054001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200055 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3055002, 3056001, 0, 3058002, 3059001, 3060002 }
		else	
			monster_ID = { 4055002, 4056001, 4057001, 4058002, 4059001, 4060002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200061 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3061001, 3066002, 3063002, 3064002, 0, 3066002 }
		else	
			monster_ID = { 4061001, 4062001, 4063002, 4064002, 4065001, 4066002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200067 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3067002, 3068001, 3069002, 3069002, 3071001, 0 }
		else	
			monster_ID = { 4067002, 4068001, 4069002, 4070001, 4071001, 4072001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200073 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3073001, 3074002, 3074002, 0, 3077002, 3078002 }
		else	
			monster_ID = { 4073001, 4074002, 4075002, 4076001, 4077002, 4078002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200079 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3080002, 3080002, 3081001, 3082002, 3083002, 0 }
		else	
			monster_ID = { 4079001, 4080002, 4081001, 4082002, 4083002, 4084001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200085 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3084002, 3084002, 3088001, 3088002, 3089002, 0 }
		else	
			monster_ID = { 4085001, 4086001, 4087001, 4088002, 4089002, 4090001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100091 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 0, 3092002, 3092002, 3094001, 3095002, 0 }
		else	
			monster_ID = { 4091001, 4092002, 4093001, 4094001, 4095002, 4096001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100097 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3094002, 3096002, 3099001, 3100001, 3101001, 3102001 }
		else	
			monster_ID = { 4097001, 4098001, 4099001, 4100001, 4101001, 4102001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100103 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3103001, 3104002, 3105002, 3106001, 3106001, 0 }
		else	
			monster_ID = { 4103001, 4104002, 4105002, 4106001, 4107001, 4108001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100109 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3109001, 3109002, 3109002, 3112002, 3113002, 3114002 }
		else	
			monster_ID = { 4109001, 4110001, 4111002, 4112001, 4113002, 4114002 } 
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100115 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3115002, 3116002, 3117002, 3118001, 0, 3120002 }
		else	
			monster_ID = { 4115001, 4116002, 4117002, 4118001, 4119001, 4120002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100121 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3121001, 3122002, 3126002, 3128002, 3128002, 3131002 }
		else	
			monster_ID = { 4121001, 4122002, 4123002, 4124001, 4125002, 4126002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100127 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3126002, 3133002, 3134002, 3136001, 3131002, 3137002 }
		else	
			monster_ID = { 4127001, 4128002, 4129001, 4130001, 4131002, 4132001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100133 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3139002, 0, 3141002, 3145002, 3147002, 3148002 }
		else	
			monster_ID = { 4133002, 4134002, 4135002, 4136001, 4137002, 4138002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100139 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3150001, 3150002, 3150003, 3150004, 3150005, 3150006 }
		else	
			monster_ID = { 4139002, 4140002, 4141002, 4142001, 4143001, 4144002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100145 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3150007, 3150008, 3150009, 3150010, 3150011, 3150012 }
		else	
			monster_ID = { 4145001, 4146001, 4147001, 4148002, 4149001, 4150002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200002 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3011002, 3011002, 3012002, 3012002, 3013001, 3013001 }
		else	
			monster_ID = { 4002002, 4003001, 4004002, 4005002, 4006002, 4007002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200008 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3014001, 3014001, 3014002, 3014002, 3016002, 3016002 }
		else	
			monster_ID = { 4008001, 4009001, 4010001, 4011002, 4012001, 4013001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200014 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3014001, 3014001, 3017002, 3017002, 3018002, 3018002 }
		else	
			monster_ID = { 4014001, 4015002, 4016001, 4017001, 4018002, 4019001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200020 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3022001, 3022001, 3022002, 3022002, 3024002, 3025001 }
		else	
			monster_ID = { 4020002, 4021001, 4022002, 4023001, 4024002, 4025001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200026 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3026002, 3027002, 3028002, 3029002, 3029001, 3031001 }
		else	
			monster_ID = { 4026001, 4027001, 4028001, 4029002, 4030001, 4031001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200032 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3031001, 3033002, 3034002, 3035002, 3036001, 3036002 }
		else	
			monster_ID = { 4032001, 4033001, 4034002, 4035002, 4036001, 4037002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200038 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3036002, 3039002, 3039002, 0, 3042002, 3043002 }
		else	
			monster_ID = { 4038001, 4039002, 4040001, 4041002, 4042001, 4043002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200044 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3044002, 3043002, 3046002, 3047002, 3048002, 3049002 }
		else	
			monster_ID = { 4044002, 4045001, 4046002, 4047001, 4048001, 4049002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 } 	
			
	elseif ID == 1200050 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3050001, 3051001, 3052002, 3053002, 3053002, 0 }
		else	
			monster_ID = { 4050001, 4051001, 4052002, 4053001, 4054002, 4055001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200056 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3056001, 3057002, 3061002, 3059002, 3060001, 3061002 }
		else	
			monster_ID = { 4056001, 4057002, 4058001, 4059002, 4060001, 4061002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200062 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3062002, 3062002, 3063002, 3064002, 3067001, 3067001 }
		else	
			monster_ID = { 4062002, 4063001, 4064001, 4065002, 4066001, 4067001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200068 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3068002, 3069001, 3070002, 3071001, 0, 3073001 }
		else	
			monster_ID = { 4068002, 4069001, 4070002, 4071001, 4072002, 4073001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200074 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3073001, 3074002, 3076002, 3076002, 3077002, 0 }
		else	
			monster_ID = { 4074001, 4075001, 4076002, 4077001, 4078001, 4079001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200080 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3080002, 3081002, 3081001, 3082002, 3084002, 0 }
		else	
			monster_ID = { 4080001, 4081002, 4082001, 4083001, 4084002, 4085002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200086 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3088001, 3088002, 3088001, 3089002, 3090002, 3091002 }
		else	
			monster_ID = { 4086001, 4087002, 4088001, 4089001, 4090002, 4091002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100092 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3092001, 3093002, 3094002, 3094002, 3096002, 0 }
		else	
			monster_ID = { 4092001, 4093002, 4094002, 4095001, 4096002, 4097001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100098 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3103002, 3099002, 3100002, 3101002, 3102002, 3103002 }
		else	
			monster_ID = { 4098001, 4099002, 4100002, 4101002, 4102002, 4103002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100104 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3104002, 3105002, 3106002, 3107002, 3108002, 3109002 }
		else	
			monster_ID = { 4104001, 4105001, 4106002, 4107002, 4108002, 4109002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100110 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 0, 3109002, 3112002, 3113002, 3114002, 3115002 }
		else	
			monster_ID = { 4110002, 4111001, 4112002, 4113001, 4114001, 4115002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100116 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3116002, 3117001, 3118002, 3119002, 0, 3121001 }
		else	
			monster_ID = { 4116001, 4117001, 4118002, 4119002, 4120001, 4121001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100122 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3122002, 3123002, 3124002, 3128002, 3129002, 3130002 }
		else	
			monster_ID = { 4122001, 4123001, 4124002, 4125001, 4126001, 4127002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100128 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3136002, 3137001, 3138001, 3131002, 3133001, 3133001 }
		else	
			monster_ID = { 4128001, 4129002, 4130002, 4131001, 4132002, 4133001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100134 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3143002, 3143002, 3145002, 3143002, 3147002, 3148002 }
		else	
			monster_ID = { 4134001, 4135001, 4136002, 4137001, 4138001, 4139001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100140 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3150001, 3150002, 3150003, 3150004, 3150005, 3150006 }
		else	
			monster_ID = { 4140001, 4141001, 4142002, 4143002, 4144001, 4145002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100146 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3150007, 3150008, 3150009, 3150010, 3150011, 3150012 }
		else	
			monster_ID = { 4146002, 4147002, 4148001, 4149002, 4150001, 4150001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
		
	]]--	
	--[[
	--=======================================================================		
	-- 할로윈 원더랜드(할로윈 이벤트 월드)		리뉴얼 전 리스폰 
	--=======================================================================		
			
	elseif ID == 1200001 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3001001, 3002001, 3003002, 3004001, 3005001, 3006001 }
		else	
			monster_ID = { 4001001, 4002001, 4003002, 4004001, 4005001, 4006001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200007 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3007001, 3008002, 3009002, 3010002, 3011001, 3012002 }
		else	
			monster_ID = { 4007001, 4008002, 4009002, 4010002, 4011001, 4012002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200013 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3013001, 3014002, 3015001, 3016002, 3017002, 3018001 }
		else	
			monster_ID = { 4013001, 4014002, 4015001, 4016002, 4017002, 4018001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200019 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3019001, 3020001, 3021002, 3022001, 3023002, 3024001 }
		else	
			monster_ID = { 4019001, 4020001, 4021002, 4022001, 4023002, 4024001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200025 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3025001, 3026002, 3027002, 3028002, 3029001, 3030002 }
		else	
			monster_ID = { 4025001, 4026002, 4027002, 4028002, 4029001, 4030002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200031 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3031001, 3032001, 3033002, 3034001, 3035001, 3036002 }
		else	
			monster_ID = { 4031001, 4032001, 4033002, 4034001, 4035001, 4036002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200037 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3037001, 3038001, 3039001, 3040001, 3041001, 3042002 }
		else	
			monster_ID = { 4037001, 4038001, 4039001, 4040001, 4041001, 4042002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200043 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3043001, 3044001, 3045002, 3046001, 3047002, 3048002 }
		else	
			monster_ID = { 4043001, 4044001, 4045002, 4046001, 4047002, 4048002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200049 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3049001, 3050002, 3051002, 3052001, 3053002, 3054001 }
		else	
			monster_ID = { 4049001, 4050002, 4051002, 4052001, 4053002, 4054001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200055 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3055002, 3056001, 3057001, 3058002, 3059001, 3060002 }
		else	
			monster_ID = { 4055002, 4056001, 4057001, 4058002, 4059001, 4060002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200061 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3061001, 3062001, 3063002, 3064002, 3065001, 3066002 }
		else	
			monster_ID = { 4061001, 4062001, 4063002, 4064002, 4065001, 4066002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200067 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3067002, 3068001, 3069002, 3070001, 3071001, 3072001 }
		else	
			monster_ID = { 4067002, 4068001, 4069002, 4070001, 4071001, 4072001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200073 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3073001, 3074002, 3075002, 3076001, 3077002, 3078002 }
		else	
			monster_ID = { 4073001, 4074002, 4075002, 4076001, 4077002, 4078002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200079 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3079001, 3080002, 3081001, 3082002, 3083002, 3084001 }
		else	
			monster_ID = { 4079001, 4080002, 4081001, 4082002, 4083002, 4084001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200085 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3085001, 3086001, 3087001, 3088002, 3089002, 3090001 }
		else	
			monster_ID = { 4085001, 4086001, 4087001, 4088002, 4089002, 4090001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100091 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3091001, 3092002, 3093001, 3094001, 3095002, 3096001 }
		else	
			monster_ID = { 4091001, 4092002, 4093001, 4094001, 4095002, 4096001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100097 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3097001, 3098001, 3099001, 3100001, 3101001, 3102001 }
		else	
			monster_ID = { 4097001, 4098001, 4099001, 4100001, 4101001, 4102001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100103 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3103001, 3104002, 3105002, 3106001, 3107001, 3108001 }
		else	
			monster_ID = { 4103001, 4104002, 4105002, 4106001, 4107001, 4108001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100109 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3109001, 3110001, 3111002, 3112001, 3113002, 3114002 }
		else	
			monster_ID = { 4109001, 4110001, 4111002, 4112001, 4113002, 4114002 } 
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100115 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3115001, 3116002, 3117002, 3118001, 3119001, 3120002 }
		else	
			monster_ID = { 4115001, 4116002, 4117002, 4118001, 4119001, 4120002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100121 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3121001, 3122002, 3123002, 3124001, 3125002, 3126002 }
		else	
			monster_ID = { 4121001, 4122002, 4123002, 4124001, 4125002, 4126002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100127 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3127001, 3128002, 3129001, 3130001, 3131002, 3132001 }
		else	
			monster_ID = { 4127001, 4128002, 4129001, 4130001, 4131002, 4132001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100133 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3133002, 3134002, 3135002, 3136001, 3137002, 3138002 }
		else	
			monster_ID = { 4133002, 4134002, 4135002, 4136001, 4137002, 4138002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100139 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3139002, 0, 3141002, 3142001, 3143001, 3144002 }
		else	
			monster_ID = { 4139002, 4140002, 4141002, 4142001, 4143001, 4144002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100145 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3145001, 3146001, 3147001, 3148002, 3149001, 3150002 }
		else	
			monster_ID = { 4145001, 4146001, 4147001, 4148002, 4149001, 4150002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200002 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3002002, 3003001, 3004002, 3005002, 3006002, 3007002 }
		else	
			monster_ID = { 4002002, 4003001, 4004002, 4005002, 4006002, 4007002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200008 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3008001, 3009001, 3010001, 3011002, 3012001, 3013001 }
		else	
			monster_ID = { 4008001, 4009001, 4010001, 4011002, 4012001, 4013001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200014 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3014001, 3015002, 3016001, 3017001, 3018002, 3019001 }
		else	
			monster_ID = { 4014001, 4015002, 4016001, 4017001, 4018002, 4019001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 }	
			
	elseif ID == 1200020 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3020002, 3021001, 3022002, 3023001, 3024002, 3025001 }
		else	
			monster_ID = { 4020002, 4021001, 4022002, 4023001, 4024002, 4025001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200026 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3026001, 3027001, 3028001, 3029002, 3030001, 3031001 }
		else	
			monster_ID = { 4026001, 4027001, 4028001, 4029002, 4030001, 4031001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200032 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3032001, 3033001, 3034002, 3035002, 3036001, 3037002 }
		else	
			monster_ID = { 4032001, 4033001, 4034002, 4035002, 4036001, 4037002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200038 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3038001, 3039002, 3040001, 3041002, 3042001, 3043002 }
		else	
			monster_ID = { 4038001, 4039002, 4040001, 4041002, 4042001, 4043002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200044 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3044002, 3045001, 3046002, 3047001, 3048001, 3049002 }
		else	
			monster_ID = { 4044002, 4045001, 4046002, 4047001, 4048001, 4049002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 400, 600, 800, 1000, 1200, 1500 } 	
			
	elseif ID == 1200050 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3050001, 3051001, 3052002, 3053001, 3054002, 3055001 }
		else	
			monster_ID = { 4050001, 4051001, 4052002, 4053001, 4054002, 4055001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200056 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3056001, 3057002, 3058001, 3059002, 3060001, 3061002 }
		else	
			monster_ID = { 4056001, 4057002, 4058001, 4059002, 4060001, 4061002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200062 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3062002, 3063001, 3064001, 3065002, 3066001, 3067001 }
		else	
			monster_ID = { 4062002, 4063001, 4064001, 4065002, 4066001, 4067001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200068 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3068002, 3069001, 3070002, 3071001, 3072002, 3073001 }
		else	
			monster_ID = { 4068002, 4069001, 4070002, 4071001, 4072002, 4073001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200074 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3074001, 3075001, 3076002, 3077001, 3078001, 3079001 }
		else	
			monster_ID = { 4074001, 4075001, 4076002, 4077001, 4078001, 4079001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200080 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3080001, 3081002, 3082001, 3083001, 3084002, 3085002 }
		else	
			monster_ID = { 4080001, 4081002, 4082001, 4083001, 4084002, 4085002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1200086 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3086001, 3087002, 3088001, 3089001, 3090002, 3091002 }
		else	
			monster_ID = { 4086001, 4087002, 4088001, 4089001, 4090002, 4091002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100092 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3092001, 3093002, 3094002, 3095001, 3096002, 3097001 }
		else	
			monster_ID = { 4092001, 4093002, 4094002, 4095001, 4096002, 4097001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100098 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3098001, 3099002, 3100002, 3101002, 3102002, 3103002 }
		else	
			monster_ID = { 4098001, 4099002, 4100002, 4101002, 4102002, 4103002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 } 	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100104 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3104001, 3105001, 3106002, 3107002, 3108002, 3109002 }
		else	
			monster_ID = { 4104001, 4105001, 4106002, 4107002, 4108002, 4109002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100110 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 0, 3111001, 3112002, 3113001, 3114001, 3115002 }
		else	
			monster_ID = { 4110002, 4111001, 4112002, 4113001, 4114001, 4115002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100116 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3116001, 3117001, 3118002, 3119002, 3120001, 3121001 }
		else	
			monster_ID = { 4116001, 4117001, 4118002, 4119002, 4120001, 4121001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100122 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3122001, 3123001, 3124002, 3125001, 3126001, 3127002 }
		else	
			monster_ID = { 4122001, 4123001, 4124002, 4125001, 4126001, 4127002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100128 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3128001, 3129002, 3130002, 3131001, 3132002, 3133001 }
		else	
			monster_ID = { 4128001, 4129002, 4130002, 4131001, 4132002, 4133001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100134 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3134001, 3135001, 3136002, 3137001, 3138001, 3139001 }
		else	
			monster_ID = { 4134001, 4135001, 4136002, 4137001, 4138001, 4139001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100140 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3140001, 3141001, 3142002, 3143002, 3144001, 3145002 }
		else	
			monster_ID = { 4140001, 4141001, 4142002, 4143002, 4144001, 4145002 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
			
	elseif ID == 1100146 and halloween_on == 1 then		
		if other_drop == 0 then	
			monster_ID = { 3146002, 3147002, 3148001, 3149002, 3150001, 3150001 }
		else	
			monster_ID = { 4146002, 4147002, 4148001, 4149002, 4150001, 4150001 }
		end	
			
		density = { 0.50, 0.30, 0.20, 0.16, 0.12, 0.07 }	
		interval = { 1000, 1000, 1200, 1500, 2000, 3000 }	
	
	]]--
	
		
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
---------------------6랭크 던전 엘 카시아 몬스터 배치---------------------------------
---------------------이벤트 영역 id 범위 : 1701101 ~ 1701120--------------------------
---------------------이벤트 영역 id 범위 : 1701207 ~ 1701218--------------------------
----------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------


-----------------엘 카시아 1단계 구역------------------------		
	
	elseif ID == 1701101 then
		monster_ID = { 125005, 126005, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }							-----리젠 시간 (1 / 100초)

		
	elseif ID == 1701102 then
		monster_ID = { 125006, 126006, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.10, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }	-----리젠 시간 (1 / 100초)
		
		
	elseif ID == 1701103 then
		monster_ID = { 128003, 129005, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1500, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
		
		
	elseif ID == 1701104 then
		monster_ID = { 125007, 127004, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 15000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
		
		
	elseif ID == 1701105 then	----- 수정 트랩있는 곳
		monster_ID = { 130005, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.40, 0.20, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1500, 1500, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
		

	elseif ID == 1701106 then	
		monster_ID = { 128004, 129006, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1100, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
	
	
-----------------엘 카시아 2단계 구역------------------------		
	
	elseif ID == 1701107 then	
		monster_ID = { 132007, 133006, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)				
	
	
	elseif ID == 1701108 then	
		monster_ID = { 132007, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
	
	
	elseif ID == 1701109 then	
		monster_ID = { 130005, 131008, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1500, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)	
	
	
	elseif ID == 1701110 then
		monster_ID = { 132008, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1500, 1500, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
	
	
	elseif ID == 1701111 then
		monster_ID = { 132009, 133007, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)			
	
	
	elseif ID == 1701112 then
		monster_ID = { 135006, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)				


	elseif ID == 1701113 then
		monster_ID = { 135007, 139004, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)


	elseif ID == 1701114 then	---- 수정트랩 있는 곳
		monster_ID = { 138003, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.40, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
		
		
	elseif ID == 1701116 then
		monster_ID = { 136003, 139005, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
		
		
	elseif ID == 1701117 then	---- 수정트랩 있는 곳
		monster_ID = { 138004, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.40, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1500, 1500, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)				
		
		
	elseif ID == 1701118 then
		monster_ID = { 134007, 135008, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
		

-----------------엘 카시아 3단계 구역------------------------		
		
	elseif ID == 1701119 then
		monster_ID = { 141004, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.10, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1500, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
		
		
	elseif ID == 1701121 then
		monster_ID = { 141005, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.10, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1500, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)				
		
	elseif ID == 1701122 then
		monster_ID = { 141004, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.10, 0.15, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1000, 1000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
		
		
	elseif ID == 1701123 then
		monster_ID = { 141005, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.10, 0.15, 0.10, 0.10, 0.45, 0.40 }					-----표준 밀도 
		interval = { 1000, 1500, 1000, 1500, 2000, 2000 }							-----리젠 시간 (1 / 100초)						
		
		
	elseif ID == 1701124 then
		monster_ID = { 141004, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.10, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1000, 1500, 1000, 1000, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
		
		
	elseif ID == 1701120 then		---- 보스 리젠 구역
	
		Raremob_ID = {145010, 0, 0, 0}			---- 오시리스
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	---- 리젠 시간 360000 == 1시간	
	
		Raid_Raremob_ID = {145010, 0, 0, 0}		---- 오시리스
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }	---- 리젠 시간 360000 == 1시간

		
--------------------------------------------------------------------------------------------------
------------------------------------엘 카시아 2층 몬스터 배치-------------------------------------
--------------------------------------------------------------------------------------------------

	elseif ID == 1701207 then
		monster_ID = { 141004, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.20, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1500, 1500, 2000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
		
		
	elseif ID == 1701208 then
		monster_ID = { 141005, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.20, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1500, 1500, 2000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)	


	elseif ID == 1701209 then
		monster_ID = { 141004, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.20, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1500, 1500, 2000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
		
		
	elseif ID == 1701210 then
		monster_ID = { 141005, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.20, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1500, 1500, 2000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
		
		
	elseif ID == 1701211 then
		monster_ID = { 141004, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.20, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1500, 1500, 2000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)	
		
		
	elseif ID == 1701212 then
		monster_ID = { 141005, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.20, 0.10, 0.10, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 1500, 1500, 2000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		


--------------------------------------------------------------------------------------------------
-------------------------------엘 카시아 2층 레어 몬스터 배치-------------------------------------
--------------------------------------------------------------------------------------------------
-- 로밍 몬스터 배치로 삭제

	--elseif ID == 1701213 then					---- 
		--raremob_ID = { 144008,0,0,0 }			---- 몬스터 id	미스틱 지니 3단계
		--raremob_count = { 1, 1, 1, 1 }							---- 마리 수 (이벤트 영역이 나뉘어진 갯수만큼 곱해서 리젠된다. (예)이벤트 영역이 2구역으로 나뉘어 졌으면 (리젠수 * 2)의 수로 리젠된다. ) 
		--raremob_interval = { 180000, 1000, 1000, 1000 }			---- 리젠 시간 (1 / 100초)	180000 == 30분		

		
	--elseif ID == 1701214 then
		--raremob_ID = { 144011,0,0,0 }			---- 몬스터 id	에델 아우게 매지션
		--raremob_count = { 1, 1, 1, 1 }							---- 마리 수 (이벤트 영역이 나뉘어진 갯수만큼 곱해서 리젠된다. (예)이벤트 영역이 2구역으로 나뉘어 졌으면 (리젠수 * 2)의 수로 리젠된다. ) 
		--raremob_interval = { 180000, 1000, 1000, 1000 }			---- 리젠 시간 (1 / 100초)	180000 == 30분
		
		
	--elseif ID == 1701215 then
		--raremob_ID = { 144010,0,0,0 }			---- 몬스터 id	타파리 3단계
		--raremob_count = { 1, 1, 1, 1 }							---- 마리 수 (이벤트 영역이 나뉘어진 갯수만큼 곱해서 리젠된다. (예)이벤트 영역이 2구역으로 나뉘어 졌으면 (리젠수 * 2)의 수로 리젠된다. ) 
		--raremob_interval = { 180000, 1000, 1000, 1000 }			---- 리젠 시간 (1 / 100초)	180000 == 30분		
		
		
	--elseif ID == 1701216 then
		--raremob_ID = { 144007,0,0,0 }			---- 몬스터 id	지니 3단계
		--raremob_count = { 1, 1, 1, 1 }							---- 마리 수 (이벤트 영역이 나뉘어진 갯수만큼 곱해서 리젠된다. (예)이벤트 영역이 2구역으로 나뉘어 졌으면 (리젠수 * 2)의 수로 리젠된다. ) 
		--raremob_interval = { 180000, 1000, 1000, 1000 }			---- 리젠 시간 (1 / 100초)	180000 == 30분		
		
		
	--elseif ID == 1701217 then
		--raremob_ID = { 144012,0,0,0 }			---- 몬스터 id	에델 아우게 레인져
		--raremob_count = { 1, 1, 1, 1 }							---- 마리 수 (이벤트 영역이 나뉘어진 갯수만큼 곱해서 리젠된다. (예)이벤트 영역이 2구역으로 나뉘어 졌으면 (리젠수 * 2)의 수로 리젠된다. ) 
		--raremob_interval = { 180000, 1000, 1000, 1000 }			---- 리젠 시간 (1 / 100초)	180000 == 30분		


	--elseif ID == 1701218 then
		--raremob_ID = { 144009,0,0,0 }			---- 몬스터 id	아이무스 3단계
		--raremob_count = { 1, 1, 1, 1 }							---- 마리 수 (이벤트 영역이 나뉘어진 갯수만큼 곱해서 리젠된다. (예)이벤트 영역이 2구역으로 나뉘어 졌으면 (리젠수 * 2)의 수로 리젠된다. ) 
		--raremob_interval = { 180000, 1000, 1000, 1000 }			---- 리젠 시간 (1 / 100초)	180000 == 30분		
		
		
--------------------------------------------------------------------------------------------------------------		
--------------------------------------데스매치 몬스터 리젠 설정-----------------------------------------------
--------------------------------------------------------------------------------------------------------------

	elseif ID == 180101 or ID == 180102 or ID == 180103 or ID == 180104 or ID == 180105 then	-- 1랭크 지역
		monster_ID = { 15016, 15017, 15018, 0, 0, 0 }		-----몬스터 id
		density = { 0.50, 0.50, 0.50, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 6000, 6000, 6000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)


	elseif ID == 180201 or  ID == 180202 or  ID == 180203 or  ID == 180204 or  ID == 180205 then	-- 2랭크 지역
		monster_ID = { 15016, 15017, 15018, 0, 0, 0 }		-----몬스터 id
		density = { 0.50, 0.50, 0.50, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 6000, 6000, 6000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)
	
	
	elseif ID == 180301 or  ID == 180302 or  ID == 180303 or  ID == 180304 or  ID == 180305 or  ID == 180306 then	-- 3랭크 지역
		monster_ID = { 15016, 15017, 15018, 0, 0, 0 }		-----몬스터 id
		density = { 0.50, 0.50, 0.50, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 6000, 6000, 6000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)	
	
	
	elseif ID == 180401 or  ID == 180402 or  ID == 180403 or  ID == 180404 or  ID == 180405 then	-- 4랭크 지역
		monster_ID = { 15016, 15017, 15018, 0, 0, 0 }		-----몬스터 id
		density = { 0.50, 0.50, 0.50, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 6000, 6000, 6000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
	
		
	elseif ID == 180501 or  ID == 180502 or  ID == 180503 or  ID == 180504 or  ID == 180505 then	-- 5랭크 지역
		monster_ID = { 15016, 15017, 15018, 0, 0, 0 }		-----몬스터 id
		density = { 0.50, 0.50, 0.50, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 6000, 6000, 6000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
		
		
	elseif ID == 180601 or  ID == 180602 or  ID == 180603 or  ID == 180604 or  ID == 180605 then	-- 6랭크 지역
		monster_ID = { 15016, 15017, 15018, 0, 0, 0 }		-----몬스터 id
		density = { 0.50, 0.50, 0.50, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 6000, 6000, 6000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
		
		
	elseif ID == 180701 or  ID == 180702 or ID ==  180703 or  ID == 180704 or  ID == 180705 or  ID == 180706 or  ID == 180707 or  ID == 180708 or  ID == 180709 or  ID == 180710 or  ID == 180711 or  ID == 180712 or  ID == 180713 then	-- 7랭크 지역
		monster_ID = { 15016, 15017, 15018, 0, 0, 0 }		-----몬스터 id
		density = { 0.50, 0.50, 0.50, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 6000, 6000, 6000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
		
		
	elseif ID == 180001 or  ID == 180002 or  ID == 180003 then	-- 자유 지역
		monster_ID = { 15016, 15017, 15018, 0, 0, 0 }		-----몬스터 id
		density = { 0.50, 0.50, 0.50, 0.15, 0.15, 0.15 }					-----표준 밀도 
		interval = { 6000, 6000, 6000, 1500, 1500, 1500 }							-----리젠 시간 (1 / 100초)		
		
--------------------------------------------------------------------------------------------------------------		
--------------------------------------숨겨진 엘카시아 보스 몬스터 리젠 설정-----------------------------------------------
--------------------------------------------------------------------------------------------------------------	

	
	elseif ID == 1900001 then		---- 보스 리젠 구역
	
		Raremob_ID = {10145010, 0, 0, 0}			---- 오시리스
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	---- 리젠 시간 360000 == 1시간	
	
		Raid_Raremob_ID = {10145010, 0, 0, 0}		---- 오시리스
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }	---- 리젠 시간 360000 == 1시간

--------------------------------------------------------------------------------------------------------------		
--------------------------------------숨겨진 팔미르 보스 몬스터 리젠 설정-----------------------------------------------
--------------------------------------------------------------------------------------------------------------	

	elseif ID == 1912021  then
		Raremob_ID = {10095024, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10095024, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }


	elseif ID == 1912022	then
		Raremob_ID = {10095023, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10095023, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }

	elseif ID == 1912023	then
		Raremob_ID = {10100020, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10100020, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }


	elseif ID == 1912024	then
		Raremob_ID = {10105025, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10105025, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }

	elseif ID == 1912025	then
		Raremob_ID = {10108021, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10108021, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }


	elseif ID == 1912026	then
		Raremob_ID = {10108022, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10108022, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }


	elseif ID == 1912027	then
		Raremob_ID = {10105024, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10105024, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }


	elseif ID == 1912028	then
		Raremob_ID = {10100019, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10100019, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }

	
	elseif ID == 1912029	then
		Raremob_ID = {10110015, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10110015, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }

--------------------------------------------------------------------------------------------------------------		
--------------------------------------숨겨진 수정계곡 보스 몬스터 리젠 설정-----------------------------------------------
--------------------------------------------------------------------------------------------------------------	

	elseif ID == 1907011  then
		Raremob_ID = {10075010, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10075010, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1907012  then
		Raremob_ID = {10080013, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10080013, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1907013  then
		Raremob_ID = {10085011, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10085011, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1907014  then
		Raremob_ID = {10089018, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 120000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10089018, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 120000, 0, 0, 0 }
		
	elseif ID == 1907015  then
		Raremob_ID = {10080014, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10080014, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }
		
	elseif ID == 1907016  then
		Raremob_ID = {10090019, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10090019, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }

--------------------------------------------------------------------------------------------------------------		
--------------------------------------숨겨진 백룡의 쉼터 보스 몬스터 리젠 설정-----------------------------------------------
--------------------------------------------------------------------------------------------------------------	

	
	elseif ID == 1910001 then
		monster_ID = {10150008,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10150008,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1910002 then
		monster_ID = {10150008,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10150008,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1910003 then
		monster_ID = {10157007,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10157007,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
			
	elseif ID == 1910004 then
		monster_ID = {10157007,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10157007,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1910005 then
		monster_ID = {10157014,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10157014,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1910006 then
		monster_ID = {10157015,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10157015,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1910007 then
		monster_ID = {10158001,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10158001,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1910008  then
		Raremob_ID = {10158002, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10158002, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }

--------------------------------------------------------------------------------------------------------------		
--------------------------------------숨겨진 흑룡의 그늘 보스 몬스터 리젠 설정-----------------------------------------------
--------------------------------------------------------------------------------------------------------------	


	elseif ID == 1909001 then
		monster_ID = {10161004,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10161004,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1909002 then
		monster_ID = {10161004,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10161004,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1909003 then
		monster_ID = {10168003,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10168003,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
			
	elseif ID == 1909004 then
		monster_ID = {10169003,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10169003,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	

	elseif ID == 1909005 then
		monster_ID = {9169005,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {9169005,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1909006 then
		monster_ID = {10169005,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10169005,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1909007 then
		monster_ID = {10169005,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10169005,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1909008  then
		Raremob_ID = {10170001,0,0,0,0,0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10170001,0,0,0,0,0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }


--------------------------------------------------------------------------------------------------------------		
--------------------------------------숨겨진 사룡의 그늘 보스 몬스터 리젠 설정-----------------------------------------------
--------------------------------------------------------------------------------------------------------------	

	elseif ID == 1908001 then
		monster_ID = {10185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		

	elseif ID == 1908002 then
		monster_ID = {10185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1908003 then
		monster_ID = {10185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }			
			
	elseif ID == 1908004 then
		monster_ID = {10185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1602041 then
		monster_ID = {10185002,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	

	elseif ID == 1908005 then
		monster_ID = {10185002,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10185002,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
		
	elseif ID == 1908006 then
		monster_ID = {10185003,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10185003,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }	
	
	elseif ID == 1908007 then
		monster_ID = {10185003,0,0,0,0,0}
		density = { 0.05, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 6000, 1000, 1200, 1500, 2000, 3000 }
		
		monster_ID = {10185003,0,0,0,0,0}
		Raidmob_density = { 0.37, 0.37, 0.37, 0.37, 0.37, 0.37 }
		Raidmob_interval = { 10000, 10000, 12000, 15000, 20000, 30000 }		
		
	elseif ID == 1908008  then
		Raremob_ID = {10190001,0,0,0,0,0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10190001,0,0,0,0,0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }



----------------------숨겨진 수정계곡 키 몬스터 리젠 설정-----------------------------------------------

	elseif ID == 1907017  then
		Raremob_ID = {10070016, 0, 0, 0}
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
		Raid_Raremob_ID = {10070016, 0, 0, 0}
		Raid_Raremob_count = { 1, 1, 1, 1 }
		Raid_Raremob_interval = { 360000, 0, 0, 0 }
		
		
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
----------------------잃어버린 비밀의 섬 확장지역 몬스터 배치 5-7--------------------------		
-------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
		
	elseif ID == 2000000 then							--------------광포한 발톱 쿤틸(지역보스) 리젠 지역
		monster_ID = {150011, 150011, 150011, 152002, 0, 0 }
		density = { 1, 1, 1, 1, 1, 1 }
		interval = { 3000, 1000, 1200, 38000, 2000, 3000 }				
		
	elseif ID == 2000006 then
		monster_ID = {146009,146008,147009,146009,0,0}
		density = { 1, 0.4, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 3000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000023 then
		monster_ID = {146009,146008,147009,146009,0,0}
		density = { 1, 0.4, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 3000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000005 then
		monster_ID = {146009,146008,147009,146009,0,0}
		density = { 1, 0.4, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 3000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000022 then
		monster_ID = {146009,146008,147009,146009,0,0}
		density = { 1, 0.4, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 3000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000004 then
		monster_ID = {147008,148007,148008,146009,0,0}
		density = { 0.05, 0.4, 1, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000021 then
		monster_ID = {147008,148007,148008,147008,0,0}
		density = { 0.05, 0.4, 1, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000003 then
		monster_ID = {147008,148007,148008,147008,0,0}
		density = { 0.05, 0.4, 1, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000020 then
		monster_ID = {147008,148007,148008,147008,0,0}
		density = { 0.05, 0.4, 1, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000002 then
		monster_ID = {147008,148007,148008,147008,0,0}
		density = { 0.05, 0.4, 1, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000001 then
		monster_ID = {147008,148007,148008,147008,0,0}
		density = { 0.05, 0.4, 1, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000000 then
		monster_ID = {147008,148007,148008,147008,0,0}
		density = { 0.05, 0.4, 1, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000019 then
		monster_ID = {149005,149006,150008,149005,0,0}
		density = { 0.05, 0.3, 0.4, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000024 then
		monster_ID = {149005,149006,150008,149005,0,0}
		density = { 0.05, 0.3, 0.4, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000027 then
		monster_ID = {149005,149006,150008,149005,0,0}
		density = { 0.05, 0.3, 0.4, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000018 then
		monster_ID = {149005,149006,150008,149005,0,0}
		density = { 0.05, 0.3, 0.4, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000017 then
		monster_ID = {149005,149006,150008,149005,0,0}
		density = { 0.05, 0.3, 0.4, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000031 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000050 then							
		monster_ID = {150009,150010,150011,150011,150011,150011}
		density = { 1, 1, 1, 1, 1, 1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
				
	elseif ID == 2000016 then
		monster_ID = {150009,150010,150011,150009,0,0}
		density = { 0.05, 1, 0.3, 0.5, 10, 1.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000049 then
		monster_ID = {150009,150010,150011,150009,0,0}
		density = { 0.05, 1, 0.3, 0.5, 10, 1.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000015 then
		monster_ID = {150009,150010,150011,150009,0,0}
		density = { 0.05, 1, 0.3, 0.5, 10, 1.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000048 then
		monster_ID = {150009,150010,150011,150009,0,0}
		density = { 0.05, 1, 0.3, 0.5, 10, 1.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000014 then
		monster_ID = {150009,150010,150011,150009,0,0}
		density = { 0.05, 1, 0.3, 0.5, 10, 1.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000047 then
		monster_ID = {150009,150010,150011,150009,0,0}
		density = { 0.05, 1, 0.3, 0.5, 10, 1.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000013 then
		monster_ID = {150009,150010,150011,150009,0,0}
		density = { 0.05, 1, 0.3, 0.5, 10, 1.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000046 then
		monster_ID = {150009,150010,150011,150009,0,0}
		density = { 0.05, 1, 0.3, 0.5, 10, 1.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000012 then
		monster_ID = {149005,149006,150008,149005,0,0}
		density = { 0.05, 0.3, 0.4, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000011 then
		monster_ID = {149005,149006,150008,149005,0,0}
		density = { 0.05, 0.3, 0.4, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000010 then
		monster_ID = {149005,149006,150008,149005,0,0}
		density = { 0.05, 0.3, 0.4, 1.5, 2.5, 10 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000009 then
		monster_ID = {146009,146008,147009,146009,0,0}
		density = { 1, 0.4, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 3000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000008 then
		monster_ID = {146009,146008,147009,146009,0,0}
		density = { 1, 0.4, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 3000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000007 then
		monster_ID = {146009,146008,147009,146009,0,0}
		density = { 1, 0.4, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 3000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000025 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000026 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000028 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000029 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000030 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000032 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000033 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000034 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000035 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000036 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000037 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000038 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000039 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000040 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000041 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000042 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000043 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000044 then
		monster_ID = {154002,154002,154002,0,0,0}
		density = { 1.5, 1.3, 0.1, 1.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000045 then								
		monster_ID = {154002,154002,0,154002,0,154002}
		density = { 2, 2, 2, 1.5, 1.5, 1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
		Raremob_ID = {155002, 0, 0, 0}						--------------바리칸(지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	
		
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
----------------------잃어버린 비밀의 섬 확장지역 몬스터 배치 5-8--------------------------		
-------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------				
		
		
	elseif ID == 2000051 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

		Raremob_ID = {175004, 0, 0, 0}						--------------골디쉬 웜피카(지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }							

	elseif ID == 2000052 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }						

	elseif ID == 2000053 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }						

	elseif ID == 2000054 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000055 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000056 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000057 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000058 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000059 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000060 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000061 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000062 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000063 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000064 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000065 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000066 then
		monster_ID = {165002,165003,165004,158002,0,0}
		density = { 1, 0.5, 0.3, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000067 then
		monster_ID = {165002,165003,165004,158002,0,0}
		density = { 1, 0.5, 0.3, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000068 then
		monster_ID = {165002,165003,165004,158002,0,0}
		density = { 1, 0.5, 0.3, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000069 then
		monster_ID = {165002,165003,165004,158002,0,0}
		density = { 1, 0.5, 0.3, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000070 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000071 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000072 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000073 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000074 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000075 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000076 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000077 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000078 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000079 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000080 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000081 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000082 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000083 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000084 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1, 1, 1, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000085 then
		monster_ID = {165002,165003,165004,165002,165002,165002}
		density = { 1, 1, 1, 1, 1, 1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000086 then
		monster_ID = {158002,160003,160004,158002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000087 then
		monster_ID = {165002,165003,165004,158002,0,0}
		density = { 1, 0.5, 0.3, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000088 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000089 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000090 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000091 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }						
		
	elseif ID == 2000092 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000093 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }						

	elseif ID == 2000094 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }						
		
	elseif ID == 2000095 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }						
		
	elseif ID == 2000096 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }						
		
	elseif ID == 2000097 then
		monster_ID = {171003,170002,172003,171003,0,0}
		density = { 0.5, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }						
		
	elseif ID == 2000098 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000099 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000100 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000101 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000102 then
		monster_ID = {160006,163005,165008,167002,0,0}
		density = { 1, 0.3, 0.3, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000103 then
		monster_ID = {165002,165003,165004,158002,0,0}
		density = { 1, 0.5, 0.3, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000104 then
		monster_ID = {165002,165003,165004,158002,0,0}
		density = { 1, 0.5, 0.3, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000105 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000106 then
		monster_ID = {165002,165003,165004,158002,0,0}
		density = { 1, 0.5, 0.3, 1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000107 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000108 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000109 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000110 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000111 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000112 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000113 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000114 then
		monster_ID = {158002,160003,160004,165002,0,0}
		density = { 1, 0.8, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
		
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
----------------------잃어버린 비밀의 섬 확장지역 몬스터 배치 6-7--------------------------		
-------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------				

	elseif ID == 2000115 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000137 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000116 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000136 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000117 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000135 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000118 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000134 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000119 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000133 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000120 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000132 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000121 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000131 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000122 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000129 then
		monster_ID = {150012,151002,152003,150012,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000123 then
		monster_ID = {153002,154002,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000138 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000139 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000280 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000140 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000141 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000142 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000143 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000130 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000128 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000127 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000126 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000125 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000124 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000144 then
		monster_ID = {162004,162002,159002,162004,0,0}
		density = { 0.5, 0.8, 1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = {165006, 0, 0, 0}						--------------테루진(지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }						

	elseif ID == 2000157 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000156 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000281 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000155 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000154 then
		monster_ID = {158002,158003,158002,158003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000153 then
		monster_ID = {0,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000152 then
		monster_ID = {0,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000151 then
		monster_ID = {0,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000150 then
		monster_ID = {0,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000149 then
		monster_ID = {0,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000148 then
		monster_ID = {159002,0,0,0,0,0}
		density = { 1, 0.8, 0.5, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000147 then
		monster_ID = {159002,162002,159002,162002,0,0}
		density = { 1, 0.8, 0.5, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000146 then
		monster_ID = {159002,162002,159002,162002,0,0}
		density = { 1.5, 0.5, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000169 then
		monster_ID = {159002,162002,162004,159002,0,0}
		density = { 1.5, 0.5, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000170 then
		monster_ID = {159002,162002,162004,159002,0,0}
		density = { 1.5, 0.5, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000171 then
		monster_ID = {159002,162002,162004,159002,0,0}
		density = { 1.5, 0.5, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000172 then
		monster_ID = {159002,162002,162003,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000173 then
		monster_ID = {159002,162002,162003,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000145 then
		monster_ID = {159002,162002,159002,162002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000174 then
		monster_ID = {163002,163003,163004,159002,0,0}
		density = { 0.5, 0.3, 0.3, 1, 0.8, 0.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000175 then
		monster_ID = {163002,163003,163004,159002,0,0}
		density = { 0.5, 0.3, 0.3, 1, 0.8, 0.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = {165007, 0, 0, 0}						--------------드라카(지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }				

	elseif ID == 2000158 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000159 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000160 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000161 then
		monster_ID = {158002,160002,158002,160002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000162 then
		monster_ID = {0,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000163 then
		monster_ID = {0,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000164 then
		monster_ID = {159002,0,0,0,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000165 then
		monster_ID = {159002,162002,159002,162002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000166 then
		monster_ID = {162002,0,0,0,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000167 then
		monster_ID = {159002,0,0,0,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000168 then
		monster_ID = {159002,162002,159002,162002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000183 then
		monster_ID = {159002,160005,162004,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000182 then
		monster_ID = {159002,160005,162004,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000181 then
		monster_ID = {159002,160005,162004,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000180 then
		monster_ID = {159002,160005,162004,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000176 then
		monster_ID = {163002,163003,163004,159002,0,0}
		density = { 0.5, 0.3, 0.3, 1, 0.8, 0.5 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000177 then
		monster_ID = {159002,162002,162003,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000178 then
		monster_ID = {159002,162002,162003,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000179 then
		monster_ID = {159002,162002,162003,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000215 then
		monster_ID = {159002,162002,162003,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }

	elseif ID == 2000214 then
		monster_ID = {159002,162002,162003,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000213 then
		monster_ID = {159002,162002,162003,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
		Raremob_ID = {165005, 0, 0, 0}						--------------크루진(지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }		
		
	elseif ID == 2000212 then
		monster_ID = {159002,160005,162004,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000211 then
		monster_ID = {159002,160005,162004,159002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000210 then
		monster_ID = {160005,162003,163004,160005,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000209 then
		monster_ID = {160005,162003,163004,160005,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000208 then
		monster_ID = {160005,162003,163004,160005,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000207 then
		monster_ID = {160005,0,0,0,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000184 then
		monster_ID = {160005,162003,163004,160005,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000185 then
		monster_ID = {160005,162003,160005,162003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000186 then
		monster_ID = {160005,162003,160005,162003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000187 then
		monster_ID = {160005,163004,160005,163004,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000188 then
		monster_ID = {160005,0,0,0,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }
		
	elseif ID == 2000189 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000190 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000191 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000192 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000193 then
		monster_ID = {177001,176001,177001,176001,0,0}
		density = { 1, 0.5, 0.8, 0.3, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000194 then
		monster_ID = {165002,165003,165004,165002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000195 then
		monster_ID = {0,0,0,0,0,0}
		density = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000196 then
		monster_ID = {177001,176001,178002,179001,178002,179001}
		density = { 1, 0.5, 0.8, 0.3, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
		Raremob_ID = {180003, 0, 0, 0}					--------------------- 탐험대장 켈리컷(지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }
		
	elseif ID == 2000197 then
		monster_ID = {177001,176001,178002,179001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000198 then
		monster_ID = {177001,176001,178002,179001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000199 then
		monster_ID = {177001,176001,178002,179001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000200 then
		monster_ID = {175001,178001,175001,178001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000201 then
		monster_ID = {175001,178001,175001,178001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000202 then
		monster_ID = {175001,178001,175001,178001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000203 then
		monster_ID = {175001,178001,175001,178001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
		Raremob_ID = {180001, 0, 0, 0}					--------------------- 설원의 멧돼지오크 대장 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }		
		
	elseif ID == 2000204 then
		monster_ID = {175001,178001,175001,178001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000205 then
		monster_ID = {175001,178001,175001,178001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000206 then
		monster_ID = {175001,178001,175001,178001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
		
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------		
----------------------잃어버린 비밀의 섬 확장지역 몬스터 배치 6-8--------------------------		
-------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------		
-------------------------------------------------------------------------------------------						
		
	elseif ID == 2000216 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000217 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000218 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000219 then
		monster_ID = {172004,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000220 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000221 then
		monster_ID = {173001,174001,172004,174003,0,0}
		density = { 1, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000222 then
		monster_ID = {173001,174001,172004,174003,0,0}
		density = { 1, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }			
		
	elseif ID == 2000223 then
		monster_ID = {172004,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000224 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000225 then
		monster_ID = {171002,172002,172004,171002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000226 then
		monster_ID = {171002,172002,172004,171002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000227 then
		monster_ID = {171001,172001,170004,171001,0,0}
		density = { 1, 0.5, 1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000228 then
		monster_ID = {171002,172002,170004,171002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000229 then
		monster_ID = {171002,172002,170004,171002,0,0}
		density = { 0.5, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000230 then
		monster_ID = {171001,172001,170004,171001,0,0}
		density = { 1, 0.5, 1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000231 then
		monster_ID = {171001,172001,170004,171001,0,0}
		density = { 1, 0.5, 1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000232 then
		monster_ID = {171001,172001,170004,171001,0,0}
		density = { 1, 0.5, 1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000233 then
		monster_ID = {171002,172002,172004,171002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000234 then
		monster_ID = {171001,172001,170004,171002,0,0}
		density = { 1, 0.5, 1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000235 then
		monster_ID = {171001,172001,171001,172001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000236 then
		monster_ID = {173001,174001,172004,174003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }			
		
	elseif ID == 2000237 then
		monster_ID = {172004,172001,173001,174001,0,0}
		density = { 0.5, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000238 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000239 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000240 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000241 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000242 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000243 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000244 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000245 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000246 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
	elseif ID == 2000247 then
		monster_ID = {174003,172001,173001,174001,0,0}
		density = { 0.5, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000248 then
		monster_ID = {174003,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
		Raremob_ID = {175003, 0, 0, 0}					--------------------- 붉은 숨 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }						

	elseif ID == 2000249 then
		monster_ID = {173001,174001,172004,174003,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }			

	elseif ID == 2000250 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000251 then
		monster_ID = {174003,172001,173001,174001,0,0}
		density = { 0.5, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000252 then
		monster_ID = {174003,172001,173001,174001,0,0}
		density = { 0.5, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000253 then
		monster_ID = {171002,172002,174003,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000254 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000255 then
		monster_ID = {174003,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000256 then
		monster_ID = {174003,172001,173001,174001,0,0}
		density = { 0.5, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				
		
		Raremob_ID = {175002, 0, 0, 0}					--------------------- 푸른불꽃 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }		
		
	elseif ID == 2000257 then
		monster_ID = {174003,172001,173001,174001,0,0}
		density = { 0.5, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000258 then
		monster_ID = {174003,172001,173001,174001,0,0}
		density = { 0.5, 0.5, 1, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000259 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000260 then
		monster_ID = {172004,172001,173001,174001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000261 then
		monster_ID = {172004,172001,173001,174001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000262 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000263 then
		monster_ID = {171001,172001,173001,174001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000264 then
		monster_ID = {171002,172002,172004,171002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000265 then
		monster_ID = {171002,172002,173002,174002,0,0}
		density = { 0.5, 0.5, 0.5, 0.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000266 then
		monster_ID = {171002,172002,170004,171002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000267 then
		monster_ID = {171001,172001,170004,171001,0,0}
		density = { 1, 0.5, 1, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000268 then
		monster_ID = {171002,172002,170004,171002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000269 then
		monster_ID = {171002,172002,170004,171002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000270 then
		monster_ID = {171001,172001,170004,171001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }				

	elseif ID == 2000271 then
		monster_ID = {181001,181001,183001,183001,0,0}
		density = { 0.5, 0.5, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		
		
	elseif ID == 2000272 then
		monster_ID = {181001,183001,181001,181001,183001,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000273 then
		monster_ID = {180002,182001,184001,180002,0,0}
		density = { 1, 1, 0.3, 0.1, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000274 then
		monster_ID = {181001,183001,181001,183001,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000275 then
		monster_ID = {180002,182001,184001,180002,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000276 then
		monster_ID = {181001,183001,184002,181001,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

		Raremob_ID = {185001, 0, 0, 0}					--------------------- 그류페인 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	

	elseif ID == 2000277 then
		monster_ID = {180002,182001,184001,180002,0,0}
		density = { 0.2, 0.2, 0.2, 0.2, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000278 then
		monster_ID = {181001,183001,181001,183001,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

	elseif ID == 2000279 then
		monster_ID = {180002,182001,184001,180002,0,0}
		density = { 1.5, 1.5, 1.5, 1.5, 0.1, 0.1 }
		interval = { 3000, 1000, 1200, 1500, 2000, 3000 }		

		
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
----------------------------에스포에 몬스터 배치--------------------------------------
---------------------이벤트 영역 id 범위 : 2010001 ~ 2010036--------------------------
---------------------이벤트 영역 id 범위 : 2020001 ~ 2020029--------------------------
---------------------이벤트 영역 id 범위 : 2030001 ~ 2030020--------------------------
---------------------이벤트 영역 id 범위 : 2040001 ~ 2040004--------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------


-----------------에스포에 제 1지역 3단계 구역_사탄소녀 & 폐기물 분쇄기------------------------		
	
	elseif ID == 2010001 then
		monster_ID = { 151013, 153003, 151014, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010002 then
		monster_ID = { 151013, 153003, 0, 153003, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010003 then
		monster_ID = { 151013, 0, 153003, 153003, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010004 then
		monster_ID = { 151013, 153003, 151013, 153003, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010005 then
		monster_ID = { 151013, 153003, 151013, 153003, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010006 then
		monster_ID = { 0, 153003, 151013, 153003, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010007 then
		monster_ID = { 0, 153003, 153003, 153003, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010008 then
		monster_ID = { 151013, 153003, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }

		Raremob_ID = {154003, 0, 0, 0}					--------------------- 폐기물 분쇄기 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	

-----------------에스포에 제 1지역 2단계 구역------------------------		
	
	elseif ID == 2010009 then
		monster_ID = { 0, 144015, 143018, 0, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010010 then
		monster_ID = { 144015, 0, 143018, 143018, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.03, 0.02, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010011 then
		monster_ID = { 144015, 0, 143018, 0, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010012 then
		monster_ID = { 0, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010013 then
		monster_ID = { 147010, 147010, 0, 143018, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010014 then
		monster_ID = { 147010, 0, 146010, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010015 then
		monster_ID = { 0, 147010, 0, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010016 then
		monster_ID = { 147010, 0, 146010, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010017 then
		monster_ID = { 147010, 0, 146010, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010018 then
		monster_ID = { 150013, 150013, 0, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010019 then
		monster_ID = { 150013, 0, 146010, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010020 then
		monster_ID = { 150013, 0, 0, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010021 then
		monster_ID = { 150013, 0, 146010, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010022 then
		monster_ID = { 150013, 0, 146010, 146010, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

		
-----------------에스포에 제 1지역 1단계 구역------------------------

	elseif ID == 2010023 then
		monster_ID = { 143015, 145012, 146010, 0, 0, 0 }		-----몬스터 id
		density = { 0.01, 0.01, 0.01, 0.015, 0.015, 0.015 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)


	elseif ID == 2010024 then
		monster_ID = { 0, 143015, 153015, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010025 then
		monster_ID = { 143015, 0, 145012, 145012, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010026 then
		monster_ID = { 143015, 145012, 153013, 145012, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010027 then
		monster_ID = { 0, 0, 153015, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010028 then
		monster_ID = { 0, 0, 145012, 145012, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

-----------------에스포에 제 1지역 3단계 구역------------------------

	elseif ID == 2010029 then
		monster_ID = { 154004, 0, 0, 156004, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010030 then
		monster_ID = { 154004, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010031 then
		monster_ID = { 0, 0, 0, 156004, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010032 then
		monster_ID = { 0, 0, 157004, 156004, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010033 then
		monster_ID = { 156004, 0, 157004, 157004, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010034 then
		monster_ID = { 0, 0, 0, 156004, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010035 then
		monster_ID = { 0, 0, 154004, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2010036 then
		monster_ID = { 154004, 0, 0, 156004, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

-----------------에스포에 제 2지역 1단계 구역------------------------	

	elseif ID == 2020001 then
		monster_ID = { 154004, 154009, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

		Raremob_ID = {157007, 0, 0, 0}					--------------------- 암흑의 파열음 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	

	elseif ID == 2020002 then
		monster_ID = { 0, 154009, 0, 156009, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020003 then
		monster_ID = { 154009, 0, 0, 156009, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020004 then
		monster_ID = { 0, 156009, 158008, 158009, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020005 then
		monster_ID = { 0, 156009, 0, 158008, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020006 then
		monster_ID = { 0, 0, 158008, 156009, 0, 0 }		-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

-----------------에스포에 제 2지역 2단계 구역------------------------	

	elseif ID == 2020007 then
		monster_ID = { 158009, 158009, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.00, 0.00, 0.00, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020008 then
		monster_ID = { 0, 0, 158011, 160007, 0, 0 }		-----몬스터 id
		density = { 0.00, 0.00, 0.00, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020009 then
		monster_ID = { 0, 162005, 0, 160007, 0, 0 }		-----몬스터 id
		density = { 0.00, 0.00, 0.00, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020010 then
		monster_ID = { 162005, 160007, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.00, 0.00, 0.00, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020011 then
		monster_ID = { 0, 0, 0, 162005, 0, 0 }		-----몬스터 id
		density = { 0.00, 0.00, 0.00, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020012 then
		monster_ID = { 0, 0, 160007, 162005, 0, 0 }		-----몬스터 id
		density = { 0.00, 0.00, 0.00, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020013 then
		monster_ID = { 0, 0, 158011, 160007, 0, 0 }		-----몬스터 id
		density = { 0.00, 0.00, 0.00, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020014 then
		monster_ID = { 0, 0, 0, 162005, 0, 0 }		-----몬스터 id
		density = { 0.00, 0.00, 0.00, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

		Raremob_ID = {163006, 0, 0, 0}					--------------------- 암흑의 파열음 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	


-----------------에스포에 제 2지역 3단계 구역------------------------	

	elseif ID == 2020015 then
		monster_ID = { 0, 0, 0, 158014, 0, 0 }		-----몬스터 id
		density = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020016 then
		monster_ID = { 158014, 0, 162011, 0, 0, 0 }		-----몬스터 id
		density = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020017 then
		monster_ID = { 158014, 0, 162011, 0, 0, 0 }		-----몬스터 id
		density = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020018 then
		monster_ID = { 162011, 0, 0, 158014, 0, 0 }		-----몬스터 id
		density = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020019 then
		monster_ID = { 162011, 0, 0, 0, 0, 0 }		-----몬스터 id
		density = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020020 then
		monster_ID = { 158014, 0, 162011, 0, 0, 0 }		-----몬스터 id
		density = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020021 then
		monster_ID = { 158014, 0, 0, 162011, 0, 0 }		-----몬스터 id
		density = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020022 then
		monster_ID = { 0, 0, 0, 162011, 0, 0 }		-----몬스터 id
		density = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

		Raremob_ID = {162016, 0, 0, 0}					--------------------- 암흑의 파열음 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	

-----------------에스포에 제 2지역 4단계 구역------------------------	

	elseif ID == 2020023 then
		monster_ID = { 0, 158019, 0, 159006, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020024 then
		monster_ID = { 158019, 159006, 159007, 165011, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020025 then
		monster_ID = { 158019, 159007, 159007, 165011, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020026 then
		monster_ID = { 0, 0, 158019, 159007, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020027 then
		monster_ID = { 158019, 159006, 159007, 165012, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020028 then
		monster_ID = { 165013, 165012, 159007, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2020029 then
		monster_ID = { 0, 159007, 159006, 165013, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

-----------------에스포에 제 3지역 1단계 구역------------------------	

	elseif ID == 2030001 then
		monster_ID = { 165013, 0, 165014, 170005, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030002 then
		monster_ID = { 0, 170005, 0, 168002, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030003 then
		monster_ID = { 0, 0, 165014, 170005, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030004 then
		monster_ID = { 0, 0, 168002, 165014, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030005 then
		monster_ID = { 0, 165014, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

-----------------에스포에 제 3지역 2단계 구역------------------------

	elseif ID == 2030006 then
		monster_ID = { 0, 0, 0, 168002, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030007 then
		monster_ID = { 0, 168002, 0, 175006, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030008 then
		monster_ID = { 0, 175006, 0, 175009, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

		Raremob_ID = {171004, 0, 0, 0}					--------------------- 가짜 마녀 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }

	elseif ID == 2030009 then
		monster_ID = { 168002, 0, 175006, 175009, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030010 then
		monster_ID = { 0, 168002, 0, 175009, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

-----------------에스포에 제 3지역 3단계 구역------------------------

	elseif ID == 2030011 then
		monster_ID = { 0, 0, 0, 170005, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030012 then
		monster_ID = { 0, 0, 0, 170005, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030013 then
		monster_ID = { 0, 0, 0, 170005, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030014 then
		monster_ID = { 0, 170005, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030015 then
		Raremob_ID = {175005, 0, 0, 0}					--------------------- 뉴타입 결전의 마녀 (지역보스) 리젠 지역
		Raremob_count = { 1, 1, 1, 1 }
		Raremob_interval = { 360000, 0, 0, 0 }	

-----------------에스포에 제 3지역 4단계 구역------------------------

	elseif ID == 2030016 then
		monster_ID = { 180004, 180005, 165013, 180005, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030017 then
		monster_ID = { 0, 165013, 0, 180005, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030018 then
		monster_ID = { 175009, 165012, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030019 then
		monster_ID = { 175009, 165012, 177004, 177004, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	elseif ID == 2030020 then
		monster_ID = { 175009, 0, 177004, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
		
		
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------
---------------------------- 마스터 몬스터 배치 --------------------------------------
---------------------이벤트 영역 id 범위 : 2050001 ~ 2050008--------------------------
---------------------이벤트 영역 id 범위 : 2060001 ~ 2060008--------------------------
---------------------이벤트 영역 id 범위 : 2070001 ~ 2070008--------------------------
---------------------이벤트 영역 id 범위 : 2080001 ~ 2080008--------------------------
---------------------이벤트 영역 id 범위 : 2090001 ~ 2090008--------------------------
--------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------


-----------------마스터 제 13지역 1단계 구역------------------------

	elseif ID == 2050001 then
		monster_ID = { 136019, 136019, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2050002 then
		monster_ID = { 136019, 136019, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2050003 then
		monster_ID = { 136019, 136019, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2050004 then
		monster_ID = { 136019, 136019, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	
-----------------마스터 제 13지역 2단계 구역------------------------

	elseif ID == 2050005 then
		monster_ID = { 136020, 136020, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2050006 then
		monster_ID = { 136020, 136020, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2050007 then
		monster_ID = { 136020, 136020, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2050008 then
		monster_ID = { 136020, 136020, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	

-----------------마스터 제 14지역 1단계 구역------------------------

	elseif ID == 2060001 then
		monster_ID = { 145025, 145025, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2060002 then
		monster_ID = { 145025, 145025, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2060003 then
		monster_ID = { 145025, 145025, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2060004 then
		monster_ID = { 145025, 145025, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	
-----------------마스터 제 14지역 2단계 구역------------------------

	elseif ID == 2060005 then
		monster_ID = { 145026, 145026, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2060006 then
		monster_ID = { 145026, 145026, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2060007 then
		monster_ID = { 145026, 145026, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2060008 then
		monster_ID = { 145026, 145026, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
		

-----------------마스터 제 15지역 1단계 구역------------------------

	elseif ID == 2070001 then
		monster_ID = { 158026, 158026, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2070002 then
		monster_ID = { 158026, 158026, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2070003 then
		monster_ID = { 158026, 158026, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2070004 then
		monster_ID = { 158026, 158026, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	
-----------------마스터 제 15지역 2단계 구역------------------------

	elseif ID == 2070005 then
		monster_ID = { 158027, 158027, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2070006 then
		monster_ID = { 158027, 158027, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2070007 then
		monster_ID = { 158027, 158027, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2070008 then
		monster_ID = { 158027, 158027, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

-----------------마스터 제 16지역 1단계 구역------------------------

	elseif ID == 2080001 then
		monster_ID = { 163010, 163010, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2080002 then
		monster_ID = { 163010, 163010, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2080003 then
		monster_ID = { 163010, 163010, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2080004 then
		monster_ID = { 163010, 163010, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	
-----------------마스터 제 16지역 2단계 구역------------------------

	elseif ID == 2080005 then
		monster_ID = { 163011, 163011, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2080006 then
		monster_ID = { 163011, 163011, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2080007 then
		monster_ID = { 163011, 163011, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2080008 then
		monster_ID = { 163011, 163011, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

-----------------마스터 제 17지역 1단계 구역------------------------

	elseif ID == 2090001 then
		monster_ID = { 177021, 177021, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2090002 then
		monster_ID = { 177021, 177021, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2090003 then
		monster_ID = { 177021, 0, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2090004 then
		monster_ID = { 177021, 0, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)

	
-----------------마스터 제 17지역 2단계 구역------------------------

	elseif ID == 2090005 then
		monster_ID = { 177022, 0, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2090006 then
		monster_ID = { 177022, 0, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2090007 then
		monster_ID = { 177022, 0, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
	
	elseif ID == 2090008 then
		monster_ID = { 177022, 0, 0, 0, 0, 0 }   	-----몬스터 id
		density = { 0.05, 0.05, 0.05, 0.15, 0.15, 0.15 }		-----표준 밀도 
		interval = { 1000, 1000, 1000, 2000, 1500, 1500 }		-----리젠 시간 (1 / 100초)
		
end

	
	-- 이벤트 박스의 사이즈를 구한다
	size = math.abs( left - right ) * math.abs( top - bottom )

	-- 필드 몬스터와 레이드용 몬스터 리스폰 (필드 몬스터 리스폰 함수는 레이드 던전에서는 무시되고, 레이드 몬스터 리스폰 함수는 일반 필드 및 일반 던전에서는 무시된다.)	

	-- 필드 몬스터 리스폰
	-- table_length 수 만큼 몬스터 리스폰을 할당한다. 0이면 스킵.
	local table_length = table.getn( monster_ID )
	
	for i = 1, table_length do
	
		-- 등록된 몬스터ID가 0이 아닐때만 리스폰을 수행함
		if monster_ID[i] ~= 0 then

			-- 몬스터 리스폰 개체수를 구한다. (넓이130000당 1마리가 표준. 최소 1마리)
			max_num = math.floor( ( size / 130000 ) * density[i] + .5 )
			if max_num < 1 then
				max_num = 1
			end
	
			-- 1회에 동시 리스폰될 최대값을 구한다.  (최대값의 100%. 최소 1마리)
			max_respawn_once = max_num * 0.2
			if max_respawn_once < 1 then
				max_respawn_once = 1
			end
	
			
			-- 리스폰을 셋팅한다.
			respawn( ID , interval[i] , left, top, right, bottom, monster_ID[i] , max_num , max_respawn_once )
	
		end
		
	end
	
	-- 레이드 몬스터 리스폰
	-- 1~6의 몬스터 리스폰을 할당한다. 0이면 스킵.
	table_length = table.getn( Raidmob_ID )
		
	for i = 1, table_length do

		-- 등록된 몬스터ID가 0이 아닐때만 리스폰을 수행함
		if Raidmob_ID[i] ~= 0 then

			-- 몬스터 리스폰 개체수를 구한다. (넓이130000당 1마리가 표준. 최소 1마리)
			max_num = math.floor( ( size / 130000 ) * Raidmob_density[i] + .5 )
			if max_num < 1 then
				max_num = 1
			end
	
			-- 1회에 동시 리스폰될 최대값을 구한다.  (최대값의 100%. 최소 1마리)
			max_respawn_once = max_num * 0.2
			if max_respawn_once < 1 then
				max_respawn_once = 1
			end
	
			
			-- 리스폰을 셋팅한다.
			raid_respawn( ID , Raidmob_interval[i] , left, top, right, bottom, Raidmob_ID[i] , max_num , max_respawn_once )
	
		end
		
	end
	
	-- 일반 레어몹 리스폰의 처리.
	-- 1~4의 레어몬스터를 할당한다. 0이면 스킵.
	table_length = table.getn( Raremob_ID )
	
	for i = 1, table_length do
	
		local respawn_passing = false
		
		-- 등록된 몬스터ID가 0이 아닐때만 리스폰을 수행함
		if Raremob_ID[i] ~= 0 then

			-- 오토 트랩 여부 체크
			if get_env("game.use_auto_trap") ~= 1 and Raremob_ID[i] < 310000 then
			
				local auto_tag = math.mod(Raremob_ID[i], 100)
			
				if auto_tag == 41 or auto_tag == 43 or auto_tag == 44 or auto_tag == 46 or auto_tag == 49 then
					respawn_passing = true
					-- cprint("respawn_passing : ID = " .. Raremob_ID[i])
				end
			end
		
			if respawn_passing == false then
		
				-- cprint( ID .. "번 지역에 뿌릴 레어몹 : " .. Raremob_ID[i] .. "번 몹을 " .. Raremob_count[i] .. "마리" )
			
				-- 리스폰을 셋팅한다.
				respawn( ID , Raremob_interval[i] , left, top, right, bottom, Raremob_ID[i] , Raremob_count[i] , Raremob_count[i] )
				
				local auto_tag = math.mod(Raremob_ID[i], 100)
				-- if auto_tag == 41 or auto_tag == 43 or auto_tag == 44 or auto_tag == 46 or auto_tag == 49 then
					-- cprint("respawn auto trap : ID = " .. Raremob_ID[i])
				-- end

			end
	
		end
		
	end
	
	-- 레이드용 레어몹 리스폰의 처리
	-- 1~4의 레어몬스터를 할당한다. 0이면 스킵.
	table_length = table.getn( Raid_Raremob_ID )
	
	for i = 1, table_length do

		-- 등록된 몬스터ID가 0이 아닐때만 리스폰을 수행함
		if Raid_Raremob_ID[i] ~= 0 then

			-- cprint( ID .. "번 지역에 뿌릴 레어몹 : " .. Raid_Raremob_ID[i] .. "번 몹을 " .. Raid_Raremob_count[i] .. "마리" )
			
			-- 리스폰을 셋팅한다.
			raid_respawn( ID , Raid_Raremob_interval[i] , left, top, right, bottom, Raid_Raremob_ID[i] , Raid_Raremob_count[i] , Raid_Raremob_count[i] )
	
		end
		
	end
	
end


	