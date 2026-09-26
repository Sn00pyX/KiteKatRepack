--  Lua 스크립트 암호화
function get_module_name()
             return "NPC_TeleportTown"
end
	 


--  귀환지역을 설정.
function BindingArea( text, pos_x, pos_y, range)
    if (gv('rx') == nil or gv('rx') == "") and (gv('ry') == nil or gv('ry') == "") then -- 해외
		set_flag( "rx", pos_x + math.random(0,range))
		set_flag( "ry", pos_y + math.random(0,range))
	else -- 국내
		sv( "rx", pos_x + math.random(0,range))
		sv( "ry", pos_y + math.random(0,range))
	end	
	message( "@" .. text)
end


--  텔레포트실행:  list중 select_id로 텔레포트
function TeleportSelect(pdata, select_id)	
	local cnt_dungeon = table.getn(pdata)/3;
	for i = 1, cnt_dungeon do
		local base = 1+(i-1)*3
		if pdata[base] == select_id then			
			   warp( pdata[base+1] + math.random(0,60) , pdata[base+2] + math.random(0,60) )
			return
		end
	end
end


--  텔레포트실행: 공통펑션
function RunTeleport( cost , x_pos , y_pos )
	local gold = get_value( "gold" )
	if gold < cost then
		message( "@90010008" )
		return
	end
	set_value( "gold", gold - cost )
    update_gold_chaos()
	save()
	warp( x_pos + math.random(0,10) , y_pos + math.random(0,10) )
	local state_code = get_local_info()
	if	state_code == 256 then -- 러시아
		add_state(201085, 1, 1500)
	elseif	state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then --미국, 유럽
		add_state(201085, 1, 3000)
	else
		add_state(201085, 1, 1000)
	end
end

--  텔레포트메뉴: 공통펑션
function MenuTeleportItem ( text, pos_x, pos_y, price )
	dlg_menu( "@" .. text .. "\v#@price@#\v" .. price  , "RunTeleport(" .. price .. "," .. pos_x .. "," .. pos_y .. ")" ) 
end

--  텔레포트메뉴: list중 skip_id를 제외한 지역을 메뉴에 추가
function MenuTeleport(pdata, skip_id)	
	local cnt_dungeon = table.getn(pdata)/7;
	local	price_idx = 3				-- 표준가격
	if is_premium() then
			price_idx = 4				-- 프리미엄 초저가
	elseif get_local_info() == 4 then
			price_idx = 5				-- 저가 (일본)
	elseif get_local_info() == 8192 then 
			price_idx = 6				-- 고가 (중동)
	end
	for i = 1, cnt_dungeon do
		local base = 1+(i-1)*7
		if pdata[base] ~= skip_id then			
			MenuTeleportItem( pdata[base],pdata[base+1],pdata[base+2],pdata[base+price_idx])
		end
	end
end


--  텔레포트메뉴: 시크루트
function MenuTeleportSecroute()
	if is_premium()	then
		dlg_menu( "@90101603\v#@price@#\v0",	'RunTeleport( 0 , 222219 , 20106 )' )		
	end
end


--  텔레포트메뉴: 길드소유던전
function MenuOwnDungeon()
	local dungeon_id = get_own_dungeon_id()
	local dungeon_relation_code = get_dungeon_relation( dungeon_id )
	if get_own_dungeon_id() ~= 0 then
		if ( dungeon_relation_code > 0 and dungeon_relation_code < 5 ) or dungeon_relation_code == 9 then
			dlg_menu( "@90010231", 'scf_teleport_to_owned_dungeon()' )
			dlg_menu( "@90605270", 'scf_teleport_to_owned_secret_dungeon()' )
		end
	end
end

--  텔레포트실행: 길드소유던전
function scf_teleport_to_owned_dungeon()
	local dungeon_id = get_own_dungeon_id()
	local pdata =	{ -- id,		x,			y,
						130000 ,	155817	,	103724	,								-- 잃어버린 갱도1
						130600 ,	152309	,	102886	,								-- 잃어버린 갱도2
						130300 ,	103210	,	100366	,								-- 수정 계곡1
						130500 ,	99757	,	103236	,								-- 수정 계곡2
						130400 ,	132995	,	87096	,								-- 메마른 달빛의 유적1
						130700 ,	130842	,	79586	,								-- 메마른 달빛의 유적2
						130800 ,	132680	,	128030	,								-- 팔미르 제 1 유적 
						130900 ,	137441	,	128115	,								-- 팔미르 제 2 유적 
						121000 ,	91985	,	117044	,								-- 백룡의 쉼터
						122000 ,	85720	,	118033	,								-- 흑룡의 그늘
						123000 ,	92027	,	124430	,								-- 사룡의 그늘
						120700 ,	146188	,	135579	,								-- 엘 카시아
	}
	TeleportSelect(pdata,dungeon_id)
	local state_code = get_local_info()
	if	state_code == 256 then -- 러시아
		add_state(201085, 1, 1500)
	elseif	state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then --미국, 유럽
		add_state(201085, 1, 3000)
	else
		add_state(201085, 1, 1000)
	end
end


--  텔레포트실행: 길드소유숨던
function scf_teleport_to_owned_secret_dungeon()
	local dungeon_id = get_own_dungeon_id()
	if dungeon_id == 130300  or dungeon_id == 130500 then								-- 수정 계곡
		warp_to_secret_dungeon(70101)
	elseif dungeon_id == 130800 or dungeon_id == 130900 then							-- 팔미르 유적 
		warp_to_secret_dungeon(120201)
	elseif dungeon_id == 121000 then													-- 백룡의 쉼터
		warp_to_secret_dungeon(100101)		
	elseif dungeon_id == 122000 then													-- 흑룡의 그늘
		warp_to_secret_dungeon(90101)
	elseif dungeon_id == 123000 then													-- 사룡의 그늘
		warp_to_secret_dungeon(80101)
	elseif dungeon_id == 120700 then													-- 엘 카시아
		warp_to_secret_dungeon(110101)
	end	
	local state_code = get_local_info()
	if	state_code == 256 then -- 러시아
		add_state(201085, 1, 1500)
	elseif	state_code == 4 or state_code == 8 or state_code == 128 or state_code == 16384 or state_code == 32768 or state_code == 65536 then --미국, 유럽
		add_state(201085, 1, 3000)
	else
		add_state(201085, 1, 1000)
	end
