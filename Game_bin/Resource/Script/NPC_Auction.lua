-- Lua 스크립트 암호화
function get_module_name()
             return "NPC_Auction"
end

   --============================================================
   --             <<<<<< 데바 경매장 측 NPC >>>>>>
   --============================================================

-- 데바 경매장 npc
function NPC_Auction_Deva_contact()
 
		if get_global_variable("disableAuction") == 1 then
			return
		else
			-- 경매장 호출 함수
			show_auction_window()
		end
end
    
   --============================================================
   --             <<<<<< 가이아 gaia 경매장 측 NPC >>>>>>
   --============================================================

-- 가이아 경매장 npc
function NPC_Auction_Gaia_contact()
 
		if get_global_variable("disableAuction") == 1 then
			return
		else
         -- 경매장 호출 함수
         show_auction_window()
		end
end

   --============================================================
   --             <<<<<< 아수라 asura 경매장 측 NPC >>>>>>
   --============================================================

-- 아수라 경매장 npc
function NPC_Auction_Asura_contact()
 
		if get_global_variable("disableAuction") == 1 then
			return
		else
         -- 경매장 호출 함수
         show_auction_window()
		end
end

   --============================================================
   --             <<<<<< 론도 rondoh 경매장 측 NPC >>>>>>
   --============================================================

-- 론도 경매장 npc
function NPC_Auction_Rondoh_contact()
 
		if get_global_variable("disableAuction") == 1 then
			return
		else
         -- 경매장 호출 함수
         show_auction_window()
		end
end



   --============================================================
   --             <<<<<< 시크루트 secroute 측 NPC >>>>>>
   --============================================================
-- 시크루트 경매장 npc
function NPC_Auction_Secroute_contact()
 
		if get_global_variable("disableAuction") == 1 then
			return
		else
         -- 경매장 호출 함수
         show_auction_window()
		end
end


   --============================================================
   --             <<<<<< 마리캣 마켓 maricat market NPC >>>>>>
   --============================================================
-- 마리캣 마켓 npc
function NPC_Auction_maricat_market_contact()
 
		if get_global_variable("disableAuction") == 1 then
			return
		else
         -- 경매장 호출 함수
         show_auction_window()
		end
end
