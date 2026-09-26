
#include <vector>

#include <toolkit/XEnv.h>
#include <mmo/ArcadiaServer.h>
#include <logging/FileLog.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "MixManager.h"
#include "StructItem.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructSkill.h"
#include "DB_Commands.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "GameContent.h"
#include "RandomManager.h"

static std::vector< MixBase >			s_vMixInfo;
static std::vector< EnhanceInfo >		s_vEnhanceInfo;
static std::vector< DecomposeBase >		s_vDecomposeInfo;

void MixManager::procEnhanceFail( StructPlayer *pPlayer, StructItem *pItem, int nFailResult )
{
	if( nFailResult == EnhanceInfo::RESULT_SKILL_CARD_FAIL )
	{
		if( pItem->GetItemEnhance() > 3 )
		{
			// 4강 이상인데 실패한 경우 -3강짜리 카드 만들어서 하나 넣어준다.
			StructItem * pNewItem = StructItem::AllocItem( 0, pItem->GetItemCode(), 1 );
			pNewItem->CopyFrom( pItem );
			pNewItem->SetIdx( 0 );
			pNewItem->SetCount( 1 );
			pNewItem->SetItemEnhance( pNewItem->GetItemEnhance() - 3 );
			pPlayer->PushItem( pNewItem, 1 );
		}
		
		pPlayer->EraseItem( pItem, 1 );
	}
	else if( nFailResult == EnhanceInfo::RESULT_FAIL )
	{
		pItem->SetInstanceFlagOn( ItemInstance::ITEM_FLAG_FAILED );

		// 벨트의 소켓은 다른 용도로 쓰이므로 복구가 가능하도록 원래 값을 유지한다.
		if( pItem->GetItemClass() != ItemBase::CLASS_BELT )
		{
			for( int i = 0 ; i < ItemBase::MAX_SOCKET_NUMBER ; ++i )
			{
				pItem->SetSocketCode( i, 0 );
			}
		}

		pItem->DBQuery( new DB_UpdateItem( pItem ) );
		SendItemMessage( pPlayer, pItem );
	}
	else if( nFailResult == EnhanceInfo::RESULT_ACCESSORY_FAIL )
	{
		if( pItem->GetItemEnhance() <= 3 )
		{
			pItem->SetItemEnhance( 0 );
			// DB 업데이트, SendItemMessage는 이하에서 담당
			pItem = pPlayer->onAfterChangeItemProperty( pItem );
		}
		else
		{
			pItem->SetItemEnhance( pItem->GetItemEnhance() - 3 );
			// DB 업데이트, SendItemMessage는 이하에서 담당
			pItem = pPlayer->onAfterChangeItemProperty( pItem );
		}
	}
	else if( nFailResult == EnhanceInfo::RESULT_DO_NOTHING )
	{
		// 따로 실패에 대한 패널티 처리 안 함. (혹시 이걸 호출한 함수에서 pItem에 무슨 짓을 했을수도 있으니 저장만 해주자.)
		pPlayer->onAfterChangeItemProperty( pItem );
	}
}

void MixManager::RegisterMixInfo( const MixBase & info )
{
	for( std::vector< MixBase >::iterator it = s_vMixInfo.begin(); it != s_vMixInfo.end(); ++it )
	{
		if( (*it).id == info.id )
		{
			(*it) = info;

			return;
		}
	}

	s_vMixInfo.push_back( info );
}

void MixManager::RegisterDecomposeInfo( const DecomposeBase & info )
{
	for( std::vector< DecomposeBase >::iterator it = s_vDecomposeInfo.begin(); it != s_vDecomposeInfo.end(); ++it )
	{
		if( (*it).id == info.id )
		{
			(*it) = info;

			return;
		}
	}

	s_vDecomposeInfo.push_back( info );
}

// 조합 재료 체크 37번, 기획자가 정한 특수 유형들은 여기서 체크 (데이터로 제어하지 않고, 기획자의 요청에 따라 하드코딩 됨)
bool check_designer_defined_type( const StructItem *pItem, int nType )
{
	int nGroup = pItem->GetItemGroup();

	switch( nType )
	{
	case DESIGNER_TYPE_WEAPON:
	{
		if( nGroup == ItemBase::GROUP_WEAPON )
			return true;
		break;
	}

	case DESIGNER_TYPE_ARMOR:
	{
		if( nGroup == ItemBase::GROUP_ARMOR || nGroup == ItemBase::GROUP_SHIELD ||
			nGroup == ItemBase::GROUP_HELM  || nGroup == ItemBase::GROUP_GLOVE || 
			nGroup == ItemBase::GROUP_BOOTS || nGroup == ItemBase::GROUP_BELT ||
			nGroup == ItemBase::GROUP_MANTLE )
			return true;
		break;
	}

	case DESIGNER_TYPE_WEAR_PRIMARY:
	{
		if (nGroup < ItemBase::GROUP_SKILLCARD)
			return true;
		break;
	}

	case DESIGNER_TYPE_WEAR_ALL:
	{
		if ((nGroup >= ItemBase::GROUP_ETC && ItemBase::GROUP_SKILLCARD) ||
			nGroup == ItemBase::GROUP_FACE || nGroup == ItemBase::GROUP_UNDERWEAR ||
			nGroup == ItemBase::GROUP_BAG || nGroup == ItemBase::GROUP_PET_CAGE ||
			nGroup == ItemBase::GROUP_BACKPACK || nGroup == ItemBase::GROUP_EMBLEM ||
			nGroup == ItemBase::GROUP_DECO || nGroup == ItemBase::GROUP_RIDING ||
			nGroup == ItemBase::GROUP_EQUIPMENT_ON_BELT)
			return true;
		break;
	}

	case 4:
	{
		if (nGroup == ItemBase::GROUP_ARMOR || nGroup == ItemBase::GROUP_SHIELD ||
			nGroup == ItemBase::GROUP_HELM || nGroup == ItemBase::GROUP_GLOVE ||
			nGroup == ItemBase::GROUP_BOOTS || nGroup == ItemBase::GROUP_BELT ||
			nGroup == ItemBase::GROUP_MANTLE)
			return false;
		break;
	}

	default: return false;
	}

	return false;
}

