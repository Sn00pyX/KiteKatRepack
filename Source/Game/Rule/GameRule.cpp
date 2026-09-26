
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cmath>
#include <string>
#include <vector>

#include <toolkit/XEnv.h>
#include <toolkit/ILock.h>
#include <logging/FileLog.h>


#include "GameRule.h"
#include "StructMisc.h"
#include "Constant.h"
#include "StructCreature.h"


namespace GameRule
{
AR_TIME	nBattleArenaReconnectWaitDuration	= 18000;	// ��� ���� �� ������ ���� ������ �ٽ� ��⿡ ������ ������ ��ٷ��ִ� �ð�(�⺻�� 3��)

float fPlyMod					= 0.0f;
float fPartyEXPRate[ 7 ]			= { 1, 1, 1, 1, 1, 1, 1 };	// ��Ƽ ��ġ ���ʽ�
float fEXPRate					= 1.0f;		// ��ġ ����
float fGoldDropRate				= 1.0f;		// �� ��� ����
float fItemDropRate				= 1.0f;		// ������ ��� ����
float fChaosDropRate				= 1.0f;	// ȥ�� ��� ����
float fPVPDamageRateForPlayer	= 0.05f;	// PVP �� ���� ������ ����(������: �÷��̾�)
float fPVPDamageRateForSummon	= 0.05f;	// PVP �� ���� ������ ����(������: ��ȯ��)

float	fStaminaBonusRate					= 1.0f;							// ���׹̳� ���ʽ� ���� ����(EXP/JP)
float	fSuperSaveBonusRate					= 2.0f;							// ������ ����(�� ���� ���̹�) ���ʽ� ���� ����(EXP/JP)
int		anSuperSaveLevelMinLimit[ 7 ]		= {  1,  121, 131, 141, 121, 151, 171 };	// ������ ����(�� ���� ���̹�) ���� �ּ� ����
int		anSuperSaveLevelMaxLimit[ 7 ]		= { 130, 140, 150, 160, 160, 170, 180 };	// ������ ����(�� ���� ���̹�) ���� �ִ� ����
float	fSummonStatminaSaveBonusRate		= 0.5f;							// ��ȯ�� ���׹̳� ���̹�(���� ũ��Ŀ) ���ʽ� ���� ����(EXP)

int			nGuildDonateGold				= 1000000;
int			nGuildBuffMinute				= 60;


float	fAllyPCBangBonusRate				= 0.1f;		// ����� PC�� ���� ����(EXP/JP)
float	fAllyPCBangChaosBonusRate			= 0.1f;		// ����� PC�� ���� ����(Lac)
float	fPremiumPCBangBonusRate				= 1.2f;		// ���� �÷��� PC�� ���� ����(EXP/JP)
float	fPremiumPCBangChaosBonusRate		= 0.1f;		// ���� �÷��� PC�� ���� ����(Lac)
float	fPremiumPCBangGoldBonusDropRate		= 1.0f;		// ���� �÷��� PC�� ���� ����(���� �����, �⺻��: 1.0)
float	fPremiumPCBangItemBonusDropRate		= 1.0f;		// ���� �÷��� PC�� ���� ����(������ �����, �⺻��: 1.0)
float	fPremiumPCBangChaosBonusDropRate	= 1.0f;		// ���� �÷��� PC�� ���� ����(��ũ �����, �⺻��: 1.0)
bool	bApplyStaminaBonusInPremiumPCBang	= false;	// ���� �÷��� PC�濡�� ���׹̳� ȿ�� ���� ��� ����

bool	bUsePlayPoint						= false;	// �÷��� ����Ʈ ��� ����
int		nPlayPointAccumulateTerm			= 60;		// �÷��� ����Ʈ ���� ����(�� ����)
int		nPlayPointAccumulateAmount			= 1;		// �� ���� �÷��� ����Ʈ ������ ������ ����Ʈ ��
float	fPremiumPCBangPlayPointBonusRate	= 2.0f;		// ���� �÷��� PC�� ���� ����(�÷��� ����Ʈ)

bool	bUseTimeBasedEventScript			= false;	// ��ũ��Ʈ ��� �ð��� �̺�Ʈ ��� ����
bool	bUseTimeBasedEventDB				= false;	// DB SP ��� �ð��� �̺�Ʈ ��� ����
int		nTermForTimeBasedEventScript		= 60;		// ��ũ��Ʈ ��� �ð��� �̺�Ʈ �߻� ����(�� ����)
int		nTermForTimeBasedEventDB			= 60;		// DB SP ��� �ð��� �̺�Ʈ �߻� ����(�� ����)

bool bIsNoCollisionCheck = false;
bool bSkipLoadingAttribute = false;

bool bMonsterWandering = true;
bool bMonsterCollisionToLine = true;
bool bMonsterPathFinding = false;
bool bLogMonsterPathFinding = true;
bool bLogSchedulingStatus = true;
bool bIgnoreSkillCoolTime = false;
bool bIsPKServer = false;
bool Hardcore = false;
float HardcoreExpRate;
int		nPKPenaltyLevel = 10;
bool bDisablePKOn = false;					// PK On ���� ����
bool bIsAdultServer = false;
bool bRestrictSpeicialChar = true;			// �α��ν� ������ Ư�� ���� ��� ���� ����
std::string strAllowedSpecialChar = "";		// �α��ν� ������ ����� Ư�� ����

bool bAutoOpen = false;

bool bDisableHuntaholic = false;					// Huntaholic ���� ����

bool bUseAutoJail = true;							// ������ ��� ����

int nSecuritySolutionType = 0;						// ���� �ַ�� ����(0: ��� �� ��, 1: ���Ӱ���, 2: �ٽǵ�, 3: X-Trap)
AR_TIME nPeriodOfSecuritySolutionCheck = 5*60*100;	// ���� �ַ�� Ŭ��/���� üũ �ֱ�(5��)
AR_TIME nSecuritySolutionResponseTimeout = 30*100;	// ���� �ַ�� Ŭ��/���� üũ ��û �� ������� ���� �ð�(30��)
std::string strSecuritySolutionExceptionalIP;		// ���� �ַ�� Ŭ��/���� üũ ������ IP �ּ� ���(;���� ����)

bool bDisableDungeonRaidSiege = false;
bool bIsCashUsableServer = false;
bool bUseAccountAuthorityDB = false;
bool bCashItemDropable = false;
bool bUseAutoTrap = true;
bool bBroadcastEventItemPickup = false;
bool bUseGuildDonationPoint = false;
bool bRestrictBanWordForBooth = false;
int nMinBoothStartableLevel = 0;
bool bLimitBoothOpenableLayerToZero = false;	// ������ 0�� ���̾���� �� �� �ֵ��� ����(�������� �� ��� ���� �ǿ� ����)
bool bDisableBuyBooth = false;					// ���� ������ ����� �� ������ �����ϴ� ���
bool bDisableBooth = false;
bool bDisableTrade = false;

bool bLimitAdvChatCount = true;
int nMinGlobalChatUsableLevel = 0;
int nMaxStorageItemCount = 1000;
int nMaxCharactersPerAccount = 8;				// ������ �ִ� ĳ���� ��

bool bUseSecurityNo = false;
bool bUseSecurityNoForStorage = false;
bool bUseSecurityNoForDeletingCharacter = false;
bool bCheckStorageSecurityAlways = true;

bool bLimitFieldLogout = false;
AR_TIME nLogoutTimer = 1000;
bool bForceUnregisterAccountOnKickFail = false;		// ���� ���� �õ� �� ű ó���� ���������� �̷������ ���� ��� ���� ���� �α��� ���� ���� ����

bool bForbiddenScriptInitialized = false;		// ä�� â�� ���� �Էµ� ���ɾ� �� ������ ���ɾ� ��� �ʱ�ȭ ����
												// ä�� â�� ���� ��ũ��Ʈ ���ɾ� �Է� �� �� ���� false�� �������� �ٽ� �Ľ���
std::string strForbiddenScript;					// ä�� â�� ���� �Էµ� ���ɾ� �� ������ ���ɾ� ���(�� ������ = ';', �� ���� �׸� ������ = ',')

bool bUseLoginLogoutDebug = false;

bool bLogVulcanusDungeon = false;

bool bLimitGameTime = false;

int SummonExpPenaltyLevel = 10;
int PartyExpPenaltyLevel = 5;
int PartyExpPenaltyLevelCutoff = 175;
float fTamingSuccessRate2 = 0;
float fTamingSuccessRate3 = 0;
float fTamingSuccessRate4 = 0;
float fTamingSuccessRate5 = 0;
bool bGMsNeverFail = true;
int	nSkyFortressStartIndex = 0;

int nMaxGameTimeLimitedAge = 17;
AR_TIME nMaxHealthyGameTime = 1080000;
AR_TIME nMaxTiredGameTime = 1800000;

int nEtherealDurabilityBaseConsumptionOnNormalAttack = 31;	// ���׸��� ������ ��Ÿ ���� �� �⺻ �Ҹ�
int nEtherealDurabilityBaseConsumptionOnSkillAttack = 51;	// ���׸��� ������ ��ų ���� �� �⺻ �Ҹ�(���� ����)
int nEtherealDurabilityBaseConsumptionOnDamage = 98;		// ���׸��� ������ �ǰ� �� �⺻ �Ҹ�

int nMaxLevel = 230;	// Epic9.1 - �ִ� ĳ���� ���� 180���� ����

std::string strLogRequiredStateList;		// �Ҹ� �� �α׸� ���ܾ� �ϴ� ����ȿ�� ID ���
std::string strLogRequiredItemList;			// �Ⱓ ���� �� �α׸� ���ܾ� �ϴ� ������ ID ���

AR_TIME nAuctionSearchRequestMinInterval = 300;		// ��� �˻� �ݺ� ���� �ִ� �ð� ����(1/100 �� ����)
AR_TIME nAuctionProcessRequestMinInterval = 100;	// ��� �Ϲ� ���� �ݺ� ���� �ִ� �ð� ����(�˻� ���� ��� ����)

int nFarmNormalSummonEXP	= 145763;	// ��ȯ�� �⺻���� ���� �ð��� ����ġ
int	nFarmGrowthSummonEXP	= 1118029;	// ��ȯ�� �������� ���� �ð��� ����ġ
int nFarmEvolveSummonEXP	= 3708799;	// ��ȯ�� ��ȭ���� ���� �ð��� ����ġ

int nPremiumFarmNormalSummonEXP	= 728814;	// ��ȯ�� �⺻���� ���� �ð��� ����ġ (�����̾� Ƽ�� �̿�)
int nPremiumFarmGrowthSummonEXP	= 13975356;	// ��ȯ�� �⺻���� ���� �ð��� ����ġ (�����̾� Ƽ�� �̿�)
int nPremiumFarmEvolveSummonEXP	= 37087982;	// ��ȯ�� �⺻���� ���� �ð��� ����ġ (�����̾� Ƽ�� �̿�)

float stamina_ratio[MAX_LEVEL];

int player_exp_limit[MAX_LEVEL];

int normal_summon_exp_limit[NORMAL_SUMMON_MAX_LEVEL];
int growth_summon_exp_limit[GROWTH_SUMMON_MAX_LEVEL];
int evolve_summon_exp_limit[EVOLVE_SUMMON_MAX_LEVEL];

int	GetMaxWeight( int level, int strength )
{
	// (2007-03-24 ������ ����)
	return ( level + strength ) * 10;
}

XSpinLock block_account_lock;
std::vector< std::string > vBlockedAccount;

void RegisterBlockAccount( const char * szAccount )
{
	THREAD_SYNCRONIZE( block_account_lock );
	vBlockedAccount.push_back( szAccount );

	FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "AutoBlockLog", "Auto-blocked account added: [%s]", szAccount );
}

