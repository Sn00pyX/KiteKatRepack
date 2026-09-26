#pragma once

#include <vector>

#include <toolkit/c_fixed.h>
#include <toolkit/XEnv.h>
#include <dump/XException.h>
#include <toolkit/XConsole.h>
#include <logging/FileLog.h>
#include <toolkit/XStringUtil.h>

#include "StructItem.h"
#include "GameDBUtil.h"


struct _AWAKEN_INFO
{
	_AWAKEN_INFO()
	{
		nSid = 0;
		nType = 0;

		memset( Level, 0, sizeof( Level ));
	}

	struct Awaken_Level
	{
		int nValue1;
		int nValue2;
	};

	int nSid;
	int nType;
	Awaken_Level Level[40];
};

struct _AWAKEN_PROBABILITIES_INFO
{
	_AWAKEN_PROBABILITIES_INFO()
	{
		nSid = 0;
		nLevel = 0;
	}

	int					nSid;
	int					nLevel;
	c_fixed10			fProbabilities;	
};

struct RandomItemOptionInfo
{
	int						optionID;		// 옵션 ID
	int						subID;			// 보조 ID

	int						optionType;		// 증가/증폭형 결정
	int						optionValue1;	// 옵션 타입 결정
	c_fixed10				minValue2;		// 최소 랜덤 값
	c_fixed10				maxValue2;		// 최대 랜덤 값
	c_fixed10				extremeRate;	// 극값이 나올 확률
};

struct RandomItemOptionGroupInfo
{
	int						optionID;		// 옵션 ID
	std::vector< RandomItemOptionInfo > optionList;		// 같은 optionID를 가진 옵션리스트
};

struct RandomItemPresetInfo
{
	RandomItemPresetInfo() : presetID( 0 )
	{
		memset( optionID, 0, sizeof( optionID ) );
		memset( optionSucRate, 0, sizeof( optionSucRate ) );
	}
	int						presetID;		// 프리셋 ID
	int						optionID[ ItemInstance::MAX_RANDOM_OPTION_NUMBER ];				// 옵션 아이디들
	int						optionSucRate[ ItemInstance::MAX_RANDOM_OPTION_NUMBER ];		// 각 옵션별 성공률
};


struct RandomManager
{
	// DB로드 유효성 확인하는 함수 (DB로드부에서 한번 부름)
	static void CheckValidityForRandomOptionResource();

	static void RegisterAwakenInfo( const _AWAKEN_INFO & _AwakenInfo );
	static void RegisterAwakenProbabilitiesInfo( struct _AWAKEN_PROBABILITIES_INFO & _AwakenProbabilities_info );

	static void RegisterRandomItemOptionInfo( struct RandomItemOptionInfo & _RandomItemOptionInfo );
	static void RegisterRandomItemPresetInfo( struct RandomItemPresetInfo & _RandomItemPresetInfo );

	static int GetAwakenInfoValue( int _nLevel, int _nValueType, int & _nBitSet, int & _nType );
	static int GetMaxAwakenCount();

	static c_fixed10 GetAwakenProbabilitiesInfo( int _nlevel );
	static void GetAwakenProbabilitiesInfo( int *_arProvabilities, int _nCnt );
	static bool CheckMaxAwakenOption( const ItemInstance::RANDOM_OPTION * _pAwakenOption, int _nBitSet, int _nType, int _nData );

	static bool PickRandomOptionByPreset( struct ItemInstance::RANDOM_OPTION * option, int presetID );

	static void	SetMaxRandomOptionSID( int nSID );
	static int	AllocRandomSID();

protected:
	static ItemInstance::OPTION_INFO _PickRandomOptionFromOptionID( int id );
};