bool check_material_info( const MaterialInfo & info, const StructItem * pItem, unsigned short *pItemCount )
{
	extern volatile int g_nCurrentLocalFlag;
	bool bIsCountChecked = false;

	// 최초 체크 조건이 0(없음)이면 아무 조건도 없는 것이 아니라 아이템이 들어오믄 안됨
	if( !info.type[0] )
	{
		return !pItem;
	}

	// 아이템이 없다면 조건 체크에 들어오면 안 됨
	if( !pItem && info.type[0] != 50 )
	{
		return false;
	}

	for( int i = 0; i < MaterialInfo::MATERIAL_INFO_COUNT ; ++i )
	{
		switch( info.type[i] )
		{
		case MIX_CHECK_ITEM_GROUP:
			if( pItem->GetItemGroup() != info.value[i] )
				return false;
			break;

		case MIX_CHECK_ITEM_CLASS:
			if( pItem->GetItemClass() != info.value[i] )
				return false;
			break;

		case MIX_CHECK_ITEM_ID:
			if( pItem->GetItemCode() != info.value[i] )
				return false;
			break;

		case MIX_CHECK_ITEM_RANK:
			if( pItem->GetItemRank() != info.value[i] )
				return false;
			break;

		case MIX_CHECK_ITEM_LEVEL:
			if( pItem->GetItemLevel() != info.value[i] )
				return false;
			break;

		case MIX_CHECK_INSTANCE_FLAG_ON:
			if( !pItem->GetInstanceFlag().IsOn( info.value[i] ) )
				return false;
			break;

		case MIX_CHECK_INSTANCE_FLAG_OFF:
			if( pItem->GetInstanceFlag().IsOn( info.value[i] ) )
				return false;
			break;
		case MIX_CHECK_ENHANCE_MATCH:
			if( pItem->GetItemEnhance() != info.value[i] )
				return false;
			break;
		case MIX_CHECK_ENHANCE_DISMATCH:
			if( pItem->GetItemEnhance() == info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_COUNT:
			bIsCountChecked = true;
			if( (*pItemCount) != info.value[i] )
				return false;
			break;
		case MIX_CHECK_ELEMENTAL_EFFECT_MATCH:
			{
				// 비트셋으로 value를 지정하므로 무속성: 0, 그 외의 속성: 2^(속성값 - 1)
				// 이를 위해서 2^(속성값) / 2로 처리함(무속성은 2^0 = 1 이므로 int로 캐스팅 후에 2로 나누어(= 1비트 Right-Shift) 0이 됨)
				int nItemElementalEffectBitflag = static_cast< int >( pow( 2.0, pItem->GetElementalEffectType() ) ) >> 1;

				// 아이템의 속성이 무속성이라면
				if( !nItemElementalEffectBitflag )
				{
					// 체크 조건 값도 무속성인 경우에만 통과
					if( info.value[i] )
						return false;
				}
				// 무속성이 아니라면 체크 조건 값과 비트 연산 체크
				else if( ( nItemElementalEffectBitflag & info.value[i] ) != nItemElementalEffectBitflag )
					return false;
			}
			break;
		case MIX_CHECK_ELEMENTAL_EFFECT_MISMATCH:
			{
				// 비트셋으로 value를 지정하므로 무속성: 0, 그 외의 속성: 2^(속성값 - 1)
				// 이를 위해서 2^(속성값) / 2로 처리함(무속성은 2^0 = 1 이므로 int로 캐스팅 후에 2로 나누어(= 1비트 Right-Shift) 0이 됨)
				int nItemElementalEffectBitflag = static_cast< int >( pow( 2.0, pItem->GetElementalEffectType() ) ) >> 1;

				// 아이템의 속성이 무속성이라면
				if( !nItemElementalEffectBitflag )
				{
					// 체크 조건 값이 무속성이 아닌 경우에만 통과
					if( !info.value[i] )
						return false;
				}
				// 무속성이 아니라면 체크 조건 값과 비트 연산 체크
				else if( ( nItemElementalEffectBitflag & info.value[i] ) == nItemElementalEffectBitflag )
					return false;
			}
			break;
		case MIX_CHECK_ITEM_WEAR_POSITION_MATCH:
			if( pItem->GetWearType() != info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_WEAR_POSITION_MISMATCH:
			if( pItem->GetWearType() == info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_COUNT_GE:
			bIsCountChecked = true;
			if( (*pItemCount) < info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_ETHEREAL_DURABILITY_E:
			if( pItem->GetCurrentEtherealDurability() != info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_ETHEREAL_DURABILITY_NE:
			if( pItem->GetCurrentEtherealDurability() == info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_GRADE:
			if( pItem->GetItemGrade() != info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_EXPIRED_TIME_GE:
			if( info.value[i] == -1 && pItem->IsExpireItem() ) return false;
			if( info.value[i] != -1 && pItem->IsExpireItem() && pItem->GetExpireTime() < info.value[i] ) return false;
			break;
		case MIX_CHECK_ITEM_EXPIRED_TIME_LE:
			if( info.value[i] == -1 && pItem->IsExpireItem() ) return false;
			if( info.value[i] != -1 && ( !pItem->IsExpireItem() || pItem->GetExpireTime() > info.value[i] ) ) return false;
			break;

		case MIX_CHECK_ITEM_FIRST_SOCKET_CODE_MATCH:
			if( pItem->GetSocketCode( 0 ) != info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_MAX_ETHEREAL_DURABILITY_E:
			if( pItem->GetMaxEtherealDurability() != info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_MAX_ETHEREAL_DURABILITY_NE:
			if( pItem->GetMaxEtherealDurability() == info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_TYPE:
			if( pItem->GetItemType() != info.value[i] )
				return false;
			break;
		case MIX_CHECK_ITEM_GROUP_NE:
			if( pItem->GetItemGroup() == info.value[i] )
				return false;
			break;
		case MIX_CHECK_AWAKEN_ITEM:
			if( pItem->IsAwaken() == !info.value[i] )
				return false;
			break;
		case MIX_CHECK_SAME_SUMMON_RATE:
			{
#ifdef _DEBUG
				// 소환수 카드가 아닌 아이템을 확인하는데 사용되어서는 안 되는 식별형태
				// 만약 소환수 코드가 없거나 혹은 소환수 정보가 없는 경우는 아래의 GetSummonInfo를 얻어올 때 assert가 걸린다.
				if( !pItem->IsSummonCard() )
				{
					assert( 0 );
					return false;
				}
#endif
				if( !pItem->GetSummonCode() )
					return false;

				if( GameContent::GetSummonInfo( pItem->GetSummonCode() )->rate != info.value[i] )
					return false;
			}
			break;		
		case MIX_CHECK_BASE_FLAG_ON:
			if( !pItem->GetBaseFlag().IsOn( info.value[i] ) )
				return false;
			break;
		case MIX_CHECK_BASE_FLAG_OFF:
			if( pItem->GetBaseFlag().IsOn( info.value[i] ) ) 
				return false;
			break;
		case MIX_CHECK_DESIGNER_DEFIEND_TYPE:
			if( !check_designer_defined_type( pItem, info.value[i] ) )
				return false;
			break;
		case MIX_CHECK_LOCALE:
			if (!(info.value[i] & g_nCurrentLocalFlag))
				return false;
			break;
		}
	}

	if( !bIsCountChecked )
		*pItemCount = 1;

	return true;
}

const bool post_arrange_check_material_info( const MaterialInfo & info, const StructItem * pMainMaterial, int nSubMaterialCount, StructItem ** pArrangedSubMaterial, unsigned short *pArrangedCountList, const StructItem * pItem, unsigned short *pItemCount )
{
	for( int i = 0 ; i < MaterialInfo::MATERIAL_INFO_COUNT ; ++i )
	{
		switch( info.type[ i ] )
		{
		case MIX_CHECK_SAME_ITEM_ID:
			{
				int nSlotIndex = info.value[ i ];

				// 잘못된 비교 대상 슬롯 번호 데이터
				if( 0 > nSlotIndex && nSlotIndex > nSubMaterialCount )
				{
					assert( 0 );
					return false;
				}

				// 메인 슬롯의 아이템과 같은 아이템 코드
				if( !nSlotIndex )
				{
					if( pItem->GetItemCode() != pMainMaterial->GetItemCode() )
						return false;
				}
				else if( pItem->GetItemCode() != pArrangedSubMaterial[ nSlotIndex - 1 ]->GetItemCode() )
					return false;
			}
			break;
		case MIX_CHECK_SAME_SUMMON_CODE:
			{
				int nSlotIndex = info.value[ i ];

				// 잘못된 비교 대상 슬롯 번호 데이터
				if( 0 > nSlotIndex && nSlotIndex > nSubMaterialCount )
				{
					assert( 0 );
					return false;
				}

				// 메인 슬롯의 아이템과 같은 소환수 코드 (ItemBase에 박혀 있는 소환수 코드를 우선으로 검사한다.)
				int nItemSummonCode = (pItem->GetItemBase().nSummonId)? pItem->GetItemBase().nSummonId : pItem->GetSummonCode();
				int nMaterialSummonCode = (pMainMaterial->GetItemBase().nSummonId)? pMainMaterial->GetItemBase().nSummonId : pMainMaterial->GetSummonCode();

				if( nItemSummonCode != nMaterialSummonCode )
					return false;
			}
			break;
		case MIX_CHECK_SAME_ITEM_ENHANCE:
			{
				int nSlotIndex = info.value[ i ];

				// 잘못된 비교 대상 슬롯 번호 데이터
				if( 0 > nSlotIndex && nSlotIndex > nSubMaterialCount )
				{
					assert( 0 );
					return false;
				}

				// 메인 슬롯의 아이템과 같은 강화 수치
				if( !nSlotIndex )
				{
					if( pItem->GetItemEnhance() != pMainMaterial->GetItemEnhance() )
						return false;
				}
				else if( pItem->GetItemEnhance() != pArrangedSubMaterial[ nSlotIndex - 1 ]->GetItemEnhance() )
					return false;
			}
			break;
		case MIX_CHECK_SAME_SKILL_ID:
			{
				int nSlotIndex = info.value[ i ];

				// 잘못된 비교 대상 슬롯 번호 데이터
				if( 0 > nSlotIndex && nSlotIndex > nSubMaterialCount )
				{
					assert( 0 );
					return false;
				}

				// 메인 슬롯의 아이템과 같은 스킬 카드
				if( !nSlotIndex )
				{
					if( pItem->GetSkillId() != pMainMaterial->GetSkillId() )
						return false;
				}
				else if( pItem->GetSkillId() != pArrangedSubMaterial[ nSlotIndex - 1 ]->GetSkillId() )
					return false;
			}
			break;

		case MIX_CHECK_ITEM_FIRST_SOCKET_CODE_G:
			{
				int nSlotIndex = info.value[ i ];

				// 잘못된 비교 대상 슬롯 번호 데이터
				if( 0 > nSlotIndex && nSlotIndex > nSubMaterialCount )
				{
					assert( 0 );
					return false;
				}

				// 메인 슬롯 아이템의 소켓 값보다 커야 한다.
				if( !nSlotIndex )
				{
					if( pItem->GetSocketCode( 0 ) <= pMainMaterial->GetSocketCode( 0 ) )
						return false;
				}
				else
				{
					if( pItem->GetSocketCode( 0 ) <= pArrangedSubMaterial[ nSlotIndex - 1 ]->GetSocketCode( 0 ) )
						return false;
				}
			}
			break;

		case MIX_CHECK_ITEM_FIRST_SOCKET_CODE_L:
			{
				int nSlotIndex = info.value[ i ];

				// 잘못된 비교 대상 슬롯 번호 데이터
				if( 0 > nSlotIndex && nSlotIndex > nSubMaterialCount )
				{
					assert( 0 );
					return false;
				}

				// 메인 슬롯 아이템의 소켓 값보다 작아야 한다.
				if( !nSlotIndex )
				{
					if( pItem->GetSocketCode( 0 ) >= pMainMaterial->GetSocketCode( 0 ) )
						return false;
				}
				else
				{
					if( pItem->GetSocketCode( 0 ) >= pArrangedSubMaterial[ nSlotIndex - 1 ]->GetSocketCode( 0 ) )
						return false;
				}
			}
			break;
		case MIX_CHECK_SAME_ITEM_CLASS:
			{
				int nSlotIndex = info.value[ i ];

				// 잘못된 비교 대상 슬롯 번호 데이터
				if( 0 > nSlotIndex && nSlotIndex > nSubMaterialCount )
				{
					assert( 0 );
					return false;
				}

				// 메인 슬롯의 아이템과 같은 중분류
				if( !nSlotIndex )
				{
					if( pItem->GetItemClass() != pMainMaterial->GetItemClass() )
						return false;
				}
				else if( pItem->GetItemClass() != pArrangedSubMaterial[ nSlotIndex - 1 ]->GetItemClass() )
					return false;
			}
			break;
		case MIX_CHECK_INCLUDE_RACE_LIMIT:
			{
				int nSlotIndex = info.value[ i ];

				// 잘못된 비교 대상 슬롯 번호 데이터
				if( 0 > nSlotIndex && nSlotIndex > nSubMaterialCount )
				{
					assert( 0 );
					return false;
				}

				const int RACE = ItemBase::LIMIT_DEVA | ItemBase::LIMIT_ASURA | ItemBase::LIMIT_GAIA;

				// 메인 슬롯의 아이템과 같은 종족 제한
				if( !nSlotIndex )
				{
					if( ( pItem->GetItemBase().nLimit & pMainMaterial->GetItemBase().nLimit & RACE ) != ( pItem->GetItemBase().nLimit & RACE ) )
						return false;
				}
				else
				{
					if( ( pItem->GetItemBase().nLimit & pArrangedSubMaterial[ nSlotIndex - 1 ]->GetItemBase().nLimit & RACE ) != ( pItem->GetItemBase().nLimit & RACE ) )
						return false;
				}
			}
			break;
		}
	}

	return true;
}

const MixBase * MixManager::GetProperMixInfoAndArrangeSubMaterials( StructItem * pMainMaterial, int nSubMaterialCount, StructItem ** pSubMaterial, unsigned short *pCountList )
{
	for( std::vector< MixBase >::iterator it = s_vMixInfo.begin(); it != s_vMixInfo.end(); ++it )
	{
		//assert((*it).id != 4067);

		if( (*it).sub_material_cnt != nSubMaterialCount )
			continue;

		// 메인 슬롯은 그냥 무조건 1개인거다
		unsigned short nMainMaterialCount = 1;
		if( !check_material_info( (*it).main_material, pMainMaterial, &nMainMaterialCount ) )
			continue;

		bool bSuccess = true;

		StructItem * apSubMaterialArrangeBuffer[ MixBase::MAX_SUB_MATERIAL_COUNT ] = { NULL };
		unsigned short anSubMaterialArrangeCountList[ MixBase::MAX_SUB_MATERIAL_COUNT ] = { 0 };
		bool abSubMaterialChecked[ MixBase::MAX_SUB_MATERIAL_COUNT ] = { false };

		// 각 보조 재료 개별 조건 체크(루프 조건 체크에서 정확히는 nSubMaterialCount 대신 (*it).sub_material_cnt와 비교하는 게 맞지만 어차피 값은 같으므로 참조 단순화를 위해 nSubMaterialCount를 사용)
		for( int nMaterialInfoIdx = 0 ; nMaterialInfoIdx < nSubMaterialCount ; ++nMaterialInfoIdx )
		{
			bool bFind = false;

			for( int nSubMaterialIdx = 0 ; nSubMaterialIdx < nSubMaterialCount ; ++nSubMaterialIdx )
			{
				// 이미 체크되었던 아이템이면 패스
				if( abSubMaterialChecked[ nSubMaterialIdx ] )
					continue;

				// 현재 재료 체크 조건에 만족되는 아이템이면 해당 아이템 정보를 세팅
				if( check_material_info( (*it).sub_material[ nMaterialInfoIdx ], pSubMaterial[ nSubMaterialIdx ], pCountList + nSubMaterialIdx ) )
				{
					abSubMaterialChecked[ nSubMaterialIdx ] = true;										// pSubMaterial[ nSubMaterialIdx ]는 체크 완료된 아이템이라는 플래그 체크
					apSubMaterialArrangeBuffer[ nMaterialInfoIdx ] = pSubMaterial[ nSubMaterialIdx ];	// nMaterialInfoIdx 번째 보조 재료 조건에 해당된 아이템 포인터 보관
					anSubMaterialArrangeCountList[ nMaterialInfoIdx ] = pCountList[ nSubMaterialIdx ];	// nMaterialInfoIdx 번째 보조 재료 조건에 해당된 아이템 개수 보관

					bFind = true;
					break;
				}
			}

			if( !bFind )
			{
				 bSuccess = false;
				 break;
			}
		}

		if( !bSuccess )
			continue;

		// 보조 재료 개별 조건 체크에 만족된 경우 정렬된 SubMaterial 목록을 가지고 재료간 상호 연관 조건을 체크
		if( !post_arrange_check_material_info( (*it).main_material, pMainMaterial, nSubMaterialCount,
			apSubMaterialArrangeBuffer, anSubMaterialArrangeCountList, pMainMaterial, &nMainMaterialCount ) )
			continue;

		for( int nMaterialInfoIdx = 0 ; nMaterialInfoIdx < nSubMaterialCount ; ++nMaterialInfoIdx )
		{
			if( !post_arrange_check_material_info( (*it).sub_material[ nMaterialInfoIdx ], pMainMaterial, nSubMaterialCount, apSubMaterialArrangeBuffer,
				anSubMaterialArrangeCountList, apSubMaterialArrangeBuffer[ nMaterialInfoIdx ], anSubMaterialArrangeCountList + nMaterialInfoIdx ) )
			{
				bSuccess = false;
				break;
			}
		}

		if( !bSuccess )
			continue;

		// 재료간 상호 연관 조건 체크도 만족된 경우 정렬된 보조 재료 목록 및 개수 목록을 적용 후 조합 정보 리턴
		s_memcpy( pSubMaterial, sizeof( StructItem * ) * nSubMaterialCount, apSubMaterialArrangeBuffer, sizeof( StructItem * ) * nSubMaterialCount );
		s_memcpy( pCountList, sizeof( *pCountList ) * nSubMaterialCount, anSubMaterialArrangeCountList, sizeof( unsigned short ) * nSubMaterialCount );
		return &(*it);
	}

	return NULL;
}

bool MixManager::FindAndDecompose( struct StructPlayer* pPlayer, std::vector< std::pair< struct StructItem*, __int64 > >* vCreatedItem, AR_HANDLE hItem, int nCount )
{
	StructItem::iterator it = StructItem::get( hItem );
	if( !(*it) ) return false;

	StructItem *pItem = static_cast< StructItem * >( *it );
	if( !pItem || !pItem->IsItem() || nCount > pItem->GetCount() || nCount > GameRule::MAX_DECOMPOSE_COUNT_PER_ITEM )
	{
		return false;
	}

	if( !pPlayer->IsDecomposable( pItem ) )
	{
		return false;
	}	

	for( std::vector< DecomposeBase >::iterator it = s_vDecomposeInfo.begin(); it != s_vDecomposeInfo.end(); ++it )
	{
		unsigned short nTemporaryCount = 1;
		if( check_material_info( it->material, pItem, &nTemporaryCount ) )
		{
			int cost = it->neededPrice * nCount;
			// 조합에 필요한 돈 계산
			if( cost > pPlayer->GetGold().GetRawData() )
			{
				return false;
			}

			pPlayer->ChangeGold( pPlayer->GetGold() - cost );

			// 아이템 지우기
			LOG::Log11N4S( LM_ITEM_DECOMPOSE_MATERIAL, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemCode(), nCount, pItem->GetItemEnhance() * 100, pItem->GetItemLevel(), cost, pPlayer->GetGold().GetRawData(), 0, DecomposeBase::DECOMPOSE_CREATE_ITEM, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "DECOMPOSE_CREATE_ITEM", LOG::STR_NTS );
			if( !pPlayer->EraseItem( pItem, nCount ) )
			{
				pPlayer->ChangeGold( pPlayer->GetGold() + cost ); // 삭제된 돈 복구해준다.
				return false;
			}

			switch( it->type )
			{
			case DecomposeBase::DECOMPOSE_CREATE_ITEM:
				if( !MixManager::CreateItemByDecompose( pPlayer, (*it), vCreatedItem, nCount ) )
					return false;
				break;

			default:
				return false;

			}

			return true;
		}
	}

	return false;
}

void MixManager::RegisterEnhanceInfo( EnhanceInfo & info )
{
	std::vector< EnhanceInfo >::iterator it;
	for( it = s_vEnhanceInfo.begin(); it != s_vEnhanceInfo.end(); ++it )
	{
		if( (*it).nSID == info.nSID ) 
		{
			(*it) = info;
			return;
		}
	}

	s_vEnhanceInfo.push_back( info );
}

const EnhanceInfo * getEnhanceInfo( int sid )
{
	std::vector< EnhanceInfo >::iterator it;
	for( it = s_vEnhanceInfo.begin(); it != s_vEnhanceInfo.end(); ++it )
	{
		if( (*it).nSID == sid )
			return &(*it);
	}

	return NULL;
}

bool MixManager::RestoreEnhance( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	int nCurrentEnhance = pMainMaterial->GetItemEnhance();

	StructItem * pPowder = pSubMaterial[0];

	if( !pPowder )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 로그
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pPowder->GetItemEnhance() * 100 + pPowder->GetItemLevel(), pPowder->GetItemCode(), 1, pPowder->GetCount() - 1, pMixInfo->id, pPowder->GetCurrentEtherealDurability(), 0, 0, pPowder->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "EITEM", LOG::STR_NTS );
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "EITEM", LOG::STR_NTS );

	pPlayer->EraseItem( pPowder, 1 );

	ItemUID uid = pMainMaterial->GetItemUID();

	switch( ENV().GetInt( "game.enhance_fail_type", 0 ) )
	{
	case 1:
		nCurrentEnhance = nCurrentEnhance - 1;
		break;
	case 2:
		// 강화 수치 그대로 ㄱㄱ싱
		// nCurrentEnhance = nCurrentEnhance;
		break;
	case 0:
	default:
		nCurrentEnhance = ( ( XRandom( nCurrentEnhance * pMixInfo->value[2], nCurrentEnhance * pMixInfo->value[3] ) / 1000 ) + 5 ) / 10;
		break;
	}

	// 성공
	pMainMaterial->SetItemEnhance( nCurrentEnhance );
	
	if( pMixInfo->value[1] )
		pMainMaterial->SetInstanceFlagOn( pMixInfo->value[0] );
	else
		pMainMaterial->SetInstanceFlagOff( pMixInfo->value[0] );

	// DB 업데이트, SendItemMessage는 이하에서 담당
	pMainMaterial = pPlayer->onAfterChangeItemProperty( pMainMaterial );

	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &tmp, 1 );

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );

	return true;

}

bool MixManager::EnhanceItem( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	int nCurrentEnhance = pMainMaterial->GetItemEnhance();

	const EnhanceInfo *pInfo = getEnhanceInfo( pMixInfo->value[0] );

	// 강화정보가 없음
	if( !pInfo )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 정렬되어 들어오기 때문에 첫 슬롯은 항상 큐브
	StructItem * pCube = pSubMaterial[0];
	if( !pCube || pCube->GetItemCode() != pInfo->nNeedItemCode )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 강화 수치 결정
	int itemEnhance = ( pMixInfo->type == MixBase::MIX_ENHANCE ) ? XRandom( pMixInfo->value[1], pMixInfo->value[2] ) : 1;

	if( pInfo->nMaxEnhance < pMainMaterial->GetItemEnhance() + itemEnhance )
		itemEnhance = pInfo->nMaxEnhance - pMainMaterial->GetItemEnhance();

	// 강화 max치 검사
	if( itemEnhance <= 0 )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 로그
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "EITEM", LOG::STR_NTS );
	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		StructItem *pItem = pSubMaterial[i];
		unsigned short nCount = pCountList[i];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, pCube->GetCurrentEtherealDurability(), 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "EITEM", LOG::STR_NTS );

		pPlayer->EraseItem( pItem, nCount );
	}

	ItemUID uid = pMainMaterial->GetItemUID();

	int nRandom = (int)(pInfo->fPercentage[ nCurrentEnhance ] * 100000);

	bool bResult = false;
	const char * szRes = "FAIL";

	if( (XRandom( 0, 100000 ) <= nRandom) || pPlayer->GetAlwaysPassMix())
	{
		// 성공
		pMainMaterial->SetItemEnhance( nCurrentEnhance + itemEnhance );
		// DB 업데이트, SendItemMessage는 이하에서 담당
		pMainMaterial = pPlayer->onAfterChangeItemProperty( pMainMaterial );

		AR_HANDLE tmp = pMainMaterial->GetHandle();
		SendMixResult( pPlayer, &tmp, 1 );

		szRes = "SUCS";
		bResult = true;

		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );
	}
	else
	{
		// 실패
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );

		if( pMixInfo->type != MixBase::MIX_ENHANCE_WITHOUT_FAIL )
		{
			// procEnhanceFail 함수 안에서 DB Update를 처리하므로 아래에서 따로 처리할 필요 없음
			MixManager::procEnhanceFail( pPlayer, pMainMaterial, pInfo->nFailResult );		
		}
		else
		{
			switch( ENV().GetInt( "game.enhance_fail_type", 0 ) )
			{
			case 1:
				pMainMaterial->SetItemEnhance( nCurrentEnhance - 1 );
			case 2:
				// 강화 수치 그대로 ㄱㄱ싱
				// pMainMaterial->SetItemEnhance( nCurrentEnhance );
				break;
			case 0:
			default:
				pMainMaterial->SetItemEnhance( ( ( XRandom( nCurrentEnhance * pMixInfo->value[2], nCurrentEnhance * pMixInfo->value[3] ) / 1000 ) + 5 ) / 10 );
				break;
			}

			// DB 업데이트, SendItemMessage는 이하에서 담당
			pMainMaterial = pPlayer->onAfterChangeItemProperty( pMainMaterial );
		}

		SendMixResult( pPlayer, NULL, 0 );
	}

	return bResult;
}

bool MixManager::EnhanceSkillCard( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	const EnhanceInfo *pInfo = getEnhanceInfo( pMixInfo->value[0] );

	// 강화정보가 없음
	if( !pInfo )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 정렬되어 들어오기 때문에 첫 슬롯은 항상 큐브
	StructItem * pCube = pSubMaterial[0];
	if( !pCube || pCube->GetItemCode() != pInfo->nNeedItemCode )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 강화 MAX치 검사
	if( pInfo->nMaxEnhance <= pMainMaterial->GetItemEnhance() )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 카드가 중첩되면서 메인 슬롯과 보조 슬롯이 같을 수 있다 이 경우 최소 2개 이상인지 체크 (메인 슬롯 = 보조 슬롯이 되버림)
	if( ( pMainMaterial == pSubMaterial[1] ) && pMainMaterial->GetCount() < 2 )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}
	
	// 보조재료 날려주고
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ESKILLCARD", LOG::STR_NTS );
	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		StructItem *pItem = pSubMaterial[i];
		unsigned short nCount = pCountList[i];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, pCube->GetCurrentEtherealDurability(), 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ESKILLCARD", LOG::STR_NTS );

		pPlayer->EraseItem( pItem, nCount );
	}

	ItemUID uid = pMainMaterial->GetItemUID();
	int nRandom = (int)( pInfo->fPercentage[ pMainMaterial->GetItemEnhance() ] * 100000 );
	
	bool bResult = false;
	const char * szRes = "FAIL";
	
	if( (XRandom( 0, 100000 ) <= nRandom) || pPlayer->GetAlwaysPassMix())
	{
		// 성공했으므로 기존 카드 제거 후 +1짜리 하나 생성 해서 하나 인벤에 넣어준다.
		// 우선 +1강 카드 생성
		StructItem * pNewItem = StructItem::AllocItem( 0, pMainMaterial->GetItemCode(), 1 );
		pNewItem->CopyFrom( pMainMaterial );
		pNewItem->SetItemEnhance( pNewItem->GetItemEnhance() + 1 );
		pNewItem->SetIdx( 0 );
		pNewItem->SetCount( 1 );
		pPlayer->PushItem( pNewItem, 1 );
		// 그리고 기존 메인 슬롯의 카드 1개 제거
		pPlayer->EraseItem( pMainMaterial, 1 );

		AR_HANDLE tmp = pNewItem->GetHandle();

		SendMixResult( pPlayer, &tmp, 1 );

		bResult = true;
		szRes = "SUCS";

		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pNewItem->GetItemEnhance() * 100 + pNewItem->GetItemLevel(), pNewItem->GetItemCode(), 1, pMixInfo->id, pNewItem->GetCurrentEtherealDurability(), 0, 0, 0, pNewItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );
	}
	else
	{
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );

		// 실패처리. 만약 MIX_ENHANCE_SKILL_CARD_WITHOUT_FAIL 타입이면 메인 카드는 살려준다.
		if( pMixInfo->type != MixBase::MIX_ENHANCE_SKILL_CARD_WITHOUT_FAIL )
		{
			// procEnhanceFail 함수 안에서 DB Update를 처리하므로 아래에서 따로 처리할 필요 없음
			MixManager::procEnhanceFail( pPlayer, pMainMaterial, pInfo->nFailResult );
		}

		SendMixResult( pPlayer, NULL, 0 );
	}

	return bResult;
}