void DeleteFromBlockAccount( const char * szAccount )
{
	THREAD_SYNCRONIZE( block_account_lock );

	for( std::vector< std::string >::iterator it = vBlockedAccount.begin(); it != vBlockedAccount.end(); ++it )
	{
		if( (*it) == szAccount )
		{
			vBlockedAccount.erase( it );

			FileLogHandler::GetFileLogHandler()->LogStringEx( NULL, "AutoBlockLog", "Auto-blocked account removed: [%s]", szAccount );

			break;
		}
	}
}

bool IsBlockedAccount( const char * szAccount )
{
	THREAD_SYNCRONIZE( block_account_lock );

	for( std::vector< std::string >::iterator it = vBlockedAccount.begin(); it != vBlockedAccount.end(); ++it )
	{
		if( (*it) == szAccount )
			return true;
	}

	return false;
}

// ������ �ֿ� �� �ִ� �Ÿ�
int GetPickableRange()
{
	return 20;
}

// ��޿� ���� ������ ���� ����
int GetItemLevelLimitByRank( int item_rank )
{
	static int _table[] =
	{
		0, 20, 50, 80, 100, 120, 150, 170
	};

	if( item_rank < 1 ) item_rank = 1;
	if( item_rank > 8 ) item_rank = 8;

	return _table[ item_rank-1 ];
}