end


--  텔레포트메뉴: 마을
function MenuAreaCity(current_city)   -- current_city를 제외한 나머지 도시를 보여준다
	local pdata =	{ -- text,x,y,	standard	premium	low		high
		90019001,	172682 , 52104   , 0	,	0	, 0		, 0			,			 -- 수련자의 섬
		90200505,	  6625 , 6980    , 500	,	0	, 500	, 500		,			 -- 데바 라크시
		90100506,	116799 , 58205   , 500	,	0	, 500	, 500		,			 -- 아수라 카탄
		90100510,	153506 , 77175   , 500	,	0	, 500	, 500		,			 -- 가이아 호라이즌    
		90100511,	137874 , 105078  , 500	,	0	, 500	, 70000		,			 -- 론도
		90010151,	152943 , 151081  , 180000 , 4000, 80000	, 180000	,			 -- 도시 유적
		90100516,	140101 , 102600  , 70000  , 2000, 50000	, 70000		,			 -- 크리쳐 농장
		90606233,	162990 , 116317  , 800000 , 7500, 200000, 800000	,			 -- 붉은 거미 서커스장 입구	
	}
	if get_env("gmzone") == 1 then	
		dlg_menu( "<#ff0000>[ 플로트 템플 ] - 간담회 참석!!!", "warp(5000, 25000)" )
	end
	MenuTeleport(pdata,current_city)	
end


--  텔레포트메뉴: 라크시 필드
function MenuAreaDeva()
	local pdata =	{ -- text,x,y,	standard	premium	low		high
		90999311,	120718 , 143033  ,   15000 , 4500 ,	15000, 	15000 ,				-- 라크시 북쪽 삼거리
		90999312,	131613 , 136593  ,   27000 , 4500 ,	27000, 	27000 ,	 			-- 라크시 동쪽 요정의 숲 출구
		90100505,	132546 , 139965  ,   35000 , 3000 ,	35000, 	35000 ,	 			-- 마법 실험지	
		90700615,	142013 , 147483  ,   63000 , 3000 ,	63000, 	63000 ,	 			-- 리자드맨 서식지
		90100504,	139011 , 141396  ,   61000 , 3000 ,	61000, 	61000 ,	 			-- 템플러 헤드쿼터
		90700614,	142086 , 132207  ,   60000 , 3000 ,	60000, 	60000 ,	 			-- 세이렌의 섬	
	}
	MenuTeleport(pdata,0)	
end


--  텔레포트메뉴: 호라이즌 필드
function MenuAreaGaia()
	local pdata =	{ -- text,x,y,	standard	premium	low		high
		90700620,	139982 , 85162  ,   58000 , 3000 ,	48000, 	58000 ,				--월하의 공동 묘지
		90400503,	138722 , 75351  ,   58000 , 3000 ,	48000, 	58000 ,				--사혼의 제단
		90700621,	155533 , 85778  ,   37000 , 3000 ,	27000, 	37000 ,	 			-- 제 1 발모어 탄광
		90999309,	146894 , 77650  ,   31000 , 4500 ,	21000, 	31000 ,	 			-- 호라이즌 서쪽 필드
		90999310,	155104 , 93975  ,   61000 , 4500 ,	51000, 	61000 ,	 			-- 우거진 대나무 숲
	}
	MenuTeleport(pdata,0)	
end


--  텔레포트메뉴: 카탄 필드
function MenuAreaAsura()
	local pdata =	{ -- text,x,y,	standard	premium	low		high
		90700617,	107700 , 76121 ,  120000,  	3000 ,	60000, 	230000 ,			-- 사이라그 페허
		90700618,	124939 , 71987 ,  100000, 	3000 ,	48000, 	200000 ,			-- 애도의 묘지
		90999300,	124736 , 57288 ,  50000 ,  	4500 ,	24000, 	50000  ,			-- 카탄 필드 동쪽
		90999301,	123179 , 65374 ,  180000, 	4500 ,	30000, 	180000 ,			-- 카탄 북동쪽 통로 
		90999302,	109591 , 58449 ,  40000 , 	4500 ,	21000, 	40000  ,			-- 카탄 서쪽 필드
		90999304,	100576 , 82868 ,  110000, 	4500 ,	90000, 	250000 ,			-- 단절의 장벽 입구
		90010263,	113371 , 88368 ,  120000,	4500 ,  6000,	240000 ,			-- 세리우 사막
		90010262,	120236 , 72040 ,  110000,	4500 ,  55000, 	220000 ,			-- 화형터
	}
	MenuTeleport(pdata,0)	
end


