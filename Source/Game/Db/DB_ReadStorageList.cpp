
#include <atlcomtime.h>
#include <oledb.h>
#include <oledberr.h>
#include <icrsint.h>
#include <vector>

#include <network/IConnection.h>
#include <network/XIOCPConnection.h>
#include <toolkit/XConsole.h>
#include <mmo/ArcadiaServer.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "DB_Commands.h"
#include "SendMessage.h"
#include "GameMessage.h"
#include "StructPlayer.h"
#include "StructItem.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "GameProc.h"
#include "RandomManager.h"


bool DB_ReadStorageList::readSummonStateInfo( DBConnection & db, StructSummon * pSummon )
{
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ReadStorageList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_state_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_state_list";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_OWNER_UID", adInteger, adParamInput, 4, 0  ) );
	cmd->Parameters->Append( cmd->CreateParameter( "IN_SUMMON_ID", adInteger, adParamInput, 4, pSummon->GetSummonSID() ) );

	_RecordsetPtr pRS = cmd->Execute(NULL, NULL,adCmdStoredProc);

	StructState state;
	for( ; pRS->State != adStateClosed && !pRS->EndOfFile; pRS->MoveNext() )
	{
		int sid = pRS->Fields->Item["sid"]->Value;
		int code = pRS->Fields->Item["code"]->Value;

		unsigned short level = pRS->Fields->Item["level"]->Value;
		AR_TIME duration = pRS->Fields->Item["duration"]->Value;
		AR_TIME remain_time = pRS->Fields->Item["remain_time"]->Value;
		int base_damage = pRS->Fields->Item["base_damage"]->Value;
		AR_TIME remain_fire_time = pRS->Fields->Item["remain_fire_time"]->Value;

		int state_value = pRS->Fields->Item["state_value"]->Value;

		_bstr_t state_string_value = pRS->Fields->Item["state_string_value"]->Value;

		int enable = pRS->Fields->Item["enable"]->Value;

		const StateInfo * pInfo = GameContent::GetStateInfo( code );

		if( !pInfo )
			continue;

		if(	pInfo->state_time_type & StateInfo::TIME_DECREASE_ON_LOGOUT )
		{
			if( remain_time != AR_TIME(-1) )
			{
				if( remain_time <= AR_TIME(m_pPlayer->GetLogoutDuration() * 100 * 60) )
					continue;

				remain_time -= AR_TIME(m_pPlayer->GetLogoutDuration() * 100 * 60);
			}
		}

		AR_HANDLE handle = remain_time ? m_pPlayer->GetHandle() : 0;

		AR_TIME t = GetArTime();

		if( enable == 1 )
		{
			state.SetState( (StructState::StateCode) code, sid, handle, level, duration, remain_time, GetArTime() + remain_fire_time - pInfo->fire_interval * 100, base_damage, state_value, state_string_value, enable );

			pSummon->m_vStateList.push_back( state );
		}
	}

	return true;
}