// ��޺� ������ ���巹�� ���� ���̺�
int GetItemRecommendModTable( int item_rank )
{
	static int _table[] =
	{
		0, 3, 3, 2, 2, 3, 2, 2
	};

	if( item_rank < 1 ) item_rank = 1;
	if( item_rank > 8 ) item_rank = 8;

	return _table[ item_rank-1 ];
}

// ������ ���� ����
int GetItemRecommendLevel( int item_rank, int item_level, int min_item_usable_level )
{
	if( item_rank <= 1 ) return 0;

	return std::min( std::max( GetItemLevelLimitByRank( item_rank ), min_item_usable_level ) + (item_level-1) * GetItemRecommendModTable( item_rank ), GetItemLevelLimitByRank( item_rank + 1 ) );
}

// ������ ���� ������ ���� ���Ƽ ����
c_fixed10 GetItemLevelPenalty( int creature_level, int item_rank, int item_level, int min_item_usable_level )
{
	c_fixed10 A;
	A.set( 10000 );
	c_fixed10 B;
	B.set( 500 );

	c_fixed10 result;
	result.set( 10000 );

	int recommend_level = GetItemRecommendLevel( item_rank, item_level, min_item_usable_level );
	int limit_level		= std::max( GetItemLevelLimitByRank( item_rank ), min_item_usable_level );

	if( item_level		==	1				||
		creature_level	<	limit_level		||
		creature_level	>=	recommend_level	)
	{
		return result;
	}

	if( creature_level < limit_level )
	{
		creature_level = limit_level;
	}

	result = static_cast< c_fixed10 >( recommend_level - creature_level ) / static_cast< c_fixed10 >( recommend_level - limit_level );
	result = result * static_cast< c_fixed10 >( A - B * item_level );
	
	return ( c_fixed10( 1 ) - result );
}

int GetDecreasedEndurancePoint( int previous_endurance, int current_endurance )
{
	return ( ( previous_endurance + 99999 ) / 100000 ) - ( ( current_endurance + 99999 ) / 100000 );
}

