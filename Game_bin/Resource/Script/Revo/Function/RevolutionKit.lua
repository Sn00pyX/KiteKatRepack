----Private Kit Made by Revolutionteam .

--241401 درع التنين ذو الرأسين
--3000339 ألدرع الاضافي
--223401 عباءة الانوار
--302402 خوذة الحرب
--303401 قفازات المناور
--304403 حذاء الوقار
--105201 الفأس ذو النصلين
--105303 فاس القمة
--111401 العصا الدوارة
--108401 القوس العملاق
--109401 قوس العلامة
--490011 حقيبة جلد كبيرة
--105401 Great axe
--105102 Half-Bladed Axe
--	function first_login_gift()  --First login kit function -weapon+armor
	
--local id = {241401, 3000339, 223401, 302402, 303401, 304403, 105201, 111401, 108401, 109401, 105401, 105102}
--local level = {10}
--local enhance = {25}

--for i = 1,#id do
--insert_item(id[i],1,enhance[1],level[1])
--end

--end
function first_login_states()
local state_id = {9971} --Buffs ids
local state_power = {4} --Buffs Power
local state_time = 360000
local in_flag = GAF("FLS")
if in_flag == 0 then

for i = 1 , #state_id do 
add_state(state_id[i],state_power[i],state_time)
end 
set_account_flag("FLS",1)
end 
end 

function GAF(value)
local current_flag = get_account_flag(value)

if current_flag == nil or current_flag == "" then
return 0
end 
return current_flag
end

--[[
function on_change_weapon()  --Packet bug temp fix
local name = gv("name")
	local Time = get_os_time()
	local flag_cnt  = get_flag("Change_item_cnt")	if flag_cnt  == nil or flag_cnt == "" then flag_cnt = 0 end
	local flag_time = get_flag("Change_item_time")	if flag_time == nil or flag_time == "" then flag_time = Time end
	if ( flag_time + 2) == Time then 
		set_flag("Change_item_cnt",flag_cnt + 1)
		if flag_cnt >= 2 then --in_punish
			add_state(6012, 10, 5000, name)
			add_state(13754, 10, 5000, name)
		end
	else 
		set_flag("Change_item_cnt", 0)
	end
	set_flag("Change_item_time", Time)
end
]]--



---------------

function Server_Gift()
local Account = tostring(gv("account"))
local Has_Gift = GGV("Server_Gift_"..Account)
local item = {603001, -- لفافة العودة
			 2010454} -- زجاجة أم الجنيات<09's KOR event>
local item1 = {2012823, -- مضاعف الخبرة المطور/ذاتي
			  950073,-- مثبت النشاط الفاخر
			  490011}	 --حقيبة جلد كبيرة
   if( Has_Gift == "" ) or ( Has_Gift == nil ) then Has_Gift = 0 end
     if( Has_Gift ~= 1 ) then
		 --// Array Item
	     for i,v in pairs(item) do
		     SetItem(item[i],10)
			 SetItem(item1[i],1)
		 end
		 --// crtal
		 for i = 0,10 do
			 SetItem(910064+i,10)
		 end 
		insert_item( 2012832, 5 ) -- طماطم
		insert_item( 910090, 3 ) -- مخفية
		 --//  Gold
		 sv("gold",gv("gold")+ 100000000)
		 --// make sure don't have gift more than one for same account
		 SGV("Server_Gift_"..Account,1)
		 message("<#ff00ff> Welcome to Revolution International server ")
	 end
end
--// Some Functions
function SGV( name ,value )
	if (GGV(name) == "") then
		set_global_variable(name,value)
	end
end

function DGV( name )
	if(GGV(name) ~= "") then
		del_global_variable(name) 
	end
end

function GGV( name )
return ( get_global_variable(name) )
end

function SetItem( id , count )
insert_item( id , count )
end