bool MixManager::EnhanceCreatureCard( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	struct StructItem *pMainCreatureCard	= pMainMaterial;
	struct StructItem *pSubCreatureCard		= pSubMaterial[ 0 ];
	struct StructItem *pSoulMixer			= pSubMaterial[ 1 ];
	struct StructItem *pJoker				= NULL;
	if( pMixInfo->type == MixBase::MIX_ENHANCE_CREATURE_CARD_WITH_JOKER )
		pJoker								= pSubMaterial[ 2 ];

	// 조커 카드는 예외적으로 강화가 제한된다.
	if( pMainCreatureCard->GetItemCode() == ItemBase::ITEM_CODE_JOKER )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 내구도가 다한 카드는 조합할 수 없다.
	if( ( pMainCreatureCard->GetMaxEtherealDurability() && !pMainCreatureCard->GetCurrentEtherealDurability() ) || ( pSubCreatureCard->GetMaxEtherealDurability() && !pSubCreatureCard->GetCurrentEtherealDurability() ) )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 두 크리처카드는 강화 단계가 같아야 함.
	if( pMainCreatureCard->GetItemEnhance() != pSubCreatureCard->GetItemEnhance() )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	const EnhanceInfo *pInfo = getEnhanceInfo( pMixInfo->value[0] );

	// 강화정보가 없음
	if( !pInfo )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 강화 max치 검사
	if( pInfo->nMaxEnhance <= pMainCreatureCard->GetItemEnhance() )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 편성되어 있으면 실패
	for( int nBindIndex = 0 ; nBindIndex < 6 ; ++nBindIndex )
	{
		StructItem *pItem = pPlayer->GetSummonCardAt( nBindIndex );

		if( pItem == pMainCreatureCard || pItem == pSubCreatureCard || ( pJoker && pItem == pJoker ) )
		{
			SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
			return false;
		}
	}

	// 확률 결정 : 기본 강화 확률 + 크리처 레벨의 합 / 8 + 조커 카드 성공률
	int nMainLevel	= pMainCreatureCard->GetSummonStruct() ? pMainCreatureCard->GetSummonStruct()->GetLevel() : 1;
	int nSubLevel	= pSubCreatureCard->GetSummonStruct() ? pSubCreatureCard->GetSummonStruct()->GetLevel() : 1;

	int nRate = static_cast< int >( pInfo->fPercentage[ pMainCreatureCard->GetItemEnhance() ] * 100000 );
	int nAdditionRate = ( nMainLevel + nSubLevel ) * 125;
	if( pJoker && pJoker->GetSummonStruct() )
	{
		StructSkill *pSkill = pJoker->GetSummonStruct()->GetCurrentPassiveSkill( StructSkill::SKILL_FRIENDSHIP_OF_CROWN );
		if( pSkill ) nAdditionRate += pSkill->GetVar( 0 ) * pSkill->GetCurrentSkillLevel() * 1000;
	}

	// 로그
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainCreatureCard->GetItemEnhance() * 100 + pMainCreatureCard->GetItemLevel(), pMainCreatureCard->GetItemCode(), 1, pMainCreatureCard->GetCount() - 1, pMixInfo->id, 0, 0, 0, pMainCreatureCard->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "MIX_ENHANCE_CREATURE_CARD", LOG::STR_NTS );
	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		StructItem *pItem = pSubMaterial[i];
		unsigned short nCount = pCountList[i];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, 0, 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "MIX_ENHANCE_CREATURE_CARD", LOG::STR_NTS );

		pPlayer->EraseItem( pItem, nCount );
	}

	bool bResult = false;
	const char * szRes = "FAIL";
	
	if(pPlayer->GetAlwaysPassMix() || XRandom( 0, 100000 ) <= nRate + nAdditionRate )
	{
		// 강화 수치에 맞는 카드 내구도 세팅
		pMainCreatureCard->SetItemEnhance( pMainCreatureCard->GetItemEnhance() + 1 );
		pMainCreatureCard->SetCurrentEtherealDurability( pMainCreatureCard->GetMaxEtherealDurability() );

		if( pMainCreatureCard->GetSummonStruct() )
		{
			StructSummon *pSummon = pMainCreatureCard->GetSummonStruct();

			pSummon->Reborn();
			pSummon->DBQuery( new DB_UpdateSummon( pSummon ) );
		}

		// DB 업데이트, SendItemMessage는 이하에서 담당
		pMainCreatureCard = pPlayer->onAfterChangeItemProperty( pMainCreatureCard );

		AR_HANDLE tmp = pMainCreatureCard->GetHandle();

		SendMixResult( pPlayer, &tmp, 1 );

		bResult = true;
		szRes = "SUCS";

		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainCreatureCard->GetItemEnhance() * 100 + pMainCreatureCard->GetItemLevel(), pMainCreatureCard->GetItemCode(), 1, pMixInfo->id, pMainCreatureCard->GetCurrentEtherealDurability(), 0, 0, 0, pMainCreatureCard->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );

		pPlayer->UpdateTitleConditionBySummonEnhance( pMainCreatureCard->GetSummonCode(), GameContent::GetSummonInfo( pMainCreatureCard->GetSummonCode() )->rate, pMainCreatureCard->GetItemEnhance() );
	}
	else
	{
		// +3 이상의 강화 카드/노멀 베이직 이상
		if( pMainCreatureCard->GetItemEnhance() >= 3 &&
			GameContent::GetSummonInfo( pMainCreatureCard->GetSummonCode() )->rate >= 1 &&
			(int)XRandom( 0, 100 ) <= pMixInfo->value[5] )
		{
			// 아이템 할당
			ItemBase::ItemCode nItemID = pMixInfo->value[3];
			__int64 nItemCount = 1;

			while( nItemID < 0 )
			{
				GameContent::SelectItemIDFromDropGroup( nItemID, nItemID, nItemCount );
			}

			if( nItemID > 0 )
			{
				StructItem *pItem = StructItem::AllocItem( 0, nItemID, nItemCount, ItemInstance::BY_MIX, -1 );

				if( pItem->IsJoinable() )
				{
					PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@254\v#@item_name@#\v@%d\v#@item_num@#\v%d", pItem->GetItemBase().nNameId, pItem->GetCount() );
				}
				else
				{
					PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@253\v#@item_name@#\v@%d", pItem->GetItemBase().nNameId );
				}

				StructItem *pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );
				if( pNewItem )
				{
					LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "MIX", LOG::STR_NTS );
				}

				if( pNewItem != pItem ) 
				{
					StructItem::PendFreeItem( pItem );
				}
			}
		}

		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainCreatureCard->GetItemEnhance() * 100 + pMainCreatureCard->GetItemLevel(), pMainCreatureCard->GetItemCode(), 1, pMixInfo->id, pMainCreatureCard->GetCurrentEtherealDurability(), 0, 0, 0, pMainCreatureCard->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );

		// procEnhanceFail 함수 안에서 DB Update를 처리하므로 아래에서 따로 처리할 필요 없음 
		MixManager::procEnhanceFail( pPlayer, pMainCreatureCard, pInfo->nFailResult );

		SendMixResult( pPlayer, NULL, 0 );
	}

	return bResult;
}

bool MixManager::UltimateEnhance( const MixBase * pMixInfo, StructPlayer * pPlayer, StructItem * pMainMaterial, int nSubMaterialCount, StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	// 유형 111: 궁극 강화
	// 값0: 강화ID, 값1: 궁극강화를하기위한최소강화수치, 값2/값3: 강화 수치 최소/최대
	
	const EnhanceInfo *pInfo = getEnhanceInfo( pMixInfo->value[0] );
	// 강화정보가 없음
	if( !pInfo )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	int nCurrentEnhance = pMainMaterial->GetItemEnhance();
	// 최소 강화 수치 만족하나 검사
	if( pMixInfo->value[1] > nCurrentEnhance )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 기본적으로 궁극 강화를 하려면 정상적인 아이템이어야한다. 유니드카드화/강화실패작/내구0짜리 안됨
	if( pMainMaterial->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_CARD ) ||
		pMainMaterial->GetInstanceFlag().IsOn( ItemInstance::ITEM_FLAG_FAILED ) ||
		( pMainMaterial->GetMaxEtherealDurability() && !pMainMaterial->GetCurrentEtherealDurability() ) )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 첫 슬롯은 항상 강화 촉매 아이템
	StructItem * pMaterial = pSubMaterial[0];
	if( !pMaterial || pMaterial->GetItemCode() != pInfo->nNeedItemCode )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 강화 수치 결정
	int itemEnhance = XRandom( pMixInfo->value[2], pMixInfo->value[3] );

	if( pInfo->nMaxEnhance < pMainMaterial->GetItemEnhance() + itemEnhance )
		itemEnhance = pInfo->nMaxEnhance - pMainMaterial->GetItemEnhance();

	// 강화 max치 검사
	if( itemEnhance <= 0 )
	{
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 로그
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ULTI_ENHANCE", LOG::STR_NTS );
	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		StructItem *pItem = pSubMaterial[i];
		unsigned short nCount = pCountList[i];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, pItem->GetCurrentEtherealDurability(), 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ULTI_ENHANCE", LOG::STR_NTS );

		pPlayer->EraseItem( pItem, nCount );
	}

	ItemUID uid = pMainMaterial->GetItemUID();

	int nRandom = (int)(pInfo->fPercentage[ nCurrentEnhance ] * 100000);

	bool bResult = false;
	const char * szRes = "FAIL";
	
	if( (XRandom( 0, 100000 ) <= nRandom ) || pPlayer->GetAlwaysPassMix())
	{
		// 성공
		pMainMaterial->SetItemEnhance( nCurrentEnhance + itemEnhance );
		// DB 업데이트, SendItemMessage는 이하에서 담당
		pMainMaterial = pPlayer->onAfterChangeItemProperty( pMainMaterial );

		AR_HANDLE tmp = pMainMaterial->GetHandle();
		SendMixResult( pPlayer, &tmp, 1 );

		szRes = "SUCS";
		bResult = true;

		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );
	}
	else
	{
		// 실패
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );

		// procEnhanceFail 함수 안에서 DB Update를 처리하므로 따로 처리할 필요 없음
		MixManager::procEnhanceFail( pPlayer, pMainMaterial, pInfo->nFailResult );
		SendMixResult( pPlayer, NULL, 0 );
	}

	return bResult;
}

