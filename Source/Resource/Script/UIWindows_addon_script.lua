-- Lua 스크립트 암호화
function get_module_name()
             return "UIWindows_addon_script"
end

--======================================================================
-- 게임 내 사용된 UI Windows들의 AddOn Window들
--======================================================================

-- "window_lobby_login.nui"			//UIWINDOW_LOGIN				1
-- "window_lobby_login_logo.nui"		//UIWINDOW_LOGIN_LOGO				2
-- ""						//UIWINDOW_NOTIFY				3
-- "window_main_minimap.nui",			//UIWINDOW_MINIMAP				4
-- "window_lobby_character_select02.nui"	//UIWINDOW_CHARSELECT				5
-- "window_lobby_character_create.nui" 		//UIWINDOW_CREATECHAR				6
-- "window_lobby_server_list.nui"		//UIWINDOW_SERVERLIST				7
-- "window_lobby_notice.nui"			//UIWINDOW_SERVERNOTICE				8
-- "window_lobby_clanselect.nui"		//UIWINDOW_CLANSELECT				9
-- "window_main_player.nui"			//UIWINDOW_MAINFRAME				10
-- "window_main_chat.nui"			//UIWINDOW_CHATTING				11
-- "window_inventory.nui"			//UIWINDOW_INVENTORY				12
-- "window_main_target.nui"			//UIWINDOW_TARGET				13
-- "window_main_target_sub.nui"			//UIWINDOW_TARGETPORTRAIT			14
-- "window_main_simple_summon.nui"		//UIWINDOW_CREATURE				15
-- ""						//UIWINDOW_CONSOLE				16
-- "window_lobby_character_select.nui"		//UIWINDOW_CHARNAMELIST				17
-- "window_lobby_character_select03.nui"	//UIWINDOW_SELECTCHAR_INFO			18
-- ""						//UIWINDOW_DEVOUTPUT				19
-- "window_main_menu.nui"			//UIWINDOW_MAINMENU				20
-- "window_main_menu_sub.nui"			//UIWINDOW_MAINMENU_SUB				21
-- "window_npc_talk.nui"			//UIWINDOW_NPCDIALOG				22
-- "window_main_quickslot.nui"			//UIWINDOW_HQUICKSLOT				23
-- ""						//UIWINDOW_VQUICKSLOT				24
-- ""						//UIWINDOW_HQUICKSLOTMENU			25
-- "window_system_number_input.nui"		//UIWINDOW_INPUTNUMBER				26
-- "window_player_skill.nui"			//UIWINDOW_SKILL				27
-- "window_player_skill_sub.nui"		//UIWINDOW_SKILLFUND				28
-- ""						//UIWINDOW_SYSMENU				29
-- "window_enchant.nui"				//UIWINDOW_ITEMCOMBINE				30
-- "window_contribution.nui"			//UIWINDOW_ITEMCONTRIBUTION			31
-- "window_business_shop.nui"			//UIWINDOW_SHOP					32
-- "window_business_shop_sub.nui"		//UIWINDOW_SHOPKART				33
-- ""						//UIWINDOW_STORAGEKART				34
-- "window_main_state_h_effect.nui"		//UIWINDOW_STATE_H_EFFECT			35
-- "window_main_state_h_effect.nui"		//UIWINDOW_STATE_V_EFFECT			36
-- "window_operation.nui"			//UIWINDOW_MOTION				37
-- "window_system_def.nui"			//UIWINDOW_SYSTEM				38
-- "window_system_setup.nui"			//UIWINDOW_SYSTEM_MAIN				39
-- ""						//UIWINDOW_MOTION_NORMAL			40
-- ""						//UIWINDOW_MOTION_PARTY				41
-- ""						//UIWINDOW_MOTION_GESTURE			42
-- ""						//UIWINDOW_SYSTEM_NORMAL			43
-- "window_main_summon.nui"			//UIWINDOW_CREATURE_MAIN			44
-- "window_summon_formation.nui"		//UIWINDOW_CREATUREFORM				45
-- "window_business_trade.nui"			//UIWINDOW_TRADE				46
-- "window_info_player.nui"			//UIWINDOW_CHARINFO				47
-- "window_info_player_sub.nui"			//UIWINDOW_CHARINFO_DETAIL			48
-- "window_summon_info.nui"			//UIWINDOW_CREATURE_PAGE			49
-- "window_summon_info_sub.nui"			//UIWINDOW_CREATURE_DETAIL			50
-- "window_summon_skill.nui"			//UIWINDOW_CREATURE_SKILL			51
-- "window_summon_skill_sub.nui"		//UIWINDOW_CREATURE_SKILLFUND			52
-- "window_main_party.nui"			//UIWINDOW_PARTY				53
-- "window_main_party_state.nui"		//UIWINDOW_PARTY_STATE				54
-- "window_business_warehouse.nui"		//UIWINDOW_STORAGE				55
-- "window_system_auction.nui"			//UIWINDOW_AUCTION				56
-- ""						//UIWINDOW_AUCTION_SEARCH			57
-- ""						//UIWINDOW_AUCTION_REGISTER			58
-- ""						//UIWINDOW_AUCTION_TENDER			59
-- ""						//UIWINDOW_AUCTION_DEPOSIT			60
-- "window_system_text_input_box.nui"		//UIWINDOW_INPUTTEXT				61
-- "window_main_party_sub.nui"			//UIWINDOW_PARTY_MENU				62
-- "window_main_private_party_sub.nui"		//UIWINDOW_PRIVATE_PARTY_MENU			63
-- "window_main_quickslot_summon.nui"		//UIWINDOW_CREATURE_HQUICK			64
-- "window_world_map.nui"			//UIWINDOW_WORLDMAP				65
-- "window_main_chat_sub.nui"			//UIWINDOW_CHAT_OPTION				66
-- "window_target_state_h_effect.nui"		//UIWINDOW_TARGET_STATE				67
-- "window_business_booth_create.nui"		//UIWINDOW_STORE_HOST				68
-- "window_business_booth_exchange.nui"		//UIWINDOW_STORE_CLIENT				69
-- "window_quest.nui"				//UIWINDOW_QUEST_LIST				70
-- "window_quest_sub.nui"			//UIWINDOW_QUEST_INFO				71
-- "window_quest_reward.nui"			//UIWINDOW_QUEST_REWARD				72
-- "window_quest_main.nui"			//UIWINDOW_QUEST_DIALOG				73
-- "window_area_title.nui"			//UIWINDOW_LOCAL_SIGN				74
-- "window_party_create.nui"			//UIWINDOW_PARTY_CREATE				75
-- ""						//UIWINDOW_PARTY_INVITE				76
-- "window_Socket.nui"				//UIWINDOW_JEWEL_EQUIP				77
-- "window_SoulCharge.nui"			//UIWINDOW_SOUL_CHARGE				78
-- "window_dungeon_stone.nui"			//UIUIWINDOW_DUNGEON_STONE			79
-- "window_dungeon_unit.nui"			//UIUIWINDOW_DUNGEON_UNIT			80
-- "window_system_license.nui"			//UIWINDOW_LICENSE				81
-- "window_messenger.nui"			//UIWINDOW_COMMUNITY				82
-- ""						//UIWINDOW_GUILD				83
-- "window_messenger_friend.nui"		//UIWINDOW_FRIEND				84
-- "window_messenger_cut.nui"			//UIWINDOW_CUT					85
-- "window_lobby_character_create01.nui"	//UIWINDOW_CREATECHAR_SECOND			86
-- "window_lobby_rotate00.nui"			//UIWINDOW_CREATECHAR_ROTATE00			87
-- "window_lobby_rotate01.nui"			//UIWINDOW_CREATECHAR_ROTATE01			88
-- ""						//UIWINDOW_NOTICE				89
-- ""						//UIWINDOW_CHINAWARRING				90
-- ""						//UIWINDOW_WEBGAMESUTDOWN			91
-- "window_fulldown_player.nui"			//UIWINDOW_FULLDOWN_PLAYER			92
-- "window_fulldown_target_player.nui"		//UIWINDOW_FULLDOWN_TARGETPLAYER		93
-- "window_fulldown_target_monster.nui"		//UIWINDOW_FULLDOWN_TARGETMONSTER		94
-- "window_fulldown_minimap.nui"		//UIWINDOW_FULLDOWN_MINIMAP			95
-- "window_fulldown_itemdrop.nui"		//UIWINDOW_FULLDOWN_ITEMDROP			96
-- "window_fulldown_creature.nui"		//UIWINDOW_FULLDOWN_CREATURE			97
-- "window_business_cash.nui"			//UIWINDOW_CASH_STORAGE				98
-- "window_lobby_character_create_2.nui"	//UIWINDOW_CREATECHAR_BOTTOM			99
-- "window_lobby_character_create01_2.nui"	//UIWINDOW_CREATECHAR_SECOND_BOTTOM		100
-- "window_cash01.nui"				//UIWINDOW_CASH_TITLE				101
-- "window_guild_bar.nui"			//UIWINDOW_GUILD_TITLE				102
-- "window_system_resurrect.nui"		//UIWINDOW_RESURRECT				103
-- "window_system_recall.nui"			//UIWINDOW_RECALL				104
-- "window_help_bar.nui"			//UIWINDOW_HELP_TITLE				105
-- "window_popurl_bar.nui"			//UIWINDOW_POPURL_TITLE				106
-- "window_lobby_gamerating.nui"		//UIWINDOW_GAME_RATING				107
-- "window_lobby_gamerating_18only.nui"		//UIWINDOW_GAME_RATING_18ONLY			108
-- ""						//UIWINDOW_SCREENSHOT				109
-- "window_lobby_race_select.nui"		//UIWINDOW_RACE_SELECT				110
-- "window_lobby_racetext_select.nui"		//UIWINDOW_RACE_TEXT				111
-- "window_lobby_race_desc.nui"			//UIWINDOW_RACE_DESC				112
-- ""						//UIWINDOW_EFFECT_FOCUS				113
-- ""						//UIWINDOW_LOADING				114
-- "window_creaturecard.nui"			//UIWINDOW_CREATURE_CARD			115
-- "window_creaturecard_sub.nui"		//UIWINDOW_CREATURE_CARD_SUB			116
-- "window_SkinColor.nui"			//UIWINDOW_SKIN_COLOR				117
-- "window_system_contibution_msgbox.nui"	//UIWINDOW_CONTRIBUTION_MSGBOX			118
-- "window_grade_15.nui"			//UIWINDOW_KOREAWARRING_GRADE_15		119
-- "window_grade_18.nui"			//UIWINDOW_KOREAWARRING_GRADE_18		120
-- "window_grade_language.nui"			//UIWINDOW_KOREAWARRING_VIO_LAN			121
-- "window_lock_input.nui"			//UIWINDOW_SECURITYSETTING			122
-- "window_lock_change.nui"			//UIWINDOW_SECURITYSETTINGMODIFY		123
-- "window_lock_question1.nui"			//UIWINDOW_SECURITYCHARACTER			124
-- "window_lock_question2.nui"			//UIWINDOW_SECURITYSTORAGE			125
-- ""						//UIWINDOW_TEST					126

