#pragma once

#include "LuaVM.h"

int SCRIPT_DropItem( struct lua_State *L );
int SCRIPT_DropGold( struct lua_State *L );
int SCRIPT_InsertItem( struct lua_State *L );
int SCRIPT_DeleteItem( struct lua_State *L );
int	SCRIPT_PutoffItem( struct lua_State *L );
int SCRIPT_IsErasableItem( struct lua_State *L );
int SCRIPT_InsertGold( struct lua_State *L );
int SCRIPT_GetItemName( struct lua_State *L );	// handle 에서 아이템 이름을 얻는다
int SCRIPT_GetItemLevel( struct lua_State *L );	// handle 에서 아이템 레벨을 얻는다
int SCRIPT_GetItemEnhance( struct lua_State *L );
int SCRIPT_GetItemGrade( struct lua_State *L );
int SCRIPT_GetItemRank( struct lua_State *L );

int SCRIPT_RefreshEventDropInfo( struct lua_State *L );
int SCRIPT_StopEventDrop( struct lua_State *L );
int SCRIPT_StartEventDrop( struct lua_State *L );

int SCRIPT_StartEventDungeonDroprate( struct lua_State *L );
int SCRIPT_StopEventDungeonDroprate( struct lua_State *L );
int SCRIPT_RefreshEventDungeonDroprate( struct lua_State *L );

int SCRIPT_RefreshEventSupplyInfo( struct lua_State *L );
int SCRIPT_StartEventSupply( struct lua_State *L );
int SCRIPT_StopEventSupply( struct lua_State *L );
int SCRIPT_EventSupply( struct lua_State *L );
int SCRIPT_SupplyEventItem( struct lua_State *L );

int SCRIPT_FindItem( struct lua_State *L );

int SCRIPT_GetItemHandle( struct lua_State *L );
int SCRIPT_GetItemHandleList( struct lua_State *L );
int SCRIPT_GetItemPrice( struct lua_State *L );
int SCRIPT_SetItemLevel( struct lua_State *L );	// handle 에서 아이템 레벨을 세팅한다
int SCRIPT_SetItemEnhance( struct lua_State *L );
int SCRIPT_SetItemEnhanceFail( struct lua_State *L );
int SCRIPT_GetItemCode( struct lua_State *L );	// handle 에서 아이템 코드를 얻는다
int SCRIPT_ItemName( struct lua_State *L );		// 아이템 코드에서 아이템 이름을 얻는다
int SCRIPT_GetWearItemHandle( struct lua_State *L );	// 현재 장착한 아이템 handle 을 얻는다
int SCRIPT_GetItemNameId( struct lua_State *L );		// 아이템 코드에서 아이템 이름의 ID를 얻는다

int SCRIPT_GetItemEtherealDurability( struct lua_State *L );		// handle 에 해당하는 아이템의 에테리얼 내구도를 얻는다
int SCRIPT_SetItemEtherealDurability( struct lua_State *L );		// handle 에 해당하는 아이템의 에테리얼 내구도를 설정한다
int SCRIPT_GetMaxItemEtherealDurability( struct lua_State *L );		// handle 에 해당하는 아이템의 최대 에테리얼 내구도를 얻는다

int SCRIPT_GetItemSoulStoneEndurance( struct lua_State *L );		// handle 에 해당하는 아이템의 내구도(소울스톤 영혼력)를 얻는다
int SCRIPT_GetMaxItemSoulStoneEndurance( struct lua_State *L );		// handle 에 해당하는 아이템의 최대 내구도(소울스톤 영혼력)를 얻는다

int SCRIPT_GetItemEndurance( struct lua_State *L );	// handle 에서 아이템 내구도를 얻는다
int SCRIPT_SetItemEndurance( struct lua_State *L );	// handle 에서 아이템 내구도를 설정한다

int SCRIPT_GetItemSocketCode( struct lua_State *L ); // handle 에서 아이템 소켓 코드를 얻는다
int SCRIPT_SetItemSocketCode( struct lua_State *L ); // handle 에서 아이템 소켓 코드를 설정하고 해당 코드에 맞는 최대 내구도를 채워준다

int SCRIPT_SetItemElement( struct lua_State *L );			// handle 의 속성 스톤을 설정한다.
int SCRIPT_SetItemElementParameter( struct lua_State *L );	// handle 의 속성 스톤 보너스값 (공격력, 마력 보너스)를 설정한다.

int SCRIPT_GetItemAppearanceCode( struct lua_State *L );	// handle 의 변형된 형상 코드를 얻는다.
int SCRIPT_SetItemAppearanceCode( struct lua_State *L );	// handle 의 변형된 형상 코드를 설정한다.

int SCRIPT_InsertAwakenOption( struct lua_State *L );		// 아이템을 각성한다.
int SCRIPT_DeleteAwakenOption( struct lua_State *L );		// 아이템에 부여된 각성을 해제한다.

int SCRIPT_IdentifyItemForRandomOption( struct lua_State *L );		// 미확인 아이템을 확인하여 랜덤옵션을 부여한다
int SCRIPT_ClearInventory( struct lua_State *L );				//인벤토리 아이템 삭제
int SCRIPT_InsertSummonBySummonID( struct lua_State *L );			// 소환수 ID로 봉카 생성
int SCRIPT_InsertSummonByMonsterID( struct lua_State *L );			// 몬스터 ID로 봉카 생성
int SCRIPT_ChangeItemCode( struct lua_State *L );			// handle의 아이템 코드를 바꾼다.
int SCRIPT_GetItemIdentifiedOption( struct lua_State *L );	// handle 아이템의 랜덤 옵션을 얻어온다.
int SCRIPT_SetItemIdentifiedOption( struct lua_State *L );	// handle 아이템의 랜덤 옵션을 세팅한다. 
int	SCRIPT_PickItemInDropGroup( struct lua_State *L );		// 해당 드랍 그룹 내에서 랜덤으로 한 아이템과 개수를 뽑는다.