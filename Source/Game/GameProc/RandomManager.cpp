#include "RandomManager.h"

#include <toolkit/ILock.h>

#include "StructMisc.h"


static std::vector<_AWAKEN_INFO>					g_vAwakenInfo;
static std::vector<_AWAKEN_PROBABILITIES_INFO>		g_vAwakenProbabilitiesInfo;
static std::vector< RandomItemPresetInfo >			g_vRandomItemPresetInfo;	// 프리셋 정보 벡터
static std::vector< RandomItemOptionGroupInfo >		g_vRandomItemOptionInfo;	// 옵션 그룹 벡터 (같은 option ID를 공유하는 옵션리스트의 벡터)

static volatile LONG				s_nRandomMaxSID;

void RandomManager::CheckValidityForRandomOptionResource()
{
	for( std::vector< RandomItemPresetInfo >::iterator it = g_vRandomItemPresetInfo.begin() ; it != g_vRandomItemPresetInfo.end() ; ++it )
	{
		for( int i = 0 ; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++ i )
		{
			if( it->optionID[ i ] )
			{
				bool bFind = false;
				for( std::vector< RandomItemOptionGroupInfo >::iterator inIt = g_vRandomItemOptionInfo.begin() ; inIt != g_vRandomItemOptionInfo.end() ; ++inIt )
				{
					if( it->optionID[ i ] == inIt->optionID )
					{
						bFind = true;
						break;
					}
				}
				// 프리셋이 가리키는 옵션 ID가 존재하지 않음
				if( !bFind )
					assert( 0 );
			}
		}
	}
}

void RandomManager::RegisterAwakenInfo( const _AWAKEN_INFO & _AwakenInfo )
{
	std::vector<_AWAKEN_INFO>::iterator it;
	bool find = false;
	for( it = g_vAwakenInfo.begin() ; it != g_vAwakenInfo.end() ; ++it )
	{
		if( (*it).nSid == _AwakenInfo.nSid )
		{
			find = true;
			(*it).nSid = _AwakenInfo.nSid;
			(*it).nType = _AwakenInfo.nType;
			s_memcpy( (*it).Level, sizeof( (*it).Level ), _AwakenInfo.Level, sizeof( _AwakenInfo.Level ) );
			break;
		}
	}

	if( !find )
	{
		_AWAKEN_INFO info;
		info.nSid = _AwakenInfo.nSid;
		info.nType = _AwakenInfo.nType;
		s_memcpy( info.Level, sizeof( info.Level ), _AwakenInfo.Level, sizeof( _AwakenInfo.Level ) );

		g_vAwakenInfo.push_back( info );
	}
};


void RandomManager::RegisterAwakenProbabilitiesInfo( struct _AWAKEN_PROBABILITIES_INFO & _AwakenProbabilities_info )
{
	std::vector<_AWAKEN_PROBABILITIES_INFO>::iterator it;
	bool find = false;
	for( it = g_vAwakenProbabilitiesInfo.begin() ; it != g_vAwakenProbabilitiesInfo.end() ; ++it )
	{
		if( (*it).nSid == _AwakenProbabilities_info.nSid )
		{
			find = true;
			(*it).nSid = _AwakenProbabilities_info.nSid;
			(*it).nLevel = _AwakenProbabilities_info.nLevel;
			(*it).fProbabilities = _AwakenProbabilities_info.fProbabilities;
			break;
		}
	}

	if( !find )
	{
		_AWAKEN_PROBABILITIES_INFO info;
		info.nSid = _AwakenProbabilities_info.nSid;
		info.nLevel = _AwakenProbabilities_info.nLevel;
		info.fProbabilities = _AwakenProbabilities_info.fProbabilities;
		g_vAwakenProbabilitiesInfo.push_back( info );
	}
};