bool MixManager::MixItemLevel( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	int nEnhance = 0;
	int nLevel = 0;
	
	if( pMixInfo->type == MixBase::MIX_SET_LEVEL ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_CREATE_ITEM ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL )
	{
		nEnhance = pMixInfo->value[0] / 100;
		if( !nEnhance && pMainMaterial ) nEnhance = pMainMaterial->GetItemEnhance();
		nLevel = pMixInfo->value[0] % 100;
		if( !nLevel && pMainMaterial ) nLevel = pMainMaterial->GetItemLevel();
	}
	else if( pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL_SET_ZERO )
	{
		nEnhance = pMixInfo->value[0] / 100;
		nLevel = pMixInfo->value[0] % 100;
	}
	else if( pMixInfo->type == MixBase::MIX_SET_LEVEL_ON_SUB_MATERIAL_LEVEL_SET_FLAG )
	{
		int i = pMixInfo->value[0];
		assert( i >= 1 && i <= nSubMaterialCount );

		nEnhance = pSubMaterial[i-1]->GetItemEnhance();
		nLevel = pSubMaterial[i-1]->GetItemLevel();
	}
	else if( pMixInfo->type == MixBase::MIX_ADD_LEVEL ||
			 pMixInfo->type == MixBase::MIX_ADD_LEVEL_CREATE_ITEM ||
			 pMixInfo->type == MixBase::MIX_ADD_LEVEL_SET_FLAG ||
			 pMixInfo->type == MixBase::MIX_ADD_LEVEL_SET_FLAG_CREATE_ITEM )
	{
		nEnhance = pMainMaterial->GetItemEnhance() + pMixInfo->value[0] / 100;
		nLevel = pMainMaterial->GetItemLevel() + pMixInfo->value[0] % 100;
	}

	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial ? pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel() : 0, pMainMaterial ? pMainMaterial->GetItemCode() : 0, 1, pMainMaterial ? pMainMaterial->GetCount() : 0, pMixInfo->id, pMainMaterial ? pMainMaterial->GetCurrentEtherealDurability() : 0, 0, 0, pMainMaterial ? pMainMaterial->GetItemUID() : 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ITEMLVUP", LOG::STR_NTS );

	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pSubMaterial[i]->GetItemEnhance() * 100 + pSubMaterial[i]->GetItemLevel(), pSubMaterial[i]->GetItemCode(), 1, pSubMaterial[i]->GetCount() - 1, pMixInfo->id, pSubMaterial[i]->GetCurrentEtherealDurability(), 0, 0, pSubMaterial[i]->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ITEMLVUP", LOG::STR_NTS );
		pPlayer->EraseItem( pSubMaterial[i], pCountList[i] );
	}

	bool bSuccess = true;

	StructItem * pNewItem = NULL;
	
	if( pMixInfo->type == MixBase::MIX_SET_LEVEL_CREATE_ITEM ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL_SET_ZERO ||
		pMixInfo->type == MixBase::MIX_ADD_LEVEL_CREATE_ITEM ||
		pMixInfo->type == MixBase::MIX_ADD_LEVEL_SET_FLAG_CREATE_ITEM )		
	{
		if( ((int)XRandom( 0, 100 ) <= pMixInfo->value[5] ) || pPlayer->GetAlwaysPassMix())
		{
			// 아이템 할당
			ItemBase::ItemCode nItemID = pMixInfo->value[3];
			__int64 nItemCount = 1;

			if( nItemID < 0 )
			{
				GameContent::SelectItemIDFromDropGroup( nItemID, nItemID, nItemCount );
			}

			if( nItemID > 0 )
			{
				int nEnhance = pMixInfo->value[4] / 100;
				int nLevel = pMixInfo->value[4] % 100;

				if( pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL ||
					pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL_SET_ZERO )
				{
					nEnhance = pMainMaterial->GetItemEnhance();
					nLevel = pMainMaterial->GetItemLevel();
				}
				
				StructItem *pItem = StructItem::AllocItem( 0, nItemID, nItemCount, ItemInstance::BY_MIX, nLevel, nEnhance );

				if( pItem->IsJoinable() )
				{
					PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@254\v#@item_name@#\v@%d\v#@item_num@#\v%d", pItem->GetItemBase().nNameId, pItem->GetCount() );
				}
				else
				{
					PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@253\v#@item_name@#\v@%d", pItem->GetItemBase().nNameId );
				}

				pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );
				if( pNewItem )
				{
					LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "MIX", LOG::STR_NTS );
				}
				
				if( pNewItem != pItem ) 
				{
					StructItem::PendFreeItem( pItem );
				}
			}
		}
		else
			bSuccess = false;
	}

	if( pMainMaterial )
	{
		pMainMaterial->SetItemEnhance( nEnhance );
		pMainMaterial->SetItemLevel( nLevel );
	}

	if(	pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_SET_FLAG_CREATE_ITEM_WITH_MAIN_MATERIAL_LEVEL_SET_ZERO ||
		pMixInfo->type == MixBase::MIX_SET_LEVEL_ON_SUB_MATERIAL_LEVEL_SET_FLAG ||
		pMixInfo->type == MixBase::MIX_ADD_LEVEL_SET_FLAG ||
		pMixInfo->type == MixBase::MIX_ADD_LEVEL_SET_FLAG_CREATE_ITEM )
	{
		if( pMainMaterial )
		{
			if( pMixInfo->value[2] )
				pMainMaterial->SetInstanceFlagOn( pMixInfo->value[1] );
			else
				pMainMaterial->SetInstanceFlagOff( pMixInfo->value[1] );
		}
	}

	if( bSuccess )
	{
		if( pMainMaterial )
			LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
		if( pNewItem )
			LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pNewItem->GetItemEnhance() * 100 + pNewItem->GetItemLevel(), pNewItem->GetItemCode(), 1, pMixInfo->id, pNewItem->GetCurrentEtherealDurability(), 0, 0, 0, pNewItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );

		AR_HANDLE tmp = 0;
		
		if( pMainMaterial )
		{
			tmp = pMainMaterial->GetHandle();
		}

		SendMixResult( pPlayer, &tmp, 1 );
	}
	else
	{
		if( pMainMaterial )
			LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0,  "FAIL", LOG::STR_NTS );

		SendMixResult( pPlayer, NULL, 0 );
	}

	if( pMainMaterial )
	{
		// DB 업데이트, SendItemMessage는 이하에서 담당
		pPlayer->onAfterChangeItemProperty( pMainMaterial );
	}

	// Save? 이 짓을 해야하나 싶다.. 우선 보류. 나중에 쓸데없는 Save 싹 다 리팩토링 예정
	pPlayer->Save();

	return true;
}

bool MixManager::RecycleItem( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	// 첫 번째가 장비고, 두 번째가 큐브라는 건 이미 밖에서 검사하고 들어왔음. (Check type/value들에 의해서)
	StructItem * pEquipment = pSubMaterial[0];
	StructItem * pReturnCube = pSubMaterial[1];

	// 재료 순서 조정 (장비인지 아닌지 구분해야 함)
	if( 9 < pEquipment->GetItemGroup() || pEquipment->GetItemGroup() < 1 )
	{
		StructItem *pTemp = pEquipment;
		pEquipment = pReturnCube;
		pReturnCube = pTemp;
	}

	// 로그
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "EITEM", LOG::STR_NTS );
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pEquipment->GetItemEnhance() * 100 + pEquipment->GetItemLevel(), pEquipment->GetItemCode(), 1, pEquipment->GetCount() - 1, pMixInfo->id, pEquipment->GetCurrentEtherealDurability(), 0, 0, pEquipment->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "EITEM", LOG::STR_NTS );
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pReturnCube->GetItemEnhance() * 100 + pReturnCube->GetItemLevel(), pReturnCube->GetItemCode(), 1, pReturnCube->GetCount() - 1, pMixInfo->id, pReturnCube->GetCurrentEtherealDurability(), 0, 0, pReturnCube->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "EITEM", LOG::STR_NTS );

	int nEnhanceEquip = pEquipment->GetItemEnhance();

	// 어쨌든 큐브는 없앤다(장비는 실패 시에 로그 남긴 이후에 삭제, 성공 시에는 즉시 삭제)
	pPlayer->EraseItem( pReturnCube, 1 );

	int nRandom = pMixInfo->value[4] * 100;
	
	bool bResult = false;
	const char * szRes = "FAIL";
	
	if( (XRandom( 1, 10000 ) <= nRandom ) || pPlayer->GetAlwaysPassMix())
	{
		// 성공(장비 아이템 즉시 삭제)
		pPlayer->EraseItem( pEquipment, 1 );

		// 결과물 몇 개 생성할 건지 결정
		__int64 nResultCnt;

		// MIX_RECYCLE_ENHANCE일 경우는 10000 : 1로 대응되므로 연산 후에 10000으로 나눠야 개수가 됨
		// 반올림 처리를 단순화하기 위해 1000으로 나누고 5를 더한 후 10으로 나눔(몫만 취하므로 반올림 됨)
		if( pMixInfo->type == MixBase::MIX_RECYCLE_ENHANCE )
			nResultCnt = ( ( XRandom( nEnhanceEquip * pMixInfo->value[2], nEnhanceEquip * pMixInfo->value[3] ) / 1000 ) + 5 ) / 10;
		else
			nResultCnt = XRandom( nEnhanceEquip * pMixInfo->value[2], nEnhanceEquip * pMixInfo->value[3] );

		// 생성될 개수가 0이면 실패 처리
		if( !nResultCnt )
		{
			SendMixResult( pPlayer, NULL, 0 );
			return false;
		}
			
		StructItem * pItem = StructItem::AllocItem( 0, pMixInfo->value[0], nResultCnt, ItemInstance::BY_MIX, pMixInfo->value[1] );

		if( pItem->IsJoinable() )
		{
			PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@254\v#@item_name@#\v@%d\v#@item_num@#\v%d", pItem->GetItemBase().nNameId, pItem->GetCount() );
		}
		else
		{
			PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@253\v#@item_name@#\v@%d", pItem->GetItemBase().nNameId );
		}

		StructItem *pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );
		if( !pNewItem )
		{
			return bResult;
		}

		LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "MIX", LOG::STR_NTS );

		if( pNewItem != pItem ) 
		{
			StructItem::PendFreeItem( pItem );
		}

		pNewItem->TurnOnUpdateFlag();

		pNewItem->DBQuery( new DB_UpdateItem( pNewItem ) );

		szRes = "SUCS";
		bResult = true;

		AR_HANDLE hItem = pNewItem->GetHandle();
		SendMixResult( pPlayer, &hItem, 1 );

		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pNewItem->GetItemEnhance() * 100 + pNewItem->GetItemLevel(), pNewItem->GetItemCode(), 1, pMixInfo->id, pNewItem->GetCurrentEtherealDurability(), 0, 0, 0, pNewItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );
	}
	else
	{
		// 실패 - 따로 처리할 게 없다
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pEquipment->GetItemEnhance() * 100 + pEquipment->GetItemLevel(), pEquipment->GetItemCode(), 1, pMixInfo->id, pEquipment->GetCurrentEtherealDurability(), 0, 0, 0, pEquipment->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, szRes, LOG::STR_NTS );

		pPlayer->EraseItem( pEquipment, 1 );

		SendMixResult( pPlayer, NULL, 0 );
	}

	return bResult;
}