--  텔레포트메뉴: 론도 필드
function MenuAreaRondoh()
	local pdata =	{ -- text,x,y,	standard	premium	 low		high
		90010171,	129112 , 109201, 5000,   	0   ,	 1000  , 	5000  ,			-- 론도 대련장 (구: 대련장)
		90700623,	108976 , 103279, 175000, 	5000,	 81000 , 	175000,			-- 수정의산 입구 (구:수정의 산)
		90700624,	134710 , 120610, 75000,  	5000,	 30000 , 	75000 ,			-- 팔미르고원 입구 - 좌표보정
		90700625,	150691 , 117877, 150000, 	5000,	 60000 , 	150000,			-- 칠흑의의 숲
		90999005,	96900  , 101308, 250000, 	5000,	 123000, 	250000,			-- 수정협곡 (구:수정의산 인근)
		90999306,	129250 , 94073 , 100000, 	7500,	 39000 , 	100000,			-- 론도 남쪽 삼거리 (구:호라이즌 인근) 
		90999307,	123635 , 103436, 100000, 	7500,	 36000 , 	100000,			-- 붉은농장 서쪽입구 (구:붉은농장가는길) 
		90999308,	150508 , 111503, 100000, 	7500,	 48000 , 	100000,			-- 마르두카 감시탑
		90999502,	159670 , 124498, 100000, 	7500,	 55000 , 	100000,			-- 마르두카군락지입구
		90999503,	125457 , 121567, 100000, 	7500,	 55000 , 	100000,			-- 해안가 (구:잃어버린 비밀의 섬) - 좌표보정
	}
	MenuTeleport(pdata,0)	
end

-- 텔레포트 메뉴 : 도시유적 인근
function MenuAreaRuinsCity()
	local pdata =	{ -- text,x,y,	standard	premium	 low		high
		90610476,	162943,	131365,	90000,		4500,	45000,		90000, -----------------마레마을
		90610477,	153062,	131621,	90000,		4500,	45000,		90000, -----------------마르두카 남부 감시탑
		90610478,	146243,	135598,	90000,		4500,	45000,		90000, -----------------폭포
		90610479,	154476,	152325,	90000,		4500,	45000,		90000, -----------------흔적의 섬
		90610480,	159650,	124060,	90000,		4500,	45000,		90000, -----------------칠흑의 숲 북쪽 입구
		90610481,	153506,	136821,	90000,		4500,	45000,		90000, -----------------마르두카 와디
	}
	MenuTeleport(pdata,0)	
end 

-- 텔레포트 메뉴 : 잃어버린 섬
function MenuAreaLostIsland()
	local pdata =	{ -- text,x,y,	standard	premium	 low		high
		90610482,	87534,	140987,	90000,		4500,	45000,		90000, -----------------얼어붙은 해안가
		90610483,	93947,	121759,	90000,		4500,	45000,		90000, -----------------얼음가시 숲
		90610484,	108074,	117640,	90000,		4500,	45000,		90000, -----------------마르두카 설원 본진
		90610485,	106923,	122380,	90000,		4500,	45000,		90000, -----------------눈보라 낫 계곡 입구
		90610486,	88991,	121541,	90000,		4500,	45000,		90000, -----------------설원 분지
	}
	MenuTeleport(pdata,0)	
end 


--  NPC: 라크시 
function NPC_TeleportTown_Deva_contact(title,text)
	
	dlg_title( "@" .. title )  
	dlg_text( "@" .. text )  


	--dlg_menu( "@90604959", "Trick_or_treat_2011()" )								--할로윈 사탕받기
	dlg_menu( "@90100507", "BindingArea(90100508, 6625,6980, 100)" )				-- 귀환지역 설정


	local npc_id = get_npc_id()
	if npc_id ==  1006 then															-- 텔레포터 오드리(1006)은 라크시 지상에 있기때문에 위로 천공으로 올려주는 메뉴가 있다.
		--dlg_title( "@90100601" )
		dlg_text( "@90100602" )
		MenuTeleportItem( 90100603 , 6625 , 6980, 0 )					-- 라크시로 텔레포트 한다 
	else	
		MenuTeleportItem ( 90100503, 122934, 138140, 0 )					-- 라크시 필드로   
	end
	MenuTeleportSecroute()
	MenuAreaCity(90200505)
	MenuAreaDeva()
	MenuOwnDungeon()																-- 땅으로 갈필요 없이 한방에 보내주자
	dlg_menu( "@90010001", " " )
	dlg_show()
end


--  NPC: 카탄
function NPC_TeleportTown_Asura_contact(title,text)
	dlg_title( "@" .. title ) 
	dlg_text( "@" .. text )  
	dlg_menu( "@90200507", "BindingArea(90200508, 116799,58205, 100)" )				-- 귀환지역 설정
	MenuTeleportSecroute()
	MenuAreaCity(90100506)
	MenuAreaAsura()
	MenuOwnDungeon()
	dlg_menu( "@90010001", " " )
	dlg_show()
end

--  NPC: 호라이즌	
function NPC_TeleportTown_Gaia_contact(title,text)
	dlg_title( "@" .. title) 
	dlg_text( "@" .. text)  
	dlg_menu( "@90400508", "BindingArea(90400509, 153513,77203, 100)" )				-- 귀환지역 설정
	MenuTeleportSecroute()
	MenuAreaCity(90100510)
	MenuAreaGaia()
	MenuOwnDungeon()
	local npc_id = get_npc_id()
	if npc_id == 4005 then
		dlg_menu( "@90999617", "quest_rumor6()" )
	end
   	dlg_menu( "@90010001", " " )
	dlg_show()
 
end

--  NPC: 호라이즌 퀘스트 (리벤델 세부대화 - 미래를 내다보는 소녀)
function quest_rumor6()
	dlg_title( "@90400501" )
	dlg_text_without_quest_menu( "@90999620" )
	dlg_menu( "@90999621", "quest_rumor_a_3()" )				
	dlg_menu( "@90010001", " " )
	dlg_show()
end

function quest_rumor_a_3()
	dlg_title( "@90400501" )
	dlg_text_without_quest_menu( "@90999624" )
	dlg_menu( "@90999627", "quest_rumor_b_3()" )				
	dlg_menu( "@90010001", " " )	
	dlg_show()

end

function quest_rumor_b_3()
	dlg_title( "@90400501" )
	dlg_text_without_quest_menu( "@90999630" )
	dlg_menu( "@90010001", " " )
	dlg_show()