bool DB_ReadStorageList::readStorageSummonList( DBConnection & db, std::vector<StructSummon *> & vSummonList )
{
	int account_id = m_pPlayer->GetAccountID();
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readStorageSummonList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_storage_summon_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_storage_summon_list";
	
	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, account_id ) );

	_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		int nSummonSID = pRS->Fields->Item["sid"]->Value;

		// 리로드일 경우 이미 로드되었던 소환수면 패스
		if( m_bReload )
		{
			std::vector< StructSummon * >::const_iterator it;

			for( it = m_pPlayer->m_vStorageSummonList.begin() ; it != m_pPlayer->m_vStorageSummonList.end() ; ++it )
			{
				if( (*it)->GetSummonSID() == nSummonSID )
					break;
			}

			if( it != m_pPlayer->m_vStorageSummonList.end() )
			{
				pRS->MoveNext();
				continue;
			}

			for( it = m_pPlayer->m_vSummonList.begin() ; it != m_pPlayer->m_vSummonList.end() ; ++it )
			{
				if( (*it)->GetSummonSID() == nSummonSID )
					break;
			}

			if( it != m_pPlayer->m_vSummonList.end() )
			{
				assert( 0 && "Loading summon info from stroage error");
				pRS->MoveNext();
				continue;
			}
		}

		StructSummon *pSummon = StructSummon::AllocSummon( NULL, pRS->Fields->Item["code"]->Value );

		pSummon->SetAccountId( account_id );
		// _oprint( "ALLOC SUMMON : %08X\n", pSummon );
		pSummon->SetEXP( pRS->Fields->Item["exp"]->Value );
		pSummon->SetLevel( pRS->Fields->Item["lv"]->Value );
		pSummon->SetJobLevel( pRS->Fields->Item["lv"]->Value );
		pSummon->SetMaxReachedLevel( pRS->Fields->Item["max_level"]->Value );
		pSummon->SetSummonSID( nSummonSID );
		pSummon->SetJP( pRS->Fields->Item["jp"]->Value );
		pSummon->SetName( static_cast< _bstr_t >( pRS->Fields->Item["name"]->Value ) );
		pSummon->SetCurrentXY( m_pPlayer->GetX(), m_pPlayer->GetY() );
		pSummon->SetPrevJobId( 0, pRS->Fields->Item["prev_id_01"]->Value );
		pSummon->SetPrevJobId( 1, pRS->Fields->Item["prev_id_02"]->Value );
		pSummon->SetPrevJobLevel( 0, pRS->Fields->Item["prev_level_01"]->Value );
		pSummon->SetPrevJobLevel( 1, pRS->Fields->Item["prev_level_02"]->Value );
		pSummon->m_nLastDecreasedEXP = pRS->Fields->Item["last_decreased_exp"]->Value.llVal;

		vSummonList.push_back( pSummon );

		readSummonStateInfo( db, pSummon );

		pSummon->CalculateStat();

		pSummon->SetSP( pRS->Fields->Item["sp"]->Value );
		pSummon->SetHP( pRS->Fields->Item["hp"]->Value );
		pSummon->SetMP( pRS->Fields->Item["mp"]->Value );

		pRS->MoveNext();
	}

	return true;
}

bool DB_ReadStorageList::readStoragePetList( DBConnection & db, std::vector<StructPet *> & vPetList )
{
	int account_id = m_pPlayer->GetAccountID();
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readStoragePetList : CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_storage_pet_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_storage_pet_list";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, account_id  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

	while( pRS->State != adStateClosed && !pRS->EndOfFile )
	{
		int nPetSID = pRS->Fields->Item["sid"]->Value;

		// 리로드일 경우 이미 로드되었던 펫이면 패스
		if( m_bReload )
		{
			std::vector< StructPet * >::const_iterator it;

			for( it = m_pPlayer->m_vStoragePetList.begin() ; it != m_pPlayer->m_vStoragePetList.end() ; ++it )
			{
				if( (*it)->GetPetSID() == nPetSID )
					break;
			}

			if( it != m_pPlayer->m_vStoragePetList.end() )
			{
				pRS->MoveNext();
				continue;
			}

			for( it = m_pPlayer->m_vPetList.begin() ; it != m_pPlayer->m_vPetList.end() ; ++it )
			{
				if( (*it)->GetPetSID() == nPetSID )
					break;
			}

			if( it != m_pPlayer->m_vPetList.end() )
			{
				assert( 0 && "loading pet info from storage error");
				pRS->MoveNext();
				continue;
			}
		}

		StructPet *pPet = StructPet::AllocPet( NULL, pRS->Fields->Item["code"]->Value );

		pPet->SetPetSID( nPetSID );
		pPet->SetName( static_cast< _bstr_t >( pRS->Fields->Item["name"]->Value ) );
		pPet->SetCurrentXY( m_pPlayer->GetX(), m_pPlayer->GetY() );

		vPetList.push_back( pPet );

		pPet->SetHP( 100 );
		pPet->SetMP( 100 );

		pRS->MoveNext();
	}

	return true;
}