bool MixManager::CreateItem( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	unsigned short nCount = 1;
	if( pMixInfo->type == MixBase::MIX_CREATE_ITEM_IN_MASS )
	{
		nCount = USHRT_MAX;

		// 605번 유형은 메인 재료가 없어야 한다.
		if( pMainMaterial )
			return false;

		for( int i = 0 ; i < nSubMaterialCount ; ++i )
		{
			bool bFind = false;
			for( int j = 0 ; j < MixBase::VALUE_COUNT ; ++j )
			{
				bFind = false;
				if( pMixInfo->sub_material[i].type[j] == MIX_CHECK_ITEM_COUNT_GE )
				{
					unsigned short nUnitCount = (unsigned short)pMixInfo->sub_material[i].value[j];
					nCount = std::min( (unsigned short)( pCountList[i] / nUnitCount ), nCount );
					pCountList[i] = nUnitCount;
					bFind = true;
					break;
				}
			}
			// 605번 유형은 반드시 15번 개수 이상 체크 조건이 있어야 한다.
			if( !bFind )
				return false;
			bFind = false;
		}

		if( nCount > MixBase::MAX_CREATABLE_ITEM )
		{
			nCount = MixBase::MAX_CREATABLE_ITEM;
		}
	}
	
	if( pMainMaterial )
	{
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, nCount, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "CREATE_ITEM", LOG::STR_NTS );
		pPlayer->EraseItem( pMainMaterial, pMainMaterial->GetCount() );
	}

	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pSubMaterial[i]->GetItemEnhance() * 100 + pSubMaterial[i]->GetItemLevel(), pSubMaterial[i]->GetItemCode(), 1, pSubMaterial[i]->GetCount() - 1, pMixInfo->id, pSubMaterial[i]->GetCurrentEtherealDurability(), 0, nCount, pSubMaterial[i]->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "CREATE_ITEM", LOG::STR_NTS );
		pPlayer->EraseItem( pSubMaterial[i], pCountList[i] * nCount );
	}
	
	std::vector< std::pair< ItemBase::ItemCode, __int64 > > vItemList;
	int nFailCount = 0;
	
	for( int i = 0 ; i < nCount ; ++i )
	{
		if( (pMixInfo->value[ 2 ] > XRandom( 0, 99 ) ) || pPlayer->GetAlwaysPassMix())
		{
			int nCount = XRandom( pMixInfo->value[3], pMixInfo->value[4] );
			int nResultCount = ( pMixInfo->value[ 0 ] >= 0 ) ? 1 : nCount;

			for( int i = 0 ; i < nResultCount ; ++i )
			{
				ItemBase::ItemCode nItemID = pMixInfo->value[ 0 ];
				__int64 nItemCount = nCount;

				while( nItemID < 0 )
				{
					GameContent::SelectItemIDFromDropGroup( nItemID, nItemID, nItemCount );
				}

				// 구조 트리 또는 해쉬 맵으로 개선 (나중에)
				bool bFind = false;
				for( auto it = vItemList.begin() ; it != vItemList.end() ; ++it )
				{
					if( (*it).first == nItemID )
					{
						bFind = true;
						(*it).second += nItemCount;
					}
				}

				if( !bFind )
					vItemList.push_back( std::pair< ItemBase::ItemCode, __int64 >( nItemID, nItemCount ) );
			}
		}
		else
		{
			nFailCount++;
		}
	}

	for( auto it = vItemList.begin() ; it != vItemList.end() ; ++it )
	{
		ItemBase::ItemCode nItemID = (*it).first;
		__int64 nItemCount = (*it).second;

		// 만약 결과물이 중첩 불가능한 아이템이면 일일이 넣어준다.
		__int64 nLoopCount = 1;
		ItemBase &itemBase = StructItem::GetItemBase( nItemID );
		if( !itemBase.Flag.IsOn( ItemBase::FLAG_JOIN ) )
		{
			nLoopCount = nItemCount;
			nItemCount = 1;
		}
					
		for( __int64 i = 0 ; i < nLoopCount ; ++i )
		{
			StructItem * pNewItem = NULL;

			StructItem * pItem = StructItem::AllocItem( 0, nItemID, nItemCount, ItemInstance::BY_MIX, pMixInfo->value[1] );
			nItemCount = pItem->GetCount();
				
			pNewItem = pPlayer->PushItem( pItem, nItemCount );
			if( !pNewItem )
			{
				continue;
			}
			LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pNewItem->GetItemEnhance() * 100 + pNewItem->GetItemLevel(), pNewItem->GetItemCode(), nItemCount, pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "MIX", LOG::STR_NTS );

			if( pNewItem != pItem ) 
			{
				StructItem::PendFreeItem( pItem );
			}

			vCreatedItem->push_back( std::make_pair( pNewItem, nItemCount ) );
					
			if( pNewItem )
				LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pNewItem->GetItemEnhance() * 100 + pNewItem->GetItemLevel(), pNewItem->GetItemCode(), nCount, pMixInfo->id, pNewItem->GetCurrentEtherealDurability(), 0, 0, 0, pNewItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
			else
				LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 0, pMixInfo->id, 0, 0, 0, 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
		}
	}

	if( nFailCount > 0 )
	{
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 0, pMixInfo->id, 0, 0, 0, nFailCount, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "FAIL_COUNT", LOG::STR_NTS );
	}

	if( vCreatedItem->empty() )
	{
		SendMixResult( pPlayer, NULL, 0 );
	}
	else
	{
		AR_HANDLE hItem = NULL;
		for( auto it = vCreatedItem->begin() ; it != vCreatedItem->end() ; ++it )
		{
			StructItem *pItem = (*it).first;
			__int64 nCount = (*it).second;
			hItem = pItem->GetHandle();

			PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@254\v#@item_name@#\v@%d\v#@item_num@#\v%d", pItem->GetItemBase().nNameId, nCount );
		}
		SendMixResult( pPlayer, &hItem, 1 );
	}
	
	return true;
}

bool MixManager::ChangeItemCode(const MixBase* pMixInfo, struct StructPlayer* pPlayer, struct StructItem* pMainMaterial, int nSubMaterialCount, struct StructItem** pSubMaterial, unsigned short* pCountList, std::vector< std::pair< StructItem*, __int64 > >* vCreatedItem)
{

	int nCode = 0;

	switch (pMixInfo->type)
	{
		case MixBase::MIX_REPLACE_WITH:
			nCode = pMixInfo->value[1];
			break;

		case MixBase::MIX_CHANGE_CODE_ADD:
			nCode = pMainMaterial->GetItemCode() + pMixInfo->value[1];
			break;

		case MixBase::MIX_REPLACE_WITH_RANDOM:
			nCode = XRandom(0, pMixInfo->value[2]) + pMixInfo->value[1];
			break;

		default:
			return false;
	}


	bool bResult = (XRandom(0,100) < pMixInfo->value[0]) || pPlayer->GetAlwaysPassMix();
	if (bResult) {
		if (!pMainMaterial->ChangeItemCode(nCode)) {
			SendResult(pPlayer, pMixInfo->id, RESULT_INVALID_ARGUMENT);
			return false;
		}
	}

	LOG::Log11N4S(LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "CHANGE_CODE", LOG::STR_NTS);

	for (int i = 0; i < nSubMaterialCount; ++i)
	{
		LOG::Log11N4S(LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pSubMaterial[i]->GetItemEnhance() * 100 + pSubMaterial[i]->GetItemLevel(), pSubMaterial[i]->GetItemCode(), 1, pSubMaterial[i]->GetCount() - 1, pMixInfo->id, pSubMaterial[i]->GetCurrentEtherealDurability(), 0, 0, pSubMaterial[i]->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "CHANGE_CODE", LOG::STR_NTS);

		if (pPlayer->IsErasable(pSubMaterial[i]) && (pSubMaterial[i]->GetCount() != 0) && (pCountList[i] != 0))
		{
			pPlayer->EraseItem(pSubMaterial[i], pCountList[i]);
		}
	}

	SendItemMessage(pPlayer, pMainMaterial);

	pMainMaterial->DBQuery(new DB_UpdateItem(pMainMaterial));

	LOG::Log11N4S(LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), 
		pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, 
		pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, bResult ? "SUCS" : "FAIL", LOG::STR_NTS);

	
	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult(pPlayer, &tmp, bResult ? 1 : 0);
	
	return true;
}


bool MixManager::ChangeAppearanceCode( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	StructItem * pAppearanceSupplier = pSubMaterial[0];
	ItemBase::ItemCode nAppearanceCode = pAppearanceSupplier->GetItemCode();

	// 현재는 실패하는 경우가 없으나 만약 값 설정에 실패하였을 때 재료가 사라져서는 안 되므로 확인을 한다.
	if( !pMainMaterial->SetAppearanceCode( nAppearanceCode ) )
	{
		SendResult( pPlayer, pMixInfo->id, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 로그 및 재료 아이템 제거
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "CHANGE_APPEARANCE", LOG::STR_NTS );

	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pSubMaterial[i]->GetItemEnhance() * 100 + pSubMaterial[i]->GetItemLevel(), pSubMaterial[i]->GetItemCode(), 1, pSubMaterial[i]->GetCount() - 1, pMixInfo->id, pSubMaterial[i]->GetCurrentEtherealDurability(), 0, 0, pSubMaterial[i]->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "CHANGE_APPEARANCE", LOG::STR_NTS );
		pPlayer->EraseItem( pSubMaterial[i], pCountList[i] );
	}

	SendItemMessage( pPlayer, pMainMaterial );
	pMainMaterial->DBQuery( new DB_UpdateItem( pMainMaterial ) );

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &tmp, 1 );

	return true;
}

bool MixManager::ChangeItemUsablePeriod( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	// MIX_CHANGE_USABLE_PERIOD 조합유형
	// value[0]: 0일때 초기화, 1일때 value[1]에 적힌 기간만큼 기간 연장

	if( pMixInfo->value[0] == 0 )
	{
		pMainMaterial->SetRemainTime( time( NULL ) + pMainMaterial->GetItemBase().available_time );
	}
	else if( pMixInfo->value[0] == 1 && pMixInfo->value[1] > 0 )
	{
		int nExtendingTime = pMixInfo->value[1];

		// remain_time늘려줄 때 int_max 값을 넘어버리면 안 해줌. (DB에 남은시간을 int로 넣기때문)
		time_t remain_time = 0;
		if( pMainMaterial->GetItemBase().decrease_type == ItemBase::DECREASE_ALWAYS )
		{
			remain_time = pMainMaterial->GetExpireTime() + nExtendingTime;
		}
		else
		{
			remain_time = pMainMaterial->GetExpireTime() - time( NULL ) + nExtendingTime;
		}
		if( remain_time > INT_MAX )
		{
			SendResult( pPlayer, TM_CS_MIX, RESULT_ENHANCE_LIMIT );
			return false;
		}

		pMainMaterial->SetRemainTime( pMainMaterial->GetExpireTime() + nExtendingTime );
	}
	else
	{
		// 조합식 오류
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	// 로그 및 재료 아이템 제거
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "CHANGE_APPEARANCE", LOG::STR_NTS );

	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pSubMaterial[i]->GetItemEnhance() * 100 + pSubMaterial[i]->GetItemLevel(), pSubMaterial[i]->GetItemCode(), 1, pSubMaterial[i]->GetCount() - 1, pMixInfo->id, pSubMaterial[i]->GetCurrentEtherealDurability(), 0, 0, pSubMaterial[i]->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "EXTEND_ITEM_TIME", LOG::STR_NTS );
		pPlayer->EraseItem( pSubMaterial[i], pCountList[i] );
	}

	SendItemMessage( pPlayer, pMainMaterial );
	pMainMaterial->DBQuery( new DB_UpdateItem( pMainMaterial ) );

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetExpireTime(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &tmp, 1 );

	return true;
}


bool MixManager::SetElementalEffect( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	// 속성 이펙트 적용 아이템과 속성 이펙트 적용 재료가 있고 적절한 아이템인지는 조합식의 체크 조건에 의해 검사되어 있음

	StructItem * pElementalEffector = pSubMaterial[0];

	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ELEMENTAL_EFFECT", LOG::STR_NTS );
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pElementalEffector->GetItemEnhance() * 100 + pElementalEffector->GetItemLevel(), pElementalEffector->GetItemCode(), 1, pElementalEffector->GetCount(), pMixInfo->id, pElementalEffector->GetCurrentEtherealDurability(), 0, 0, pElementalEffector->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ELEMENTAL_EFFECT", LOG::STR_NTS );

	unsigned char cElementalType = pElementalEffector->GetItemBase().fOptVar1[ 0 ].GetAsInt64();
	if( !cElementalType )
	{
		if( pMainMaterial->GetElementalEffectType() && pMainMaterial->GetElementalEffectExpireTime() )
		{
			pPlayer->RemoveFromElementalEffectedItemList( pMainMaterial );
		}
		pMainMaterial->ClearElementalEffect();
	}
	else
	{
		time_t tExpire = pElementalEffector->GetItemBase().fOptVar2[ 0 ] ? time( NULL ) + pElementalEffector->GetItemBase().fOptVar2[ 0 ] : 0;

		if( pMainMaterial->SetElementalEffect( cElementalType, tExpire ) != RESULT_SUCCESS )
		{
			LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "FAIL", LOG::STR_NTS );
			SendMixResult( pPlayer, NULL, 0 );
			return false;
		}

		// 만약 영구 속성이 부여된 아이템이라면 만료 처리를 하지 않기 위해 Elemental Effected Item List에 추가해서는 안 될 뿐더러 행여나 존재할 경우에는 제거를 해줘야 한다.
		if( tExpire )
			pPlayer->AddToElementalEffectedItemList( pMainMaterial );
		else
			pPlayer->RemoveFromElementalEffectedItemList( pMainMaterial );
	}
	pMainMaterial->TurnOnUpdateFlag();

	pPlayer->EraseItem( pElementalEffector, 1 );

	SendItemMessage( pPlayer, pMainMaterial );

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &tmp, 1 );

	return true;
}

bool MixManager::SetElementalEffectParameter( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	// 속성 이펙트 적용된 아이템과 속성 이펙트 추가 성능 적용 재료가 있고 적절한 아이템인지는 조합식의 체크 조건에 의해 검사되어 있음

	StructItem * pElementalEffectEnhancer = pSubMaterial[0];

	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ELEMENTAL_EFFECT_ENHANCE", LOG::STR_NTS );
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pElementalEffectEnhancer->GetItemEnhance() * 100 + pElementalEffectEnhancer->GetItemLevel(), pElementalEffectEnhancer->GetItemCode(), 1, pElementalEffectEnhancer->GetCount(), pMixInfo->id, pElementalEffectEnhancer->GetCurrentEtherealDurability(), 0, 0, pElementalEffectEnhancer->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "ELEMENTAL_EFFECT_ENHANCE", LOG::STR_NTS );

	std::string strResult = "";

	// 공격력 세팅 체크(최소/최대 값 중에 0이지 않은 값이 있는 경우만 적용)
	int nMinValue = pElementalEffectEnhancer->GetItemBase().fOptVar1[ 0 ];
	int nMaxValue = pElementalEffectEnhancer->GetItemBase().fOptVar2[ 0 ];
	if( nMinValue || nMaxValue )
	{
		int nAttackPoint = XRandom( nMinValue, nMaxValue );
		pMainMaterial->SetElementalEffectAttackPoint( nAttackPoint );

		XStringUtil::Format( strResult, "Attack:%d ", nAttackPoint );
	}

	// 마력 세팅 체크(최소/최대 값 중에 0이지 않은 값이 있는 경우만 적용)
	nMinValue = pElementalEffectEnhancer->GetItemBase().fOptVar1[ 1 ];
	nMaxValue = pElementalEffectEnhancer->GetItemBase().fOptVar2[ 1 ];
	if( nMinValue || nMaxValue )
	{
		int nMagicPoint = XRandom( nMinValue, nMaxValue );
		pMainMaterial->SetElementalEffectMagicPoint( nMagicPoint );

		XStringUtil::Format( strResult, "%sMagic:%d", strResult.c_str(), nMagicPoint );
	}

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, strResult.c_str(), LOG::STR_NTS );

	pMainMaterial->TurnOnUpdateFlag();

	pPlayer->EraseItem( pElementalEffectEnhancer, 1 );

	SendItemMessage( pPlayer, pMainMaterial );
	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &tmp, 1 );

	return true;
}

