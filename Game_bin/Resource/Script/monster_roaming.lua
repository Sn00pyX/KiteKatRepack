-- Lua 스크립트 암호화1
function get_module_name()
             return "monster_roaming"
end 

--set_way_point_type( roaming_id, roaming_type )
--roaming_id : 로밍 아이디
--roaming_type : 로밍 타입 1:왕복, 2:회전

--function respawn_rare_mob( id, interval, x, y, mob_id, count, is_wandering, roaming_id )

--id: 쓰레기 값.-_- -1을 넣어주세요. 기존 함수와 호환성 유지를 위해.
--interval: 1/100초 단위. 1초는 100, 1분은 6000
--x: x좌표
--y: y좌표
--mob_id: 몬스터 id
--count: 숫자
--is_wandering: 로밍 여부. 1이면 로밍. 0이면 로밍 안 함.
--roaming_id : 로밍 아이디


function roaming()


		--- 메마른 달빛 유적 2 ---

	set_way_point_type( 1001, 1 )
	add_way_point( 1001, 218479, 125300 )
	add_way_point( 1001, 218518, 124798 )
	add_way_point( 1001, 218985, 124551 )
	add_way_point( 1001, 219262, 124437 )
	add_way_point( 1001, 219729, 124623 )
	add_way_point( 1001, 219887, 125143 )
	add_way_point( 1001, 219823, 125913 )
	add_way_point( 1001, 218902, 125961 )
	add_way_point( 1001, 218490, 125686 )
	add_way_point( 1001, 218472, 125285 )

	
	set_way_point_type( 1002, 1 )
	add_way_point( 1002, 217867, 124496 )
	add_way_point( 1002, 217837, 125398 )
	add_way_point( 1002, 218277, 125933 )
	add_way_point( 1002, 218700, 126298 )
	add_way_point( 1002, 219365, 126888 )
	add_way_point( 1002, 219653, 127133 )
	add_way_point( 1002, 219747, 127210 )
	add_way_point( 1002, 220020, 127211 )
	add_way_point( 1002, 220072, 127061 )

	set_way_point_type( 1003, 1 )
	add_way_point( 1003, 220862, 128057 )
	add_way_point( 1003, 221018, 128043 )
	add_way_point( 1003, 220950, 127776 )
	add_way_point( 1003, 220974, 127298 )
	add_way_point( 1003, 221279, 127096 )
	add_way_point( 1003, 221366, 127011 )
	add_way_point( 1003, 221217, 126871 )
	add_way_point( 1003, 220958, 126880 )
	add_way_point( 1003, 220858, 127088 )
	add_way_point( 1003, 220972, 127295 )
	add_way_point( 1003, 220941, 127790 )
	add_way_point( 1003, 220860, 128070 )
	
	set_way_point_type( 1004, 1 )
	add_way_point( 1004, 222763, 126344 )
	add_way_point( 1004, 222763, 126163 )
	add_way_point( 1004, 222677, 125992 )
	add_way_point( 1004, 222239, 125633 )
	add_way_point( 1004, 221908, 125324 )
	add_way_point( 1004, 221856, 125173 )
	add_way_point( 1004, 222087, 124605 )
	add_way_point( 1004, 222209, 124194 )
	add_way_point( 1004, 222258, 123705 )
	
	
	set_way_point_type( 1005, 1 )
	add_way_point( 1005, 221269, 122304 )
	add_way_point( 1005, 221301, 121980 )
	add_way_point( 1005, 221845, 122448 )
	add_way_point( 1005, 222185, 122298 )
	add_way_point( 1005, 222173, 122007 )
	add_way_point( 1005, 221797, 121946 )
	add_way_point( 1005, 221437, 122312 )
	add_way_point( 1005, 221269, 122304 )
	
	
	set_way_point_type( 1006, 1 )
	add_way_point( 1006, 223706, 122977 )
	add_way_point( 1006, 224046, 122727 )
	add_way_point( 1006, 224188, 122629 )
	add_way_point( 1006, 224320, 122619 )
	add_way_point( 1006, 224461, 122700 )
	add_way_point( 1006, 224513, 122873 )
	add_way_point( 1006, 224557, 123127 )
	add_way_point( 1006, 224538, 123469 )
	add_way_point( 1006, 224568, 123694 )
	add_way_point( 1006, 224184, 123729 )
	add_way_point( 1006, 224073, 123914 )
	add_way_point( 1006, 223952, 123917 )
	add_way_point( 1006, 223898, 124044 )
	add_way_point( 1006, 223950, 124095 )
	add_way_point( 1006, 223990, 124130 )
	add_way_point( 1006, 224026, 124158 )
	add_way_point( 1006, 224098, 124100 )
	add_way_point( 1006, 224167, 124034 )
	add_way_point( 1006, 224411, 124231 )
	add_way_point( 1006, 224616, 124423 )
	add_way_point( 1006, 224924, 124578 )
	add_way_point( 1006, 224909, 124387 )
	add_way_point( 1006, 225089, 124426 )
	add_way_point( 1006, 225257, 124246 )
	
	
	set_way_point_type( 1007, 1 )
	add_way_point( 1007, 218563, 120821 )
	add_way_point( 1007, 218774, 120827 )
	add_way_point( 1007, 218930, 121381 )
	add_way_point( 1007, 219168, 121672 )
	
	
	set_way_point_type( 1008, 1 )
	add_way_point( 1008, 217983, 122555 )
	add_way_point( 1008, 217610, 122502 )
	add_way_point( 1008, 217937, 121907 )
	add_way_point( 1008, 218061, 121705 )
	add_way_point( 1008, 218574, 121764 )
	add_way_point( 1008, 218775, 121855 )
	add_way_point( 1008, 218996, 121855 )
	add_way_point( 1008, 219179, 121686 )
	
	
	set_way_point_type( 1009, 1 )
	add_way_point( 1009, 219197, 121644 )
	add_way_point( 1009, 219277, 121381 )
	add_way_point( 1009, 219423, 121004 )
	add_way_point( 1009, 219597, 120859 )
	add_way_point( 1009, 219760, 120745 )
	add_way_point( 1009, 219886, 120721 )
	add_way_point( 1009, 220160, 120731 )
	add_way_point( 1009, 220248, 120811 )
	add_way_point( 1009, 220249, 120977 )
	add_way_point( 1009, 220252, 121218 )
	add_way_point( 1009, 220281, 121351 )
	add_way_point( 1009, 220377, 121400 )
	add_way_point( 1009, 220588, 121358 )
	add_way_point( 1009, 220906, 121257 )
	add_way_point( 1009, 221028, 121200 )
	add_way_point( 1009, 221253, 121321 )
	add_way_point( 1009, 221311, 121422 )
	
	
	set_way_point_type( 1010, 1 )
	add_way_point( 1010, 220120, 122017 )
	add_way_point( 1010, 220141, 121965 )
	add_way_point( 1010, 219769, 121986 )
	add_way_point( 1010, 219716, 121999 )
	add_way_point( 1010, 219282, 122334 )
	add_way_point( 1010, 218998, 122559 )
	add_way_point( 1010, 218820, 122477 )
	add_way_point( 1010, 218836, 122625 )
	add_way_point( 1010, 218642, 122769 )
	add_way_point( 1010, 218437, 122763 )
	add_way_point( 1010, 218263, 122721 )
	add_way_point( 1010, 218194, 122696 )
	
	
	set_way_point_type( 1011, 1 )	
	add_way_point( 1011, 223033, 125027 )
	add_way_point( 1011, 223168, 124860 )
	add_way_point( 1011, 222892, 124712 )
	add_way_point( 1011, 223176, 124678 )
	add_way_point( 1011, 222967, 124499 )
	add_way_point( 1011, 223199, 124423 )
	add_way_point( 1011, 223098, 124229 )
	add_way_point( 1011, 223223, 124177 )
	add_way_point( 1011, 223215, 123933 )
	add_way_point( 1011, 223227, 123787 )
	add_way_point( 1011, 223234, 123590 )
	add_way_point( 1011, 223243, 123433 )
	add_way_point( 1011, 223226, 123258 )
	add_way_point( 1011, 223138, 123317 )
	add_way_point( 1011, 223099, 123067 )
	add_way_point( 1011, 222864, 123051 )
	add_way_point( 1011, 222707, 122680 )
	add_way_point( 1011, 221729, 121885 )
	
	
	set_way_point_type( 1012, 1 )	
	add_way_point( 1012, 223452, 125994 )
	add_way_point( 1012, 223398, 125542 )
	add_way_point( 1012, 223963, 125510 )
	add_way_point( 1012, 224325, 125017 )
	add_way_point( 1012, 224339, 124859 )
	add_way_point( 1012, 224104, 124619 )
	
	
	set_way_point_type( 1013, 1 )	
	add_way_point( 1013, 216914, 124432 )
	add_way_point( 1013, 217509, 124460 )
	add_way_point( 1013, 217764, 124574 )
	add_way_point( 1013, 217940, 124554 )
	add_way_point( 1013, 218020, 124373 )
	add_way_point( 1013, 218016, 124251 )
	add_way_point( 1013, 217901, 124160 )
	add_way_point( 1013, 217692, 124249 )
	add_way_point( 1013, 217601, 124370 )
	add_way_point( 1013, 217513, 124455 )
	add_way_point( 1013, 216927, 124381 )
	
	
	set_way_point_type( 1014, 1 )	
	add_way_point( 1014, 217818, 124280 )
	add_way_point( 1014, 217677, 124163 )
	add_way_point( 1014, 217510, 124039 )
	add_way_point( 1014, 217187, 123721 )
	add_way_point( 1014, 217309, 123438 )
	add_way_point( 1014, 217404, 123290 )
	add_way_point( 1014, 217519, 123223 )
	add_way_point( 1014, 218332, 123535 )
	add_way_point( 1014, 218800, 123753 )
	add_way_point( 1014, 219134, 123985 )


	set_way_point_type( 1015, 1 )	
	add_way_point( 1015, 219867, 124551 )
	add_way_point( 1015, 219849, 124242 )
	add_way_point( 1015, 219763, 123933 )
	add_way_point( 1015, 219803, 123264 )
	add_way_point( 1015, 219880, 123029 )
	add_way_point( 1015, 220083, 122927 )
	add_way_point( 1015, 220265, 122839 )


	set_way_point_type( 1016, 1 )	
	add_way_point( 1016, 219896, 124549 )
	add_way_point( 1016, 219837, 124167 )
	add_way_point( 1016, 219798, 123840 )
	add_way_point( 1016, 219802, 123591 )
	add_way_point( 1016, 219806, 123315 )
	add_way_point( 1016, 219854, 123069 )
	add_way_point( 1016, 220023, 122906 )
	add_way_point( 1016, 220126, 122857 )
	add_way_point( 1016, 220243, 122807 )


	set_way_point_type( 1017, 1 )	
	add_way_point( 1017, 218523, 125553 )
	add_way_point( 1017, 218467, 125138 )
	add_way_point( 1017, 218486, 124904 )
	add_way_point( 1017, 218612, 124677 )
	add_way_point( 1017, 218813, 124554 )
	add_way_point( 1017, 219017, 124489 )
	add_way_point( 1017, 219252, 124426 )
	add_way_point( 1017, 219411, 124405 )
	add_way_point( 1017, 219591, 124535 )
	add_way_point( 1017, 219795, 124712 )
	add_way_point( 1017, 219863, 124946 )
	add_way_point( 1017, 219897, 125217 )
	add_way_point( 1017, 219888, 125454 )
	add_way_point( 1017, 219899, 125715 )
	add_way_point( 1017, 219782, 125902 )
	add_way_point( 1017, 219649, 125930 )
	add_way_point( 1017, 218813, 125950 )
	add_way_point( 1017, 218622, 125671 )
	add_way_point( 1017, 218536, 125571 )
	add_way_point( 1017, 218526, 125553 )


	set_way_point_type( 1018, 1 )	
	add_way_point( 1018, 220932, 123882 )
	add_way_point( 1018, 220565, 123892 )
	add_way_point( 1018, 220461, 123496 )
	add_way_point( 1018, 220779, 123257 )
	add_way_point( 1018, 220916, 123339 )
	add_way_point( 1018, 221085, 123432 )
	add_way_point( 1018, 221015, 123838 )
	add_way_point( 1018, 220598, 123814 )
	add_way_point( 1018, 220531, 123500 )
	add_way_point( 1018, 220779, 123311 )


	set_way_point_type( 1019, 1 )	
	add_way_point( 1019, 220946, 123965 )
	add_way_point( 1019, 221191, 123777 )
	add_way_point( 1019, 221296, 123402 )
	add_way_point( 1019, 220846, 123063 )
	add_way_point( 1019, 220766, 123120 )
	add_way_point( 1019, 220826, 123242 )
	add_way_point( 1019, 220446, 123526 )
	add_way_point( 1019, 220358, 123429 )
	add_way_point( 1019, 220294, 123473 )
	add_way_point( 1019, 220482, 123998 )
	add_way_point( 1019, 220844, 124041 )
	add_way_point( 1019, 220946, 123967 )


	set_way_point_type( 1020, 1 )	
	add_way_point( 1020, 220639, 123776 )
	add_way_point( 1020, 220622, 123730 )
	add_way_point( 1020, 220594, 123508 )
	add_way_point( 1020, 220797, 123348 )
	add_way_point( 1020, 221022, 123471 )
	add_way_point( 1020, 221010, 123633 )
	add_way_point( 1020, 220969, 123790 )
	add_way_point( 1020, 220775, 123782 )
	add_way_point( 1020, 220693, 123771 )
	add_way_point( 1020, 220642, 123771 )


	set_way_point_type( 1021, 1 )	
	add_way_point( 1021, 220305, 122931 )
	add_way_point( 1021, 220681, 123431 )
	add_way_point( 1021, 220571, 123529 )
	add_way_point( 1021, 220639, 123794 )
	add_way_point( 1021, 220976, 123804 )
	add_way_point( 1021, 221050, 123461 )
	add_way_point( 1021, 220831, 123334 )
	add_way_point( 1021, 220555, 123557 )
	add_way_point( 1021, 220655, 123842 )
	add_way_point( 1021, 220891, 123856 )
	add_way_point( 1021, 220912, 124678 )


	set_way_point_type( 1022, 1 )	
	add_way_point( 1022, 220146, 122034 )
	add_way_point( 1022, 220261, 121883 )
	add_way_point( 1022, 221455, 121936 )
	add_way_point( 1022, 221926, 122668 )
	add_way_point( 1022, 222185, 123048 )
	add_way_point( 1022, 221770, 122227 )
	add_way_point( 1022, 220721, 122363 )


	set_way_point_type( 1023, 1 )	
	add_way_point( 1023, 220110, 127187 )
	add_way_point( 1023, 220816, 127224 )
	add_way_point( 1023, 221006, 127040 )
	add_way_point( 1023, 221154, 127028 )
	add_way_point( 1023, 221442, 127067 )
	add_way_point( 1023, 221811, 127194 )
	add_way_point( 1023, 222037, 127218 )
	add_way_point( 1023, 222152, 127149 )


	set_way_point_type( 1024, 1 )	
	add_way_point( 1024, 219103, 123983 )
	add_way_point( 1024, 218914, 123802 )
	add_way_point( 1024, 218740, 123723 )
	add_way_point( 1024, 218732, 123660 )
	add_way_point( 1024, 218821, 123511 )
	add_way_point( 1024, 218956, 123333 )
	add_way_point( 1024, 219131, 123173 )
	add_way_point( 1024, 219331, 122955 )
	add_way_point( 1024, 219471, 122814 )
	add_way_point( 1024, 219668, 122714 )
	add_way_point( 1024, 219796, 122644 )
	add_way_point( 1024, 219948, 122552 )
	add_way_point( 1024, 220070, 122495 )
	add_way_point( 1024, 220381, 122406 )
	add_way_point( 1024, 220564, 122368 )


	set_way_point_type( 1025, 1 )	
	add_way_point( 1025, 221064, 121109 )
	add_way_point( 1025, 221274, 120737 )
	add_way_point( 1025, 221958, 120888 )
	add_way_point( 1025, 221305, 120904 )
	add_way_point( 1025, 221209, 120924 )
	add_way_point( 1025, 221076, 121104 )
	add_way_point( 1025, 221063, 121110 )


	set_way_point_type( 1026, 1 )	
	add_way_point( 1026, 223118, 121358 )
	add_way_point( 1026, 223558, 121541 )
	add_way_point( 1026, 223742, 121637 )
	add_way_point( 1026, 223773, 121727 )
	add_way_point( 1026, 223669, 121848 )
	add_way_point( 1026, 223563, 121941 )
	add_way_point( 1026, 223504, 122020 )
	add_way_point( 1026, 223411, 122137 )
	add_way_point( 1026, 223345, 122249 )
	add_way_point( 1026, 223363, 122310 )
	add_way_point( 1026, 223421, 122369 )
	add_way_point( 1026, 223487, 122374 )
	add_way_point( 1026, 223580, 122378 )
	add_way_point( 1026, 223654, 122408 )
	add_way_point( 1026, 223680, 122452 )
	add_way_point( 1026, 223695, 122525 )
	add_way_point( 1026, 223705, 122587 )
	add_way_point( 1026, 223700, 122674 )
	add_way_point( 1026, 223681, 122728 )
	add_way_point( 1026, 223675, 122791 )


	set_way_point_type( 1027, 1 )	
	add_way_point( 1027, 220632, 124768 )
	add_way_point( 1027, 221167, 124767 )


	set_way_point_type( 1028, 1 )	
	add_way_point( 1028, 221168, 124754 )
	add_way_point( 1028, 220632, 124755 )



	respawn_rare_mob( -1, 50000, 218479, 125300, 9047008, 1, 0, 1001 )
	respawn_rare_mob( -1, 50000, 217867, 124496, 9047002, 1, 0, 1002 )
	respawn_rare_mob( -1, 50000, 220862, 128057, 9047007, 1, 0, 1003 )
	respawn_rare_mob( -1, 50000, 222763, 126344, 9045008, 1, 0, 1004 )
	respawn_rare_mob( -1, 50000, 221269, 122304, 9045007, 1, 0, 1005 )
	respawn_rare_mob( -1, 50000, 223706, 122977, 9036010, 1, 0, 1006 )
	respawn_rare_mob( -1, 50000, 218563, 120821, 9039008, 1, 0, 1007 )
	respawn_rare_mob( -1, 50000, 217983, 122555, 9045006, 1, 0, 1008 )
	respawn_rare_mob( -1, 50000, 219197, 121644, 9040011, 1, 0, 1009 )
	respawn_rare_mob( -1, 50000, 220120, 122017, 9041017, 1, 0, 1010 )
	respawn_rare_mob( -1, 50000, 223033, 125027, 9042005, 1, 0, 1011 )
	respawn_rare_mob( -1, 50000, 223452, 125994, 9039012, 1, 0, 1012 )
	respawn_rare_mob( -1, 50000, 216914, 124432, 9042002, 1, 0, 1013 )
	respawn_rare_mob( -1, 50000, 217818, 124280, 9046002, 1, 0, 1014 )
	respawn_rare_mob( -1, 50000, 219867, 124551, 9049002, 1, 0, 1015 )
	respawn_rare_mob( -1, 50000, 219896, 124549, 9049009, 1, 0, 1016 )
	respawn_rare_mob( -1, 50000, 218523, 125553, 9050012, 1, 0, 1017 )
	respawn_rare_mob( -1, 50000, 220932, 123882, 9049013, 1, 0, 1018 )
	respawn_rare_mob( -1, 50000, 220946, 123965, 9049012, 1, 0, 1019 )
	respawn_rare_mob( -1, 50000, 220639, 123776, 9049012, 1, 0, 1020 )
	respawn_rare_mob( -1, 50000, 220305, 122931, 9049012, 1, 0, 1021 )
	respawn_rare_mob( -1, 50000, 220146, 122034, 9049012, 1, 0, 1022 )
	respawn_rare_mob( -1, 50000, 220110, 127187, 9049012, 1, 0, 1023 )
	respawn_rare_mob( -1, 50000, 219103, 123983, 9044018, 1, 0, 1024 )
	respawn_rare_mob( -1, 50000, 221064, 121109, 9034002, 1, 0, 1025 )
	respawn_rare_mob( -1, 50000, 223118, 121358, 9034003, 1, 0, 1026 )
	respawn_rare_mob( -1, 50000, 220632, 124768, 9050012, 1, 0, 1027 )
	respawn_rare_mob( -1, 50000, 221168, 124754, 9050012, 1, 0, 1028 )
	


	-- 매마른 달빛 유적 1 --
	
	
	set_way_point_type(1501, 1)
	add_way_point(1501,218479,76916)
	add_way_point(1501,218518,76414)
	add_way_point(1501,218985,76167)
	add_way_point(1501,219262,76053)
	add_way_point(1501,219729,76239)
	add_way_point(1501,219887,76759)
	add_way_point(1501,219823,77529)
	add_way_point(1501,218902,77577)
	add_way_point(1501,218490,77302)
	add_way_point(1501,218472,76901)
	
	
	set_way_point_type(1502, 1)
	add_way_point(1502,217867,76112)
	add_way_point(1502,217837,77014)
	add_way_point(1502,218277,77549)
	add_way_point(1502,218700,77914)
	add_way_point(1502,219365,78504)
	add_way_point(1502,219653,78749)
	add_way_point(1502,219747,78826)
	add_way_point(1502,220020,78827)
	add_way_point(1502,220072,78677)
	
	set_way_point_type(1503, 1)
	add_way_point(1503,220862,79673)
	add_way_point(1503,221518,79659)
	add_way_point(1503,220950,79392)
	add_way_point(1503,220974,78914)
	add_way_point(1503,221279,78712)
	add_way_point(1503,221366,78627)
	add_way_point(1503,221217,78487)
	add_way_point(1503,220958,78496)
	add_way_point(1503,220858,78704)
	add_way_point(1503,220972,78911)
	add_way_point(1503,220941,79406)
	add_way_point(1503,220860,79686)
	
	set_way_point_type(1504, 1)
	add_way_point(1504,222763,77960)
	add_way_point(1504,222763,77779)
	add_way_point(1504,222677,77608)
	add_way_point(1504,222239,77249)
	add_way_point(1504,221908,76940)
	add_way_point(1504,221856,76789)
	add_way_point(1504,222087,76221)
	add_way_point(1504,222209,75810)
	add_way_point(1504,222258,75321)
	
	
	set_way_point_type(1505, 1)
	add_way_point(1505,221269,73920)
	add_way_point(1505,221301,73596)
	add_way_point(1505,221845,74064)
	add_way_point(1505,222185,73914)
	add_way_point(1505,222173,73623)
	add_way_point(1505,221797,73562)
	add_way_point(1505,221437,73928)
	add_way_point(1505,221269,73920)
	
	
	set_way_point_type(1506, 1)
	add_way_point(1506,223706,74593)
	add_way_point(1506,224046,74343)
	add_way_point(1506,224188,74245)
	add_way_point(1506,224320,74235)
	add_way_point(1506,224461,74316)
	add_way_point(1506,224513,74489)
	add_way_point(1506,224557,74743)
	add_way_point(1506,224538,75085)
	add_way_point(1506,224568,75310)
	add_way_point(1506,224184,75345)
	add_way_point(1506,224073,75530)
	add_way_point(1506,223952,75533)
	add_way_point(1506,223898,75660)
	add_way_point(1506,223950,75711)
	add_way_point(1506,223990,75746)
	add_way_point(1506,224026,75774)
	add_way_point(1506,224098,75716)
	add_way_point(1506,224167,75650)
	add_way_point(1506,224411,75847)
	add_way_point(1506,224616,76039)
	add_way_point(1506,224924,76194)
	add_way_point(1506,224909,76003)
	add_way_point(1506,225089,76042)
	add_way_point(1506,225257,75862)
	
	
	set_way_point_type(1507, 1)
	add_way_point(1507,218563,72437)
	add_way_point(1507,218774,72443)
	add_way_point(1507,218930,72997)
	add_way_point(1507,219168,73288)
	
	
	set_way_point_type(1508, 1)
	add_way_point(1508,217983,74171)
	add_way_point(1508,217610,74118)
	add_way_point(1508,217937,73523)
	add_way_point(1508,218061,73321)
	add_way_point(1508,218574,73380)
	add_way_point(1508,218775,73471)
	add_way_point(1508,218996,73471)
	add_way_point(1508,219179,73302)
	
	
	set_way_point_type(1509, 1)
	add_way_point(1509,219197,73260)
	add_way_point(1509,219277,72997)
	add_way_point(1509,219423,72620)
	add_way_point(1509,219597,72475)
	add_way_point(1509,219760,72361)
	add_way_point(1509,219886,72337)
	add_way_point(1509,220160,72347)
	add_way_point(1509,220248,72427)
	add_way_point(1509,220249,72593)
	add_way_point(1509,220252,72834)
	add_way_point(1509,220281,72967)
	add_way_point(1509,220377,73016)
	add_way_point(1509,220588,72974)
	add_way_point(1509,220906,72873)
	add_way_point(1509,221528,72816)
	add_way_point(1509,221253,72937)
	add_way_point(1509,221311,73038)
	
	
	set_way_point_type(1510, 1)
	add_way_point(1510,220120,73633)
	add_way_point(1510,220141,73581)
	add_way_point(1510,219769,73602)
	add_way_point(1510,219716,73615)
	add_way_point(1510,219282,73950)
	add_way_point(1510,218998,74175)
	add_way_point(1510,218820,74093)
	add_way_point(1510,218836,74241)
	add_way_point(1510,218642,74385)
	add_way_point(1510,218437,74379)
	add_way_point(1510,218263,74337)
	add_way_point(1510,218194,74312)
	
	
	set_way_point_type(1511, 1)
	add_way_point(1511,223033,76643)
	add_way_point(1511,223168,76476)
	add_way_point(1511,222892,76328)
	add_way_point(1511,223176,76294)
	add_way_point(1511,222967,76115)
	add_way_point(1511,223199,76039)
	add_way_point(1511,223098,75845)
	add_way_point(1511,223223,75793)
	add_way_point(1511,223215,75549)
	add_way_point(1511,223227,75403)
	add_way_point(1511,223234,75206)
	add_way_point(1511,223243,75049)
	add_way_point(1511,223226,74874)
	add_way_point(1511,223138,74933)
	add_way_point(1511,223099,74683)
	add_way_point(1511,222864,74667)
	add_way_point(1511,222707,74296)
	add_way_point(1511,221729,73501)
	
	
	set_way_point_type(1512, 1)
	add_way_point(1512,223452,77610)
	add_way_point(1512,223398,77158)
	add_way_point(1512,223963,77126)
	add_way_point(1512,224325,76633)
	add_way_point(1512,224339,76475)
	add_way_point(1512,224104,76235)
	
	
	set_way_point_type(1513, 1)
	add_way_point(1513,216914,76048)
	add_way_point(1513,217509,76076)
	add_way_point(1513,217764,76190)
	add_way_point(1513,217940,76170)
	add_way_point(1513,218020,75989)
	add_way_point(1513,218016,75867)
	add_way_point(1513,217901,75776)
	add_way_point(1513,217692,75865)
	add_way_point(1513,217601,75986)
	add_way_point(1513,217513,76071)
	add_way_point(1513,216927,75997)
	
	
	set_way_point_type(1514, 1)
	add_way_point(1514,217818,75896)
	add_way_point(1514,217677,75779)
	add_way_point(1514,217510,75655)
	add_way_point(1514,217187,75337)
	add_way_point(1514,217309,75054)
	add_way_point(1514,217404,74906)
	add_way_point(1514,217519,74839)
	add_way_point(1514,218332,75151)
	add_way_point(1514,218800,75369)
	add_way_point(1514,219134,75601)
	
	
	set_way_point_type(1515, 1)
	add_way_point(1515,219867,76167)
	add_way_point(1515,219849,75858)
	add_way_point(1515,219763,75549)
	add_way_point(1515,219803,74880)
	add_way_point(1515,219880,74645)
	add_way_point(1515,220083,74543)
	add_way_point(1515,220265,74455)
	
	
	set_way_point_type(1516, 1)
	add_way_point(1516,219896,76165)
	add_way_point(1516,219837,75783)
	add_way_point(1516,219798,75456)
	add_way_point(1516,219802,75207)
	add_way_point(1516,219806,74931)
	add_way_point(1516,219854,74685)
	add_way_point(1516,220023,74522)
	add_way_point(1516,220126,74473)
	add_way_point(1516,220243,74423)
	
	
	set_way_point_type(1517, 1)
	add_way_point(1517,218523,77169)
	add_way_point(1517,218467,76754)
	add_way_point(1517,218486,76520)
	add_way_point(1517,218612,76293)
	add_way_point(1517,218813,76170)
	add_way_point(1517,219017,76105)
	add_way_point(1517,219252,76042)
	add_way_point(1517,219411,76021)
	add_way_point(1517,219591,76151)
	add_way_point(1517,219795,76328)
	add_way_point(1517,219863,76562)
	add_way_point(1517,219897,76833)
	add_way_point(1517,219888,77070)
	add_way_point(1517,219899,77331)
	add_way_point(1517,219782,77518)
	add_way_point(1517,219649,77546)
	add_way_point(1517,218813,77566)
	add_way_point(1517,218622,77287)
	add_way_point(1517,218536,77187)
	add_way_point(1517,218526,77169)
	
	
	set_way_point_type(1518, 1)
	add_way_point(1518,220932,75498)
	add_way_point(1518,220565,75508)
	add_way_point(1518,220461,75112)
	add_way_point(1518,220779,74873)
	add_way_point(1518,220916,74955)
	add_way_point(1518,221085,75048)
	add_way_point(1518,221515,75454)
	add_way_point(1518,220598,75430)
	add_way_point(1518,220531,75116)
	add_way_point(1518,220779,74927)
	
	
	set_way_point_type(1519, 1)
	add_way_point(1519,220946,75581)
	add_way_point(1519,221191,75393)
	add_way_point(1519,221296,75018)
	add_way_point(1519,220846,74679)
	add_way_point(1519,220766,74736)
	add_way_point(1519,220826,74858)
	add_way_point(1519,220446,75142)
	add_way_point(1519,220358,75045)
	add_way_point(1519,220294,75089)
	add_way_point(1519,220482,75614)
	add_way_point(1519,220844,75657)
	add_way_point(1519,220946,75583)
	
	
	set_way_point_type(1520, 1)
	add_way_point(1520,220639,75392)
	add_way_point(1520,220622,75346)
	add_way_point(1520,220594,75124)
	add_way_point(1520,220797,74964)
	add_way_point(1520,221522,75087)
	add_way_point(1520,221510,75249)
	add_way_point(1520,220969,75406)
	add_way_point(1520,220775,75398)
	add_way_point(1520,220693,75387)
	add_way_point(1520,220642,75387)
	
	
	set_way_point_type(1521, 1)
	add_way_point(1521,220305,74547)
	add_way_point(1521,220681,75047)
	add_way_point(1521,220571,75145)
	add_way_point(1521,220639,75410)
	add_way_point(1521,220976,75420)
	add_way_point(1521,221050,75077)
	add_way_point(1521,220831,74950)
	add_way_point(1521,220555,75173)
	add_way_point(1521,220655,75458)
	add_way_point(1521,220891,75472)
	add_way_point(1521,220912,76294)
	
	
	set_way_point_type(1522, 1)
	add_way_point(1522,220146,73650)
	add_way_point(1522,220261,73499)
	add_way_point(1522,221455,73552)
	add_way_point(1522,221926,74284)
	add_way_point(1522,222185,74664)
	add_way_point(1522,221770,73843)
	add_way_point(1522,220721,73979)
	
	
	set_way_point_type(1523, 1)
	add_way_point(1523,220110,78803)
	add_way_point(1523,220816,78840)
	add_way_point(1523,221506,78656)
	add_way_point(1523,221154,78644)
	add_way_point(1523,221442,78683)
	add_way_point(1523,221811,78810)
	add_way_point(1523,222037,78834)
	add_way_point(1523,222152,78765)
	
	
	set_way_point_type(1524, 1)
	add_way_point(1524,219103,75599)
	add_way_point(1524,218914,75418)
	add_way_point(1524,218740,75339)
	add_way_point(1524,218732,75276)
	add_way_point(1524,218821,75127)
	add_way_point(1524,218956,74949)
	add_way_point(1524,219131,74789)
	add_way_point(1524,219331,74571)
	add_way_point(1524,219471,74430)
	add_way_point(1524,219668,74330)
	add_way_point(1524,219796,74260)
	add_way_point(1524,219948,74168)
	add_way_point(1524,220070,74111)
	add_way_point(1524,220381,74022)
	add_way_point(1524,220564,73984)
	
	
	set_way_point_type(1525, 1)
	add_way_point(1525,221064,72725)
	add_way_point(1525,221274,72353)
	add_way_point(1525,221958,72504)
	add_way_point(1525,221305,72520)
	add_way_point(1525,221209,72540)
	add_way_point(1525,221076,72720)
	add_way_point(1525,221063,72726)
	
	
	set_way_point_type(1526, 1)
	add_way_point(1526,223118,72974)
	add_way_point(1526,223558,73157)
	add_way_point(1526,223742,73253)
	add_way_point(1526,223773,73343)
	add_way_point(1526,223669,73464)
	add_way_point(1526,223563,73557)
	add_way_point(1526,223504,73636)
	add_way_point(1526,223411,73753)
	add_way_point(1526,223345,73865)
	add_way_point(1526,223363,73926)
	add_way_point(1526,223421,73985)
	add_way_point(1526,223487,73990)
	add_way_point(1526,223580,73994)
	add_way_point(1526,223654,74024)
	add_way_point(1526,223680,74068)
	add_way_point(1526,223695,74141)
	add_way_point(1526,223705,74203)
	add_way_point(1526,223700,74290)
	add_way_point(1526,223681,74344)
	add_way_point(1526,223675,74407)
	
	
	set_way_point_type(1527, 1)
	add_way_point(1527,220632,76384)
	add_way_point(1527,221167,76383)
	
	
	set_way_point_type(1528, 1)
	add_way_point(1528,221168,76370)
	add_way_point(1528,220632,76371)
	
	
	respawn_rare_mob(-1,50000,218479,76916,9047008,1,0,1501)
	respawn_rare_mob(-1,50000,217867,76112,9047002,1,0,1502)
	respawn_rare_mob(-1,50000,220862,79673,9047007,1,0,1503)
	respawn_rare_mob(-1,50000,222763,77960,9045008,1,0,1504)
	respawn_rare_mob(-1,50000,221269,73920,9045007,1,0,1505)
	respawn_rare_mob(-1,50000,223706,74593,9036010,1,0,1506)
	respawn_rare_mob(-1,50000,218563,72437,9039008,1,0,1507)
	respawn_rare_mob(-1,50000,217983,74171,9045006,1,0,1508)
	respawn_rare_mob(-1,50000,219197,73260,9040011,1,0,1509)
	respawn_rare_mob(-1,50000,220120,73633,9041017,1,0,1510)
	respawn_rare_mob(-1,50000,223033,76643,9042005,1,0,1511)
	respawn_rare_mob(-1,50000,223452,77610,9039012,1,0,1512)
	respawn_rare_mob(-1,50000,216914,76048,9042002,1,0,1513)
	respawn_rare_mob(-1,50000,217818,75896,9046002,1,0,1514)
	respawn_rare_mob(-1,50000,219867,76167,9049002,1,0,1515)
	respawn_rare_mob(-1,50000,219896,76165,9049009,1,0,1516)
	respawn_rare_mob(-1,50000,218523,77169,9050012,1,0,1517)
	respawn_rare_mob(-1,50000,220932,75498,9049013,1,0,1518)
	respawn_rare_mob(-1,50000,220946,75581,9049012,1,0,1519)
	respawn_rare_mob(-1,50000,220639,75392,9049012,1,0,1520)
	respawn_rare_mob(-1,50000,220305,74547,9049012,1,0,1521)
	respawn_rare_mob(-1,50000,220146,73650,9049012,1,0,1522)
	respawn_rare_mob(-1,50000,220110,78803,9049012,1,0,1523)
	respawn_rare_mob(-1,50000,219103,75599,9044018,1,0,1524)
	respawn_rare_mob(-1,50000,221064,72725,9034002,1,0,1525)
	respawn_rare_mob(-1,50000,223118,72974,9034003,1,0,1526)
	respawn_rare_mob(-1,50000,220632,76384,9050012,1,0,1527)
	respawn_rare_mob(-1,50000,221168,76370,9050012,1,0,1528)

	

			--- 수정 계곡 2 ---
		
	set_way_point_type( 2001, 1 )
	add_way_point( 2001, 219000, 90142 )
	add_way_point( 2001, 218834, 90438 )
	add_way_point( 2001, 218877, 90616 )
	add_way_point( 2001, 218987, 90789 )
	add_way_point( 2001, 219000, 90952 )
	add_way_point( 2001, 218973, 91117 )
	add_way_point( 2001, 218915, 91227 )
	add_way_point( 2001, 218873, 91340 )
	add_way_point( 2001, 218726, 91377 )
	add_way_point( 2001, 218568, 91392 )
	add_way_point( 2001, 218467, 91543 )
	add_way_point( 2001, 218515, 91663 )
	add_way_point( 2001, 218695, 91744 )
	add_way_point( 2001, 218842, 91691 )
	add_way_point( 2001, 218881, 91550 )
	add_way_point( 2001, 218881, 91407 )
	add_way_point( 2001, 218873, 91348 )
	
	set_way_point_type( 2002, 1 )
	add_way_point( 2002, 218668, 91401 )
	add_way_point( 2002, 218566, 91445 )
	add_way_point( 2002, 218504, 91551 )
	add_way_point( 2002, 218537, 91639 )
	add_way_point( 2002, 218678, 91702 )
	add_way_point( 2002, 218792, 91669 )
	add_way_point( 2002, 218833, 91580 )
	add_way_point( 2002, 218838, 91483 )
	add_way_point( 2002, 218768, 91426 )
	add_way_point( 2002, 218693, 91420 )
	add_way_point( 2002, 218639, 91429 )
	add_way_point( 2002, 218568, 91474 )
	add_way_point( 2002, 218540, 91571 )
	add_way_point( 2002, 218578, 91641 )
	add_way_point( 2002, 218651, 91669 )
	add_way_point( 2002, 218699, 91786 )
	add_way_point( 2002, 218707, 91875 )
	add_way_point( 2002, 218686, 91951 )
	add_way_point( 2002, 218660, 92014 )
	add_way_point( 2002, 218652, 92087 )
	add_way_point( 2002, 218646, 92178 )
	
	set_way_point_type( 2003, 1 )
	add_way_point( 2003, 218827, 91681 )
	add_way_point( 2003, 218630, 91664 )
	add_way_point( 2003, 218533, 91583 )
	add_way_point( 2003, 218537, 91499 )
	add_way_point( 2003, 218626, 91409 )
	add_way_point( 2003, 218733, 91404 )
	add_way_point( 2003, 218849, 91485 )
	add_way_point( 2003, 218838, 91569 )
	add_way_point( 2003, 218829, 91644 )
	add_way_point( 2003, 218832, 91673 )
	add_way_point( 2003, 218875, 91728 )
	add_way_point( 2003, 219030, 91838 )
	add_way_point( 2003, 219089, 91913 )
	add_way_point( 2003, 219175, 92016 )
	add_way_point( 2003, 219344, 92112 )
	add_way_point( 2003, 219439, 92169 )
	add_way_point( 2003, 219505, 92207 )
	add_way_point( 2003, 219594, 92306 )
	add_way_point( 2003, 219703, 92400 )
	add_way_point( 2003, 219721, 92489 )
	add_way_point( 2003, 219668, 92605 )
	add_way_point( 2003, 219617, 92703 )
	add_way_point( 2003, 219599, 92826 )
	add_way_point( 2003, 219575, 92959 )
	add_way_point( 2003, 219592, 93057 )
	add_way_point( 2003, 219659, 93220 )
	add_way_point( 2003, 219721, 93324 )
	add_way_point( 2003, 219789, 93442 )
	add_way_point( 2003, 219879, 93581 )
	add_way_point( 2003, 219935, 93685 )
	add_way_point( 2003, 219985, 93784 )
	add_way_point( 2003, 220083, 93905 )
	add_way_point( 2003, 220176, 93848 )
	add_way_point( 2003, 220280, 93762 )
	add_way_point( 2003, 220381, 93683 )
	add_way_point( 2003, 220462, 93601 )
	add_way_point( 2003, 220623, 93573 )
	add_way_point( 2003, 220783, 93447 )
	add_way_point( 2003, 220930, 93272 )
	add_way_point( 2003, 220971, 93164 )
	add_way_point( 2003, 221014, 93041 )
	add_way_point( 2003, 221035, 92925 )
	add_way_point( 2003, 220962, 92829 )
	add_way_point( 2003, 220839, 92772 )
	add_way_point( 2003, 220590, 92747 )
	add_way_point( 2003, 220367, 92594 )
	add_way_point( 2003, 220311, 92435 )
	add_way_point( 2003, 220278, 92268 )
	
	set_way_point_type( 2004, 1 )
	add_way_point( 2004, 220286, 95568 )
	add_way_point( 2004, 220384, 95833 )
	add_way_point( 2004, 220545, 95824 )
	add_way_point( 2004, 220701, 95681 )
	add_way_point( 2004, 220764, 95496 )
	add_way_point( 2004, 220686, 95347 )
	add_way_point( 2004, 220568, 95209 )
	add_way_point( 2004, 220450, 95189 )
	add_way_point( 2004, 220350, 95291 )
	add_way_point( 2004, 220298, 95432 )
	add_way_point( 2004, 220289, 95553 )
	
	set_way_point_type( 2005, 1 )
	add_way_point( 2005, 220357, 95696 )
	add_way_point( 2005, 220647, 95704 )
	add_way_point( 2005, 220671, 95380 )
	add_way_point( 2005, 220391, 95368 )
	add_way_point( 2005, 220364, 95695 )
	add_way_point( 2005, 220268, 95531 )
	add_way_point( 2005, 220112, 95397 )
	add_way_point( 2005, 219986, 95420 )
	add_way_point( 2005, 219850, 95384 )
	add_way_point( 2005, 219678, 95351 )
	add_way_point( 2005, 219510, 95298 )
	add_way_point( 2005, 219351, 95261 )
	add_way_point( 2005, 219220, 95068 )
	add_way_point( 2005, 218922, 94691 )
	add_way_point( 2005, 218741, 94371 )
	add_way_point( 2005, 218675, 94223 )
	add_way_point( 2005, 218756, 94109 )
	add_way_point( 2005, 218721, 93960 )
	add_way_point( 2005, 218687, 93871 )
	add_way_point( 2005, 218735, 93753 )
	add_way_point( 2005, 218774, 93571 )
	add_way_point( 2005, 218702, 93381 )
	add_way_point( 2005, 218743, 93003 )
	add_way_point( 2005, 218753, 92802 )
	add_way_point( 2005, 218752, 92778 )
	
	set_way_point_type( 2006, 1 )
	add_way_point( 2006, 221303, 96354 )
	add_way_point( 2006, 220824, 96142 )
	add_way_point( 2006, 220676, 96018 )
	add_way_point( 2006, 220454, 95854 )
	add_way_point( 2006, 220303, 95508 )
	add_way_point( 2006, 220330, 95235 )
	add_way_point( 2006, 220491, 95147 )
	add_way_point( 2006, 220692, 95244 )
	add_way_point( 2006, 220815, 95458 )
	add_way_point( 2006, 220798, 95554 )
	add_way_point( 2006, 220696, 95683 )
	add_way_point( 2006, 220535, 95829 )
	add_way_point( 2006, 220484, 95872 )
	
	set_way_point_type( 2007, 1 )
	add_way_point( 2007, 221268, 94451 )
	add_way_point( 2007, 221498, 94612 )
	add_way_point( 2007, 222012, 94814 )
	add_way_point( 2007, 222600, 94869 )
	add_way_point( 2007, 222867, 94834 )
	add_way_point( 2007, 222901, 94590 )
	add_way_point( 2007, 222868, 94435 )
	add_way_point( 2007, 222670, 94339 )
	add_way_point( 2007, 222354, 94282 )
	add_way_point( 2007, 222127, 94197 )
	add_way_point( 2007, 221839, 94161 )
	add_way_point( 2007, 221527, 94176 )
	add_way_point( 2007, 221391, 94242 )
	add_way_point( 2007, 221288, 94362 )
	add_way_point( 2007, 221265, 94442 )
	
	set_way_point_type( 2008, 1 )
	add_way_point( 2008, 221826, 93276 )
	add_way_point( 2008, 221944, 93329 )
	add_way_point( 2008, 221775, 93560 )
	add_way_point( 2008, 221697, 93653 )
	add_way_point( 2008, 221593, 93739 )
	add_way_point( 2008, 221574, 93929 )
	add_way_point( 2008, 221524, 94100 )
	add_way_point( 2008, 221309, 94259 )
	add_way_point( 2008, 221259, 94470 )
	add_way_point( 2008, 221395, 94608 )
	add_way_point( 2008, 221594, 94731 )
	add_way_point( 2008, 221864, 94791 )
	add_way_point( 2008, 222291, 94868 )
	add_way_point( 2008, 222595, 94911 )
	add_way_point( 2008, 222850, 94863 )
	add_way_point( 2008, 222910, 94695 )
	add_way_point( 2008, 222872, 94488 )
	add_way_point( 2008, 222861, 94400 )
	add_way_point( 2008, 222683, 94264 )
	add_way_point( 2008, 222663, 94129 )
	add_way_point( 2008, 222680, 94018 )
	add_way_point( 2008, 222708, 93804 )
	add_way_point( 2008, 222758, 93630 )
	add_way_point( 2008, 222768, 93437 )
	add_way_point( 2008, 222762, 93325 )
	add_way_point( 2008, 222751, 93243 )
	add_way_point( 2008, 222975, 93223 )
	
	set_way_point_type( 2009, 1 )
	add_way_point( 2009, 223831, 92530 )
	add_way_point( 2009, 223937, 92364 )
	add_way_point( 2009, 224060, 92209 )
	add_way_point( 2009, 224176, 92021 )
	add_way_point( 2009, 224263, 91877 )
	add_way_point( 2009, 224372, 91713 )
	add_way_point( 2009, 224520, 91531 )
	add_way_point( 2009, 224675, 91447 )
	add_way_point( 2009, 224817, 91423 )
	add_way_point( 2009, 224925, 91479 )
	add_way_point( 2009, 224870, 91568 )
	add_way_point( 2009, 224789, 91653 )
	add_way_point( 2009, 224785, 91777 )
	add_way_point( 2009, 224790, 91903 )
	add_way_point( 2009, 224857, 91987 )
	add_way_point( 2009, 224850, 92091 )
	add_way_point( 2009, 224894, 92187 )
	add_way_point( 2009, 224963, 92323 )
	add_way_point( 2009, 225009, 92537 )
	add_way_point( 2009, 225049, 92688 )
	add_way_point( 2009, 225034, 92914 )
	add_way_point( 2009, 224975, 93140 )
	add_way_point( 2009, 224877, 93365 )
	add_way_point( 2009, 224785, 93431 )
	add_way_point( 2009, 224658, 93559 )
	add_way_point( 2009, 224726, 93752 )
	add_way_point( 2009, 224885, 93824 )
	add_way_point( 2009, 225011, 93727 )
	add_way_point( 2009, 225051, 93548 )
	add_way_point( 2009, 224975, 93485 )
	add_way_point( 2009, 224873, 93365 )
	
	set_way_point_type( 2010, 1 )
	add_way_point( 2010, 224753, 93725 )
	add_way_point( 2010, 224919, 93734 )
	add_way_point( 2010, 224962, 93538 )
	add_way_point( 2010, 224810, 93486 )
	add_way_point( 2010, 224711, 93545 )
	add_way_point( 2010, 224743, 93726 )
	add_way_point( 2010, 224543, 93847 )
	add_way_point( 2010, 224406, 93984 )
	add_way_point( 2010, 224382, 94179 )
	add_way_point( 2010, 224195, 94361 )
	add_way_point( 2010, 224031, 94335 )
	add_way_point( 2010, 223886, 94402 )
	add_way_point( 2010, 223865, 94544 )
	add_way_point( 2010, 223975, 94582 )
	add_way_point( 2010, 224075, 94472 )
	add_way_point( 2010, 224176, 94466 )
	add_way_point( 2010, 224200, 94584 )
	add_way_point( 2010, 224162, 94696 )
	add_way_point( 2010, 224126, 94795 )
	add_way_point( 2010, 224024, 94903 )
	add_way_point( 2010, 223961, 95081 )
	add_way_point( 2010, 223881, 95254 )
	add_way_point( 2010, 223761, 95401 )
	add_way_point( 2010, 223668, 95527 )
	add_way_point( 2010, 223605, 95762 )
	add_way_point( 2010, 223609, 95855 )
	add_way_point( 2010, 223833, 95994 )
	add_way_point( 2010, 224000, 96053 )
	add_way_point( 2010, 224259, 96171 )
	add_way_point( 2010, 224442, 96215 )
	
	set_way_point_type( 2011, 1 )
	add_way_point( 2011, 225238, 95651 )
	add_way_point( 2011, 225424, 95612 )
	add_way_point( 2011, 225385, 95415 )
	add_way_point( 2011, 225318, 95260 )
	add_way_point( 2011, 225190, 95145 )
	add_way_point( 2011, 225042, 94968 )
	add_way_point( 2011, 224948, 94809 )
	add_way_point( 2011, 224904, 94700 )
	add_way_point( 2011, 224936, 94483 )
	add_way_point( 2011, 224934, 94233 )
	add_way_point( 2011, 224980, 94101 )
	add_way_point( 2011, 224987, 93904 )
	add_way_point( 2011, 225023, 93600 )
	add_way_point( 2011, 224918, 93443 )
	add_way_point( 2011, 224966, 93149 )
	add_way_point( 2011, 224997, 92942 )
	add_way_point( 2011, 225066, 92728 )
	add_way_point( 2011, 225120, 92607 )
	add_way_point( 2011, 225192, 92529 )
	add_way_point( 2011, 225221, 92455 )
	
	set_way_point_type( 2012, 1 )
	add_way_point( 2012, 224696, 91583 )
	add_way_point( 2012, 224783, 91396 )
	add_way_point( 2012, 224865, 91238 )
	add_way_point( 2012, 224947, 91079 )
	add_way_point( 2012, 225009, 90939 )
	add_way_point( 2012, 225096, 90848 )
	add_way_point( 2012, 225159, 90727 )
	add_way_point( 2012, 225242, 90551 )
	add_way_point( 2012, 225214, 90430 )
	add_way_point( 2012, 225188, 90321 )
	add_way_point( 2012, 225143, 90144 )
	add_way_point( 2012, 225128, 90016 )
	add_way_point( 2012, 224787, 90049 )
	add_way_point( 2012, 224460, 90019 )
	
	set_way_point_type( 2013, 1 )
	add_way_point( 2013, 222104, 89257 )
	add_way_point( 2013, 222091, 88865 )
	add_way_point( 2013, 222773, 88901 )
	add_way_point( 2013, 223079, 89303 )
	add_way_point( 2013, 223419, 89430 )
	add_way_point( 2013, 223677, 89434 )
	add_way_point( 2013, 223872, 89217 )
	add_way_point( 2013, 224075, 89122 )
	add_way_point( 2013, 224362, 88920 )
	add_way_point( 2013, 224618, 88884 )
	add_way_point( 2013, 224892, 88909 )
	add_way_point( 2013, 225025, 89080 )
	add_way_point( 2013, 225186, 89360 )
	add_way_point( 2013, 225263, 89523 )
	add_way_point( 2013, 225173, 89790 )
	add_way_point( 2013, 225203, 90102 )
	
	set_way_point_type( 2014, 1 )
	add_way_point( 2014, 222483, 91722 )
	add_way_point( 2014, 222392, 91897 )
	add_way_point( 2014, 222540, 92026 )
	add_way_point( 2014, 222685, 91925 )
	add_way_point( 2014, 222746, 91773 )
	add_way_point( 2014, 222487, 91722 )
	add_way_point( 2014, 222568, 91594 )
	add_way_point( 2014, 222618, 91444 )
	add_way_point( 2014, 222698, 91239 )
	add_way_point( 2014, 222777, 91109 )
	add_way_point( 2014, 222824, 90977 )
	add_way_point( 2014, 222829, 90817 )
	add_way_point( 2014, 222806, 90615 )
	add_way_point( 2014, 222810, 90481 )
	add_way_point( 2014, 222769, 90341 )
	add_way_point( 2014, 222689, 90210 )
	
	set_way_point_type( 2015, 1 )
	add_way_point( 2015, 222691, 90361 )
	add_way_point( 2015, 222900, 90415 )
	add_way_point( 2015, 222944, 90334 )
	add_way_point( 2015, 222880, 90212 )
	add_way_point( 2015, 222733, 90141 )
	add_way_point( 2015, 222590, 90176 )
	add_way_point( 2015, 222495, 90281 )
	add_way_point( 2015, 222428, 90386 )
	add_way_point( 2015, 222295, 90550 )
	add_way_point( 2015, 222236, 90710 )
	add_way_point( 2015, 222212, 90846 )
	add_way_point( 2015, 222171, 90973 )
	add_way_point( 2015, 222112, 91058 )
	add_way_point( 2015, 222058, 91129 )
	add_way_point( 2015, 221934, 91165 )
	add_way_point( 2015, 221716, 91165 )
	add_way_point( 2015, 221593, 91098 )
	add_way_point( 2015, 221521, 90968 )
	add_way_point( 2015, 221545, 90851 )
	add_way_point( 2015, 221565, 90706 )
	add_way_point( 2015, 221566, 90589 )
	add_way_point( 2015, 221602, 90458 )
	add_way_point( 2015, 221640, 90327 )
	add_way_point( 2015, 221555, 90181 )
	add_way_point( 2015, 221524, 90101 )
	add_way_point( 2015, 221642, 90058 )
	add_way_point( 2015, 221716, 90115 )
	add_way_point( 2015, 221682, 90245 )
	
	set_way_point_type( 2016, 1 )
	add_way_point( 2016, 221150, 92165 )
	add_way_point( 2016, 221210, 92233 )
	add_way_point( 2016, 221236, 92327 )
	add_way_point( 2016, 221297, 92393 )
	add_way_point( 2016, 221431, 92422 )
	add_way_point( 2016, 221547, 92416 )
	add_way_point( 2016, 221593, 92368 )
	add_way_point( 2016, 221576, 92240 )
	add_way_point( 2016, 221520, 92166 )
	add_way_point( 2016, 221383, 92148 )
	add_way_point( 2016, 221234, 92170 )
	add_way_point( 2016, 221162, 92170 )
	
	set_way_point_type( 2017, 1 )
	add_way_point( 2017, 221399, 92286 )
	add_way_point( 2017, 221368, 92118 )
	add_way_point( 2017, 221368, 91921 )
	add_way_point( 2017, 221396, 91784 )
	add_way_point( 2017, 221413, 91596 )
	add_way_point( 2017, 221407, 91400 )
	add_way_point( 2017, 221409, 91275 )
	add_way_point( 2017, 221436, 91137 )
	add_way_point( 2017, 221486, 90940 )
	add_way_point( 2017, 221540, 90604 )
	add_way_point( 2017, 221586, 90479 )
	add_way_point( 2017, 221611, 90361 )
	add_way_point( 2017, 221632, 90189 )
	
	set_way_point_type( 2018, 1 )
	add_way_point( 2018, 220353, 90775 )
	add_way_point( 2018, 220208, 90801 )
	add_way_point( 2018, 220182, 90628 )
	add_way_point( 2018, 220273, 90534 )
	add_way_point( 2018, 220374, 90616 )
	add_way_point( 2018, 220361, 90775 )
	
	set_way_point_type( 2019, 1 )
	add_way_point( 2019, 220285, 90675 )
	add_way_point( 2019, 220419, 90606 )
	add_way_point( 2019, 220552, 90565 )
	add_way_point( 2019, 220723, 90552 )
	add_way_point( 2019, 220928, 90581 )
	add_way_point( 2019, 221088, 90693 )
	add_way_point( 2019, 221259, 90835 )
	add_way_point( 2019, 221294, 91058 )
	add_way_point( 2019, 221341, 91169 )
	add_way_point( 2019, 221575, 91158 )
	add_way_point( 2019, 221565, 90810 )
	add_way_point( 2019, 221360, 90837 )
	add_way_point( 2019, 221343, 91167 )
	
	set_way_point_type( 2020, 1 )
	add_way_point( 2020, 220117, 94152 )
	add_way_point( 2020, 219979, 93926 )
	add_way_point( 2020, 220230, 93955 )
	add_way_point( 2020, 220114, 94140 )
	add_way_point( 2020, 220373, 94084 )
	add_way_point( 2020, 220651, 94103 )
	add_way_point( 2020, 220874, 94195 )
	add_way_point( 2020, 221156, 94280 )
	add_way_point( 2020, 221631, 94249 )
	add_way_point( 2020, 221829, 94211 )
	add_way_point( 2020, 221987, 94186 )
	add_way_point( 2020, 222158, 94222 )
	add_way_point( 2020, 222290, 94288 )
	add_way_point( 2020, 222321, 94396 )
	add_way_point( 2020, 222468, 94469 )
	add_way_point( 2020, 222671, 94465 )
	add_way_point( 2020, 222829, 94487 )
	
	set_way_point_type( 2021, 1 )
	add_way_point( 2021, 220635, 95524 )
	add_way_point( 2021, 220838, 95545 )
	add_way_point( 2021, 221038, 95571 )
	add_way_point( 2021, 221248, 95586 )
	add_way_point( 2021, 221355, 95624 )
	add_way_point( 2021, 221457, 95585 )
	add_way_point( 2021, 221556, 95587 )
	add_way_point( 2021, 221640, 95650 )
	add_way_point( 2021, 221851, 95727 )
	add_way_point( 2021, 222013, 95853 )
	add_way_point( 2021, 222099, 95992 )
	add_way_point( 2021, 222157, 96141 )
	add_way_point( 2021, 222305, 96200 )
	add_way_point( 2021, 222453, 96153 )
	add_way_point( 2021, 222627, 96032 )
	add_way_point( 2021, 222873, 95964 )
	add_way_point( 2021, 222976, 95940 )
	add_way_point( 2021, 223137, 95849 )
	add_way_point( 2021, 223246, 95830 )
	add_way_point( 2021, 223438, 95817 )
	add_way_point( 2021, 223605, 95829 )
	add_way_point( 2021, 223976, 96001 )
	add_way_point( 2021, 224468, 96169 )
	add_way_point( 2021, 224603, 96204 )
	add_way_point( 2021, 224388, 96324 )
	
	set_way_point_type( 2022, 1 )
	add_way_point( 2022, 223675, 92608 )
	add_way_point( 2022, 223966, 92523 )
	add_way_point( 2022, 223958, 92304 )
	add_way_point( 2022, 224207, 91987 )
	add_way_point( 2022, 224429, 91648 )
	add_way_point( 2022, 224551, 91574 )
	add_way_point( 2022, 224709, 91616 )
	add_way_point( 2022, 224879, 91599 )
	add_way_point( 2022, 224937, 91534 )
	add_way_point( 2022, 224869, 91405 )
	add_way_point( 2022, 224818, 91265 )
	add_way_point( 2022, 224942, 91068 )
	add_way_point( 2022, 225060, 90872 )
	add_way_point( 2022, 225214, 90544 )
	add_way_point( 2022, 225178, 90203 )
	add_way_point( 2022, 225133, 89993 )
	add_way_point( 2022, 224442, 89979 )
	add_way_point( 2022, 224346, 89927 )
	add_way_point( 2022, 224284, 89788 )
	add_way_point( 2022, 224302, 89702 )
	add_way_point( 2022, 224226, 89555 )
	add_way_point( 2022, 224218, 89379 )
	add_way_point( 2022, 224222, 89241 )
	add_way_point( 2022, 224295, 89100 )
	add_way_point( 2022, 224466, 89015 )
	add_way_point( 2022, 224706, 88974 )
	add_way_point( 2022, 224889, 89056 )
	add_way_point( 2022, 225001, 89124 )
	add_way_point( 2022, 225064, 89248 )
	add_way_point( 2022, 225162, 89405 )
	add_way_point( 2022, 225165, 89521 )
	add_way_point( 2022, 225136, 89658 )
	add_way_point( 2022, 225149, 89845 )
	add_way_point( 2022, 225140, 89983 )
	
	set_way_point_type( 2023, 1 )
	add_way_point( 2023, 222784, 91845 )
	add_way_point( 2023, 222507, 92021 )
	add_way_point( 2023, 222433, 91680 )
	add_way_point( 2023, 222821, 91805 )
	add_way_point( 2023, 223080, 91851 )
	add_way_point( 2023, 223261, 91855 )
	add_way_point( 2023, 223352, 91782 )
	add_way_point( 2023, 223447, 91647 )
	add_way_point( 2023, 223489, 91502 )
	add_way_point( 2023, 223444, 91394 )
	add_way_point( 2023, 223378, 91233 )
	add_way_point( 2023, 223354, 91113 )
	add_way_point( 2023, 223404, 90995 )
	add_way_point( 2023, 223495, 90953 )
	add_way_point( 2023, 223617, 90988 )
	add_way_point( 2023, 223715, 91037 )
	add_way_point( 2023, 223859, 90988 )
	add_way_point( 2023, 223988, 90909 )
	add_way_point( 2023, 224027, 90821 )
	add_way_point( 2023, 224004, 90719 )
	add_way_point( 2023, 223933, 90626 )
	add_way_point( 2023, 223876, 90538 )
	add_way_point( 2023, 223851, 90390 )
	add_way_point( 2023, 223908, 90272 )
	add_way_point( 2023, 223981, 90165 )
	add_way_point( 2023, 224084, 90049 )
	add_way_point( 2023, 224164, 89917 )
	add_way_point( 2023, 224285, 89829 )
	add_way_point( 2023, 224354, 89775 )
	
	set_way_point_type( 2024, 1 )
	add_way_point( 2024, 220636, 92676 )
	add_way_point( 2024, 220388, 92822 )
	add_way_point( 2024, 220303, 92579 )
	add_way_point( 2024, 220668, 92674 )
	add_way_point( 2024, 220327, 92804 )
	add_way_point( 2024, 220261, 92537 )
	add_way_point( 2024, 220259, 92328 )
	add_way_point( 2024, 220226, 92142 )
	add_way_point( 2024, 220243, 91977 )
	add_way_point( 2024, 220248, 91724 )
	add_way_point( 2024, 220363, 91636 )
	add_way_point( 2024, 220496, 91606 )
	add_way_point( 2024, 220621, 91556 )
	add_way_point( 2024, 220652, 91454 )
	add_way_point( 2024, 220736, 91408 )
	add_way_point( 2024, 220849, 91464 )
	add_way_point( 2024, 220878, 91546 )
	add_way_point( 2024, 220830, 91643 )
	add_way_point( 2024, 220790, 91752 )
	add_way_point( 2024, 220811, 91902 )
	add_way_point( 2024, 220827, 92030 )
	add_way_point( 2024, 220874, 92090 )
	add_way_point( 2024, 220978, 92123 )
	add_way_point( 2024, 221112, 92128 )
	add_way_point( 2024, 221152, 92202 )
	add_way_point( 2024, 221319, 92307 )
	add_way_point( 2024, 221404, 92312 )
	
	set_way_point_type( 2025, 1 )
	add_way_point( 2025, 223864, 89597 )
	add_way_point( 2025, 223806, 89499 )
	add_way_point( 2025, 223834, 89346 )
	add_way_point( 2025, 224004, 89536 )
	add_way_point( 2025, 223990, 89273 )
	add_way_point( 2025, 224207, 89389 )
	add_way_point( 2025, 224160, 89105 )
	add_way_point( 2025, 224377, 89096 )
	add_way_point( 2025, 224417, 88880 )
	add_way_point( 2025, 224620, 89075 )
	add_way_point( 2025, 224835, 88869 )
	add_way_point( 2025, 224905, 89144 )
	add_way_point( 2025, 225135, 89220 )
	add_way_point( 2025, 225132, 89385 )
	add_way_point( 2025, 225269, 89503 )
	add_way_point( 2025, 225121, 89646 )
	add_way_point( 2025, 225083, 89839 )
	add_way_point( 2025, 224831, 89900 )
	add_way_point( 2025, 224818, 90034 )
	add_way_point( 2025, 224616, 89950 )
	add_way_point( 2025, 224529, 90031 )
	add_way_point( 2025, 224444, 89823 )
	add_way_point( 2025, 224254, 89768 )
	add_way_point( 2025, 224295, 89563 )
	add_way_point( 2025, 224163, 89439 )
	add_way_point( 2025, 224101, 89420 )
	add_way_point( 2025, 223980, 89520 )
	add_way_point( 2025, 223862, 89588 )
	
	set_way_point_type( 2026, 1 )
	add_way_point( 2026, 219693, 93199 )
	add_way_point( 2026, 219615, 93276 )
	add_way_point( 2026, 219661, 93469 )
	add_way_point( 2026, 219722, 93510 )
	add_way_point( 2026, 219791, 93471 )
	add_way_point( 2026, 219885, 93439 )
	add_way_point( 2026, 219891, 93396 )
	add_way_point( 2026, 219826, 93330 )
	add_way_point( 2026, 219755, 93279 )
	add_way_point( 2026, 219709, 93197 )
	add_way_point( 2026, 219702, 93192 )
	
	set_way_point_type( 2027, 1 )
	add_way_point( 2027, 218500, 94410 )
	add_way_point( 2027, 218859, 94368 )
	add_way_point( 2027, 218843, 94281 )
	add_way_point( 2027, 218837, 94155 )
	add_way_point( 2027, 218730, 94113 )
	add_way_point( 2027, 218583, 94105 )
	add_way_point( 2027, 218533, 94151 )
	add_way_point( 2027, 218481, 94257 )
	add_way_point( 2027, 218500, 94410 )
	
	set_way_point_type( 2028, 1 )
	add_way_point( 2028, 221596, 94525 )
	add_way_point( 2028, 221492, 94781 )
	add_way_point( 2028, 221750, 94880 )
	add_way_point( 2028, 221945, 94884 )
	add_way_point( 2028, 222101, 94909 )
	add_way_point( 2028, 222242, 94961 )
	add_way_point( 2028, 222354, 94972 )
	add_way_point( 2028, 222534, 94977 )
	add_way_point( 2028, 222702, 94974 )
	add_way_point( 2028, 222854, 94967 )
	add_way_point( 2028, 223035, 94933 )
	add_way_point( 2028, 223042, 94852 )
	add_way_point( 2028, 222975, 94815 )
	add_way_point( 2028, 222747, 94818 )
	add_way_point( 2028, 222559, 94801 )
	add_way_point( 2028, 222299, 94814 )
	add_way_point( 2028, 222095, 94785 )
	add_way_point( 2028, 222040, 94712 )
	add_way_point( 2028, 222003, 94622 )
	add_way_point( 2028, 221914, 94669 )
	add_way_point( 2028, 221829, 94695 )
	add_way_point( 2028, 221714, 94657 )
	add_way_point( 2028, 221660, 94582 )
	add_way_point( 2028, 221603, 94530 )
	
	set_way_point_type( 2029, 1 )
	add_way_point( 2029, 223838, 94637 )
	add_way_point( 2029, 224152, 94707 )
	add_way_point( 2029, 224248, 94300 )
	add_way_point( 2029, 223877, 94300 )
	add_way_point( 2029, 223776, 94431 )
	add_way_point( 2029, 223834, 94640 )
	
	set_way_point_type( 2030, 1 )
	add_way_point( 2030, 225481, 95473 )
	add_way_point( 2030, 225220, 95624 )
	add_way_point( 2030, 225239, 95712 )
	add_way_point( 2030, 225350, 95716 )
	add_way_point( 2030, 225438, 95626 )
	add_way_point( 2030, 225414, 95381 )
	add_way_point( 2030, 225204, 95140 )
	add_way_point( 2030, 225077, 95005 )
	add_way_point( 2030, 224987, 94863 )
	add_way_point( 2030, 224909, 94696 )
	add_way_point( 2030, 224891, 94546 )
	add_way_point( 2030, 224939, 94369 )
	add_way_point( 2030, 224940, 94173 )
	add_way_point( 2030, 224983, 94006 )
	add_way_point( 2030, 224976, 93843 )
	add_way_point( 2030, 224803, 93805 )
	add_way_point( 2030, 224646, 93791 )
	add_way_point( 2030, 224520, 93863 )
	add_way_point( 2030, 224402, 94024 )
	add_way_point( 2030, 224363, 94167 )
	add_way_point( 2030, 224199, 94319 )
	add_way_point( 2030, 224066, 94475 )
	add_way_point( 2030, 224083, 94684 )
	add_way_point( 2030, 224085, 94854 )
	add_way_point( 2030, 224001, 94990 )
	add_way_point( 2030, 223871, 95262 )
	add_way_point( 2030, 223692, 95501 )
	add_way_point( 2030, 223607, 95823 )
	add_way_point( 2030, 223257, 95835 )
	add_way_point( 2030, 222865, 95992 )
	add_way_point( 2030, 222462, 96129 )
	add_way_point( 2030, 222244, 96211 )
	add_way_point( 2030, 222167, 96092 )
	add_way_point( 2030, 222014, 95875 )
	add_way_point( 2030, 221814, 95721 )
	add_way_point( 2030, 221627, 95659 )
	add_way_point( 2030, 221550, 95589 )
	add_way_point( 2030, 221410, 95607 )
	add_way_point( 2030, 221330, 95645 )
	add_way_point( 2030, 221240, 95599 )
	add_way_point( 2030, 221068, 95576 )
	add_way_point( 2030, 220885, 95542 )
	add_way_point( 2030, 220629, 95523 )

	respawn_rare_mob(-1,30000,219000,90142,9070015,1,0,2001)
	respawn_rare_mob(-1,30000,218668,91401,9071004,1,0,2002)
	respawn_rare_mob(-1,30000,218827,91681,9072003,1,0,2003)
	respawn_rare_mob(-1,30000,220286,95568,9074008,1,0,2004)
	respawn_rare_mob(-1,30000,220357,95696,9074009,1,0,2005)
	respawn_rare_mob(-1,30000,221303,96354,9075007,1,0,2006)
	respawn_rare_mob(-1,30000,221268,94451,9075008,1,0,2007)
	respawn_rare_mob(-1,30000,221826,93276,9075009,1,0,2008)
	respawn_rare_mob(-1,30000,223831,92530,9076001,1,0,2009)
	respawn_rare_mob(-1,30000,224753,93725,9076007,1,0,2010)
	respawn_rare_mob(-1,30000,225238,95651,9078011,1,0,2011)
	respawn_rare_mob(-1,30000,224696,91583,9080007,1,0,2012)
	respawn_rare_mob(-1,30000,222104,89257,9090017,1,0,2013)
	respawn_rare_mob(-1,30000,222483,91722,9089014,1,0,2014)
	respawn_rare_mob(-1,30000,222691,90361,9090007,1,0,2015)
	respawn_rare_mob(-1,30000,221150,92165,9089005,1,0,2016)
	respawn_rare_mob(-1,30000,221399,92286,9088013,1,0,2017)
	respawn_rare_mob(-1,30000,220353,90775,9088014,1,0,2018)
	respawn_rare_mob(-1,30000,220285,90675,9088015,1,0,2019)
	respawn_rare_mob(-1,30000,220117,94152,9087014,1,0,2020)
	respawn_rare_mob(-1,30000,220635,95524,9088001,1,0,2021)
	respawn_rare_mob(-1,30000,223675,92608,9088002,1,0,2022)
	respawn_rare_mob(-1,30000,222784,91845,9086007,1,0,2023)
	respawn_rare_mob(-1,30000,220636,92676,9086008,1,0,2024)
	respawn_rare_mob(-1,30000,223864,89597,9086009,1,0,2025)
	respawn_rare_mob(-1,30000,219693,93199,9086010,1,0,2026)
	respawn_rare_mob(-1,30000,218500,94410,9084004,1,0,2027)
	respawn_rare_mob(-1,30000,221596,94525,9084005,1,0,2028)
	respawn_rare_mob(-1,30000,223838,94637,9084006,1,0,2029)
	respawn_rare_mob(-1,30000,225481,95473,9084002,1,0,2030)
		
	
		-- 수정 계곡 1 --
	set_way_point_type(2501,1)
	add_way_point(2501,219000,57886)
	add_way_point(2501,218834,58182)
	add_way_point(2501,218877,58360)
	add_way_point(2501,218987,58533)
	add_way_point(2501,219000,58696)
	add_way_point(2501,218973,58861)
	add_way_point(2501,218915,58971)
	add_way_point(2501,218873,59084)
	add_way_point(2501,218726,59121)
	add_way_point(2501,218568,59136)
	add_way_point(2501,218467,59287)
	add_way_point(2501,218515,59407)
	add_way_point(2501,218695,59488)
	add_way_point(2501,218842,59435)
	add_way_point(2501,218881,59294)
	add_way_point(2501,218881,59151)
	add_way_point(2501,218873,59092)
	
	set_way_point_type(2502,1)
	add_way_point(2502,218668,59145)
	add_way_point(2502,218566,59189)
	add_way_point(2502,218504,59295)
	add_way_point(2502,218537,59383)
	add_way_point(2502,218678,59446)
	add_way_point(2502,218792,59413)
	add_way_point(2502,218833,59324)
	add_way_point(2502,218838,59227)
	add_way_point(2502,218768,59170)
	add_way_point(2502,218693,59164)
	add_way_point(2502,218639,59173)
	add_way_point(2502,218568,59218)
	add_way_point(2502,218540,59315)
	add_way_point(2502,218578,59385)
	add_way_point(2502,218651,59413)
	add_way_point(2502,218699,59530)
	add_way_point(2502,218707,59619)
	add_way_point(2502,218686,59695)
	add_way_point(2502,218660,59758)
	add_way_point(2502,218652,59831)
	add_way_point(2502,218646,59922)
	
	set_way_point_type(2503,1)
	add_way_point(2503,218827,59425)
	add_way_point(2503,218630,59408)
	add_way_point(2503,218533,59327)
	add_way_point(2503,218537,59243)
	add_way_point(2503,218626,59153)
	add_way_point(2503,218733,59148)
	add_way_point(2503,218849,59229)
	add_way_point(2503,218838,59313)
	add_way_point(2503,218829,59388)
	add_way_point(2503,218832,59417)
	add_way_point(2503,218875,59472)
	add_way_point(2503,219030,59582)
	add_way_point(2503,219089,59657)
	add_way_point(2503,219175,59760)
	add_way_point(2503,219344,59856)
	add_way_point(2503,219439,59913)
	add_way_point(2503,219505,59951)
	add_way_point(2503,219594,60050)
	add_way_point(2503,219703,60144)
	add_way_point(2503,219721,60233)
	add_way_point(2503,219668,60349)
	add_way_point(2503,219617,60447)
	add_way_point(2503,219599,60570)
	add_way_point(2503,219575,60703)
	add_way_point(2503,219592,60801)
	add_way_point(2503,219659,60964)
	add_way_point(2503,219721,61068)
	add_way_point(2503,219789,61186)
	add_way_point(2503,219879,61325)
	add_way_point(2503,219935,61429)
	add_way_point(2503,219985,61528)
	add_way_point(2503,220083,61649)
	add_way_point(2503,220176,61592)
	add_way_point(2503,220280,61506)
	add_way_point(2503,220381,61427)
	add_way_point(2503,220462,61345)
	add_way_point(2503,220623,61317)
	add_way_point(2503,220783,61191)
	add_way_point(2503,220930,61016)
	add_way_point(2503,220971,60908)
	add_way_point(2503,221014,60785)
	add_way_point(2503,221035,60669)
	add_way_point(2503,220962,60573)
	add_way_point(2503,220839,60516)
	add_way_point(2503,220590,60491)
	add_way_point(2503,220367,60338)
	add_way_point(2503,220311,60179)
	add_way_point(2503,220278,60012)
	
	set_way_point_type(2504,1)
	add_way_point(2504,220286,63312)
	add_way_point(2504,220384,63577)
	add_way_point(2504,220545,63568)
	add_way_point(2504,220701,63425)
	add_way_point(2504,220764,63240)
	add_way_point(2504,220686,63091)
	add_way_point(2504,220568,62953)
	add_way_point(2504,220450,62933)
	add_way_point(2504,220350,63035)
	add_way_point(2504,220298,63176)
	add_way_point(2504,220289,63297)
	
	set_way_point_type(2505,1)
	add_way_point(2505,220357,63440)
	add_way_point(2505,220647,63448)
	add_way_point(2505,220671,63124)
	add_way_point(2505,220391,63112)
	add_way_point(2505,220364,63439)
	add_way_point(2505,220268,63275)
	add_way_point(2505,220112,63141)
	add_way_point(2505,219986,63164)
	add_way_point(2505,219850,63128)
	add_way_point(2505,219678,63095)
	add_way_point(2505,219510,63042)
	add_way_point(2505,219351,63005)
	add_way_point(2505,219220,62812)
	add_way_point(2505,218922,62435)
	add_way_point(2505,218741,62115)
	add_way_point(2505,218675,61967)
	add_way_point(2505,218756,61853)
	add_way_point(2505,218721,61704)
	add_way_point(2505,218687,61615)
	add_way_point(2505,218735,61497)
	add_way_point(2505,218774,61315)
	add_way_point(2505,218702,61125)
	add_way_point(2505,218743,60747)
	add_way_point(2505,218753,60546)
	add_way_point(2505,218752,60522)
	
	set_way_point_type(2506,1)
	add_way_point(2506,221303,64098)
	add_way_point(2506,220824,63886)
	add_way_point(2506,220676,63762)
	add_way_point(2506,220454,63598)
	add_way_point(2506,220303,63252)
	add_way_point(2506,220330,62979)
	add_way_point(2506,220491,62891)
	add_way_point(2506,220692,62988)
	add_way_point(2506,220815,63202)
	add_way_point(2506,220798,63298)
	add_way_point(2506,220696,63427)
	add_way_point(2506,220535,63573)
	add_way_point(2506,220484,63616)
	
	set_way_point_type(2507,1)
	add_way_point(2507,221268,62195)
	add_way_point(2507,221498,62356)
	add_way_point(2507,222012,62558)
	add_way_point(2507,222600,62613)
	add_way_point(2507,222867,62578)
	add_way_point(2507,222901,62334)
	add_way_point(2507,222868,62179)
	add_way_point(2507,222670,62083)
	add_way_point(2507,222354,62026)
	add_way_point(2507,222127,61941)
	add_way_point(2507,221839,61905)
	add_way_point(2507,221527,61920)
	add_way_point(2507,221391,61986)
	add_way_point(2507,221288,62106)
	add_way_point(2507,221265,62186)
	
	set_way_point_type(2508,1)
	add_way_point(2508,221826,61020)
	add_way_point(2508,221944,61073)
	add_way_point(2508,221775,61304)
	add_way_point(2508,221697,61397)
	add_way_point(2508,221593,61483)
	add_way_point(2508,221574,61673)
	add_way_point(2508,221524,61844)
	add_way_point(2508,221309,62003)
	add_way_point(2508,221259,62214)
	add_way_point(2508,221395,62352)
	add_way_point(2508,221594,62475)
	add_way_point(2508,221864,62535)
	add_way_point(2508,222291,62612)
	add_way_point(2508,222595,62655)
	add_way_point(2508,222850,62607)
	add_way_point(2508,222910,62439)
	add_way_point(2508,222872,62232)
	add_way_point(2508,222861,62144)
	add_way_point(2508,222683,62008)
	add_way_point(2508,222663,61873)
	add_way_point(2508,222680,61762)
	add_way_point(2508,222708,61548)
	add_way_point(2508,222758,61374)
	add_way_point(2508,222768,61181)
	add_way_point(2508,222762,61069)
	add_way_point(2508,222751,60987)
	add_way_point(2508,222975,60967)
	
	set_way_point_type(2509,1)
	add_way_point(2509,223831,60274)
	add_way_point(2509,223937,60108)
	add_way_point(2509,224060,59953)
	add_way_point(2509,224176,59765)
	add_way_point(2509,224263,59621)
	add_way_point(2509,224372,59457)
	add_way_point(2509,224520,59275)
	add_way_point(2509,224675,59191)
	add_way_point(2509,224817,59167)
	add_way_point(2509,224925,59223)
	add_way_point(2509,224870,59312)
	add_way_point(2509,224789,59397)
	add_way_point(2509,224785,59521)
	add_way_point(2509,224790,59647)
	add_way_point(2509,224857,59731)
	add_way_point(2509,224850,59835)
	add_way_point(2509,224894,59931)
	add_way_point(2509,224963,60067)
	add_way_point(2509,225009,60281)
	add_way_point(2509,225049,60432)
	add_way_point(2509,225034,60658)
	add_way_point(2509,224975,60884)
	add_way_point(2509,224877,61109)
	add_way_point(2509,224785,61175)
	add_way_point(2509,224658,61303)
	add_way_point(2509,224726,61496)
	add_way_point(2509,224885,61568)
	add_way_point(2509,225011,61471)
	add_way_point(2509,225051,61292)
	add_way_point(2509,224975,61229)
	add_way_point(2509,224873,61109)
	
	set_way_point_type(2510,1)
	add_way_point(2510,224753,61469)
	add_way_point(2510,224919,61478)
	add_way_point(2510,224962,61282)
	add_way_point(2510,224810,61230)
	add_way_point(2510,224711,61289)
	add_way_point(2510,224743,61470)
	add_way_point(2510,224543,61591)
	add_way_point(2510,224406,61728)
	add_way_point(2510,224382,61923)
	add_way_point(2510,224195,62105)
	add_way_point(2510,224031,62079)
	add_way_point(2510,223886,62146)
	add_way_point(2510,223865,62288)
	add_way_point(2510,223975,62326)
	add_way_point(2510,224075,62216)
	add_way_point(2510,224176,62210)
	add_way_point(2510,224200,62328)
	add_way_point(2510,224162,62440)
	add_way_point(2510,224126,62539)
	add_way_point(2510,224024,62647)
	add_way_point(2510,223961,62825)
	add_way_point(2510,223881,62998)
	add_way_point(2510,223761,63145)
	add_way_point(2510,223668,63271)
	add_way_point(2510,223605,63506)
	add_way_point(2510,223609,63599)
	add_way_point(2510,223833,63738)
	add_way_point(2510,224000,63797)
	add_way_point(2510,224259,63915)
	add_way_point(2510,224442,63959)
	
	set_way_point_type(2511,1)
	add_way_point(2511,225238,63395)
	add_way_point(2511,225424,63356)
	add_way_point(2511,225385,63159)
	add_way_point(2511,225318,63004)
	add_way_point(2511,225190,62889)
	add_way_point(2511,225042,62712)
	add_way_point(2511,224948,62553)
	add_way_point(2511,224904,62444)
	add_way_point(2511,224936,62227)
	add_way_point(2511,224934,61977)
	add_way_point(2511,224980,61845)
	add_way_point(2511,224987,61648)
	add_way_point(2511,225023,61344)
	add_way_point(2511,224918,61187)
	add_way_point(2511,224966,60893)
	add_way_point(2511,224997,60686)
	add_way_point(2511,225066,60472)
	add_way_point(2511,225120,60351)
	add_way_point(2511,225192,60273)
	add_way_point(2511,225221,60199)
	
	set_way_point_type(2512,1)
	add_way_point(2512,224696,59327)
	add_way_point(2512,224783,59140)
	add_way_point(2512,224865,58982)
	add_way_point(2512,224947,58823)
	add_way_point(2512,225009,58683)
	add_way_point(2512,225096,58592)
	add_way_point(2512,225159,58471)
	add_way_point(2512,225242,58295)
	add_way_point(2512,225214,58174)
	add_way_point(2512,225188,58065)
	add_way_point(2512,225143,57888)
	add_way_point(2512,225128,57760)
	add_way_point(2512,224787,57793)
	add_way_point(2512,224460,57763)
	
	set_way_point_type(2513,1)
	add_way_point(2513,222104,57001)
	add_way_point(2513,222091,56609)
	add_way_point(2513,222773,56645)
	add_way_point(2513,223079,57047)
	add_way_point(2513,223419,57174)
	add_way_point(2513,223677,57178)
	add_way_point(2513,223872,56961)
	add_way_point(2513,224075,56866)
	add_way_point(2513,224362,56664)
	add_way_point(2513,224618,56628)
	add_way_point(2513,224892,56653)
	add_way_point(2513,225025,56824)
	add_way_point(2513,225186,57104)
	add_way_point(2513,225263,57267)
	add_way_point(2513,225173,57534)
	add_way_point(2513,225203,57846)
	
	set_way_point_type(2514,1)
	add_way_point(2514,222483,59466)
	add_way_point(2514,222392,59641)
	add_way_point(2514,222540,59770)
	add_way_point(2514,222685,59669)
	add_way_point(2514,222746,59517)
	add_way_point(2514,222487,59466)
	add_way_point(2514,222568,59338)
	add_way_point(2514,222618,59188)
	add_way_point(2514,222698,58983)
	add_way_point(2514,222777,58853)
	add_way_point(2514,222824,58721)
	add_way_point(2514,222829,58561)
	add_way_point(2514,222806,58359)
	add_way_point(2514,222810,58225)
	add_way_point(2514,222769,58085)
	add_way_point(2514,222689,57954)
	
	set_way_point_type(2515,1)
	add_way_point(2515,222691,58105)
	add_way_point(2515,222900,58159)
	add_way_point(2515,222944,58078)
	add_way_point(2515,222880,57956)
	add_way_point(2515,222733,57885)
	add_way_point(2515,222590,57920)
	add_way_point(2515,222495,58025)
	add_way_point(2515,222428,58130)
	add_way_point(2515,222295,58294)
	add_way_point(2515,222236,58454)
	add_way_point(2515,222212,58590)
	add_way_point(2515,222171,58717)
	add_way_point(2515,222112,58802)
	add_way_point(2515,222058,58873)
	add_way_point(2515,221934,58909)
	add_way_point(2515,221716,58909)
	add_way_point(2515,221593,58842)
	add_way_point(2515,221521,58712)
	add_way_point(2515,221545,58595)
	add_way_point(2515,221565,58450)
	add_way_point(2515,221566,58333)
	add_way_point(2515,221602,58202)
	add_way_point(2515,221640,58071)
	add_way_point(2515,221555,57925)
	add_way_point(2515,221524,57845)
	add_way_point(2515,221642,57802)
	add_way_point(2515,221716,57859)
	add_way_point(2515,221682,57989)
	
	set_way_point_type(2516,1)
	add_way_point(2516,221150,59909)
	add_way_point(2516,221210,59977)
	add_way_point(2516,221236,60071)
	add_way_point(2516,221297,60137)
	add_way_point(2516,221431,60166)
	add_way_point(2516,221547,60160)
	add_way_point(2516,221593,60112)
	add_way_point(2516,221576,59984)
	add_way_point(2516,221520,59910)
	add_way_point(2516,221383,59892)
	add_way_point(2516,221234,59914)
	add_way_point(2516,221162,59914)
	
	set_way_point_type(2517,1)
	add_way_point(2517,221399,60030)
	add_way_point(2517,221368,59862)
	add_way_point(2517,221368,59665)
	add_way_point(2517,221396,59528)
	add_way_point(2517,221413,59340)
	add_way_point(2517,221407,59144)
	add_way_point(2517,221409,59019)
	add_way_point(2517,221436,58881)
	add_way_point(2517,221486,58684)
	add_way_point(2517,221540,58348)
	add_way_point(2517,221586,58223)
	add_way_point(2517,221611,58105)
	add_way_point(2517,221632,57933)
	
	set_way_point_type(2518,1)
	add_way_point(2518,220353,58519)
	add_way_point(2518,220208,58545)
	add_way_point(2518,220182,58372)
	add_way_point(2518,220273,58278)
	add_way_point(2518,220374,58360)
	add_way_point(2518,220361,58519)
	
	set_way_point_type(2519,1)
	add_way_point(2519,220285,58419)
	add_way_point(2519,220419,58350)
	add_way_point(2519,220552,58309)
	add_way_point(2519,220723,58296)
	add_way_point(2519,220928,58325)
	add_way_point(2519,221088,58437)
	add_way_point(2519,221259,58579)
	add_way_point(2519,221294,58802)
	add_way_point(2519,221341,58913)
	add_way_point(2519,221575,58902)
	add_way_point(2519,221565,58554)
	add_way_point(2519,221360,58581)
	add_way_point(2519,221343,58911)
	
	set_way_point_type(2520,1)
	add_way_point(2520,220117,61896)
	add_way_point(2520,219979,61670)
	add_way_point(2520,220230,61699)
	add_way_point(2520,220114,61884)
	add_way_point(2520,220373,61828)
	add_way_point(2520,220651,61847)
	add_way_point(2520,220874,61939)
	add_way_point(2520,221156,62024)
	add_way_point(2520,221631,61993)
	add_way_point(2520,221829,61955)
	add_way_point(2520,221987,61930)
	add_way_point(2520,222158,61966)
	add_way_point(2520,222290,62032)
	add_way_point(2520,222321,62140)
	add_way_point(2520,222468,62213)
	add_way_point(2520,222671,62209)
	add_way_point(2520,222829,62231)
	
	set_way_point_type(2521,1)
	add_way_point(2521,220635,63268)
	add_way_point(2521,220838,63289)
	add_way_point(2521,221038,63315)
	add_way_point(2521,221248,63330)
	add_way_point(2521,221355,63368)
	add_way_point(2521,221457,63329)
	add_way_point(2521,221556,63331)
	add_way_point(2521,221640,63394)
	add_way_point(2521,221851,63471)
	add_way_point(2521,222013,63597)
	add_way_point(2521,222099,63736)
	add_way_point(2521,222157,63885)
	add_way_point(2521,222305,63944)
	add_way_point(2521,222453,63897)
	add_way_point(2521,222627,63776)
	add_way_point(2521,222873,63708)
	add_way_point(2521,222976,63684)
	add_way_point(2521,223137,63593)
	add_way_point(2521,223246,63574)
	add_way_point(2521,223438,63561)
	add_way_point(2521,223605,63573)
	add_way_point(2521,223976,63745)
	add_way_point(2521,224468,63913)
	add_way_point(2521,224603,63948)
	add_way_point(2521,224388,64068)
	
	set_way_point_type(2522,1)
	add_way_point(2522,223675,60352)
	add_way_point(2522,223966,60267)
	add_way_point(2522,223958,60048)
	add_way_point(2522,224207,59731)
	add_way_point(2522,224429,59392)
	add_way_point(2522,224551,59318)
	add_way_point(2522,224709,59360)
	add_way_point(2522,224879,59343)
	add_way_point(2522,224937,59278)
	add_way_point(2522,224869,59149)
	add_way_point(2522,224818,59009)
	add_way_point(2522,224942,58812)
	add_way_point(2522,225060,58616)
	add_way_point(2522,225214,58288)
	add_way_point(2522,225178,57947)
	add_way_point(2522,225133,57737)
	add_way_point(2522,224442,57723)
	add_way_point(2522,224346,57671)
	add_way_point(2522,224284,57532)
	add_way_point(2522,224302,57446)
	add_way_point(2522,224226,57299)
	add_way_point(2522,224218,57123)
	add_way_point(2522,224222,56985)
	add_way_point(2522,224295,56844)
	add_way_point(2522,224466,56759)
	add_way_point(2522,224706,56718)
	add_way_point(2522,224889,56800)
	add_way_point(2522,225001,56868)
	add_way_point(2522,225064,56992)
	add_way_point(2522,225162,57149)
	add_way_point(2522,225165,57265)
	add_way_point(2522,225136,57402)
	add_way_point(2522,225149,57589)
	add_way_point(2522,225140,57727)
	
	set_way_point_type(2523,1)
	add_way_point(2523,222784,59589)
	add_way_point(2523,222507,59765)
	add_way_point(2523,222433,59424)
	add_way_point(2523,222821,59549)
	add_way_point(2523,223080,59595)
	add_way_point(2523,223261,59599)
	add_way_point(2523,223352,59526)
	add_way_point(2523,223447,59391)
	add_way_point(2523,223489,59246)
	add_way_point(2523,223444,59138)
	add_way_point(2523,223378,58977)
	add_way_point(2523,223354,58857)
	add_way_point(2523,223404,58739)
	add_way_point(2523,223495,58697)
	add_way_point(2523,223617,58732)
	add_way_point(2523,223715,58781)
	add_way_point(2523,223859,58732)
	add_way_point(2523,223988,58653)
	add_way_point(2523,224027,58565)
	add_way_point(2523,224004,58463)
	add_way_point(2523,223933,58370)
	add_way_point(2523,223876,58282)
	add_way_point(2523,223851,58134)
	add_way_point(2523,223908,58016)
	add_way_point(2523,223981,57909)
	add_way_point(2523,224084,57793)
	add_way_point(2523,224164,57661)
	add_way_point(2523,224285,57573)
	add_way_point(2523,224354,57519)
	
	set_way_point_type(2524,1)
	add_way_point(2524,220636,60420)
	add_way_point(2524,220388,60566)
	add_way_point(2524,220303,60323)
	add_way_point(2524,220668,60418)
	add_way_point(2524,220327,60548)
	add_way_point(2524,220261,60281)
	add_way_point(2524,220259,60072)
	add_way_point(2524,220226,59886)
	add_way_point(2524,220243,59721)
	add_way_point(2524,220248,59468)
	add_way_point(2524,220363,59380)
	add_way_point(2524,220496,59350)
	add_way_point(2524,220621,59300)
	add_way_point(2524,220652,59198)
	add_way_point(2524,220736,59152)
	add_way_point(2524,220849,59208)
	add_way_point(2524,220878,59290)
	add_way_point(2524,220830,59387)
	add_way_point(2524,220790,59496)
	add_way_point(2524,220811,59646)
	add_way_point(2524,220827,59774)
	add_way_point(2524,220874,59834)
	add_way_point(2524,220978,59867)
	add_way_point(2524,221112,59872)
	add_way_point(2524,221152,59946)
	add_way_point(2524,221319,60051)
	add_way_point(2524,221404,60056)
	
	set_way_point_type(2525,1)
	add_way_point(2525,223864,57341)
	add_way_point(2525,223806,57243)
	add_way_point(2525,223834,57090)
	add_way_point(2525,224004,57280)
	add_way_point(2525,223990,57017)
	add_way_point(2525,224207,57133)
	add_way_point(2525,224160,56849)
	add_way_point(2525,224377,56840)
	add_way_point(2525,224417,56624)
	add_way_point(2525,224620,56819)
	add_way_point(2525,224835,56613)
	add_way_point(2525,224905,56888)
	add_way_point(2525,225135,56964)
	add_way_point(2525,225132,57129)
	add_way_point(2525,225269,57247)
	add_way_point(2525,225121,57390)
	add_way_point(2525,225083,57583)
	add_way_point(2525,224831,57644)
	add_way_point(2525,224818,57778)
	add_way_point(2525,224616,57694)
	add_way_point(2525,224529,57775)
	add_way_point(2525,224444,57567)
	add_way_point(2525,224254,57512)
	add_way_point(2525,224295,57307)
	add_way_point(2525,224163,57183)
	add_way_point(2525,224101,57164)
	add_way_point(2525,223980,57264)
	add_way_point(2525,223862,57332)
	
	set_way_point_type(2526,1)
	add_way_point(2526,219693,60943)
	add_way_point(2526,219615,61020)
	add_way_point(2526,219661,61213)
	add_way_point(2526,219722,61254)
	add_way_point(2526,219791,61215)
	add_way_point(2526,219885,61183)
	add_way_point(2526,219891,61140)
	add_way_point(2526,219826,61074)
	add_way_point(2526,219755,61023)
	add_way_point(2526,219709,60941)
	add_way_point(2526,219702,60936)
	
	set_way_point_type(2527,1)
	add_way_point(2527,218500,62154)
	add_way_point(2527,218859,62112)
	add_way_point(2527,218843,62025)
	add_way_point(2527,218837,61899)
	add_way_point(2527,218730,61857)
	add_way_point(2527,218583,61849)
	add_way_point(2527,218533,61895)
	add_way_point(2527,218481,62001)
	add_way_point(2527,218500,62154)
	
	set_way_point_type(2528,1)
	add_way_point(2528,221596,62269)
	add_way_point(2528,221492,62525)
	add_way_point(2528,221750,62624)
	add_way_point(2528,221945,62628)
	add_way_point(2528,222101,62653)
	add_way_point(2528,222242,62705)
	add_way_point(2528,222354,62716)
	add_way_point(2528,222534,62721)
	add_way_point(2528,222702,62718)
	add_way_point(2528,222854,62711)
	add_way_point(2528,223035,62677)
	add_way_point(2528,223042,62596)
	add_way_point(2528,222975,62559)
	add_way_point(2528,222747,62562)
	add_way_point(2528,222559,62545)
	add_way_point(2528,222299,62558)
	add_way_point(2528,222095,62529)
	add_way_point(2528,222040,62456)
	add_way_point(2528,222003,62366)
	add_way_point(2528,221914,62413)
	add_way_point(2528,221829,62439)
	add_way_point(2528,221714,62401)
	add_way_point(2528,221660,62326)
	add_way_point(2528,221603,62274)
	
	set_way_point_type(2529,1)
	add_way_point(2529,223838,62381)
	add_way_point(2529,224152,62451)
	add_way_point(2529,224248,62044)
	add_way_point(2529,223877,62044)
	add_way_point(2529,223776,62175)
	add_way_point(2529,223834,62384)
	
	set_way_point_type(2530,1)
	add_way_point(2530,225481,63217)
	add_way_point(2530,225220,63368)
	add_way_point(2530,225239,63456)
	add_way_point(2530,225350,63460)
	add_way_point(2530,225438,63370)
	add_way_point(2530,225414,63125)
	add_way_point(2530,225204,62884)
	add_way_point(2530,225077,62749)
	add_way_point(2530,224987,62607)
	add_way_point(2530,224909,62440)
	add_way_point(2530,224891,62290)
	add_way_point(2530,224939,62113)
	add_way_point(2530,224940,61917)
	add_way_point(2530,224983,61750)
	add_way_point(2530,224976,61587)
	add_way_point(2530,224803,61549)
	add_way_point(2530,224646,61535)
	add_way_point(2530,224520,61607)
	add_way_point(2530,224402,61768)
	add_way_point(2530,224363,61911)
	add_way_point(2530,224199,62063)
	add_way_point(2530,224066,62219)
	add_way_point(2530,224083,62428)
	add_way_point(2530,224085,62598)
	add_way_point(2530,224001,62734)
	add_way_point(2530,223871,63006)
	add_way_point(2530,223692,63245)
	add_way_point(2530,223607,63567)
	add_way_point(2530,223257,63579)
	add_way_point(2530,222865,63736)
	add_way_point(2530,222462,63873)
	add_way_point(2530,222244,63955)
	add_way_point(2530,222167,63836)
	add_way_point(2530,222014,63619)
	add_way_point(2530,221814,63465)
	add_way_point(2530,221627,63403)
	add_way_point(2530,221550,63333)
	add_way_point(2530,221410,63351)
	add_way_point(2530,221330,63389)
	add_way_point(2530,221240,63343)
	add_way_point(2530,221068,63320)
	add_way_point(2530,220885,63286)
	add_way_point(2530,220629,63267)
	
	
	respawn_rare_mob(-1,30000,219000,57886,9070015,1,0,2501)
	respawn_rare_mob(-1,30000,218668,59145,9071004,1,0,2502)
	respawn_rare_mob(-1,30000,218827,59425,9072003,1,0,2503)
	respawn_rare_mob(-1,30000,220286,63312,9074008,1,0,2504)
	respawn_rare_mob(-1,30000,220357,63440,9074009,1,0,2505)
	respawn_rare_mob(-1,30000,221303,64098,9075007,1,0,2506)
	respawn_rare_mob(-1,30000,221268,62195,9075008,1,0,2507)
	respawn_rare_mob(-1,30000,221826,61020,9075009,1,0,2508)
	respawn_rare_mob(-1,30000,223831,60274,9076001,1,0,2509)
	respawn_rare_mob(-1,30000,224753,61469,9076007,1,0,2510)
	respawn_rare_mob(-1,30000,225238,63395,9078011,1,0,2511)
	respawn_rare_mob(-1,30000,224696,59327,9080007,1,0,2512)
	respawn_rare_mob(-1,30000,222104,57001,9090017,1,0,2513)
	respawn_rare_mob(-1,30000,222483,59466,9089014,1,0,2514)
	respawn_rare_mob(-1,30000,222691,58105,9090007,1,0,2515)
	respawn_rare_mob(-1,30000,221150,59909,9089005,1,0,2516)
	respawn_rare_mob(-1,30000,221399,60030,9088013,1,0,2517)
	respawn_rare_mob(-1,30000,220353,58519,9088014,1,0,2518)
	respawn_rare_mob(-1,30000,220285,58419,9088015,1,0,2519)
	respawn_rare_mob(-1,30000,220117,61896,9087014,1,0,2520)
	respawn_rare_mob(-1,30000,220635,63268,9088001,1,0,2521)
	respawn_rare_mob(-1,30000,223675,60352,9088002,1,0,2522)
	respawn_rare_mob(-1,30000,222784,59589,9086007,1,0,2523)
	respawn_rare_mob(-1,30000,220636,60420,9086008,1,0,2524)
	respawn_rare_mob(-1,30000,223864,57341,9086009,1,0,2525)
	respawn_rare_mob(-1,30000,219693,60943,9086010,1,0,2526)
	respawn_rare_mob(-1,30000,218500,62154,9084004,1,0,2527)
	respawn_rare_mob(-1,30000,221596,62269,9084005,1,0,2528)
	respawn_rare_mob(-1,30000,223838,62381,9084006,1,0,2529)
	respawn_rare_mob(-1,30000,225481,63217,9084002,1,0,2530)

		
					-- 잃어 버린 갱도 2 --
						
	set_way_point_type( 3001, 1 )					
	add_way_point( 3001, 223680, 106406 )
	add_way_point( 3001, 223701, 106709 )
	add_way_point( 3001, 223799, 106863 )
	add_way_point( 3001, 224021, 106930 )
	add_way_point( 3001, 224252, 106916 )
	add_way_point( 3001, 224464, 106897 )
	add_way_point( 3001, 224604, 106771 )
	add_way_point( 3001, 224666, 106537 )
	add_way_point( 3001, 224643, 106301 )
	add_way_point( 3001, 224615, 106144 )
	add_way_point( 3001, 224523, 106083 )
	add_way_point( 3001, 224392, 106020 )
	add_way_point( 3001, 224065, 105968 )
	add_way_point( 3001, 223841, 105955 )
	add_way_point( 3001, 223734, 106023 )
	add_way_point( 3001, 223678, 106222 )
	add_way_point( 3001, 223680, 106406 )
	
	set_way_point_type( 3002, 1 )					
	add_way_point( 3002, 224177, 106979 )
	add_way_point( 3002, 224395, 106961 )
	add_way_point( 3002, 224560, 106888 )
	add_way_point( 3002, 224660, 106764 )
	add_way_point( 3002, 224667, 106590 )
	add_way_point( 3002, 224708, 106436 )
	add_way_point( 3002, 224700, 106338 )
	add_way_point( 3002, 224675, 106233 )
	add_way_point( 3002, 224631, 106141 )
	add_way_point( 3002, 224546, 106092 )
	add_way_point( 3002, 224454, 106053 )
	add_way_point( 3002, 224317, 105991 )
	add_way_point( 3002, 224036, 105945 )
	add_way_point( 3002, 223861, 105932 )
	add_way_point( 3002, 223725, 105965 )
	add_way_point( 3002, 223649, 106066 )
	add_way_point( 3002, 223635, 106206 )
	add_way_point( 3002, 223648, 106317 )
	add_way_point( 3002, 223663, 106413 )
	add_way_point( 3002, 223666, 106498 )
	add_way_point( 3002, 223672, 106612 )
	add_way_point( 3002, 223687, 106692 )
	add_way_point( 3002, 223717, 106760 )
	add_way_point( 3002, 223759, 106823 )
	add_way_point( 3002, 223825, 106859 )
	add_way_point( 3002, 223925, 106903 )
	add_way_point( 3002, 224011, 106944 )
	add_way_point( 3002, 224173, 106982 )

	set_way_point_type( 3003, 1 )	
	add_way_point( 3003, 223578, 105738 )
	add_way_point( 3003, 223525, 105821 )
	add_way_point( 3003, 223530, 105927 )
	add_way_point( 3003, 223542, 106053 )
	add_way_point( 3003, 223554, 106128 )
	add_way_point( 3003, 223587, 106184 )
	add_way_point( 3003, 223626, 106203 )
	add_way_point( 3003, 223691, 106147 )
	add_way_point( 3003, 223750, 106075 )
	add_way_point( 3003, 223796, 106042 )
	add_way_point( 3003, 223857, 106029 )
	add_way_point( 3003, 223916, 106042 )
	add_way_point( 3003, 224007, 106081 )
	add_way_point( 3003, 224075, 106071 )
	add_way_point( 3003, 224146, 106061 )
	add_way_point( 3003, 224256, 106042 )
	add_way_point( 3003, 224321, 106039 )
	add_way_point( 3003, 224392, 106030 )
	add_way_point( 3003, 224480, 106043 )
	add_way_point( 3003, 224559, 106096 )
	add_way_point( 3003, 224628, 106141 )
	add_way_point( 3003, 224704, 106203 )
	add_way_point( 3003, 224783, 106246 )
	add_way_point( 3003, 224839, 106301 )
	add_way_point( 3003, 224863, 106347 )
	add_way_point( 3003, 224891, 106410 )
	add_way_point( 3003, 224924, 106452 )
	add_way_point( 3003, 224944, 106497 )
	add_way_point( 3003, 224939, 106554 )
	add_way_point( 3003, 224876, 106659 )
	add_way_point( 3003, 224711, 106796 )
	add_way_point( 3003, 224590, 106918 )
	add_way_point( 3003, 224512, 107005 )
	add_way_point( 3003, 224436, 107039 )
	add_way_point( 3003, 224287, 107050 )
	add_way_point( 3003, 224144, 107030 )
	add_way_point( 3003, 223994, 106988 )
	add_way_point( 3003, 223908, 106931 )
	add_way_point( 3003, 223743, 106826 )
	add_way_point( 3003, 223720, 106753 )
	add_way_point( 3003, 223698, 106672 )
	add_way_point( 3003, 223692, 106522 )
	add_way_point( 3003, 223673, 106426 )
	add_way_point( 3003, 223648, 106275 )
	add_way_point( 3003, 223662, 106180 )
	add_way_point( 3003, 223724, 106088 )
	add_way_point( 3003, 223780, 106042 )
	add_way_point( 3003, 223854, 106003 )
	add_way_point( 3003, 223922, 105961 )
	add_way_point( 3003, 224061, 105962 )
	add_way_point( 3003, 224149, 105917 )
	add_way_point( 3003, 224149, 105888 )
	add_way_point( 3003, 224103, 105869 )
	add_way_point( 3003, 224029, 105846 )
	add_way_point( 3003, 223981, 105804 )
	add_way_point( 3003, 223926, 105763 )
	add_way_point( 3003, 223858, 105728 )
	add_way_point( 3003, 223794, 105693 )
	add_way_point( 3003, 223720, 105705 )
	add_way_point( 3003, 223613, 105722 )
	add_way_point( 3003, 223584, 105747 )
	
	set_way_point_type( 3004, 1 )	
	add_way_point( 3004, 223704, 106750 )
	add_way_point( 3004, 223670, 106429 )
	add_way_point( 3004, 223536, 106367 )
	add_way_point( 3004, 223511, 106217 )
	add_way_point( 3004, 223535, 106096 )
	add_way_point( 3004, 223497, 105998 )
	add_way_point( 3004, 223502, 105889 )
	add_way_point( 3004, 223500, 105786 )
	add_way_point( 3004, 223555, 105706 )
	add_way_point( 3004, 223659, 105683 )
	add_way_point( 3004, 223739, 105683 )
	add_way_point( 3004, 223800, 105721 )
	add_way_point( 3004, 223913, 105795 )
	add_way_point( 3004, 224081, 105878 )
	add_way_point( 3004, 224194, 105923 )
	add_way_point( 3004, 224304, 105997 )
	add_way_point( 3004, 224435, 106036 )
	add_way_point( 3004, 224543, 106079 )
	add_way_point( 3004, 224677, 106132 )
	add_way_point( 3004, 224792, 106216 )
	add_way_point( 3004, 224875, 106320 )
	add_way_point( 3004, 224911, 106426 )
	add_way_point( 3004, 224883, 106561 )
	add_way_point( 3004, 224798, 106669 )
	add_way_point( 3004, 224577, 106867 )
	add_way_point( 3004, 224426, 106995 )
	add_way_point( 3004, 224160, 107037 )
	add_way_point( 3004, 223911, 106975 )
	add_way_point( 3004, 223792, 106853 )
	add_way_point( 3004, 223713, 106800 )
	add_way_point( 3004, 223714, 106723 )
	add_way_point( 3004, 223682, 106572 )
	add_way_point( 3004, 223663, 106422 )
	add_way_point( 3004, 223553, 106239 )
	add_way_point( 3004, 223506, 106112 )
	add_way_point( 3004, 223524, 105969 )
	add_way_point( 3004, 223580, 105827 )
	add_way_point( 3004, 223648, 105789 )
	add_way_point( 3004, 223800, 105795 )
	add_way_point( 3004, 223916, 105849 )
	add_way_point( 3004, 224016, 105907 )
	
	set_way_point_type( 3005, 1 )
	add_way_point( 3005, 224754, 105824 )
	add_way_point( 3005, 224593, 106033 )
	add_way_point( 3005, 224751, 105828 )
	add_way_point( 3005, 224796, 105616 )
	
	set_way_point_type( 3006, 1 )
	add_way_point( 3006, 223359, 106375 )
	add_way_point( 3006, 223169, 106419 )
	add_way_point( 3006, 223039, 106410 )
	add_way_point( 3006, 223012, 106309 )
	add_way_point( 3006, 222975, 106174 )
	add_way_point( 3006, 222948, 106044 )
	add_way_point( 3006, 222925, 105911 )
	add_way_point( 3006, 222895, 105803 )
	add_way_point( 3006, 222879, 105701 )
	add_way_point( 3006, 222843, 105586 )
	add_way_point( 3006, 222788, 105482 )
	add_way_point( 3006, 222726, 105446 )
	add_way_point( 3006, 222641, 105486 )
	add_way_point( 3006, 222548, 105592 )
	add_way_point( 3006, 222508, 105695 )
	add_way_point( 3006, 222458, 105775 )
	add_way_point( 3006, 222404, 105851 )
	add_way_point( 3006, 222308, 105921 )
	add_way_point( 3006, 222271, 105935 )
	add_way_point( 3006, 222247, 106052 )
	add_way_point( 3006, 222130, 106075 )
	add_way_point( 3006, 222042, 106021 )
	add_way_point( 3006, 222041, 105892 )
	add_way_point( 3006, 222095, 105818 )
	add_way_point( 3006, 222237, 105834 )
	add_way_point( 3006, 222265, 105931 )
	
	set_way_point_type( 3007, 1 )
	add_way_point( 3007, 222093, 106085 )
	add_way_point( 3007, 222001, 106021 )
	add_way_point( 3007, 221895, 105963 )
	add_way_point( 3007, 221847, 105896 )
	add_way_point( 3007, 221874, 105787 )
	add_way_point( 3007, 221943, 105723 )
	add_way_point( 3007, 222039, 105742 )
	add_way_point( 3007, 222161, 105757 )
	add_way_point( 3007, 222276, 105801 )
	add_way_point( 3007, 222371, 105860 )
	add_way_point( 3007, 222411, 105917 )
	add_way_point( 3007, 222391, 105974 )
	add_way_point( 3007, 222308, 106048 )
	add_way_point( 3007, 222239, 106098 )
	add_way_point( 3007, 222076, 106086 )
	
	set_way_point_type( 3008, 1 )
	add_way_point( 3008, 222050, 106058 )
	add_way_point( 3008, 222017, 105981 )
	add_way_point( 3008, 222028, 105867 )
	add_way_point( 3008, 222055, 105810 )
	add_way_point( 3008, 222141, 105809 )
	add_way_point( 3008, 222272, 105808 )
	add_way_point( 3008, 222299, 105887 )
	add_way_point( 3008, 222299, 105988 )
	add_way_point( 3008, 222245, 106058 )
	add_way_point( 3008, 222109, 106082 )
	add_way_point( 3008, 222004, 105984 )
	add_way_point( 3008, 221965, 105874 )
	add_way_point( 3008, 221980, 105746 )
	add_way_point( 3008, 222007, 105623 )
	add_way_point( 3008, 222038, 105503 )
	add_way_point( 3008, 222074, 105384 )
	add_way_point( 3008, 222088, 105302 )
	add_way_point( 3008, 222075, 105238 )
	add_way_point( 3008, 222018, 105124 )
	add_way_point( 3008, 221957, 105037 )
	add_way_point( 3008, 221906, 104992 )
	add_way_point( 3008, 221806, 104983 )
	add_way_point( 3008, 221731, 104995 )
	add_way_point( 3008, 221653, 105050 )
	add_way_point( 3008, 221589, 105170 )
	add_way_point( 3008, 221510, 105254 )
	add_way_point( 3008, 221423, 105365 )
	add_way_point( 3008, 221333, 105444 )
	add_way_point( 3008, 221227, 105522 )
	add_way_point( 3008, 221141, 105572 )
	
	set_way_point_type( 3009, 1 )
	add_way_point( 3009, 220724, 105108 )
	add_way_point( 3009, 220697, 104949 )
	add_way_point( 3009, 220623, 104843 )
	add_way_point( 3009, 220605, 104705 )
	add_way_point( 3009, 220683, 104726 )
	add_way_point( 3009, 220718, 104744 )
	add_way_point( 3009, 220762, 104677 )
	add_way_point( 3009, 220822, 104698 )
	add_way_point( 3009, 220846, 104751 )
	add_way_point( 3009, 220884, 104787 )
	add_way_point( 3009, 220823, 104878 )
	add_way_point( 3009, 220808, 104942 )
	add_way_point( 3009, 220764, 105044 )
	add_way_point( 3009, 220724, 105097 )
	
	set_way_point_type( 3010, 1 )
	add_way_point( 3010, 220921, 106230 )
	add_way_point( 3010, 220939, 106102 )
	add_way_point( 3010, 221000, 105959 )
	add_way_point( 3010, 221047, 105832 )
	add_way_point( 3010, 221083, 105713 )
	add_way_point( 3010, 221101, 105617 )
	add_way_point( 3010, 221036, 105544 )
	add_way_point( 3010, 220960, 105463 )
	add_way_point( 3010, 220895, 105345 )
	add_way_point( 3010, 220838, 105280 )
	add_way_point( 3010, 220774, 105173 )
	add_way_point( 3010, 220734, 105085 )
	add_way_point( 3010, 220730, 104968 )
	add_way_point( 3010, 220734, 104894 )
	add_way_point( 3010, 220741, 104830 )
	
	set_way_point_type( 3011, 1 )
	add_way_point( 3011, 221159, 105602 )
	add_way_point( 3011, 220986, 105966 )
	add_way_point( 3011, 220917, 106158 )
	add_way_point( 3011, 220928, 106309 )
	add_way_point( 3011, 221013, 106375 )
	add_way_point( 3011, 221113, 106447 )
	add_way_point( 3011, 221205, 106484 )
	add_way_point( 3011, 221294, 106475 )
	add_way_point( 3011, 221371, 106441 )
	add_way_point( 3011, 221431, 106461 )
	add_way_point( 3011, 221505, 106522 )
	add_way_point( 3011, 221558, 106622 )
	add_way_point( 3011, 221611, 106697 )
	add_way_point( 3011, 221690, 106758 )
	add_way_point( 3011, 221784, 106797 )
	add_way_point( 3011, 221900, 106841 )
	add_way_point( 3011, 221959, 106822 )
	add_way_point( 3011, 222010, 106767 )
	add_way_point( 3011, 222050, 106732 )
	add_way_point( 3011, 222191, 106868 )
	
	set_way_point_type( 3012, 1 )
	add_way_point( 3012, 223187, 108133 )
	add_way_point( 3012, 223100, 108227 )
	add_way_point( 3012, 223029, 108300 )
	add_way_point( 3012, 223082, 108458 )
	add_way_point( 3012, 223130, 108539 )
	add_way_point( 3012, 223154, 108624 )
	add_way_point( 3012, 223243, 108721 )
	add_way_point( 3012, 223322, 108837 )
	add_way_point( 3012, 223382, 108934 )
	add_way_point( 3012, 223345, 109065 )
	add_way_point( 3012, 223371, 109209 )
	add_way_point( 3012, 223504, 109272 )
	add_way_point( 3012, 223609, 109217 )
	add_way_point( 3012, 223627, 109093 )
	add_way_point( 3012, 223584, 108994 )
	add_way_point( 3012, 223491, 108927 )
	add_way_point( 3012, 223404, 108944 )
	
	set_way_point_type( 3013, 1 )
	add_way_point( 3013, 223243, 108155 )
	add_way_point( 3013, 223344, 108203 )
	add_way_point( 3013, 223437, 108244 )
	add_way_point( 3013, 223551, 108172 )
	add_way_point( 3013, 223611, 108105 )
	add_way_point( 3013, 223714, 108018 )
	add_way_point( 3013, 223787, 107979 )
	add_way_point( 3013, 223873, 107936 )
	add_way_point( 3013, 223964, 107859 )
	add_way_point( 3013, 224060, 107812 )
	add_way_point( 3013, 224221, 107814 )
	add_way_point( 3013, 224337, 107845 )
	add_way_point( 3013, 224453, 107883 )
	add_way_point( 3013, 224571, 107843 )
	add_way_point( 3013, 224662, 107793 )
	add_way_point( 3013, 224753, 107732 )
	add_way_point( 3013, 224867, 107633 )
	add_way_point( 3013, 224973, 107540 )
	add_way_point( 3013, 225056, 107453 )
	add_way_point( 3013, 225122, 107389 )
	add_way_point( 3013, 225242, 107402 )
	add_way_point( 3013, 225286, 107499 )
	add_way_point( 3013, 225262, 107549 )
	add_way_point( 3013, 225177, 107486 )
	add_way_point( 3013, 225113, 107504 )
	
	set_way_point_type( 3014, 1 )
	add_way_point( 3014, 223204, 108190 )
	add_way_point( 3014, 223192, 108025 )
	add_way_point( 3014, 223059, 107849 )
	add_way_point( 3014, 223213, 107818 )
	add_way_point( 3014, 223107, 107735 )
	add_way_point( 3014, 223048, 107749 )
	add_way_point( 3014, 223026, 107795 )
	add_way_point( 3014, 223174, 107862 )
	add_way_point( 3014, 223135, 107647 )
	add_way_point( 3014, 223127, 107506 )
	add_way_point( 3014, 223106, 107376 )
	add_way_point( 3014, 223066, 107276 )
	add_way_point( 3014, 222981, 107206 )
	add_way_point( 3014, 222846, 107162 )
	add_way_point( 3014, 222679, 107137 )
	add_way_point( 3014, 222525, 107100 )
	add_way_point( 3014, 222365, 107009 )
	add_way_point( 3014, 222324, 106934 )
	add_way_point( 3014, 222235, 106786 )
	add_way_point( 3014, 222162, 106731 )
	add_way_point( 3014, 222032, 106735 )
	add_way_point( 3014, 222091, 106858 )
	add_way_point( 3014, 222247, 106903 )
	
	set_way_point_type( 3015, 1 )
	add_way_point( 3015, 223617, 108998 )
	add_way_point( 3015, 223447, 108991 )
	add_way_point( 3015, 223338, 109104 )
	add_way_point( 3015, 223419, 109244 )
	add_way_point( 3015, 223562, 109232 )
	add_way_point( 3015, 223634, 109113 )
	add_way_point( 3015, 223661, 108985 )
	add_way_point( 3015, 223749, 108953 )
	add_way_point( 3015, 223816, 108891 )
	add_way_point( 3015, 223879, 108797 )
	add_way_point( 3015, 223926, 108727 )
	add_way_point( 3015, 223996, 108679 )
	add_way_point( 3015, 224082, 108634 )
	add_way_point( 3015, 224182, 108598 )
	add_way_point( 3015, 224305, 108553 )
	add_way_point( 3015, 224398, 108522 )
	add_way_point( 3015, 224477, 108497 )
	add_way_point( 3015, 224566, 108475 )
	add_way_point( 3015, 224663, 108458 )
	add_way_point( 3015, 224624, 108384 )
	
	set_way_point_type( 3016, 1 )
	add_way_point( 3016, 223267, 109081 )
	add_way_point( 3016, 223353, 108974 )
	add_way_point( 3016, 223439, 108935 )
	add_way_point( 3016, 223616, 108960 )
	add_way_point( 3016, 223770, 108957 )
	add_way_point( 3016, 223875, 108935 )
	add_way_point( 3016, 223940, 108947 )
	add_way_point( 3016, 224038, 108964 )
	add_way_point( 3016, 224158, 109013 )
	add_way_point( 3016, 224261, 109063 )
	add_way_point( 3016, 224411, 109112 )
	add_way_point( 3016, 224547, 109234 )
	add_way_point( 3016, 224662, 109320 )
	add_way_point( 3016, 224794, 109404 )
	add_way_point( 3016, 224879, 109529 )
	add_way_point( 3016, 224969, 109648 )
	add_way_point( 3016, 225025, 109798 )
	add_way_point( 3016, 225063, 109998 )
	add_way_point( 3016, 225049, 110232 )
	
	set_way_point_type( 3017, 1 )
	add_way_point( 3017, 224117, 110774 )
	add_way_point( 3017, 223980, 110791 )
	add_way_point( 3017, 223981, 110954 )
	add_way_point( 3017, 224151, 110994 )
	add_way_point( 3017, 224294, 111018 )
	add_way_point( 3017, 224491, 110934 )
	add_way_point( 3017, 224605, 110811 )
	add_way_point( 3017, 224711, 110699 )
	add_way_point( 3017, 224829, 110599 )
	add_way_point( 3017, 224928, 110444 )
	add_way_point( 3017, 224987, 110213 )
	add_way_point( 3017, 225071, 110276 )
	add_way_point( 3017, 225108, 110204 )
	add_way_point( 3017, 225049, 110012 )
	
	set_way_point_type( 3018, 1 )
	add_way_point( 3018, 223981, 110985 )
	add_way_point( 3018, 224148, 110905 )
	add_way_point( 3018, 224167, 110841 )
	add_way_point( 3018, 224125, 110750 )
	add_way_point( 3018, 224143, 110639 )
	add_way_point( 3018, 224145, 110569 )
	add_way_point( 3018, 224191, 110485 )
	add_way_point( 3018, 224245, 110385 )
	add_way_point( 3018, 224273, 110285 )
	add_way_point( 3018, 224280, 110157 )
	add_way_point( 3018, 224284, 110055 )
	add_way_point( 3018, 224250, 109989 )
	add_way_point( 3018, 224196, 109932 )
	add_way_point( 3018, 224106, 109912 )
	add_way_point( 3018, 223981, 109945 )
	add_way_point( 3018, 223857, 110031 )
	add_way_point( 3018, 223765, 110124 )
	add_way_point( 3018, 223653, 110226 )
	add_way_point( 3018, 223514, 110374 )
	add_way_point( 3018, 223410, 110386 )
	add_way_point( 3018, 223333, 110315 )
	add_way_point( 3018, 223266, 110259 )
	add_way_point( 3018, 223242, 110192 )
	add_way_point( 3018, 223209, 110093 )
	add_way_point( 3018, 223190, 110002 )
	add_way_point( 3018, 223246, 109909 )
	add_way_point( 3018, 223316, 109848 )
	add_way_point( 3018, 223349, 109738 )
	add_way_point( 3018, 223348, 109665 )
	add_way_point( 3018, 223407, 109642 )
	add_way_point( 3018, 223424, 109584 )
	add_way_point( 3018, 223445, 109515 )
	add_way_point( 3018, 223447, 109450 )
	add_way_point( 3018, 223442, 109358 )
	add_way_point( 3018, 223464, 109217 )
	
	set_way_point_type( 3019, 1 )
	add_way_point( 3019, 222517, 110131 )
	add_way_point( 3019, 222640, 110145 )
	add_way_point( 3019, 222758, 110215 )
	add_way_point( 3019, 222818, 110174 )
	add_way_point( 3019, 222927, 110154 )
	add_way_point( 3019, 222984, 110119 )
	add_way_point( 3019, 223036, 110096 )
	add_way_point( 3019, 223097, 110084 )
	add_way_point( 3019, 223168, 110069 )
	add_way_point( 3019, 223148, 109962 )
	add_way_point( 3019, 223135, 109919 )
	add_way_point( 3019, 223043, 109930 )
	add_way_point( 3019, 222941, 109922 )
	add_way_point( 3019, 222871, 109903 )
	add_way_point( 3019, 222810, 109954 )
	add_way_point( 3019, 222716, 109908 )
	add_way_point( 3019, 222638, 109928 )
	add_way_point( 3019, 222616, 110026 )
	add_way_point( 3019, 222540, 110066 )
	add_way_point( 3019, 222514, 110132 )
	
	set_way_point_type( 3020, 1 )
	add_way_point( 3020, 221075, 111079 )
	add_way_point( 3020, 221054, 110861 )
	add_way_point( 3020, 221271, 110864 )
	add_way_point( 3020, 221428, 111042 )
	add_way_point( 3020, 221569, 111070 )
	add_way_point( 3020, 221692, 111019 )
	add_way_point( 3020, 221864, 110909 )
	add_way_point( 3020, 222005, 110840 )
	add_way_point( 3020, 222096, 110742 )
	add_way_point( 3020, 222145, 110652 )
	add_way_point( 3020, 222217, 110556 )
	add_way_point( 3020, 222237, 110425 )
	add_way_point( 3020, 222265, 110325 )
	add_way_point( 3020, 222306, 110189 )
	add_way_point( 3020, 222379, 110172 )
	add_way_point( 3020, 222489, 110134 )
	add_way_point( 3020, 222621, 110091 )
	add_way_point( 3020, 222790, 110055 )
	add_way_point( 3020, 222869, 110034 )
	add_way_point( 3020, 223075, 110009 )
	
	set_way_point_type( 3021, 1 )
	add_way_point( 3021, 220735, 112085 )
	add_way_point( 3021, 220612, 112077 )
	add_way_point( 3021, 220551, 112009 )
	add_way_point( 3021, 220619, 111935 )
	add_way_point( 3021, 220705, 111823 )
	add_way_point( 3021, 220789, 111855 )
	add_way_point( 3021, 220989, 111933 )
	add_way_point( 3021, 220805, 111988 )
	add_way_point( 3021, 220742, 112058 )
	add_way_point( 3021, 220643, 112026 )
	add_way_point( 3021, 220733, 111926 )
	
	set_way_point_type( 3022, 1 )
	add_way_point( 3022, 220740, 111977 )
	add_way_point( 3022, 220886, 111955 )
	add_way_point( 3022, 221031, 111938 )
	add_way_point( 3022, 221137, 111955 )
	add_way_point( 3022, 221261, 111974 )
	add_way_point( 3022, 221385, 111972 )
	add_way_point( 3022, 221479, 111929 )
	add_way_point( 3022, 221519, 111842 )
	add_way_point( 3022, 221519, 111742 )
	add_way_point( 3022, 221502, 111642 )
	add_way_point( 3022, 221450, 111521 )
	add_way_point( 3022, 221428, 111394 )
	add_way_point( 3022, 221444, 111222 )
	add_way_point( 3022, 221425, 111137 )
	add_way_point( 3022, 221338, 111036 )
	add_way_point( 3022, 221259, 110940 )
	add_way_point( 3022, 221172, 110918 )
	add_way_point( 3022, 221122, 111013 )
	add_way_point( 3022, 221179, 111079 )
	add_way_point( 3022, 221285, 111095 )
	
	set_way_point_type( 3023, 1 )
	add_way_point( 3023, 219976, 108017 )
	add_way_point( 3023, 220034, 107909 )
	add_way_point( 3023, 220199, 107924 )
	add_way_point( 3023, 220263, 108109 )
	add_way_point( 3023, 220457, 108174 )
	add_way_point( 3023, 220697, 108268 )
	add_way_point( 3023, 220870, 108406 )
	add_way_point( 3023, 220797, 108768 )
	add_way_point( 3023, 220729, 109067 )
	add_way_point( 3023, 220676, 109261 )
	add_way_point( 3023, 220548, 109356 )
	
	set_way_point_type( 3024, 1 )
	add_way_point( 3024, 221795, 109835 )
	add_way_point( 3024, 221899, 109673 )
	add_way_point( 3024, 221684, 109651 )
	add_way_point( 3024, 221529, 109544 )
	add_way_point( 3024, 221445, 109487 )
	add_way_point( 3024, 221387, 109484 )
	add_way_point( 3024, 221347, 109435 )
	add_way_point( 3024, 221364, 109345 )
	add_way_point( 3024, 221400, 109173 )
	add_way_point( 3024, 221456, 108908 )
	add_way_point( 3024, 221497, 108724 )
	add_way_point( 3024, 221652, 108726 )
	add_way_point( 3024, 221788, 108765 )
	add_way_point( 3024, 221924, 108894 )
	add_way_point( 3024, 222035, 108819 )
	add_way_point( 3024, 222001, 108605 )
	add_way_point( 3024, 221948, 108472 )
	add_way_point( 3024, 222045, 108343 )
	
	set_way_point_type( 3025, 1 )
	add_way_point( 3025, 221427, 108752 )
	add_way_point( 3025, 220908, 108515 )
	add_way_point( 3025, 220617, 108157 )
	add_way_point( 3025, 220498, 107741 )
	add_way_point( 3025, 220610, 107618 )
	add_way_point( 3025, 220874, 107698 )
	add_way_point( 3025, 221159, 107800 )
	add_way_point( 3025, 221288, 107681 )
	add_way_point( 3025, 221549, 107696 )
	add_way_point( 3025, 221423, 108036 )
	add_way_point( 3025, 221511, 108255 )
	add_way_point( 3025, 221737, 108342 )
	add_way_point( 3025, 222040, 108345 )
	
	set_way_point_type( 3026, 1 )
	add_way_point( 3026, 221937, 108805 )
	add_way_point( 3026, 221319, 108588 )
	add_way_point( 3026, 221027, 108459 )
	add_way_point( 3026, 220795, 108142 )
	add_way_point( 3026, 220647, 107863 )
	add_way_point( 3026, 220731, 107770 )
	add_way_point( 3026, 221157, 107906 )
	add_way_point( 3026, 221287, 108173 )
	add_way_point( 3026, 221525, 108375 )
	add_way_point( 3026, 221783, 108446 )
	add_way_point( 3026, 221955, 108650 )
	add_way_point( 3026, 221939, 108794 )
	
	set_way_point_type( 3027, 1 )
	add_way_point( 3027, 221850, 108704 )
	add_way_point( 3027, 221280, 108461 )
	add_way_point( 3027, 220961, 108232 )
	add_way_point( 3027, 220824, 107946 )
	add_way_point( 3027, 221074, 107980 )
	add_way_point( 3027, 221246, 108260 )
	add_way_point( 3027, 221470, 108433 )
	add_way_point( 3027, 221798, 108582 )
	add_way_point( 3027, 221850, 108701 )
	
	set_way_point_type( 3028, 1 )
	add_way_point( 3028, 219731, 110233 )
	add_way_point( 3028, 219619, 110288 )
	add_way_point( 3028, 219593, 110142 )
	add_way_point( 3028, 219590, 109955 )
	add_way_point( 3028, 219683, 109905 )
	add_way_point( 3028, 219806, 109929 )
	add_way_point( 3028, 219912, 110012 )
	add_way_point( 3028, 219955, 110125 )
	add_way_point( 3028, 219935, 110227 )
	add_way_point( 3028, 219821, 110255 )
	add_way_point( 3028, 219762, 110243 )
	add_way_point( 3028, 219742, 110234 )
	
	set_way_point_type( 3030, 1 )
	add_way_point( 3030, 219621, 110016 )
	add_way_point( 3030, 219678, 110154 )
	add_way_point( 3030, 219815, 110177 )
	add_way_point( 3030, 219914, 110140 )
	add_way_point( 3030, 220124, 110230 )
	add_way_point( 3030, 220278, 110284 )
	add_way_point( 3030, 220359, 110419 )
	add_way_point( 3030, 220505, 110604 )
	add_way_point( 3030, 220689, 110752 )
	add_way_point( 3030, 220823, 110803 )
	add_way_point( 3030, 220963, 110816 )
	add_way_point( 3030, 221083, 110853 )
	add_way_point( 3030, 221205, 110991 )
	add_way_point( 3030, 221332, 111131 )
	add_way_point( 3030, 221392, 111277 )
	add_way_point( 3030, 221514, 111151 )
	
	set_way_point_type( 3031, 1 )
	add_way_point( 3031, 218499, 109257 )
	add_way_point( 3031, 218657, 109253 )
	add_way_point( 3031, 218725, 109174 )
	add_way_point( 3031, 218730, 109094 )
	add_way_point( 3031, 218588, 109072 )
	add_way_point( 3031, 218489, 109096 )
	add_way_point( 3031, 218493, 109196 )
	add_way_point( 3031, 218497, 109240 )
	
	set_way_point_type( 3032, 1 )
	add_way_point( 3032, 218973, 109873 )
	add_way_point( 3032, 218814, 109969 )
	add_way_point( 3032, 218600, 109989 )
	add_way_point( 3032, 218440, 109889 )
	add_way_point( 3032, 218370, 109769 )
	add_way_point( 3032, 218329, 109653 )
	add_way_point( 3032, 218337, 109506 )
	add_way_point( 3032, 218410, 109400 )
	add_way_point( 3032, 218529, 109230 )
	add_way_point( 3032, 218626, 109149 )
	add_way_point( 3032, 218812, 109123 )
	add_way_point( 3032, 219028, 109105 )
	add_way_point( 3032, 219151, 109126 )
	add_way_point( 3032, 219207, 109125 )
	add_way_point( 3032, 219289, 109125 )
	add_way_point( 3032, 219410, 109115 )
	add_way_point( 3032, 219489, 109069 )
	add_way_point( 3032, 219570, 109003 )
	add_way_point( 3032, 219599, 108887 )
	add_way_point( 3032, 219614, 108785 )
	add_way_point( 3032, 219590, 108642 )
	add_way_point( 3032, 219524, 108545 )
	add_way_point( 3032, 219366, 108445 )
	add_way_point( 3032, 219216, 108378 )
	
	set_way_point_type( 3033, 1 )
	add_way_point( 3033, 218485, 107136 )
	add_way_point( 3033, 218338, 106908 )
	add_way_point( 3033, 218542, 106706 )
	add_way_point( 3033, 218693, 106896 )
	add_way_point( 3033, 218495, 107136 )
	
	set_way_point_type( 3034, 1 )
	add_way_point( 3034, 218498, 107024 )
	add_way_point( 3034, 218445, 107455 )
	add_way_point( 3034, 218680, 107514 )
	add_way_point( 3034, 218961, 107483 )
	add_way_point( 3034, 219114, 107364 )
	add_way_point( 3034, 219283, 107228 )
	add_way_point( 3034, 219443, 107260 )
	add_way_point( 3034, 219602, 107211 )
	add_way_point( 3034, 219689, 107140 )
	add_way_point( 3034, 219411, 106938 )
	add_way_point( 3034, 219293, 107216 )
	
	set_way_point_type( 3035, 1 )
	add_way_point( 3035, 218420, 106886 )
	add_way_point( 3035, 218603, 106680 )
	add_way_point( 3035, 218729, 106559 )
	add_way_point( 3035, 218802, 106351 )
	add_way_point( 3035, 218808, 106260 )
	add_way_point( 3035, 218685, 106201 )
	add_way_point( 3035, 218513, 106149 )
	add_way_point( 3035, 218347, 106091 )
	
	set_way_point_type( 3036, 1 )
	add_way_point( 3036, 218607, 107062 )
	add_way_point( 3036, 218636, 106813 )
	add_way_point( 3036, 218674, 106638 )
	add_way_point( 3036, 218788, 106479 )
	add_way_point( 3036, 218835, 106314 )
	add_way_point( 3036, 218876, 106286 )
	add_way_point( 3036, 219018, 106273 )
	add_way_point( 3036, 219129, 106210 )
	add_way_point( 3036, 219179, 106114 )
	add_way_point( 3036, 219195, 105978 )
	add_way_point( 3036, 219201, 105798 )
	add_way_point( 3036, 219233, 105686 )
	add_way_point( 3036, 219315, 105595 )
	add_way_point( 3036, 219377, 105505 )
	add_way_point( 3036, 219449, 105434 )
	add_way_point( 3036, 219531, 105334 )
	add_way_point( 3036, 219542, 105219 )
	add_way_point( 3036, 219485, 105089 )
	add_way_point( 3036, 219392, 104959 )
	add_way_point( 3036, 219245, 104879 )
	add_way_point( 3036, 219089, 104897 )
	add_way_point( 3036, 219134, 104764 )
	add_way_point( 3036, 219290, 104824 )
	add_way_point( 3036, 219151, 104967 )
	
	set_way_point_type( 3037, 1 )
	add_way_point( 3037, 219585, 105294 )
	add_way_point( 3037, 219760, 105383 )
	add_way_point( 3037, 219893, 105478 )
	add_way_point( 3037, 219998, 105562 )
	add_way_point( 3037, 220057, 105703 )
	add_way_point( 3037, 220127, 105861 )
	add_way_point( 3037, 220226, 105900 )
	add_way_point( 3037, 220302, 105888 )
	add_way_point( 3037, 220362, 105795 )
	add_way_point( 3037, 220427, 105737 )
	add_way_point( 3037, 220447, 105594 )
	add_way_point( 3037, 220476, 105449 )
	add_way_point( 3037, 220500, 105319 )
	add_way_point( 3037, 220585, 105222 )
	add_way_point( 3037, 220677, 105163 )
	add_way_point( 3037, 220738, 105140 )
	
	
	respawn_rare_mob(-1,40000,223680,106406,9070010,1,0,3001)
	respawn_rare_mob(-1,40000,224177,106979,9070011,1,0,3002)
	respawn_rare_mob(-1,40000,223578,105738,9068001,1,0,3003)
	respawn_rare_mob(-1,40000,223704,106750,9068002,1,0,3004)
	respawn_rare_mob(-1,40000,224754,105824,9068003,1,0,3005)
	respawn_rare_mob(-1,40000,223359,106375,9067001,1,0,3006)
	respawn_rare_mob(-1,40000,222093,106085,9066001,1,0,3007)
	respawn_rare_mob(-1,40000,222050,106058,9066002,1,0,3008)
	respawn_rare_mob(-1,40000,220724,105108,9066003,1,0,3009)
	respawn_rare_mob(-1,40000,220921,106230,9065002,1,0,3010)
	respawn_rare_mob(-1,40000,221159,105602,9065003,1,0,3011)
	respawn_rare_mob(-1,40000,223187,108133,9065004,1,0,3012)
	respawn_rare_mob(-1,40000,223243,108155,9064007,1,0,3013)
	respawn_rare_mob(-1,40000,223204,108190,9063008,1,0,3014)
	respawn_rare_mob(-1,40000,223617,108998,9063009,1,0,3015)
	respawn_rare_mob(-1,40000,223267,109081,9063010,1,0,3016)
	respawn_rare_mob(-1,40000,224117,110774,9063011,1,0,3017)
	respawn_rare_mob(-1,40000,223981,110985,9064001,1,0,3018)
	respawn_rare_mob(-1,40000,222517,110131,9063003,1,0,3019)
	respawn_rare_mob(-1,40000,221075,111079,9062013,1,0,3020)
	respawn_rare_mob(-1,40000,220735,112085,9062007,1,0,3021)
	respawn_rare_mob(-1,40000,220740,111977,9062008,1,0,3022)
	respawn_rare_mob(-1,40000,219976,108017,9062009,1,0,3023)
	respawn_rare_mob(-1,40000,221795,109835,9062010,1,0,3024)
	respawn_rare_mob(-1,40000,221427,108752,9062011,1,0,3025)
	respawn_rare_mob(-1,40000,221937,108805,9061003,1,0,3026)
	respawn_rare_mob(-1,40000,221850,108704,9061004,1,0,3027)
	respawn_rare_mob(-1,40000,219731,110233,9061005,1,0,3028)
	respawn_rare_mob(-1,40000,219621,110016,9059011,1,0,3030)
	respawn_rare_mob(-1,40000,218499,109257,9059012,1,0,3031)
	respawn_rare_mob(-1,40000,218973,109873,9059013,1,0,3032)
	respawn_rare_mob(-1,40000,218485,107136,9060001,1,0,3033)
	respawn_rare_mob(-1,40000,218498,107024,9060002,1,0,3034)
	respawn_rare_mob(-1,40000,218420,106886,9059003,1,0,3035)
	respawn_rare_mob(-1,40000,218607,107062,9051001,1,0,3036)
	respawn_rare_mob(-1,40000,219585,105294,9051001,1,0,3037)


	-- 잃어 버린 갱도 1 --
	
	set_way_point_type(3501,1)
	add_way_point(3501,223680,9638)
	add_way_point(3501,223701,9941)
	add_way_point(3501,223799,10095)
	add_way_point(3501,224021,10162)
	add_way_point(3501,224252,10148)
	add_way_point(3501,224464,10129)
	add_way_point(3501,224604,10003)
	add_way_point(3501,224666,9769)
	add_way_point(3501,224643,9533)
	add_way_point(3501,224615,9376)
	add_way_point(3501,224523,9315)
	add_way_point(3501,224392,9252)
	add_way_point(3501,224065,9200)
	add_way_point(3501,223841,9187)
	add_way_point(3501,223734,9255)
	add_way_point(3501,223678,9454)
	add_way_point(3501,223680,9638)
	
	set_way_point_type(3502,1)
	add_way_point(3502,224177,10211)
	add_way_point(3502,224395,10193)
	add_way_point(3502,224560,10120)
	add_way_point(3502,224660,9996)
	add_way_point(3502,224667,9822)
	add_way_point(3502,224708,9668)
	add_way_point(3502,224700,9570)
	add_way_point(3502,224675,9465)
	add_way_point(3502,224631,9373)
	add_way_point(3502,224546,9324)
	add_way_point(3502,224454,9285)
	add_way_point(3502,224317,9223)
	add_way_point(3502,224036,9177)
	add_way_point(3502,223861,9164)
	add_way_point(3502,223725,9197)
	add_way_point(3502,223649,9298)
	add_way_point(3502,223635,9438)
	add_way_point(3502,223648,9549)
	add_way_point(3502,223663,9645)
	add_way_point(3502,223666,9730)
	add_way_point(3502,223672,9844)
	add_way_point(3502,223687,9924)
	add_way_point(3502,223717,9992)
	add_way_point(3502,223759,10055)
	add_way_point(3502,223825,10091)
	add_way_point(3502,223925,10135)
	add_way_point(3502,224011,10176)
	add_way_point(3502,224173,10214)
	
	set_way_point_type(3503,1)
	add_way_point(3503,223578,8970)
	add_way_point(3503,223525,9053)
	add_way_point(3503,223530,9159)
	add_way_point(3503,223542,9285)
	add_way_point(3503,223554,9360)
	add_way_point(3503,223587,9416)
	add_way_point(3503,223626,9435)
	add_way_point(3503,223691,9379)
	add_way_point(3503,223750,9307)
	add_way_point(3503,223796,9274)
	add_way_point(3503,223857,9261)
	add_way_point(3503,223916,9274)
	add_way_point(3503,224007,9313)
	add_way_point(3503,224075,9303)
	add_way_point(3503,224146,9293)
	add_way_point(3503,224256,9274)
	add_way_point(3503,224321,9271)
	add_way_point(3503,224392,9262)
	add_way_point(3503,224480,9275)
	add_way_point(3503,224559,9328)
	add_way_point(3503,224628,9373)
	add_way_point(3503,224704,9435)
	add_way_point(3503,224783,9478)
	add_way_point(3503,224839,9533)
	add_way_point(3503,224863,9579)
	add_way_point(3503,224891,9642)
	add_way_point(3503,224924,9684)
	add_way_point(3503,224944,9729)
	add_way_point(3503,224939,9786)
	add_way_point(3503,224876,9891)
	add_way_point(3503,224711,10028)
	add_way_point(3503,224590,10150)
	add_way_point(3503,224512,10237)
	add_way_point(3503,224436,10271)
	add_way_point(3503,224287,10282)
	add_way_point(3503,224144,10262)
	add_way_point(3503,223994,10220)
	add_way_point(3503,223908,10163)
	add_way_point(3503,223743,10058)
	add_way_point(3503,223720,9985)
	add_way_point(3503,223698,9904)
	add_way_point(3503,223692,9754)
	add_way_point(3503,223673,9658)
	add_way_point(3503,223648,9507)
	add_way_point(3503,223662,9412)
	add_way_point(3503,223724,9320)
	add_way_point(3503,223780,9274)
	add_way_point(3503,223854,9235)
	add_way_point(3503,223922,9193)
	add_way_point(3503,224061,9194)
	add_way_point(3503,224149,9149)
	add_way_point(3503,224149,9120)
	add_way_point(3503,224103,9101)
	add_way_point(3503,224029,9078)
	add_way_point(3503,223981,9036)
	add_way_point(3503,223926,8995)
	add_way_point(3503,223858,8960)
	add_way_point(3503,223794,8925)
	add_way_point(3503,223720,8937)
	add_way_point(3503,223613,8954)
	add_way_point(3503,223584,8979)
	
	set_way_point_type(3504,1)
	add_way_point(3504,223704,9982)
	add_way_point(3504,223670,9661)
	add_way_point(3504,223536,9599)
	add_way_point(3504,223511,9449)
	add_way_point(3504,223535,9328)
	add_way_point(3504,223497,9230)
	add_way_point(3504,223502,9121)
	add_way_point(3504,223500,9018)
	add_way_point(3504,223555,8938)
	add_way_point(3504,223659,8915)
	add_way_point(3504,223739,8915)
	add_way_point(3504,223800,8953)
	add_way_point(3504,223913,9027)
	add_way_point(3504,224081,9110)
	add_way_point(3504,224194,9155)
	add_way_point(3504,224304,9229)
	add_way_point(3504,224435,9268)
	add_way_point(3504,224543,9311)
	add_way_point(3504,224677,9364)
	add_way_point(3504,224792,9448)
	add_way_point(3504,224875,9552)
	add_way_point(3504,224911,9658)
	add_way_point(3504,224883,9793)
	add_way_point(3504,224798,9901)
	add_way_point(3504,224577,10099)
	add_way_point(3504,224426,10227)
	add_way_point(3504,224160,10269)
	add_way_point(3504,223911,10207)
	add_way_point(3504,223792,10085)
	add_way_point(3504,223713,10032)
	add_way_point(3504,223714,9955)
	add_way_point(3504,223682,9804)
	add_way_point(3504,223663,9654)
	add_way_point(3504,223553,9471)
	add_way_point(3504,223506,9344)
	add_way_point(3504,223524,9201)
	add_way_point(3504,223580,9059)
	add_way_point(3504,223648,9021)
	add_way_point(3504,223800,9027)
	add_way_point(3504,223916,9081)
	add_way_point(3504,224016,9139)
	
	set_way_point_type(3505,1)
	add_way_point(3505,224754,9056)
	add_way_point(3505,224593,9265)
	add_way_point(3505,224751,9060)
	add_way_point(3505,224796,8848)
	
	set_way_point_type(3506,1)
	add_way_point(3506,223359,9607)
	add_way_point(3506,223169,9651)
	add_way_point(3506,223039,9642)
	add_way_point(3506,223012,9541)
	add_way_point(3506,222975,9406)
	add_way_point(3506,222948,9276)
	add_way_point(3506,222925,9143)
	add_way_point(3506,222895,9035)
	add_way_point(3506,222879,8933)
	add_way_point(3506,222843,8818)
	add_way_point(3506,222788,8714)
	add_way_point(3506,222726,8678)
	add_way_point(3506,222641,8718)
	add_way_point(3506,222548,8824)
	add_way_point(3506,222508,8927)
	add_way_point(3506,222458,9007)
	add_way_point(3506,222404,9083)
	add_way_point(3506,222308,9153)
	add_way_point(3506,222271,9167)
	add_way_point(3506,222247,9284)
	add_way_point(3506,222130,9307)
	add_way_point(3506,222042,9253)
	add_way_point(3506,222041,9124)
	add_way_point(3506,222095,9050)
	add_way_point(3506,222237,9066)
	add_way_point(3506,222265,9163)
	
	set_way_point_type(3507,1)
	add_way_point(3507,222093,9317)
	add_way_point(3507,222001,9253)
	add_way_point(3507,221895,9195)
	add_way_point(3507,221847,9128)
	add_way_point(3507,221874,9019)
	add_way_point(3507,221943,8955)
	add_way_point(3507,222039,8974)
	add_way_point(3507,222161,8989)
	add_way_point(3507,222276,9033)
	add_way_point(3507,222371,9092)
	add_way_point(3507,222411,9149)
	add_way_point(3507,222391,9206)
	add_way_point(3507,222308,9280)
	add_way_point(3507,222239,9330)
	add_way_point(3507,222076,9318)
	
	set_way_point_type(3508,1)
	add_way_point(3508,222050,9290)
	add_way_point(3508,222017,9213)
	add_way_point(3508,222028,9099)
	add_way_point(3508,222055,9042)
	add_way_point(3508,222141,9041)
	add_way_point(3508,222272,9040)
	add_way_point(3508,222299,9119)
	add_way_point(3508,222299,9220)
	add_way_point(3508,222245,9290)
	add_way_point(3508,222109,9314)
	add_way_point(3508,222004,9216)
	add_way_point(3508,221965,9106)
	add_way_point(3508,221980,8978)
	add_way_point(3508,222007,8855)
	add_way_point(3508,222038,8735)
	add_way_point(3508,222074,8616)
	add_way_point(3508,222088,8534)
	add_way_point(3508,222075,8470)
	add_way_point(3508,222018,8356)
	add_way_point(3508,221957,8269)
	add_way_point(3508,221906,8224)
	add_way_point(3508,221806,8215)
	add_way_point(3508,221731,8227)
	add_way_point(3508,221653,8282)
	add_way_point(3508,221589,8402)
	add_way_point(3508,221510,8486)
	add_way_point(3508,221423,8597)
	add_way_point(3508,221333,8676)
	add_way_point(3508,221227,8754)
	add_way_point(3508,221141,8804)
	
	set_way_point_type(3509,1)
	add_way_point(3509,220724,8340)
	add_way_point(3509,220697,8181)
	add_way_point(3509,220623,8075)
	add_way_point(3509,220605,7937)
	add_way_point(3509,220683,7958)
	add_way_point(3509,220718,7976)
	add_way_point(3509,220762,7909)
	add_way_point(3509,220822,7930)
	add_way_point(3509,220846,7983)
	add_way_point(3509,220884,8019)
	add_way_point(3509,220823,8110)
	add_way_point(3509,220808,8174)
	add_way_point(3509,220764,8276)
	add_way_point(3509,220724,8329)
	
	set_way_point_type(3510,1)
	add_way_point(3510,220921,9462)
	add_way_point(3510,220939,9334)
	add_way_point(3510,221000,9191)
	add_way_point(3510,221047,9064)
	add_way_point(3510,221083,8945)
	add_way_point(3510,221101,8849)
	add_way_point(3510,221036,8776)
	add_way_point(3510,220960,8695)
	add_way_point(3510,220895,8577)
	add_way_point(3510,220838,8512)
	add_way_point(3510,220774,8405)
	add_way_point(3510,220734,8317)
	add_way_point(3510,220730,8200)
	add_way_point(3510,220734,8126)
	add_way_point(3510,220741,8062)
	
	set_way_point_type(3511,1)
	add_way_point(3511,221159,8834)
	add_way_point(3511,220986,9198)
	add_way_point(3511,220917,9390)
	add_way_point(3511,220928,9541)
	add_way_point(3511,221013,9607)
	add_way_point(3511,221113,9679)
	add_way_point(3511,221205,9716)
	add_way_point(3511,221294,9707)
	add_way_point(3511,221371,9673)
	add_way_point(3511,221431,9693)
	add_way_point(3511,221505,9754)
	add_way_point(3511,221558,9854)
	add_way_point(3511,221611,9929)
	add_way_point(3511,221690,9990)
	add_way_point(3511,221784,10029)
	add_way_point(3511,221900,10073)
	add_way_point(3511,221959,10054)
	add_way_point(3511,222010,9999)
	add_way_point(3511,222050,9964)
	add_way_point(3511,222191,10100)
	
	set_way_point_type(3512,1)
	add_way_point(3512,223187,11365)
	add_way_point(3512,223100,11459)
	add_way_point(3512,223029,11532)
	add_way_point(3512,223082,11690)
	add_way_point(3512,223130,11771)
	add_way_point(3512,223154,11856)
	add_way_point(3512,223243,11953)
	add_way_point(3512,223322,12069)
	add_way_point(3512,223382,12166)
	add_way_point(3512,223345,12297)
	add_way_point(3512,223371,12441)
	add_way_point(3512,223504,12504)
	add_way_point(3512,223609,12449)
	add_way_point(3512,223627,12325)
	add_way_point(3512,223584,12226)
	add_way_point(3512,223491,12159)
	add_way_point(3512,223404,12176)
	
	set_way_point_type(3513,1)
	add_way_point(3513,223243,11387)
	add_way_point(3513,223344,11435)
	add_way_point(3513,223437,11476)
	add_way_point(3513,223551,11404)
	add_way_point(3513,223611,11337)
	add_way_point(3513,223714,11250)
	add_way_point(3513,223787,11211)
	add_way_point(3513,223873,11168)
	add_way_point(3513,223964,11091)
	add_way_point(3513,224060,11044)
	add_way_point(3513,224221,11046)
	add_way_point(3513,224337,11077)
	add_way_point(3513,224453,11115)
	add_way_point(3513,224571,11075)
	add_way_point(3513,224662,11025)
	add_way_point(3513,224753,10964)
	add_way_point(3513,224867,10865)
	add_way_point(3513,224973,10772)
	add_way_point(3513,225056,10685)
	add_way_point(3513,225122,10621)
	add_way_point(3513,225242,10634)
	add_way_point(3513,225286,10731)
	add_way_point(3513,225262,10781)
	add_way_point(3513,225177,10718)
	add_way_point(3513,225113,10736)
	
	set_way_point_type(3514,1)
	add_way_point(3514,223204,11422)
	add_way_point(3514,223192,11257)
	add_way_point(3514,223059,11081)
	add_way_point(3514,223213,11050)
	add_way_point(3514,223107,10967)
	add_way_point(3514,223048,10981)
	add_way_point(3514,223026,11027)
	add_way_point(3514,223174,11094)
	add_way_point(3514,223135,10879)
	add_way_point(3514,223127,10738)
	add_way_point(3514,223106,10608)
	add_way_point(3514,223066,10508)
	add_way_point(3514,222981,10438)
	add_way_point(3514,222846,10394)
	add_way_point(3514,222679,10369)
	add_way_point(3514,222525,10332)
	add_way_point(3514,222365,10241)
	add_way_point(3514,222324,10166)
	add_way_point(3514,222235,10018)
	add_way_point(3514,222162,9963)
	add_way_point(3514,222032,9967)
	add_way_point(3514,222091,10090)
	add_way_point(3514,222247,10135)
	
	set_way_point_type(3515,1)
	add_way_point(3515,223617,12230)
	add_way_point(3515,223447,12223)
	add_way_point(3515,223338,12336)
	add_way_point(3515,223419,12476)
	add_way_point(3515,223562,12464)
	add_way_point(3515,223634,12345)
	add_way_point(3515,223661,12217)
	add_way_point(3515,223749,12185)
	add_way_point(3515,223816,12123)
	add_way_point(3515,223879,12029)
	add_way_point(3515,223926,11959)
	add_way_point(3515,223996,11911)
	add_way_point(3515,224082,11866)
	add_way_point(3515,224182,11830)
	add_way_point(3515,224305,11785)
	add_way_point(3515,224398,11754)
	add_way_point(3515,224477,11729)
	add_way_point(3515,224566,11707)
	add_way_point(3515,224663,11690)
	add_way_point(3515,224624,11616)
	
	set_way_point_type(3516,1)
	add_way_point(3516,223267,12313)
	add_way_point(3516,223353,12206)
	add_way_point(3516,223439,12167)
	add_way_point(3516,223616,12192)
	add_way_point(3516,223770,12189)
	add_way_point(3516,223875,12167)
	add_way_point(3516,223940,12179)
	add_way_point(3516,224038,12196)
	add_way_point(3516,224158,12245)
	add_way_point(3516,224261,12295)
	add_way_point(3516,224411,12344)
	add_way_point(3516,224547,12466)
	add_way_point(3516,224662,12552)
	add_way_point(3516,224794,12636)
	add_way_point(3516,224879,12761)
	add_way_point(3516,224969,12880)
	add_way_point(3516,225025,13030)
	add_way_point(3516,225063,13230)
	add_way_point(3516,225049,13464)
	
	set_way_point_type(3517,1)
	add_way_point(3517,224117,14006)
	add_way_point(3517,223980,14023)
	add_way_point(3517,223981,14186)
	add_way_point(3517,224151,14226)
	add_way_point(3517,224294,14250)
	add_way_point(3517,224491,14166)
	add_way_point(3517,224605,14043)
	add_way_point(3517,224711,13931)
	add_way_point(3517,224829,13831)
	add_way_point(3517,224928,13676)
	add_way_point(3517,224987,13445)
	add_way_point(3517,225071,13508)
	add_way_point(3517,225108,13436)
	add_way_point(3517,225049,13244)
	
	set_way_point_type(3518,1)
	add_way_point(3518,223981,14217)
	add_way_point(3518,224148,14137)
	add_way_point(3518,224167,14073)
	add_way_point(3518,224125,13982)
	add_way_point(3518,224143,13871)
	add_way_point(3518,224145,13801)
	add_way_point(3518,224191,13717)
	add_way_point(3518,224245,13617)
	add_way_point(3518,224273,13517)
	add_way_point(3518,224280,13389)
	add_way_point(3518,224284,13287)
	add_way_point(3518,224250,13221)
	add_way_point(3518,224196,13164)
	add_way_point(3518,224106,13144)
	add_way_point(3518,223981,13177)
	add_way_point(3518,223857,13263)
	add_way_point(3518,223765,13356)
	add_way_point(3518,223653,13458)
	add_way_point(3518,223514,13606)
	add_way_point(3518,223410,13618)
	add_way_point(3518,223333,13547)
	add_way_point(3518,223266,13491)
	add_way_point(3518,223242,13424)
	add_way_point(3518,223209,13325)
	add_way_point(3518,223190,13234)
	add_way_point(3518,223246,13141)
	add_way_point(3518,223316,13080)
	add_way_point(3518,223349,12970)
	add_way_point(3518,223348,12897)
	add_way_point(3518,223407,12874)
	add_way_point(3518,223424,12816)
	add_way_point(3518,223445,12747)
	add_way_point(3518,223447,12682)
	add_way_point(3518,223442,12590)
	add_way_point(3518,223464,12449)
	
	set_way_point_type(3519,1)
	add_way_point(3519,222517,13363)
	add_way_point(3519,222640,13377)
	add_way_point(3519,222758,13447)
	add_way_point(3519,222818,13406)
	add_way_point(3519,222927,13386)
	add_way_point(3519,222984,13351)
	add_way_point(3519,223036,13328)
	add_way_point(3519,223097,13316)
	add_way_point(3519,223168,13301)
	add_way_point(3519,223148,13194)
	add_way_point(3519,223135,13151)
	add_way_point(3519,223043,13162)
	add_way_point(3519,222941,13154)
	add_way_point(3519,222871,13135)
	add_way_point(3519,222810,13186)
	add_way_point(3519,222716,13140)
	add_way_point(3519,222638,13160)
	add_way_point(3519,222616,13258)
	add_way_point(3519,222540,13298)
	add_way_point(3519,222514,13364)
	
	set_way_point_type(3520,1)
	add_way_point(3520,221075,14311)
	add_way_point(3520,221054,14093)
	add_way_point(3520,221271,14096)
	add_way_point(3520,221428,14274)
	add_way_point(3520,221569,14302)
	add_way_point(3520,221692,14251)
	add_way_point(3520,221864,14141)
	add_way_point(3520,222005,14072)
	add_way_point(3520,222096,13974)
	add_way_point(3520,222145,13884)
	add_way_point(3520,222217,13788)
	add_way_point(3520,222237,13657)
	add_way_point(3520,222265,13557)
	add_way_point(3520,222306,13421)
	add_way_point(3520,222379,13404)
	add_way_point(3520,222489,13366)
	add_way_point(3520,222621,13323)
	add_way_point(3520,222790,13287)
	add_way_point(3520,222869,13266)
	add_way_point(3520,223075,13241)
	
	set_way_point_type(3521,1)
	add_way_point(3521,220735,15317)
	add_way_point(3521,220612,15309)
	add_way_point(3521,220551,15241)
	add_way_point(3521,220619,15167)
	add_way_point(3521,220705,15055)
	add_way_point(3521,220789,15087)
	add_way_point(3521,220989,15165)
	add_way_point(3521,220805,15220)
	add_way_point(3521,220742,15290)
	add_way_point(3521,220643,15258)
	add_way_point(3521,220733,15158)
	
	set_way_point_type(3522,1)
	add_way_point(3522,220740,15209)
	add_way_point(3522,220886,15187)
	add_way_point(3522,221031,15170)
	add_way_point(3522,221137,15187)
	add_way_point(3522,221261,15206)
	add_way_point(3522,221385,15204)
	add_way_point(3522,221479,15161)
	add_way_point(3522,221519,15074)
	add_way_point(3522,221519,14974)
	add_way_point(3522,221502,14874)
	add_way_point(3522,221450,14753)
	add_way_point(3522,221428,14626)
	add_way_point(3522,221444,14454)
	add_way_point(3522,221425,14369)
	add_way_point(3522,221338,14268)
	add_way_point(3522,221259,14172)
	add_way_point(3522,221172,14150)
	add_way_point(3522,221122,14245)
	add_way_point(3522,221179,14311)
	add_way_point(3522,221285,14327)
	
	set_way_point_type(3523,1)
	add_way_point(3523,219976,11249)
	add_way_point(3523,220034,11141)
	add_way_point(3523,220199,11156)
	add_way_point(3523,220263,11341)
	add_way_point(3523,220457,11406)
	add_way_point(3523,220697,11500)
	add_way_point(3523,220870,11638)
	add_way_point(3523,220797,12000)
	add_way_point(3523,220729,12299)
	add_way_point(3523,220676,12493)
	add_way_point(3523,220548,12588)
	
	set_way_point_type(3524,1)
	add_way_point(3524,221795,13067)
	add_way_point(3524,221899,12905)
	add_way_point(3524,221684,12883)
	add_way_point(3524,221529,12776)
	add_way_point(3524,221445,12719)
	add_way_point(3524,221387,12716)
	add_way_point(3524,221347,12667)
	add_way_point(3524,221364,12577)
	add_way_point(3524,221400,12405)
	add_way_point(3524,221456,12140)
	add_way_point(3524,221497,11956)
	add_way_point(3524,221652,11958)
	add_way_point(3524,221788,11997)
	add_way_point(3524,221924,12126)
	add_way_point(3524,222035,12051)
	add_way_point(3524,222001,11837)
	add_way_point(3524,221948,11704)
	add_way_point(3524,222045,11575)
	
	set_way_point_type(3525,1)
	add_way_point(3525,221427,11984)
	add_way_point(3525,220908,11747)
	add_way_point(3525,220617,11389)
	add_way_point(3525,220498,10973)
	add_way_point(3525,220610,10850)
	add_way_point(3525,220874,10930)
	add_way_point(3525,221159,11032)
	add_way_point(3525,221288,10913)
	add_way_point(3525,221549,10928)
	add_way_point(3525,221423,11268)
	add_way_point(3525,221511,11487)
	add_way_point(3525,221737,11574)
	add_way_point(3525,222040,11577)
	
	set_way_point_type(3526,1)
	add_way_point(3526,221937,12037)
	add_way_point(3526,221319,11820)
	add_way_point(3526,221027,11691)
	add_way_point(3526,220795,11374)
	add_way_point(3526,220647,11095)
	add_way_point(3526,220731,11002)
	add_way_point(3526,221157,11138)
	add_way_point(3526,221287,11405)
	add_way_point(3526,221525,11607)
	add_way_point(3526,221783,11678)
	add_way_point(3526,221955,11882)
	add_way_point(3526,221939,12026)
	
	set_way_point_type(3527,1)
	add_way_point(3527,221850,11936)
	add_way_point(3527,221280,11693)
	add_way_point(3527,220961,11464)
	add_way_point(3527,220824,11178)
	add_way_point(3527,221074,11212)
	add_way_point(3527,221246,11492)
	add_way_point(3527,221470,11665)
	add_way_point(3527,221798,11814)
	add_way_point(3527,221850,11933)
	
	set_way_point_type(3528,1)
	add_way_point(3528,219731,13465)
	add_way_point(3528,219619,13520)
	add_way_point(3528,219593,13374)
	add_way_point(3528,219590,13187)
	add_way_point(3528,219683,13137)
	add_way_point(3528,219806,13161)
	add_way_point(3528,219912,13244)
	add_way_point(3528,219955,13357)
	add_way_point(3528,219935,13459)
	add_way_point(3528,219821,13487)
	add_way_point(3528,219762,13475)
	add_way_point(3528,219742,13466)
	
	set_way_point_type(3530,1)
	add_way_point(3530,219621,13248)
	add_way_point(3530,219678,13386)
	add_way_point(3530,219815,13409)
	add_way_point(3530,219914,13372)
	add_way_point(3530,220124,13462)
	add_way_point(3530,220278,13516)
	add_way_point(3530,220359,13651)
	add_way_point(3530,220505,13836)
	add_way_point(3530,220689,13984)
	add_way_point(3530,220823,14035)
	add_way_point(3530,220963,14048)
	add_way_point(3530,221083,14085)
	add_way_point(3530,221205,14223)
	add_way_point(3530,221332,14363)
	add_way_point(3530,221392,14509)
	add_way_point(3530,221514,14383)
	
	set_way_point_type(3531,1)
	add_way_point(3531,218499,12489)
	add_way_point(3531,218657,12485)
	add_way_point(3531,218725,12406)
	add_way_point(3531,218730,12326)
	add_way_point(3531,218588,12304)
	add_way_point(3531,218489,12328)
	add_way_point(3531,218493,12428)
	add_way_point(3531,218497,12472)
	
	set_way_point_type(3532,1)
	add_way_point(3532,218973,13105)
	add_way_point(3532,218814,13201)
	add_way_point(3532,218600,13221)
	add_way_point(3532,218440,13121)
	add_way_point(3532,218370,13001)
	add_way_point(3532,218329,12885)
	add_way_point(3532,218337,12738)
	add_way_point(3532,218410,12632)
	add_way_point(3532,218529,12462)
	add_way_point(3532,218626,12381)
	add_way_point(3532,218812,12355)
	add_way_point(3532,219028,12337)
	add_way_point(3532,219151,12358)
	add_way_point(3532,219207,12357)
	add_way_point(3532,219289,12357)
	add_way_point(3532,219410,12347)
	add_way_point(3532,219489,12301)
	add_way_point(3532,219570,12235)
	add_way_point(3532,219599,12119)
	add_way_point(3532,219614,12017)
	add_way_point(3532,219590,11874)
	add_way_point(3532,219524,11777)
	add_way_point(3532,219366,11677)
	add_way_point(3532,219216,11610)
	
	set_way_point_type(3533,1)
	add_way_point(3533,218485,10368)
	add_way_point(3533,218338,10140)
	add_way_point(3533,218542,9938)
	add_way_point(3533,218693,10128)
	add_way_point(3533,218495,10368)
	
	set_way_point_type(3534,1)
	add_way_point(3534,218498,10256)
	add_way_point(3534,218445,10687)
	add_way_point(3534,218680,10746)
	add_way_point(3534,218961,10715)
	add_way_point(3534,219114,10596)
	add_way_point(3534,219283,10460)
	add_way_point(3534,219443,10492)
	add_way_point(3534,219602,10443)
	add_way_point(3534,219689,10372)
	add_way_point(3534,219411,10170)
	add_way_point(3534,219293,10448)
	
	set_way_point_type(3535,1)
	add_way_point(3535,218420,10118)
	add_way_point(3535,218603,9912)
	add_way_point(3535,218729,9791)
	add_way_point(3535,218802,9583)
	add_way_point(3535,218808,9492)
	add_way_point(3535,218685,9433)
	add_way_point(3535,218513,9381)
	add_way_point(3535,218347,9323)
	
	set_way_point_type(3536,1)
	add_way_point(3536,218607,10294)
	add_way_point(3536,218636,10045)
	add_way_point(3536,218674,9870)
	add_way_point(3536,218788,9711)
	add_way_point(3536,218835,9546)
	add_way_point(3536,218876,9518)
	add_way_point(3536,219018,9505)
	add_way_point(3536,219129,9442)
	add_way_point(3536,219179,9346)
	add_way_point(3536,219195,9210)
	add_way_point(3536,219201,9030)
	add_way_point(3536,219233,8918)
	add_way_point(3536,219315,8827)
	add_way_point(3536,219377,8737)
	add_way_point(3536,219449,8666)
	add_way_point(3536,219531,8566)
	add_way_point(3536,219542,8451)
	add_way_point(3536,219485,8321)
	add_way_point(3536,219392,8191)
	add_way_point(3536,219245,8111)
	add_way_point(3536,219089,8129)
	add_way_point(3536,219134,7996)
	add_way_point(3536,219290,8056)
	add_way_point(3536,219151,8199)
	
	set_way_point_type(3537,1)
	add_way_point(3537,219585,8526)
	add_way_point(3537,219760,8615)
	add_way_point(3537,219893,8710)
	add_way_point(3537,219998,8794)
	add_way_point(3537,220057,8935)
	add_way_point(3537,220127,9093)
	add_way_point(3537,220226,9132)
	add_way_point(3537,220302,9120)
	add_way_point(3537,220362,9027)
	add_way_point(3537,220427,8969)
	add_way_point(3537,220447,8826)
	add_way_point(3537,220476,8681)
	add_way_point(3537,220500,8551)
	add_way_point(3537,220585,8454)
	add_way_point(3537,220677,8395)
	add_way_point(3537,220738,8372)
	
	respawn_rare_mob(-1,40000,223680,9638,9070010,1,0,3501)
	respawn_rare_mob(-1,40000,224177,10211,9070011,1,0,3502)
	respawn_rare_mob(-1,40000,223578,8970,9068001,1,0,3503)
	respawn_rare_mob(-1,40000,223704,9982,9068002,1,0,3504)
	respawn_rare_mob(-1,40000,224754,9056,9068003,1,0,3505)
	respawn_rare_mob(-1,40000,223359,9607,9067001,1,0,3506)
	respawn_rare_mob(-1,40000,222093,9317,9066001,1,0,3507)
	respawn_rare_mob(-1,40000,222050,9290,9066002,1,0,3508)
	respawn_rare_mob(-1,40000,220724,8340,9066003,1,0,3509)
	respawn_rare_mob(-1,40000,220921,9462,9065002,1,0,3510)
	respawn_rare_mob(-1,40000,221159,8834,9065003,1,0,3511)
	respawn_rare_mob(-1,40000,223187,11365,9065004,1,0,3512)
	respawn_rare_mob(-1,40000,223243,11387,9064007,1,0,3513)
	respawn_rare_mob(-1,40000,223204,11422,9063008,1,0,3514)
	respawn_rare_mob(-1,40000,223617,12230,9063009,1,0,3515)
	respawn_rare_mob(-1,40000,223267,12313,9063010,1,0,3516)
	respawn_rare_mob(-1,40000,224117,14006,9063011,1,0,3517)
	respawn_rare_mob(-1,40000,223981,14217,9064001,1,0,3518)
	respawn_rare_mob(-1,40000,222517,13363,9063003,1,0,3519)
	respawn_rare_mob(-1,40000,221075,14311,9062013,1,0,3520)
	respawn_rare_mob(-1,40000,220735,15317,9062007,1,0,3521)
	respawn_rare_mob(-1,40000,220740,15209,9062008,1,0,3522)
	respawn_rare_mob(-1,40000,219976,11249,9062009,1,0,3523)
	respawn_rare_mob(-1,40000,221795,13067,9062010,1,0,3524)
	respawn_rare_mob(-1,40000,221427,11984,9062011,1,0,3525)
	respawn_rare_mob(-1,40000,221937,12037,9061003,1,0,3526)
	respawn_rare_mob(-1,40000,221850,11936,9061004,1,0,3527)
	respawn_rare_mob(-1,40000,219731,13465,9061005,1,0,3528)
	respawn_rare_mob(-1,40000,219621,13248,9059011,1,0,3530)
	respawn_rare_mob(-1,40000,218499,12489,9059012,1,0,3531)
	respawn_rare_mob(-1,40000,218973,13105,9059013,1,0,3532)
	respawn_rare_mob(-1,40000,218485,10368,9060001,1,0,3533)
	respawn_rare_mob(-1,40000,218498,10256,9060002,1,0,3534)
	respawn_rare_mob(-1,40000,218420,10118,9059003,1,0,3535)
	respawn_rare_mob(-1,40000,218607,10294,9051001,1,0,3536)
	respawn_rare_mob(-1,40000,219585,8526,9051001,1,0,3537)
	
	
	--set_way_point_type( roaming_id, roaming_type )
	--roaming_id : 로밍 아이디
	--roaming_type : 로밍 타입 1:왕복, 2:회전

	--function respawn_rare_mob( id, interval, x, y, mob_id, count, is_wandering, roaming_id )

	--id: 쓰레기 값.-_- -1을 넣어주세요. 기존 함수와 호환성 유지를 위해.
	--interval: 1/100초 단위. 1초는 100, 1분은 6000
	--x: x좌표
	--y: y좌표
	--mob_id: 몬스터 id
	--count: 숫자
	--is_wandering: 로밍 여부. 1이면 로밍. 0이면 로밍 안 함.
	--roaming_id : 로밍 아이디
		
	-- 쥬시를 따르는 동물 타미아스
	set_way_point_type(3590,2)
	add_way_point(3590,126647,102562)
	add_way_point(3590,126624,102561)
	add_way_point(3590,126597,102560)
	add_way_point(3590,126571,102531)
	add_way_point(3590,126583,102496)
	add_way_point(3590,126620,102498)
	add_way_point(3590,126651,102519)
	
	respawn_rare_mob(-1, 3000, 126647, 102562, 1000051, 1, 1, 3590)
	
	-- 쥬시를 따르는 동물 문래빗
	set_way_point_type(3591,2)
	add_way_point(3591,126624,102561)
	add_way_point(3591,126597,102560)
	add_way_point(3591,126571,102531)
	add_way_point(3591,126583,102496)
	add_way_point(3591,126620,102498)
	add_way_point(3591,126651,102519)
	
	respawn_rare_mob(-1, 3000, 126624, 102561, 1000053, 1, 1, 3591)
	
	
	-- 울프 2마리
	respawn_rare_mob(-1, 3000, 126616, 102525, 1000054, 1, 0)
	respawn_rare_mob(-1, 3000, 126620, 102523, 1000054, 1, 0)
	
	-- 붉은 타미아스
	respawn_rare_mob(-1, 3000, 126507, 102505, 1000052, 1, 0)
	
	-- 블루 픽시
	respawn_rare_mob(-1, 3000, 126590, 102545, 1000055, 1, 0)

	
	--respawn_roaming_mob( 그룹 로밍 ID, 리스폰 몬스터 ID, 방향 (각도), 로밍 동선과의 거리(M미터), 리스폰 주기(초) )
	
	--====================================================================================
	--마을 광신도 리젠: 론도
	--====================================================================================
	--set_way_point_type(3600,2)
	--add_way_point(3600,140381 ,102343)
	--add_way_point(3600,140486 ,102253)
	--add_way_point(3600,140347 ,102055)
	--add_way_point(3600,139893 ,101854)
	--add_way_point(3600,139058 ,101274)
	--add_way_point(3600,139176 ,102139)
	
	--respawn_rare_mob(-1, 3000, 140381, 102343, 53013, 13, 1, 3600) -- 광신 습격대
	--respawn_rare_mob(-1, 3000, 140381, 102343, 53014, 1, 1, 3600) -- 광신 습격대장
	
	--론도 남쪽
	respawn_roaming_mob( 8006, 77009, 0, 0, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8006, 73007, 0, 1, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8006, 73007, 0, 2, 60 )
	respawn_roaming_mob( 8006, 73007, 0, 3, 60 )
	respawn_roaming_mob( 8006, 73007, 0, 4, 60 )
	respawn_roaming_mob( 8006, 73005, 180, 1, 60 ) -- 광신 스격대 적색
	respawn_roaming_mob( 8006, 73005, 180, 2, 60 )
	respawn_roaming_mob( 8006, 73005, 180, 3, 60 )
	respawn_roaming_mob( 8006, 73005, 180, 4, 60 )
	
	-- 론도 북쪽
	respawn_roaming_mob( 8008, 77009, 0, 0, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8008, 73007, 0, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8008, 73007, 0, 4, 60 )
	respawn_roaming_mob( 8008, 73007, 0, 6, 60 )
	respawn_roaming_mob( 8008, 73007, 0, 8, 60 )
	respawn_roaming_mob( 8008, 73005, 180, 2, 60 ) -- 광신 스격대 적색
	respawn_roaming_mob( 8008, 73005, 180, 4, 60 )
	respawn_roaming_mob( 8008, 73005, 180, 5, 60 )
	respawn_roaming_mob( 8008, 73005, 180, 6, 60 )
	respawn_roaming_mob( 8008, 73005, 90, 2, 60 ) -- 광신 스격대 적색
	respawn_roaming_mob( 8008, 73005, 90, 4, 60 )
	respawn_roaming_mob( 8008, 73005, 90, 5, 60 )
	respawn_roaming_mob( 8008, 73005, 90, 6, 60 )
	
	-- 론도 동쪽
	respawn_roaming_mob( 8009, 77009, 0, 0, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8009, 73007, 0, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8009, 73007, 90, 2, 60 )
	respawn_roaming_mob( 8009, 73007, 180, 2, 60 )
	respawn_roaming_mob( 8009, 73007, 270, 2, 60 )
	respawn_roaming_mob( 8009, 73007, 0, 4, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8009, 73007, 90, 4, 60 )
	respawn_roaming_mob( 8009, 73007, 180, 4, 60 )
	respawn_roaming_mob( 8009, 73007, 270, 4, 60 )
	
	respawn_roaming_mob( 8009, 73006, 90, 10, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8009, 73005, 90, 12, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8009, 73005, 90, 14, 60 )
	respawn_roaming_mob( 8009, 73005, 90, 16, 60 )
	
	-- 론도 북쪽 영방향
	respawn_roaming_mob( 8015, 77009, 0, 0, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8015, 73007, 0, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8015, 73007, 90, 2, 60 )
	respawn_roaming_mob( 8015, 73007, 180, 2, 60 )
	respawn_roaming_mob( 8015, 73007, 270, 2, 60 )
	respawn_roaming_mob( 8015, 73007, 0, 4, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8015, 73007, 90, 4, 60 )
	respawn_roaming_mob( 8015, 73007, 180, 4, 60 )
	respawn_roaming_mob( 8015, 73007, 270, 4, 60 )
	
	respawn_roaming_mob( 8015, 73006, 90, 10, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8015, 73005, 90, 12, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8015, 73005, 90, 14, 60 )
	respawn_roaming_mob( 8015, 73005, 90, 16, 60 )
	
	-- 론도 남쪽 역방향
	respawn_roaming_mob( 8013, 77009, 0, 0, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8013, 73007, 0, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8013, 73007, 0, 4, 60 )
	respawn_roaming_mob( 8013, 73007, 0, 6, 60 )
	respawn_roaming_mob( 8013, 73007, 0, 8, 60 )
	respawn_roaming_mob( 8013, 73005, 180, 2, 60 ) -- 광신 스격대 적색
	respawn_roaming_mob( 8013, 73005, 180, 4, 60 )
	respawn_roaming_mob( 8013, 73005, 180, 5, 60 )
	respawn_roaming_mob( 8013, 73005, 180, 6, 60 )
	respawn_roaming_mob( 8013, 73005, 90, 2, 60 ) -- 광신 스격대 적색
	respawn_roaming_mob( 8013, 73005, 90, 4, 60 )
	respawn_roaming_mob( 8013, 73005, 90, 5, 60 )
	respawn_roaming_mob( 8013, 73005, 90, 6, 60 )
	
	-- 론도 동쪽 역방향
	respawn_roaming_mob( 8016, 77009, 0, 0, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8016, 73007, 0, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8016, 73007, 0, 4, 60 )
	respawn_roaming_mob( 8016, 73007, 0, 6, 60 )
	respawn_roaming_mob( 8016, 73007, 0, 8, 60 )
	respawn_roaming_mob( 8016, 73005, 180, 2, 60 ) -- 광신 스격대 적색
	respawn_roaming_mob( 8016, 73005, 180, 4, 60 )
	respawn_roaming_mob( 8016, 73005, 180, 5, 60 )
	respawn_roaming_mob( 8016, 73005, 180, 6, 60 )
	respawn_roaming_mob( 8016, 73005, 90, 2, 60 ) -- 광신 스격대 적색
	respawn_roaming_mob( 8016, 73005, 90, 4, 60 )
	respawn_roaming_mob( 8016, 73005, 90, 5, 60 )
	respawn_roaming_mob( 8016, 73005, 90, 6, 60 )
	
	
	
	--====================================================================================
	--마을 광신도 리젠: 호라이즌
	--====================================================================================
	--respawn_rare_mob(-1, 3000, 153389 , 78861, 53013, 11, 1, 0) -- 광신 습격대
	--respawn_rare_mob(-1, 3000, 153389 , 78861, 53014, 1, 1, 0) -- 광신 습격대장
	
	respawn_roaming_mob( 8004, 73006, 0, 2, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8004, 73008, 180, 2, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8004, 73005, 45, 2, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8004, 73005, 45, 4, 60 )
	respawn_roaming_mob( 8004, 73005, 45, 8, 60 )
	respawn_roaming_mob( 8004, 73005, 45, 10, 60 )
	respawn_roaming_mob( 8004, 73007, 135, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8004, 73007, 135, 4, 60 )
	respawn_roaming_mob( 8004, 73007, 135, 8, 60 )
	respawn_roaming_mob( 8004, 73007, 135, 10, 60 )
	
	-- 호라이즌 역방향
	respawn_roaming_mob( 8011, 73008, 0, 0, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8011, 73007, 0, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8011, 73007, 90, 2, 60 )
	respawn_roaming_mob( 8011, 73007, 180, 2, 60 )
	respawn_roaming_mob( 8011, 73007, 270, 2, 60 )
	
	respawn_roaming_mob( 8011, 73006, 90, 10, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8011, 73005, 90, 11, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8011, 73005, 90, 12, 60 )
	respawn_roaming_mob( 8011, 73005, 90, 13, 60 )
	
	--====================================================================================
	--마을 광신도 리젠: 카탄
	--====================================================================================
	--set_way_point_type(3601,2)
	--add_way_point(3601,120288 ,55275)
	--add_way_point(3601,120531 ,55097)
	--add_way_point(3601,120536 ,54841)
	--add_way_point(3601,120246 ,54982)
		
	--respawn_rare_mob(-1, 3000, 120288, 55275, 53013, 13, 1, 3601) -- 광신 습격대
	--respawn_rare_mob(-1, 3000, 120288, 55275, 53014, 1, 1, 3601) -- 광신 습격대장
	
	respawn_roaming_mob( 8003, 73008, 0, 0, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8003, 73007, 0, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8003, 73007, 90, 2, 60 )
	respawn_roaming_mob( 8003, 73007, 180, 2, 60 )
	respawn_roaming_mob( 8003, 73007, 270, 2, 60 )
	
	respawn_roaming_mob( 8003, 73006, 90, 10, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8003, 73005, 90, 11, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8003, 73005, 90, 12, 60 )
	respawn_roaming_mob( 8003, 73005, 90, 13, 60 )
	
	--카탄 역방향
	respawn_roaming_mob( 8010, 73006, 0, 0, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8010, 73005, 90, 2, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8010, 73007, 90, 4, 60 )
	respawn_roaming_mob( 8010, 73005, 90, 6, 60 )
	respawn_roaming_mob( 8010, 73007, 90, 8, 60 )
	
	--====================================================================================
	--마을 광신도 리젠: 라크시
	--====================================================================================
	--respawn_rare_mob(-1, 3000, 117206 , 138830, 53013, 11, 1, 0) -- 광신 습격대
	--respawn_rare_mob(-1, 3000, 117206 , 138830, 53014, 1, 1, 0) -- 광신 습격대장
	
	respawn_roaming_mob( 8005, 73006, 0, 0, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8005, 73005, 90, 2, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8005, 73007, 90, 4, 60 )
	respawn_roaming_mob( 8005, 73005, 90, 6, 60 )
	respawn_roaming_mob( 8005, 73007, 90, 8, 60 )
	
	-- 라크시 역방향
	respawn_roaming_mob( 8012, 73006, 0, 2, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8012, 73008, 180, 2, 60 ) -- 광신 습격대장 녹색
	respawn_roaming_mob( 8012, 73005, 45, 2, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8012, 73005, 45, 4, 60 )
	respawn_roaming_mob( 8012, 73005, 45, 8, 60 )
	respawn_roaming_mob( 8012, 73005, 45, 10, 60 )
	respawn_roaming_mob( 8012, 73007, 135, 2, 60 ) -- 광신 습격대 녹색
	respawn_roaming_mob( 8012, 73007, 135, 4, 60 )
	respawn_roaming_mob( 8012, 73007, 135, 8, 60 )
	respawn_roaming_mob( 8012, 73007, 135, 10, 60 )
	
	--====================================================================================
	--마을 광신도 리젠: 붉은농장
	--====================================================================================
	--set_way_point_type(3602,1)
	--add_way_point(3602,120770 ,104592)
	--add_way_point(3602,120807 ,105077)
	--add_way_point(3602,121390 ,105504)
	--add_way_point(3602,121891 ,105645)
	
	--set_way_point_type(3603,1)