-- create_addon( parent_window_name, addon_window_name, position_by_parent_x, position_by_parent_y, open_by_click, control_name... )
-- position_by_parent_x = "mouse", "center_h", "center_v", "left_in", "left_out", "right_in", "right_out", "top_in", "top_out", "bottom_in", "bottom_out", user_input( number )
-- position_by_parent_y = "mouse", "center_h", "center_v", "left_in", "left_out", "right_in", "right_out", "top_in", "top_out", "bottom_in", "bottom_out", user_input( number )
-- open_by_click = lbutton_up, lbutton_down, rbutton_up, rbutton_down, wheel_up, wheel_down, lbutton_dblclk, rbutton_dblclk
-- control_name = addon 윈도우를 열때 연결할 NUI 내 컨트롤 들이다 복사 컨트롤일 경우 사이즈 지정후 컨트롤 네임 설정 56, "icon_item" ( icon_item00 ~ icon_item56 )

-- AddOn 사용을 위한 초기화
function initialize_addon_list()

	init_addon_list()

end

function get_addon_list()

	create_addon( "window_main_target_sub.nui", "window_target_wnd_addon.nui", "center_h", "bottom_out", rbutton_down )
	create_addon( "window_lobby_login.nui", "window_target_wnd_addon.nui", "mouse", "mouse", rbutton_down, "accountedit", "passwordedit" )
	create_addon( "window_inventory.nui", "window_addon_test2.nui", "right_out", "bottom_out", rbutton_down, 10, 56, "icon_item" )
--	create_addon( "window_lobby_rotate00.nui", "window_addon_test.nui", "right_out", "top_in", rbutton_down )
--	create_addon( "window_lobby_character_create.nui", "window_addon_test.nui", 20, 10, right_click )
--	create_addon( "window_lobby_server_list.nui", "window_addon_test.nui", 5, "left_in", right_click )

end