end


--  NPC: 론도
function NPC_TeleportTown_Rondoh_contact(title,text)
 	dlg_title( "@" .. title )
	dlg_text( "@" .. text ) 
	dlg_menu( "@90600508", "BindingArea(90600509, 137843,105078,100)" )				-- 귀환지역 설정
	MenuTeleportSecroute()
	MenuAreaCity(90100511)
	MenuAreaRondoh()
	MenuOwnDungeon()
	dlg_menu( "@90010001", " " )
 	dlg_show()
end


--  NPC: 초보자섬	
function NPC_TeleportField_Beginner_contact()
 
	dlg_title( "@90300501" )														-- 텔레포터 오시어
	dlg_text( "@90300502" )															-- 전 브라이튼의 신관으로 텔레포트를 지원하고 있습니다
	dlg_menu( "@90300507", "BindingArea(90300508, 172185,52095,10)" )				
	local quest_progress1 =  get_quest_progress(1025)								-- 동쪽 해안가로 이동을 위한 상급 교관 찾아가기 퀘스트 수행 체크
	if quest_progress1 == 255 then
		dlg_menu( "@90300503", 'RunTeleport( 0 , 175711 ,56887 )' )	
    end	
	local race = get_value( "race" )												-- 전직후엔 다른 도시(라크시 또는 카탄)으로 이동가능. 종족ID = 가이아 3, 데바 4, 아수라 5
	if get_value( "job_depth" ) > 0 then
		if race == 4 then															-- 데바 라크시
			dlg_menu( "@90600505\v#@price@#\v500", 'RunTeleport( 10, 6625 , 6980 )' )
		elseif race == 5 then														-- 아수라 카탄
			dlg_menu( "@90600506\v#@price@#\v500", 'RunTeleport( 10, 116799 , 58205 )')	
		else																		-- 가이아 호라이즌
			dlg_menu( "@90600510\v#@price@#\v500", 'RunTeleport( 10, 153506 , 77175 )')	
		end
	end
	dlg_menu( "@90300513", 'Teleport_channel( 1000 )')								-- 채널? (다른 수련자 캠프로 이동?)
	dlg_menu( "@90010001", " " )
	dlg_show()
 
end


--  NPC: 도시유적
function NPC_TeleportTown_Ancient_relic_contact(title,text)

	local pdata =	{ -- text,x,y,	standard	premium	low		high
		90703303,	6625	, 6980	, 100000, 	0	,	 80000  , 	100000 ,		--  라크시
		90703304,	116799	, 58205	, 100000, 	0	,	 80000  , 	100000 ,	 	--  카탄
		90703305,	153506	, 77175	, 100000, 	0	,	 80000  , 	100000 ,	 	--  호라이즌
		90703306,	137874 , 105078, 100000, 	0	,	 80000  , 	100000 ,	 	--  론도
		90100518,	140101	, 102600, 150000, 	2000,	 80000  , 	150000 ,	 	--  크리쳐 농장
		90999001,	159628	, 135186, 800000, 	7500,	 51000  , 	110000 ,	 	--  마레마을 
		90999002,	148005	, 136193, 110000, 	7500,	 45000  , 	90000  ,		--  폭포
		90606233,	162990	, 116317,  90000, 	7500,	 200000 , 	800000 ,		--  붉은 거미 서커스장 입구
	}
	dlg_title( "@" .. title )  -- 텔레포터 이간
	dlg_text( "@" .. text )  -- 텔레포트하길 원하신다면 이동하실 지역을 선택하세요
	dlg_menu( "@90703307", "BindingArea(90703308, 152634,151508,100)" )				-- 귀환지역 설정
	MenuTeleportSecroute()
	MenuTeleport(pdata,0)	
	MenuOwnDungeon()
	dlg_menu( "@90010001", " " )
	dlg_show()
end


function NPC_DungeonInfo( dungeon_id )
	--local pdata = { 130400,	130700,	130300,	130500,	130800,	130900,	121000,	122000,	123000,	120700}
	local pdata = { 130400,	130700,	130000,	130600,	130300,	130500,	130800,	130900,	120700,	121000,	122000,	123000}
	local cnt	= table.getn(pdata);
	if dungeon_id == 0 then
		--소유 길드란 매주 열리는 던전 시즈에서 우승하여 해당 던전을 소유하게 된 길드를 말합니다.
		--던전 시즈는 매주 열리며 언제든지 소유 길드가 바뀔 수 있습니다. 던전을 차지하게 되면 여러 혜택을 얻을 수 있으니 길드에 가입해 계시다면 한 번 도전해 보세요.
		dlg_text( "@90408511")	
	else
		--90408510	현재 던전의 소유 길드는 #@own_guild@#이며, 세율은 #@tax_rate@#퍼센트 입니다.
		local own_guild = get_own_guild_name( dungeon_id ) 
		local tax_rate =  get_tax_rate( dungeon_id )

		local title = sconv("@90408510" , "#@own_guild@#" , own_guild , "#@tax_rate@#" , tostring(tax_rate))
		dlg_text(  "@70" .. dungeon_id .. "|<br>|" .. title)
	end

	for i = 1, cnt	do
		local tid = pdata[i]
		local text = "@70" .. tid .. "|-|<#FFFFFF>" .. get_own_guild_name( tid )
		dlg_menu( text , "NPC_DungeonInfo(" .. tid .. ")") 
	end

	dlg_menu( "@90010001", " " )
	dlg_show()
end

