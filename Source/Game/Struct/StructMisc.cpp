
#include <mmo/ArTime.h>

#include "DB_Commands.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructMisc.h"
#include "GameContent.h"


// 인자가 너무 많음. 구조체를 인자로 받게 하든 뭘 하든 조치가 필요!
StructState::StructState( StateCode code, int uid, AR_HANDLE caster, unsigned short level, AR_TIME start_time, AR_TIME end_time, int base_damage, bool bIsAura, int nStateValue, const char * szStateValue, const bool bByEvent, int nDBEnable, const bool bHideTime, const bool bInvisible )
{
	init( uid, code );

	m_nCode	= code;

	m_nLevel = level;
	m_hCaster = caster;
	m_nBaseDamage = base_damage;
	m_nStartTime = start_time;
	m_nEndTime = end_time;
	m_nLostDurationByLoading = 0;
	m_nRemainDuration = 0;
	m_bAura = bIsAura;
	m_nLastProcessedTime = GetArTime();

	m_nTotalDamage = 0;

	m_nStateValue = nStateValue;

	s_strcpy( m_szStateValue, _countof( m_szStateValue ), szStateValue );

	m_bByEvent = bByEvent;

	m_nDBEnable = nDBEnable;
}

StructState::StructState()
{
	init( 0, 0 );
}

void StructState::Save( struct StructCreature * pCreature, const int nEnable )
{
	// UID가 할당되어있지 않다면 저장을 할 수 없으며 필요도 없는 경우이다.
	if( !IsSavable() ) return;

	SetEnable( nEnable );

	if( pCreature->IsPlayer() )
		static_cast< StructPlayer * >( pCreature )->DBQuery( new DB_UpdateState( pCreature, this ) );
	else if( pCreature->IsSummon() )
		static_cast< StructSummon * >( pCreature )->DBQuery( new DB_UpdateState( pCreature, this ) );

	SetDBEnable( nEnable );
}

void StructState::SaveByDead( struct StructCreature * pCreature )
{
	// 저장이 되지 않는 모든 지속효과도 복구될 수는 있어야 한다.
	// 함수 이름과 같이 사망 시에 지워지는 지속효과를 복구하기 위한 용도로 저장을 하는 함수이므로 애초에 사망 시에 지워지지 않는 지속효과는 호출되지 않는다.
	// 따라서 중첩 부여는 우려하지 않아도 된다.
	if( !IsSavable() )
		SetUID( StructCreature::AllocStateUID( m_nDBEnable ) );

	Save( pCreature, USED_IN_DB_BY_DEAD );
}

void StructState::Expire( struct StructCreature * pCreature )
{
	// 이하는 DB에 저장과 관련된 작업이다. 만약 DB에 저장된 적이 없다면 어떠한 처리도 필요하지 않다.
	if( !IsInDB() ) return;

	// 사용하고 있지 않던 지속효과 데이터를 재활용 하였으며 그 지속효과가 만료되었다면 어떠한 저장 작업도 필요없이 UID만 반환을 하면 된다.
	// DB에는 여전히 재활용 가능한 데이터로 존재를 하며 게임 엔진에서는 지속효과가 사라졌기 때문이다.
	if( IsReusable() )
	{
		StructCreature::DeallocStateUID( GetUID() );
		return;
	}

	// 저장 작업을 하더라도 UID 반환은 필요하지만 여기서 하지 않는 이유는 DB 작업이 완료되기 전에 UID를 반환하게 되면,
	// 반환된 UID를 다른 플레이어나 소환수가 사용하게 되어 하나의 지속효과에 두 개 이상의 DB 큐가 존재하게 된다. 결과적으로 DB 작업의 작업 순서가 지켜지지 않게 된다.
	// 따라서 DB 작업이 완료된 후에 UID를 반환하도록 한다.
	Save( pCreature, UNUSED_IN_DB );
}

void StructState::init( int uid, int code )
{
	if( code )
	{
		m_nUID = uid;
		m_nCode = static_cast< StructState::StateCode >( code );
		m_pInfo = GameContent::GetStateInfo( code );

		static StateInfo _inst;
		if( !m_pInfo )
		{
			assert( 0 );
			m_pInfo = &_inst;
		}
	}
	else
	{
		m_pInfo = NULL;
		m_nUID = 0;
		m_nCode = static_cast< StructState::StateCode >( 0 );
	}

	m_nLevel = 0;
	m_hCaster = 0;
	m_nBaseDamage = 0;
	m_nStartTime = 0;
	m_nEndTime = 0;
	m_nLostDurationByLoading = 0;
	m_nRemainDuration = 0;
	m_bAura = false;
	m_nLastProcessedTime = 0;

	m_nTotalDamage = 0;

	m_nStateValue = 0;

	memset( m_szStateValue, 0, sizeof( m_szStateValue ) );

	m_nEnable = 1;

	m_nLastSavedTime = GetArTime();

	m_bByEvent = false;
}

bool StructState::IsHarmful() const
{
	return !!m_pInfo->is_harmful;
}