const c_fixed10 GetEtherealDurabilityConsumeRate( const int nLevel, const int nJobID, const bool bIsAttack, const int nDamage )
{
	c_fixed10 fConsumeRate;

	// �ƹ�Ÿ ����, ������ ���� ���� ��ġ ���� ���
	fConsumeRate.set( ( ( nLevel * 10 / 3 + 5 ) / 10 * 100 ) + ( ( nDamage * ( bIsAttack ? 1 : 20 ) + 90 ) / 100 * 100 ) );

	// ������ ���� ����/�ǰ� �� �Ҹ��� ����(���� ������ ���ų� �⺻ �����̸� �Ҹ��� �⺻�� ����)
	const JobInfo * pJobInfo = GameContent::GetJobInfo( nJobID );
	if( pJobInfo && pJobInfo->job_depth )
	{
		switch( pJobInfo->job_class )
		{
		// ����迭 ����ġ: ���� �� 45%, �ǰ� �� 5%
		case JobInfo::FIGHTER:	fConsumeRate.set( fConsumeRate.get() + ( ( bIsAttack ) ? 4500 : 500 ) );		break;
		// ���Ͱ迭 ����ġ: ���� �� 20%, �ǰ� �� 15%
		case JobInfo::HUNTER:	fConsumeRate.set( fConsumeRate.get() + ( ( bIsAttack ) ? 2000 : 1500 ) );		break;
		// ������迭 ����ġ: ���� �� 10%, �ǰ� �� 50%
		case JobInfo::MAGICIAN:	fConsumeRate.set( fConsumeRate.get() + ( ( bIsAttack ) ? 1000 : 5000 ) );		break;
		// ��ȯ��迭 ����ġ: ���� �� 25%, �ǰ� �� 25%
		case JobInfo::SUMMONER:	fConsumeRate.set( fConsumeRate.get() + ( ( bIsAttack ) ? 2500 : 2500 ) );		break;
		}
	}

	assert( fConsumeRate.get() % 100 == 0 );

	return fConsumeRate;
}

const c_fixed10 GetEtherealDurabilityConsumeRateByItem( const int nRank, const int nGrade )
{
	c_fixed10 fConsumeRate;

	// ������ ��ũ�� �Ҹ���: (��ũ ^ 2) / 2 * 5%     : / 2 ���� �Ҽ� 1° �ڸ����� �ø�
	// ������ ��޺� �Ҹ���: ��� * 50%
	fConsumeRate.set( ( nRank * nRank * 10 / 2 + 9 ) / 10 * 500 + nGrade * 5000 );

	return fConsumeRate;
}

const int GetEtherealDurabilityBaseConsumption( const bool bIsAttack, /*StructCreature::DamageType*/ const int nDamageType )
{
	int nBaseConsumption = 0;

	if( bIsAttack )
	{
		switch( nDamageType )
		{
		case StructCreature::DT_NORMAL_PHYSICAL_DAMAGE:
		case StructCreature::DT_NORMAL_PHYSICAL_LEFT_HAND_DAMAGE:
			nBaseConsumption = nEtherealDurabilityBaseConsumptionOnNormalAttack;
			break;
		case StructCreature::DT_NORMAL_PHYSICAL_SKILL_DAMAGE:
		case StructCreature::DT_NORMAL_MAGICAL_DAMAGE:
		case StructCreature::DT_STATE_PHYSICAL_DAMAGE:
		case StructCreature::DT_STATE_MAGICAL_DAMAGE:
			nBaseConsumption = nEtherealDurabilityBaseConsumptionOnSkillAttack;
			break;
		}
	}
	else
		nBaseConsumption = nEtherealDurabilityBaseConsumptionOnDamage;

	return nBaseConsumption;
}

// �� ������ ���� ���� ���� (������, �� ����)
const c_fixed10 GetItemValue( c_fixed10 item_current_value, int item_rank_value, int creature_level, int item_rank, int item_level, int min_item_usable_level )
{
	c_fixed10 v = static_cast< c_fixed10 >( item_current_value - item_rank_value ) * GetItemLevelPenalty( creature_level, item_rank, item_level, min_item_usable_level ) + item_rank_value;

	return v;
}

const c_fixed10 GetDonationRewardMoralPoint( const __int64 & nDonateGoldAmount )
{
	c_fixed10 fReward;
	fReward.set( nDonateGoldAmount );

	c_fixed10 fRate;
	fRate.set( DONATE_GOLD_UNIT_COUNT );

	fReward /= fRate;
	return fReward;
}