bool DB_ReadStorageList::readStorageRandomOptionList( DBConnection & db, std::vector< ItemInstance::RANDOM_OPTION > & vRandomOptionList )
{
	int account_id = m_pPlayer->GetAccountID();
	_CommandPtr cmd;
	if( db.CreateCommand( cmd ) == false )	throw XException( "readStorageRandomOptionList: CreateInstance(command) error" );

	cmd->CommandType = adCmdStoredProc;
	cmd->CommandText = _bstr_t( "dbo.smp_read_storage_random_option_list" );
	// Store the name of current stored-procedure for debugging
	szStoredProcedureName = "dbo.smp_read_storage_random_option_list";

	cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, account_id  ) );

	_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

	for( ; pRS->State != adStateClosed && !pRS->EndOfFile ; pRS->MoveNext() )
	{
		struct ItemInstance::RANDOM_OPTION RandomOption;
		std::string strBuffer;
		_decimal_variant decBuffer;

		RandomOption.nSID		= pRS->Fields->Item["sid"]->Value;
		RandomOption.nRandomType= pRS->Fields->Item["random_type"]->Value;

		for( int i = 0; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER; ++i )
		{
			XStringUtil::Format( strBuffer, "type_%02d", i + 1 );
			RandomOption.OptionInfo[ i ].nType = pRS->Fields->Item[strBuffer.c_str()]->Value;

			XStringUtil::Format( strBuffer, "value1_%02d", i + 1 );
			*static_cast< DECIMAL * >( &decBuffer ) = pRS->Fields->Item[strBuffer.c_str()]->Value.decVal;
			RandomOption.OptionInfo[ i ].fValue1.set( decBuffer.getMultipleInteger( 10000 ) );

			XStringUtil::Format( strBuffer, "value2_%02d", i + 1 );
			*static_cast< DECIMAL * >( &decBuffer ) = pRS->Fields->Item[strBuffer.c_str()]->Value.decVal;
			RandomOption.OptionInfo[ i ].fValue2.set( decBuffer.getMultipleInteger( 10000 ) );
		}

		vRandomOptionList.push_back( RandomOption );
	}

	return true;
}

void DB_ReadStorageList::getRandomOption( std::vector< ItemInstance::RANDOM_OPTION > & vRandomOptionList, int _SID, ItemInstance::RANDOM_OPTION *_pRandomOption )
{
	std::vector< ItemInstance::RANDOM_OPTION >::iterator it = vRandomOptionList.begin();
	for( ; it != vRandomOptionList.end() ; ++it )
	{
		if( _SID == (*it).nSID )
		{
			_pRandomOption->nSID = _SID;
			_pRandomOption->nRandomType = it->nRandomType;

			for( int nCnt = 0 ; nCnt < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++nCnt )
			{
				_pRandomOption->OptionInfo[ nCnt ].nType = (*it).OptionInfo[ nCnt ].nType;
				_pRandomOption->OptionInfo[ nCnt ].fValue1 = (*it).OptionInfo[ nCnt ].fValue1;
				_pRandomOption->OptionInfo[ nCnt ].fValue2 = (*it).OptionInfo[ nCnt ].fValue2;
			}

			vRandomOptionList.erase( it );
			return;
		}
	}
}