--	add_way_point(3603,120770 ,104592)
--	add_way_point(3603,120807 ,105077)
--	add_way_point(3603,121390 ,105504)
--	add_way_point(3603,121891 ,105645)
	
--	set_way_point_type(3604,1)
--	add_way_point(3604,120807 ,105077)
--	add_way_point(3604,121390 ,105504)
--	add_way_point(3604,121891 ,105645)
--	add_way_point(3604,120770 ,104592)
	
--	set_way_point_type(3605,1)
--	add_way_point(3605,120807 ,105077)
--	add_way_point(3605,121390 ,105504)
--	add_way_point(3605,121891 ,105645)
--	add_way_point(3605,120770 ,104592)
	
--	set_way_point_type(3606,1)
--	add_way_point(3606,120770 ,104592)
--	add_way_point(3606,120807 ,105077)
--	add_way_point(3606,121390 ,105504)
--	add_way_point(3606,121891 ,105645)
	
--	set_way_point_type(3607,1)
--	add_way_point(3607,121891 ,105645)
--	add_way_point(3607,121390 ,105504)
--	add_way_point(3607,120807 ,105077)
--	add_way_point(3607,120770 ,104592)
	
	
--	respawn_rare_mob(-1, 3000, 120770, 104592, 53013, 4, 1, 3602) -- 광신 습격대
--	respawn_rare_mob(-1, 3000, 120770, 104592, 53013, 3, 1, 3603) -- 광신 습격대
--	respawn_rare_mob(-1, 3000, 120770, 104592, 53013, 4, 1, 3604) -- 광신 습격대
--	respawn_rare_mob(-1, 3000, 120770, 104592, 53013, 3, 1, 3605) -- 광신 습격대
--	respawn_rare_mob(-1, 3000, 120770, 104592, 53014, 1, 1, 3606) -- 광신 습격대장
--	respawn_rare_mob(-1, 3000, 120770, 104592, 53014, 1, 1, 3607) -- 광신 습격대장

	respawn_roaming_mob( 8001, 53014, 0, 0, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8001, 53013, 0, 5, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8001, 53013, 0, 10, 60 )
	respawn_roaming_mob( 8001, 53013, 0, 15, 60 )
	respawn_roaming_mob( 8001, 53013, 180, 5, 60 )
	respawn_roaming_mob( 8001, 53013, 180, 10, 60 )
	respawn_roaming_mob( 8001, 53013, 180, 15, 60 )
	
	respawn_roaming_mob( 8002, 53014, 0, 0, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8002, 53013, 0, 5, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8002, 53013, 0, 10, 60 )
	respawn_roaming_mob( 8002, 53013, 0, 15, 60 )
	respawn_roaming_mob( 8002, 53013, 180, 5, 60 )
	respawn_roaming_mob( 8002, 53013, 180, 10, 60 )
	respawn_roaming_mob( 8002, 53013, 180, 15, 60 )
	
	--====================================================================================
	--가짜 마녀 리젠: 마법 실험지
	--====================================================================================
	
	
	respawn_roaming_mob( 8007, 76006, 0, 0, 60 ) -- 가짜 마녀 리젠
	respawn_roaming_mob( 8007, 73006, 90, 4, 60 ) -- 광신 습격대장 적색
	respawn_roaming_mob( 8007, 73005, 180, 4, 60 ) -- 광신 습격대 적색
	respawn_roaming_mob( 8007, 73007, 270, 4, 60 )
	respawn_roaming_mob( 8007, 73005, 0, 4, 60 )
	
	
	--====================================================================================
	--결전의 마녀 리젠: 사혼의 제단
	--====================================================================================
	--set_way_point_type(3603,1)
	--add_way_point(3603,139640 ,73220)
	
	respawn_rare_mob(-1, 3000, 139640, 73220, 104007, 1, 1, 0) -- 결전의 마녀

	
	--====================================================================================
	--크리쳐 농장
	--====================================================================================
	--set_way_point_type(3603,1)
	--add_way_point(3603,139640 ,73220)

	--set_way_point_type( roaming_id, roaming_type )
	--roaming_id : 로밍 아이디
	--roaming_type : 로밍 타입 1:왕복, 2:회전

	--function respawn_rare_mob( id, interval, x, y, mob_id, count, is_wandering, roaming_id )

	--id: 쓰레기 값.-_- -1을 넣어주세요. 기존 함수와 호환성 유지를 위해.
	--interval: 1/100초 단위. 1초는 100, 1분은 6000
	--x: x좌표
	--y: y좌표
	--mob_id: 몬스터 id
	--count: 숫자
	--is_wandering: 로밍 여부. 1이면 로밍. 0이면 로밍 안 함.
	--roaming_id : 로밍 아이디
		
	-- 빙글빙글 도는 미스틱 지니
	set_way_point_type(3701,2)
	add_way_point(3701,139982,102767)
	add_way_point(3701,139923,102776)
	add_way_point(3701,139889,102863)
	add_way_point(3701,139911,102912)	
	add_way_point(3701,139966,102948)
	add_way_point(3701,140011,102916)	
	add_way_point(3701,140038,102917)		
	add_way_point(3701,140005,102763)
	
	respawn_rare_mob(-1, 3000, 139982, 102767, 1000077, 1, 1, 3701)

	-- 빙글빙글 도는 화이트드래곤
	set_way_point_type(3702,2)
	add_way_point(3702,139923,102776)
	add_way_point(3702,139889,102863)
	add_way_point(3702,139911,102912)	
	add_way_point(3702,139966,102948)
	add_way_point(3702,140011,102916)	
	add_way_point(3702,140038,102917)		
	add_way_point(3702,140005,102763)
	
	respawn_rare_mob(-1, 3000, 139923, 102776, 1000078, 1, 1, 3702)

	-- 소심한 지니
	respawn_rare_mob(-1, 3000, 140342, 103111, 1000076, 1, 0)
	
	-- 맹인 옥토퍼스
	respawn_rare_mob(-1, 3000, 140282, 103175, 1000066, 1, 0)

	-- 친절한 블루 픽시
	respawn_rare_mob(-1, 3000, 140261, 103161, 1000064, 1, 0)

	-- 섹시한 타파리
	respawn_rare_mob(-1, 3000, 139972, 102841, 1000074, 1, 0)

	-- 섹시한 타파리
	respawn_rare_mob(-1, 3000, 139961, 102859, 1000062, 1, 0)

	-- 와리가리 오크
	set_way_point_type(3703,1)
	add_way_point(3703,140315,103195)
	add_way_point(3703,140293,103199)
 
	respawn_rare_mob(-1, 3000, 140315, 103195, 1000060, 1, 1, 3703)
	
	-- 꾀많은 아이무스
	respawn_rare_mob(-1, 3000, 140080, 103019, 1000073, 1, 0)

	-- 용의주도한 엔젤
	respawn_rare_mob(-1, 3000, 140064, 103004, 1000071, 1, 0)

	-- 골목대장 샐러맨더
	respawn_rare_mob(-1, 3000, 140068, 103031, 1000056, 1, 0)
	
	-- 왕자병 켄타우로스
	respawn_rare_mob(-1, 3000, 139954, 103275, 1000072, 1, 0)

	-- 음치 윈드송
	respawn_rare_mob(-1, 3000, 140014, 103275, 1000068, 1, 0)

	-- 문지기 데스타일런트
	respawn_rare_mob(-1, 3000, 140059, 102594, 1000079, 1, 0)
	respawn_rare_mob(-1, 3000, 140144, 102588, 1000079, 1, 0)
	
	-- 울보 울프
	set_way_point_type(3705,2)
	add_way_point(3705,139875,103234)
	add_way_point(3705,139850,103094)	
	add_way_point(3705,139970,103094)	
	add_way_point(3705,139994,103206)	
 
	respawn_rare_mob(-1, 3000, 140197, 102865, 1000057, 1, 1, 3705)	

	-- 사랑스런 호크맨
	respawn_rare_mob(-1, 3000, 140008, 103118, 1000067, 1, 0)

	-- 허약한 켈베로스
	set_way_point_type(3706,1)
	add_way_point(3706,139774,103195)
	add_way_point(3706,139824,103105)
	respawn_rare_mob(-1, 3000, 139774, 103195, 1000075, 1, 1, 3706)

	-- 폭군 레드 픽시
	set_way_point_type(3707,2)

	add_way_point(3707,139949,103133)
	add_way_point(3707,139920,103136)
	add_way_point(3707,139950,103175)
	add_way_point(3707,139930,103195)
	add_way_point(3707,139900,103155)
	add_way_point(3707,139905,103179)	
	
	respawn_rare_mob(-1, 3000, 139949,103133, 1000063, 1, 1, 3707)

	-- 비관적인 다크혼

	set_way_point_type(3708,2)

	add_way_point(3708,139905,103179)	
	add_way_point(3708,139949,103133)
	add_way_point(3708,139920,103136)
	add_way_point(3708,139950,103175)
	add_way_point(3708,139930,103195)
	add_way_point(3708,139900,103155)
	
	respawn_rare_mob(-1, 3000, 139905,103179, 1000070, 1, 1, 3708)


	-- 어지러운 카벙클
	set_way_point_type(3709,2)	
	add_way_point(3709,140172,102776)
	add_way_point(3709,140173,102727)
	add_way_point(3709,140144,102713)
	add_way_point(3709,140107,102727)
	add_way_point(3709,140111,102773)
	add_way_point(3709,140145,102787)
	respawn_rare_mob(-1, 3000, 140172,102776, 1000080, 1, 1, 3709)

	-- 어지러운 카벙클
	set_way_point_type(3710,2)	
	add_way_point(3710,140173,102727)
	add_way_point(3710,140144,102713)
	add_way_point(3710,140107,102727)
	add_way_point(3710,140111,102773)
	add_way_point(3710,140145,102787)
	add_way_point(3710,140172,102776)
	respawn_rare_mob(-1, 3000, 140173,102727, 1000080, 1, 1, 3710)

	-- 어지러운 카벙클
	set_way_point_type(3711,2)	
	add_way_point(3711,140144,102713)
	add_way_point(3711,140107,102727)
	add_way_point(3711,140111,102773)
	add_way_point(3711,140145,102787)
	add_way_point(3711,140172,102776)
	add_way_point(3711,140173,102727)
	
	respawn_rare_mob(-1, 3000, 140144,102713, 1000080, 1, 1, 3711)

	-- 어지러운 카벙클
	set_way_point_type(3712,2)	
	add_way_point(3712,140107,102727)
	add_way_point(3712,140111,102773)
	add_way_point(3712,140145,102787)
	add_way_point(3712,140172,102776)
	add_way_point(3712,140173,102727)
	add_way_point(3712,140144,102713)
	respawn_rare_mob(-1, 3000, 140107,102727, 1000080, 1, 1, 3712)

	-- 어지러운 카벙클
	set_way_point_type(3713,2)	
	add_way_point(3713,140111,102773)
	add_way_point(3713,140145,102787)
	add_way_point(3713,140172,102776)
	add_way_point(3713,140173,102727)
	add_way_point(3713,140144,102713)
	add_way_point(3713,140107,102727)
	respawn_rare_mob(-1, 3000, 140111,102773, 1000080, 1, 1, 3713)

	-- 어지러운 카벙클
	set_way_point_type(3714,2)	
	add_way_point(3714,140145,102787)
	add_way_point(3714,140172,102776)
	add_way_point(3714,140173,102727)
	add_way_point(3714,140144,102713)
	add_way_point(3714,140107,102727)
	add_way_point(3714,140111,102773)
	respawn_rare_mob(-1, 3000, 140145,102787, 1000080, 1, 1, 3714)
	
	--respawn_roaming_mob( 그룹 로밍 ID, 리스폰 몬스터 ID, 방향 (각도), 로밍 동선과의 거리(M미터), 리스폰 주기(초) )

	
	-- 각 마을 그룹 로밍
    -- 호라이즌
    respawn_roaming_mob( 1001, 1000050, 180, 2, 10 )
    respawn_roaming_mob( 1001, 1000050, 60, 2, 10 )
    respawn_roaming_mob( 1001, 1000050, 300, 2, 10 )

    respawn_roaming_mob( 1002, 1000050, 180, 2, 10 )
    respawn_roaming_mob( 1002, 1000050, 60, 2, 10 )
    respawn_roaming_mob( 1002, 1000050, 300, 2, 10 )

    respawn_roaming_mob( 1003, 1000050, 180, 2, 10 )
    respawn_roaming_mob( 1003, 1000050, 60, 2, 10 )
    respawn_roaming_mob( 1003, 1000050, 300, 2, 10 )

    respawn_roaming_mob( 1004, 1000050, 180, 2, 10 )
    respawn_roaming_mob( 1004, 1000050, 60, 2, 10 )
    respawn_roaming_mob( 1004, 1000050, 300, 2, 10 )

    respawn_roaming_mob( 1005, 1000050, 180, 2, 10 )
    respawn_roaming_mob( 1005, 1000050, 60, 2, 10 )
    respawn_roaming_mob( 1005, 1000050, 300, 2, 10 )

    -- 라크시
    respawn_roaming_mob( 2001, 1000048, 180, 2, 10 )
    respawn_roaming_mob( 2001, 1000048, 60, 2, 10 )
    respawn_roaming_mob( 2001, 1000048, 300, 2, 10 )

    respawn_roaming_mob( 2002, 1000048, 180, 2, 10 )
    respawn_roaming_mob( 2002, 1000048, 60, 2, 10 )
    respawn_roaming_mob( 2002, 1000048, 300, 2, 10 )

    respawn_roaming_mob( 2003, 1000048, 180, 2, 10 )
    respawn_roaming_mob( 2003, 1000048, 60, 2, 10 )
    respawn_roaming_mob( 2003, 1000048, 300, 2, 10 )

    -- 론도
    respawn_roaming_mob( 3001, 1000047, 180, 2, 10 )
    respawn_roaming_mob( 3001, 1000047, 60, 2, 10 )
    respawn_roaming_mob( 3001, 1000047, 300, 2, 10 )

    respawn_roaming_mob( 3002, 1000047, 180, 2, 10 )
    respawn_roaming_mob( 3002, 1000047, 60, 2, 10 )
    respawn_roaming_mob( 3002, 1000047, 300, 2, 10 )

    respawn_roaming_mob( 3003, 1000047, 180, 2, 10 )
    respawn_roaming_mob( 3003, 1000047, 60, 2, 10 )
    respawn_roaming_mob( 3003, 1000047, 300, 2, 10 )

    respawn_roaming_mob( 3004, 1000047, 180, 2, 10 )
    respawn_roaming_mob( 3004, 1000047, 60, 2, 10 )
    respawn_roaming_mob( 3004, 1000047, 300, 2, 10 )

    -- 카탄
    respawn_roaming_mob( 4001, 1000049, 180, 2, 10 )
    respawn_roaming_mob( 4001, 1000049, 60, 2, 10 )
    respawn_roaming_mob( 4001, 1000049, 300, 2, 10 )

    respawn_roaming_mob( 4002, 1000049, 180, 2, 10 )
    respawn_roaming_mob( 4002, 1000049, 60, 2, 10 )
    respawn_roaming_mob( 4002, 1000049, 300, 2, 10 )

    respawn_roaming_mob( 4003, 1000049, 180, 2, 10 )
    respawn_roaming_mob( 4003, 1000049, 60, 2, 10 )
    respawn_roaming_mob( 4003, 1000049, 300, 2, 10 )

    respawn_roaming_mob( 4004, 1000049, 180, 2, 10 )
    respawn_roaming_mob( 4004, 1000049, 60, 2, 10 )
    respawn_roaming_mob( 4004, 1000049, 300, 2, 10 )
	
	
	--respawn_roaming_mob( 그룹 로밍 ID, 리스폰 몬스터 ID, 방향 (각도), 로밍 동선과의 거리(M미터), 리스폰 주기(초) )
	
	--110 필드 그룹 로밍 보스
	respawn_roaming_mob( 5110, 110102, 180, 0, 300 )
	respawn_roaming_mob( 5110, 110202, 0, 4, 15  )
	respawn_roaming_mob( 5110, 110203, 90, 4, 15  )
	respawn_roaming_mob( 5110, 110204, 180, 4, 15  )
	respawn_roaming_mob( 5110, 110205, 270, 4, 15  )
	
		--120 필드 그룹 로밍 보스
	respawn_roaming_mob( 5120, 120102, 180, 0, 300 )
	respawn_roaming_mob( 5120, 120202, 0, 4, 15  )
	respawn_roaming_mob( 5120, 120203, 90, 4, 15  )
	respawn_roaming_mob( 5120, 120204, 180, 4, 15  )
	respawn_roaming_mob( 5120, 120205, 270, 4, 15  )
	
	--130 필드 그룹 로밍 보스
	respawn_roaming_mob( 5130, 130102, 180, 0, 300 )
	respawn_roaming_mob( 5130, 130202, 0, 4, 15  )
	respawn_roaming_mob( 5130, 130203, 90, 4, 15  )
	respawn_roaming_mob( 5130, 130204, 180, 4, 15  )
	respawn_roaming_mob( 5130, 130205, 270, 4, 15  )

	-- 140 필드 그룹 로밍 보스
	respawn_roaming_mob( 5140, 140102, 180, 0, 300 )
	respawn_roaming_mob( 5140, 140202, 0, 4, 15  )
	respawn_roaming_mob( 5140, 140203, 90, 4, 15  )
	respawn_roaming_mob( 5140, 140204, 180, 4, 15  )
	respawn_roaming_mob( 5140, 140205, 270, 4, 15  )

	-- 150 필드 그룹 로밍 보스
	respawn_roaming_mob( 5150, 150102, 180, 0, 300 )
	respawn_roaming_mob( 5150, 150202, 0, 4, 15  )
	respawn_roaming_mob( 5150, 150203, 90, 4, 15  )
	respawn_roaming_mob( 5150, 150204, 180, 4, 15  )
	respawn_roaming_mob( 5150, 150205, 270, 4, 15  )

	-- 160 필드 그룹 로밍 보스
	respawn_roaming_mob( 5160, 160102, 180, 0, 300 )
	respawn_roaming_mob( 5160, 160202, 0, 4, 15  )
	respawn_roaming_mob( 5160, 160203, 90, 4, 15  )
	respawn_roaming_mob( 5160, 160204, 180, 4, 15  )
	respawn_roaming_mob( 5160, 160205, 270, 4, 15  )

	-- 170 필드 그룹 로밍 보스
	respawn_roaming_mob( 5170, 170102, 180, 0, 300 )
	respawn_roaming_mob( 5170, 170202, 0, 4, 60 )
	respawn_roaming_mob( 5170, 170203, 90, 4, 60 )
	respawn_roaming_mob( 5170, 170204, 180, 4, 60 )
	respawn_roaming_mob( 5170, 170205, 270, 4, 60 )
	
	-- 50 필드 로밍 보스 슬래터
	respawn_roaming_mob( 10001, 300002, 180, 0, 360000 )
	
	-- 150 필드 로밍 보스 운다인
	respawn_roaming_mob( 10004, 300003, 180, 0, 360000 )
	
	-- 180 필드 로밍 보스 픽
	respawn_roaming_mob( 10005, 300005, 180, 0, 360000 )
	
	
	--respawn_roaming_mob( 그룹 로밍 ID, 리스폰 몬스터 ID, 방향 (각도), 로밍 동선과의 거리(M미터), 리스폰 주기(초) )
	--엘 카시아 금벽장 1 
	respawn_roaming_mob( 7001, 144008, 180, 0, 60 ) -- 미스틱 지니
	respawn_roaming_mob( 7001, 141007, 0, 4, 60 )
	respawn_roaming_mob( 7001, 141007, 90, 4, 60 )
	--respawn_roaming_mob( 7001, 141007, 180, 4, 60 )
	--respawn_roaming_mob( 7001, 141007, 270, 4, 60 )
	
	--엘 카시아 독벽장 1
	respawn_roaming_mob( 7002, 144011, 180, 0, 60 ) -- 에델 아우게 매지션
	respawn_roaming_mob( 7002, 141008, 0, 4, 60 )
	respawn_roaming_mob( 7002, 141008, 90, 4, 60 )
	--respawn_roaming_mob( 7002, 141008, 180, 4, 60 )
	--respawn_roaming_mob( 7002, 141008, 270, 4, 60 )

	--엘 카시아 수벽장 1
	respawn_roaming_mob( 7003, 144010, 180, 0, 60 ) -- 타파리
	respawn_roaming_mob( 7003, 141005, 0, 4, 60 )
	respawn_roaming_mob( 7003, 141005, 90, 4, 60 )
	--respawn_roaming_mob( 7003, 141005, 180, 4, 60 )
	--respawn_roaming_mob( 7003, 141005, 270, 4, 60 )

	--엘 카시아 금벽장 2
	respawn_roaming_mob( 7004, 144007, 180, 0, 60 ) -- 지니
	respawn_roaming_mob( 7004, 141007, 0, 4, 60 )
	--respawn_roaming_mob( 7004, 141007, 90, 4, 60 )
	--respawn_roaming_mob( 7004, 141007, 180, 4, 60 )
	--respawn_roaming_mob( 7004, 141007, 270, 4, 60 )

	--엘 카시아 독벽장 2
	respawn_roaming_mob( 7005, 144012, 180, 0, 60 ) -- 에델 아우게 레인저
	respawn_roaming_mob( 7005, 141009, 0, 4, 60 )
	respawn_roaming_mob( 7005, 141009, 90, 4, 60 )
	--respawn_roaming_mob( 7005, 141009, 180, 4, 60 )
	--respawn_roaming_mob( 7005, 141009, 270, 4, 60 )

	--엘 카시아 수벽장 2
	respawn_roaming_mob( 7006, 144009, 180, 0, 60 ) -- 아이무스
	respawn_roaming_mob( 7006, 141004, 0, 4, 60 )
	respawn_roaming_mob( 7006, 141004, 90, 4, 60 )
	--respawn_roaming_mob( 7006, 141004, 180, 4, 60 )
	--respawn_roaming_mob( 7006, 141004, 270, 4, 60 )


	--숨겨진 팔미르 던전1
	--기존 영역1331093
	respawn_roaming_mob( 1202001, 10093008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202001, 10094007, 0, 4, 30000 )		
	respawn_roaming_mob( 1202001, 10095009, 90, 4, 30000 )		
							
	respawn_roaming_mob( 1202002, 10096011, 180, 0, 30000 )	
	respawn_roaming_mob( 1202002, 10097010, 0, 4, 30000 )		
	respawn_roaming_mob( 1202002, 10098010, 90, 4, 30000 )		
								


	--숨겨진 팔미르 던전2
	--기존 영역1330093
	respawn_roaming_mob( 1202003, 10093006, 180, 0, 30000 )	
	respawn_roaming_mob( 1202003, 10094008, 0, 4, 30000 )		
	respawn_roaming_mob( 1202003, 10095010, 90, 4, 30000 )		
	respawn_roaming_mob( 1202003, 10093006, 270, 4, 30000 )		

	respawn_roaming_mob( 1202004, 10096009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202004, 10097010, 0, 4, 30000 )		
	respawn_roaming_mob( 1202004, 10098009, 90, 4, 30000 )		
	respawn_roaming_mob( 1202004, 10094008, 270, 4, 30000)		


	--숨겨진 팔미르 던전3	
	--기존 영역1332094
	respawn_roaming_mob( 1202005, 10093008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202005, 10094008, 0, 4, 30000 )		
	respawn_roaming_mob( 1202005, 10095009, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202006, 10096009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202006, 10097010, 0, 4, 30000 )		
	respawn_roaming_mob( 1202006, 10098011, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전4
	--기존 영역1333095
	respawn_roaming_mob( 1202007, 10095009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202007, 10096009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202007, 10097013, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202008, 10098010, 180, 0, 30000 )	
	respawn_roaming_mob( 1202008, 10099008, 0, 4, 30000 )		
	respawn_roaming_mob( 1202008, 10100008, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전5
	--기존 영역 : 1374095
	respawn_roaming_mob( 1202009, 10095021, 180, 0, 30000 )	
	respawn_roaming_mob( 1202009, 10096018, 0, 4, 30000 )	
	respawn_roaming_mob( 1202009, 10097018, 90, 4, 30000 )	
								

	--숨겨진 팔미르 던전6
	--기존 영역 : 1375096
	respawn_roaming_mob( 1202010, 10096018, 180, 0, 30000 )	
	respawn_roaming_mob( 1202010, 10097019, 0, 4, 30000 )	
	respawn_roaming_mob( 1202010, 10098017, 90, 4, 30000 )	
								

	--숨겨진 팔미르 던전8
	--기존 영역 : 1329092
	respawn_roaming_mob( 1202012, 10092004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202012, 10093006, 0, 4, 30000 )		
	respawn_roaming_mob( 1202012, 10094007, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202013, 10095009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202013, 10096009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202013, 10097009, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전9
	--기존 영역 : 1372093
	respawn_roaming_mob( 1202014, 10092008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202014, 10093014, 0, 4, 30000 )	
	respawn_roaming_mob( 1202014, 10094017, 90, 4, 30000 )	
								
	--숨겨진 팔미르 던전10
	--기존 영역 : 1302091
	respawn_roaming_mob( 1202015, 10091002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202015, 10092002, 0, 4, 30000 )	
	respawn_roaming_mob( 1202015, 10093001, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202016, 10094001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202016, 10095002, 0, 4, 30000 )	
	respawn_roaming_mob( 1202016, 10096001, 90, 4, 30000 )	
								

	--숨겨진 팔미르 던전11
	--기존 영역 : 1369092
	respawn_roaming_mob( 1202017, 10092008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202017, 10093014, 0, 4, 30000 )	
	respawn_roaming_mob( 1202017, 10094017, 90, 4, 30000 )	
								
	--숨겨진 팔미르 던전12
	--기존 영역 : 1303092
	respawn_roaming_mob( 1202018, 10092003, 180, 0, 30000 )	
	respawn_roaming_mob( 1202018, 10093003, 0, 4, 30000 )	
	respawn_roaming_mob( 1202018, 10094003, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202019, 10095001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202019, 10096002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202019, 10097001, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전13
	--기존 영역 : 1301090
	respawn_roaming_mob( 1202020, 10090021, 180, 0, 30000 )	
	respawn_roaming_mob( 1202020, 10091001, 0, 4, 30000 )	
	respawn_roaming_mob( 1202020, 10092001, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202021, 10093001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202021, 10094001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202021, 10095001, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전14
	--기존 영역 : 1346090
	respawn_roaming_mob( 1202022, 10090022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202022, 10091003, 0, 4, 30000 )		
	respawn_roaming_mob( 1202022, 10092005, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전15
	--기존 영역 : 1347092
	respawn_roaming_mob( 1202023, 10092006, 180, 0, 30000 )	
	respawn_roaming_mob( 1202023, 10093010, 0, 4, 30000 )		
	respawn_roaming_mob( 1202023, 10094011, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전16
	--기존 영역 : 1349092
	respawn_roaming_mob( 1202024, 10092006, 180, 0, 30000 )	
	respawn_roaming_mob( 1202024, 10093010, 0, 4, 30000 )		
	respawn_roaming_mob( 1202024, 10094011, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전17
	--기존 영역 : 1304093
	respawn_roaming_mob( 1202025, 10093003, 180, 0, 30000 )	
	respawn_roaming_mob( 1202025, 10094004, 0, 4, 30000 )		
	respawn_roaming_mob( 1202025, 10095004, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202026, 10096002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202026, 10097002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202026, 10098001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전18
	--기존 영역 : 1351093
	respawn_roaming_mob( 1202027, 10093010, 180, 0, 30000 )	
	respawn_roaming_mob( 1202027, 10094014, 0, 4, 30000 )		
	respawn_roaming_mob( 1202027, 10095016, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전19
	--기존 영역 : 1305093
	respawn_roaming_mob( 1202028, 10093001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202028, 10094001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202028, 10095005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202029, 10096004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202029, 10097001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202029, 10098001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전20
	--기존 영역 : 1352094
	respawn_roaming_mob( 1202030, 10094014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202030, 10095017, 0, 4, 30000 )		
	respawn_roaming_mob( 1202030, 10096016, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전21
	--기존 영역 : 1305093
	respawn_roaming_mob( 1202031, 10093001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202031, 10094001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202031, 10095005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202032, 10096004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202032, 10097001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202032, 10098001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전22
	--기존 영역 : 1307095
	respawn_roaming_mob( 1202033, 10095001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202033, 10096002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202033, 10097001, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202034, 10098001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202034, 10099002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202034, 10100001, 90, 4, 30000 )		

	--숨겨진 팔미르 던전23
	--기존 영역 : 1308095
	respawn_roaming_mob( 1202035, 10095001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202035, 10096001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202035, 10097002, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202036, 10098005, 180, 0, 30000 )	
	respawn_roaming_mob( 1202036, 10099001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202036, 10100001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전24
	--기존 영역 : 1309096
	respawn_roaming_mob( 1202037, 10096002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202037, 10097002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202037, 10098001, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202038, 10099001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202038, 10100001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202038, 10101001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전25
	--기존 영역1310097
	respawn_roaming_mob( 1202039, 10097008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202039, 10098007, 0, 4, 30000 )		
	respawn_roaming_mob( 1202039, 10099005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202040, 10100004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202040, 10101002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202040, 10102001, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전26
	--기존 영역1311098
	respawn_roaming_mob( 1202041, 10098008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202041, 10099002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202041, 10100004, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202042, 10101003, 180, 0, 30000 )	
	respawn_roaming_mob( 1202042, 10102001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202042, 10103001, 90, 4, 30000 )		
								


	--숨겨진 팔미르 던전27
	--기존 영역1312099
	respawn_roaming_mob( 1202043, 10099002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202043, 10100004, 0, 4, 30000 )		
	respawn_roaming_mob( 1202043, 10101004, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202044, 10102001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202044, 10103002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202044, 10104001, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전28
	--기존 영역 : 1356100
	respawn_roaming_mob( 1202045, 10100015, 180, 0, 30000 )	
	respawn_roaming_mob( 1202045, 10101015, 0, 4, 30000 )		
	respawn_roaming_mob( 1202045, 10102015, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전29
	--기존 영역1313100
	respawn_roaming_mob( 1202046, 10100004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202046, 10101002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202046, 10102001, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202047, 10103003, 180, 0, 30000 )	
	respawn_roaming_mob( 1202047, 10104002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202047, 10105001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전30
	--기존 영역1314101
	respawn_roaming_mob( 1202048, 10101002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202048, 10102005, 0, 4, 30000 )		
	respawn_roaming_mob( 1202048, 10103003, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202049, 10104003, 180, 0, 30000 )	
	respawn_roaming_mob( 1202049, 10105001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202049, 10106001, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전31
	--기존 영역1315101
	respawn_roaming_mob( 1202050, 10101004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202050, 10102005, 0, 4, 30000 )		
	respawn_roaming_mob( 1202050, 10103001, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202051, 10104001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202051, 10105003, 0, 4, 30000 )		
	respawn_roaming_mob( 1202051, 10106001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전32
	--기존 영역 : 1358102
	respawn_roaming_mob( 1202052, 10102017, 180, 0, 30000 )	
	respawn_roaming_mob( 1202052, 10103015, 0, 4, 30000 )		
	respawn_roaming_mob( 1202052, 10104016, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전33
	--기존 영역 : 1359103
	respawn_roaming_mob( 1202053, 10103017, 180, 0, 30000 )	
	respawn_roaming_mob( 1202053, 10104017, 0, 4, 30000 )		
	respawn_roaming_mob( 1202053, 10105016, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전34
	--기존 영역 : 130000104
	respawn_roaming_mob( 1202054, 10104017, 180, 0, 30000 )	
	respawn_roaming_mob( 1202054, 10105016, 0, 4, 30000 )		
	respawn_roaming_mob( 1202054, 10106017, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전35
	--기존 영역1316102
	respawn_roaming_mob( 1202055, 10102007, 180, 0, 30000 )	
	respawn_roaming_mob( 1202055, 10103003, 0, 4, 30000 )		
	respawn_roaming_mob( 1202055, 10104003, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202056, 10105004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202056, 10106001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202056, 10107001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전36
	--기존 영역1317103
	respawn_roaming_mob( 1202057, 10103007, 180, 0, 30000 )	
	respawn_roaming_mob( 1202057, 10104002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202057, 10105005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202058, 10106004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202058, 10107002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202058, 10108001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전37
	--기존 영역1318104
	respawn_roaming_mob( 1202059, 10104002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202059, 10105003, 0, 4, 30000 )		
	respawn_roaming_mob( 1202059, 10106001, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202060, 10107002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202060, 10108002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202060, 10109001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전38
	--기존 영역1319105
	respawn_roaming_mob( 1202061, 10105005, 180, 0, 30000 )	
	respawn_roaming_mob( 1202061, 10106006, 0, 4, 30000 )		
	respawn_roaming_mob( 1202061, 10107002, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202062, 10108003, 180, 0, 30000 )	
	respawn_roaming_mob( 1202062, 10109001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202062, 10110001, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전39
	--기존 영역1320104
	respawn_roaming_mob( 1202063, 10104008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202063, 10105005, 0, 4, 30000 )		
	respawn_roaming_mob( 1202063, 10106007, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202064, 10107001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202064, 10108004, 0, 4, 30000 )		
	respawn_roaming_mob( 1202064, 10109003, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전40
	--기존 영역 : 1359103
	respawn_roaming_mob( 1202065, 10105018, 180, 0, 30000 )	
	respawn_roaming_mob( 1202065, 10106017, 0, 4, 30000 )		
	respawn_roaming_mob( 1202065, 10107015, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전41
	--기존 영역1321106
	respawn_roaming_mob( 1202066, 10106001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202066, 10107006, 0, 4, 30000 )		
	respawn_roaming_mob( 1202066, 10108005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202067, 10109004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202067, 10110002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202067, 10111001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전42
	--기존 영역1322106
	respawn_roaming_mob( 1202068, 10106009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202068, 10107001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202068, 10108001, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202069, 10109005, 180, 0, 30000 )	
	respawn_roaming_mob( 1202069, 10110002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202069, 10111001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전43
	--기존 영역1323107
	respawn_roaming_mob( 1202070, 10107008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202070, 10108001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202070, 10109005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202071, 10110004, 180, 0, 30000 )	
	respawn_roaming_mob( 1202071, 10111003, 0, 4, 30000 )		
	respawn_roaming_mob( 1202071, 10112001, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전44
	--기존 영역 : 1364108
	respawn_roaming_mob( 1202072, 10108018, 180, 0, 30000 )	
	respawn_roaming_mob( 1202072, 10109015, 0, 4, 30000 )		
	respawn_roaming_mob( 1202072, 10110012, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전45
	--기존 영역1324107
	respawn_roaming_mob( 1202073, 10107009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202073, 10108001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202073, 10109005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202074, 10110002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202074, 10111003, 0, 4, 30000 )		
	respawn_roaming_mob( 1202074, 10112002, 90, 4, 30000 )		
								

	--기존 영역1325108
	respawn_roaming_mob( 1202075, 10108001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202075, 10109001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202075, 10110004, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202076, 10111001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202076, 10112002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202076, 10113001, 90, 4, 30000 )		
								
	--기존 영역1324107
	respawn_roaming_mob( 1202077, 10107009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202077, 10108001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202077, 10109005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202078, 10110002, 180, 0, 30000 )	
	respawn_roaming_mob( 1202078, 10111003, 0, 4, 30000 )		
	respawn_roaming_mob( 1202078, 10112002, 90, 4, 30000 )		
								

	--기존 영역1325108
	respawn_roaming_mob( 1202079, 10108001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202079, 10109001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202079, 10110004, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202080, 10111001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202080, 10112002, 0, 4, 30000 )		
	respawn_roaming_mob( 1202080, 10113001, 90, 4, 30000 )		
								

	--기존 영역f
	respawn_roaming_mob( 1202081, 10107009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202081, 10108001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202081, 10109005, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전46	
	--기존 영역1365109
	respawn_roaming_mob( 1202082, 10109016, 180, 0, 30000 )	
	respawn_roaming_mob( 1202082, 10110013, 0, 4, 30000 )		
	respawn_roaming_mob( 1202082, 10111010, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전47
	--기존 영역1326108
	respawn_roaming_mob( 1202083, 10108001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202083, 10109005, 0, 4, 30000 )		
	respawn_roaming_mob( 1202083, 10110004, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202084, 10111003, 180, 0, 30000 )	
	respawn_roaming_mob( 1202084, 10112001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202084, 10113001, 90, 4, 30000 )		
								
	--숨겨진 팔미르 던전48	
	--기존 영역1366109
	respawn_roaming_mob( 1202085, 10109016, 180, 0, 30000 )	
	respawn_roaming_mob( 1202085, 10110012, 0, 4, 30000 )		
	respawn_roaming_mob( 1202085, 10111010, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전49
	--기존 영역1326108
	respawn_roaming_mob( 1202086, 10108001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202086, 10109005, 0, 4, 30000 )		
	respawn_roaming_mob( 1202086, 10109005, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202087, 10111001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202087, 10112005, 0, 4, 30000 )		
	respawn_roaming_mob( 1202087, 10113003, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전50
	--기존 영역1328108
	respawn_roaming_mob( 1202088, 10108001, 180, 0, 30000 )	
	respawn_roaming_mob( 1202088, 10109005, 0, 4, 30000 )		
	respawn_roaming_mob( 1202088, 10110001, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202089, 10111003, 180, 0, 30000 )	
	respawn_roaming_mob( 1202089, 10112001, 0, 4, 30000 )		
	respawn_roaming_mob( 1202089, 10113001, 90, 4, 30000 )		

	--숨겨진 팔미르 던전51
	--기존 영역1345106
	respawn_roaming_mob( 1202090, 10106016, 180, 0, 30000 )	
	respawn_roaming_mob( 1202090, 10107011, 0, 4, 30000 )		
	respawn_roaming_mob( 1202090, 10108014, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202091, 10109012, 180, 0, 30000 )	
	respawn_roaming_mob( 1202091, 10110011, 0, 4, 30000 )		
	respawn_roaming_mob( 1202091, 10111009, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전52	
	--기존 영역1385107
	respawn_roaming_mob( 1202092, 10107019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202092, 10108014, 0, 4, 30000 )		
	respawn_roaming_mob( 1202092, 10109012, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전53
	--기존 영역1345105
	respawn_roaming_mob( 1202093, 10105015, 180, 0, 30000 )	
	respawn_roaming_mob( 1202093, 10106012, 0, 4, 30000 )	
	respawn_roaming_mob( 1202093, 10107013, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202094, 10108013, 180, 0, 30000 )	
	respawn_roaming_mob( 1202094, 10109012, 0, 4, 30000 )		
	respawn_roaming_mob( 1202094, 10110010, 90, 4, 30000 )		

	--숨겨진 팔미르 던전54	
	--기존 영역1384106
	respawn_roaming_mob( 1202095, 10106023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202095, 10107019, 0, 4, 30000 )	
	respawn_roaming_mob( 1202095, 10108014, 90, 4, 30000 )	


	--숨겨진 팔미르 던전55
	--기존 영역1344104
	respawn_roaming_mob( 1202096, 10104013, 180, 0, 30000 )	
	respawn_roaming_mob( 1202096, 10105014, 0, 4, 30000 )		
	respawn_roaming_mob( 1202096, 10106013, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202097, 10107012, 180, 0, 30000 )	
	respawn_roaming_mob( 1202097, 10108014, 0, 4, 30000 )	
	respawn_roaming_mob( 1202097, 10109012, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전56	
	--기존 영역1382105
	respawn_roaming_mob( 1202098, 10105020, 180, 0, 30000 )	
	respawn_roaming_mob( 1202098, 10106010, 0, 4, 30000 )	
	respawn_roaming_mob( 1202098, 10107010, 90, 4, 30000 )	

	--숨겨진 팔미르 던전57	
	--기존 영역1381104
	respawn_roaming_mob( 1202099, 10104019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202099, 10105020, 0, 4, 30000 )	
	respawn_roaming_mob( 1202099, 10106010, 90, 4, 30000 )	

	--숨겨진 팔미르 던전58
	--기존 영역1343103
	respawn_roaming_mob( 1202100, 10103011, 180, 0, 30000 )	
	respawn_roaming_mob( 1202100, 10104014, 0, 4, 30000 )	
	respawn_roaming_mob( 1202100, 10105012, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202101, 10106013, 180, 0, 30000 )	
	respawn_roaming_mob( 1202101, 10107011, 0, 4, 30000 )	
	respawn_roaming_mob( 1202101, 10108013, 90, 4, 30000 )	
								
	--숨겨진 팔미르 던전59	
	--기존 영역1381104
	respawn_roaming_mob( 1202102, 10103018, 180, 0, 30000 )	
	respawn_roaming_mob( 1202102, 10104019, 0, 4, 30000 )		
	respawn_roaming_mob( 1202102, 10105019, 90, 4, 30000 )		

	--숨겨진 팔미르 던전60
	--기존 영역1342102
	respawn_roaming_mob( 1202103, 10102008, 180, 0, 30000 )	
	respawn_roaming_mob( 1202103, 10103013, 0, 4, 30000 )		
	respawn_roaming_mob( 1202103, 10104013, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202104, 10105012, 180, 0, 30000 )	
	respawn_roaming_mob( 1202104, 10106012, 0, 4, 30000 )		
	respawn_roaming_mob( 1202104, 10107010, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전61
	--기존 영역1341101
	respawn_roaming_mob( 1202105, 10101013, 180, 0, 30000 )	
	respawn_roaming_mob( 1202105, 10102009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202105, 10103013, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202106, 10104011, 180, 0, 30000 )	
	respawn_roaming_mob( 1202106, 10105009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202106, 10106010, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전62	
	--기존 영역1379102
	respawn_roaming_mob( 1202107, 10102018, 180, 0, 30000 )	
	respawn_roaming_mob( 1202107, 10103018, 0, 4, 30000 )		
	respawn_roaming_mob( 1202107, 10104019, 90, 4, 30000 )		

	--숨겨진 팔미르 던전63
	--기존 영역1340101
	respawn_roaming_mob( 1202108, 10101013, 180, 0, 30000 )	
	respawn_roaming_mob( 1202108, 10102012, 0, 4, 30000 )	
	respawn_roaming_mob( 1202108, 10103011, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202109, 10104011, 180, 0, 30000 )	
	respawn_roaming_mob( 1202109, 10105009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202109, 10106010, 90, 4, 30000 )	
								

	--숨겨진 팔미르 던전64
	--기존 영역1339100
	respawn_roaming_mob( 1202110, 10100012, 180, 0, 30000 )	
	respawn_roaming_mob( 1202110, 10101012, 0, 4, 30000 )		
	respawn_roaming_mob( 1202110, 10102011, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202111, 10103010, 180, 0, 30000 )	
	respawn_roaming_mob( 1202111, 10104009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202111, 10105009, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전65
	--기존 영역1338099
	respawn_roaming_mob( 1202112, 10099013, 180, 0, 30000 )	
	respawn_roaming_mob( 1202112, 10100013, 0, 4, 30000 )		
	respawn_roaming_mob( 1202112, 10101011, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202113, 10102009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202113, 10103008, 0, 4, 30000 )		
	respawn_roaming_mob( 1202113, 10104009, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전66
	--기존 영역1337098
	respawn_roaming_mob( 1202114, 10098016, 180, 0, 30000 )	
	respawn_roaming_mob( 1202114, 10099011, 0, 4, 30000 )		
	respawn_roaming_mob( 1202114, 10100012, 90, 4, 30000 )	

	respawn_roaming_mob( 1202115, 10101010, 180, 0, 30000 )	
	respawn_roaming_mob( 1202115, 10102009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202115, 10103008, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전67	
	--기존 영역1377098
	respawn_roaming_mob( 1202116, 10098018, 180, 0, 30000 )	
	respawn_roaming_mob( 1202116, 10099017, 0, 4, 30000 )		
	respawn_roaming_mob( 1202116, 10100018, 90, 4, 30000 )		


	--숨겨진 팔미르 던전68
	--기존 영역133000097
	respawn_roaming_mob( 1202117, 10097016, 180, 0, 30000 )	
	respawn_roaming_mob( 1202117, 10098015, 0, 4, 30000 )		
	respawn_roaming_mob( 1202117, 10099011, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202118, 10100011, 180, 0, 30000 )	
	respawn_roaming_mob( 1202118, 10101009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202118, 10102008, 90, 4, 30000 )		
							

	--숨겨진 팔미르 던전69	
	--기존 영역1376097
	respawn_roaming_mob( 1202119, 10097019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202119, 10098018, 0, 4, 30000 )		
	respawn_roaming_mob( 1202119, 10099016, 90, 4, 30000 )		

	--숨겨진 팔미르 던전70
	--기존 영역1335096
	respawn_roaming_mob( 1202120, 10096009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202120, 10097013, 0, 4, 30000 )		
	respawn_roaming_mob( 1202120, 10098010, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202121, 10099010, 180, 0, 30000 )	
	respawn_roaming_mob( 1202121, 10100010, 0, 4, 30000 )		
	respawn_roaming_mob( 1202121, 10101008, 90, 4, 30000 )		
								

	--숨겨진 팔미르 던전71
	--기존 영역1334095
	respawn_roaming_mob( 1202122, 10095009, 180, 0, 30000 )	
	respawn_roaming_mob( 1202122, 10096011, 0, 4, 30000 )		
	respawn_roaming_mob( 1202122, 10097010, 90, 4, 30000 )		
								
	respawn_roaming_mob( 1202123, 10098010, 180, 0, 30000 )	
	respawn_roaming_mob( 1202123, 10099009, 0, 4, 30000 )		
	respawn_roaming_mob( 1202123, 10100008, 90, 4, 30000 )		
						

	--숨겨진 팔미르 던전72
	--기존 영역1395105
	respawn_roaming_mob( 1202123, 10105029, 180, 0, 30000 )	 
	respawn_roaming_mob( 1202123, 10106027, 0, 4, 30000 )		
	respawn_roaming_mob( 1202123, 10107025, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202124, 10108026, 180, 0, 30000 )	
	respawn_roaming_mob( 1202124, 10109022, 0, 4, 30000 )		
	respawn_roaming_mob( 1202124, 10110017, 90, 4, 30000 )	

	--숨겨진 팔미르 던전73
	--기존 영역1396106
	respawn_roaming_mob( 1202125, 10106028, 180, 0, 30000 )	
	respawn_roaming_mob( 1202125, 10106029, 0, 4, 30000 )		
	respawn_roaming_mob( 1202125, 10107026, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202126, 10107027, 180, 0, 30000 )	
	respawn_roaming_mob( 1202126, 10108027, 0, 4, 30000 )		
	respawn_roaming_mob( 1202126, 10109023, 90, 4, 30000 )	

	--숨겨진 팔미르 던전74
	--기존 영역1395105
	respawn_roaming_mob( 1202127, 10105029, 180, 0, 30000 )	
	respawn_roaming_mob( 1202127, 10106027, 0, 4, 30000 )		
	respawn_roaming_mob( 1202127, 10107025, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202128, 10108026, 180, 0, 30000 )	
	respawn_roaming_mob( 1202128, 10109022, 0, 4, 30000 )		
	respawn_roaming_mob( 1202128, 10110017, 90, 4, 30000 )	
								
	--숨겨진 팔미르 던전75
	--기존 영역1396106
	respawn_roaming_mob( 1202129, 10106028, 180, 0, 30000 )	
	respawn_roaming_mob( 1202129, 10106029, 0, 4, 30000 )		
	respawn_roaming_mob( 1202129, 10107026, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202130, 10107027, 180, 0, 30000 )	
	respawn_roaming_mob( 1202130, 10108027, 0, 4, 30000 )		
	respawn_roaming_mob( 1202130, 10109023, 90, 4, 30000 )		
	
	--숨겨진 팔미르 던전76
	--기존 영역1395105
	respawn_roaming_mob( 1202131, 10105029, 180, 0, 30000 )	
	respawn_roaming_mob( 1202131, 10106027, 0, 4, 30000 )	
	respawn_roaming_mob( 1202131, 10107025, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202132, 10108026, 180, 0, 30000 )	
	respawn_roaming_mob( 1202132, 10109022, 0, 4, 30000 )	
	respawn_roaming_mob( 1202132, 10110017, 90, 4, 30000 )	

	--숨겨진 팔미르 던전77
	--기존 영역1396106
	respawn_roaming_mob( 1202133, 10106028, 180, 0, 30000 )	
	respawn_roaming_mob( 1202133, 10106029, 0, 4, 30000 )	
	respawn_roaming_mob( 1202133, 10107026, 90, 4, 30000 )	
								

	--숨겨진 팔미르 던전85 가운데 던전 
	--기존 영역1367105
	respawn_roaming_mob( 1202134, 10105026, 180, 0, 30000 )	
	respawn_roaming_mob( 1202134, 10105027, 0, 4, 30000 )	
	respawn_roaming_mob( 1202134, 10106024, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202135, 10107021, 180, 0, 30000 )	
	respawn_roaming_mob( 1202135, 10108023, 0, 4, 30000 )	
	respawn_roaming_mob( 1202135, 10109019, 90, 4, 30000 )	

	--숨겨진 팔미르 던전86
	--기존 영역1367105
	respawn_roaming_mob( 1202136, 10105026, 180, 0, 30000 )	
	respawn_roaming_mob( 1202136, 10105027, 0, 4, 30000 )	
	respawn_roaming_mob( 1202136, 10106024, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202137, 10107021, 180, 0, 30000 )	
	respawn_roaming_mob( 1202137, 10108023, 0, 4, 30000 )	
	respawn_roaming_mob( 1202137, 10109019, 90, 4, 30000 )	


	--숨겨진 팔미르 던전87
	--기존 영역1368106
	respawn_roaming_mob( 1202138, 10106025, 180, 0, 30000 )	
	respawn_roaming_mob( 1202138, 10107022, 0, 4, 30000 )	
	respawn_roaming_mob( 1202138, 10107023, 90, 4, 30000 )	

	respawn_roaming_mob( 1202139, 10108024, 180, 0, 30000 )	
	respawn_roaming_mob( 1202139, 10109023, 0, 4, 30000 )	
	respawn_roaming_mob( 1202139, 10109019, 90, 4, 30000 )	

	--숨겨진 팔미르 던전88
	--기존 영역1368106
	respawn_roaming_mob( 1202140, 10106025, 180, 0, 30000 )	
	respawn_roaming_mob( 1202140, 10107022, 0, 4, 30000 )	
	respawn_roaming_mob( 1202140, 10107023, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202141, 10108024, 180, 0, 30000 )	
	respawn_roaming_mob( 1202141, 10109023, 0, 4, 30000 )	
	respawn_roaming_mob( 1202141, 10109019, 90, 4, 30000 )	

	--숨겨진 팔미르 던전89
	--기존 영역1368106
	respawn_roaming_mob( 1202142, 10106025, 180, 0, 30000 )	
	respawn_roaming_mob( 1202142, 10107022, 0, 4, 30000 )	
	respawn_roaming_mob( 1202142, 10107023, 90, 4, 30000 )	
								
	respawn_roaming_mob( 1202143, 10108024, 180, 0, 30000 )	
	respawn_roaming_mob( 1202143, 10109023, 0, 4, 30000 )	
	respawn_roaming_mob( 1202143, 10109019, 90, 4, 30000 )	

	--숨겨진 팔미르 던전90
	--기존 영역1368106
	respawn_roaming_mob( 1202144, 10106025, 180, 0, 30000 )	
	respawn_roaming_mob( 1202144, 10107022, 0, 4, 30000 )	
	respawn_roaming_mob( 1202145, 10107023, 90, 4, 30000 )		

	respawn_roaming_mob( 1202145, 10108024, 180, 0, 30000 )	
	respawn_roaming_mob( 1202145, 10109023, 0, 4, 30000 )	
	respawn_roaming_mob( 1202145, 10109019, 90, 4, 30000 )	
								

	--숨겨진 팔미르 던전91
	--기존 영역1368106
	respawn_roaming_mob( 1202146, 10106025, 180, 0, 30000 )	
	respawn_roaming_mob( 1202146, 10107022, 0, 4, 30000 )	
	respawn_roaming_mob( 1202146, 10107023, 90, 4, 30000 )	


	--47기존 영역1372093
	respawn_roaming_mob( 1202147, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202147, 10094016, 0, 4, 30000 )	
	respawn_roaming_mob( 1202147, 10095019, 90, 4, 30000 )	


	--48기존 영역 1373093
	respawn_roaming_mob( 1202148, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202148, 10094016, 0, 4, 30000 )	
	respawn_roaming_mob( 1202148, 10095019, 90, 4, 30000 )	

	--49기존 영역 1396095
	respawn_roaming_mob( 1202149, 10095022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202149, 10095022, 0, 4, 30000 )	

	--50기존 영역 1396095
	respawn_roaming_mob( 1202150, 10095022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202150, 10095022, 0, 4, 30000 )

	--51기존 영역 1396095
	respawn_roaming_mob( 1202151, 10095022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202151, 10095022, 0, 4, 30000 )

	--52기존 영역 1396095
	respawn_roaming_mob( 1202152, 10095022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202152, 10095022, 0, 4, 30000 )

	--53기존 영역 1396095
	respawn_roaming_mob( 1202153, 10095022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202153, 10095022, 0, 4, 30000 )

	--54기존 영역 1396095 암브
	respawn_roaming_mob( 1202154, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202154, 10094016, 0, 4, 30000 )	
	respawn_roaming_mob( 1202154, 10095019, 90, 4, 30000 )

	--55기존 영역 1396095
	respawn_roaming_mob( 1202155, 10095022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202155, 10095022, 0, 4, 30000 )

	--56기존 영역 1396095
	respawn_roaming_mob( 1202156, 10095022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202156, 10095022, 0, 4, 30000 )

	--57기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202157, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202157, 10101019, 0, 4, 30000 )

	--58기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202158, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202158, 10105023, 0, 4, 30000 )

	--59기존 영역 1396095
	respawn_roaming_mob( 1202159, 10095022, 180, 0, 30000 )	
	respawn_roaming_mob( 1202159, 10095022, 0, 4, 30000 )

	--60기존 영역 1396095 암브
	respawn_roaming_mob( 1202160, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202160, 10094016, 0, 4, 30000 )	
	respawn_roaming_mob( 1202160, 10095019, 90, 4, 30000 )


	--61기존 영역 1396095 암브
	respawn_roaming_mob( 1202161, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202161, 10094016, 0, 4, 30000 )	
	respawn_roaming_mob( 1202161, 10095019, 90, 4, 30000 )


	--62기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202162, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202162, 10105023, 0, 4, 30000 )


	--63기존 영역 1396095 암브
	respawn_roaming_mob( 1202163, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202163, 10094016, 0, 4, 30000 )	

	--64기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202164, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202164, 10101019, 0, 4, 30000 )

	--65기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202165, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202165, 10101019, 0, 4, 30000 )

	--66기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202166, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202166, 10101019, 0, 4, 30000 )

	--67기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202167, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202167, 10101019, 0, 4, 30000 )

	--68기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202168, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202168, 10101019, 0, 4, 30000 )


	--69기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202169, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202169, 10101019, 0, 4, 30000 )

	--70기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202170, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202170, 10105023, 0, 4, 30000 )

	--71기존 영역 1383105 
	respawn_roaming_mob( 1202171, 10105020, 180, 0, 30000 )	
	respawn_roaming_mob( 1202171, 10106010, 0, 4, 30000 )	
	respawn_roaming_mob( 1202171, 10107010, 90, 4, 30000 )

	--72기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202172, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202172, 10105023, 0, 4, 30000 )

	--73기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202173, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202173, 10105023, 0, 4, 30000 )

	--74기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202174, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202174, 10105023, 0, 4, 30000 )

	--75기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202175, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202175, 10105023, 0, 4, 30000 )

	--76기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202176, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202176, 10105023, 0, 4, 30000 )


	--77기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202177, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202177, 10105023, 0, 4, 30000 )


	--78기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202178, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202178, 10105023, 0, 4, 30000 )

	--79기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202179, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202179, 10105023, 0, 4, 30000 )

	--80기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202180, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202180, 10105023, 0, 4, 30000 )

	--81기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202181, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202181, 10105023, 0, 4, 30000 )


	--82기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202182, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202182, 10101019, 0, 4, 30000 )

	--83기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202183, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202183, 10101019, 0, 4, 30000 )

	--84기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202184, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202184, 10101019, 0, 4, 30000 )

	--85기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202185, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202185, 10105023, 0, 4, 30000 )

	--86기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202186, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202186, 10105023, 0, 4, 30000 )

	--87기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202187, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202187, 10105023, 0, 4, 30000 )

	--88기존 영역 1363106 
	respawn_roaming_mob( 1202188, 10106017, 180, 0, 30000 )	
	respawn_roaming_mob( 1202188, 10107016, 0, 4, 30000 )	
	respawn_roaming_mob( 1202188, 10108017, 90, 4, 30000 )


	--89기존 영역 1397105 오르페가
	respawn_roaming_mob( 1202189, 10105023, 180, 0, 30000 )	
	respawn_roaming_mob( 1202189, 10105023, 0, 4, 30000 )


	--90기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202190, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202190, 10101019, 0, 4, 30000 )

	--91기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202191, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202191, 10101019, 0, 4, 30000 )


	--92기존 영역 1395101 타타리아
	respawn_roaming_mob( 1202192, 10101019, 180, 0, 30000 )	
	respawn_roaming_mob( 1202192, 10101019, 0, 4, 30000 )

	--93기존 영역 1357101
	respawn_roaming_mob( 1202193, 10101015, 180, 0, 30000 )	
	respawn_roaming_mob( 1202193, 10102016, 0, 4, 30000 )	
	respawn_roaming_mob( 1202193, 10103015, 90, 4, 30000 )

	--94기존 영역 1396095 암브
	respawn_roaming_mob( 1202194, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202194, 10094016, 0, 4, 30000 )

	--95기존 영역 1354099 
	respawn_roaming_mob( 1202195, 10099014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202195, 10100015, 0, 4, 30000 )	
	respawn_roaming_mob( 1202195, 10101015, 90, 4, 30000 )


	--96기존 영역 1396095 암브
	respawn_roaming_mob( 1202196, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202196, 10094016, 0, 4, 30000 )


	--97기존 영역 1353095
	respawn_roaming_mob( 1202197, 10095015, 180, 0, 30000 )	
	respawn_roaming_mob( 1202197, 10096017, 0, 4, 30000 )	
	respawn_roaming_mob( 1202197, 10097017, 90, 4, 30000 )


	--98기존 영역 1396095 암브
	respawn_roaming_mob( 1202198, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202198, 10094016, 0, 4, 30000 )


	--99기존 영역 1396095 암브
	respawn_roaming_mob( 1202199, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202199, 10094016, 0, 4, 30000 )

	--100기존 영역 1350093
	respawn_roaming_mob( 1202200, 10093012, 180, 0, 30000 )	
	respawn_roaming_mob( 1202200, 10094013, 0, 4, 30000 )	
	respawn_roaming_mob( 1202200, 10095015, 90, 4, 30000 )


	--101기존 영역 1396095 암브
	respawn_roaming_mob( 1202201, 10093014, 180, 0, 30000 )	
	respawn_roaming_mob( 1202201, 10094016, 0, 4, 30000 )



	--숨겨진 수정 계곡 1
	--기존 영역1101070
	respawn_roaming_mob( 701001, 10070013, 180, 0, 30000 )
	respawn_roaming_mob( 701001, 10071001, 0, 4, 30000 )
	respawn_roaming_mob( 701001, 10072001, 90, 4, 30000 )

	respawn_roaming_mob( 701002, 10073001, 180, 0, 30000 )
	respawn_roaming_mob( 701002, 10074001, 0, 4, 30000 )
	respawn_roaming_mob( 701002, 10075001, 90, 4, 30000 )

	--숨겨진 수정 계곡 2
	--기존 영역1112070
	respawn_roaming_mob( 701003, 10070015, 180, 0, 30000 )
	respawn_roaming_mob( 701003, 10071003, 0, 4, 30000 )
	respawn_roaming_mob( 701003, 10072003, 90, 4, 30000 )

	respawn_roaming_mob( 701004, 10073004, 180, 0, 30000 )
	respawn_roaming_mob( 701004, 10074004, 0, 4, 30000 )
	respawn_roaming_mob( 701004, 10075005, 90, 4, 30000 )

	--숨겨진 수정 계곡 3
	--기존 영역1103072
	respawn_roaming_mob( 701005, 10070014, 180, 0, 30000 )
	respawn_roaming_mob( 701005, 10071002, 0, 4, 30000 )
	respawn_roaming_mob( 701005, 10072002, 90, 4, 30000 )

	respawn_roaming_mob( 701006, 10073002, 180, 0, 30000 )
	respawn_roaming_mob( 701006, 10074002, 0, 4, 30000 )
	respawn_roaming_mob( 701006, 10075002, 90, 4, 30000 )

	--숨겨진 수정 계곡 4
	--기존 영역1114071
	respawn_roaming_mob( 701007, 10071004, 180, 0, 30000 )
	respawn_roaming_mob( 701007, 10072004, 0, 4, 30000 )
	respawn_roaming_mob( 701007, 10073005, 90, 4, 30000 )

	respawn_roaming_mob( 701008, 10074005, 180, 0, 30000 )
	respawn_roaming_mob( 701008, 10075006, 0, 4, 30000 )
	respawn_roaming_mob( 701008, 10076005, 90, 4, 30000 )

	--숨겨진 수정 계곡 5
	--기존 영역1115073
	respawn_roaming_mob( 701009, 10073006, 180, 0, 30000 )
	respawn_roaming_mob( 701009, 10073007, 0, 4, 30000 )
	respawn_roaming_mob( 701009, 10074006, 90, 4, 30000 )

	respawn_roaming_mob( 701010, 10074007, 180, 0, 30000 )
	respawn_roaming_mob( 701010, 10075007, 0, 4, 30000 )
	respawn_roaming_mob( 701010, 10076006, 90, 4, 30000 )

	--숨겨진 수정 계곡 6
	--기존 영역1104073
	respawn_roaming_mob( 701011, 10073003, 180, 0, 30000 )
	respawn_roaming_mob( 701011, 10074003, 0, 4, 30000 )
	respawn_roaming_mob( 701011, 10075003, 90, 4, 30000 )

	respawn_roaming_mob( 701012, 10076001, 180, 0, 30000 )
	respawn_roaming_mob( 701012, 10077001, 0, 4, 30000 )
	respawn_roaming_mob( 701012, 10078001, 90, 4, 30000 )

	--숨겨진 수정 계곡 7
	--기존 영역1105075
	respawn_roaming_mob( 701013, 10075004, 180, 0, 30000 )
	respawn_roaming_mob( 701013, 10076002, 0, 4, 30000 )
	respawn_roaming_mob( 701013, 10077002, 90, 4, 30000 )

	respawn_roaming_mob( 701014, 10078002, 180, 0, 30000 )
	respawn_roaming_mob( 701014, 10079001, 0, 4, 30000 )
	respawn_roaming_mob( 701014, 10080001, 90, 4, 30000 )

	--숨겨진 수정 계곡 8
	--기존 영역1106076
	respawn_roaming_mob( 701015, 10076003, 180, 0, 30000 )
	respawn_roaming_mob( 701015, 10077003, 0, 4, 30000 )
	respawn_roaming_mob( 701015, 10078003, 90, 4, 30000 )

	respawn_roaming_mob( 701016, 10079002, 180, 0, 30000 )
	respawn_roaming_mob( 701016, 10080002, 0, 4, 30000 )
	respawn_roaming_mob( 701016, 10081001, 90, 4, 30000 )

	--숨겨진 수정 계곡 9
	--기존 영역1108076
	respawn_roaming_mob( 701017, 10076004, 180, 0, 30000 )
	respawn_roaming_mob( 701017, 10077004, 0, 4, 30000 )
	respawn_roaming_mob( 701017, 10078005, 90, 4, 30000 )

	respawn_roaming_mob( 701018, 10079004, 180, 0, 30000 )
	respawn_roaming_mob( 701018, 10080004, 0, 4, 30000 )
	respawn_roaming_mob( 701018, 10081003, 90, 4, 30000 )

	--숨겨진 수정 계곡 10
	--기존 영역1101070
	respawn_roaming_mob( 701019, 10078004, 180, 0, 30000 )
	respawn_roaming_mob( 701019, 10079003, 0, 4, 30000 )
	respawn_roaming_mob( 701019, 10080003, 90, 4, 30000 )

	respawn_roaming_mob( 701020, 10081002, 180, 0, 30000 )
	respawn_roaming_mob( 701020, 10082001, 0, 4, 30000 )
	respawn_roaming_mob( 701020, 10082001, 90, 4, 30000 )


	--숨겨진 수정 계곡 11
	--기존 영역1109078
	respawn_roaming_mob( 701021, 10078006, 180, 0, 30000 )
	respawn_roaming_mob( 701021, 10079005, 0, 4, 30000 )
	respawn_roaming_mob( 701021, 10080005, 90, 4, 30000 )

	respawn_roaming_mob( 701022, 10081004, 180, 0, 30000 )
	respawn_roaming_mob( 701022, 10082002, 0, 4, 30000 )
	respawn_roaming_mob( 701022, 10082002, 90, 4, 30000 )


	--숨겨진 수정 계곡 12
	--기존 영역1110080
	respawn_roaming_mob( 701023, 10080006, 180, 0, 30000 )
	respawn_roaming_mob( 701023, 10081005, 0, 4, 30000 )
	respawn_roaming_mob( 701023, 10082003, 90, 4, 30000 )

	respawn_roaming_mob( 701024, 10083001, 180, 0, 30000 )
	respawn_roaming_mob( 701024, 10084001, 0, 4, 30000 )
	respawn_roaming_mob( 701024, 10084001, 90, 4, 30000 )


	--숨겨진 수정 계곡 13
	--기존 영역1134084
	respawn_roaming_mob( 701025, 10084010, 180, 0, 30000 )
	respawn_roaming_mob( 701025, 10085007, 0, 4, 30000 )
	respawn_roaming_mob( 701025, 10086006, 90, 4, 30000 )

	respawn_roaming_mob( 701026, 10087007, 180, 0, 30000 )
	respawn_roaming_mob( 701026, 10088008, 0, 4, 30000 )
	respawn_roaming_mob( 701026, 10089005, 90, 4, 30000 )


	--숨겨진 수정 계곡 14
	--기존 영역1111080
	respawn_roaming_mob( 701027, 10080007, 180, 0, 30000 )
	respawn_roaming_mob( 701027, 10081006, 0, 4, 30000 )
	respawn_roaming_mob( 701027, 10082004, 90, 4, 30000 )

	respawn_roaming_mob( 701028, 10083002, 180, 0, 30000 )
	respawn_roaming_mob( 701028, 10084002, 0, 4, 30000 )
	respawn_roaming_mob( 701028, 10084012, 90, 4, 30000 )


	--숨겨진 수정 계곡 15
	--기존 영역1135082
	respawn_roaming_mob( 701029, 10082010, 180, 0, 30000 )
	respawn_roaming_mob( 701029, 10083006, 0, 4, 30000 )
	respawn_roaming_mob( 701029, 10084011, 90, 4, 30000 )

	respawn_roaming_mob( 701030, 10085008, 180, 0, 30000 )
	respawn_roaming_mob( 701030, 10086007, 0, 4, 30000 )
	respawn_roaming_mob( 701030, 10087008, 90, 4, 30000 )


	--숨겨진 수정 계곡 16
	--기존 영역1136085
	respawn_roaming_mob( 701031, 10085009, 180, 0, 30000 )
	respawn_roaming_mob( 701031, 10086008, 0, 4, 30000 )
	respawn_roaming_mob( 701031, 10086009, 90, 4, 30000 )

	respawn_roaming_mob( 701032, 10087009, 180, 0, 30000 )
	respawn_roaming_mob( 701032, 10088009, 0, 4, 30000 )
	respawn_roaming_mob( 701032, 10089006, 90, 4, 30000 )


	--숨겨진 수정 계곡 17
	--기존 영역1138086
	respawn_roaming_mob( 701033, 10087011, 0, 4, 30000 )
	respawn_roaming_mob( 701033, 10086010, 270, 4, 30000 )

	respawn_roaming_mob( 701034, 10088012, 90, 4, 30000 )
	respawn_roaming_mob( 701034, 10090011, 180, 0, 30000 )

	--숨겨진 수정 계곡 18
	--기존 영역1137087
	respawn_roaming_mob( 701035, 10087010, 180, 0, 30000 )
	respawn_roaming_mob( 701035, 10088011, 0, 4, 30000 )
	respawn_roaming_mob( 701035, 10088010, 90, 4, 30000 )

	respawn_roaming_mob( 701036, 10089007, 180, 0, 30000 )
	respawn_roaming_mob( 701036, 10090009, 0, 4, 30000 )
	respawn_roaming_mob( 701036, 10090010, 90, 4, 30000 )


	--숨겨진 수정 계곡 19
	--기존 영역1133088
	respawn_roaming_mob( 701037, 10088007, 180, 0, 30000 )
	respawn_roaming_mob( 701037, 10089003, 0, 4, 30000 )
	respawn_roaming_mob( 701037, 10089004, 90, 4, 30000 )

	respawn_roaming_mob( 701038, 10090006, 180, 0, 30000 )
	respawn_roaming_mob( 701038, 10090007, 0, 4, 30000 )
	respawn_roaming_mob( 701038, 10090008, 90, 4, 30000 )

	--숨겨진 수정 계곡 20
	--기존 영역1132087
	respawn_roaming_mob( 701039, 10087006, 180, 0, 30000 )
	respawn_roaming_mob( 701039, 10088006, 0, 4, 30000 )
	respawn_roaming_mob( 701039, 10089002, 90, 4, 30000 )

	respawn_roaming_mob( 701040, 10090003, 180, 0, 30000 )
	respawn_roaming_mob( 701040, 10090004, 0, 4, 30000 )
	respawn_roaming_mob( 701040, 10090005, 90, 4, 30000 )


	--숨겨진 수정 계곡 21
	--기존 영역1131087
	respawn_roaming_mob( 701041, 10087005, 180, 0, 30000 )
	respawn_roaming_mob( 701041, 10088004, 0, 4, 30000 )
	respawn_roaming_mob( 701041, 10088005, 90, 4, 30000 )

	respawn_roaming_mob( 701042, 10089001, 180, 0, 30000 )
	respawn_roaming_mob( 701042, 10090001, 0, 4, 30000 )
	respawn_roaming_mob( 701042, 10090002, 90, 4, 30000 )


	--숨겨진 수정 계곡 22
	--기존 영역1125084
	respawn_roaming_mob( 701043, 10084006, 180, 0, 30000 )
	respawn_roaming_mob( 701043, 10084007, 0, 4, 30000 )
	respawn_roaming_mob( 701043, 10085001, 90, 4, 30000 )

	respawn_roaming_mob( 701044, 10086001, 180, 0, 30000 )
	respawn_roaming_mob( 701044, 10087001, 0, 4, 30000 )
	respawn_roaming_mob( 701044, 10088001, 90, 4, 30000 )


	--숨겨진 수정 계곡 23
	--기존 영역1126084
	respawn_roaming_mob( 701045, 10084008, 180, 0, 30000 )
	respawn_roaming_mob( 701045, 10084009, 0, 4, 30000 )
	respawn_roaming_mob( 701045, 10085002, 90, 4, 30000 )

	respawn_roaming_mob( 701046, 10085003, 180, 0, 30000 )
	respawn_roaming_mob( 701046, 10086002, 0, 4, 30000 )
	respawn_roaming_mob( 701046, 10086003, 90, 4, 30000 )


	--숨겨진 수정 계곡 24
	--기존 영역1129086
	respawn_roaming_mob( 701047, 10087003, 0, 4, 30000 )
	respawn_roaming_mob( 701047, 10086005, 270, 4, 30000 )
	respawn_roaming_mob( 701047, 10087004, 90, 4, 30000 )
	respawn_roaming_mob( 701047, 10088003, 180, 0, 30000 )


	--숨겨진 수정 계곡 25
	--기존 영역1127085
	respawn_roaming_mob( 701048, 10085004, 180, 0, 30000 )
	respawn_roaming_mob( 701048, 10085005, 0, 4, 30000 )
	respawn_roaming_mob( 701048, 10085006, 90, 4, 30000 )

	respawn_roaming_mob( 701049, 10086004, 180, 0, 30000 )
	respawn_roaming_mob( 701049, 10087002, 0, 4, 30000 )
	respawn_roaming_mob( 701049, 10088002, 90, 4, 30000 )


	--숨겨진 수정 계곡 26
	--기존 영역1124082
	respawn_roaming_mob( 701050, 10082008, 270, 4, 30000 )
	respawn_roaming_mob( 701050, 10082009, 0, 4, 30000 )

	respawn_roaming_mob( 701051, 10083005, 90, 4, 30000 )
	respawn_roaming_mob( 701051, 10084005, 180, 0, 30000 )


	--숨겨진 수정 계곡 27
	--기존 영역1122080
	respawn_roaming_mob( 701052, 10080012, 180, 0, 30000 )
	respawn_roaming_mob( 701052, 10081010, 0, 4, 30000 )
	respawn_roaming_mob( 701052, 10082007, 90, 4, 30000 )

	respawn_roaming_mob( 701053, 10083004, 180, 0, 30000 )
	respawn_roaming_mob( 701053, 10084003, 0, 4, 30000 )
	respawn_roaming_mob( 701053, 10084004, 90, 4, 30000 )


	--숨겨진 수정 계곡 28
	--기존 영역1121076
	respawn_roaming_mob( 701054, 10076010, 180, 0, 30000 )
	respawn_roaming_mob( 701054, 10077009, 0, 4, 30000 )
	respawn_roaming_mob( 701054, 10078011, 90, 4, 30000 )

	--숨겨진 수정 계곡 29
	--기존 영역1119075
	respawn_roaming_mob( 701055, 10075009, 180, 0, 30000 )
	respawn_roaming_mob( 701055, 10076009, 0, 4, 30000 )
	respawn_roaming_mob( 701055, 10077008, 90, 4, 30000 )

	--숨겨진 수정 계곡 30
	--기존 영역1120080
	respawn_roaming_mob( 701057, 10080010, 270, 4, 30000 )
	respawn_roaming_mob( 701057, 10081009, 0, 4, 30000 )

	respawn_roaming_mob( 701058, 10082006, 90, 4, 30000 )
	respawn_roaming_mob( 701058, 10083003, 180, 0, 30000 )

	
	--숨겨진 수정 계곡 31
	--기존 영역1117076
	respawn_roaming_mob( 701059, 10076008, 180, 0, 30000 )
	respawn_roaming_mob( 701059, 10077006, 0, 4, 30000 )
	respawn_roaming_mob( 701059, 10078008, 90, 4, 30000 )

	respawn_roaming_mob( 701060, 10079006, 180, 0, 30000 )
	respawn_roaming_mob( 701060, 10080008, 0, 4, 30000 )
	respawn_roaming_mob( 701060, 10081007, 90, 4, 30000 )

	--1116074영역 삭제
	--숨겨진 수정 계곡 32
	--기존 영역1117076
	respawn_roaming_mob( 701061, 10076008, 180, 0, 30000 )
	respawn_roaming_mob( 701061, 10077006, 0, 4, 30000 )
	respawn_roaming_mob( 701061, 10078008, 90, 4, 30000 )

	respawn_roaming_mob( 701062, 10079006, 180, 0, 30000 )
	respawn_roaming_mob( 701062, 10080008, 0, 4, 30000 )
	respawn_roaming_mob( 701062, 10081007, 90, 4, 30000 )


	--숨겨진 수정 계곡 33
	--기존 영역1117076
	respawn_roaming_mob( 701063, 10076008, 180, 0, 30000 )
	respawn_roaming_mob( 701063, 10077006, 0, 4, 30000 )
	respawn_roaming_mob( 701063, 10078008, 90, 4, 30000 )

	respawn_roaming_mob( 701064, 10079006, 180, 0, 30000 )
	respawn_roaming_mob( 701064, 10080008, 0, 4, 30000 )
	respawn_roaming_mob( 701064, 10081007, 90, 4, 30000 )


	--숨겨진 수정 계곡 34
	--기존 영역1118077
	respawn_roaming_mob( 701065, 10077007, 180, 0, 30000 )
	respawn_roaming_mob( 701065, 10078009, 0, 4, 30000 )
	respawn_roaming_mob( 701065, 10079007, 90, 4, 30000 )

	respawn_roaming_mob( 701066, 10080009, 180, 0, 30000 )
	respawn_roaming_mob( 701066, 10081008, 0, 4, 30000 )
	respawn_roaming_mob( 701066, 10082005, 90, 4, 30000 )


	--숨겨진 수정 계곡 35
	--기존 영역1118077
	respawn_roaming_mob( 701067, 10077007, 180, 0, 30000 )
	respawn_roaming_mob( 701067, 10078009, 0, 4, 30000 )
	respawn_roaming_mob( 701067, 10079007, 90, 4, 30000 )

	respawn_roaming_mob( 701068, 10080009, 180, 0, 30000 )
	respawn_roaming_mob( 701068, 10081008, 0, 4, 30000 )
	respawn_roaming_mob( 701068, 10082005, 90, 4, 30000 )


	--숨겨진 수정 계곡 36
	--기존 영역1143088
	respawn_roaming_mob( 701069, 10088019, 180, 0, 30000 )
	respawn_roaming_mob( 701069, 10089015, 0, 4, 30000 )
	respawn_roaming_mob( 701069, 10089016, 90, 4, 30000 )

	respawn_roaming_mob( 701070, 10090016, 180, 0, 30000 )
	respawn_roaming_mob( 701070, 10090017, 0, 4, 30000 )
	respawn_roaming_mob( 701070, 10090018, 90, 4, 30000 )

	--숨겨진 수정 계곡 37
	--기존 영역1143088
	respawn_roaming_mob( 701071, 10088019, 180, 0, 30000 )
	respawn_roaming_mob( 701071, 10089015, 0, 4, 30000 )
	respawn_roaming_mob( 701071, 10089016, 90, 4, 30000 )

	respawn_roaming_mob( 701072, 10090016, 180, 0, 30000 )
	respawn_roaming_mob( 701072, 10090017, 0, 4, 30000 )
	respawn_roaming_mob( 701072, 10090018, 90, 4, 30000 )

	--숨겨진 수정 계곡 38
	--기존 영역1143088
	respawn_roaming_mob( 701073, 10088019, 180, 0, 30000 )
	respawn_roaming_mob( 701073, 10089015, 0, 4, 30000 )
	respawn_roaming_mob( 701073, 10089016, 90, 4, 30000 )

	respawn_roaming_mob( 701074, 10090016, 180, 0, 30000 )
	respawn_roaming_mob( 701074, 10090017, 0, 4, 30000 )
	respawn_roaming_mob( 701074, 10090018, 90, 4, 30000 )

	--숨겨진 수정 계곡 39
	--기존 영역1139088
	respawn_roaming_mob( 701075, 10088013, 180, 0, 30000 )
	respawn_roaming_mob( 701075, 10088014, 0, 4, 30000 )
	respawn_roaming_mob( 701075, 10089008, 90, 4, 30000 )

	respawn_roaming_mob( 701076, 10089009, 180, 0, 30000 )
	respawn_roaming_mob( 701076, 10090012, 0, 4, 30000 )
	respawn_roaming_mob( 701076, 10090013, 90, 4, 30000 )

	--숨겨진 수정 계곡 40
	--기존 영역1139088
	respawn_roaming_mob( 701077, 10088013, 180, 0, 30000 )
	respawn_roaming_mob( 701077, 10088014, 0, 4, 30000 )
	respawn_roaming_mob( 701077, 10089008, 90, 4, 30000 )

	respawn_roaming_mob( 701078, 10089009, 180, 0, 30000 )
	respawn_roaming_mob( 701078, 10090012, 0, 4, 30000 )
	respawn_roaming_mob( 701078, 10090013, 90, 4, 30000 )

	--숨겨진 수정 계곡 41
	--기존 영역1139088
	respawn_roaming_mob( 701079, 10088013, 180, 0, 30000 )
	respawn_roaming_mob( 701079, 10088014, 0, 4, 30000 )
	respawn_roaming_mob( 701079, 10089008, 90, 4, 30000 )
	
	respawn_roaming_mob( 701080, 10089009, 180, 0, 30000 )
	respawn_roaming_mob( 701080, 10090012, 0, 4, 30000 )
	respawn_roaming_mob( 701080, 10090013, 90, 4, 30000 )

	--81기존 영역1144072	
	respawn_roaming_mob( 701081, 10072005, 180, 0, 30000 )
	respawn_roaming_mob( 701081, 10072005, 0, 4, 30000 )

	--82기존 영역1148086	
	respawn_roaming_mob( 701082, 10086014, 180, 0, 30000 )
	respawn_roaming_mob( 701082, 10086014, 0, 4, 30000 )

	--83기존 영역1147088	
	respawn_roaming_mob( 701083, 10088021, 180, 0, 30000 )
	respawn_roaming_mob( 701083, 10088021, 0, 4, 30000 )

	--84기존 영역1146089	
	respawn_roaming_mob( 701084, 10089017, 180, 0, 30000 )
	respawn_roaming_mob( 701084, 10089017, 0, 4, 30000 )

	--85기존 영역1145088	
	respawn_roaming_mob( 701085, 10088020, 180, 0, 30000 )
	respawn_roaming_mob( 701085, 10088020, 0, 4, 30000 )


	--숨겨진 엘카시아 3단계 구역
	--기존 영역1701123
	respawn_roaming_mob( 1101001, 10141004, 180, 0, 30000 )
	respawn_roaming_mob( 1101001, 10141005, 0, 4, 30000 )
	respawn_roaming_mob( 1101001, 10141004, 90, 4, 30000 )

	--기존 영역1701124
	respawn_roaming_mob( 1101002, 10141004, 180, 0, 30000 )
	respawn_roaming_mob( 1101002, 10141005, 0, 4, 30000 )
	respawn_roaming_mob( 1101002, 10141004, 90, 4, 30000 )

	--기존 영역1701121
	respawn_roaming_mob( 1101003, 10141004, 180, 0, 30000 )
	respawn_roaming_mob( 1101003, 10141005, 0, 4, 30000 )
	respawn_roaming_mob( 1101003, 10141004, 90, 4, 30000 )

	--기존 영역1701122
	respawn_roaming_mob( 1101004, 10141004, 180, 0, 30000 )
	respawn_roaming_mob( 1101004, 10141005, 0, 4, 30000 )
	respawn_roaming_mob( 1101004, 10141004, 90, 4, 30000 )

	--기존 영역1701119
	respawn_roaming_mob( 1101005, 10141004, 180, 0, 30000 )
	respawn_roaming_mob( 1101005, 10141005, 0, 4, 30000 )
	respawn_roaming_mob( 1101005, 10141004, 90, 4, 30000 )

	--숨겨진 엘카시아 2단계 구역
	respawn_roaming_mob( 1101006, 10135006, 180, 0, 30000 )
	respawn_roaming_mob( 1101006, 10138003, 0, 4, 30000 )
	respawn_roaming_mob( 1101006, 10135006, 90, 4, 30000 )
	respawn_roaming_mob( 1101006, 10135008, 45, 4, 30000 )

	
	--기존 영역1114071
	respawn_roaming_mob( 1101007, 10135006, 180, 0, 30000 )
	respawn_roaming_mob( 1101007, 10138004, 0, 4, 30000 )
	respawn_roaming_mob( 1101007, 10135006, 90, 4, 30000 )
	respawn_roaming_mob( 1101007, 10135008, 45, 4, 30000 )

	respawn_roaming_mob( 1101008, 10131008, 180, 0, 30000 )
	respawn_roaming_mob( 1101008, 10130005, 0, 4, 30000 )
	respawn_roaming_mob( 1101008, 10131008, 90, 4, 30000 )

	--기존 영역1115073
	respawn_roaming_mob( 1101009, 10133007, 180, 0, 30000 )
	respawn_roaming_mob( 1101009, 10132009, 0, 4, 30000 )
	respawn_roaming_mob( 1101009, 10133007, 90, 4, 30000 )

	respawn_roaming_mob( 1101010, 10135006, 180, 0, 30000 )
	respawn_roaming_mob( 1101010, 10132009, 0, 4, 30000 )
	respawn_roaming_mob( 1101010, 10135006, 90, 4, 30000 )

	--기존 영역1104073
	respawn_roaming_mob( 1101011, 10131008, 180, 0, 30000 )
	respawn_roaming_mob( 1101011, 10132008, 0, 4, 30000 )
	respawn_roaming_mob( 1101011, 10131008, 90, 4, 30000 )

	respawn_roaming_mob( 1101012, 10132007, 180, 0, 30000 )
	respawn_roaming_mob( 1101012, 10130005, 0, 4, 30000 )
	respawn_roaming_mob( 1101012, 10132007, 90, 4, 30000 )

	--숨겨진 엘카시아 7
	--기존 영역1105075
	respawn_roaming_mob( 1101013, 10136003, 180, 0, 30000 )
	respawn_roaming_mob( 1101013, 10139005, 0, 4, 30000 )
	respawn_roaming_mob( 1101013, 10136003, 90, 4, 30000 )

	respawn_roaming_mob( 1101014, 10132008, 180, 0, 30000 )
	respawn_roaming_mob( 1101014, 10139005, 0, 4, 30000 )
	respawn_roaming_mob( 1101014, 10132008, 90, 4, 30000 )

	--숨겨진 엘카시아 8
	--기존 영역1106076
	respawn_roaming_mob( 1101015, 10134007, 180, 0, 30000 )
	respawn_roaming_mob( 1101015, 10136003, 0, 4, 30000 )
	respawn_roaming_mob( 1101015, 10134007, 90, 4, 30000 )

	respawn_roaming_mob( 1101016, 10126006, 180, 0, 30000 )
	respawn_roaming_mob( 1101016, 10125006, 0, 4, 30000 )
	respawn_roaming_mob( 1101016, 10126006, 90, 4, 30000 )

	--숨겨진 엘카시아 9
	--기존 영역1108076
	respawn_roaming_mob( 1101017, 10127004, 180, 0, 30000 )
	respawn_roaming_mob( 1101017, 10128003, 0, 4, 30000 )
	respawn_roaming_mob( 1101017, 10127004, 90, 4, 30000 )

	respawn_roaming_mob( 1101018, 10125005, 180, 0, 30000 )
	respawn_roaming_mob( 1101018, 10125006, 0, 4, 30000 )
	respawn_roaming_mob( 1101018, 10125005, 90, 4, 30000 )

	--숨겨진 엘카시아 10
	--기존 영역1101070
	respawn_roaming_mob( 1101019, 10125007, 180, 0, 30000 )
	respawn_roaming_mob( 1101019, 10128004, 0, 4, 30000 )
	respawn_roaming_mob( 1101019, 10125007, 90, 4, 30000 )

	respawn_roaming_mob( 1101020, 10126005, 180, 0, 30000 )
	respawn_roaming_mob( 1101020, 10130005, 0, 4, 30000 )
	respawn_roaming_mob( 1101020, 10129006, 90, 4, 30000 )


	--숨겨진 엘카시아 11
	--기존 영역1109078
	respawn_roaming_mob( 1101021, 10134007, 180, 0, 30000 )
	respawn_roaming_mob( 1101021, 10136004, 0, 4, 30000 )
	respawn_roaming_mob( 1101021, 10134007, 90, 4, 30000 )

	respawn_roaming_mob( 1101022, 10134006, 180, 0, 30000 )
	respawn_roaming_mob( 1101022, 10135006, 0, 4, 30000 )
	respawn_roaming_mob( 1101022, 10134006, 90, 4, 30000 )

	--숨겨진 엘카시아 12
	--기존 영역1109078
	respawn_roaming_mob( 1101023, 10125005, 180, 0, 30000 )
	respawn_roaming_mob( 1101023, 10128003, 0, 4, 30000 )
	respawn_roaming_mob( 1101023, 10125005, 90, 4, 30000 )

	respawn_roaming_mob( 1101024, 10134007, 180, 0, 30000 )
	respawn_roaming_mob( 1101024, 10136004, 0, 4, 30000 )
	respawn_roaming_mob( 1101024, 10134007, 90, 4, 30000 )


--respawn_roaming_mob( 그룹 로밍 ID, 리스폰 몬스터 ID, 방향 (각도), 로밍 동선과의 거리(M미터), 리스폰 주기(초) )
	--숨겨진 엘 카시아 금벽장 1 
	respawn_roaming_mob( 1101025, 141007, 45, 4, 30000 )
	respawn_roaming_mob( 1101025, 144008, 180, 0, 30000 ) -- 미스틱 지니
	respawn_roaming_mob( 1101025, 141007, 0, 4, 30000 )
	respawn_roaming_mob( 1101025, 141007, 90, 4, 30000 )
	respawn_roaming_mob( 1101025, 141007, 270, 4, 30000 )
	--respawn_roaming_mob( 7001, 141007, 180, 4, 30000 )
	--respawn_roaming_mob( 7001, 141007, 270, 4, 30000 )
	
	--숨겨진 엘 카시아 독벽장 1
	respawn_roaming_mob( 1101026, 10141008, 45, 4, 30000 )
	respawn_roaming_mob( 1101026, 10144011, 180, 0, 30000 ) -- 에델 아우게 매지션
	respawn_roaming_mob( 1101026, 10141008, 0, 4, 30000 )
	respawn_roaming_mob( 1101026, 10141008, 90, 4, 30000 )
	respawn_roaming_mob( 1101026, 10141008, 270, 4, 30000 )
	--respawn_roaming_mob( 7002, 10141008, 180, 4, 30000 )
	--respawn_roaming_mob( 7002, 10141008, 270, 4, 30000 )

	--숨겨진 엘 카시아 수벽장 1
	respawn_roaming_mob( 1101027, 10141005, 45, 4, 30000 )
	respawn_roaming_mob( 1101027, 10144010, 180, 0, 30000 ) -- 타파리
	respawn_roaming_mob( 1101027, 10141005, 0, 4, 30000 )
	respawn_roaming_mob( 1101027, 10141005, 90, 4, 30000 )
	respawn_roaming_mob( 1101027, 10141005, 270, 4, 30000 )
	--respawn_roaming_mob( 7003, 10141005, 180, 4, 30000 )
	--respawn_roaming_mob( 7003, 10141005, 270, 4, 30000 )

	--숨겨진 엘 카시아 금벽장 2
	respawn_roaming_mob( 1101028, 10141007, 45, 4, 30000 )
	respawn_roaming_mob( 1101028, 10144007, 180, 0, 30000 ) -- 지니
	respawn_roaming_mob( 1101028, 10141007, 0, 4, 30000 )
	respawn_roaming_mob( 1101028, 10141007, 90, 4, 30000 )
	respawn_roaming_mob( 1101028, 10141007, 90, 4, 30000 )
	--respawn_roaming_mob( 7004, 10141007, 180, 4, 30000 )
	--respawn_roaming_mob( 7004, 10141007, 270, 4, 30000 )

	--숨겨진 엘 카시아 독벽장 2
	respawn_roaming_mob( 1101029, 10141009, 45, 4, 30000 )
	respawn_roaming_mob( 1101029, 10144012, 180, 0, 30000 ) -- 에델 아우게 레인저
	respawn_roaming_mob( 1101029, 10141009, 0, 4, 30000 )
	respawn_roaming_mob( 1101029, 10141009, 90, 4, 30000 )
	respawn_roaming_mob( 1101029, 10141009, 270, 4, 30000 )
	--respawn_roaming_mob( 7005, 10141009, 180, 4, 30000 )
	--respawn_roaming_mob( 7005, 10141009, 270, 4, 30000 )

	--숨겨진 엘 카시아 수벽장 2
	respawn_roaming_mob( 1101030, 10141004, 45, 4, 30000 )
	respawn_roaming_mob( 1101030, 10144009, 180, 0, 30000 ) -- 아이무스
	respawn_roaming_mob( 1101030, 10141004, 0, 4, 30000 )
	respawn_roaming_mob( 1101030, 10141004, 90, 4, 30000 )
	respawn_roaming_mob( 1101030, 10141004, 270, 4, 30000 )
	--respawn_roaming_mob( 7006, 10141004, 180, 4, 30000 )
	--respawn_roaming_mob( 7006, 10141004, 270, 4, 30000 )


	--숨겨진 백룡의 쉼터 1
	--기존 영역1401001 
	respawn_roaming_mob( 1001001, 10146008, 180, 0, 30000 )
	respawn_roaming_mob( 1001001, 10148008, 0, 4, 30000 )

	respawn_roaming_mob( 1001002, 10149008, 180, 0, 30000 )
	respawn_roaming_mob( 1001002, 10150004, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 2
	--기존 영역1401011
	respawn_roaming_mob( 1001003, 10150004, 180, 0, 30000 )
	respawn_roaming_mob( 1001003, 10151003, 0, 4, 30000 )
	respawn_roaming_mob( 1001003, 10152003, 90, 4, 30000 )

	respawn_roaming_mob( 1001004, 10152009, 180, 0, 30000 )
	respawn_roaming_mob( 1001004, 10151008, 0, 4, 30000 )
	respawn_roaming_mob( 1001004, 10152008, 90, 4, 30000 )

	--숨겨진 백룡의 쉼터 3
	--기존 영역1401021
	respawn_roaming_mob( 1001005, 10150004, 180, 0, 30000 )
	respawn_roaming_mob( 1001005, 10151003, 0, 4, 30000 )
	respawn_roaming_mob( 1001005, 10152003, 90, 4, 30000 )
	respawn_roaming_mob( 1001005, 10152009, 270, 4, 30000 )

	--숨겨진 백룡의 쉼터 4
	--기존 영역1401001
	respawn_roaming_mob( 1001006, 10147004, 180, 0, 30000 )
	respawn_roaming_mob( 1001006, 10148007, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 5
	--기존 영역1401041
	respawn_roaming_mob( 1001007, 10148005, 180, 0, 30000 )
	respawn_roaming_mob( 1001007, 10149005, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 6
	--기존 영역1401041
	respawn_roaming_mob( 1001008, 10150005, 180, 0, 30000 )
	respawn_roaming_mob( 1001008, 10149006, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 7
	--기존 영역1401031
	respawn_roaming_mob( 1001009, 10148005, 180, 0, 30000 )
	respawn_roaming_mob( 1001009, 10149005, 0, 4, 30000 )

	respawn_roaming_mob( 1001010, 10150005, 180, 0, 30000 )
	respawn_roaming_mob( 1001010, 10150006, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 8
	--기존 영역1401061
	respawn_roaming_mob( 1001011, 10148005, 180, 0, 30000 )
	respawn_roaming_mob( 1001011, 10149005, 0, 4, 30000 )

	--숨겨진 백룔의 쉼터 9
	--기존 영역1401061
	respawn_roaming_mob( 1001012, 10150005, 90, 4, 30000 )
	respawn_roaming_mob( 1001012, 10149006, 0, 0, 30000 )
	respawn_roaming_mob( 1001012, 10150006, 180, 0, 30000 )

	--숨겨진 백룡의 쉼터 10
	--기존 영역1401051
	respawn_roaming_mob( 1001013, 10148005, 180, 0, 30000 )
	respawn_roaming_mob( 1001013, 10149005, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 11
	--기존 영역1403001
	respawn_roaming_mob( 1001014, 10148006, 180, 0, 30000 )
	respawn_roaming_mob( 1001014, 10148006, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 12
	--기존 영역1403011
	respawn_roaming_mob( 1001015, 10148006, 180, 0, 30000 )
	respawn_roaming_mob( 1001015, 10148006, 0, 4, 30000 )


	--숨겨진 백룡의 쉼터 13
	--기존 영역1401081
	respawn_roaming_mob( 1001016, 10155006, 180, 0, 30000 )
	respawn_roaming_mob( 1001016, 10156006, 0, 4, 30000 )
	respawn_roaming_mob( 1001016, 10157006, 90, 4, 30000 )

	respawn_roaming_mob( 1001017, 10157012, 180, 0, 30000 )
	respawn_roaming_mob( 1001017, 10157013, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 14
	--기존 영역1401071
	respawn_roaming_mob( 1001018, 10155006, 180, 0, 30000 )
	respawn_roaming_mob( 1001018, 10156006, 0, 4, 30000 )
	respawn_roaming_mob( 1001018, 10157006, 90, 4, 30000 )

	respawn_roaming_mob( 1001019, 10156010, 180, 0, 30000 )
	respawn_roaming_mob( 1001019, 10157010, 0, 4, 30000 )


	--숨겨진 백룡의 쉼터 15
	--기존 영역1401091
	respawn_roaming_mob( 1001020, 10155013, 180, 0, 30000 )
	respawn_roaming_mob( 1001020, 10157012, 0, 4, 30000 )
	respawn_roaming_mob( 1001020, 10156012, 90, 4, 30000 )

	--숨겨진 백룡의 쉼터 16
	--기존 영역1401101
	respawn_roaming_mob( 1001021, 10156006, 180, 0, 30000 )
	respawn_roaming_mob( 1001021, 10157013, 0, 4, 30000 )
	respawn_roaming_mob( 1001021, 10157006, 90, 4, 30000 )

	--숨겨진 백룡의 쉼터 17
	--기존 영역1401111
	respawn_roaming_mob( 1001022, 10156006, 180, 0, 30000 )
	respawn_roaming_mob( 1001022, 10157006, 0, 4, 30000 )
	respawn_roaming_mob( 1001022, 10157013, 90, 4, 30000 )

	--숨겨진 백룡의 쉼터 18
	--기존 영역1401121
	respawn_roaming_mob( 1001023, 10156006, 180, 0, 30000 )
	respawn_roaming_mob( 1001023, 10157006, 0, 4, 30000 )
	respawn_roaming_mob( 1001023, 10157013, 90, 4, 30000 )

	--숨겨진 백룡의 쉼터 19
	--기존 영역1401151
	respawn_roaming_mob( 1001024, 10155013, 180, 0, 30000 )
	respawn_roaming_mob( 1001024, 10156012, 0, 4, 30000 )
	respawn_roaming_mob( 1001024, 10157006, 90, 4, 30000 )

	--숨겨진 백룡의 쉼터 20
	--기존 영역1401161
	respawn_roaming_mob( 1001025, 10156006, 180, 0, 30000 )
	respawn_roaming_mob( 1001025, 10157013, 0, 4, 30000 )
	respawn_roaming_mob( 1001025, 10157007, 90, 4, 30000 )

	--숨겨진 백룡의 쉼터 21
	--기존 영역1401171
	respawn_roaming_mob( 1001026, 10157008, 180, 0, 30000 )
	respawn_roaming_mob( 1001026, 10157013, 0, 4, 30000 )
	respawn_roaming_mob( 1001026, 10157012, 90, 4, 30000 )

	respawn_roaming_mob( 1001027, 10156008, 180, 0, 30000 )
	respawn_roaming_mob( 1001027, 10157002, 0, 4, 30000 )

	--숨겨진 백룡의 쉼터 22
	--기존 영역1401171
	respawn_roaming_mob( 1001028, 10156008, 180, 0, 30000 )
	respawn_roaming_mob( 1001028, 10157013, 0, 4, 30000 )
	respawn_roaming_mob( 1001028, 10156012, 90, 4, 30000 )


	--숨겨진 백룡의 쉼터 23
	--기존 영역1401131
	respawn_roaming_mob( 1001029, 10156012, 45, 0, 30000 )
	respawn_roaming_mob( 1001029, 10157013, 0, 4, 30000 )
	respawn_roaming_mob( 1001029, 10157012, 225, 4, 30000 )

	--숨겨진 백룡의 쉼터 23
	--기존 영역1401141
	respawn_roaming_mob( 1001030, 10157006, 45, 0, 30000 )
	respawn_roaming_mob( 1001030, 10157013, 0, 4, 30000 )
	respawn_roaming_mob( 1001030, 10157007, 225, 4, 30000 )

	--숨겨진 흑룡의 쉼터 1
	--기존 영역151001 
	respawn_roaming_mob( 901001, 10158003, 90, 0, 30000 )
	respawn_roaming_mob( 901001, 10159003, 0, 4, 30000 )

	respawn_roaming_mob( 901002, 10161002, 90, 0, 30000 )
	respawn_roaming_mob( 901002, 10161003, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 2
	--기존 영역1501011
	respawn_roaming_mob( 901003, 10158004, 180, 0, 30000 )
	respawn_roaming_mob( 901003, 10161003, 0, 4, 30000 )
	respawn_roaming_mob( 901003, 10160002, 90, 4, 30000 )

	respawn_roaming_mob( 901004, 10162003, 180, 0, 30000 )
	respawn_roaming_mob( 901004, 10163004, 0, 4, 30000 )
	respawn_roaming_mob( 901004, 10162005, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 3
	--기존 영역1501021
	respawn_roaming_mob( 901005, 10162002, 180, 0, 30000 )
	respawn_roaming_mob( 901005, 10160003, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 4
	--기존 영역1501041
	respawn_roaming_mob( 901006, 10159002, 180, 0, 30000 )
	respawn_roaming_mob( 901006, 10160004, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 5
	--기존 영역1501041
	respawn_roaming_mob( 901007, 10163004, 180, 0, 30000 )
	respawn_roaming_mob( 901007, 10163004, 0, 4, 30000 )
	respawn_roaming_mob( 901007, 10162003, 90, 4, 30000 )
	respawn_roaming_mob( 901007, 10161003, 270, 4, 30000 )


	--숨겨진 흑룡의 쉼터 6
	--기존 영역1501031
	respawn_roaming_mob( 901008, 10159002, 180, 0, 30000 )
	respawn_roaming_mob( 901008, 10160004, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 7
	--기존 영역1501041
	respawn_roaming_mob( 901009, 10162002, 180, 0, 30000 )
	respawn_roaming_mob( 901009, 10160003, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 8
	--기존 영역1501031
	respawn_roaming_mob( 901010, 10159002, 180, 0, 30000 )
	respawn_roaming_mob( 901010, 10160004, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 9
	--기존 영역1501031
	respawn_roaming_mob( 901011, 10162002, 180, 0, 30000 )
	respawn_roaming_mob( 901011, 10161005, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 10   
	--기존 영역1501031
	respawn_roaming_mob( 901012, 10159002, 180, 0, 30000 )
	respawn_roaming_mob( 901012, 10160004, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 11
	--기존 영역1501061
	respawn_roaming_mob( 901013, 10159002, 180, 0, 30000 )
	respawn_roaming_mob( 901013, 10162002, 0, 4, 30000 )
	respawn_roaming_mob( 901013, 10160004, 90, 0, 30000 )

	--숨겨진 흑룡의 쉼터 12	
	--기존 영역1501051
	respawn_roaming_mob( 901014, 10159002, 180, 0, 30000 )
	respawn_roaming_mob( 901014, 10162002, 0, 4, 30000 )
	respawn_roaming_mob( 901014, 10161005, 90, 4, 30000 )


	--숨겨진 흑룡의 쉼터 13		
	--기존 영역1501051
	respawn_roaming_mob( 901015, 10160004, 180, 0, 30000 )
	respawn_roaming_mob( 901015, 10160003, 0, 4, 30000 )


	--숨겨진 흑룡의 쉼터 14
	--기존 영역1401071
	respawn_roaming_mob( 901016, 10163004, 0, 4, 30000 )
	respawn_roaming_mob( 901016, 10162005, 180, 4, 30000 )


	--숨겨진 흑룡의 쉼터 15
	--기존 영역1503001
	respawn_roaming_mob( 901017, 10158005, 180, 0, 30000 )
	respawn_roaming_mob( 901017, 10159004, 0, 4, 30000 )


	--숨겨진 흑룡의 쉼터 16
	--기존 영역1503011
	respawn_roaming_mob( 901018, 10158005, 180, 0, 30000 )
	respawn_roaming_mob( 901018, 10159004, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 17
	--기존 영역1501081
	respawn_roaming_mob( 901019, 10164001, 180, 0, 30000 )
	respawn_roaming_mob( 901019, 10166002, 0, 4, 30000 )
	respawn_roaming_mob( 901019, 10168002, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 18
	--기존 영역1501081
	respawn_roaming_mob( 901020, 10165003, 180, 0, 30000 )
	respawn_roaming_mob( 901020, 10167004, 0, 4, 30000 )
	respawn_roaming_mob( 901020, 10169004, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 19
	--기존 영역1501071
	respawn_roaming_mob( 901021, 10164001, 180, 0, 30000 )
	respawn_roaming_mob( 901021, 10166002, 0, 4, 30000 )
	respawn_roaming_mob( 901021, 10165003, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 20	11
	--기존 영역1501071
	respawn_roaming_mob( 901022, 10167004, 180, 0, 30000 )
	respawn_roaming_mob( 901022, 10169004, 0, 4, 30000 )

	--숨겨진 흑룡의 쉼터 21
	--기존 영역1501091
	respawn_roaming_mob( 901023, 10165002, 180, 0, 30000 )
	respawn_roaming_mob( 901023, 10167002, 0, 4, 30000 )
	respawn_roaming_mob( 901023, 10168002, 45, 4, 30000 )
	respawn_roaming_mob( 901023, 10169002, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 22 		11
	--기존 영역1501101
	respawn_roaming_mob( 901024, 10169003, 180, 0, 30000 )
	respawn_roaming_mob( 901024, 10166002, 0, 4, 30000 )
	respawn_roaming_mob( 901024, 10168002, 45, 4, 30000 )
	respawn_roaming_mob( 901024, 10169003, 90, 4, 30000 )


	--숨겨진 흑룡의 쉼터 23
	--기존 영역1501121
	respawn_roaming_mob( 901025, 10165002, 180, 0, 30000 )
	respawn_roaming_mob( 901025, 10167002, 0, 4, 30000 )
	respawn_roaming_mob( 901025, 10168002, 45, 4, 30000 )
	respawn_roaming_mob( 901025, 10169002, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 24
	--기존 영역1501111
	respawn_roaming_mob( 901026, 10169003, 180, 0, 30000 )
	respawn_roaming_mob( 901026, 10166002, 0, 4, 30000 )
	respawn_roaming_mob( 901026, 10168002, 45, 4, 30000 )
	respawn_roaming_mob( 901026, 10169003, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 25
	--기존 영역1501131 
	respawn_roaming_mob( 901027, 10165002, 180, 0, 30000 )
	respawn_roaming_mob( 901027, 10167002, 0, 4, 30000 )
	respawn_roaming_mob( 901027, 10168002, 45, 4, 30000 )
	respawn_roaming_mob( 901027, 10169002, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 26			
	--기존 영역1501141 
	respawn_roaming_mob( 901028, 10169003, 180, 0, 30000 )
	respawn_roaming_mob( 901028, 10166002, 0, 4, 30000 )
	respawn_roaming_mob( 901028, 10168002, 45, 4, 30000 )
	respawn_roaming_mob( 901028, 10169003, 90, 4, 30000 )

	--숨겨진 흑룡의 쉼터 27
	--기존 영역1501151
	respawn_roaming_mob( 901029, 10167005, 180, 0, 30000 )
	respawn_roaming_mob( 901029, 10168004, 0, 4, 30000 )
	respawn_roaming_mob( 901029, 10169005, 45, 4, 30000 )

	--숨겨진 흑룡의 쉼터 28
	--기존 영역1501161
	respawn_roaming_mob( 901030, 10169002, 180, 0, 30000 )
	respawn_roaming_mob( 901030, 10167005, 0, 4, 30000 )
	respawn_roaming_mob( 901030, 10168004, 45, 4, 30000 )


	--숨겨진 흑룡의 쉼터 29
	--기존 영역1501171 
	respawn_roaming_mob( 901031, 10169003, 180, 0, 30000 )
	respawn_roaming_mob( 901031, 10167002, 0, 4, 30000 )
	respawn_roaming_mob( 901031, 10167005, 45, 4, 30000 )


	--숨겨진 흑룡의 쉼터 30
	--기존 영역1501171 
	respawn_roaming_mob( 901032, 10169005, 90, 4, 30000 )
	respawn_roaming_mob( 901032, 10168004, 0, 0, 30000 )
	respawn_roaming_mob( 901032, 10169003, 180, 4, 30000 )


	--숨겨진 사룡의 심장 1
	--기존 영역1601001 
	respawn_roaming_mob( 801001, 10165005, 90, 0, 30000 )
	respawn_roaming_mob( 801001, 10166004, 0, 4, 30000 )

	respawn_roaming_mob( 801002, 10165004, 90, 0, 30000 )
	respawn_roaming_mob( 801002, 10166004, 0, 4, 30000 )

	--숨겨진 사룡의 심장 2
	--기존 영역1601001
	respawn_roaming_mob( 801003, 10166004, 180, 0, 30000 )
	respawn_roaming_mob( 801003, 10166004, 0, 4, 30000 )

	--숨겨진 사룡의 심장 3
	--기존 영역1601011
	respawn_roaming_mob( 801004, 10166004, 180, 0, 30000 )
	respawn_roaming_mob( 801004, 10166005, 0, 4, 30000 )
	respawn_roaming_mob( 801004, 10166004, 90, 4, 30000 )


	--숨겨진 사룡의 심장 4
	--기존 영역1601011
	respawn_roaming_mob( 801005, 10166005, 180, 0, 30000 )
	respawn_roaming_mob( 801005, 10166005, 0, 4, 30000 )
	respawn_roaming_mob( 801005, 10166005, 90, 4, 30000 )

	--숨겨진 사룡의 심장 5
	--기존 영역1601021 
	respawn_roaming_mob( 801006, 10167006, 180, 0, 30000 )
	respawn_roaming_mob( 801006, 10167008, 0, 4, 30000 )
	respawn_roaming_mob( 801006, 10167007, 90, 4, 30000 )

	--숨겨진 사룡의 심장 6
	--기존 영역1601021 
	respawn_roaming_mob( 801007, 10167007, 180, 0, 30000 )
	respawn_roaming_mob( 801007, 10168005, 0, 4, 30000 )
	respawn_roaming_mob( 801007, 10168005, 90, 4, 30000 )

	--숨겨진 사룡의 심장 7
	--기존 영역1601041 
	respawn_roaming_mob( 801008, 10170002, 180, 0, 30000 )
	respawn_roaming_mob( 801008, 10170003, 0, 4, 30000 )
	respawn_roaming_mob( 801008, 10170002, 90, 4, 30000 )
	respawn_roaming_mob( 801008, 10170002, 270, 4, 30000 )

	--숨겨진 사룡의 심장 8
	--기존 영역1601041 
	respawn_roaming_mob( 801009, 10170002, 90, 0, 30000 )
	respawn_roaming_mob( 801009, 10170003, 0, 4, 30000 )

	--숨겨진 사룡의 심장 9
	--기존 영역1601041 
	respawn_roaming_mob( 801010, 10170003, 90, 0, 30000 )
	respawn_roaming_mob( 801010, 10170002, 0, 4, 30000 )

	--숨겨진 사룡의 심장 9
	--기존 영역1601031 
	respawn_roaming_mob( 801011, 10168005, 90, 0, 30000 )
	respawn_roaming_mob( 801011, 10168005, 0, 4, 30000 )

	--숨겨진 사룡의 심장 10   
	--기존 영역1601031 
	respawn_roaming_mob( 801012, 10168006, 90, 0, 30000 )
	respawn_roaming_mob( 801012, 10168006, 0, 4, 30000 )

	--숨겨진 사룡의 심장 11
	--기존 영역1601061
	respawn_roaming_mob( 801013, 10170004, 180, 0, 30000 )
	respawn_roaming_mob( 801013, 10170004, 0, 4, 30000 )
	respawn_roaming_mob( 801013, 10170004, 90, 0, 30000 )

	--숨겨진 사룡의 심장 12	
	--기존 영역1601051 
	respawn_roaming_mob( 801014, 10170005, 180, 0, 30000 )
	respawn_roaming_mob( 801014, 10170005, 0, 4, 30000 )
	respawn_roaming_mob( 801014, 10170004, 90, 4, 30000 )


	--숨겨진 사룡의 심장 13		
	--기존 영역1501051
	respawn_roaming_mob( 801015, 10172001, 90, 0, 30000 )
	respawn_roaming_mob( 801015, 10173001, 0, 4, 30000 )


	--숨겨진 사룡의 심장 14
	--기존 영역1603001
	respawn_roaming_mob( 801016, 10185002, 0, 4, 30000 )
	respawn_roaming_mob( 801016, 10185002, 180, 4, 30000 )


	--숨겨진 사룡의 심장 15
	--기존 영역1603011 
	respawn_roaming_mob( 801017, 10185002, 180, 0, 30000 )
	respawn_roaming_mob( 801017, 10185002, 0, 4, 30000 )


	--숨겨진 사룡의 심장 17
	--기존 영역1601081
	respawn_roaming_mob( 801019, 10175002, 180, 0, 30000 )
	respawn_roaming_mob( 801019, 10180002, 0, 4, 30000 )
	respawn_roaming_mob( 801019, 10180001, 90, 4, 30000 )

	--숨겨진 사룡의 심장 18
	--기존 영역1601081
	respawn_roaming_mob( 801020, 10180002, 180, 0, 30000 )
	respawn_roaming_mob( 801020, 10180002, 0, 4, 30000 )
	respawn_roaming_mob( 801020, 10180001, 90, 4, 30000 )

	--숨겨진 사룡의 심장 19
	--기존 영역1601071
	respawn_roaming_mob( 801021, 10174001, 180, 0, 30000 )
	respawn_roaming_mob( 801021, 10175003, 0, 4, 30000 )
	respawn_roaming_mob( 801021, 10175001, 90, 4, 30000 )

	--숨겨진 사룡의 심장 20	
	--기존 영역1601071
	respawn_roaming_mob( 801022, 10174001, 180, 0, 30000 )
	respawn_roaming_mob( 801022, 10175003, 0, 4, 30000 )
	respawn_roaming_mob( 801022, 10175001, 90, 4, 30000 )

	--숨겨진 사룡의 심장 21
	--기존 영역1601091
	respawn_roaming_mob( 801023, 10181001, 180, 0, 30000 )
	respawn_roaming_mob( 801023, 10182001, 0, 4, 30000 )
	respawn_roaming_mob( 801023, 10181002, 45, 4, 30000 )
	respawn_roaming_mob( 801023, 10181001, 90, 4, 30000 )

	--숨겨진 사룡의 심장 22 			
	--기존 영역1601101
	respawn_roaming_mob( 801024, 10182001, 180, 0, 30000 )
	respawn_roaming_mob( 801024, 10182001, 0, 4, 30000 )
	respawn_roaming_mob( 801024, 10182002, 45, 4, 30000 )
	respawn_roaming_mob( 801024, 10181001, 90, 4, 30000 )


	--숨겨진 사룡의 심장 23
	--기존 영역1601111
	respawn_roaming_mob( 801025, 10182002, 180, 0, 30000 )
	respawn_roaming_mob( 801025, 10183001, 0, 4, 30000 )
	respawn_roaming_mob( 801025, 10183004, 45, 4, 30000 )
	respawn_roaming_mob( 801025, 10183004, 90, 4, 30000 )

	--숨겨진 사룡의 심장 24
	--기존 영역1601121
	respawn_roaming_mob( 801026, 10183004, 180, 0, 30000 )
	respawn_roaming_mob( 801026, 10183002, 0, 4, 30000 )
	respawn_roaming_mob( 801026, 10183003, 45, 4, 30000 )
	respawn_roaming_mob( 801026, 10183002, 90, 4, 30000 )

	--숨겨진 사룡의 심장 25
	--기존 영역1601131
	respawn_roaming_mob( 801027, 10183003, 180, 0, 30000 )
	respawn_roaming_mob( 801027, 10184003, 0, 4, 30000 )
	respawn_roaming_mob( 801027, 10184001, 45, 4, 30000 )
	respawn_roaming_mob( 801027, 10184001, 90, 4, 30000 )

	--숨겨진 사룡의 심장 26		11	
	--기존 영역1601141 
	respawn_roaming_mob( 801028, 10184001, 180, 0, 30000 )
	respawn_roaming_mob( 801028, 10184003, 0, 4, 30000 )
	respawn_roaming_mob( 801028, 10184001, 45, 4, 30000 )
	respawn_roaming_mob( 801028, 10184001, 90, 4, 30000 )

	--숨겨진 사룡의 심장 27
	--기존 영역1601151
	respawn_roaming_mob( 801029, 10184003, 180, 0, 30000 )
	respawn_roaming_mob( 801029, 10184002, 0, 4, 30000 )
	respawn_roaming_mob( 801029, 10184002, 45, 4, 30000 )

	--숨겨진 사룡의 심장 28
	--기존 영역1601161
	respawn_roaming_mob( 801030, 10185001, 180, 0, 30000 )
	respawn_roaming_mob( 801030, 10185004, 0, 4, 30000 )
	respawn_roaming_mob( 801030, 10185001, 45, 4, 30000 )


	--숨겨진 사룡의 심장 29
	--기존 영역1601171 
	respawn_roaming_mob( 801031, 10185004, 90, 0, 30000 )
	respawn_roaming_mob( 801031, 10185004, 0, 4, 30000 )


	--숨겨진 사룡의 심장 30
	--기존 영역1601171 
	respawn_roaming_mob( 801032, 10185004, 90, 4, 30000 )
	respawn_roaming_mob( 801032, 10185004, 0, 0, 30000 )


	--숨겨진 사룡의 심장 31
	--기존 영역1601171 
	respawn_roaming_mob( 801033, 10184002, 90, 4, 30000 )
	respawn_roaming_mob( 801033, 10184002, 0, 0, 30000 )

	--잃어버린 섬 확장 지역 카이넨 호수 설원 마르두카 캠프 위
	--퀸 이바
	respawn_roaming_mob( 801034, 170003, 0, 0, 300 )

	--잃어버린 섬 확장 지역 유니콘의 숲 A
	--푸른 불꽃
	respawn_roaming_mob( 801035, 175002, 0, 0, 30000 )

	--잃어버린 섬 확장 지역 유니콘의 숲 B
	--붉은 숨
	respawn_roaming_mob( 801036, 175003, 0, 0, 60000 )
	
	
-------------------------------------------------------------
-------------------------------------------------------------
	--respawn_roaming_mob( 그룹 로밍 ID, 리스폰 몬스터 ID, 방향 (각도), 로밍 동선과의 거리(M미터), 리스폰 주기(초) 
-------------------------------------------------------------
-------------------------------------------------------------
	local state_code = get_local_info()
	local current_time = get_os_date( "%Y-%m-%d %H:%M:%S" )
	local check = 0	
	
	if state_code == 1 and '2017-10-20 10:00:00' <= current_time and current_time < '2017-11-16 10:00:00' then
		check = 1
	elseif state_code == 16 and '2016-11-08 11:00:00' <= current_time and current_time < '2016-11-29 11:00:00' then
		check = 1
	elseif state_code == 256 and '2017-10-30 11:00:00' <= current_time and current_time < '2017-11-13 11:00:00' then
		check = 1
	end
	
	if check == 1 then
		-- 할로윈 원더랜드 : 공동묘지 3
		-- 네오 빨간종이
		respawn_roaming_mob( 8019, 3150013, 0, 0, 3600 )
		
		-- 할로윈 원더랜드 : 나이트메어 1
		-- 네오 빨간종이
		respawn_roaming_mob( 8020, 3150016, 0, 0, 3600 )
		
		-- 할로윈 원더랜드 : 나이트메어 2
		-- 네오 빨간종이
		respawn_roaming_mob( 8021, 3150015, 0, 0, 3600 )
		respawn_roaming_mob( 8018, 3150017, 0, 0, 3600 )
	end
	
-------------------------------------------------------------
-------------------------------------------------------------
--[[2014 부활절 이벤트 용
-------------------------------------------------------------
-------------------------------------------------------------	
	
	-- 할로윈 원더랜드 : 공동묘지 3
	-- 부활절 보스 펑크 토끼
	respawn_roaming_mob( 8019, 21190328, 0, 0, 1800 )
	
	-- 할로윈 원더랜드 : 나이트메어 1
	-- 부활절 보스 펑크 토끼
	respawn_roaming_mob( 8020, 21190329, 0, 0, 1800 )
	
	-- 할로윈 원더랜드 : 나이트메어 2
	-- 부활절 보스 펑크 토끼
	respawn_roaming_mob( 8021, 21190330, 0, 0, 1800 )
	]]
-------------------------------------------------------------
-------------------------------------------------------------
--2017 일본 몬스터 소환 이벤트용
-------------------------------------------------------------
-------------------------------------------------------------	
--respawn_roaming_mob( 그룹 로밍 ID, 리스폰 몬스터 ID, 방향 (각도), 로밍 동선과의 거리(M미터), 리스폰 주기(초) )	
	if state_code == 16 then
		-- 바질리스크
		respawn_roaming_mob( 801100, 20190086, 0, 0, 60000 )
		respawn_roaming_mob( 801100, 20190085, 0, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 36, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 72, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 108, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 144, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 180, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 216, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 252, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 288, 4, 60000 )
		respawn_roaming_mob( 801100, 20190085, 324, 4, 60000 )

		--메두사
		respawn_roaming_mob( 801101, 20190087, 0, 0, 60000 )
		respawn_roaming_mob( 801101, 20190085, 0, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 36, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 72, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 108, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 144, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 180, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 216, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 252, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 288, 4, 60000 )
		respawn_roaming_mob( 801101, 20190085, 324, 4, 60000 )
		
		--타란튤라
		respawn_roaming_mob( 801102, 20190088, 0, 0, 60000 )
		respawn_roaming_mob( 801102, 20190085, 0, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 36, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 72, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 108, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 144, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 180, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 216, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 252, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 288, 4, 60000 )
		respawn_roaming_mob( 801102, 20190085, 324, 4, 60000 )
		
		--블랙 위도우
		respawn_roaming_mob( 801103, 20190089, 0, 0, 60000 )
		respawn_roaming_mob( 801103, 20190085, 0, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 36, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 72, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 108, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 144, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 180, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 216, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 252, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 288, 4, 60000 )
		respawn_roaming_mob( 801103, 20190085, 324, 4, 60000 )
		
		--에우오플로케팔루스
		respawn_roaming_mob( 801104, 20190090, 0, 0, 60000 )
		respawn_roaming_mob( 801104, 20190085, 0, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 36, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 72, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 108, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 144, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 180, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 216, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 252, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 288, 4, 60000 )
		respawn_roaming_mob( 801104, 20190085, 324, 4, 60000 )
		
		--미크로랍토르
		respawn_roaming_mob( 801105, 20190091, 0, 0, 60000 )
		respawn_roaming_mob( 801105, 20190085, 0, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 36, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 72, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 108, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 144, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 180, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 216, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 252, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 288, 4, 60000 )
		respawn_roaming_mob( 801105, 20190085, 324, 4, 60000 )
		
		--블러드 슬라임
		respawn_roaming_mob( 801106, 20190092, 0, 0, 60000 )
		respawn_roaming_mob( 801106, 20190085, 0, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 36, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 72, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 108, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 144, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 180, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 216, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 252, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 288, 4, 60000 )
		respawn_roaming_mob( 801106, 20190085, 324, 4, 60000 )
		
		--본 드래곤
		respawn_roaming_mob( 801107, 20190093, 0, 0, 60000 )
		respawn_roaming_mob( 801107, 20190085, 0, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 36, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 72, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 108, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 144, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 180, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 216, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 252, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 288, 4, 60000 )
		respawn_roaming_mob( 801107, 20190085, 324, 4, 60000 )
		
		--메피스토
		respawn_roaming_mob( 801108, 22000050, 0, 0, 60000 )
		respawn_roaming_mob( 801108, 22000043, 0, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 36, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 72, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 108, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 144, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 180, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 216, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 252, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 288, 4, 60000 )
		respawn_roaming_mob( 801108, 22000043, 324, 4, 60000 )
		
		--릴리스
		respawn_roaming_mob( 801109, 22000051, 0, 0, 60000 )
		respawn_roaming_mob( 801109, 22000043, 0, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 36, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 72, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 108, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 144, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 180, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 216, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 252, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 288, 4, 60000 )
		respawn_roaming_mob( 801109, 22000043, 324, 4, 60000 )
		
		--광기의 사념체
		respawn_roaming_mob( 801110, 22000052, 0, 0, 60000 )
		respawn_roaming_mob( 801110, 22000043, 0, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 36, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 72, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 108, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 144, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 180, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 216, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 252, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 288, 4, 60000 )
		respawn_roaming_mob( 801110, 22000043, 324, 4, 60000 )
		
		--피에 굶주린 살육자
		respawn_roaming_mob( 801111, 22000158, 0, 0, 60000 )
		respawn_roaming_mob( 801111, 22000155, 0, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 36, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 72, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 108, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 144, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 180, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 216, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 252, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 288, 4, 60000 )
		respawn_roaming_mob( 801111, 22000155, 324, 4, 60000 )
		
		--크리스탈 골렘
		respawn_roaming_mob( 801112, 22000159, 0, 0, 60000 )
		respawn_roaming_mob( 801112, 22000155, 0, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 36, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 72, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 108, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 144, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 180, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 216, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 252, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 288, 4, 60000 )
		respawn_roaming_mob( 801112, 22000155, 324, 4, 60000 )
		
		--기사단장 헥토르
		respawn_roaming_mob( 801113, 22000160, 0, 0, 60000 )
		respawn_roaming_mob( 801113, 22000155, 0, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 36, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 72, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 108, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 144, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 180, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 216, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 252, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 288, 4, 60000 )
		respawn_roaming_mob( 801113, 22000155, 324, 4, 60000 )
		
		--피에 굶주린 살육자
		respawn_roaming_mob( 801114, 22000158, 0, 0, 60000 )
		respawn_roaming_mob( 801114, 22000155, 0, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 36, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 72, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 108, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 144, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 180, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 216, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 252, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 288, 4, 60000 )
		respawn_roaming_mob( 801114, 22000155, 324, 4, 60000 )
	end
end	