const StructGold GetItemSellPrice( const StructGold & price, const int rank, const int lv, const bool same_price_for_buying, const bool ethereal_durability_exhausted, const bool is_equipment )
{
	StructGold add_price( 0 );

	// ǥ�ذ��� ������ ��� (1~8��ũ ��� ����)
	StructGold k( price );

	if( rank > 8 )
	{
		assert(false && "8��ũ�� �Ѵ� �������� ������ �����Ϸ� �߼���.: GameRule.cpp:GetItemSellPrice");
		return StructGold( 0 );
	}

	// ���ǰ�� ��� �������� ��ȭ ���� �� ������ ������ش�.
	if( is_equipment )
	{
		// 2013.06.03 - ��ȹ������ ����ϰ� �ִ� ��� ���̺��� �������� ������ �ִ� ���̺��� �޶� ��ȹ�����̺��� ����
		float f[] = {1.35f, 0.2f, 0.115f, 0.092f, 0.085f, 0.1f, 0.1f, 0.1f};		// ��ũ�� �������ݿ� ���� ���̺� (from ��ȹ��- NPC_ItemUp.lua)

		// �� ���� �⺻�� +1�����̹Ƿ� +2�������ʹ� ��ȭ�� ���� �� ���� ������ش�.
		for( int i = 2; i <= lv; i++ )
		{
			if( rank == 0 )			add_price.SetRawData( (__int64)(k.GetRawData() * f[rank] * 0.1f) * 10 );
			else if( rank == 1 )	add_price.SetRawData( (__int64)(k.GetRawData() * f[rank-1] * 0.1f) * 10 );
			else if( rank == 2 )	add_price.SetRawData( (__int64)(k.GetRawData() * f[rank-1] * 0.01f) * 100 );
			else					add_price.SetRawData( (__int64)(k.GetRawData() * f[rank-1] * 0.001f) * 1000 );

			k += add_price;
		}
	}

	// �������� Lv�� ǥ�ذ��� �������� + �߰�����   (Lv1�϶� add_price = 0�� �Ҵ�� ������)
	return ( k.GetRawData() * ( same_price_for_buying ? 1.0f : ITEM_SELL_RATIO ) ) / ( ( ethereal_durability_exhausted ) ? 20 : 1 );
}