--  NPC:  던전텔레포터 공통
function NPC_TeleportDungeon(str_title)	
							
	local pdata =	{ -- text,x,y,	standard	premium	low		high
		90606128,	132995,	87096,		10000, 		3000,	10000, 	100000,			-- 메마른 달빛의 유적 제 1실
		90606129,	130842,	79586,		10000, 		3000,	10000, 	100000,		 	-- 메마른 달빛의 유적 제 2실
		90606130,	155817, 103724,		50000, 		3000,	50000, 	150000,		 	-- 잃어버린 갱도 제 1탄광 
		90606131,	152309, 102886,		50000, 		3000,	50000, 	150000,		 	-- 잃어버린 갱도 제 2탄광 
		90606132,	103210, 100366,		50000, 		3000,	50000, 	200000,		 	-- 제 1 수정계곡 
		90606133,	99757,	103236,		50000, 		3000,	50000, 	200000,		 	-- 제 2 수정계곡 
		90606134,	132680, 128030,		50000, 		3000,	50000, 	500000,			-- 팔미르 유적 제 1실 
		90606135,	137441, 128115,		50000, 		3000,	50000, 	500000,			-- 팔미르 유적 제 2실
		90606136,	146188, 135579,		150000,		3000,	150000,	2000000,		-- 엘 카시아	
		90606137,	91958,	117044,		150000,		3000,	150000,	2000000,		-- 백룡의 쉼터 
		90606138,	85720,	118033,		150000,		3000,	150000,	2000000,		-- 흑룡의 심장 
		90606139,	92027,	124430,		150000,		3000,	150000,	2000000,		-- 사룡의 심장 
		90606140,	98965,	129204,		200000,		3000,	200000,	5000000,		-- 큐브릭 던전 
		90606607,	38300,	118300,		90000,		7500,	200000 ,800000 ,		-- 붉은 거미 서커스장
		90610407,	108660,	76468,		500000,		7500,	500000,	500000,			--	고대인의 유적
		90610408,	98529,	127391,		500000,		7500,	500000,	500000,			--	마계의 신전
	}
	dlg_title( "@" .. str_title )
	dlg_text( "@90606127")
	dlg_menu( "@9569" , "NPC_DungeonInfo(0)" )	--9569	던전 소유 길드 정보<(version:7.3)>
	MenuTeleport(pdata,0)	
	MenuOwnDungeon()
   	dlg_menu( "@90010001", " " )
	dlg_show()
end


--  NPC: 시크루트 도시  (텔레포터 유리에는 론도로 복귀, 야미만 사용)
function NPC_TeleportTown_1_Secroute_contact()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title( npc_name)  -- 	텔레포터 야미
	if is_premium() then		
		dlg_text( "@90999705" )  -- 시크루트를 방문하신 귀한 손님들을 위하여, 깜짝 놀랄 가격에 각 종족 마을로 텔레포트 해드립니다
   		MenuAreaCity(0)
	else
		dlg_text( "@90700118" )
	end
	MenuOwnDungeon()
	dlg_menu( "@90010001", " " )
	dlg_show()
end


--  NPC: 시크루트 필드 (유적도시는 없다)
function NPC_TeleportTown_2_Secroute_contact()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	if is_premium() then
		dlg_title( npc_name)
		dlg_text( "@90700602" )  -- 시크루트에서는 방문하신 모든 분들께 사냥터로의 텔레포트를 제공해 드리고 있습니다. 원하시는 지역을 선택하신 후 해당 지역의 사냥터를 골라보세요.
		dlg_menu( "@90700613", 'NPC_TeleportTown_2_Secroute_Sub( 1 )' )
		dlg_menu( "@90700616", 'NPC_TeleportTown_2_Secroute_Sub( 2 )' )
		dlg_menu( "@90700619", 'NPC_TeleportTown_2_Secroute_Sub( 3 )' )
		dlg_menu( "@90700622", 'NPC_TeleportTown_2_Secroute_Sub( 4 )' )
		dlg_menu( "@90610474", 'NPC_TeleportTown_2_Secroute_Sub( 5 )' )
		dlg_menu( "@90610475", 'NPC_TeleportTown_2_Secroute_Sub( 6 )' )
	else
		dlg_text( "@90700118" )
	end
	MenuOwnDungeon()
	dlg_menu( "@90010001", " " )
	dlg_show()
 end


--  NPC: 간담회 지역
function NPC_TeleportTown_GM_contact()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title( npc_name)  -- 	텔레포터 야미
	dlg_text( "각 종족 마을로 텔레포트 해드립니다" )
   	MenuAreaCity(0)
	dlg_menu( "@90700613", 'NPC_TeleportTown_2_Secroute_Sub( 1 )' )
	dlg_menu( "@90700616", 'NPC_TeleportTown_2_Secroute_Sub( 2 )' )
	dlg_menu( "@90700619", 'NPC_TeleportTown_2_Secroute_Sub( 3 )' )
	dlg_menu( "@90700622", 'NPC_TeleportTown_2_Secroute_Sub( 4 )' )
	MenuOwnDungeon()
	dlg_menu( "@90010001", " " )
	dlg_show()
 end
 
 --  NPC: 시크루트 필드세부
function NPC_TeleportTown_2_Secroute_Sub( select )
	
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title( npc_name)
	dlg_text( "@90700602" )
	if select == 1 then
		MenuAreaDeva()
	elseif select == 2 then
		MenuAreaAsura()	
	elseif select == 3 then
		MenuAreaGaia()
	elseif select == 4 then
		MenuAreaRondoh()
	elseif select == 5 then
		MenuAreaRuinsCity()
	elseif select == 6 then
		MenuAreaLostIsland()
	end
	--dlg_menu( "@90010003", 'NPC_TeleportTown_2_Secroute_contact()' )
	dlg_menu( "@90010001", " " )
	dlg_show()
   
end

-- play point 상점 추가