void RandomManager::RegisterRandomItemOptionInfo( struct RandomItemOptionInfo & info )
{
	for( std::vector< RandomItemOptionGroupInfo >::iterator it = g_vRandomItemOptionInfo.begin(); it != g_vRandomItemOptionInfo.end(); ++it )
	{
		if( (*it).optionID == info.optionID )
		{
			(*it).optionList.push_back( info );
			return;
		}
	}

	if(info.minValue2 > info.maxValue2)
	{
		_cprint("error item random option min/max value [id %d]\n", info.optionID);
	}

	RandomItemOptionGroupInfo groupInfo;
	groupInfo.optionID = info.optionID;
	groupInfo.optionList.push_back( info );
	g_vRandomItemOptionInfo.push_back( groupInfo );
}

void RandomManager::RegisterRandomItemPresetInfo( struct RandomItemPresetInfo & info )
{
	for( std::vector< RandomItemPresetInfo >::iterator it = g_vRandomItemPresetInfo.begin(); it != g_vRandomItemPresetInfo.end(); ++it )
	{
		if( (*it).presetID == info.presetID )
		{
			(*it) = info;

			return;
		}
	}

	g_vRandomItemPresetInfo.push_back( info );
}

c_fixed10 RandomManager::GetAwakenProbabilitiesInfo( int _nlevel )
{
	std::vector<_AWAKEN_PROBABILITIES_INFO>::iterator it;
	for( it = g_vAwakenProbabilitiesInfo.begin() ; it != g_vAwakenProbabilitiesInfo.end() ; ++it )
	{
		if( (*it).nLevel == _nlevel )
		{
			return (*it).fProbabilities;
		}
	}

	return 0;
}


void RandomManager::GetAwakenProbabilitiesInfo( int *_arProvabilities, int _nCnt )
{
	if( _nCnt > g_vAwakenProbabilitiesInfo.size() )
	{
		// LOG 남기자
		return;
	}

	memset( _arProvabilities, 0, sizeof(int)*_nCnt );

	int nCnt = 0;
	int nPrevCnt = 0;
	std::vector<_AWAKEN_PROBABILITIES_INFO>::iterator it;
	for( it = g_vAwakenProbabilitiesInfo.begin() ; it != g_vAwakenProbabilitiesInfo.end() ; ++it )
	{
		if( nCnt > _nCnt )
		{
			// LOG 남기자
			return;
		}

		_arProvabilities[ nCnt ] = _arProvabilities[ nPrevCnt ] + ( (*it).fProbabilities * 1000 );

		nPrevCnt = nCnt;
		nCnt++;
	}
}

int RandomManager::GetAwakenInfoValue( int _nLevel, int _nValueType, int & _nBitSet, int & _Type )
{
	int nValueType = 0;

	switch( _nValueType )
	{
	case 1:		nValueType = StructState::FLAG_STR;					break;
	case 2:		nValueType = StructState::FLAG_VIT;					break;
	case 3:		nValueType = StructState::FLAG_AGI;					break;
	case 4:		nValueType = StructState::FLAG_DEX;					break;
	case 5:		nValueType = StructState::FLAG_INT;					break;
	case 6:		nValueType = StructState::FLAG_MEN;					break;
	case 7:		nValueType = StructState::FLAG_ATTACK_POINT;		break;
	case 8:		nValueType = StructState::FLAG_MAGIC_POINT;			break;
	case 9:		nValueType = StructState::FLAG_DEFENCE;				break;
	case 10:	nValueType = StructState::FLAG_MAGIC_DEFENCE;		break;
	case 11:	nValueType = StructState::FLAG_ATTACK_SPEED;		break;
	case 12:	nValueType = StructState::FLAG_CAST_SPEED;			break;
	case 13:	nValueType = StructState::FLAG_MOVE_SPEED;			break;
	case 14:	nValueType = StructState::FLAG_ACCURACY;			break;
	case 15:	nValueType = StructState::FLAG_MAGIC_ACCURACY;		break;
	case 16:	nValueType = StructState::FLAG_AVOID;				break;
	case 17:	nValueType = StructState::FLAG_MAGIC_RESISTANCE;	break;

	default:
		{
			return 0;
		}
		break;
	}

	std::vector<_AWAKEN_INFO>::iterator it;
	for( it = g_vAwakenInfo.begin() ; it != g_vAwakenInfo.end() ; ++it )
	{
		if( (*it).Level[ _nLevel ].nValue1 == nValueType )
		{
			_Type = (*it).nType;
			_nBitSet = nValueType;
			return (*it).Level[ _nLevel ].nValue2;
		}
	}

	return 0;
}