bool MixManager::SetSocket( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	int index = -1;
	for( int i = 0; i < ItemBase::MAX_SOCKET_NUMBER; ++i )
	{
		if( pMixInfo->value[0] == pMainMaterial->GetSocketCode( i ) )
		{
			index = i;
			break;
		}
	}

	// 대상 소울스톤 ID를 찾지 못 했으면 실패
	if( index == -1 ) return false;

	// 로그 및 재료 아이템 제거
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, index, pMixInfo->value[0], 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SET_SOCKET", LOG::STR_NTS );

	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		StructItem *pItem = pSubMaterial[i];
		unsigned short nCount = pCountList[i];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, 0, 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SET_SOCKET", LOG::STR_NTS );

		pPlayer->EraseItem( pItem, nCount );
	}
	
	bool bSuccess = (XRandom( 0, 99 ) < pMixInfo->value[2]) || pPlayer->GetAlwaysPassMix();
	const char *pszResult = ( bSuccess ) ? "SUCS" : "FAIL";
	int code = ( bSuccess ) ? pMixInfo->value[1] : pMixInfo->value[3];

	if( pMixInfo->value[4] && pMixInfo->value[0] == 0 && code != 0 )
	{
		// 대상 아이템이 없음
		assert( StructItem::GetItemBase( code ).nCode );
		pMainMaterial->SetCurrentEndurance( pMainMaterial->GetCurrentEndurance() + StructItem::GetItemBase( code ).nEndurance );
	}

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, index, code, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, pszResult, LOG::STR_NTS );
	
	pMainMaterial->SetSocketCode( index, code );
	pMainMaterial->TurnOnUpdateFlag();
	pMainMaterial->DBQuery( new DB_UpdateItem( pMainMaterial ) );

	if( bSuccess && pMixInfo->value[5] )
	{
		// 성공 시 아이템 할당
		ItemBase::ItemCode nItemID = pMixInfo->value[5];
		__int64 nItemCount = 1;

		while( nItemID < 0 )
		{
			GameContent::SelectItemIDFromDropGroup( nItemID, nItemID, nItemCount );
		}

		if( nItemID > 0 )
		{
			StructItem *pItem = StructItem::AllocItem( 0, nItemID, nItemCount, ItemInstance::BY_MIX );

			if( pItem->IsJoinable() )
			{
				PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@254\v#@item_name@#\v@%d\v#@item_num@#\v%d", pItem->GetItemBase().nNameId, pItem->GetCount() );
			}
			else
			{
				PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@253\v#@item_name@#\v@%d", pItem->GetItemBase().nNameId );
			}

			StructItem *pNewItem = pPlayer->PushItem( pItem, pItem->GetCount() );
			if( pNewItem )
			{
				LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), pItem->GetCount(), pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "MIX", LOG::STR_NTS );
			}

			if( pNewItem != pItem ) 
			{
				StructItem::PendFreeItem( pItem );
			}
		}
	}

	SendItemMessage( pPlayer, pMainMaterial );
	AR_HANDLE hItem = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &hItem, bSuccess );

	return bSuccess;
}

bool MixManager::ReplaceSocketWith( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	// 존재하지 않는 보조 재료를 택함
	if( pMixInfo->value[0] < 1 || pMixInfo->value[0] > nSubMaterialCount )
	{
		assert( 0 );
		SendResult( pPlayer, TM_CS_MIX, RESULT_INVALID_ARGUMENT );
		return false;
	}

	int nTargetIndex = pMixInfo->value[0];
	bool bErase = false;
	bool bReset = false;
	StructItem *pTargetItem = pSubMaterial[nTargetIndex - 1];
	
	// 주 재료에 대한 로그
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "REPLACE_SOCKET_WITH", LOG::STR_NTS );

	// 나머지 재료에 대한 로그 및 삭제
	// 대상 재료는 성공 여부에 따라 로그 포맷이 바뀌므로 따로 처리한다.
	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		if( i == nTargetIndex - 1 ) continue;

		StructItem *pItem = pSubMaterial[i];
		unsigned short nCount = pCountList[i];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, 0, 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SET_SOCKET", LOG::STR_NTS );

		pPlayer->EraseItem( pItem, nCount );
	}

	// 소켓 정보 이전
	
	bool bSuccess = (XRandom( 0, 99 ) < pMixInfo->value[1]) || pPlayer->GetAlwaysPassMix();
	const char *pszResult = ( bSuccess ) ? "SUCS" : "FAIL";
	if( bSuccess )
	{
		for( int i = 0; i < pMainMaterial->GetMaxSocketCount(); i++ )
		{
			pMainMaterial->SetSocketCode( i, pTargetItem->GetSocketCode( i ) );
		}

		pMainMaterial->DBQuery( new DB_UpdateItem( pMainMaterial ) );

		bErase = ( pMixInfo->value[2] == MixBase::RESULT_SOCKET_ERASE );
		bReset = ( pMixInfo->value[2] == MixBase::RESULT_SOCKET_RESET );
	}
	else
	{
		bErase = ( pMixInfo->value[3] == MixBase::RESULT_SOCKET_ERASE );
		bReset = ( pMixInfo->value[3] == MixBase::RESULT_SOCKET_RESET );
	}

	// 대상 재료에 대한 정책
	if( bErase )
	{
		unsigned short nCount = pCountList[nTargetIndex - 1];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pTargetItem->GetItemEnhance() * 100 + pTargetItem->GetItemLevel(), pTargetItem->GetItemCode(), 1, 0, pMixInfo->id, 0, 0, 0, pTargetItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "REPLACE_SOCKET_WITH", LOG::STR_NTS );

		pPlayer->EraseItem( pTargetItem, nCount );
	}
	else if( bReset )
	{
		// 옮겨진 소켓과는 상관없이 모든 소켓이 초기화된다.
		for( int i = 0; i < pTargetItem->GetMaxSocketCount(); i++ )
		{
			pTargetItem->SetSocketCode( i, 0 );
		}

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pTargetItem->GetItemEnhance() * 100 + pTargetItem->GetItemLevel(), pTargetItem->GetItemCode(), 1, 1, pMixInfo->id, 0, 0, 0, pTargetItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "REPLACE_SOCKET_WITH", LOG::STR_NTS );
		LOG::Log11N4S( LM_ITEM_SOCKET_INFO, pPlayer->GetAccountID(), pPlayer->GetSID(), pTargetItem->GetCurrentEndurance() * 100 + pTargetItem->GetItemLevel(), pTargetItem->GetMaxEndurance(), pTargetItem->GetSocketCode( 0 ), pTargetItem->GetSocketCode( 1 ), pTargetItem->GetSocketCode( 2 ), pTargetItem->GetSocketCode( 3 ), pTargetItem->GetSocketCode( 4 ), pTargetItem->GetSocketCode( 5 ), pTargetItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		pTargetItem->DBQuery( new DB_UpdateItem( pTargetItem ) );
		SendItemMessage( pPlayer, pTargetItem );
	}

	// 결과 로그
	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, pszResult, LOG::STR_NTS );
	LOG::Log11N4S( LM_ITEM_SOCKET_INFO, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetCurrentEndurance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetMaxEndurance(), pMainMaterial->GetSocketCode( 0 ), pMainMaterial->GetSocketCode( 1 ), pMainMaterial->GetSocketCode( 2 ), pMainMaterial->GetSocketCode( 3 ), pMainMaterial->GetSocketCode( 4 ), pMainMaterial->GetSocketCode( 5 ), pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	SendItemMessage( pPlayer, pMainMaterial );
	AR_HANDLE hItem = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &hItem, bSuccess );

	return bSuccess;
}

bool MixManager::SacrificeItemForEtherealDurability( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	bool bSuccess = false;

	for( int i = 0 ; i < nSubMaterialCount ; ++i )
	{
		// 완충 상태면 충전 시도 중지(여러 개 충전 중간에 완충 상태가 될 수도 있으므로 매 루프마다 검사)
		if( pMainMaterial->GetCurrentEtherealDurability() >= pMainMaterial->GetMaxEtherealDurability() )
			break;

		StructItem * pSacrifice = pSubMaterial[ i ];
		unsigned short nCount = pCountList[ i ];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SACRIFICE_ITEM_FOR_ETHEREAL_DURABILITY", LOG::STR_NTS );
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pSacrifice->GetItemEnhance() * 100 + pSacrifice->GetItemLevel(), pSacrifice->GetItemCode(), nCount, pSacrifice->GetCount() - nCount, pMixInfo->id, 0, 0, 0, pSacrifice->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SACRIFICE_ITEM_FOR_ETHEREAL_DURABILITY", LOG::STR_NTS );

		// 1번째 보조 재료는 복구용이 아니고 그냥 소모용 재료임(고정 소모 재료 아이템에 대한 결과 로그는 남지 않음)
		if( i != 0 )
		{
			// 성공률 체크
			
			if( (XRandom( 0, 99 ) < pMixInfo->value[ 3 ] ) || pPlayer->GetAlwaysPassMix())
			{
				bSuccess = true;

				__int64 nRecover = pSacrifice->GetItemBase().nPrice.GetRawData() * pMixInfo->value[ 0 ] / 100 * nCount;
				nRecover += pMixInfo->value[ 1 ];

				// 파괴된 보조 재료템은 25%만 적용
				if( pSacrifice->GetMaxEtherealDurability() && !pSacrifice->GetCurrentEtherealDurability() )
					nRecover /= 4;

				pMainMaterial->AddCurrentEtherealDurability( static_cast< int >( nRecover ) );
				if( pMixInfo->type == MixBase::MIX_SACRIFICE_ITEM_FOR_ETHEREAL_DURABILITY_WITH_MESSAGE )
					PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@7900\v#@Ethereal_Durability@#\v%d", nRecover / 10000 );

				LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
			}
			else
			{
				LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "FAIL", LOG::STR_NTS );
			}
		}

		// 보호 대상 아이템이 아닌 경우만 희생 아이템 삭제
		if( ( pMixInfo->value[ 2 ] & ( 1 << i ) ) == 0 )
			pPlayer->EraseItem( pSacrifice, nCount );
	}

	if( bSuccess )
	{
		SendItemMessage( pPlayer, pMainMaterial );
		AR_HANDLE tmp = pMainMaterial->GetHandle();
		SendMixResult( pPlayer, &tmp, 1 );
	}
	else
	{
		SendMixResult( pPlayer, NULL, 0 );
	}

	return bSuccess;
}

bool MixManager::SacrificeItemForEtherealStoneDurability( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	bool bSuccess = false;
	__int64 nRecover = 0;

	for( int i = 0 ; i < nSubMaterialCount ; ++i )
	{
		// 완충 상태면 충전 시도 중지(여러 개 충전 중간에 완충 상태가 될 수도 있으므로 매 루프마다 검사)
		if( pPlayer->GetEtherealStoneDurability() >= GameRule::MAX_ETHEREAL_STONE_DURABILITY )
			break;

		StructItem * pSacrifice = pSubMaterial[ i ];
		unsigned short nCount = pCountList[ i ];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pSacrifice->GetItemEnhance() * 100 + pSacrifice->GetItemLevel(), pSacrifice->GetItemCode(), nCount, pSacrifice->GetCount() - nCount, pMixInfo->id, 0, 0, 0, pSacrifice->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SACRIFICE_ITEM_FOR_ETHEREAL_STONE_DURABILITY", LOG::STR_NTS );

		// 성공률 체크
		
		if( (XRandom( 0, 99 ) < pMixInfo->value[ 3 ] ) || pPlayer->GetAlwaysPassMix())
		{
			bSuccess = true;

			__int64 nCurrentRecover = pSacrifice->GetItemBase().nPrice.GetRawData() * pMixInfo->value[ 0 ] / 100 * nCount;
			nCurrentRecover += pMixInfo->value[ 1 ];

			// 파괴된 보조 재료템은 25%만 적용
			if( pSacrifice->GetMaxEtherealDurability() && !pSacrifice->GetCurrentEtherealDurability() )
				nCurrentRecover /= 4;

			int nPrevEtherealStoneDurability = pPlayer->GetEtherealStoneDurability();

			pPlayer->AddEtherealStoneDurability( static_cast< int >( nCurrentRecover ) );
			nRecover += nCurrentRecover;	

			// 메인 아이템은 없고 보조 아이템은 Erase 로그가 남기 때문에, 성공 여부와 함께 플레이어의 정보를 남긴다.
			LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 1, pMixInfo->id, pPlayer->GetEtherealStoneDurability(), 0, 0, 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );

			LOG::Log11N4S( LM_CHARGE_ETHEREAL_DURABILITY, pPlayer->GetAccountID(), pPlayer->GetSID(), nPrevEtherealStoneDurability, pPlayer->GetEtherealStoneDurability(), 0, 0, 0, 0, 0, 0, pSacrifice->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );
		}
		else
		{
			LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 1, pMixInfo->id, pPlayer->GetEtherealStoneDurability(), 0, 0, 0, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "FAIL", LOG::STR_NTS );
		}


		// 보호 대상 아이템이 아닌 경우만 희생 아이템 삭제
		if( ( pMixInfo->value[ 2 ] & ( 1 << i ) ) == 0 )
			pPlayer->EraseItem( pSacrifice, nCount );
	}

	if( bSuccess )
	{
		PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@7900\v#@Ethereal_Durability@#\v%d", nRecover / 10000 );
	}
	else
	{
		SendMixResult( pPlayer, NULL, 0 );
	}

	return bSuccess;
}

bool MixManager::TransmitEtherealDurability( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	StructItem * pBattery = pSubMaterial[ 0 ];
	StructItem * pCatalyst = pSubMaterial[ 1 ];
	int nDedicatedCatalystCount = pCountList[ 1 ];

	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "TRANSMIT_ETHEREAL_DURABILITY", LOG::STR_NTS );
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pBattery->GetItemEnhance() * 100 + pBattery->GetItemLevel(), pBattery->GetItemCode(), 1, pBattery->GetCount(), pMixInfo->id, 0, 0, 0, pBattery->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "TRANSMIT_ETHEREAL_DURABILITY", LOG::STR_NTS );

	// 툴팁에서 보여지는 단위로 처리(실제 수치의 1/10000)
	int nUsedEtherealDurability = ( pMainMaterial->GetMaxEtherealDurability() - pMainMaterial->GetCurrentEtherealDurability() ) / 10000;

	// 회복될 장비 아이템의 내구도가 최소 복구 단위 이상 소진되어 있다는 조건은 조합 DB에서 걸려있지만 그냥 한 번 더 'ㅠ '
	// 회복될 장비가 아예 내구도가 바닥났으면 여기서 복구 안 해줌(RecoverExhaustedEtherealDurability에 의해 복구시켜야 함)
	if( !nUsedEtherealDurability || !pMainMaterial->GetCurrentEtherealDurability() )
	{
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "FAIL", LOG::STR_NTS );
		SendMixResult( pPlayer, NULL, 0 );
		return false;
	}

	// 충전율에 따라 최대 충전에 소요될 보조 재료 및 에테리얼 내구도 계산
	c_fixed10 fRequiredEtherealDurability( nUsedEtherealDurability );
	fRequiredEtherealDurability = ( fRequiredEtherealDurability - pMixInfo->value[ 1 ] ) * 100 / pMixInfo->value[ 0 ];

	// 실제 에테리얼 충전용 아이템에서 꺼낼 수 있는 에테리얼 양을 얻고 다시 해당 결과와 소모성 재료의 개수를 기준으로 비교(개수 = 회복량으로 사용됨)
	// * 충전용 아이템에 50000 의 내구도가 있을 경우 최대 회복 가능 량은 50000이 아닌 40000임(현재 충전 량보다 적은 양만 사용 가능)
	int nExtractAmount = std::min( static_cast< int >( fRequiredEtherealDurability ), ( pBattery->GetCurrentEtherealDurability() - 1 ) / 10000 );
	nExtractAmount = std::min( nExtractAmount, nDedicatedCatalystCount );

	// 소모될 촉매의 개수(=복구될 에테리얼 내구도) 체크
	if( !nExtractAmount || pCatalyst->GetCount() < nExtractAmount || ( pBattery->GetCurrentEtherealDurability() / 10000 ) < nExtractAmount )
	{
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "FAIL", LOG::STR_NTS );
		SendMixResult( pPlayer, NULL, 0 );
		return false;
	}

	// 실제 촉매 소모량을 결정한 이후에 촉매에 대한 조합 시도 로그 생성
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pCatalyst->GetItemEnhance() * 100 + pCatalyst->GetItemLevel(), pCatalyst->GetItemCode(), nExtractAmount, pCatalyst->GetCount() - nExtractAmount, pMixInfo->id, 0, 0, 0, pCatalyst->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "TRANSMIT_ETHEREAL_DURABILITY", LOG::STR_NTS );

	pBattery->AddCurrentEtherealDurability( -1 * nExtractAmount * 10000 );
	SendItemMessage( pPlayer, pBattery );
	pMainMaterial->AddCurrentEtherealDurability( nExtractAmount * 10000 * pMixInfo->value[ 0 ] / 100 + pMixInfo->value[ 1 ] );
	SendItemMessage( pPlayer, pMainMaterial );
	PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@7901\v#@Ethereal_Durability@#\v%d", ( nExtractAmount * 10000 * pMixInfo->value[ 0 ] / 100 + pMixInfo->value[ 1 ] ) / 10000 );

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );
	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pBattery->GetItemEnhance() * 100 + pBattery->GetItemLevel(), pBattery->GetItemCode(), 1, pMixInfo->id, pBattery->GetCurrentEtherealDurability(), 0, 0, 0, pBattery->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );

	// 보호 대상 아이템이 아닌 경우만 희생 아이템 삭제(배터리가 삭제되는 수도 있으므로 모든 충전 처리 이후에 삭제)
	for( int i = 0 ; i < nSubMaterialCount ; ++i )
	{
		if( ( pMixInfo->value[ 2 ] & ( 1 << i ) ) == 0 )
			pPlayer->EraseItem( pSubMaterial[ i ], ( pSubMaterial[ i ] == pCatalyst ) ? nExtractAmount : pCountList[ i ] );
	}

	AR_HANDLE hResults[] = { pBattery->GetHandle(), pMainMaterial->GetHandle() };
	SendMixResult( pPlayer, hResults, _countof( hResults ) );
	return true;
}