function NPC_TeleportSecroute_PlayTime( select )
	local pdata =	{ 	-- item,	name,		price, 
				910089, 10910084, 	24,	-- 시크 1시간
				910090, 10910090, 	168,	-- 시크 1일
				910091, 10910091, 	720	-- 시크 1주
			}
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()   -- 으로 대체.
	dlg_title(npc_name)		
	point = gv('play_time_point')
	
	head = ""	
	if	select > 0 then
		cost = 	pdata[select*3-3+3]
		if	point >= cost	then
			head = "교환하였습니다. (|@" .. pdata[select*3-3+2] .. "|)<BR>"
			point = point - cost
			sv('play_time_point',point)
			insert_item( pdata[select*3-3+1],1)
		else
			head = "포인트가 부족합니다.<BR>"
			
		end
	end

	str_title = head .. "플레이포인트로 교환할 상품을 선택하세요.<BR> (현재 "..point.." Point)"
	dlg_text(str_title)	

	dlg_menu( "<#EBE6AD>|@" .. pdata[0*3+2] .. " | <#FFFFFF> - ".. pdata[0*3+3] .. " <#FFFF00>Point" , "NPC_TeleportSecroute_PlayTime(1)" ) 
	dlg_menu( "<#EBE6AD>|@" .. pdata[1*3+2] .. " | <#FFFFFF> - ".. pdata[1*3+3] .. " <#FFFF00>Point" , "NPC_TeleportSecroute_PlayTime(2)" ) 
	dlg_menu( "<#EBE6AD>|@" .. pdata[2*3+2] .. " | <#FFFFFF> - ".. pdata[2*3+3] .. " <#FFFF00>Point" , "NPC_TeleportSecroute_PlayTime(3)" ) 
	dlg_menu( "@90010002", "" )
	dlg_show()
end

 --  NPC: 마을 파견 (일반 텔레포터가 시크루트를 보내주나 스크루트 안내기능이 있다)
function NPC_TeleportSecroute_Town_contact()
 	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()   -- 으로 대체.
	dlg_title(npc_name)		
	dlg_text("@90101602")	-- 시크루트 프리 패스를 소유하신 분들을 위한 시크루트 행 특급 텔레포트를 지원하고 있습니다.
	MenuTeleportSecroute()
	dlg_menu("@90101611", 'NPC_TeleportSecroute_Town_sub( 1 )') -- 시크루트란
	dlg_menu("@90101613", 'NPC_TeleportSecroute_Town_sub( 2 )')	-- 시크루트 프리 패스란
	-- play point 상점 추가  EOP 플레이 포인트 세팅된 곳만 추가...
	if get_env("game.use_play_point") == 1 then
		dlg_menu( "플레이 타임 포인트 교환", 'NPC_TeleportSecroute_PlayTime(0)' )
	end
	dlg_menu( "@90010002", "" )
	dlg_show()

end

function NPC_TeleportSecroute_Town_sub( select )

	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()  
	dlg_title(npc_name)		
	if select == 1 then					-- 시크루트에 대한 설명
			dlg_text("@90101612")
	elseif select == 2 then				-- 시크루트 프리패스에 대한 설명
		dlg_text("@90101614")		
	end
	dlg_menu( "@90010002", "" )
	dlg_show()
end







--  채널 텔레포트 작동 (펑션 유지. 이름구하는 부분만 정리)
 
function Teleport_channel( channel_id )
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title(npc_name)
	local start_channel, end_channel
	start_channel = 1 -- 이동 가능한 채널 번호의 시작번호
	end_channel = get_max_channel_num( channel_id ) -- 이동 가능한 채널 번호의 종료번호
	-- 채널 갯수에 따른 다이얼로그 출력 분기 구성, 이동 가능한 채널이 없을 경우의 다이얼로그 출력
	if end_channel == 1 then
		dlg_text( "@90300516" )
	elseif end_channel > 1 then
		text = sconv("@90300514", "#@number1@#", tostring( start_channel ) , "#@number2@#",tostring( end_channel ))   -- 변수를 실제 값(스트링)으로 치환 시킨다.
		dlg_text( text )       -- 대사창에 텍스트 삽입
	end
		
	-- 채널이동이 불가능할 경우
	if end_channel == 1 then
		if npc_id == 3016 then								-- 란슬롯 일 때	
			dlg_menu( "@90010003", "NPC_Tutorial_Instructor_3_contact()" )
		elseif npc_id == 3005 then							-- 텔레포터 오시어 일 때
			dlg_menu( "@90010003", "NPC_TeleportField_Beginner_contact()" )
		elseif npc_id == 3022 then							-- 혹시나 그냥 오시어
			dlg_menu( "@90010003", "NPC_TeleportField_Beginner_contact()" )
		elseif npc_id == 11120 then							-- 마리캣 주인 마리캣!
			dlg_menu( "@90010003", "NPC_maricat_market_maricat_contact()" )
		elseif npc_id == 7008 then							-- 시크루트 가이드 티미!
			dlg_menu( "@90010003", "NPC_GuideTown_Secroute_contact()" )
		end	

	-- 채널이동이 가능할 경우
	elseif end_channel > 1 then
		dlg_menu( "@90300513", "show_channel_set()" )
	end
	dlg_menu( "@90010002", " " )
	dlg_show()
end


