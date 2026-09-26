
#include <ctime>
#include <atlcomtime.h>

#include <toolkit/XConsole.h>

#include "DB_Commands.h"
#include "StructItem.h"
#include "VersionDefs.h"



DB_InsertItem::DB_InsertItem( struct StructItem * pItem ) : DBProc( "DB_InsertItem" ), m_pItem( pItem )
{	
}

bool DB_InsertItem::insertItemToDB( DBConnection & db , ItemUID uid, int owner_id, int account_id, int summon_id, int auction_id, int keeping_id, int code, ItemUID previous_uid, int flag, const __int64 & cnt, int level, int enhance, int ethereal_durability, int endurance, int socket_0, int socket_1, int socket_2, int socket_3, int gcode, int remain_time, unsigned char elemental_effect_type, time_t elemental_effect_expire_time, int elemental_effect_attack_point, int elemental_effect_magic_point, const ItemBase::ItemCode appearance_code, int summon_code, int effect_id )
{

	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_InsertItem : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_insert_item" );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_SID", adBigInt, adParamInput, 8, uid ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_ID", adInteger, adParamInput, 4, owner_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, account_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_ID", adInteger, adParamInput, 4, summon_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_AUCTION_ID", adInteger, adParamInput, 4, auction_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_KEEPING_ID", adInteger, adParamInput, 4, keeping_id ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CODE", adInteger, adParamInput, 4, code ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_PREVIOUS_SID", adBigInt, adParamInput, 8, previous_uid ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_FLAG", adInteger, adParamInput, 4, flag ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_CNT", adBigInt, adParamInput, 8, cnt ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_LEVEL", adInteger, adParamInput, 4, level ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ENHANCE", adInteger, adParamInput, 4, enhance ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ETHEREAL_DURABILITY", adInteger, adParamInput, 4, ethereal_durability ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ENDURANCE", adInteger, adParamInput, 4, endurance ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SOCKET_0", adInteger, adParamInput, 4, socket_0 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SOCKET_1", adInteger, adParamInput, 4, socket_1 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SOCKET_2", adInteger, adParamInput, 4, socket_2 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SOCKET_3", adInteger, adParamInput, 4, socket_3 ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_GCODE", adInteger, adParamInput, 4, gcode ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_REMAIN_TIME", adInteger, adParamInput, 4, remain_time ) );

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_TYPE", adInteger, adParamInput, 4, elemental_effect_type ) );
	COleDateTime dtElementalEffectExpire( elemental_effect_expire_time );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_EXPIRE_DATE", adDate, adParamInput, sizeof( DATE ), static_cast< DATE >( dtElementalEffectExpire ) ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_ATTACK_POINT", adInteger, adParamInput, 4, elemental_effect_attack_point ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ELEMENTAL_EFFECT_MAGIC_POINT", adInteger, adParamInput, 4, elemental_effect_magic_point ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_APPEARANCE_CODE", adInteger, adParamInput, 4, appearance_code ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_CODE", adInteger, adParamInput, 4, summon_code ) );

#if defined USE_ITEMEFFECT_96 || defined USE_CLIENT97
	cmd->Parameters->Append(cmd->CreateParameter("IN_EFFECT_ID", adInteger, adParamInput, 4, effect_id));
#else
	cmd->Parameters->Append(cmd->CreateParameter("IN_EFFECT_ID", adInteger, adParamInput, 4, 0));
#endif
	cmd->Execute( NULL, NULL,adCmdStoredProc );

	return true;
}

bool DB_InsertItem::proc( DBConnection & db )
{
	time_t remain_time = 0;

	if( m_pItem->IsExpireItem() )
	{
		if( m_pItem->GetItemBase().decrease_type == ItemBase::DECREASE_ALWAYS )
		{
			remain_time = m_pItem->GetExpireTime();
		}
		else
		{
			remain_time = m_pItem->GetExpireTime() - time( NULL );
		}
	}

	// 어쨋든 Insert 하면 지금 당장 DB 업데이트분까지 반영이 되었으므로 플래그 Off
	m_pItem->TurnOffDbUpdateFlag();

	insertItemToDB( db, m_pItem->GetItemUID(),
						m_pItem->GetOwnerUID(),
						m_pItem->GetAccountID(), 
						m_pItem->GetOwnSummonUID(),
						m_pItem->GetAuctionID(),
						m_pItem->GetItemKeepingID(),
						m_pItem->GetItemCode(), 
						m_pItem->GetPreviousUID(),
						*m_pItem->GetInstanceFlag().GetRawData(), 
						m_pItem->GetCount(),
						m_pItem->GetItemLevel(),
						m_pItem->GetItemEnhance(),
						m_pItem->GetCurrentEtherealDurability(),
						m_pItem->GetCurrentEndurance(),
						m_pItem->GetSocketCode( 0 ),
						m_pItem->GetSocketCode( 1 ),
						m_pItem->GetSocketCode( 2 ),
						m_pItem->GetSocketCode( 3 ),
						m_pItem->GetGenerateCode(),
						remain_time,
						m_pItem->GetElementalEffectType(),
						m_pItem->GetElementalEffectExpireTime(),
						m_pItem->GetElementalEffectAttackPoint(),
						m_pItem->GetElementalEffectMagicPoint(),
						m_pItem->GetAppearanceCode(),
						m_pItem->GetSummonCode(),
						m_pItem->GetEffectID());

	return true;
}

bool DB_InsertItem::onProcess( DBConnection & db )
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