bool MixManager::TransmitEtherealDurabilityFromEtherealStone( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "TRANSMIT_ETHEREAL_DURABILITY_FROM_ETHEREAL_STONE", LOG::STR_NTS );
	
	// 툴팁에서 보여지는 단위로 처리(실제 수치의 1/10000)
	int nUsedEtherealDurability = ( pMainMaterial->GetMaxEtherealDurability() - pMainMaterial->GetCurrentEtherealDurability() ) / 10000;

	// 회복될 장비 아이템의 내구도가 최소 복구 단위 이상 소진되어 있다는 조건은 조합 DB에서 걸려있지만 그냥 한 번 더 'ㅠ '
	// 회복될 장비가 아예 내구도가 바닥났으면 여기서 복구 안 해줌(RecoverExhaustedEtherealDurability에 의해 복구시켜야 함)
	if( !nUsedEtherealDurability || !pMainMaterial->GetCurrentEtherealDurability() )
	{
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "FAIL", LOG::STR_NTS );
		SendMixResult( pPlayer, NULL, 0 );
		return false;
	}

	// 충전율에 따라 최대 충전에 소요될 보조 재료 및 에테리얼 내구도 계산
	c_fixed10 fRequiredEtherealDurability( nUsedEtherealDurability );
	fRequiredEtherealDurability = ( fRequiredEtherealDurability - pMixInfo->value[ 1 ] ) * 100 / pMixInfo->value[ 0 ];

	// 에테리얼 스톤에서 꺼낼 수 있는 에테리얼 양을 얻는다.
	//*에테리얼 스톤에 50000 의 내구도가 있을 경우 최대 회복 가능 량은 50000이 아닌 40000임(현재 충전 량보다 적은 양만 사용 가능)
	int nExtractAmount = std::min( static_cast< int >( fRequiredEtherealDurability ), ( pPlayer->GetEtherealStoneDurability() - 1 ) / 10000 );

	// 복구될 에테리얼 내구도 체크
	if( !nExtractAmount || ( pPlayer->GetEtherealStoneDurability() / 10000 ) < nExtractAmount )
	{
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "FAIL", LOG::STR_NTS );
		SendMixResult( pPlayer, NULL, 0 );
		return false;
	}

	int nPrevEtherealStoneDurability = pPlayer->GetEtherealStoneDurability();
	int nPrevItemEtherealDurability = pMainMaterial->GetCurrentEtherealDurability();

	pPlayer->AddEtherealStoneDurability( -1 * nExtractAmount * 10000 );
	pMainMaterial->AddCurrentEtherealDurability( nExtractAmount * 10000 * pMixInfo->value[ 0 ] / 100 + pMixInfo->value[ 1 ] );
	SendItemMessage( pPlayer, pMainMaterial );
	PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@7901\v#@Ethereal_Durability@#\v%d", ( nExtractAmount * 10000 * pMixInfo->value[ 0 ] / 100 + pMixInfo->value[ 1 ] ) / 10000 );

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );

	LOG::Log11N4S( LM_TRANSMIT_ETHEREAL_DURABILITY, pPlayer->GetAccountID(), pPlayer->GetSID(), nPrevEtherealStoneDurability, pPlayer->GetEtherealStoneDurability(), nPrevItemEtherealDurability, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

	// 보호 대상 아이템이 아닌 경우만 희생 아이템 삭제(배터리가 삭제되는 수도 있으므로 모든 충전 처리 이후에 삭제)
	for( int i = 0 ; i < nSubMaterialCount ; ++i )
	{
		if( ( pMixInfo->value[ 2 ] & ( 1 << i ) ) == 0 )
			pPlayer->EraseItem( pSubMaterial[ i ], pCountList[ i ] );
	}

	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &tmp, 1 );
	return true;
}

bool MixManager::RecoverExhaustedEtherealDurability( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "TRANSMIT_ETHEREAL_DURABILITY", LOG::STR_NTS );

	for( int i = 0 ; i < nSubMaterialCount ; ++i )
	{
		StructItem * pItem = pSubMaterial[ i ];
		unsigned short nCount = pCountList[ i ];

		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, 0, 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "TRANSMIT_ETHEREAL_DURABILITY", LOG::STR_NTS );

		// 보호 대상 아이템이 아닌 경우만 재료 아이템 삭제
		if( ( pMixInfo->value[ 1 ] & ( 1 << i ) ) == 0 )
			pPlayer->EraseItem( pItem, nCount );
	}

	// 성공률 체크
	
	bool bSuccess = (XRandom( 0, 99 ) < pMixInfo->value[ 2 ]) || pPlayer->GetAlwaysPassMix();
	const char * pszResult = ( bSuccess ) ? "SUCS" : "FAIL";
	if( bSuccess )
	{
		__int64 nRecoverEtherealDurability = 0;

		// 최대치 기준 % 회복량: ( pMixInfo->value[ 2 ] )%
		assert( pMixInfo->value[ 2 ] >= 0 && pMixInfo->value[ 2 ] <= 100 );
		nRecoverEtherealDurability = pMainMaterial->GetMaxEtherealDurability();
		nRecoverEtherealDurability = nRecoverEtherealDurability * pMixInfo->value[ 2 ] / 100;

		// 절대치 회복량: pMixInfo->value[ 0 ]
		nRecoverEtherealDurability += pMixInfo->value[ 0 ];

		// 오버플로우 체크
		if( nRecoverEtherealDurability > INT_MAX )
		{
			assert( 0 );
			nRecoverEtherealDurability = INT_MAX;
		}

		pMainMaterial->AddCurrentEtherealDurability( nRecoverEtherealDurability );

		SendItemMessage( pPlayer, pMainMaterial );
		PrintfChatMessage( false, CHAT_ITEM, "@SYSTEM", pPlayer, "@7901\v#@Ethereal_Durability@#\v%d", nRecoverEtherealDurability / 10000 );

		AR_HANDLE tmp = pMainMaterial->GetHandle();
		SendMixResult( pPlayer, &tmp, 1 );
	}
	else
		SendMixResult( pPlayer, NULL, 0 );

	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, pszResult, LOG::STR_NTS );

	return bSuccess;
}

bool MixManager::CreateItemByDecompose( struct StructPlayer * pPlayer, DecomposeBase & decomposeInfo, std::vector< std::pair< struct StructItem*, __int64 > > *vCreatedItem, int nCount )
{
	// 분해 유형: 101- DECOMPOSE_CREATE_ITEM  
	// value[0]: 성공률, value[1]: 생성아이템ID, value[2]: 최소값, value[3]: 최대값 
	int nProbability = decomposeInfo.value[0] * 100 ;
	
	for( int i = 0 ; i < nCount ; ++i )
	{
		if( (XRandom( 1, 10000 ) <= nProbability ) || pPlayer->GetAlwaysPassMix())
		{
			int forCnt = 1;
			int dropCnt = XRandom( decomposeInfo.value[2], decomposeInfo.value[3] );

			// 드랍 그룹에서 선택 되면 forLoop를 itemCnt만큼 돈다.
			// DecomposeInfo 내의 최소/최대 개수와 드랍Group내의 최소/최대 개수가 따로 존재한다는 데에 주의하자.
			ItemBase::ItemCode dropID = decomposeInfo.value[1];
			if( dropID < 0 )
			{
				forCnt = dropCnt;
			}

			for( int i = 0; i < forCnt; ++i )
			{
				ItemBase::ItemCode itemCode = dropID;
				__int64 itemCnt = dropCnt;

				while( itemCode < 0 ) // 드랍 그룹일 경우 아이템 코드 선택
				{
					GameContent::SelectItemIDFromDropGroup( itemCode, itemCode, itemCnt );
				}

				StructItem* pNewItem = StructItem::AllocItem( 0, itemCode, itemCnt,  ItemInstance::BY_DECOMPOSE );
				itemCnt = pNewItem->GetCount();
				if( !pNewItem->IsJoinable() || !pPlayer->GetInventory()->Find( itemCode ) )
					pNewItem = pPlayer->PushItem( pNewItem, itemCnt );
				else
					pNewItem = pPlayer->PushItem( pNewItem, itemCnt, true );

				if( pNewItem )
				{
					LOG::Log11N4S( LM_ITEM_TAKE, pPlayer->GetAccountID(), pPlayer->GetSID(), pNewItem->GetItemEnhance() * 100 + pNewItem->GetItemLevel(), pNewItem->GetItemCode(), dropCnt, pNewItem->GetCount(), pPlayer->GetGold().GetRawData(), pPlayer->GetGold().GetRawData(), pPlayer->GetX(), pPlayer->GetY(), pNewItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "DECOMPOSE", LOG::STR_NTS );
					LOG::Log11N4S( LM_ITEM_DECOMPOSE_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pNewItem->GetItemEnhance() * 100 + pNewItem->GetItemLevel(), itemCode, dropCnt, pNewItem->GetCount(), 0, 0, 0, decomposeInfo.id, pNewItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "SUCS", LOG::STR_NTS, "DECOMPOSE_CREATE_ITEM", LOG::STR_NTS );

					bool bCheck = false;
					for( std::vector< std::pair< struct StructItem*, __int64 > >::iterator it = vCreatedItem->begin() ; it != vCreatedItem->end() ; ++it )
					{
						if( it->first == pNewItem )
						{
							it->second += itemCnt;
							bCheck = true;
							break;
						}
					}
					if( !bCheck )
						vCreatedItem->push_back( std::make_pair( pNewItem, itemCnt ) );
				}
				else
				{
					_cprint( "Pushing item failed in decomposing: itemCode(%d), itemCnt(%d), player(%s)\n", itemCode, itemCnt, pPlayer->GetName() );
					FILELOG( "Pushing item failed in decomposing: itemCode(%d), itemCnt(%d), player(%s)", itemCode, itemCnt, pPlayer->GetName() );
				}
			}
		}
		else
		{
			LOG::Log11N4S( LM_ITEM_DECOMPOSE_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), 0, 0, 0, 0, 0, 0, 0, decomposeInfo.id, 0, pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "FAIL", LOG::STR_NTS, "DECOMPOSE_CREATE_ITEM", LOG::STR_NTS );
		}
	}
	
	return true;
}