function on_channel_set( channel_number )
 	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title(npc_name)
	local input_layer, player_layer, channel_id, start_channel, end_channel
    input_layer = get_layer_of_channel( channel_id, channel_number )
    player_layer = gv("layer")
    
	if get_user_count_in_channel( channel_id , channel_number ) == 0 then		-- 아무도 없는 채널 번호를 입력했을 경우
        dlg_text( "@90300518")													-- 이동 불가 관련 메세지 다이얼로그 호출
        dlg_menu( "@90300513", "show_channel_set()" )							-- 채널 번호 입력 텍스트박스 호출
        dlg_menu( "@90010002", " " )
        dlg_show()
        return
	end
	
	if input_layer == player_layer then											-- 현재 채널과 동일한 채널 번호를 입력한 경우		
		dlg_text( "@90300519")													-- 이동 불가 관련 메세지 다이얼로그 호출
		dlg_menu( "@90300513", "show_channel_set()" )							-- 채널 번호 입력 텍스트박스 호출
		dlg_menu( "@90010002", " " )
		dlg_show()
		return

	end
	-- 전혀 무관한 숫자 입력한 경우
	start_channel = get_min_channel_num( channel_id )   -- 이동 가능한 채널 번호의 최소번호
	end_channel = get_max_channel_num( channel_id )     -- 이동 가능한 채널 번호의 최대번호

	if channel_number < start_channel or channel_number > end_channel then
		message( "@90010093" )
		return
	end
    warp( gv("x") + math.random(0,10) , gv("y") + math.random(0,10) , channel_id , channel_number )

end


-- 마리캣 (유지)
-- 마리캣 (유지)
function warp_to_market()

	if get_env("game.close_market") == 1 then
		local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
		dlg_title(npc_name)
		dlg_text_without_quest_menu( "Market is closed." )
		dlg_menu( "@90010003", "NPC_maricat_market_teleport_contact()" )	-- 돌아가기
		dlg_menu( "@90010002", " " )	-- 대화종료 
		dlg_show()
	else
		set_flag( 'mx', get_value('x') )
		set_flag( 'my', get_value('y') )
		local gate_num = math.random( 0, 2)
		-- 랜덤으로 워프 시킴
		if gate_num == 0 then
		warp( 200898 + math.random( 0, 5 ), 72983 + math.random( 0, 5 ) )
		elseif gate_num == 1 then
		warp( 202322 + math.random( 0, 5 ), 72989 + math.random( 0, 5 ) )
		elseif gate_num == 2 then
		warp( 201596 + math.random( 0, 5 ), 71744 + math.random( 0, 5 ) )
		end
	end
end

-- 마을로 돌려 보내주는 워프게이트
function quit_market()
	local mx = get_flag( 'mx' )
	local my = get_flag( 'my' )
	if mx == nil or my == nil or mx == 0 or my == 0 or mx == '' or my == '' then		
		if (gv('rx') == nil or gv('rx') == "") and (gv('ry') == nil or gv('ry') == "") then -- 해외
			mx = get_flag( 'rx' )
			my = get_flag( 'ry' )
		else -- 국내
			mx = gv( 'rx' )
			my = gv( 'ry' )
		end
	end
	if mx == nil or my == nil or mx == 0 or my == 0 or mx == '' or my == '' then
		mx = 185788
		my = 72361
	end
	del_flag( 'mx' )
	del_flag( 'my' )
	warp( mx + math.random( 0, 10 ), my + math.random( 0, 10 ) )
end	
	

-- 마리캣 마켓의 오벨리스크
function market_obelisk_tower()

	cprint("!오벨리스크 가동")
	local item_small_sujung_count = find_item( 1100303 )
	if item_small_sujung_count == 1 then
		delete_item ( get_item_handle( 1100303 ), item_small_sujung_count )		-- 작은 수정 삭제
	else
	end
end	
	
--  NPC: 마켓주인 마리캣
function NPC_maricat_market_maricat_contact()
 	
	dlg_title( "@90999373" )
	dlg_text( "@90999374" )
	dlg_menu( "@90999375", "maricat_talkLink_1()" )			-- 마리캣 마켓이란의 신상?
	dlg_menu( "@90999377", "maricat_talkLink_2()" )			-- 마을로 돌아가기?
	dlg_menu( "@90999379", "maricat_talkLink_3()" )			-- 마리캣의 신상?
	dlg_menu( "@90300513", 'Teleport_channel( 120400 )')	-- 채널 이동 해드림(마리켓 마켓)
	dlg_menu( "@90010001", " " )
	dlg_show()
 
end

--  부가정보 1 	마리캣 - 마켓이란?
function maricat_talkLink_1()

	dlg_title( "@90999373" )
	dlg_text_without_quest_menu( "@90999376" )
	dlg_menu( "@90010003", "NPC_maricat_market_maricat_contact()" )
	dlg_menu( "@90010002", " " )
	dlg_show()
end

--  부가정보 2 	마리캣 - 마을 돌아가기
function maricat_talkLink_2()
	dlg_title( "@90999373" )
	dlg_text_without_quest_menu( "@90999378" )							-- 다이얼로그 출력
	dlg_menu( "@90010003", "NPC_maricat_market_maricat_contact()" )		-- 돌아가기
	dlg_menu( "@90010002", " " )										-- 대화종료 
	dlg_show()
end

-- 부가정보 3 	마리캣 - 마리캣이란?
function maricat_talkLink_3()
	dlg_title( "@90999373" )
	dlg_text_without_quest_menu( "@90999380" )							-- 다이얼로그 출력
	dlg_menu( "@90010003", "NPC_maricat_market_maricat_contact()" )		-- 돌아가기
	dlg_menu( "@90010002", " " )										-- 대화종료 
	dlg_show()
end


--  마켓가드 파푸캣
function NPC_maricat_market_guard_contact()
	dlg_title( "@90999382" )
	dlg_text( "@90999383" )									-- 다이얼로그 출력
	dlg_menu( "@90999384", "maricat_guard_talkLink_1()" )	-- 마리캣의 신상?
	dlg_menu( "@90999386", "maricat_guard_talkLink_2()" )	-- 마리캣 마켓이란??
	dlg_menu( "@90010001", " " )
	dlg_show()
end