bool IsValidName( int code_page, const char * name, int nBufferSize, int nLimitMin, int nLimitMax )
{
	// �����ڵ� ��ȯ �� ���� üũ ���� ���� üũ �켱(Multibyte ��Ʈ������ �����ؾ� ��)
	if( static_cast< int >( strlen( name ) ) < nLimitMin || static_cast< int >( strlen( name ) ) > nLimitMax )
		return false;

	// ���� ���̰� �ִ� ����(�� ����)���� ��� �� ���ڱ����� ����(�� �ڴ� �˻��� �ʿ� ����)
	if( nBufferSize > nLimitMax+1 )
		nBufferSize = nLimitMax+1;

	wchar_t buf[1024];
	
	if( code_page <= 0 )
	{
		code_page = ENV().GetInt( "CodePage", CP_ACP );
	}

	MultiByteToWideChar( code_page, 0, name, nBufferSize, buf, 1024 );

	wchar_t* c = buf;

	int cnt = 0;

	// �̸��� ���Ǵ� �� �� ������ �����ؾ� �ϴ� ������ ���� ������ ���ڵ��� ��Ʈ������ üũ
	enum APPEARED_LANGUAGE
	{
		APPEARED_ENGLISH		= 1 << 0,
		APPEARED_NATIVE			= 1 << 1,
	};
	int nAppeared = 0;

	for( int i=0; ; i++,c++,cnt++ )
	{
		// ��� ������ �̸�(2Byte ���ڴ� �Ʒ��� ���ǹ����� cnt ���� �� �������� �� �����ϹǷ� 2�� ����)
		if( *c == L'\0' )
		{ 
			// ���� �� �� �� üũ
			if( cnt < nLimitMin || cnt > nLimitMax )
				return false;
			else
				break;
		}

		// ����, ������ ���� pass
		if( L'0' <= *c && *c <= L'9' )
			continue;

		if( ( L'a' <= *c && *c <= L'z' ) || ( L'A' <= *c && *c <= L'Z' ) )
		{
			nAppeared |= APPEARED_ENGLISH;
			continue;
		}
		
		// Modifié par Sn00pyX le 20/09/2026
		// Caractères autorisés dans les noms
		if( *c == L' ' || *c == L'-' || *c == L'_' )
		{
    		continue;
		}

		nAppeared |= APPEARED_NATIVE;

		if( code_page == CP_HONGKONG ) // ȫ��� (BIG5)
		{
			if( 0x2E80 <= *c && *c <= 0x2EF3 ) { cnt++; continue; }		// CJK Radicals Supplement
			if( 0x2F00 <= *c && *c <= 0x2FD5 ) { cnt++; continue; }		// Kangxi Radicals
			if( 0x3105 <= *c && *c <= 0x312C ) { cnt++; continue; }		// Bopomofo
			if( 0x31A0 <= *c && *c <= 0x31B7 ) { cnt++; continue; }		// Bopomofo Extended
			if( 0x3400 <= *c && *c <= 0x4DB5 ) { cnt++; continue; }		// CJK Ideographs Ext. A
			if( 0x4E00 <= *c && *c <= 0x9FBB ) { cnt++; continue; }		// Unified CJK Ideographs
			if( 0xF900 <= *c && *c <= 0xFAD9 ) { cnt++; continue; }		// CJK Compatibility Ideographs
		}
		else if( code_page == CP_JAPAN )	// �Ϻ��� (Shift-JIS)
		{
			if( 0x2E80 <= *c && *c <= 0x2EF3 ) { cnt++; continue; }		// CJK Radicals Supplement
			if( 0x2F00 <= *c && *c <= 0x2FD5 ) { cnt++; continue; }		// Kangxi Radicals
			if( 0x3041 <= *c && *c <= 0x3093 ) { cnt++; continue; }		// Hiragana
			if( 0x30A1 <= *c && *c <= 0x30FA ) { cnt++; continue; }		// Katakana
			if( 0x31F0 <= *c && *c <= 0x31FF ) { cnt++; continue; }		// Katakana Phonetic Ext.
			if( 0x3005 == *c ) { cnt++; continue; }						// Ideographic iteration mark
			if( 0x30FC == *c ) { cnt++; continue; }						// Hiragana, Katakana Prolonged sound mark
			if( 0x30FB == *c ) { cnt++; continue; }						// Katakana Middle Dot
			if( 0x3400 <= *c && *c <= 0x4DB5 ) { cnt++; continue; }		// CJK Ideographs Ext. A
			if( 0x4E00 <= *c && *c <= 0x9FBB ) { cnt++; continue; }		// Unified CJK Ideographs
			if( 0xF900 <= *c && *c <= 0xFAD9 ) { cnt++; continue; }		// CJK Compatibility Ideographs
		}
		else if( code_page == CP_CHINA ) // �߱��� (GB2312 - Chinese Simplified)
		{
			if( 0x2E80 <= *c && *c <= 0x2EF3 ) { cnt++; continue; }		// CJK Radicals Supplement
			if( 0x2F00 <= *c && *c <= 0x2FD5 ) { cnt++; continue; }		// Kangxi Radicals
			if( 0x3105 <= *c && *c <= 0x312C ) { cnt++; continue; }		// Bopomofo
			if( 0x31A0 <= *c && *c <= 0x31B7 ) { cnt++; continue; }		// Bopomofo Extended
			if( 0x3400 <= *c && *c <= 0x4DB5 ) { cnt++; continue; }		// CJK Ideographs Ext. A
			if( 0x4E00 <= *c && *c <= 0x9FBB ) { cnt++; continue; }		// Unified CJK Ideographs
			if( 0xF900 <= *c && *c <= 0xFAD9 ) { cnt++; continue; }		// CJK Compatibility Ideographs
		}
		else if( code_page == CP_RUSSIA ) // ���þƿ�
		{
			if( ( 0x0401 <= *c && *c <= 0x040C ) ||
				( 0x040E <= *c && *c <= 0x040F ) ||
				( 0x0410 <= *c && *c <= 0x044F ) ||
				( 0x0451 <= *c && *c <= 0x045C ) ||
				( 0x045E <= *c && *c <= 0x045F ) ||
				( 0x0490 <= *c && *c <= 0x0491 ) )
			{
				// �ش� ������ Codepage�� ���ڵ��ϸ� 1 Byte �����̹Ƿ� cnt�� ������Ű�� �� ��.
				continue;
			}
		}
		else if( code_page == CP_WEST_EUROPE ) // �������� ( ANSI - Latin I / West European Latin )
		{
			if( 0x00C0 <= *c && *c <= 0x00FF )		// C1 Controls and Latin-1 Supplement )
			{
				// �ش� ������ Codepage�� ���ڵ��ϸ� 1 Byte �����̹Ƿ� cnt�� ������Ű�� �� ��.
				continue;
			}
		}
		else if( code_page == CP_MIDEAST ) // �ߵ��� (Arabic - Windows)
		{
			if( ( 0x0621 <= *c && *c <= 0x063A ) ||
				( 0x0641 <= *c && *c <= 0x064A ) ||		// Based on ISO 8859-6
				( 0x0671 <= *c && *c <= 0x0678 ) ||		// Extended Arabic Letters
				( 0x061B <= *c && *c <= 0x061F ) ||
				( 0x066A <= *c && *c <= 0x066D ) ||
				0x060C == *c || 0x060D == *c )			// Punctuation
			{
				// �ش� ������ Codepage�� ���ڵ��ϸ� 1 Byte �����̹Ƿ� cnt�� ������Ű�� �� ��.
				continue;
			}
		}
		else if( code_page == CP_TURKEY )
		{
			if( *c == 0x0130 || *c == 0x0131 ||
				*c == 0x011E || *c == 0x011F ||
				*c == 0x015E || *c == 0x015F ||
				*c == 0x00D6 || *c == 0x00DC ||
				*c == 0x00F6 || *c == 0x00FC ||
				*c == 0x00C7 || *c == 0x00E7 )
			{
				// �ش� ������ Codepage�� ���ڵ��ϸ� 1 Byte �����̹Ƿ� cnt�� ������Ű�� �� ��.
				continue;
			}

		}
		else if( code_page == CP_THAILAND )
		{
			if( ( 0x0E01 <= *c && *c <= 0x0E0D ) ||
				( 0x0E0F <= *c && *c <= 0x0E59 ) ||
				*c == L'_' )
			{
				// �ش� ������ Codepage�� ���ڵ��ϸ� 1 Byte �����̹Ƿ� cnt�� ������Ű�� �� ��.
				continue;
			}
		}
		else if( code_page == CP_CENTRAL_EUROPE )
		{
			if( ( 0x0104 <= *c && *c <= 0x0107 ) ||
				( 0x0118 <= *c && *c <= 0x0119 ) ||
				( 0x0141 <= *c && *c <= 0x0144 ) ||
				( 0x015A <= *c && *c <= 0x015B ) ||
				( 0x0179 <= *c && *c <= 0x017C ) )
			{
				// �ش� ������ Codepage�� ���ڵ��ϸ� 1 Byte �����̹Ƿ� cnt�� ������Ű�� �� ��.
				continue;
			}

			if( ( 0x00D3 == *c ) ||
				( 0x00F3 == *c ) )
			{
				// �ش� ������ Codepage�� ���ڵ��ϸ� 1 Byte �����̹Ƿ� cnt�� ������Ű�� �� ��.
				continue;
			}
		}
		else // ������
		{
			//if( 0x1100 <= *c && *c < 0x115A ) { cnt++; continue; }
			//if( 0x115F <= *c && *c < 0x11A3 ) { cnt++; continue; }
			//if( 0x11A8 <= *c && *c < 0x11FA ) { cnt++; continue; }
			//if( 0x302E <= *c && *c < 0x3030 ) { cnt++; continue; }
			//if( 0x3131 <= *c && *c < 0x318F ) { cnt++; continue; }
			if( 0xAC00 <= *c && *c < 0xD7A4 ) { cnt++; continue; }
		}

		return false;
	}

	// �̸��� ���Ǵ� �� �� ������ ����
	switch( code_page )
	{
	case CP_RUSSIA:
		if( nAppeared == ( APPEARED_ENGLISH | APPEARED_NATIVE ) )
			return false;
	}

	return true;
}