void StructState::HoldRemainDuration()
{
	if( IsHolded() || IsAura() )
		return;

	AR_TIME t = GetArTime();

	m_nRemainDuration = m_nEndTime - t;
	m_nEndTime = -1;

	m_bAura = true;
}

void StructState::ReleaseRemainDuration()
{
	if( !IsHolded() )
		return;

	AR_TIME t = GetArTime();

	m_nEndTime = t + m_nRemainDuration;
	m_nRemainDuration = 0;

	m_bAura = false;
}

bool StructState::AddState( AR_HANDLE caster, unsigned short level, AR_TIME start_time, AR_TIME end_time, int base_damage, bool bIsAura )
{
	if( m_nLevel > level ) return false;

	m_nStartTime = start_time;
	m_nEndTime = end_time;
	if( !bIsAura ) m_nLastProcessedTime = start_time;

	if( m_nLevel == level && m_nBaseDamage == base_damage && m_hCaster == caster && m_bAura == bIsAura )
		return false;

	m_nLevel = level;
	m_nBaseDamage = base_damage;
	m_hCaster = caster;
	m_bAura = bIsAura;

	return true;
}

c_fixed10 StructState::GetValue( int idx ) const
{
	return m_pInfo->fValue[idx];
}

int StructState::GetEffectType() const
{
	return m_pInfo->effect_type;
}

int StructState::GetStateGroup() const
{
	return m_pInfo->state_group;
}

bool StructState::IsDuplicatedGroup( int nGroupID ) const 
{
	return ( ( m_pInfo->duplicate_group[0] && m_pInfo->duplicate_group[0] == nGroupID )
		|| ( m_pInfo->duplicate_group[1] && m_pInfo->duplicate_group[1] == nGroupID )
		|| ( m_pInfo->duplicate_group[2] && m_pInfo->duplicate_group[2] == nGroupID ) );
}

bool StructState::ClearExpiredState( AR_TIME t )
{
	if( m_bByEvent )
		return false;

	if( m_nLevel && m_nEndTime < t )
	{
		return true;
	}

	return false;
}

void StructState::SetState( StateCode code, int uid, AR_HANDLE caster, unsigned short level, AR_TIME duration, AR_TIME remain_time, AR_TIME last_fire_time, int base_damage, int state_value, const char * szStateValue, int nEnable )
{
	init( uid, code );

	m_nCode	= code;

	AR_TIME t = GetArTime();

	m_nLevel = level;
	m_hCaster = caster;
	m_nBaseDamage = base_damage;

	if( remain_time == (AR_TIME)-1 )
	{
		// duration 값은 영구히 걸려 있는 버프(오오라)의 경우에는 사용되지 않음
		m_nStartTime = t;
		m_nEndTime = -1;
		m_bAura = true;
	}
	else
	{
		assert( duration > 0 );

		if( duration )
		{
			// 버프가 최초로 걸린 후부터 경과한 시간이 서버가 시작되어 경과한 시간보다 길다면(섭다 직후 로그인의 경우 등)
			// m_nStartTime이 음수가 될 수는 없으므로 0 으로 세팅하고, 유실된 부분을 m_nLostDurationByLoading에 보관했다가
			// DB에 저장할 때만 다시 duration을 계산할 때 더해줌으로써 결과적으로 DB에 저장되어 있는 duration값은 변하지 않도록 함
			// * 이 증상은 게임 서버 엔진을 재시작할 때마다 AR_TIME 단위의 시간값이 0부터 다시 시작하기 때문에 발생함
			// * 이 경우는 서버 엔진 재시작 이전에 DB에 저장된 지속효과를 로드해서 SetState 함수가 호출되었을 때만 발생함
			// * 이렇게 처리해도 클라이언트에 지속효과 관련 방송 패킷에서는 StartTime/EndTime을 보내기 때문에
			//   전체 기간 대비 경과한 시간 표시(지속효과 아이콘에 시계방향으로 도는 음영 처리)는 부정확해 짐.
			//   이걸 해결하려면 클라이언트에 보내는 지속효과의 정보를 StartTime/EndTime이 아니라 Duration/RemainTime으로 변경해야 함.
			//   (작업 대비 얻을 수 있는 게 별 의미가 없으므로 패스~)
			if( t > ( duration - remain_time ) )
			{
				m_nStartTime = t - ( duration - remain_time );
			}
			else
			{
				m_nLostDurationByLoading = ( duration - remain_time ) - t;
				m_nStartTime = 0;
			}
			m_nEndTime = t + remain_time;
		}
		else
		{
			m_nStartTime = m_nEndTime = 0;
		}
	}

	m_nLastProcessedTime = last_fire_time;

	m_nStateValue = state_value;

	m_nEnable = nEnable;
	m_nDBEnable = nEnable;

	s_strcpy( m_szStateValue, _countof( m_szStateValue ), szStateValue );

	m_bByEvent = false;
}

int StructState::GetTimeType() const
{
	return m_pInfo->state_time_type;
}