int RandomManager::GetMaxAwakenCount()
{
	return static_cast< int >( g_vAwakenInfo.size() );
}


bool RandomManager::CheckMaxAwakenOption( const ItemInstance::RANDOM_OPTION * _pAwakenOption, int _nBitSet, int _nType, int _nData )
{
	if( NULL == _pAwakenOption || 0 >= _nBitSet || 0 >= _nType )
	{
		return false;
	}

	int nAwakenCheckData = 0;
	for( int nArrCnt = 0 ; nArrCnt < 5 ; ++nArrCnt )
	{
		int nAwakenCheckValue = _pAwakenOption->OptionInfo[ nArrCnt ].fValue1;
		int nAwakenCheckType = _pAwakenOption->OptionInfo[ nArrCnt ].nType;
		if( _nBitSet == nAwakenCheckValue && _nType == nAwakenCheckType )
		{
			nAwakenCheckData += _pAwakenOption->OptionInfo[ nArrCnt ].fValue2;
		}
	}

	nAwakenCheckData += _nData;

	int OneMin = 0, OneMax = 0;
	int MultiMin = 0, MultiMax = 0;

	if( _nBitSet & StructState::FLAG_STR					// 힘
		|| _nBitSet & StructState::FLAG_VIT					// 체력
		|| _nBitSet & StructState::FLAG_AGI					// 민첩
		|| _nBitSet & StructState::FLAG_DEX					// 집중
		|| _nBitSet & StructState::FLAG_INT					// 지능
		|| _nBitSet & StructState::FLAG_MEN )				// 정신
	{
		OneMin		= -50;
		OneMax		=  50;
		MultiMin	= -100;
		MultiMax	=  100;
	}

	if( _nBitSet & StructState::FLAG_ATTACK_POINT			// 물리 공격력
		|| _nBitSet & StructState::FLAG_MAGIC_POINT )		// 마법 공격력
	{
		OneMin 		= -500;
		OneMax 		=  500;
		MultiMin 	= -1500;
		MultiMax 	=  1500;
	}

	if( _nBitSet & StructState::FLAG_DEFENCE				// 물리 방어력
		|| _nBitSet & StructState::FLAG_MAGIC_DEFENCE )		// 마법 방어력
	{
		OneMin		= -200;
		OneMax		=  200;
		MultiMin	= -600;
		MultiMax	=  600;
	}


	if( _nBitSet & StructState::FLAG_ACCURACY				// 명중
		|| _nBitSet & StructState::FLAG_AVOID				// 회피
		|| _nBitSet & StructState::FLAG_MAGIC_ACCURACY		// 마법명중
		|| _nBitSet & StructState::FLAG_MAGIC_RESISTANCE )	// 마법저항
	{
		OneMin		= -200;
		OneMax		=  200;
		MultiMin	= -600;
		MultiMax	=  600;
	}

	if( _nBitSet & StructState::FLAG_ATTACK_SPEED			// 공격속도
		|| _nBitSet & StructState::FLAG_CAST_SPEED			// 시전속도
		|| _nBitSet & StructState::FLAG_MOVE_SPEED )		// 이동속도
	{
		OneMin		= -20;
		OneMax		=  20;
		MultiMin	= -60;
		MultiMax	=  60;
	}

	// 하나의 인자 값 범위 확인
	if( OneMin > _nData || OneMax < _nData )
	{
		return false;
	}

	// 누적 인자값 범위 확인
	if( MultiMin > nAwakenCheckData || MultiMax < nAwakenCheckData )
	{
		return false;
	}

	return true;
}