bool DB_ReadStorageList::onProcess( DBConnection & db )
{
	std::vector< StructSummon * > vSummonList;
	std::vector< StructPet * > vPetList;
	std::vector< ItemInstance::RANDOM_OPTION > vRandomOptionList;

	try
	{
		_CommandPtr cmd;
		if( db.CreateCommand( cmd ) == false )	throw XException( "DB_ReadStorageList : CreateInstance(command) error" );

		cmd->CommandType = adCmdStoredProc;
		cmd->CommandText = _bstr_t( "dbo.smp_read_storage_list" );
		// Store the name of current stored-procedure for debugging
		szStoredProcedureName = "dbo.smp_read_storage_list";
		s_sprintf( szStoredProcedureDebugInfo, _countof( szStoredProcedureDebugInfo ), "@IN_ACCOUNT_ID: %d", m_pPlayer->GetAccountID() );
		
		cmd->Parameters->Append( cmd->CreateParameter( "IN_ACCOUNT_ID", adInteger, adParamInput, 4, m_pPlayer->GetAccountID() ) );

		_RecordsetPtr pRS = cmd->Execute(NULL,NULL,adCmdStoredProc);

		bool bIsGoldExist = false;

		readStorageSummonList( db, vSummonList );
		readStoragePetList( db, vPetList );
		readStorageRandomOptionList( db, vRandomOptionList );

		if( pRS->State != adStateClosed )
		{
			std::vector< StructItem::INDEX_FINDER > vItemList;
			int nItemIndex = 0;
			for( ; !pRS->EndOfFile && nItemIndex < GameRule::nMaxStorageItemCount ; pRS->MoveNext() )
			{
				ItemUID	nItemUID	= pRS->Fields->Item["sid"]->Value;
				int		nItemCode	= pRS->Fields->Item["code"]->Value.intVal;
				__int64	nCount		= pRS->Fields->Item["cnt"]->Value.llVal;

				// 리로드일 경우 이미 로드되었던 아이템이면 패스
				if( m_bReload && nItemCode )
				{
					StructItem * pPrevItem = m_pPlayer->m_Storage.Find( nItemUID );
					if( !pPrevItem )
					{
						pPrevItem = m_pPlayer->m_Inventory.Find( nItemUID );
						assert( !pPrevItem );
					}

					if( pPrevItem )
					{
						// 이미 (창고든 아이템이든) 있는 아이템이면 가지고 있는 데이터가 최신이므로 무시
						++nItemIndex;
						continue;
					}

					// 창고와 인벤에 모두 없다면 처음 로딩이거나 버렸거나 상점에 팔았거나 다른 사람에게 줬거나.
					// 만약 처음이 아니라 버렸거나 다른 사람에게 줬으면 아이템 혹은 돈 복사가 일어난다.
					// DB가 이것보다 빠르게 갱신되도록 빌어야... 졸 위험타.
				}

				COleDateTime dtElementalEffectExpire( pRS->Fields->Item["elemental_effect_expire_time"]->Value.date );

				struct tm tmElementalEffectExpire;
				tmElementalEffectExpire.tm_year = dtElementalEffectExpire.GetYear() - 1900;
				tmElementalEffectExpire.tm_mon = dtElementalEffectExpire.GetMonth() - 1;
				tmElementalEffectExpire.tm_mday = dtElementalEffectExpire.GetDay();
				tmElementalEffectExpire.tm_hour = dtElementalEffectExpire.GetHour();
				tmElementalEffectExpire.tm_min = dtElementalEffectExpire.GetMinute();
				tmElementalEffectExpire.tm_sec = dtElementalEffectExpire.GetSecond();
				tmElementalEffectExpire.tm_isdst = -1;
				time_t tElementalEffectExpire = mktime( &tmElementalEffectExpire );

				StructItem *pItem = StructItem::AllocItem(	nItemUID,
					nItemCode,
					nCount,
					static_cast< ItemInstance::GenerateCode >( static_cast< int >( pRS->Fields->Item["gcode"]->Value ) ),
					pRS->Fields->Item["level"]->Value,
					pRS->Fields->Item["enhance"]->Value,
					pRS->Fields->Item["flag"]->Value,
					pRS->Fields->Item["socket_0"]->Value,
					pRS->Fields->Item["socket_1"]->Value,
					pRS->Fields->Item["socket_2"]->Value,
					pRS->Fields->Item["socket_3"]->Value,
					pRS->Fields->Item["awaken_sid"]->Value,
					pRS->Fields->Item["random_option_sid"]->Value,
					std::max( pRS->Fields->Item["remain_time"]->Value.intVal, 0 ),
					pRS->Fields->Item["elemental_effect_type"]->Value.intVal,
					tElementalEffectExpire,
					pRS->Fields->Item["elemental_effect_attack_point"]->Value,
					pRS->Fields->Item["elemental_effect_magic_point"]->Value,
					pRS->Fields->Item["appearance_code"]->Value,
					pRS->Fields->Item["summon_code"]->Value,
					pRS->Fields->Item["effect_id"]->Value);

				if( pItem->IsAwaken() )
				{
					ItemInstance::RANDOM_OPTION kAwakenOption;
					getRandomOption( vRandomOptionList, pItem->GetAwakenSID(), &kAwakenOption );
					pItem->SetAwakenOption( kAwakenOption );
				}
				if( pItem->IsIdentified() )
				{
					ItemInstance::RANDOM_OPTION kRandomOption;
					getRandomOption( vRandomOptionList, pItem->GetIdentifiedSID(), &kRandomOption );
					pItem->SetIdentifiedOption( kRandomOption );
				}

				// 소환수 테이밍 중이라는 플래그는 DB에서 읽어지면 안 됨
				pItem->SetInstanceFlagOff( ItemInstance::ITEM_FLAG_TAMING );

				pItem->SetCurrentEtherealDurability( pRS->Fields->Item["ethereal_durability"]->Value );
				pItem->SetCurrentEndurance( pRS->Fields->Item["endurance"]->Value );
				pItem->SetOwnerInfo( m_pPlayer->GetHandle(), 0, m_pPlayer->GetAccountID() );
				pItem->TurnOffDbUpdateFlag();

				// 돈일 경우
				if( nItemCode == 0 )
				{
					// 리로드일 경우에는 DB에서 돈은 읽지 않음
					if( m_bReload )
					{
						bIsGoldExist = true;

						StructItem::PendFreeItem( pItem );
						continue;
					}

					if( m_pPlayer->ChangeStorageGold( m_pPlayer->GetStorageGold() + nCount ) == RESULT_SUCCESS )
					{
						if( bIsGoldExist )
						{
							pItem->SetOwnerInfo( NULL, 0, 0 );
							pItem->DBQuery( new DB_UpdateItemOwner( pItem ) );

							m_pPlayer->DBQuery( new DB_UpdateStorageGold( m_pPlayer ) );
						}
						else
						{
							bIsGoldExist = true;
							m_pPlayer->m_nStorageGoldItemID = pItem->GetItemUID();
						}
					}
					else
					{
						_cprint( "DB_ReadStorageList::onProcess: Storage gold exceeded the maximum amount.[Account:%s, ItemSID:%I64d, Exist:%d, Owning:%I64d, Loaded:%I64d]\n", m_pPlayer->GetAccountName(), pItem->GetItemUID(), bIsGoldExist, m_pPlayer->GetStorageGold().GetRawData(), nCount );
						FILELOG( "DB_ReadStorageList::onProcess: Storage gold exceeded the maximum amount.[Account:%s, ItemSID:%I64d, Exist:%d, Owning:%I64d, Loaded:%I64d]", m_pPlayer->GetAccountName(), pItem->GetItemUID(), bIsGoldExist, m_pPlayer->GetStorageGold().GetRawData(), nCount );
					}

					StructItem::PendFreeItem( pItem );
					continue;
				}
				
				pItem->SetPreviousUID( pRS->Fields->Item["previous_sid"]->Value );
				nItemIndex++;
				vItemList.push_back( StructItem::INDEX_FINDER( pItem ) );

				// 이하 아이템 로딩 시 같이 해야 하는 작업들
				if( pItem->IsSummonCard() )
				{
					for( std::vector< StructSummon * >::iterator it = vSummonList.begin() ; it != vSummonList.end() ; ++it )
					{
						// 소환수에 해당하는 카드 아이템이 존재함. 연결.
						if( (*it)->GetSummonSID() == pItem->GetSummonSID() )
						{
							StructSummon *pSummon = (*it);

							vSummonList.erase( it );

							// 소환수 카드와 아이템의 소환수 코드가 잘못 연결 되 있으면 제대로 연결
							if( pItem->GetSummonCode() != pSummon->GetSummonCode() )
							{
								pItem->SetSummonCode( pSummon->GetSummonCode() );
								pItem->DBQuery( new DB_UpdateItem( pItem ) );
							}

							pSummon->SetParentCard( pItem );
							pItem->SetSummonStruct( pSummon );
							pItem->SetSummonSID( pSummon->GetSummonSID() );

							void readCreatureSkillList( DBConnection & db, StructCreature *pCreature );
							readCreatureSkillList( db, pSummon );

							pSummon->SetLoginComplete();

							break;
						}
					}
				}
				if( pItem->IsPetCage() )
				{
					for( std::vector< StructPet * >::iterator it = vPetList.begin() ; it != vPetList.end() ; ++it )
					{
						// 펫에 해당하는 우리 아이템이 존재함. 연결.
						if( (*it)->GetPetSID() == pItem->GetPetSID() )
						{
							StructPet *pPet = (*it);

							vPetList.erase( it );

							pPet->SetParentCage( pItem );
							pItem->SetPetStruct( pPet );
							pItem->SetPetSID( pPet->GetPetSID() );

							pPet->SetLoginComplete();

							// 소환수의 경우 편성할 때만 정보를 보내면 되지만
							// 펫은 아이템과 늘 함께 존재해야 하므로 여기서 보냄
							SendAddPetMessage( m_pPlayer, pPet );

							break;
						}
					}
				}
			}

			m_pPlayer->m_Storage.LoadItemList( vItemList, true );
		}

		// 창고에 돈 아이템이 없으면 추가한다.
		if( !bIsGoldExist )
		{
			m_pPlayer->m_nStorageGoldItemID = m_pPlayer->allocItemUID();

			DB_InsertItem::insertItemToDB( db, m_pPlayer->m_nStorageGoldItemID, 0, m_pPlayer->GetAccountID(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, ItemInstance::BY_UNKNOWN, 0, Elemental::TYPE_NONE, 0, 0, 0, 0, 0,0 );
		}

		m_pPlayer->m_bIsStorageLoaded = true;
		LOG::Log11N4S( LM_CHARACTER_INFO, m_pPlayer->GetAccountID(), m_pPlayer->GetSID(), m_pPlayer->GetLevel(), m_pPlayer->GetJobLevel(), m_pPlayer->GetJobPoint(), m_pPlayer->GetJobId(), m_pPlayer->GetGold().GetRawData(), m_pPlayer->GetStorageGold().GetRawData(), m_pPlayer->GetChaos(), m_pPlayer->GetImmoralPoint(), m_pPlayer->GetEXP(), m_pPlayer->GetAccountName(), LOG::STR_NTS, m_pPlayer->GetName(), LOG::STR_NTS, "", 0, "", 0 );

		// 창고 정보 보냄
		m_pPlayer->OpenStorage( false );

		// DB에 창고 아이템이 한계 수량 이상 있는 경우 다음 번에 창고를 열 때 DB에서 다시 로딩하도록 함
		// 창고 보관 수량 제한 룰 적용에 따른 기존 유저의 처리
		// 여기보다 이전 단계에서 처리하면 StructPlayer::OpenStorage() -> StructPlayer::openStorage() 함수 호출 구조에 문제 생김
		if( !pRS->EndOfFile )
		{
			m_pPlayer->m_bIsStorageRequested = false;
			// m_pPlayer->m_bIsStorageLoaded는 true인 상태로 놔둬서 로그인 이후 최초 창고 로딩인지 아닌지를 식별하는데 사용 함
		}
	}
	catch( ... )
	{
		// 위에서 세팅하고 남은 소환수는 해당하는 카드가 보유자한테 없는 소환수 이므로 제거.(Exception 발생시 청소하기)
		if( !vSummonList.empty() )
		{
			for( std::vector< StructSummon * >::iterator it = vSummonList.begin() ; it != vSummonList.end() ; ++it )
			{
				ArcadiaServer::Instance().DeleteObject( (*it) );
			}
		}
		if( !vPetList.empty() )
		{
			for( std::vector< StructPet * >::iterator it = vPetList.begin() ; it != vPetList.end() ; ++it )
			{
				ArcadiaServer::Instance().DeleteObject( (*it) );
			}
		}

		throw;
	}	

	// 위에서 세팅하고 남은 소환수는 해당하는 카드가 보유자한테 없는 소환수 이므로 제거.(정상 종료 후 청소하기)
	if( !vSummonList.empty() )
	{
		for( std::vector< StructSummon * >::iterator it = vSummonList.begin() ; it != vSummonList.end() ; ++it )
		{
			ArcadiaServer::Instance().DeleteObject( (*it) );
		}
	}
	if( !vPetList.empty() )
	{
		for( std::vector< StructPet * >::iterator it = vPetList.begin() ; it != vPetList.end() ; ++it )
		{
			ArcadiaServer::Instance().DeleteObject( (*it) );
		}
	}

	m_pPlayer->onEndQuery();

	return true;
}

void DB_ReadStorageList::onFail( const _com_error & exception )
{
	// Timeout이 아닌 다른 DB 오류라면 그냥 서버 다운시킴
	if( exception.Error() != DB_E_TIMEOUT && exception.Error() != DB_E_ABORTLIMITREACHED )
	{
		XSEH::InvokeUnhandledException( exception.Error() );
		return;
	}

	std::string strMessage( GameContent::GetString( 103 ) );
	strMessage += "(6)";
	SendChatMessage( false, CHAT_NOTICE, "@NOTICE", m_pPlayer, strMessage.c_str() );
	m_pPlayer->m_bIsStorageRequested = false;
	
	// 창고 아이템 데이터의 일부가 로딩되었을 경우(Ex. SQL2005 이상에서 TIME-OUT인 경우)에는 창고 다시 오픈 시 리로딩으로 간주하고 처리하도록 로딩된 것이라고 처리
	// * 창고 루피가 로딩되지 않은 상태에서 아이템만 로딩되다 만 경우에는 이후 창고 재 오픈 시도 시 창고 루피가 0으로 나타나지만
	//   재접속 후에 다시 창고를 열게 되면 창고 루피 아이템 데이터가 2건이 되어 있으므로 2개의 합산 금액이 창고 루피로 나타나게 됨.(결과적으로 손실 없음. 만사 OK)
	if( m_pPlayer->m_nStorageGoldItemID || m_pPlayer->m_Storage.GetCount() )
		m_pPlayer->m_bIsStorageLoaded = true;
	else
		m_pPlayer->m_bIsStorageLoaded = false;

	m_pPlayer->onEndQuery();
}