--  부가정보 1 	마켓 가드 - 마리캣이란?
function maricat_guard_talkLink_1()
	dlg_title( "@90999382" )
	dlg_text_without_quest_menu( "@90999385" )	-- 다이얼로그 출력
	dlg_menu( "@90010003", "NPC_maricat_market_guard_contact()" )	-- 돌아가기
	dlg_menu( "@90010002", " " )	-- 대화종료 
	dlg_show()
end

-- 부가정보 2 	마켓 가드 - 마리캣 마을은?
function maricat_guard_talkLink_2()
	dlg_title( "@90999382" )
	dlg_text_without_quest_menu( "@90999387" )	-- 다이얼로그 출력
	dlg_menu( "@90010003", "NPC_maricat_market_guard_contact()" )-- 돌아가기
	dlg_menu( "@90010002", " " )	-- 대화종료 
	dlg_show()
end

  
--  마켓 안내인
function NPC_maricat_market_teleport_contact()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title(npc_name)
	dlg_text( "@90999390" )
	dlg_menu( "@90999405", "warp_to_market()" )	-- 마켓 이동
	dlg_menu( "@90999391", "maricat_teleport_talkLink_1()" )	-- 마켓 안내인의 신상?
	dlg_menu( "@90999393", "maricat_teleport_talkLink_2()" )	-- 자유무역지구란??
	dlg_menu( "@90010001", " " )
	dlg_show()
end


-- 부가정보 1 	마켓 안내인 - 마켓 안내인이란?
function maricat_teleport_talkLink_1()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title(npc_name)
	dlg_text_without_quest_menu( "@90999392" )
	dlg_menu( "@90010003", "NPC_maricat_market_teleport_contact()" )	-- 돌아가기
	dlg_menu( "@90010002", " " )	-- 대화종료 
	dlg_show()
end


-- 부가정보 2 	마켓 안내인 - 자유무역지구?
function maricat_teleport_talkLink_2()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title(npc_name)
	dlg_text_without_quest_menu( "@90999394" )
	dlg_menu( "@90010003", "NPC_maricat_market_teleport_contact()" )	-- 돌아가기
	dlg_menu( "@90010002", " " )	-- 대화종료 
 	dlg_show()
end


--  라마단 텔레포터
function NPC_TeleportTown_ramadan_contact()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title(npc_name)
	dlg_text( "@90605386" )
	dlg_menu( "@90605383", "warp_to_ramadan()" )	-- 라마단 기도실 이동
	--dlg_menu( "@90999391", "ramadan_teleport_talkLink_1()" )			-- 라마단 기도실이란
	--dlg_menu( "@90999393", "ramadan_teleport_talkLink_2()" )			-- 라마단 기도의상
	dlg_menu( "@90010001", " " )
	dlg_show()
end

function NPC_TeleportTown_warpback_contact()
	local npc_name = "@"..get_npc_type().."|@"..get_npc_name()
	dlg_title(npc_name)
	dlg_text( "@90605389" )
	dlg_menu( "@90605390", "quit_ramadan()" )	-- 마을로 이동
	--dlg_menu( "@90999391", "ramadan_teleport_talkLink_1()" )	-- 라마단 기도실이란
	--dlg_menu( "@90999393", "ramadan_teleport_talkLink_2()" )	-- 라마단 기도의상
	dlg_menu( "@90010001", " " )
	dlg_show()
 
end


-- 라마단 기도실로 보내주는 워프게이트  
function warp_to_ramadan()
	--변신상태 입장불가
	 --4505<예티>		--4506<샐러맨더>		--4507<토깽이>			--4508<크리스탈>		--4509<마인샤프트>	 --4528<세이렌>		--4529<오크>			--4530<스켈레톤>	 --4531<블루픽시>	 
	 --4532<예티 프라임>	--4533<아발란체>		--4534<샐러맨더 크레센트>--4535<샐러맨더 킹>	--4536<세이렌 레이디>--4537<세이렌 퀸>	--4538<오크 워리어>	--4539<오크 로드>
	 --4540<스켈레톤 워리어> --4541<스켈레톤 나이트>	 --4542<아쿠아 픽시>	 --4543<오션 페어리>	 --4550<유령>	 --4555<큐브>	 --4556<크루드 큐브>	 --4557<네오 큐브>	 
	 --13754 야크 강림	 --164001 생명의 융합	 --164003 금지된 생명의 융합	 --6513 생명의 융합<시스템> --6522 금지된 생명의 융합<시스템>
	local state_id_polymorph = { 4505,4506,4507,4508,4509,4528,4529,4530,4531,4532,4533,4534,4535,4536,4537,4538,4539,4540,4541,4542,4543,4550,4555,4556,4557,164001,164003,6513,6522 }
	for i = 1, table.getn( state_id_polymorph )	do
		if get_state_level( state_id_polymorph[ i ] ) > 0 then
			cprint("@90605391")
			return
		end
	end
	set_flag( 'ramdanx', get_value('x') )
	set_flag( 'ramdany', get_value('y') )
	warp( 24927 + math.random( 0, 50 ), 8778 + math.random( 0, 50 ), 0 )

end

--  마을로 돌려 보내주는 워프게이트
function quit_ramadan()
	local ramdanx = get_flag( 'ramdanx' )
	local ramdany = get_flag( 'ramdany' )
	if ramdanx == nil or ramdany == nil or ramdanx == 0 or ramdany == 0 or ramdanx == '' or ramdany == '' then		
		if (gv('rx') == nil or gv('rx') == "") and (gv('ry') == nil or gv('ry') == "") then -- 해외
			ramdanx = get_flag( 'rx' )
			ramdany = get_flag( 'ry' )
		else -- 국내
			ramdanx = gv( 'rx' )
			ramdany = gv( 'ry' )
		end
	end
	del_flag( 'ramdanx' )
	del_flag( 'ramdany' )
	warp( ramdanx + math.random( 0, 10 ), ramdany + math.random( 0, 10 ), 0 )
end