bool RandomManager::PickRandomOptionByPreset( struct ItemInstance::RANDOM_OPTION * option, int presetID )
{
	// 프리셋 ID로 프리셋 찾기
	std::vector< RandomItemPresetInfo >::iterator it = g_vRandomItemPresetInfo.begin();
	bool bFind = false;
	for( ; it != g_vRandomItemPresetInfo.end() ; ++it )
	{
		if( presetID == it->presetID )
		{
			bFind = true;
			break;
		}
	}

	if( !bFind )
		return false;

	option->nRandomType = ItemInstance::IDENTIFIED;

	int k = 0;
	// 찾은 프리셋의 옵션 돌면서 랜덤화시키기
	for( int i = 0 ; i < ItemInstance::MAX_RANDOM_OPTION_NUMBER ; ++i )
	{
		if( !it->optionSucRate[ i ] )
			break;

		int nRandom = it->optionSucRate[ i ] * 10000;
		 // 한 옵션당 성공 or 꽝 결정
		if( static_cast< int >( XRandom( 1, 1000000 ) ) <= nRandom ) 
		{
			bool bDuplicated;
			ItemInstance::OPTION_INFO kData;
			do
			{
				bDuplicated = false;
				kData = _PickRandomOptionFromOptionID( it->optionID[ i ] );
				for( int j = 0 ; j < k ; ++ j )
				{
					// 중복 체크
					if( option->OptionInfo[ j ].nType == kData.nType && option->OptionInfo[ j ].fValue1 == kData.fValue1 )
					{
						bDuplicated = true;
						break;
					}
				}
			}
			while( bDuplicated );
			option->OptionInfo[ k ].nType = kData.nType;
			option->OptionInfo[ k ].fValue1 = kData.fValue1;
			option->OptionInfo[ k ].fValue2 = kData.fValue2;
			++k;
		}
	}
	return true;
}

ItemInstance::OPTION_INFO RandomManager::_PickRandomOptionFromOptionID( int id )
{
	ItemInstance::OPTION_INFO kOption;
	::ZeroMemory(&kOption, sizeof(kOption));

	for( std::vector< RandomItemOptionGroupInfo >::iterator it = g_vRandomItemOptionInfo.begin(); it != g_vRandomItemOptionInfo.end() ; ++it )
	{
		if( id == it->optionID )
		{
			int nSet = XRandom() % static_cast< int >( it->optionList.size() );
			RandomItemOptionInfo kInfo = it->optionList[ nSet ];

			kOption.nType = kInfo.optionType;
			kOption.fValue1 = kInfo.optionValue1;
			// XFastNormalRandom 함수는 정수를 반환하므로 소수점 1째 자리까지 표현하기 위해 곱하기 10 나누기 10 처리 해준다.
			int min = static_cast< int >( kInfo.minValue2 * 10 );
			int max = static_cast< int >( kInfo.maxValue2 * 10 );
			c_fixed10 rate;
			rate.set( kInfo.extremeRate * 100 );

			if(min > max)
			{
				max = min;
			}

			kOption.fValue2.set( XNormalRandom( min, max, rate ) * 1000 );

			break;
		}
	}
	return kOption;
}

void RandomManager::SetMaxRandomOptionSID( int nSID )
{
	if( s_nRandomMaxSID )
	{
		throw XException( "Call SetMaxRandomOptionSID twice by a manager" );
	}

	s_nRandomMaxSID = nSID;
}

int RandomManager::AllocRandomSID()
{
	return InterlockedIncrement( &s_nRandomMaxSID ) + 1;
}
