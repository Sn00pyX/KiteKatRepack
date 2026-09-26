
#include <atlcomtime.h>
#include <ctime>

#include <toolkit/XConsole.h>


#include "DB_Commands.h"
#include "StructItem.h"

bool DB_UpdateItem::update_item_count( DBConnection & db, ItemUID uid, const __int64 & count )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateItem : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_item_count" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_item_count";
	s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_SID: %d, @IN_COUNT: %d", uid, count );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adBigInt, adParamInput, 8, uid ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_COUNT", adBigInt, adParamInput, 8, count ) );

	cmd->Execute( NULL, NULL, adCmdStoredProc );

	return true;
}

bool DB_UpdateItem::update_item( DBConnection & db, struct StructItem * pItem )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_UpdateItem : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_update_item" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_update_item";

	pItem->TurnOffDbUpdateFlag();

	s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_SID: %d", pItem->GetItemUID() );

	time_t remain_time = 0;

	if( pItem->IsExpireItem() )
	{
		if( pItem->GetItemBase().decrease_type == ItemBase::DECREASE_ALWAYS )
		{
			remain_time = pItem->GetExpireTime();
		}
		else
		{
			remain_time = pItem->GetExpireTime() - time( NULL );
		}
	}

	// 테이밍 중인 소환수 카드의 테이밍 중 플래그 제거 처리(DB에 저장할 필요 없는 플래그)
	ItemInstance::InstanceFlag flagItemInstance;
	flagItemInstance.CopyFrom( pItem->GetInstanceFlag().GetRawData() );
	flagItemInstance.Off( ItemInstance::ITEM_FLAG_TAMING );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adBigInt, adParamInput, 8, pItem->GetItemUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_ID", adInteger, adParamInput, 4, pItem->GetOwnSummonUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PREVIOUS_SID", adBigInt, adParamInput, 8, pItem->GetPreviousUID() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_FLAG", adInteger, adParamInput, 4, *flagItemInstance.GetRawData() ));
	cmd->Parameters->Append( cmd->CreateParameter( "IN_COUNT", adBigInt, adParamInput, 8, pItem->GetCount() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LEVEL", adInteger, adParamInput, 4, pItem->GetItemLevel() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ENHANCE", adInteger, adParamInput, 4, pItem->GetItemEnhance() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ETHEREAL_DURABILITY", adInteger, adParamInput, 4, pItem->GetCurrentEtherealDurability() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ENDURANCE", adInteger, adParamInput, 4, pItem->GetCurrentEndurance() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_WEAR_INFO", adInteger, adParamInput, 4, pItem->GetWearInfo() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SOCKET_0", adInteger, adParamInput, 4, pItem->GetSocketCode( 0 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SOCKET_1", adInteger, adParamInput, 4, pItem->GetSocketCode( 1 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SOCKET_2", adInteger, adParamInput, 4, pItem->GetSocketCode( 2 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SOCKET_3", adInteger, adParamInput, 4, pItem->GetSocketCode( 3 ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, remain_time ) );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_TYPE", adTinyInt, adParamInput, 1, pItem->GetElementalEffectType() ) );
	COleDateTime dtElementalEffectExpire( pItem->GetElementalEffectExpireTime() );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_EXPIRE_TIME", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtElementalEffectExpire ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_ATTACK_POINT", adInteger, adParamInput, 4, pItem->GetElementalEffectAttackPoint() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_MAGIC_POINT", adInteger, adParamInput, 4, pItem->GetElementalEffectMagicPoint() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_APPEARANCE_CODE", adInteger, adParamInput, 4, pItem->GetAppearanceCode() ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_CODE", adInteger, adParamInput, 4, pItem->GetSummonCode() ) );
	cmd->Parameters->Append(cmd->CreateParameter("IN_EFFECT_ID", adInteger, adParamInput, 4, pItem->GetEffectID()));

	cmd->Execute( NULL, NULL, adCmdStoredProc );

	return true;
}

bool DB_UpdateItem::proc( DBConnection & db )
{	
	if( m_bCountOnly )
		return DB_UpdateItem::update_item_count( db, m_pItem->GetItemUID(), m_pItem->GetCount() );
	else
		return DB_UpdateItem::update_item( db, m_pItem );
}

bool DB_UpdateItem::onProcess( DBConnection & db )
{
	try
	{
		if( !m_pItem->IsVirtualItem() )
			proc( db );
	}
	catch( ... )
	{
		m_pItem->onEndQuery();
		throw;
	}
	m_pItem->onEndQuery();

	return true;
}