bool MixManager::InsertAwakenItem( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	static int anLuciasMask1[] = { 900754,900754, 900782,900782, 900804,900804 };
	static int anLuciasMask2[] = {	900765, 900766, 900767, 900768, 900769, 900770, 900771, 900772, 900773, 900774, 900775,
									900765, 900766, 900767, 900768, 900769, 900770, 900771, 900772, 900773, 900774, 900775,
									900765, 900766, 900767, 900768, 900769, 900770, 900771, 900772, 900773, 900774, 900775,
									900793, 900794, 900795, 900796, 900797, 900798, 900799, 900800, 900801, 900802, 900803,
									900793, 900794, 900795, 900796, 900797, 900798, 900799, 900800, 900801, 900802, 900803,
									900711, 900712, 900713, 900714, 900715, 900716, 900717, 900718, 900719, 900776, 900777 };

	
	if( pMainMaterial->IsAwaken() )
		return false;

	int arrRate[ 40 ] = {0, };
	RandomManager::GetAwakenProbabilitiesInfo( arrRate, _countof( arrRate ) );

	int nMinAwakenCount = pMixInfo->value[ 0 ];
	int nMaxAwakenCount = pMixInfo->value[ 1 ];

	ItemInstance::RANDOM_OPTION kAwakenOption;
	kAwakenOption.nRandomType = ItemInstance::AWAKEN;

	int nType = 0;
	int nBitSet = 0;
	int nAwakenData = 0;
	int nAwakenCount = XRandom( nMinAwakenCount, nMaxAwakenCount );

	if( nAwakenCount > ItemInstance::MAX_AWAKEN_NUMBER )
		nAwakenCount = ItemInstance::MAX_AWAKEN_NUMBER;

	int nGroup = pMainMaterial->GetItemGroup();

	int nEnhance = pMainMaterial->GetItemEnhance() - 19;

	if ((nGroup == ItemBase::GROUP_ARMOR || nGroup == ItemBase::GROUP_HELM || nGroup == ItemBase::GROUP_GLOVE 
		|| nGroup == ItemBase::GROUP_BOOTS || nGroup == ItemBase::GROUP_BELT || nGroup == ItemBase::GROUP_FACE) && nEnhance > 0)
	{
		//int nEnhance = pMainMaterial->GetItemEnhance() - 19;

		if (nEnhance < 1)
			return false;

		nMaxAwakenCount = 2;
		int nEnhanceDiv2 = (pMainMaterial->GetItemEnhance() - 18) / 2;
		if (nGroup == ItemBase::GROUP_HELM) {
			kAwakenOption.OptionInfo[0].nType = ITEM_EFFECT_PASSIVE::APPLY_EFFECT;
			kAwakenOption.OptionInfo[0].fValue1 = nEnhanceDiv2 + 1007;
			kAwakenOption.OptionInfo[0].fValue2 = nEnhanceDiv2;
			kAwakenOption.OptionInfo[1].nType = ITEM_EFFECT_PASSIVE::INC_COOLTIME_SPEED;
			kAwakenOption.OptionInfo[1].fValue1 = 10000;
			kAwakenOption.OptionInfo[1].fValue2 = nEnhance * 5;
		}
		else if (nGroup == ItemBase::GROUP_GLOVE) {
			kAwakenOption.OptionInfo[0].nType = ITEM_EFFECT_PASSIVE::INC_CASTING_SPEED;
			kAwakenOption.OptionInfo[0].fValue1 = 10001;
			kAwakenOption.OptionInfo[0].fValue2 = (nEnhanceDiv2 * 5) + 20;;
			kAwakenOption.OptionInfo[1].nType = ITEM_EFFECT_PASSIVE::INC_DOUBLE_ATTACK;
			kAwakenOption.OptionInfo[1].fValue1 = 10002;
			kAwakenOption.OptionInfo[1].fValue2 = nEnhance * 5;
		}
		else if (nGroup == ItemBase::GROUP_BOOTS) {
			kAwakenOption.OptionInfo[0].nType = ITEM_EFFECT_PASSIVE::RESIST_NEGATIVE;
			kAwakenOption.OptionInfo[0].fValue1 = 10003;
			kAwakenOption.OptionInfo[0].fValue2 = ((nEnhanceDiv2 * 5) + 10) * 2;
			kAwakenOption.OptionInfo[1].nType = ITEM_EFFECT_PASSIVE::INC_SUMMON_FCM;
			kAwakenOption.OptionInfo[1].fValue1 = 10004;
			kAwakenOption.OptionInfo[1].fValue2 = nEnhance * 10;
		}
		else if (nGroup == ItemBase::GROUP_BELT) {
			nMaxAwakenCount = 1;
			kAwakenOption.OptionInfo[0].nType = ITEM_EFFECT_PASSIVE::INC_BELTSLOT_AMP;
			kAwakenOption.OptionInfo[0].fValue1 = 10005;
			kAwakenOption.OptionInfo[0].fValue2 = nEnhance * 5 + 5;
		}
		else if (nGroup == ItemBase::GROUP_FACE) {
			int race = XRandom(0, 11);
			nMaxAwakenCount = nEnhance > 1 ? 2 : 1;
			kAwakenOption.OptionInfo[0].nType = ITEM_EFFECT_PASSIVE::APPLY_EFFECT;
			kAwakenOption.OptionInfo[0].fValue1 = anLuciasMask1[nEnhance - 1]+race;
			kAwakenOption.OptionInfo[0].fValue2 = 1;// nEnhanceDiv2;
			if (nEnhance > 1)
			{
				kAwakenOption.OptionInfo[1].nType = ITEM_EFFECT_PASSIVE::APPLY_EFFECT;
				kAwakenOption.OptionInfo[1].fValue1 = anLuciasMask2[((nEnhance-1) * 11) + race ];
				kAwakenOption.OptionInfo[1].fValue2 = 1;// nEnhance;
			}
		}
		else {
			if (nGroup == ItemBase::GROUP_ARMOR) {
				/* GROUP_ARMOR */
				nMaxAwakenCount = 1;
				kAwakenOption.OptionInfo[0].nType = ITEM_EFFECT_PASSIVE::INC_PARAMETER_A;
				kAwakenOption.OptionInfo[0].fValue1 = 0x600000;
				kAwakenOption.OptionInfo[0].fValue2 = (pMainMaterial->GetItemEnhance() - 18) * 500;
			}
		}
	}
	else
	{
		if (((nGroup != 1) && (nGroup != 3)) && ((nGroup != 9 && ((nGroup != 19 && (nGroup != 25))))))
			return false;

		for (int nOptionCnt = 0; nOptionCnt < nAwakenCount; )
		{
			int nKey = XRandom(1, 100000);
			bool bCheckResult = true;

			for (int nIndex = 0; nIndex < 40; ++nIndex)
			{
				if (nKey > arrRate[nIndex])	continue;

				int nMaxTypeCount = RandomManager::GetMaxAwakenCount();
				int nOptionType = XRandom(1, nMaxTypeCount);
				nAwakenData = RandomManager::GetAwakenInfoValue(nIndex, nOptionType, nBitSet, nType);
				if (!nAwakenData)	return false;

				if (!RandomManager::CheckMaxAwakenOption(&kAwakenOption, nBitSet, nType, nAwakenData))
				{
					bCheckResult = false;
				}
				break;
			}

			if (bCheckResult)
			{
				kAwakenOption.OptionInfo[nOptionCnt].nType = nType;
				kAwakenOption.OptionInfo[nOptionCnt].fValue1 = nBitSet;
				kAwakenOption.OptionInfo[nOptionCnt].fValue2 = nAwakenData;

				++nOptionCnt;
			}
		}
	}


	// MainMaterial
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "AWAKEN_ITEM", LOG::STR_NTS );

	kAwakenOption.nSID = RandomManager::AllocRandomSID();
	if( !pMainMaterial->SetAwakenOption( kAwakenOption ) )	return false;

	for( int nCnt = 0 ; nCnt < nSubMaterialCount ; ++nCnt )
	{
		// SubMaterial
		struct StructItem *pItem = pSubMaterial[ nCnt ];
		unsigned short nCount = pCountList[ nCnt ];
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, 0, 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "AWAKEN_ITEM", LOG::STR_NTS );

		pPlayer->EraseItem( pSubMaterial[ nCnt ], pCountList[ nCnt ] );
	}

	pMainMaterial->DBQuery( new DB_UpdateItemAwakenSID( pMainMaterial ) );
	pMainMaterial->DBQuery( new DB_SetRandomOption( pPlayer, pMainMaterial, kAwakenOption ) );

	SendItemMessage( pPlayer, pMainMaterial );

	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &tmp, 1, TS_SC_MIX_RESULT::MIX_TYPE_AWAKEN );

	// Mix Result
	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "", LOG::STR_NTS );

	// Awaken Result
	char szAwakenValue[128] = { 0, };
	char szAwakenData[128] = { 0, };
	s_itoa( pMainMaterial->GetAwakenOptionValue1( 4 ), szAwakenValue, _countof( szAwakenValue ), 10 );
	s_itoa( pMainMaterial->GetAwakenOptionValue2( 4 ), szAwakenData, _countof( szAwakenData ), 10 );
	LOG::Log11N4S( LM_INSERT_AWAKEN_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetAwakenOptionValue1(0), pMainMaterial->GetAwakenOptionValue2(0), pMainMaterial->GetAwakenOptionValue1(1), pMainMaterial->GetAwakenOptionValue2(1), pMainMaterial->GetAwakenOptionValue1(2), pMainMaterial->GetAwakenOptionValue2(2), pMainMaterial->GetAwakenOptionValue1(3), pMainMaterial->GetAwakenOptionValue2(3), pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, szAwakenValue, LOG::STR_NTS, szAwakenData, LOG::STR_NTS );

	return true;
}


bool MixManager::DeleteAwakenOption( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	if( !pMainMaterial->IsAwaken() )
		return false;

	// MainMaterial
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "UNAWAKEN_ITEM", LOG::STR_NTS );

	int nAwakenSID = pMainMaterial->GetAwakenSID();

	ItemInstance::RANDOM_OPTION kDummyOption;
	kDummyOption.nRandomType = ItemInstance::AWAKEN;
	pMainMaterial->SetRandomOption( ItemInstance::AWAKEN, kDummyOption );

	pMainMaterial->DBQuery( new DB_UpdateItemAwakenSID( pMainMaterial ) );
	pMainMaterial->DBQuery( new DB_DeleteRandomOption( pMainMaterial, nAwakenSID ) );

	for( int nCnt = 0 ; nCnt < nSubMaterialCount ; ++nCnt )
	{
		// SubMaterial
		struct StructItem *pItem = pSubMaterial[ nCnt ];
		unsigned short nCount = pCountList[ nCnt ];
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pItem->GetItemEnhance() * 100 + pItem->GetItemLevel(), pItem->GetItemCode(), nCount, pItem->GetCount() - nCount, pMixInfo->id, 0, 0, 0, pItem->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "UNAWAKEN_ITEM", LOG::STR_NTS );

		pPlayer->EraseItem( pSubMaterial[nCnt], pCountList[nCnt] );
	}

	SendItemMessage( pPlayer, pMainMaterial );

	AR_HANDLE tmp = pMainMaterial->GetHandle();
	SendMixResult( pPlayer, &tmp, 1, 1 );

	// Mix Result
	LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "SUCS", LOG::STR_NTS );

	// Awaken Result
	char szAwakenValue[128] = { 0, };
	char szAwakenData[128] = { 0, };
	s_itoa( pMainMaterial->GetAwakenOptionValue1( 4 ), szAwakenValue, _countof( szAwakenValue ), 10 );
	s_itoa( pMainMaterial->GetAwakenOptionValue2( 4 ), szAwakenData, _countof( szAwakenData ), 10 );
	LOG::Log11N4S( LM_DELETE_AWAKEN_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetAwakenOptionValue1(0), pMainMaterial->GetAwakenOptionValue2(0), pMainMaterial->GetAwakenOptionValue1(1), pMainMaterial->GetAwakenOptionValue2(1), pMainMaterial->GetAwakenOptionValue1(2), pMainMaterial->GetAwakenOptionValue2(2), pMainMaterial->GetAwakenOptionValue1(3), pMainMaterial->GetAwakenOptionValue2(3), pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, szAwakenValue, LOG::STR_NTS, szAwakenData, LOG::STR_NTS );

	return true;
}

bool MixManager::IdentifyItemForRandomOption( const MixBase * pMixInfo, struct StructPlayer * pPlayer, struct StructItem * pMainMaterial, int nSubMaterialCount, struct StructItem ** pSubMaterial, unsigned short *pCountList, std::vector< std::pair< StructItem *, __int64 > > * vCreatedItem )
{
	// 랜덤화 가능여부 조사
	if( !pPlayer->IsRandomizable( pMainMaterial ) )
		return false;

	// 로그 남긴다.
	LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "IDENTIFY_ITEM", LOG::STR_NTS );
	for( int i = 0; i < nSubMaterialCount; ++i )
	{
		LOG::Log11N4S( LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pSubMaterial[i]->GetItemEnhance() * 100 + pSubMaterial[i]->GetItemLevel(), pSubMaterial[i]->GetItemCode(), pCountList[i], pSubMaterial[i]->GetCount() - pCountList[i], pMixInfo->id, pSubMaterial[i]->GetCurrentEtherealDurability(), 0, 0, pSubMaterial[i]->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "IDENTIFY_ITEM", LOG::STR_NTS );
		pPlayer->EraseItem( pSubMaterial[i], pCountList[i] );
	}

	// value[0] = 랜덤화 성공률, value[1] = 프리셋ID
	
	int sucRate = pMixInfo->value[ 0 ] * 10000;
	if( (XRandom( 0, 1000000 ) <= sucRate ) || pPlayer->GetAlwaysPassMix())
	{
		int presetID = pMixInfo->value[ 1 ];
		ItemInstance::RANDOM_OPTION kOption;
		kOption.nSID = RandomManager::AllocRandomSID();
		kOption.nRandomType = ItemInstance::IDENTIFIED;
		if( !RandomManager::PickRandomOptionByPreset( &kOption, presetID ) )
			return false;
		pMainMaterial->SetIdentifiedOption( kOption );
		int nLogCnt = 0;
		for( ; nLogCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nLogCnt )
		{
			if( !kOption.OptionInfo[ nLogCnt ].nType )
			{
				break;
			}
		}

		// 랜덤 옵션 두개마다 로그 하나씩 기록. (실제 옵션 값은 소수점도 기록하기 위해 한자리 올려서 기록한다. 예: 15.6->156 )
		for( int i = 0 ; i < nLogCnt ; i+=2 )
		{
			LOG::Log11N4S( LM_INSERT_RANDOM_OPTION, pPlayer->GetAccountID(), pPlayer->GetSID(), kOption.OptionInfo[ i ].nType, kOption.OptionInfo[ i ].fValue1, kOption.OptionInfo[ i ].fValue2.get()/1000, kOption.OptionInfo[ i + 1 ].nType, kOption.OptionInfo[ i + 1 ].fValue1, kOption.OptionInfo[ i + 1 ].fValue2.get()/1000, pMainMaterial->GetItemCode(), pMixInfo->value[ 1 ], pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", LOG::STR_NTS, "ByMix", LOG::STR_NTS );
		}

		// DB업뎃
		pMainMaterial->DBQuery( new DB_UpdateItemRandomOptionSID( pMainMaterial ) );
		pMainMaterial->DBQuery( new DB_SetRandomOption( pPlayer, pMainMaterial, kOption ) );

		AR_HANDLE handle = pMainMaterial->GetHandle();
		SendItemMessage( pPlayer, pMainMaterial );
		SendMixResult( pPlayer, &handle, 1, TS_SC_MIX_RESULT::MIX_TYPE_RANDOM_OPTION );
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "SUCS", 0, "", LOG::STR_NTS );
	}
	else
	{
		//실패처리
		SendMixResult( pPlayer, NULL, 0, TS_SC_MIX_RESULT::MIX_TYPE_RANDOM_OPTION );
		LOG::Log11N4S( LM_ITEM_MIX_RESULT, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "FAIL", 0, "", LOG::STR_NTS );
	}

	return true;
}

bool MixManager::DeleteRandomOption(const MixBase* pMixInfo, struct StructPlayer* pPlayer, struct StructItem* pMainMaterial, int nSubMaterialCount, struct StructItem** pSubMaterial, unsigned short* pCountList, std::vector< std::pair< StructItem*, __int64 > >* vCreatedItem)
{

	// 랜덤화 가능여부 조사
	if (!pMainMaterial->IsRandomizable())
		return false;

	// MainMaterial
	LOG::Log11N4S(LM_ITEM_MIX_TRY, pPlayer->GetAccountID(), pPlayer->GetSID(), pMainMaterial->GetItemEnhance() * 100 + pMainMaterial->GetItemLevel(), pMainMaterial->GetItemCode(), 1, pMainMaterial->GetCount(), pMixInfo->id, pMainMaterial->GetCurrentEtherealDurability(), 0, 0, pMainMaterial->GetItemUID(), pPlayer->GetAccountName(), LOG::STR_NTS, pPlayer->GetName(), LOG::STR_NTS, "", 0, "UNRANDOMIZE_ITEM", LOG::STR_NTS);

	int nRandomSID = pMainMaterial->GetRandomSID(ItemInstance::IDENTIFIED);

	ItemInstance::RANDOM_OPTION kDummyOption;
	kDummyOption.nRandomType = ItemInstance::IDENTIFIED;
	pMainMaterial->SetRandomOption(ItemInstance::IDENTIFIED, kDummyOption);

//	pMainMaterial->DBQuery(new DB_UpdateItemRandomOptionSID(pMainMaterial));
	pMainMaterial->DBQuery(new DB_DeleteRandomOption(pMainMaterial, nRandomSID));

	return true;
}