// Modifie par Sn00pyX le 20/09/2026
// Ne modifie pas la casse du nom. Les majuscules/minuscules saisies par le joueur sont conservées.
bool ReformatName( char * name )
{

    const int nNameLength = static_cast< int >( strlen( name ) );

    if ( nNameLength < 1 )
        return true;

    return true;
}

bool IsValidPartyName( const char * name, int nBufferSize, int nLimitMin, int nLimitMax )
{
	if( nBufferSize > nLimitMax+1 ){	nBufferSize = nLimitMax+1; }
	
	unsigned char* c = (unsigned char*)name;

	for( int i=0; i<=nBufferSize; i++,c++ )
	{
		// ��밡���� �̸� 
		if( *c == 0 ) {
			if( i < nLimitMin ) { return false; }	else { return true; }
		}

		// ����, ������ ���� pass
		if((*c >= '0' && *c <= '9') || ( *c >= 'a' && *c <= 'z' ) || ( *c >= 'A' && *c <= 'Z' ) || ( *c == ' ' ) ) 
		{
			continue;
		}
		
		// ù�ڵ尡 �ѱ��̴�.
		if( *c >= 0xb0 && *c <= 0xc8 )
		{
			if( (i+1) >= nBufferSize ) { return false; }
			c++;i++;
			
			// �ι�° �ڵ嵵 �ѱ��̸� pass
			if( *c >= 0xa1 && *c <= 0xfe ) { continue; }
		}

		// ����, ����, �ѱ��� �ƴϹǷ� ���Ұ�.
		return false;
	}
	// ���ѱ��̸� �Ѿ �̸��̹Ƿ� ���Ұ�.
	return true;	
}

float GetStaminaRatio( int level )
{
	if( level < 1 )
		level = 1;

	if( level > MAX_LEVEL )
		level = MAX_LEVEL;

	if( !stamina_ratio[level-1] )
	{
		// 2006-11-09 ������ ������ ���� ������Ʈ�� ����
		//stamina_ratio[level-1] = (int) ( pow( (float) (level + 2), 1.8f ) * 2 - pow( (float) (level + 2) , 1.75f ) * 2 + 2 ) * 0.00055;
		stamina_ratio[level-1] = (int) ( level * 2.4 + pow( (float) (level), 1.46f ) + ( pow( (float) (level), 2 ) * 0.1f ) + 2 ) * 0.00055;
	}
	return stamina_ratio[level-1];

}

int GetSummonEXPLimit( int level )
{
	return (int)( pow( level, 2.0 ) * 200 );
}

int GetPlayerEXPLimit( int level )
{
	if( !player_exp_limit[level-1] )
	{
		// 2006-12-11 ���� ����
		//player_exp_limit[level-1] = (int)( pow( (float) level,  1.8f ) * 5.0f ) + 40;
		player_exp_limit[level-1] = (int)( pow( (float) level,  1.8f ) * 30.0f ) + 240;

		player_exp_limit[level-1] += (int)( player_exp_limit[level-1] * 0.1 * (level / 100) );
	}
	
	return player_exp_limit[level-1];
}

float GetSummonLevelPenalty( int master_level, int summon_level )
{
	int level_diff = summon_level - master_level;

	if( level_diff > 0 )
	{
		if( level_diff >= 30 )
		{
			return 10.0f;
		}

		return (int) ( level_diff * 0.25f * ( 2.34f - ( level_diff / 30.0f ) ) * 10.0f ) / 10.0f;
	}

	return level_diff;
}

float GetSummonStatPenalty( int master_level, int summon_level )
{
	int level_diff = summon_level - master_level;

	if( level_diff > 0 )
	{
		if( level_diff >= 50 )
		{
			return 0.7f;
		}

		return ( (int) ( ( 50.0f - level_diff ) * 0.6f ) + 70.0f ) / 100.0f;
	}

	return 1.0f;
}

int AppendOnetimePassword( char * pBuf, size_t buf_len, int one_time_key, int nSID, int nAccountID )
{
	struct TempStructForEncode
	{
		int nSeed;
		int n1;
		int n2;
		int n3;
		int nChecksum;
	} temp;

	temp.nSeed = 384723432;
	temp.n1 = one_time_key;
	temp.n2 = nSID;
	temp.n3 = nAccountID;
	temp.nChecksum = one_time_key + nSID + nAccountID;

	temp.n1 ^= temp.nSeed;
	temp.n2 ^= temp.n1;
	temp.n3 ^= temp.n2;
	temp.nChecksum ^= temp.n3;

	temp.n1 ^= 0xD8FB51A9;
	temp.n2 ^= 0x9DC720AC;
	temp.n3 ^= 0x31F42CB7;
	temp.nChecksum ^= 0x7F9B3D2E;

	s_sprintf( pBuf, buf_len, "%08X%08X%08X%08X%08X", temp.nSeed, temp.n1, temp.n2, temp.n3, temp.nChecksum );
	return (int)strlen( pBuf );
}

c_fixed10 GetGameTimeLimitPenalty( AR_TIME continuous_play_time )
{
	c_fixed10 fGameTimeLimitPenalty = 0;

	if( !bLimitGameTime )
		fGameTimeLimitPenalty.set( 10000 );

	if( continuous_play_time < nMaxHealthyGameTime )
	{
		fGameTimeLimitPenalty.set( 10000 );
	}
	else if( continuous_play_time < nMaxTiredGameTime )
	{
		fGameTimeLimitPenalty.set( 5000 );
	}

	return fGameTimeLimitPenalty;
}

const int GetPetShovelingRewardStateCode()
{
	static const int sRewardStateTable[ 2 ][ 4 ] = {
	StructState::PET_SHOVELING_REWARD_INC_MOVE_SPEED, StructState::PET_SHOVELING_REWARD_INC_STR_INT, StructState::PET_SHOVELING_REWARD_INC_AGI_DEX, StructState::PET_SHOVELING_REWARD_INC_VIT,
	StructState::PET_SHOVELING_REWARD_DEC_MOVE_SPEED, StructState::PET_SHOVELING_REWARD_DEC_STR_INT, StructState::PET_SHOVELING_REWARD_DEC_AGI_DEX, StructState::PET_SHOVELING_REWARD_DEC_VIT };
	
	return sRewardStateTable[ XRandom( 0, 9 ) >= 7 ][ XRandom( 0, 3 ) ];
}

const c_fixed10 GetDifficultyBonus( const unsigned char nDifficulty )
{
	static const c_fixed10 sDifficultyBonusTable[] =
	{
		c_fixed10( 1.0 ),
		c_fixed10( 1.5 ),
		c_fixed10( 2.0 )
	};

	assert( nDifficulty < sizeof( sDifficultyBonusTable ) / sizeof( c_fixed10 ) );

	return sDifficultyBonusTable[ nDifficulty ];
}

int GetBattleArenaTeamNameStringID( int nTeamNo, bool bWithColorTag )
{
	static const int sTeamName[ 2 ][ BATTLE_ARENA_MAX_TEAM_COUNT ] =
	{
		{
			2382,	// "���ձ�"
			2383	// "���౺"
		},
		{
			2494,	// <#00aeef>'���ձ�'
			2495,	// <#f26522>'���౺'
		}
	};

	if( nTeamNo < 0 || nTeamNo >= BATTLE_ARENA_MAX_TEAM_COUNT )
	{
		assert( 0 );
		return 88;		// "�����ΰ�" �� String ID
	}

	return sTeamName[ ( !bWithColorTag ) ? 0 : 1 ][ nTeamNo ];
}

time_t GetBattleArenaBlockDuration( int nPenaltyCount )
{
	static const time_t sBlockDuration[] = {
		60 * 8,
		60 * 16,
		60 * 60,
		60 * 120,
		60 * 240,
		60 * 480,
		60 * 600
	};

	// ���Ƽ Ƚ���� 0�̸� ���� ���� ���Ƽ�� ����
	if( nPenaltyCount <= 0 )
		return 0;
	else if( nPenaltyCount > _countof( sBlockDuration ) )
		nPenaltyCount = _countof( sBlockDuration );

	// ���Ƽ Ƚ�� 0�� ���� ���� ����, 1���� ���Ƽ�� �ο��Ǵ� ���̹Ƿ� nPenaltyCount - 1 ��° 
	return sBlockDuration[ nPenaltyCount - 1 ];
}

time_t GetBattleArenaPenaltyDuration( int nPenaltyCount )
{
	static const time_t sPenaltyDuration[] = {
		60 * 60,
		60 * 60,
		60 * 120,
		60 * 240,
		60 * 480,
		60 * 960,
		60 * 1200
	};

	// ���Ƽ Ƚ���� 0�̸� ���Ƽ ���� �Ⱓ�� ����
	if( nPenaltyCount <= 0 )
		return 0;
	else if( nPenaltyCount > _countof( sPenaltyDuration ) )
		nPenaltyCount = _countof( sPenaltyDuration );

	// ���Ƽ Ƚ�� 0�� ���Ƽ ���� �Ⱓ ����, 1���� �Ⱓ�� �ο��Ǵ� ���̹Ƿ� nPenaltyCount - 1 ��° 
	return sPenaltyDuration[ nPenaltyCount - 1 ];
}

};
// End of namespace GameRule
