
#include <string>

#include <openssl/md5.h>

#include <dump/XException.h>
#include <toolkit/khash.h>
#include <mmo/ArcadiaServer.h>
#include <toolkit/XConsole.h>
#include <toolkit/XStringUtil.h>
#include <toolkit/XEnv.h>
#include <logging/FileLog.h>
#include <geometry/X2DQuadTree.h>
#include <geometry/X2DQuadTree_divide_polygon.h>
#include <geometry/X2DPathFinder.h>
#include <toolkit/XDirectoryScanner.h>

#include "LogClient/LogClient.h"
#include "ErrorCode/ErrorCode.h"

#include "GameContent.h"
#include "BitmapBox.h"
#include "StructItem.h"
#include "LuaVM.h"
#include "DungeonManager.h"
#include "StructNPC.h"
#include "StructPlayer.h"
#include "StructSummon.h"
#include "StructSkill.h"
#include "GameProc.h"
#include "Extern.h"
#include "ChannelManager.h"
#include "RoamingManager.h"
#include "NPCProc.h"
#include "TimeUtil.h"


struct _EXP_TABLE
{
	__int64 exp;
	__int64 jp[4];
};

struct _SUMMON_EXP_TABLE
{
	__int64		normal_exp;
	__int64		growth_exp;
	__int64		evolve_exp;
};

struct RANDOM_POOL
{
	RANDOM_POOL( int _group_id ) : group_id( _group_id ) {}

	int group_id;

	std::vector< GameContent::RANDOM_POOL_INFO > vInfo;
};

typedef struct _QUEST_TEXT_ID {
	_QUEST_TEXT_ID( const int _nStartTextId, const int _nInProgressTextId, const int _nEndTextId )
		: nStartTextId( _nStartTextId )
		, nInProgressTextId( _nInProgressTextId )
		, nEndTextId( _nEndTextId )
	{}

	int nStartTextId;
	int nInProgressTextId;
	int nEndTextId;
} QUEST_TEXT_ID;

const bool GameContent::EVENT_AREA_INFO::IsActivatable( const struct StructPlayer * pPlayer, const int nAreaIndex ) const
{
	int nCurrentTimeInToday = static_cast< int >( time( NULL ) - GetDayBeginTime() );
	if( ( m_nBeginTime && m_nBeginTime > nCurrentTimeInToday ) ||
		( m_nEndTime && m_nEndTime < nCurrentTimeInToday ) )
	{
		return false;
	}

	if( ( m_nMinLevel && m_nMinLevel > pPlayer->GetLevel() ) ||
		( m_nMaxLevel && m_nMaxLevel < pPlayer->GetLevel() ) )
	{
		return false;
	}

	// 종족 제한이 있는 경우
	if( m_nRaceJobLimit & RACE_JOB_FLAG_ALL_RACES )
	{
		switch( pPlayer->GetRace() )
		{
		case JobInfo::GAIA:		if( !( m_nRaceJobLimit & RACE_JOB_FLAG_RACE_GAIA	) ) return false; break;
		case JobInfo::DEVA:		if( !( m_nRaceJobLimit & RACE_JOB_FLAG_RACE_DEVA	) ) return false; break;
		case JobInfo::ASURA:	if( !( m_nRaceJobLimit & RACE_JOB_FLAG_RACE_ASURA	) ) return false; break;
		}
	}

	// 직업 제한이 있는 경우(종족 플래그를 제외한 모든 비트에 1개라도 값이 있으면 직업 제한으로 간주)
	if( m_nRaceJobLimit & ~RACE_JOB_FLAG_ALL_RACES )
	{
		switch( pPlayer->GetJobId() )
		{
		// 가이아 직업들
		case 100:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_ROGUE			) ) return false; break;
		case 101:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_FIGHTER			) ) return false; break;
		case 102:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_KAHUNA			) ) return false; break;
		case 103:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_SPELL_SINGER		) ) return false; break;
		case 110:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_CHAMPION			) ) return false; break;
		case 111:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_ARCHER			) ) return false; break;
		case 112:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_DRUID			) ) return false; break;
		case 113:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_BATTLE_KAHUNA	) ) return false; break;
		case 114:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_EVOKER			) ) return false; break;
		case 120:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_BERSERKER		) ) return false; break;
		case 121:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_MASTER_ARCHER	) ) return false; break;
		case 122:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_HIGH_DRUID		) ) return false; break;
		case 123:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_GREAT_KAHUNA		) ) return false; break;
		case 124:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_BEAST_MASTER		) ) return false; break;

		// 데바 직업들
		case 200:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_GUIDE			) ) return false; break;
		case 201:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_HOLY_WARRIOR		) ) return false; break;
		case 202:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_CLERIC			) ) return false; break;
		case 203:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_TAMER			) ) return false; break;
		case 210:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_KNIGHT			) ) return false; break;
		case 211:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_SOLDIER			) ) return false; break;
		case 212:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_MAGE				) ) return false; break;
		case 213:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_PRIEST			) ) return false; break;
		case 214:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_BREEDER			) ) return false; break;
		case 220:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_CRUSADER			) ) return false; break;
		case 221:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_BLADER			) ) return false; break;
		case 222:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_ARCH_MAGE		) ) return false; break;
		case 223:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_HIGH_PRIEST		) ) return false; break;
		case 224:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_SOUL_BREEDER		) ) return false; break;

		// 아수라 직업들
		case 300:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_STEPPER			) ) return false; break;
		case 301:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_STRIDER			) ) return false; break;
		case 302:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_MAGICIAN			) ) return false; break;
		case 303:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_SUMMONER			) ) return false; break;
		case 310:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_ASSASSIN			) ) return false; break;
		case 311:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_RANGER			) ) return false; break;
		case 312:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_SORCERER			) ) return false; break;
		case 313:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_DARK_MAGICIAN	) ) return false; break;
		case 314:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_BATTLE_SUMMONER	) ) return false; break;
		case 320:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_SHADOW_CHASER	) ) return false; break;
		case 321:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_SHADOW_HUNTER	) ) return false; break;
		case 322:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_LICH				) ) return false; break;
		case 323:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_WARLOCK			) ) return false; break;
		case 324:				if( !( m_nRaceJobLimit & RACE_JOB_FLAG_JOB_NECROMANCER		) ) return false; break;
		}
	}

	// 발동 조건 관련 체크
	for( int i = 0 ; i < MAX_ACTIVATE_CONDITION ; ++i )
	{
		switch( m_nActivateCondition[ i ] )
		{
		case LIMIT_CONDITION_ITEM_COUNT_GE:
			{
				StructItem * pItem = pPlayer->FindItem( static_cast< ItemBase::ItemCode >( m_nActivateValue[ i ][ 0 ] ) );
				if( !pItem || pItem->GetCount() < m_nActivateValue[ i ][ 1 ] )
					return false;
			}
			break;
		case LIMIT_CONDITION_QUEST_STATUS:
			if( pPlayer->GetQuestProgress( m_nActivateValue[ i ][ 0 ] ) != m_nActivateValue[ i ][ 1 ] )
				return false;
			break;
		case LIMIT_CONDITION_SKILL_LEVEL_GE:
			{
				StructSkill * pSkill = pPlayer->GetSkill( m_nActivateValue[ i ][ 0 ] );
				if( !pSkill || pSkill->GetBaseSkillLevel() < m_nActivateValue[ i ][ 1 ] )
					return false;
			}
			break;
		case LIMIT_CONDITION_ITEM_WEARING:
			{
				bool bWearing = false;
				for( int nWearIdx = 0 ; nWearIdx < ItemBase::MAX_ITEM_WEAR ; ++nWearIdx )
				{
					StructItem * pItem = pPlayer->GetWearedItem( static_cast< ItemBase::ItemWearType >( nWearIdx ) );
					if( pItem && pItem->GetItemCode() == m_nActivateValue[ i ][ 0 ] )
					{
						bWearing = true;
						break;
					}
				}

				// m_nActivateValue[ i ][ 1 ] == 1 이면 착용 시 발동, 2 이면 미착용 시 발동
				if( !( ( m_nActivateValue[ i ][ 1 ] == 1 && bWearing ) || ( m_nActivateValue[ i ][ 1 ] == 2 && !bWearing ) ) )
					return false;
			}
			break;
		case LIMIT_CONDITION_SUMMON_OWNING:
			{
				bool bOwning = false;

				if( ( pPlayer->GetMainSummon() && pPlayer->GetMainSummon()->GetSummonCode() == m_nActivateValue[ i ][ 0 ] ) ||
					( pPlayer->GetSubSummon() && pPlayer->GetSubSummon()->GetSummonCode() == m_nActivateValue[ i ][ 0 ] ) )
					bOwning = true;

				// m_nActivateValue[ i ][ 1 ] == 1 이면 보유 시 발동, 2 이면 미보유 시 발동
				if( !( ( m_nActivateValue[ i ][ 1 ] == 1 && bOwning ) || ( m_nActivateValue[ i ][ 1 ] == 2 && !bOwning ) ) )
					return false;
			}
			break;
		case LIMIT_CONDITION_STATE:
			{
				bool bStateOwning = pPlayer->GetState( static_cast< StructState::StateCode >( m_nActivateValue[ i ][ 0 ] ) );
			
				// m_nActivateValue[ i ][ 1 ] == 1 이면 보유 시 발동, 2 이면 미보유 시 발동
				if( !( ( m_nActivateValue[ i ][ 1 ] == 1 && bStateOwning ) || ( m_nActivateValue[ i ][ 1 ] == 2 && !bStateOwning ) ) )
					return false;
			}
			break;

		default:
			break;
		}
	}

	// 좌표 기반 인접 여부 체크
	ArPosition posCurrent = pPlayer->GetCurrentPosition( GetArTime() );

	X2D::Polygon< int > * pArea = m_vArea[ nAreaIndex ];

	// 정밀 포함 체크 만족하면 통과(폴리곤을 포함하는 최소 사각형 체크 -> 가로선 긋고 각 엣지와 교점 수 체크 알고리즘으로 포함관계 체크)
	if( pArea->IsLooseInclude( posCurrent.x, posCurrent.y ) )
		return true;

	// 정확히 도형에 포함되지 않는 경우 폴리곤을 포함하는 최소 사각형의 5미터 이내에 없으면 그냥 바깥으로 간주
	X2D::Box< int > bxArea = pArea->GetBoundingBox();
	bxArea.Set( bxArea.GetLeft() - 60, bxArea.GetTop() - 60, bxArea.GetRight() + 60, bxArea.GetBottom() + 60 );
	if( !bxArea.IsLooseInclude( posCurrent.x, posCurrent.y ) )
		return false;

	// 각 선분과의 거리를 측정하여 거리가 5미터 이내이면 통과(Ref:http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/)
	size_t nSegmentCount = pArea->Size();
	for( size_t nSegmentIdx = 0 ; nSegmentIdx < nSegmentCount ; ++nSegmentIdx )
	{
		X2D::Line< int > line = pArea->GetSegment( nSegmentIdx );

		int dx = line.end.x - line.begin.x;
		int dy = line.end.y - line.begin.y;
		int nPrimeLineLength = dx * dx + dy * dy;

		float u = (float)( ( posCurrent.x - line.begin.x ) * ( line.end.x - line.begin.x ) + ( posCurrent.y - line.begin.y ) * ( line.end.y - line.begin.y ) )
			/ nPrimeLineLength;

		// 점과 선을 수직으로 잇는 선이 있을 경우 접점과 5미터 이내이면 통과
		if( 0.0f <= u && u <= 1.0f )
		{
			ArPosition posCross;
			posCross.x = line.begin.x + u * ( line.end.x - line.begin.x );
			posCross.y = line.begin.y + u * ( line.end.y - line.begin.y );

			if( posCross.GetDistance( posCurrent ) <= 60 )
			{
				return true;
			}
		}
		// 점과 선을 수직으로 잇는 선이 없는 경우 직선의 시작점과 끝점 중 가까운 점과의 거리가 5미터 이내이면 통과
		else if( ( u < 0.0f && posCurrent.GetDistance( ArPosition( line.begin.x, line.begin.y ) ) <= 60 )
			|| ( u > 1.0f && posCurrent.GetDistance( ArPosition( line.end.x, line.end.y ) ) <= 60 ) )
		{
			return true;
		}
	}

	// 폴리곤 포함 체크에서 아무것도 해당되지 않았으므로 포함 안 됨
	return false;
}


struct	polygon_allocator
{
	X2D::Polygon<int>*	operator()()
	{
		return new X2D::Polygon< int >();
	}
};

struct	polygon_deleter
{
	void	operator()( X2D::Polygon<int>* p )
	{
		delete p;
	}
};


#ifndef _USE_NEW_ROAMING_ONLY
std::vector< GameContent::WAY_POINT_INFO >							g_vWayPoint;
#endif
std::vector< _EXP_TABLE >											g_vExpTable;
std::vector< _SUMMON_EXP_TABLE >									g_vSummonExpTable;
std::vector< MonsterBase >											g_vMonsterInfo;
std::vector< std::vector< MonsterBase::MONSTER_TRIGGER > >			g_vMonsterTrigger;
std::vector< std::vector< MonsterBase::MONSTER_SKILL_INFO > >		g_vMonsterSkill;
std::vector< std::vector< MonsterBase::MONSTER_ITEM_DROP_INFO > >	g_vMonsterItemDrop;
std::vector< DropGroup >											g_vDropGroupInfo;
std::vector< SummonBase >											g_vSummonInfo;
std::vector< PetBase >												g_vPetInfo;
std::vector< SkillBase >											g_vSkillBase;
std::vector< QuestLink >											g_vQuestLink;
KHash< QUEST_TEXT_ID, hashPr_mod_int >								g_hsQuestTextId;
std::vector< CreatureStat >											g_vStatInfo;
std::vector< StateInfo >											g_vStateInfo;
std::vector< JobInfo >												g_vJobInfo;
std::vector< JobLevelBonus >										g_vJobLevelBonusInfo;
std::vector< SummonLevelBonus >										g_vSummonLevelBonusInfo;
std::vector< StructNPC* >											g_vNPC;
std::vector< _PROP_CONTACT_SCRIPT_INFO >							g_vPropScriptInfo;
std::vector< std::string >											g_vBanWord;
KHash< std::vector< SetItemEffectInfo * > *, hashPr_mod_int >		g_hsSetItemEffectInfo;
KHash< std::vector< EffectInfo * > *, hashPr_mod_int >				g_hsEffectInfo;
KHash< std::vector< EnhanceEffectInfo * > *, hashPr_mod_int >		g_hsEnhanceEffectInfo;
std::vector< DeathmatchInfo * >										g_vDeathmatchInfo;
std::vector< CreatureEnhanceInfo * >								g_vCreatureEnhanceInfo;
std::vector< CreatureFarmInfo * >									g_vCreatureFarmInfo;
X2D::QuadTree< AR_UNIT, GameContent::MapLocationInfo, true, 10 >	g_qtLocationInfo( 0, 0, g_nMapWidth, g_nMapHeight );

// 한 폴리곤에 1개 이상의 점이 있다면 분할을 시도한다.(즉 MAX_DEPTH 까지 분할을 무조건 한다.)
// QuadTree_divide_polygon 사용 중지
// X2D::QuadTree_divide_polygon< int, X2D::Polygon<int>*, 1, 11 >		g_qtBlockInfo( 0, 0, g_nMapWidth, g_nMapHeight );
X2D::QuadTree< int, X2D::Polygon<int>*, true, 10 >					g_qtBlockInfo( 0, 0, g_nMapWidth, g_nMapHeight );	// 이전 충돌 맵

KHash< GameContent::EVENT_AREA_INFO *, hashPr_mod_int >				g_hsEventAreaInfo;

std::vector< int >													g_avSummonUniqueName[10];
std::vector< int >													g_vSummonPrefix;
std::vector< int >													g_vSummonPostFix;
static KHash< char*, hashPr_mod_int >								g_hsString;
std::vector< NPCBase >												g_vNPCInfo;
std::vector< std::vector< SkillTree > >								g_vSkillTree;

std::vector< GameContent::MONSTER_RESPAWN_INFO >					g_vMonsterRespawnInfo;
std::vector< GameContent::MONSTER_RESPAWN_INFO >					g_vRaidMonsterRespawnInfo;
std::vector< GameContent::RANDOM_MONSTER_RESPAWN_INFO >				g_vRandomMonsterRespawnInfo;
std::vector< GameContent::RANDOM_AREA_INFO >						g_vRandomAreaInfo;
std::vector< GameContent::RANDOM_MONSTER_GROUP_INFO >				g_vRandomMonsterGroupInfo;

std::vector< MonsterCreatureInfo >									g_vMonsterCreatureInfo;
std::vector< SummonRandomSkillInfo >								g_vSummonRandomSkillInfo;

std::vector< RANDOM_POOL >					g_vRandomPool;

void GameContent::Nomalize( AR_UNIT * px, AR_UNIT * py )
{
	*px += (CELL_SIZE/2);
	*py += (CELL_SIZE/2);
	*px /= CELL_SIZE;
	*py /= CELL_SIZE;
	*px = (int)*px;
	*py = (int)*py;
	*px *= CELL_SIZE;
	*py *= CELL_SIZE;
}

std::vector< SkillTree >* GameContent::GetSkillTree( int skill_tree_id )
{
	std::vector< std::vector< SkillTree > >::iterator it;	
	for( it = g_vSkillTree.begin(); it != g_vSkillTree.end(); ++it )
	{
		if( (*it).front().skill_tree_id == skill_tree_id ) return &(*it);
	}
	return NULL;
}

bool GameContent::IsValidItemCode( ItemBase::ItemCode code )
{
	return StructItem::IsValidItemCode( code );
}

// 길찾기 부하를 위한 임시 로그 클래스
class	LongsetTimeLogger
{
public:
	LongsetTimeLogger( const char* szFunction )
	{
		m_szFunction = szFunction;
		m_dLongestTick = 0;
		m_dwUpdateTick = ::GetTickCount();

		LARGE_INTEGER li;
		::QueryPerformanceFrequency( &li );
		m_freq = li.QuadPart;
	}

	void	Start()
	{
		::QueryPerformanceCounter( &m_startTime );
	}

	void	End( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2 )
	{
		LARGE_INTEGER endTime;
		::QueryPerformanceCounter( &endTime );

		double dRunTick = static_cast<double>( (endTime.QuadPart - m_startTime.QuadPart) ) / m_freq;
		if( dRunTick > m_dLongestTick )
		{
			m_dLongestTick = dRunTick;

			FILELOG( "%s: %f (s), (%f,%f) -> (%f,%f)", m_szFunction, m_dLongestTick, x1, y1, x2, y2 );
			_cprint( "%s: %f (s), (%f,%f) -> (%f,%f)\n", m_szFunction, m_dLongestTick, x1, y1, x2, y2 );
		}

		DWORD dwNowTick = ::GetTickCount();
		if( (dwNowTick - m_dwUpdateTick) >= 60*60*1000 )	// 한시간에 한번 초기화
		{
			m_dLongestTick = 0;
			m_dwUpdateTick = dwNowTick;
		}
	}

private:

	const char*		m_szFunction;
	double			m_dLongestTick;
	DWORD			m_dwUpdateTick;

	__int64			m_freq;
	LARGE_INTEGER	m_startTime;

};

class	LongsetTimeHelper
{
public:
	LongsetTimeHelper( LongsetTimeLogger& logger, AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2 )
		: m_logger( logger )
		, m_x1( x1 )
		, m_y1( y1 )
		, m_x2( x2 )
		, m_y2( y2 )
	{
		m_logger.Start();
	}
	~LongsetTimeHelper()
	{
		m_logger.End( m_x1, m_y1, m_x2, m_y2 );
	}

private:

	LongsetTimeLogger& m_logger;

	AR_UNIT	m_x1;
	AR_UNIT m_y1;

	AR_UNIT	m_x2;
	AR_UNIT m_y2;

};

bool GameContent::IsBlocked( AR_UNIT x, AR_UNIT y )
{
	if( int(x) < 0 || int(x) > g_nMapWidth || int(y) < 0 || int(y) > g_nMapHeight )
		return true;

	if( !GameRule::bIsNoCollisionCheck )
	{
		X2D::Point< int > pt( x, y );

		//static LongsetTimeLogger logger( "GameContent::IsBlocked" );
		//LongsetTimeHelper helper( logger, x, y, 0, 0 );

		if( g_qtBlockInfo.LooseCollision( pt ) )
		{
			return true;
		}
	}

	return false;
}

bool GameContent::FindPath( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2, std::vector< ArPosition >& vMoveInfo )
{
	std::vector< X2D::Polygon<int>* > pathFindPolygons;

	GameContent::GetCollisionPolygons( x1, y1, x2, y2, pathFindPolygons );
	if( pathFindPolygons.empty() )
	{
		vMoveInfo.push_back( ArPosition( x1, y1 ) );
		vMoveInfo.push_back( ArPosition( x2, y2 ) );
		return true;
	}

	typedef std::vector< X2D::Point<int> > PATH;
	PATH vPath;

	X2D::Point< int > ptStart( x1, y1 ), ptEnd( x2, y2 );

	static LongsetTimeLogger logger( "X2D::PathFinder::Find" );
	LongsetTimeHelper helper( logger, x1, y1, x2, y2 );

	// 최대 길찾기를 통하여 갈 수 있는 거리는 가시거리 * 2이다. (적절한 파라미터 테스트 중)
	X2D::PathFinder< int, X2D::Polygon<int>* >::Find( pathFindPolygons.begin(), pathFindPolygons.end(), ptStart, ptEnd, vPath, GameRule::VISIBLE_RANGE * 2 );
	if( vPath.empty() )
	{
		return false;
	}

	for( auto it = vPath.begin() ; it != vPath.end() ; ++it )
	{
		vMoveInfo.push_back( ArPosition( (*it).GetX(), (*it).GetY() ) );
	}

	return true;
}

void GameContent::GetCollisionPolygons( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2, std::vector<X2D::Polygon<int> *> & vPolygons )
{
	X2D::Point<int> ptStart( x1, y1 ), ptEnd( x2, y2 );
	X2D::Box<int> boxRegion( ptStart, ptEnd );

	static LongsetTimeLogger logger( "GameContent::GetCollisionPolygons" );
	LongsetTimeHelper helper( logger, x1, y1, x2, y2 );

	g_qtBlockInfo.EnumLoose( boxRegion, &vPolygons );
}

bool GameContent::GetIntersectPoint( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2, AR_UNIT& xr, AR_UNIT& yr )
{
	static LongsetTimeLogger logger( "GameContent::GetIntersectPoint" );
	LongsetTimeHelper helper( logger, x1, y1, x2, y2 );

	X2D::Line<int> ray(x1, y1, x2, y2);
	X2D::Point<int> ptStart( x1, y1 ), ptEnd( x2, y2 );
	double nSmallestSquareDist = sqrt( ((double)ptEnd.x - ptStart.x) * (ptEnd.x - ptStart.x) + (ptEnd.y - ptStart.y) * (ptEnd.y - ptStart.y) );

	X2D::Box<int> boxRegion(ptStart, ptEnd);
	std::vector<X2D::Polygon<int> *> vPolygons;
	g_qtBlockInfo.EnumLoose(boxRegion, &vPolygons);

	bool isIntersect = false;
	for( std::vector<X2D::Polygon<int> *>::iterator it = vPolygons.begin(); it != vPolygons.end(); ++it )
	{
		for( size_t my_idx = 0; my_idx < (*it)->Size(); ++my_idx )
		{
			X2D::Point<int> ptTemp;
			X2D::Line<int> currLine = (*it)->GetSegment( my_idx );
			if( ray.GetIntersectPoint( currLine, &ptTemp ) )
			{
				double nSquareDist = sqrt( ((double)ptTemp.x - ptStart.x) * (ptTemp.x - ptStart.x) + (ptTemp.y - ptStart.y) * (ptTemp.y - ptStart.y) );
				if(nSmallestSquareDist > nSquareDist)
				{
					nSmallestSquareDist = nSquareDist;
					xr = ptTemp.x;
					yr = ptTemp.y;
					isIntersect = true;
				}
			}
		}
	}

	if( isIntersect == true )
	{
		// 교차점이고 소수점을 버리면서 충돌이 나는 것 같다. 시작점 방향으로 좀 땡기는게 좋을듯.
		// 1(단위백터)만큼 당긴다.
		AR_UNIT v_x = (xr-x1);
		AR_UNIT v_y = (yr-y1);

		AR_UNIT dist = sqrt( (float) v_x*v_x + v_y*v_y );
		AR_UNIT i_vx = v_x / dist;
		AR_UNIT i_vy = v_y / dist;

		//xr = (xr - i_vx);
		if( v_x < 0 )
		{
			xr = xr + 1;
		}
		else
		{
			xr = xr - 1;
		}

		//yr = (yr - i_vy);
		if( v_y < 0 )
		{
			yr = yr + 1;
		}
		else
		{
			yr = yr - 1;
		}

		if( GameContent::CollisionToLine( x1, y1, xr, yr ) == true )
		{
			xr = 0;
			yr = 0;
			isIntersect = false;
		}
	}

	return isIntersect;
}

bool GameContent::CollisionToLine( AR_UNIT x1, AR_UNIT y1, AR_UNIT x2, AR_UNIT y2 )
{
	X2D::Line<int> line( x1, y1, x2, y2 );

	if( !GameRule::bIsNoCollisionCheck )
	{
		static LongsetTimeLogger logger( "GameContent::CollisionToLine" );
		LongsetTimeHelper helper( logger, x1, y1, x2, y2 );

		return g_qtBlockInfo.LooseCollision( line );
	}

	return false;
}

bool GameContent::GetValidRandomPos( const ArPosition& center, ArPosition& valid_pos, int v )
{
	valid_pos = center;

	valid_pos.x += ( (XRandom() % v) - (v / 2) );
	valid_pos.y += ( (XRandom() % v) - (v / 2) );

	if( GameContent::CollisionToLine( center.x, center.y, valid_pos.x, valid_pos.y ) == true )
	{
		valid_pos = center;
		if( GameContent::IsBlocked( valid_pos.x, valid_pos.y ) == false )
		{
			return true;
		}

		return false;
	}

	return true;
}

KHash< size_t, hashPr_mod_int >	& GetPropScriptHash()
{
	static KHash< size_t, hashPr_mod_int > inst;
	return inst;
}

KHash< size_t, hashPr_mod_int >	& GetJobCodeHash()
{	
	static KHash< size_t, hashPr_mod_int > inst;
	return inst;
}

KHash< size_t, hashPr_mod_int  > & GetStatCodeHash()
{	
	static KHash< size_t, hashPr_mod_int  >	inst;
	return inst;
}

KHash< size_t, hashPr_mod_int > & GetMonsterCodeHash()
{	
	static KHash< size_t, hashPr_mod_int >	inst;
	return inst;
}

KHash< size_t, hashPr_mod_int > & GetMonsterTriggerCodeHash()
{
	static KHash< size_t, hashPr_mod_int >	inst;
	return inst;
}

KHash< size_t, hashPr_mod_int > & GetMonsterSkillCodeHash()
{
	static KHash< size_t, hashPr_mod_int >	inst;
	return inst;
}

KHash< size_t, hashPr_mod_int > & GetMonsterItemDropCodeHash()
{
	static KHash< size_t, hashPr_mod_int >	inst;
	return inst;
}

KHash< size_t, hashPr_mod_int > & GetDropGroupCodeHash()
{	
	static KHash< size_t, hashPr_mod_int >	inst;
	return inst;
}

KHash< size_t, hashPr_mod_int > & GetSummonCodeHash()
{	
	static KHash< size_t, hashPr_mod_int >	inst;
	return inst;
}

KHash< size_t, hashPr_mod_int > & GetPetCodeHash()
{	
	static KHash< size_t, hashPr_mod_int >	inst;
	return inst;
}

KHash< size_t, hashPr_mod_int > & GetSkillCodeHash()
{	
	static KHash< size_t, hashPr_mod_int >	inst;
	return inst;
}

const CreatureStat	& GameContent::GetStatInfo( int stat_id )
{
	static CreatureStat default_value;

	size_t idx;
	if( GetStatCodeHash().lookup( stat_id, idx ) )
	{
		return g_vStatInfo[idx];
	}

	assert( 0 && "Stat ID" );

	return default_value;
}

bool GameContent::IsBannedWord( int code_page, const char *szString )
{
	// 대문자화
	char szUprWord[256];
	XStringUtil::ToUpperThroughWChar( szUprWord, _countof( szUprWord ), szString, strlen( szString ), code_page );

	// strstr이 대/소문자를 구분하므로 모두 대문자화 되어있음
	for( std::vector< std::string >::iterator it = g_vBanWord.begin(); it != g_vBanWord.end(); ++it )
	{
		if( strstr( szUprWord, (*it).c_str() ) )
			return true;
	}

	return false;
}


__declspec( thread ) char	g_szFailString[64];
const char*	GameContent::GetString( int string_id )
{
	char *pString = "<NULL>";
	if( g_hsString.lookup( string_id, pString ) == false )
	{
		if( s_sprintf( g_szFailString, _countof( g_szFailString ), "SVR%d", string_id ) < 0 )
		{
			g_szFailString[0] = '\0';
		}

		pString = g_szFailString;
	}

	return pString;
}

const StateInfo* GameContent::GetStateInfo( int state_id )
{
	std::vector< StateInfo >::iterator it;
	for( it = g_vStateInfo.begin(); it != g_vStateInfo.end(); ++it )
	{
		if( (*it).id == state_id ) return &(*it);
	}

	return NULL;
}

#ifndef _USE_NEW_ROAMING_ONLY
void GameContent::SetWayPointSpeed( int way_point_id, int way_point_speed )
{
	for( std::vector< GameContent::WAY_POINT_INFO >::iterator it = g_vWayPoint.begin(); it != g_vWayPoint.end(); ++it )
	{
		if( (*it).way_point_id == way_point_id )
		{
			(*it).way_point_speed = way_point_speed;
			return;
		}
	}

	GameContent::WAY_POINT_INFO info;

	info.way_point_id = way_point_id;
	info.way_point_type = GameContent::WAY_POINT_CIRCULAR;
	info.way_point_speed = way_point_speed;

	g_vWayPoint.push_back( info );
}

void GameContent::SetWayPointType( int way_point_id, int way_point_type )
{
	for( std::vector< GameContent::WAY_POINT_INFO >::iterator it = g_vWayPoint.begin(); it != g_vWayPoint.end(); ++it )
	{
		if( (*it).way_point_id == way_point_id )
		{
			(*it).way_point_type = way_point_type;
			return;
		}
	}

	GameContent::WAY_POINT_INFO info;

	info.way_point_id = way_point_id;
	info.way_point_type = way_point_type;
	info.way_point_speed = 0;

	g_vWayPoint.push_back( info );
}

void GameContent::AddWayPoint( int way_point_id, AR_UNIT x, AR_UNIT y )
{
	for( std::vector< GameContent::WAY_POINT_INFO >::iterator it = g_vWayPoint.begin(); it != g_vWayPoint.end(); ++it )
	{
		if( (*it).way_point_id == way_point_id )
		{
			(*it).vWayPoint.push_back( ArPosition( x, y ) );
			return;
		}
	}

	GameContent::WAY_POINT_INFO info;

	info.way_point_id = way_point_id;
	info.way_point_type = GameContent::WAY_POINT_CIRCULAR;
	info.way_point_speed = 0;
	info.vWayPoint.push_back( ArPosition( x, y ) );

	g_vWayPoint.push_back( info );
}

const GameContent::WAY_POINT_INFO * GameContent::GetWayPoint( int way_point_id )
{
	for( std::vector< GameContent::WAY_POINT_INFO >::iterator it = g_vWayPoint.begin(); it != g_vWayPoint.end(); ++it )
	{
		if( (*it).way_point_id == way_point_id )
		{
			return &(*it);
		}
	}

	return NULL;
}
#endif

void GameContent::EnumNPCByNPCId( std::vector< struct StructNPC* > & vNPCList, int nNPCId )
{
	std::vector< StructNPC* >::iterator it;
	for( it = g_vNPC.begin(); it != g_vNPC.end(); ++it )
	{
		if( (*it)->GetNPCID() != nNPCId ) continue;
		vNPCList.push_back( *it );
	}
}

const bool GameContent::SelectItemIDFromDropGroup( const int nDropGroupID, ItemBase::ItemCode & nItemID, __int64 & nItemCount )
{
	DropGroup *pInfo = GameContent::GetDropGroupInfo( nDropGroupID );

	assert( pInfo );

	if( pInfo ) 
	{
		int nCum = 0;
		int nKey = XRandom( 1, 100000000 );

		for( int i = 0; i < DropGroup::MAX_DROP_GROUP; ++i )
		{
			if( !pInfo->drop_item_id[i] ) break;

			nCum += pInfo->drop_percentage[i];

			if( nKey > nCum ) continue;

			nItemID = pInfo->drop_item_id[i];

			nItemCount = XRandom( pInfo->drop_min_count[i], pInfo->drop_max_count[i] );

			return true;
		}
	}

	nItemID = 0;
	nItemCount = 1;

	return false;
}

DropGroup*		GameContent::GetDropGroupInfo( int drop_group_id )
{
	static DropGroup _minfo;

	size_t idx;
	if( GetDropGroupCodeHash().lookup( drop_group_id, idx ) )
	{
		return &g_vDropGroupInfo[idx];
	}

	// assert( 0 );

	return &_minfo;
}

MonsterBase *	GameContent::GetMonsterInfo( int monster_id )
{
	static MonsterBase _minfo;

	size_t idx;
	if( GetMonsterCodeHash().lookup( monster_id, idx ) )
	{
		return &g_vMonsterInfo[idx];
	}

	// assert( 0 );

	return &_minfo;
}

const std::vector< MonsterBase::MONSTER_TRIGGER > * GameContent::GetMonsterTriggerInfo( int nSkillTriggerID )
{
	static std::vector< MonsterBase::MONSTER_TRIGGER > dummy;

	// ID 값이 0이면 비어있는 더미 데이터를 반환
	if( !nSkillTriggerID )
		return &dummy;

	size_t idx;
	if( GetMonsterTriggerCodeHash().lookup( nSkillTriggerID, idx ) )
	{
		return &g_vMonsterTrigger[idx];
	}

	// 트리거는 없으나 스킬만 있는 몬스터의 경우 정상인 데이터이므로 assert를 무조건 걸지는 않음
	//assert( GetMonsterSkillCodeHash().lookup( nSkillTriggerID ) );
	_oprint("Error with monster skill Trigger: %d\n", nSkillTriggerID);

	return &dummy;
}

const std::vector< MonsterBase::MONSTER_SKILL_INFO > * GameContent::GetMonsterSkillInfo( int nSkillTriggerID )
{
	static std::vector< MonsterBase::MONSTER_SKILL_INFO > dummy;

	// ID 값이 0이면 비어있는 더미 데이터를 반환
	if( !nSkillTriggerID )
		return &dummy;

	size_t idx;
	if( GetMonsterSkillCodeHash().lookup( nSkillTriggerID, idx ) )
	{
		return &g_vMonsterSkill[idx];
	}

	// 스킬은 없으나 트리거만 있는 몬스터의 경우 정상인 데이터이므로 assert를 무조건 걸지는 않음
	//assert( GetMonsterTriggerCodeHash().lookup( nSkillTriggerID ) );
	_oprint("Error with monster skill: %d\n", nSkillTriggerID);

	return &dummy;
}

const std::vector< MonsterBase::MONSTER_ITEM_DROP_INFO > * GameContent::GetMonsterDropTable( int nItemDropInfoID )
{
	static std::vector< MonsterBase::MONSTER_ITEM_DROP_INFO > dummy;

	// ID 값이 0이면 비어있는 더미 데이터를 반환
	if( !nItemDropInfoID )
		return &dummy;

	size_t idx;
	if( GetMonsterItemDropCodeHash().lookup( nItemDropInfoID, idx ) )
	{
		return &g_vMonsterItemDrop[idx];
	}

//	assert( 0 && "Monster Drop Table Info ID" );

	return &dummy;
}

SummonBase *	GameContent::GetSummonInfo( int summon_id )
{
	static SummonBase _minfo;

	size_t idx;
	if( GetSummonCodeHash().lookup( summon_id, idx ) )
	{
		return &g_vSummonInfo[idx];
	}

	// assert( 0 ); 프로그램 서버에서 빈번하게 발생되는 스크립트라 assert 구문 삭제

	return &_minfo;
}

bool GameContent::IsUniqueSummonName( const char * name )
{

	for( int i = 0; i < 10; ++i )
	{
		std::vector< int >::iterator it;

		for( it = g_avSummonUniqueName[i].begin(); it != g_avSummonUniqueName[i].end(); ++i )
		{
			if( !_stricmp( GetString(*it), name ) )
				return false;
		}
	}

	return true;
}

std::string	GameContent::GetUniqueName( int summon_type )
{
	if( summon_type < 0 || summon_type >= 10 )
		return "";

	return GetString( g_avSummonUniqueName[summon_type][ XRandom() % g_avSummonUniqueName[summon_type].size() ] );
}

std::string GameContent::GetSummonName()
{
	std::string name;

	name = GetString( g_vSummonPrefix[ XRandom() % g_vSummonPrefix.size() ] );
	name += GetString( g_vSummonPostFix[ XRandom() % g_vSummonPostFix.size() ] );

	return name;
}

PetBase * GameContent::GetPetInfo( int pet_id )
{
	static PetBase _minfo;

	size_t idx;
	if( GetPetCodeHash().lookup( pet_id, idx ) )
	{
		return &g_vPetInfo[idx];
	}

	assert( 0 && "Pet ID" );

	return &_minfo;
}

SkillBase *	GameContent::GetSkillBase( int skill_id )
{
	size_t idx;
	if( GetSkillCodeHash().lookup( skill_id, idx ) )
	{
		return &g_vSkillBase[idx];
	}

	return NULL;
}

__int64 GameContent::GetNeedExp( int level )
{
	if( level < 1 )
		level = 1;

	if( level > GameRule::MAX_LEVEL )
		level = GameRule::MAX_LEVEL;

	if( g_vExpTable.size() < static_cast< size_t >( level ) )
	{
		level = static_cast< int >( g_vExpTable.size() ) - 1;
	}

	return g_vExpTable[level-1].exp;
}

__int64 GameContent::GetNeedSummonExp( int level )
{
	if( g_vSummonExpTable.size() < static_cast< size_t >( level ) )
	{
		return 0;
	}

	return g_vSummonExpTable[level-1].normal_exp;
}

const int GameContent::GetAllowedMaxSkillLevel( std::vector< SkillTree >* skillTree, const int skill_id )
{
	if ( skillTree == NULL )
	{
		return 0;
	}

	int allowedLevel = 0;

	for( std::vector< SkillTree >::iterator it = (*skillTree).begin(); it != skillTree->end(); ++it )
	{
		// 더 높은 단계의 스킬 트리에서 낮은 단계의 스킬을 레벨만 더 높한 상태로 가지고 있는 경우가 있음
		// 이 경우는 현재 스킬 레벨이 스킬 트리의 해당 범위에 들어가는 경우에만 포함되는 것으로 간주한다.
		if( it->skill_id == skill_id )
		{
			allowedLevel = std::max( allowedLevel, it->max_skill_lv );
		}
	}

	return allowedLevel;
}

__int64	GameContent::GetNeedJpForJobLevelUp( int level, int job_depth )
{
	assert( g_vExpTable.size() >= static_cast< size_t >( level ) );
	if( job_depth > 3 ) 
	{
		assert( 0 );
		job_depth = 3;
	}

	if( job_depth < 0 ) 
	{
		assert( 0 );
		job_depth = 0;
	}

	return g_vExpTable[level-1].jp[job_depth];
}

__int64 GameContent::GetNeedJpForSkillLevelUp( int skill_id, int skill_level, int skill_tree_id )
{
	SkillBase *pSkillBase = GetSkillBase( skill_id );

	if( !pSkillBase || skill_level > SkillBase::MAX_SKILL_LEVEL )
	{
		return -1.0f;
	}

	// 배우려는 스킬이 레벨 0일 경우 JP 소모 없음. (랜덤 스킬 기억을 위해 존재)
	if( skill_level == 0 )
		return 0;

	c_fixed10 jp_ratio = -1;

	std::vector< SkillTree >* pTree = GetSkillTree( skill_tree_id );

	std::vector< SkillTree >::iterator it;

	if( !pTree )
	{
		return -1;
	}
	
	for( it = pTree->begin(); it != pTree->end(); ++it )
	{
		if( (*it).skill_id != skill_id ) continue;

		if( (*it).max_skill_lv < skill_level ) continue;
		
		jp_ratio = (*it).jp_ratio;
	}

	if( jp_ratio == -1 )
	{
		jp_ratio = 1;
	}

	return ( jp_ratio * pSkillBase->GetNeedJobPoint( skill_level ) ).GetAsInt64();
}

int GameContent::GetNeedTpForSkillLevelUp( int skill_id, int skill_level )
{
	SkillBase *pSkillBase = GetSkillBase( skill_id );

	if( !pSkillBase || skill_level > SkillBase::MAX_SKILL_LEVEL )
	{
		return -1;
	}

	// 배우려는 스킬 레벨이 0인 경우 TP소모 없음. (랜덤 스킬 기억을 위해 존재)
	if( skill_level == 0 )
		return 0;

	return pSkillBase->GetNeedTalentPoint( skill_level );
}

void GameContent::ClearQuestLink()
{
	// { 기존 NPC-퀘스트 링크 정보 다 지우고
	std::vector< StructNPC* >::iterator it;
	for( it = g_vNPC.begin(); it != g_vNPC.end(); ++it )
	{
		(*it)->ClearQuestLink();
	}
	// }

	// 링크 데이터도 지운다
	g_vQuestLink.clear();

	// NPC없이 진행되는 퀘스트 텍스트 정보도 지움
	g_hsQuestTextId.clear();
}

void GameContent::ApplyQuestLink()
{
	for( std::vector< QuestLink >::iterator it = g_vQuestLink.begin(); it != g_vQuestLink.end(); ++it )
	{
		if( (*it).nNPCId )
		{
			std::vector< StructNPC* > vNPCList;
			GameContent::EnumNPCByNPCId( vNPCList, (*it).nNPCId );

			if( vNPCList.empty() ) 
			{
				FILELOG( "Invalid Quest Link Information : Q:%d -> NPC:%d", (*it).code, (*it).nNPCId );
				_cprint( "Invalid Quest Link Information : Q:%d -> NPC:%d\n", (*it).code, (*it).nNPCId );
				continue;
			}

			std::vector< StructNPC* >::iterator npc_it;
			for( npc_it = vNPCList.begin(); npc_it != vNPCList.end(); ++npc_it )
			{
				// _oprint( "Quest Link : Q:%d -> NPC:%d\n", (*it).code, (*npc_it)->GetNPCID() );
				(*npc_it)->LinkQuest( (*it) );
			}
		}
		else
		{
			// NPC에 의해 진행되는 퀘스트가 아닌 경우에는 시작/중간/종료 텍스트 ID를 퀘스트 정보에 
			RegisterQuestTextId( (*it).code, (*it).nStartTextId, (*it).nInProgressTextId, (*it).nEndTextId );
		}
	}
}

const int GameContent::GetQuestTextId( const QuestBase::QuestCode nQuestCode, const int nProgress )
{
	QUEST_TEXT_ID qti( 0, 0, 0 );

	if( !g_hsQuestTextId.lookup( nQuestCode, qti ) )
		return 0;

	switch( nProgress )
	{
	case QUEST_IS_STARTABLE:	return qti.nStartTextId;
	case QUEST_IS_IN_PROGRESS:	return qti.nInProgressTextId;
	case QUEST_IS_FINISHABLE:	return qti.nEndTextId;
	}

	assert( 0 && "Quest Progress Type Error" );
	return 0;
}

bool GameContent::RegisterQuestLink( QuestLink & info )
{
	g_vQuestLink.push_back( info );
	return true;
}

void GameContent::RegisterQuestTextId( const QuestBase::QuestCode nQuestCode, const int nStartTextId, const int nInProgressTextId, const int nEndTextId )
{
	QUEST_TEXT_ID qti( nStartTextId, nInProgressTextId, nEndTextId );

	if( g_hsQuestTextId.lookup( nQuestCode, qti ) )
	{
		assert( 0 && "Already Exist Quest Text ID" );

		qti.nStartTextId = nStartTextId;
		qti.nInProgressTextId = nInProgressTextId;
		qti.nEndTextId = nEndTextId;
	}
	else
		g_hsQuestTextId.add( nQuestCode, qti );
}

void GameContent::RegisterStateInfo( const StateInfo & info )
{
	std::vector< StateInfo >::iterator it;
	for( it = g_vStateInfo.begin(); it != g_vStateInfo.end(); ++it )
	{
		if( (*it).id == info.id )
		{
			(*it) = info;
			return;
		}
	}

	g_vStateInfo.push_back( info );
}

bool GameContent::RegisterPropContactScriptInfo( int prop_id, int prop_type, int model_info, float x, float y, std::vector< _PROP_CONTACT_SCRIPT_INFO::_FUNCTION_LIST > vFunctionList )
{
	if( GetPropScriptHash().has( prop_id ) )
	{
		throw XException( "Duplicate prop index!" );
		return false;
	}

	_PROP_CONTACT_SCRIPT_INFO tag;
	tag.prop_id = prop_id;
	tag.x = x;
	tag.y = y;
	tag.model_info = model_info;
	tag.vFunctionList = vFunctionList;
	tag.prop_type = prop_type;

	g_vPropScriptInfo.push_back( tag );
	GetPropScriptHash().add( prop_id, g_vPropScriptInfo.size() - 1 );

	//if( tag.prop_type == 0 ) _oprint( "NPC (%d) %d,%d %s\n", tag.prop_id, (int)tag.x, (int)tag.y, tag.vFunctionList[0].stdFunction.c_str() );

	return true;
}

void GameContent::RegisterDropGroupInfo( DropGroup & info )
{
	size_t idx = 0;

	// 이미 존재하면 수정
	if( GetDropGroupCodeHash().lookup( info.uid, idx ) )
	{
		g_vDropGroupInfo[ idx ] = info;
	}
	else
	{
		g_vDropGroupInfo.push_back( info );
		idx = g_vDropGroupInfo.size() - 1;
		GetDropGroupCodeHash().add( info.uid, idx );
	}
}

void GameContent::RegisterMonsterInfo( MonsterBase & info )
{
	size_t idx = 0;

	// 이미 존재하면 수정
	if( GetMonsterCodeHash().lookup( info.uid, idx ) )
	{
		// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
		// 로딩 완료 이전이라면 local_flag가 잘못되어 1개의 몬스터 id가 중첩 로딩된 것이므로 걸림
		assert( ENV().GetString( "game.loading" ) == "complete" );
		g_vMonsterInfo[ idx ] = info;
	}
	else
	{
		// 로딩 완료 이전이라면 최초 로딩에 의해 추가되는 것이니 허용
		// 로딩 완료상태라면 g_vMonsterInfo에 push_back이 일어나 전체적으로 reallocation 발생할 시
		//  GetMonsterInfo로 포인터를 가지고 있던 기존에 리젠되어 있던 몬스터들의 m_pContentInfo가
		//  무효한 메모리 영역을 참조하는 포인터가 되므로 서버가 다운 됨
		assert( ENV().GetString( "game.loading" ) != "complete" );
		g_vMonsterInfo.push_back( info );
		idx = g_vMonsterInfo.size() - 1;
		GetMonsterCodeHash().add( info.uid, idx );
	}
}

void GameContent::RegisterMonsterTriggerInfo( const int nID, const std::vector< MonsterBase::MONSTER_TRIGGER > & vMonsterTriggerInfo )
{
	size_t idx = 0;

	if( GetMonsterTriggerCodeHash().lookup( nID, idx ) )
	{
		// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
		assert( ENV().GetString( "game.loading" ) == "complete" );
		g_vMonsterTrigger[ idx ] = vMonsterTriggerInfo;
	}
	else
	{
		// 로딩 완료 이전이라면 최초 로딩에 의해 추가되는 것이니 허용
		// 로딩 완료상태라면 g_vMonsterTrigger에 push_back이 일어나 전체적으로 reallocation 발생할 시
		//  기존의 포인터가 무효화되므로 서버 다운 됨
		assert( ENV().GetString( "game.loading" ) != "complete" );
		g_vMonsterTrigger.push_back( vMonsterTriggerInfo );
		idx = g_vMonsterTrigger.size() - 1;
		GetMonsterTriggerCodeHash().add( nID, idx );
	}
}

void GameContent::RegisterMonsterSkillInfo( const int nID, const std::vector< MonsterBase::MONSTER_SKILL_INFO > & vMonsterSkillInfo )
{
	size_t idx = 0;

	if( GetMonsterSkillCodeHash().lookup( nID, idx ) )
	{
		// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
		assert( ENV().GetString( "game.loading" ) == "complete" );
		g_vMonsterSkill[ idx ] = vMonsterSkillInfo;
	}
	else
	{
		// 로딩 완료 이전이라면 최초 로딩에 의해 추가되는 것이니 허용
		// 로딩 완료상태라면 g_vMonsterSkill에 push_back이 일어나 전체적으로 reallocation 발생할 시
		//  기존의 포인터가 무효화되므로 서버 다운 됨
		assert( ENV().GetString( "game.loading" ) != "complete" );
		g_vMonsterSkill.push_back( vMonsterSkillInfo );
		idx = g_vMonsterSkill.size() - 1;
		GetMonsterSkillCodeHash().add( nID, idx );
	}
}

void GameContent::RegisterMonsterDropTable( const int nID, const std::vector< MonsterBase::MONSTER_ITEM_DROP_INFO > & vMonsterItemDropInfo )
{
	size_t idx = 0;

	if( GetMonsterItemDropCodeHash().lookup( nID, idx ) )
	{
		// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
		assert( ENV().GetString( "game.loading" ) == "complete" );
		g_vMonsterItemDrop[ idx ] = vMonsterItemDropInfo;
	}
	else
	{
		// 로딩 완료 이전이라면 최초 로딩에 의해 추가되는 것이니 허용
		// 로딩 완료상태라면 g_vMonsterItemDrop에 push_back이 일어나 전체적으로 reallocation 발생할 시
		//  기존의 포인터가 무효화되므로 서버 다운 됨
		assert( ENV().GetString( "game.loading" ) != "complete" );
		g_vMonsterItemDrop.push_back( vMonsterItemDropInfo );
		idx = g_vMonsterItemDrop.size() - 1;
		GetMonsterItemDropCodeHash().add( nID, idx );
	}
}

bool GameContent::RegisterSummonInfo( SummonBase & info )
{
	size_t idx = 0;

	// Summon Base에 미리 로딩한 스킬트리 정보를 연결시켜준다.
	for( int i = 0 ; i < GameRule::MAX_SUMMON_SKILL_TREE ; ++i )
	{
		if( !info.skill_tree_id[i] )
			continue;

		std::vector< std::vector< SkillTree > >::iterator it;
		for( it = g_vSkillTree.begin() ; it != g_vSkillTree.end() ; ++it )
		{
			if( (*it).front().skill_tree_id == info.skill_tree_id[i] )
			{
				info.skill_tree[i] = &(*it);
				break;
			}
		}
	}

	for( int i = 0 ; i < GameRule::MAX_SUMMON_SKILL_TREE ; ++i )
	{
		if( !info.skill_tree_id[i] )
			continue;

		if( !info.skill_tree[i] || info.skill_tree[i]->front().skill_tree_id != info.skill_tree_id[i] )
		{
			assert( 0 );
			_cprint( "MonsterLoader::RegisterSummonInfo - SkillTreeID provided, but cannot find the data envolved:[SummonCode:%d/SkillID:%d]\n", info.uid, info.skill_tree_id[i] );
			FILELOG( "MonsterLoader::RegisterSummonInfo - SkillTreeID provided, but cannot find the data envolved:[SummonCode:%d/SkillID:%d]", info.uid, info.skill_tree_id[i] );
			info.skill_tree_id[i] = 0;
			info.skill_tree[i] = NULL;
		}
	}

	// 이미 존재하면 수정
	if( GetSummonCodeHash().lookup( info.uid, idx ) )
	{
		// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
		// 로딩 완료 이전이라면 local_flag가 잘못되어 1개의 소환수 id가 중첩 로딩된 것이므로 걸림
		assert( ENV().GetString( "game.loading" ) == "complete" );
		g_vSummonInfo[ idx ] = info;
	}
	else
	{
		// 소환수 레벨 스텟 보너스 정보도 함께 추가(더미만 추가)
		SummonLevelBonus bonus;

		g_vSummonInfo.push_back( info );
		g_vSummonLevelBonusInfo.push_back( bonus );
		idx = g_vSummonInfo.size() - 1;
		GetSummonCodeHash().add( info.uid, idx );
	}

	return true;
}

void GameContent::RegisterSummonLevelBonusInfo( const SummonLevelBonus & level_bonus )
{
	size_t idx = 0;

	if( !GetSummonCodeHash().lookup( level_bonus.id, idx ) )
	{
		assert( 0 && "Already Exist Summon Level Bonus ID" );
		return;
	}

	g_vSummonLevelBonusInfo[ idx ] = level_bonus;
}

bool GameContent::RegisterPetInfo( PetBase & info )
{
	size_t idx = 0;

	// 이미 존재하면 수정
	if( GetPetCodeHash().lookup( info.uid, idx ) )
	{
		// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
		// 로딩 완료 이전이라면 local_flag가 잘못되어 1개의 펫 id가 중첩 로딩된 것이므로 걸림
		assert( ENV().GetString( "game.loading" ) == "complete" );
		g_vPetInfo[ idx ] = info;
	}
	else
	{
		g_vPetInfo.push_back( info );
		idx = g_vPetInfo.size() - 1;
		GetPetCodeHash().add( info.uid, idx );
	}

	return true;
}



bool GameContent::RegisterSkillBase( SkillBase & info )
{
	size_t idx = 0;	

	// 이미 존재하면 수정
	if( GetSkillCodeHash().lookup( info.GetID(), idx ) )
	{
		g_vSkillBase[ idx ] = info;
	}
	else
	{
		g_vSkillBase.push_back( info );
		idx = g_vSkillBase.size() - 1;
		GetSkillCodeHash().add( info.GetID(), idx );
	}

	return true;
}

bool GameContent::RegisterSkillJobPointTable( int skill_id, int *need_jp )
{
	SkillBase *pBase = GetSkillBase( skill_id );
	if( !pBase ) return false;

	for( int i = 1; i <= SkillBase::MAX_SKILL_LEVEL; ++i )
	{
		pBase->SetNeedJobPoint( i, need_jp[i-1] );
	}
	return true;
}


bool GameContent::RegisterStringInfo( int string_id, const char *szString )
{
	if( !string_id ) return true;

	char *pValue = NULL;
	size_t size = strlen(szString)+1;
	char *pString = new char[ size ];
	s_strcpy( pString, size, szString );

	// 이미 존재하면 수정
	if( g_hsString.lookup( string_id, pValue ) )
	{
		_oprint( "%d : %s\n", string_id, szString );

		delete [] pValue;
		if( !g_hsString.modify( string_id, pString ) ) assert( 0 );
	}
	else
	{
		g_hsString.add( string_id, pString );
	}

	return true;
}

void GameContent::RegisterExpTable( int level, __int64 exp, __int64 jp_0, __int64 jp_1, __int64 jp_2, __int64 jp_3 )
{
	// 이미 존재하면 수정
	if( g_vExpTable.size() >= static_cast< size_t >( level ) )
	{
		g_vExpTable[level-1].exp = exp;
		g_vExpTable[level-1].jp[0] = jp_0;
		g_vExpTable[level-1].jp[1] = jp_1;
		g_vExpTable[level-1].jp[2] = jp_2;
		g_vExpTable[level-1].jp[3] = jp_3;
		return;
	}

	_EXP_TABLE tmpTable = {exp,{ jp_0, jp_1, jp_2, jp_3 } };

	for( size_t i = g_vExpTable.size(); i < static_cast< size_t >( level ); ++i )
	{
		g_vExpTable.push_back( tmpTable );
	}
}

void GameContent::RegisterSummonExpTable( int level, __int64 normal_exp, __int64 growth_exp, __int64 evolve_exp )
{
	// 이미 존재하면 수정
	if( g_vSummonExpTable.size() >= static_cast< size_t >( level ) )
	{
		g_vSummonExpTable[level-1].normal_exp = normal_exp;
		return;
	}

	_SUMMON_EXP_TABLE tmpTable = { normal_exp, growth_exp, evolve_exp };

	for( size_t i = g_vSummonExpTable.size(); i < static_cast< size_t >( level ); ++i )
	{
		g_vSummonExpTable.push_back( tmpTable );
	}
}

void GameContent::RegisterStatInfo( const CreatureStat & info )
{
	size_t idx = 0;

	// 이미 존재하면 수정
	if( GetStatCodeHash().lookup( info.stat_id, idx ) )
	{
		g_vStatInfo[ idx ] = info;
	}
	else
	{
		g_vStatInfo.push_back( info );
		idx = g_vStatInfo.size() - 1;
		GetStatCodeHash().add( info.stat_id, idx );
	}
}

const JobInfo	* GameContent::GetJobInfo( int job_id )
{
	size_t idx;
	if( GetJobCodeHash().lookup( job_id, idx ) )
	{
		return &g_vJobInfo[idx];
	}

	return NULL;
}

void GameContent::RegisterJobInfo( JobInfo & info )
{
	size_t idx = 0;

	// Job Info에 미리 로딩한 스킬트리 정보를 연결시켜준다
	std::vector< std::vector< SkillTree > >::iterator it;
	for( it = g_vSkillTree.begin() ; it != g_vSkillTree.end() ; ++it )
	{
		if( (*it).front().skill_tree_id == info.skill_tree_id )
		{
			info.skill_tree = &(*it);
			break;
		}
	}

	if( !info.skill_tree_id || !info.skill_tree || info.skill_tree->front().skill_tree_id != info.skill_tree_id )
	{
		assert( 0 );
		_cprint( "ETCLoader::RegisterJobInfo - SkillTreeID provided, but cannot find the data envolved:[JobID:%d/SkillTreeID:%d]\n", info.id, info.skill_tree_id );
		FILELOG( "ETCLoader::RegisterJobInfo - SkillTreeID provided, but cannot find the data envolved:[JobID:%d/SkillTreeID:%d]", info.id, info.skill_tree_id );
		info.skill_tree_id = 0;
		info.skill_tree = NULL;
	}

	// 이미 존재하면 수정
	if( GetJobCodeHash().lookup( info.id, idx ) )
	{
		g_vJobInfo[ idx ] = info;
	}
	else
	{
		JobLevelBonus bonus;

		g_vJobInfo.push_back( info );
		g_vJobLevelBonusInfo.push_back( bonus ); // insert dummy;

		idx = g_vJobInfo.size() - 1;
		GetJobCodeHash().add( info.id, idx );
	}

	return;
}

void GameContent::RegisterJobLevelBonusInfo( const JobLevelBonus & level_bonus )
{
	size_t idx = 0;

	if( !GetJobCodeHash().lookup( level_bonus.job_id, idx ) )
	{
		assert( 0 && "Already Exist Job Level Bonus ID" );
		return;
	}

	g_vJobLevelBonusInfo[ idx ] = level_bonus;
}

int GameContent::isLearnableSkill( struct StructCreature *pCreature, int skill_id, int skill_level, int skill_tree_id, int creature_job_level )
{
	std::vector< SkillTree >* pTree = GameContent::GetSkillTree( skill_tree_id );
	if( !pTree ) return RESULT_ACCESS_DENIED;

	std::vector< SkillTree >::iterator it;
	bool bMaxLimit = false;
	bool bLevelLimit = false;
	bool bEnhanceLimit = false;

	for( it = pTree->begin(); it != pTree->end(); ++it )
	{
		// 스킬 ID가 틀리면 continue
		if( (*it).skill_id != skill_id ) 
			continue;

		// 이미 max level 이면 배울 수 없음
		if( (*it).max_skill_lv < skill_level )
		{
			bMaxLimit = true;
			continue;
		}

		// level 후달림
		if( (*it).lv > pCreature->GetLevel() )
			return RESULT_NOT_ENOUGH_LEVEL;	

		// job level 후달림
		if( (*it).job_lv > creature_job_level )
		{
			bLevelLimit = true;
			continue;
		}

		// 강화 체크, 플레이어는 강화 0
		if( (*it).max_enhance < pCreature->GetEnhance() || (*it).min_enhance > pCreature->GetEnhance() )
		{
			bEnhanceLimit = true;
			continue;
		}

		// 필요 스킬레벨 후달림
		for( int i = 0; i < _countof((*it).need_skill_id); ++i )
		{
			if( !(*it).need_skill_id[i] ) break;

			if( pCreature->GetBaseSkillLevel( (*it).need_skill_id[i] ) < (*it).need_skill_lv[i] ) return RESULT_NOT_ENOUGH_SKILL;	
		}

		// JP 검사
		SkillBase *pBase = GameContent::GetSkillBase( skill_id );
		if( !pBase ) return RESULT_ACCESS_DENIED;
		if( pBase->GetNeedJobPoint( skill_level ) * (*it).jp_ratio > (float) pCreature->GetJobPoint() ) return RESULT_NOT_ENOUGH_JP;

		// 특성 포인트 검사
		if( pBase->GetNeedTalentPoint( skill_level ) > pCreature->GetTalentPoint() ) return RESULT_NOT_ENOUGH_TP;

		return RESULT_SUCCESS;
	}

	if( bLevelLimit )
		return RESULT_NOT_ENOUGH_JOB_LEVEL;	

	if( bMaxLimit )
		return RESULT_LIMIT_MAX;

	if( bEnhanceLimit )
		return RESULT_ENHANCE_LIMIT;

	return RESULT_ACCESS_DENIED;
}

/*ServerPill*/
bool GameContent::LearnAllSkill( struct StructCreature *pCreature )
{
	for( int nJobDepth = 0, nSkillTreeCnt = 0; nJobDepth <= pCreature->GetJobDepth(); )
	{
		int nCurProccesingJobId = 0;
		if		( nJobDepth  < pCreature->GetJobDepth() ) nCurProccesingJobId = pCreature->GetPrevJobId( nJobDepth );
		else if	( nJobDepth == pCreature->GetJobDepth() ) nCurProccesingJobId = pCreature->GetJobId();

		std::vector< struct SkillTree > *vpSkillTree = NULL;
		if( pCreature->IsSummon() )
		{
			SummonBase *pBase = GameContent::GetSummonInfo( nCurProccesingJobId );
			if( !pBase )
				return false;

			if( nSkillTreeCnt >= GameRule::MAX_SUMMON_SKILL_TREE )
			{
				nSkillTreeCnt = 0;
				nJobDepth++;
				continue;
			}

			if( !pBase->skill_tree_id[ nSkillTreeCnt ] || !pBase->skill_tree[ nSkillTreeCnt ] )
			{
				nSkillTreeCnt++;
				continue;
			}
			
			vpSkillTree = pBase->skill_tree[ nSkillTreeCnt++ ];
		}
		else if( pCreature->IsPlayer() )
		{
			const JobInfo *pInfo = GameContent::GetJobInfo( nCurProccesingJobId );
			if( !pInfo || !pInfo->skill_tree)
				return false;

			vpSkillTree = pInfo->skill_tree;
			++nJobDepth;
		}
		else
			return false;

		for( std::vector< struct SkillTree >::iterator itSkillTree = vpSkillTree->begin(); itSkillTree != vpSkillTree->end(); ++itSkillTree )
		{
			int nSkillID = (*itSkillTree).skill_id;

			// 스킬 트리에서 가져온 스킬이 랜덤 스킬일 경우
			// 이미 배운 스킬이면 그 스킬을 nSkillID로 사용, 아니면 직접 랜덤으로 하나 뽑는다.
			// 랜덤으로 뽑힌 진짜 스킬의 나머지 룰(최대 레벨, 강화에 따른 배울 수 있는 수준 등)은
			// 그 전 껍데기 랜덤 스킬의 룰을 따른다 (itSkillTree를 따른다는 말)
			if( (*itSkillTree).skill_group_id )
			{
				StructSkill * pSkill = NULL;
				for( int i = 0 ; i < GameRule::MAX_RANDOM_SKILL_COUNT ; ++i )
				{
					pSkill = pCreature->GetSkill( GetRandomSkillIDFromGroupID( (*itSkillTree).skill_group_id, i ) );
					if( pSkill )
					{
						nSkillID = pSkill->GetSkillId();
						break;
					}
				}
				if( !pSkill )
				{
					nSkillID = GetSummonRandomSkillID( (*itSkillTree).skill_group_id );
				}
			}

			int nNextSkillLv	= pCreature->GetBaseSkillLevel( nSkillID ) + 1;
			if( nNextSkillLv > (*itSkillTree).max_skill_lv ) continue;
			if( pCreature->GetEnhance() > (*itSkillTree).max_enhance ) continue;
			if( pCreature->GetEnhance() < (*itSkillTree).min_enhance ) continue;

			int nNeedJp			= 0;
			int nNeedTp			= 0;
			while( nNextSkillLv <= (*itSkillTree).max_skill_lv )
			{
				int nNeedPoint = 0;

				nNeedPoint = GameContent::GetNeedJpForSkillLevelUp( nSkillID, nNextSkillLv, (*vpSkillTree).front().skill_tree_id );
				if( nNeedPoint != -1 ) nNeedJp += nNeedPoint;

				nNeedPoint = GameContent::GetNeedTpForSkillLevelUp( nSkillID, nNextSkillLv );
				if( nNeedPoint != -1 ) nNeedTp += nNeedPoint;

				++nNextSkillLv;
			}

			ARCADIA_LOCK( ArcadiaServer::Instance().LockObjectWithVisibleRange( pCreature ) );

			if( nNeedJp )	pCreature->SetJP( nNeedJp );
			if( nNeedTp )
			{
				if( pCreature->IsPlayer() )
				{
					StructPlayer * pPlayer = static_cast< StructPlayer * >( pCreature );
					LOG::Log11N4S( LM_CHARACTER_GAIN_TP, pPlayer->GetAccountID(), 
						pPlayer->GetSID(), 0, 
						pPlayer->GetPrevJobId( pPlayer->GetJobDepth() - 1 ), pPlayer->GetJobId(),
						pPlayer->GetPrevJobLevel( pPlayer->GetJobDepth() - 1 ), pPlayer->GetJobLevel(),
						pPlayer->GetTalentPoint(), nNeedTp, 
						0, 0,
						pPlayer->GetAccountName(), LOG::STR_NTS, 
						pPlayer->GetName(), LOG::STR_NTS,
						"", 0,
						"ByLearnAllSKill", LOG::STR_NTS );
				}
				pCreature->SetTalentPoint( nNeedTp );
			}

			pCreature->RegisterSkill( nSkillID, (*itSkillTree).max_skill_lv );
		}
	}

	return true;
}

void GameContent::RegisterSkillTree( const SkillTree & info )
{
	std::vector< SkillTree >* pTree = GetSkillTree( info.skill_tree_id );

	if( pTree ) 
	{
		for( std::vector< SkillTree >::iterator it = pTree->begin(); it != pTree->end(); ++it )
		{
			if( (*it).skill_id == info.skill_id )
			{
				if( (*it).max_skill_lv > info.max_skill_lv )
				{
					// 최대 스킬 레벨은 강화를 할 수록 증가한다. 따라서 최대 스킬 레벨만 체크.
					assert( (*it).max_enhance >= info.max_enhance );

					SkillTree prev_tree = (*it);
					(*it) = info;
					pTree->push_back( prev_tree );

					return;
				}
				break;
			}
		}

		pTree->push_back( info );
		return;
	}	

	std::vector< SkillTree > vTree;
	vTree.push_back( info );
	g_vSkillTree.push_back( vTree );
}

typedef KHash< std::string, hashPr_string_nocase > FILE_HASH;

namespace ResourceManager
{


FILE_HASH & GetResourceFileHash()
{
	static FILE_HASH _hash;
	return _hash;
}

void Clear()
{
	GetResourceFileHash().clear();
}

void Init()
{
	ResourceManager::Clear();

	struct _myScanner : IDirectoryScanner::Scanner
	{
		bool onFile( const char *szFullPath, const char *szDirectory, const char *szFileName )
		{
			ResourceManager::RegisterFile( szFullPath, szFileName );

			return true;
		}
	} scanner;

	IDirectoryScanner::Instance().Scan( ENV().GetString( "game.resource", "./Resource/" ).c_str(), &scanner );
}

std::string GetFullPathname( const char *szFileName )
{
	std::string result;
	GetResourceFileHash().lookup( szFileName, result );
	return result;
}

void RegisterFile( const char *szFullPathName, const char *szFileName )
{
	GetResourceFileHash().add( szFileName, szFullPathName );
}

};


void GameContent::LoadScript()
{
	// LuaVM에 보관되어 있는 로드된 스크립트 파일 이름 목록 재설정
	LUA()->ResetLoadedScriptList();

	FILE_HASH::node* pValueNode = NULL;

	bool bQuitFlag = ResourceManager::GetResourceFileHash().get_first_node( pValueNode ); 
	while( bQuitFlag )
	{	
		// .lua 파일일 경우만 스크립트 다시 로드
		size_t len = pValueNode->value.size();
		if( len > 4 && !_stricmp( (pValueNode->value.c_str()+(len-4)), ".lua" ) )
		{
			if( LUA()->LoadScript( pValueNode->value.c_str(), false ) == false )
			{
				FILELOG( "lua script file load failed. All functionality of lua is disabled. File: %s", pValueNode->value.c_str() );
				_cprint( "lua script file load failed. All functionality of lua is disabled. File: %s\n", pValueNode->value.c_str() );

#ifndef	_DEBUG
				LUA()->DeInit();
				throw XException( "lua script file load failed. All functionality of lua is disabled." );
#endif
			}
		}

		bQuitFlag = ResourceManager::GetResourceFileHash().get_next_node( pValueNode );
	}

	// 로드되어야 할 ELA 파일 수
	int nExpectedELACount = ENV().GetInt( "game.ela_count" );

	if( nExpectedELACount )
	{
		size_t nLoadedELACount = 0;
		bQuitFlag = ResourceManager::GetResourceFileHash().get_first_node( pValueNode ); 
		while( bQuitFlag )
		{	
			// .ela 파일일 경우만 스크립트 다시 로드
			size_t len = pValueNode->value.size();
			if( len > 4 && !_stricmp( (pValueNode->value.c_str()+(len-4)), ".ela" ) )
			{
				if( LUA()->LoadScript( pValueNode->value.c_str(), true ) )
				{
					// 서로 다른 폴더에 있는 같은 이름의 파일은 로드되지 않음(GetResourceFileHash()가 KHash에 파일 이름을 키로 사용해서...;;)

					std::string strResult;
					LUA()->RunString( "return get_module_name()", &strResult );

					unsigned char szMD5Buffer[ 256 ];

					if( strResult.empty() || strResult == "nil" || strResult.length() >= _countof( szMD5Buffer ) )
					{
						LUA()->DeInit();

						FILELOG( "Invalid lua script file detected. All functionality of lua is disabled. File: %s, strResult: %s", pValueNode->value.c_str(), strResult.c_str() );
						_cprint( "Invalid lua script file detected. All functionality of lua is disabled. File: %s, strResult: %s\n", pValueNode->value.c_str(), strResult.c_str() );

						throw XException( "Invalid lua script file detected. All functionality of lua is disabled." );
					}

					strResult.insert( 0, "lua::" );
					s_memcpy( szMD5Buffer, sizeof( szMD5Buffer ), strResult.c_str(), strResult.length() );
					szMD5Buffer[ strResult.length() ] = 0;

					MD5_CTX context;
					MD5_Init( &context );
					MD5_Update( &context, szMD5Buffer, static_cast< unsigned int >( strResult.length() ) );

					unsigned char digest[ 16 ];
					MD5_Final( digest, &context );
					char szOutput[ 37 ];
					s_sprintf( szOutput, _countof( szOutput ), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x.ela",
						digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7],
						digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15] );

					if( _stricmp( pValueNode->key.m_pStr, szOutput ) )
					{
						LUA()->DeInit();

						FILELOG( "Incorrect lua script file detected. All functionality of lua is disabled. File: %s, Key: %s, Output: %s(Result: %s)", pValueNode->value.c_str(), pValueNode->key.m_pStr, szOutput, strResult.c_str() );
						_cprint( "Incorrect lua script file detected. All functionality of lua is disabled. File: %s, Key: %s, Output: %s(Result: %s)\n", pValueNode->value.c_str(), pValueNode->key.m_pStr, szOutput, strResult.c_str() );

						throw XException( "Incorrect lua script file detected. All functionality of lua is disabled." );
					}

					++nLoadedELACount;
				}
				else
				{
					LUA()->DeInit();

					FILELOG( "lua ela script file load failed. All functionality of lua is disabled. File: %s", pValueNode->value.c_str() );
					_cprint( "lua ela script file load failed. All functionality of lua is disabled. File: %s\n", pValueNode->value.c_str() );

					throw XException( "lua ela script file load failed. All functionality of lua is disabled." );
				}
			}

			bQuitFlag = ResourceManager::GetResourceFileHash().get_next_node( pValueNode );
		}

		// 로드된 총 ELA 파일 수 체크
		if( nExpectedELACount != nLoadedELACount )
		{
			LUA()->DeInit();

			FILELOG( "Error on loading lua script files. All functionality of lua is disabled." );
			_cprint( "Error on loading lua script files. All functionality of lua is disabled.\n" );

			throw XException( "Error on loading lua script files. All functionality of lua is disabled." );
		}
	}
}

void GameContent::ClearBannedWord()
{
	g_vBanWord.clear();
}

void GameContent::RegisterBannedWord( const char *szString )
{
	g_vBanWord.push_back( szString );
}

CreatureStat GameContent::GetJobLevelBonus( int depth, int * jobs, int * levels )
{
	assert( depth >= 0 && depth < 4 );

	CreatureStat stat;
	
	memset( &stat, 0, sizeof( stat ) );

	for( int i = 0; i <= depth; ++i )
	{
		size_t idx = 0;

		bool bRet = GetJobCodeHash().lookup( jobs[i], idx );
		assert( bRet );

		//	k1 * min( 20, level ) + max( 0, k2 * min( 20, (20 - level) ) ) + max( 0, k3 * min( 20, (40 - level ) ) )
		//	* k1 1차구간 스탯보너스(레벨20까지적용) * k2 2차구간 스탯보너스(레벨40까지적용) * k3 3차구간 스탯보너스(레벨60까지적용)

		const JobLevelBonus & bonus = g_vJobLevelBonusInfo[idx];

		stat.strength += bonus.strength[0] * std::min( (int) 20, levels[i] )
			+ std::max( (int) 0, bonus.strength[1] * std::min( (int) 20, (levels[i] - 20) ) )
			+ std::max( (int) 0, bonus.strength[2] * std::min( (int) 20, (levels[i] - 40) ) )
			+ bonus.strength[3];
		stat.vital += bonus.vital[0] * std::min( (int) 20, levels[i] )
			+ std::max( (int) 0, bonus.vital[1] * std::min( (int) 20, (levels[i] - 20) ) )
			+ std::max( (int) 0, bonus.vital[2] * std::min( (int) 20, (levels[i] - 40) ) )
			+ bonus.vital[3];
		stat.dexterity += bonus.dexterity[0] * std::min( (int) 20, levels[i] )
			+ std::max( (int) 0, bonus.dexterity[1] * std::min( (int) 20, (levels[i] - 20) ) )
			+ std::max( (int) 0, bonus.dexterity[2] * std::min( (int) 20, (levels[i] - 40) ) )
			+ bonus.dexterity[3];
		stat.agility += bonus.agility[0] * std::min( (int) 20, levels[i] )
			+ std::max( (int) 0, bonus.agility[1] * std::min( (int) 20, (levels[i] - 20) ) )
			+ std::max( (int) 0, bonus.agility[2] * std::min( (int) 20, (levels[i] - 40) ) )
			+ bonus.agility[3];
		stat.intelligence += bonus.intelligence[0] * std::min( (int) 20, levels[i] )
			+ std::max( (int) 0, bonus.intelligence[1] * std::min( (int) 20, (levels[i] - 20) ) )
			+ std::max( (int) 0, bonus.intelligence[2] * std::min( (int) 20, (levels[i] - 40) ) )
			+ bonus.intelligence[3];
		stat.mentality += bonus.mentality[0] * std::min( (int) 20, levels[i] )
			+ std::max( (int) 0, bonus.mentality[1] * std::min( (int) 20, (levels[i] - 20) ) )
			+ std::max( (int) 0, bonus.mentality[2] * std::min( (int) 20, (levels[i] - 40) ) )
			+ bonus.mentality[3];
		stat.luck += bonus.luck[0] * std::min( (int) 20, levels[i] )
			+ std::max( (int) 0, bonus.luck[1] * std::min( (int) 20, (levels[i] - 20) ) )
			+ std::max( (int) 0, bonus.luck[2] * std::min( (int) 20, (levels[i] - 40) ) )
			+ bonus.luck[3];

		stat.strength = stat.strength / 100 * 100;
		stat.vital = stat.vital / 100 * 100;
		stat.dexterity = stat.dexterity / 100 * 100;
		stat.agility = stat.agility / 100 * 100;
		stat.intelligence = stat.intelligence / 100 * 100;
		stat.mentality = stat.mentality / 100 * 100;
		stat.luck = stat.luck / 100 * 100;
	}
	stat.strength /= 100;
	stat.vital /= 100;
	stat.dexterity /= 100;
	stat.agility /= 100;
	stat.intelligence /= 100;
	stat.mentality /= 100;
	stat.luck /= 100;

	return stat;
}

CreatureStat GameContent::GetSummonLevelBonus( int summon_code, int growth_depth, int level )
{
	assert( level > 0 && level <= GameRule::MAX_SUMMON_LEVEL );

	CreatureStat stat;

	memset( &stat, 0, sizeof( stat ) );

	size_t idx = 0;

	if( !GetSummonCodeHash().lookup( summon_code, idx ) )
	{
		assert( 0 && "Summon Level Bonus ID" );
		return stat;
	}

	if( static_cast< int >( g_vSummonLevelBonusInfo.size() ) <= idx )
	{
		assert( 0 );
		return stat;
	}

	if( growth_depth == 1 )
	{
		stat.strength		= (int)( g_vSummonLevelBonusInfo[ idx ].strength		* level );
		stat.vital			= (int)( g_vSummonLevelBonusInfo[ idx ].vitality		* level );
		stat.dexterity		= (int)( g_vSummonLevelBonusInfo[ idx ].dexterity		* level );
		stat.agility		= (int)( g_vSummonLevelBonusInfo[ idx ].agility			* level );
		stat.intelligence	= (int)( g_vSummonLevelBonusInfo[ idx ].intelligence	* level );
		stat.mentality		= (int)( g_vSummonLevelBonusInfo[ idx ].mentality		* level );
		stat.luck			= (int)( g_vSummonLevelBonusInfo[ idx ].luck			* level );
	}
	else
	{
		stat.strength		= (int)( g_vSummonLevelBonusInfo[ idx ].strength		* ( level + 1 - ( growth_depth - 1 ) * 50 ) );
		stat.vital			= (int)( g_vSummonLevelBonusInfo[ idx ].vitality		* ( level + 1 - ( growth_depth - 1 ) * 50 ) );
		stat.dexterity		= (int)( g_vSummonLevelBonusInfo[ idx ].dexterity		* ( level + 1 - ( growth_depth - 1 ) * 50 ) );
		stat.agility		= (int)( g_vSummonLevelBonusInfo[ idx ].agility			* ( level + 1 - ( growth_depth - 1 ) * 50 ) );
		stat.intelligence	= (int)( g_vSummonLevelBonusInfo[ idx ].intelligence	* ( level + 1 - ( growth_depth - 1 ) * 50 ) );
		stat.mentality		= (int)( g_vSummonLevelBonusInfo[ idx ].mentality		* ( level + 1 - ( growth_depth - 1 ) * 50 ) );
		stat.luck			= (int)( g_vSummonLevelBonusInfo[ idx ].luck			* ( level + 1 - ( growth_depth - 1 ) * 50 ) );
	}

	return stat;
}

void GameContent::RegisterBlockInfo( const X2D::Polygon< int > & block_info )
{
	X2D::Polygon< int > * pBlock_Info = new X2D::Polygon< int > ( block_info );

	g_qtBlockInfo.Add( pBlock_Info );
}

void GameContent::RegisterEventAreaInfo( const int nEventAreaID, const EVENT_AREA_INFO & event_area )
{
	EVENT_AREA_INFO * pEventArea = NULL;

	if( g_hsEventAreaInfo.lookup( nEventAreaID, pEventArea ) )
	{
		// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
		// 로딩 완료 이전이라면 중첩 로딩된 것이므로 걸림
		assert( ENV().GetString( "game.loading" ) == "complete" );

		*pEventArea = event_area;
	}
	else
	{
		pEventArea = new EVENT_AREA_INFO( event_area );
		g_hsEventAreaInfo.add( nEventAreaID, pEventArea );
	}
}

void GameContent::RegisterEventAreaBlock( const int nEventAreaID, const X2D::Polygon< int > & block )
{
	EVENT_AREA_INFO * pEventArea = NULL;

	if( !g_hsEventAreaInfo.lookup( nEventAreaID, pEventArea ) )
	{
		// RegisterEventAreaInfo 함수를 통해 이미 등록된 EVENT_AREA_INFO에 대해서만 블록 추가가 가능
		// EventAreaResource의 데이터 중 스크립트 내용이 비어있어서 이벤트 영역 데이터가 추가되지 않은 것일 수 있음
		_cprint( "unknown battle area id. id(%d)\n", nEventAreaID );
		FILELOG( "unknown battle area id. id(%d)", nEventAreaID );
		return;
	}

#ifdef _DEBUG
	for( std::vector< X2D::Polygon< int > * >::const_iterator it = pEventArea->m_vArea.begin() ; it != pEventArea->m_vArea.end() ; ++it )
	{
		assert( !( block == (*(*it)) ) );
	}
#endif

	pEventArea->m_vArea.push_back( new X2D::Polygon< int >( block ) );
}

void GameContent::RegisterMapLocationInfo( const MapLocationInfo & location_info )
{
	g_qtLocationInfo.Add( location_info );
}

const bool GameContent::IsActivatableEventArea( const StructPlayer * pPlayer, const int nEventAreaID, const int nAreaIndex )
{
	EVENT_AREA_INFO * pEventArea = NULL;

	if( !g_hsEventAreaInfo.lookup( nEventAreaID, pEventArea ) )
		return false;

	// nAreaIndex가 정상적으로 검색된 이벤트 영역의 구역 목록 개수 이내인지 체크
	if( nAreaIndex < 0 || static_cast< size_t >( nAreaIndex ) >= pEventArea->m_vArea.size() )
		return false;

	// 발동 횟수 제한 체크(EVENT_AREA_INFO::IsActivatable 함수에서는 해당 이벤트 영역의 ID 값을 알 수 없으므로 여기서 해야 함)
	if( pEventArea->m_nLimitActivateCount )
	{
		if( pPlayer->GetEventAreaEnterCount( nEventAreaID ) >= pEventArea->m_nLimitActivateCount )
			return false;
	}

	return pEventArea->IsActivatable( pPlayer, nAreaIndex );
}

const int GameContent::GetEnterCountLimitOfEventArea( const int nEventAreaID )
{
	EVENT_AREA_INFO * pEventArea = NULL;

	if( !g_hsEventAreaInfo.lookup( nEventAreaID, pEventArea ) )
		return 0;

	return pEventArea->m_nLimitActivateCount;
}

const char * GameContent::GetEventAreaEnterHandler( const int nEventAreaID )
{
	EVENT_AREA_INFO * pEventArea = NULL;

	if( !g_hsEventAreaInfo.lookup( nEventAreaID, pEventArea ) )
	{
		assert( 0 && "Event Area ID" );
	}

	return ( pEventArea ) ? pEventArea->m_strEnterHandler.c_str() : "";
}

const char * GameContent::GetEventAreaLeaveHandler( const int nEventAreaID )
{
	EVENT_AREA_INFO * pEventArea = NULL;

	if( !g_hsEventAreaInfo.lookup( nEventAreaID, pEventArea ) )
	{
		assert( 0 && "Event Area ID" );
	}

	return ( pEventArea ) ? pEventArea->m_strLeaveHandler.c_str() : "";
}

const GameContent::RANDOM_AREA_INFO GameContent::GetRandomAreaInfo( int random_area_id )
{
	for( std::vector< GameContent::RANDOM_AREA_INFO >::const_iterator it = g_vRandomAreaInfo.begin(); it != g_vRandomAreaInfo.end(); ++it )
	{
		if( (*it).random_area_id == random_area_id )
		{
			return (*it);
		}
	}

	return GameContent::RANDOM_AREA_INFO();
}

const GameContent::AREA_BOX GameContent::GetRandomRespawnBox( int random_area_id )
{
	GameContent::RANDOM_AREA_INFO areaInfo = GameContent::GetRandomAreaInfo( random_area_id );

	if( areaInfo.random_area_id == 0 || areaInfo.vRandomBox.empty() )
		return GameContent::AREA_BOX();
	
	return areaInfo.vRandomBox.at( XRandom( 0, static_cast< int >( areaInfo.vRandomBox.size()-1 ) ) );
}

const int GameContent::GetRandomRespawnChannelID( int random_area_id )
{
	GameContent::RANDOM_AREA_INFO areaInfo = GameContent::GetRandomAreaInfo( random_area_id );

	return areaInfo.channel_id;
}

const int GameContent::GetRandomRespawnDungeonID( int random_area_id )
{
	GameContent::RANDOM_AREA_INFO areaInfo = GameContent::GetRandomAreaInfo( random_area_id );

	return areaInfo.dungeon_id;
}

int GameContent::GetLocationId( AR_UNIT x, AR_UNIT y )
{
	std::vector< MapLocationInfo > vLocations;

	X2D::Point< AR_UNIT > pt( x, y );

	g_qtLocationInfo.Enum( pt, &vLocations );

	int priority = 0x7fffffff;
	int location_id = 0;

	std::vector< MapLocationInfo >::iterator it;

	for( it = vLocations.begin(); it != vLocations.end(); ++it )
	{
		if( (*it).priority < priority )
		{
			priority = (*it).priority;
			location_id = (*it).location_id;
		}
	}

	return location_id;
}

void GameContent::RegisterSummonDefaultName( int id, bool bIsPrefix )
{
	if( bIsPrefix )
		g_vSummonPrefix.push_back( id );
	else
		g_vSummonPostFix.push_back( id );

}
void GameContent::RegisterSummonUniqueName( int summon_type, int id )
{
	if( summon_type < 0 || summon_type >= 10 )
		return;

	g_avSummonUniqueName[summon_type].push_back( id );
}


void GameContent::RegisterNPCInfo( struct NPCBase & npc_info )
{
	g_vNPCInfo.push_back( npc_info );
}

struct NPCBase & GameContent::GetNPCInfo( const int id )
{
	for( std::vector< struct NPCBase >::iterator it = g_vNPCInfo.begin() ; it != g_vNPCInfo.end() ; ++it )
	{
		if( (*it).id == id )
			return (*it);
	}

	static NPCBase npc_base;
	npc_base.id = 0;
	return npc_base;
}

void GameContent::RegisterMonsterRespawnInfo( const GameContent::MONSTER_RESPAWN_INFO & info )
{
	g_vMonsterRespawnInfo.push_back( info );
}

void GameContent::RegisterRaidMonsterRespawnInfo( const GameContent::MONSTER_RESPAWN_INFO & info )
{
	g_vRaidMonsterRespawnInfo.push_back( info );
}

void GameContent::RegisterRandomMonsterRespawnInfo( const GameContent::RANDOM_MONSTER_RESPAWN_INFO & info )
{
	for( std::vector< GameContent::RANDOM_MONSTER_RESPAWN_INFO >::iterator it = g_vRandomMonsterRespawnInfo.begin(); it != g_vRandomMonsterRespawnInfo.end(); ++it )
	{
		if( (*it).id == info.id )
		{
			(*it).id = info.id;
			(*it).interval = info.interval;
			(*it).random_area_id = info.random_area_id;
			(*it).inc = info.inc;
			(*it).is_wandering = info.is_wandering;
			(*it).way_point_id = info.way_point_id;
			(*it).prespawn_count = info.prespawn_count;
			(*it).except_raid_siege = info.except_raid_siege;
			return;
		}
	}

	g_vRandomMonsterRespawnInfo.push_back( info );
}

void GameContent::AddRandomAreaInfo( const int random_area_id, const AR_UNIT left, const AR_UNIT top, const AR_UNIT right, const AR_UNIT bottom )
{
	assert( left <= right );
	assert( top <= bottom );

	int channel_id = ChannelManager::GetChannelId( left + ( right - left ) / 2.0f, bottom + ( top - bottom ) / 2.0f );
	int dungeon_id = DungeonManager::Instance().GetDungeonID( left + ( right - left ) / 2.0f, bottom + ( top - bottom ) / 2.0f );

	for( std::vector< GameContent::RANDOM_AREA_INFO >::iterator it = g_vRandomAreaInfo.begin(); it != g_vRandomAreaInfo.end(); ++it )
	{
		if( (*it).random_area_id == random_area_id )
		{
			assert( (*it).channel_id == channel_id );
			assert( (*it).dungeon_id == dungeon_id );

			(*it).vRandomBox.push_back( GameContent::AREA_BOX( left, top, right, bottom ) );
			return;
		}
	}

	g_vRandomAreaInfo.push_back( GameContent::RANDOM_AREA_INFO( random_area_id, channel_id, dungeon_id, GameContent::AREA_BOX( left, top, right, bottom ) ) );
}

void GameContent::AddRandomMonster( int id, int monster_id, int ratio )
{
	for( std::vector< GameContent::RANDOM_MONSTER_RESPAWN_INFO >::iterator it = g_vRandomMonsterRespawnInfo.begin(); it != g_vRandomMonsterRespawnInfo.end(); ++it )
	{
		if( (*it).id == id )
		{
			(*it).monster_list.push_back( std::make_pair( monster_id, ratio ) );
			return;
		}
	}
}

void GameContent::AddRandomMonsterGroup( int group_id, int monster_id, int ratio )
{
	for( std::vector< GameContent::RANDOM_MONSTER_GROUP_INFO >::iterator it = g_vRandomMonsterGroupInfo.begin(); it != g_vRandomMonsterGroupInfo.end(); ++it )
	{
		if( (*it).group_id == group_id )
		{
			(*it).monster_list.push_back( std::make_pair( monster_id, ratio ) );
			return;
		}
	}

	g_vRandomMonsterGroupInfo.push_back( GameContent::RANDOM_MONSTER_GROUP_INFO( group_id ) );
	g_vRandomMonsterGroupInfo[ g_vRandomMonsterGroupInfo.size() - 1 ].monster_list.push_back( std::make_pair( monster_id, ratio ) );
}

void GameContent::RegisterSetItemEffectInfo( const int nSetID, const SetItemEffectInfo & info )
{
	std::vector< SetItemEffectInfo * > * pvSetItemEffectInfo = NULL;

	// 기존에 있던 벡터면 벡터 안에서 재검색
	if( g_hsSetItemEffectInfo.lookup( nSetID, pvSetItemEffectInfo ) )
	{
		for( std::vector< SetItemEffectInfo * >::iterator it = pvSetItemEffectInfo->begin() ; it != pvSetItemEffectInfo->end() ; ++it )
		{
			// 기존에 있던 세트ID/세트파트ID 이므로 기존에 로드된 녀석을 덮어 씀
			if( (*it)->nSetPartID == info.nSetPartID )
			{
				// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
				// 로딩 완료 이전이라면 1개의 SetID에 SetPartID가 중첩 로딩된 것이므로 걸림
				assert( ENV().GetString( "game.loading" ) == "complete" );

				*(*it) = info;
				return;
			}
		}
	}
	// 기존에 한 번도 등록되지 않은 세트ID라면 벡터 추가
	else
	{
		pvSetItemEffectInfo = new std::vector< SetItemEffectInfo * >;
		g_hsSetItemEffectInfo.add( nSetID, pvSetItemEffectInfo );
	}

	// 기존에 없던 세트파트ID 이므로 추가
	SetItemEffectInfo * pInfo = new SetItemEffectInfo( info );
	pvSetItemEffectInfo->push_back( pInfo );
}

void GameContent::RegisterEffectInfo( const int nEffectID, const EffectInfo & info )
{
	std::vector< EffectInfo * > * pvEffectList = NULL;

	// 기존에 있던 벡터면 벡터 안에서 재검색
	if( g_hsEffectInfo.lookup( nEffectID, pvEffectList ) )
	{
		for( std::vector< EffectInfo * >::iterator it = pvEffectList->begin() ; it != pvEffectList->end() ; ++it )
		{
			// 기존에 있던 성능ID/순서ID 이므로 기존에 로드된 녀석을 덮어 씀
			if( (*it)->nOrdinalID == info.nOrdinalID )
			{
				// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
				// 로딩 완료 이전이라면 1개의 EffectID에 OrdinalID가 중첩 로딩된 것이므로 걸림
				assert( ENV().GetString( "game.loading" ) == "complete" );

				*(*it) = info;
				return;
			}
		}
	}
	// 기존에 한 번도 등록되지 않은 성능ID라면 벡터 추가
	else
	{
		pvEffectList = new std::vector< EffectInfo * >;
		g_hsEffectInfo.add( nEffectID, pvEffectList );
	}

	// EffectInfo::nOrdinalID 값이 순서에 맞지 않게 들어오는 경우 체크
	assert( pvEffectList->empty() || pvEffectList->back()->nOrdinalID + 1 == info.nOrdinalID );

	// 기존에 없던 순서ID이므로 추가
	EffectInfo * pInfo = new EffectInfo( info );
	pvEffectList->push_back( pInfo );
}

void GameContent::RigsterEnhanceEffectInfo( const int nEnhanceEffectID, const int nSubID, const EnhanceEffectInfo & info )
{
	std::vector< EnhanceEffectInfo * > * pvEffectList = NULL;

	// 기존에 있던 벡터면 벡터 안에서 재검색
	if( g_hsEnhanceEffectInfo.lookup( nEnhanceEffectID, pvEffectList ) )
	{
		int subID = 1;
		for( std::vector< EnhanceEffectInfo * >::iterator it = pvEffectList->begin() ; it != pvEffectList->end() ; ++it, ++subID )
		{
			// SubID는 1번부터 순차적으로 빈틈없이 간다고 가정
			if( subID == nSubID )
			{
				// 로딩 완료상태에서 있던 정보가 다시 들어오는 건 SCRIPT_Reload에 의해서이니 허용
				// 로딩 완료 이전이라면 nSubID가 같을 수는 없으므로 걸림. (사실 nSubID가 Primary Key Set이라서 걸리진 않을 듯)
				assert( ENV().GetString( "game.loading" ) == "complete" );

				*(*it) = info;
				return;
			}
		}
	}
	// 기존에 한 번도 등록되지 않은 성능ID라면 벡터 추가
	else
	{
		pvEffectList = new std::vector< EnhanceEffectInfo * >;
		g_hsEnhanceEffectInfo.add( nEnhanceEffectID, pvEffectList );
	}

	// 기존에 없던 순서ID이므로 추가
	EnhanceEffectInfo * pInfo = new EnhanceEffectInfo( info );
	pvEffectList->push_back( pInfo );

	// nSubID 순서대로 빈틈없이 들어오는 지 체크
	assert( pvEffectList->empty() || pvEffectList->size() == nSubID );
}

__declspec( thread ) StructNPC * s_pCurrentInitNPC		= NULL;

StructNPC * GetCurrentThreadInitNPC()
{
	return s_pCurrentInitNPC;
}

void AddNPCToRespawnObject( StructNPC * pNPC );
void InitNPCRespawnObject();

StructNPC * GameContent::GetNewNPC( const NPCBase & info, const unsigned char nLayer )
{
	StructNPC *pNPC = new StructNPC( &info );

	pNPC->SetCurrentLayer( nLayer );

	g_vNPC.push_back( pNPC );

	pNPC->CalculateStat();

	s_pCurrentInitNPC = pNPC;

	for( std::vector< QuestLink >::iterator qit = g_vQuestLink.begin() ; qit != g_vQuestLink.end() ; ++qit )
	{
		if( (*qit).nNPCId == pNPC->GetNPCID() )
		{
			pNPC->LinkQuest( (*qit) );
		}
	}

	return pNPC;
}

void GameContent::AddNPCToWorld()
{
	std::vector< NPCBase >::iterator it;

	for( it = g_vNPCInfo.begin(); it != g_vNPCInfo.end(); ++it )
	{
		// 일반, 캐쉬 NPC만 리젠시키되, 캐쉬 NPC는 bIsCashUsableServer 설정이 있거나 테섭이어야만 리젠
		if( (*it).spawn_type != NPCBase::SPAWN_NORMAL && ( (*it).spawn_type != NPCBase::SPAWN_CASH || ( !GameRule::bIsCashUsableServer && ENV().GetInt( "game.ServiceServer" ) != 0 ) ) )
		{
			continue;
		}

		if( (*it).roaming_id )
		{
			RoamingManager::Instance().AddRoamingCreatureRespawnInfo( (*it).roaming_id, GameContent::ROAMING_CREATURE_RESPAWN_INFO( StructRoamer::ROAMING_CREATURE_NPC, (*it).id, (*it).regen_time, (*it).x, (*it).y * GameRule::DEFAULT_UNIT_SIZE ) );
			continue;
		}

		int channel_id = ChannelManager::GetChannelId( (*it).x, (*it).y );

		std::vector< unsigned char > vLayers;
		ChannelManager::GetLayersOfChannel( channel_id, vLayers );

		for( std::vector< unsigned char >::iterator lit = vLayers.begin() ; lit != vLayers.end() ; ++lit )
		{
			StructNPC * pNPC = GetNewNPC( (*it), (*lit) );

			AddNPCToRespawnObject( pNPC );
		}
	}

	InitNPCRespawnObject();
}

void GameContent::AddRespawnObjectToWorld()
{
	std::vector< GameContent::MONSTER_RESPAWN_INFO >::iterator it;

	for( it = g_vMonsterRespawnInfo.begin(); it != g_vMonsterRespawnInfo.end(); ++it )
	{
		GameContent::MONSTER_RESPAWN_INFO info = (*it);

		ArPosition posCenter( info.left + ( info.right - info.left ) / 2.0f, info.bottom + ( info.top - info.bottom ) / 2.0f );
		int channel_id = ChannelManager::GetChannelId( posCenter.GetX(), posCenter.GetY() );
		int dungeon_id = DungeonManager::Instance().GetDungeonID( info.left + ( info.right - info.left ) / 2.0f, info.bottom + ( info.top - info.bottom ) / 2.0f );

		if( channel_id )
		{
			if( ChannelManager::GetChannelType( channel_id ) == ChannelManager::TYPE_USER_LIMIT )
			{
				std::vector< unsigned char > vLayers;
				ChannelManager::GetLayersOfChannel( channel_id, vLayers );

				for( std::vector< unsigned char >::iterator lit = vLayers.begin() ; lit != vLayers.end() ; ++lit )
				{
					info.layer = (*lit);
					AddRespawnObject( info );
				}
			}
			else if( dungeon_id )
			{
				info.dungeon_id = dungeon_id;
				AddRespawnObject( info );

				DungeonManager::Instance().RegisterDungeonMonsterRespawnInfo( dungeon_id, info );
			}
		}
		else
		{
			AddRespawnObject( info );
		}
	}

	std::vector< GameContent::RANDOM_MONSTER_RESPAWN_INFO >::iterator rIt;

	for( rIt = g_vRandomMonsterRespawnInfo.begin(); rIt != g_vRandomMonsterRespawnInfo.end(); ++rIt )
	{
		GameContent::RANDOM_MONSTER_RESPAWN_INFO info = (*rIt);

		int channel_id = GameContent::GetRandomRespawnChannelID( info.random_area_id );
		int dungeon_id = GameContent::GetRandomRespawnDungeonID( info.random_area_id );

		if( channel_id )
		{
			if( ChannelManager::GetChannelType( channel_id ) == ChannelManager::TYPE_USER_LIMIT )
			{
				std::vector< unsigned char > vLayers;
				ChannelManager::GetLayersOfChannel( channel_id, vLayers );

				for( std::vector< unsigned char >::iterator lit = vLayers.begin() ; lit != vLayers.end() ; ++lit )
				{
					info.layer = (*lit);
					AddRandomRespawnObject( info );
				}
			}
			else if( dungeon_id )
			{
				info.dungeon_id = dungeon_id;
				AddRandomRespawnObject( info );

				if( !info.except_raid_siege )
					DungeonManager::Instance().RegisterRandomDungeonMonsterRespawnInfo( dungeon_id, info );
			}
		}
		else
		{
			AddRandomRespawnObject( info );
		}
	}
}

void GameContent::RegisterRandomPoolInfo( int group_id, int quest_target_id, int target_level )
{
	std::vector< RANDOM_POOL >::iterator it;

	RANDOM_POOL * pRandomPool = NULL;

	for( it = g_vRandomPool.begin(); it != g_vRandomPool.end(); ++it )
	{
		if( group_id == (*it).group_id )
		{
			pRandomPool = &(*it);
			break;
		}
	}

	if( !pRandomPool )
	{
		g_vRandomPool.push_back( RANDOM_POOL( group_id ) );
		pRandomPool = &(g_vRandomPool.back() );
	}

	pRandomPool->vInfo.push_back( GameContent::RANDOM_POOL_INFO( quest_target_id, target_level ) );
}

bool GameContent::IsInRandomPoolMonster( int group_id, int monster_id )
{
	if( group_id == monster_id )
		return true;

	if( group_id >= 0 )
		return false;

	std::vector< RANDOM_POOL >::iterator it;

	for( it = g_vRandomPool.begin(); it != g_vRandomPool.end(); ++it )
	{
		if( group_id == (*it).group_id )
		{
			for( std::vector< GameContent::RANDOM_POOL_INFO >::iterator itRandom = (*it).vInfo.begin(); itRandom != (*it).vInfo.end(); ++itRandom )
			{
				if( (*itRandom).quest_target_id == monster_id )
					return true;
			}

			break;
		}
	}

	return false;
}

const std::vector< SetItemEffectInfo * > * GameContent::GetSetItemEffectInfoVector( const int nSetID )
{
	static std::vector< SetItemEffectInfo * > vDummy;
	std::vector< SetItemEffectInfo * > * pvInfo = NULL;

	if( g_hsSetItemEffectInfo.lookup( nSetID, pvInfo ) )
		return pvInfo;

	assert( 0 && "Set Item Effect ID" );
	return &vDummy;
}

const std::vector< EffectInfo * > * GameContent::GetEffectInfoVector( const int nEffectID )
{
	static std::vector< EffectInfo * > vDummy;
	std::vector< EffectInfo * > * pvInfo = NULL;

	if( g_hsEffectInfo.lookup( nEffectID, pvInfo ) )
		return pvInfo;

	//assert( 0 && "Effect Info Vector ID" );
	return &vDummy;
}

const std::vector< EnhanceEffectInfo * > * GameContent::GetEnhanceEffectVector( const int nEnhanceEffectID )
{
	static std::vector< EnhanceEffectInfo * > vDummy;
	std::vector< EnhanceEffectInfo * > * pvInfo = NULL;

	if( g_hsEnhanceEffectInfo.lookup( nEnhanceEffectID, pvInfo ) )
		return pvInfo;

	//assert( 0 && "EnhanceEffect information does not exist" );
	return &vDummy;
}

bool GameContent::GetRandomPoolInfo( int group_id, std::vector< GameContent::RANDOM_POOL_INFO > & rRandomPoolInfo, int level )
{
	std::vector< RANDOM_POOL >::iterator it;

	for( it = g_vRandomPool.begin(); it != g_vRandomPool.end(); ++it )
	{
		if( group_id == (*it).group_id )
		{
			rRandomPoolInfo = (*it).vInfo;

			for( std::vector< GameContent::RANDOM_POOL_INFO >::iterator itRandom = rRandomPoolInfo.begin(); itRandom != rRandomPoolInfo.end(); )
			{
				if( (*itRandom).level > level + 4 || (*itRandom).level < level - 4 )
				{
					itRandom = rRandomPoolInfo.erase( itRandom );
				}
				else
				{
					++itRandom;
				}
			}

			return true;
		}
	}

	return false;
}

void GameContent::RegisterDeathmatchInfo( const int id, const int deathmatch_type, const int min_level, const int max_level, const ArPosition position[] ) 
{
	std::vector< ArPosition > vPosition;

	for( int i=0; i<5; i++ )
	{
		if( position[i].GetX() == 0 && position[i].GetY() == 0 ) break;

		vPosition.push_back( position[i] );
	}

	// 비어있다면 데이터에 문제가 있음.
	assert( !vPosition.empty() );
	
	DeathmatchInfo *deathmatchInfo = new DeathmatchInfo( id, deathmatch_type, min_level, max_level, vPosition );
	g_vDeathmatchInfo.push_back( deathmatchInfo );
}

const ArPosition GameContent::GetDeathmatchPosition( const int level, const int deathmatch_type )
{
	std::vector< DeathmatchInfo * >::iterator it;

	for( it = g_vDeathmatchInfo.begin(); it != g_vDeathmatchInfo.end(); ++it )
	{
		if( (*it)->deathmatch_type == deathmatch_type && (*it)->min_level <= level && level <= (*it)->max_level )
		{
			break;
		}
	}

	int index = XRandom(0, static_cast< int >((*it)->m_vPosition.size()-1));
	return (*it)->m_vPosition[index];
}

const int GameContent::GetDeathmatchType( const ArPosition pos )
{
	int channel_id = ChannelManager::GetChannelId( pos.x, pos.y );
	if( ( channel_id >= TYPE_DEATHMATCH_1_RANKED && channel_id <= TYPE_DEATHMATCH_4_RANKED ) || ( channel_id >= TYPE_DEATHMATCH_5_RANKED && channel_id <= TYPE_DEATHMATCH_7_RANKED ) )
		return 1;
	else if( channel_id == TYPE_DEATHMATCH_FREED ) 
		return 2;

	return -1;
}

void GameContent::RegisterCreatureEnhanceInfo( const short enhance_level, const float stat_amplify, const short card_durability, const short slot_amount, const short jp_addition )
{
	// 데이터는 순차/연속적이어야 한다.
	assert( g_vCreatureEnhanceInfo.size() == enhance_level );

	CreatureEnhanceInfo *creatureEnhanceInfo = new CreatureEnhanceInfo( stat_amplify, card_durability, slot_amount, jp_addition );
	g_vCreatureEnhanceInfo.push_back( creatureEnhanceInfo );
}

const CreatureEnhanceInfo *GameContent::GetCreatureEnhanceInfo( const short enhance_level )
{
	assert( enhance_level >= 0 && static_cast< size_t >( enhance_level ) < g_vCreatureEnhanceInfo.size() );

	return g_vCreatureEnhanceInfo[ enhance_level ];
}

void GameContent::RegisterCreatureFarmInfo( const char rate, const char form, const char enhance_level, const char ticket_count )
{
	CreatureFarmInfo *creatureFarmInfo = new CreatureFarmInfo( rate, form, enhance_level, ticket_count );
	g_vCreatureFarmInfo.push_back( creatureFarmInfo );
}

const char GameContent::GetCreatureFarmTicketCount( const char rate, const char form, const char enhance_level )
{
	std::vector< CreatureFarmInfo * >::const_iterator it;

	for( it = g_vCreatureFarmInfo.begin(); it != g_vCreatureFarmInfo.end(); ++it )
	{
		if( (*it)->rate == rate &&
			(*it)->form == form &&
			(*it)->enhance_level == enhance_level )
			return (*it)->ticket_count;
	}

	assert( 0 && "Creature Farm Info ID" );

	return 0;
}

const int GameContent::GetRandomMonsterID( const int group_id )
{
	if( group_id < 1 ) return 0;

	for( std::vector< RANDOM_MONSTER_GROUP_INFO >::const_iterator it = g_vRandomMonsterGroupInfo.begin(); it != g_vRandomMonsterGroupInfo.end(); ++it )
	{
		if( (*it).group_id == group_id )
		{
			int nCum = 0;
			int nKey = XRandom( 1, 100000000 );

			for( std::vector< std::pair< int, int > >::const_iterator mIt = (*it).monster_list.begin(); mIt != (*it).monster_list.end(); ++mIt )
			{
				// 꽝
				if( !mIt->first ) return 0;

				nCum += mIt->second;

				if( nKey > nCum ) continue;

				return mIt->first;
			}
		}
	}

	return 0;
}


void GameContent::RegisterMonsterCreatureInfo( MonsterCreatureInfo & monster_creater )
{
	std::vector<MonsterCreatureInfo>::iterator it;
	bool find = false;
	for( it = g_vMonsterCreatureInfo.begin() ; it != g_vMonsterCreatureInfo.end() ; ++it )
	{
		if( (*it).nSid == monster_creater.nSid )
		{
			find = true;
			(*it).nSid = monster_creater.nSid;
			(*it).nUseCode = monster_creater.nUseCode;
			for( int i = 0 ; i < GameRule::MAX_MONSTER_CREATURE_COUNT ; ++i )
			{
				(*it).arSummonCode[ i ] = monster_creater.arSummonCode[ i ];
			}
			break;
		}
	}

	if( !find )
	{
		MonsterCreatureInfo info;
		info.nSid = monster_creater.nSid;
		info.nUseCode = monster_creater.nUseCode;
		for( int i = 0 ; i < GameRule::MAX_MONSTER_CREATURE_COUNT ; ++i )
		{
			info.arSummonCode[ i ] = monster_creater.arSummonCode[ i ];
		}
		g_vMonsterCreatureInfo.push_back( info );
	}
}

const int GameContent::GetMonsterCreatureTameCode( int _nMonsterCreatureCode )
{
	if( !g_vMonsterCreatureInfo.size() )
	{
		return 0;
	}

	std::vector<MonsterCreatureInfo>::iterator it;
	for( it = g_vMonsterCreatureInfo.begin() ; it != g_vMonsterCreatureInfo.end() ; ++it )
	{
		if( (*it).nSid == _nMonsterCreatureCode )
		{
			int nSummonGroupIndex = XRandom( 1, (*it).nUseCode );
			nSummonGroupIndex = nSummonGroupIndex - 1;
			if( GameRule::MAX_MONSTER_CREATURE_COUNT < nSummonGroupIndex )
			{
				// 배열의 Index 검증. Overflow Error 방지
				return 0;
			}
			return (*it).arSummonCode[ nSummonGroupIndex ];
		}
	}

	return 0;
}

// 소울 크리처와 일반 크리처가 데이터로 비교 불가능하기 때문에 몬스터 테이밍 정보를 간접적으로 참고하여 소울크리처인지 판단해야 한다.
// 따라서 nSummonCode에 "기본형" 소환수 코드를 줘야 제대로 판별이 가능하다.
// 진화된 소울 크리처들은 이 함수의 사용을 위해서는 PrevJobId를 조사하여 기본형 소환수 코드로 넘길 것.
// 함수 이름이 IsSoulCreature이므로 오해를 불러 일으킬 수 있으니 시간이 될 때 소울 크리처의 데이터 구조를 수정해야 할 듯.
bool GameContent::IsSoulCreature( int nSummonCode )
{
	std::vector< MonsterCreatureInfo >::iterator it = g_vMonsterCreatureInfo.begin();
	for( ; it != g_vMonsterCreatureInfo.end() ; ++it )
	{
		for( int i = 0 ; i < (*it).nUseCode ; ++i )
		{
			if( nSummonCode == (*it).arSummonCode[ i ] )
				return true;
		}
	}

	return false;
}

void GameContent::RegisterSummonRandomSkillInfo( SummonRandomSkillInfo & summon_random_skill )
{
	std::vector<SummonRandomSkillInfo>::iterator it;
	bool find = false;
	for( it = g_vSummonRandomSkillInfo.begin() ; it != g_vSummonRandomSkillInfo.end() ; ++it )
	{
		if( (*it).nSid == summon_random_skill.nSid )
		{
			find = true;
			(*it).nSid = summon_random_skill.nSid;
			(*it).nUseCode = summon_random_skill.nUseCode;
			for( int i = 0 ; i < GameRule::MAX_RANDOM_SKILL_COUNT ; ++i )
			{
				(*it).arSummonSkillID[ i ] = summon_random_skill.arSummonSkillID[ i ];
			}
			break;
		}
	}

	if( !find )
	{
		SummonRandomSkillInfo info;
		info.nSid = summon_random_skill.nSid;
		info.nUseCode = summon_random_skill.nUseCode;
		for( int i = 0 ; i < GameRule::MAX_RANDOM_SKILL_COUNT ; ++i )
		{
			info.arSummonSkillID[ i ] = summon_random_skill.arSummonSkillID[ i ];
		}
		g_vSummonRandomSkillInfo.push_back( info );
	}
}

const int GameContent::GetSummonRandomSkillID( int _nRandomSkillGroupID )
{
	if( !g_vSummonRandomSkillInfo.size() )
	{
		return 0;
	}

	std::vector<SummonRandomSkillInfo>::iterator it;
	for( it = g_vSummonRandomSkillInfo.begin() ; it != g_vSummonRandomSkillInfo.end() ; ++it )
	{
		if( (*it).nSid == _nRandomSkillGroupID )
		{
			int nRandomSkillIndex = XRandom( 1, (*it).nUseCode );
			nRandomSkillIndex = nRandomSkillIndex - 1;
			if( GameRule::MAX_RANDOM_SKILL_COUNT <= nRandomSkillIndex )
			{
				// 배열의 Index 검증. Overflow Error 방지
				return 0;
			}

			return (*it).arSummonSkillID[ nRandomSkillIndex ];
		}
	}

	return 0;
}

const int GameContent::GetRandomSkillIDFromGroupID( int nGroupID, int nIndex )
{
	if( nIndex > GameRule::MAX_RANDOM_SKILL_COUNT-1 )
		return NULL;

	for( std::vector<SummonRandomSkillInfo>::iterator it = g_vSummonRandomSkillInfo.begin() ; it != g_vSummonRandomSkillInfo.end() ; ++it )
	{
		if( (*it).nSid == nGroupID )
		{
			return (*it).arSummonSkillID[ nIndex ];
		}
	}

	return NULL;
}

const int GameContent::GetSkillTreeGroupID( StructCreature *_pCreature, int _skill_id )
{
	int nMacCnt = _pCreature->GetJobDepth();
	for( int nCnt = 0 ; nCnt <= nMacCnt ; ++nCnt )
	{
		int nJobID = 0;
		if( nCnt < nMacCnt )
		{
			nJobID = _pCreature->GetPrevJobId( nCnt );
		}
		else
		{
			nJobID = _pCreature->GetJobId();
		}

		if( _pCreature->IsPlayer() )
		{
			std::vector< SkillTree > *vSkillTree = NULL;
			vSkillTree = GameContent::GetJobInfo( nJobID )->skill_tree;
			if( vSkillTree )
			{
				for( std::vector< SkillTree >::iterator it = (*vSkillTree).begin() ; it != (*vSkillTree).end() ; ++it )
				{
					if( (*it).skill_id == _skill_id )
					{
						return (*it).skill_group_id;
					}
				}
			}
		}
		else if( _pCreature->IsSummon() )
		{
			std::vector< SkillTree > **vSkillTree = NULL;
			vSkillTree = GameContent::GetSummonInfo( nJobID )->skill_tree;
			for( int i = 0; i < GameRule::MAX_SUMMON_SKILL_TREE ; ++i )
			{
				if( !vSkillTree[i] )
					continue;

				for( std::vector< SkillTree >::iterator it = (*vSkillTree[i]).begin() ; it != (*vSkillTree[i]).end() ; ++it )
				if( (*it).skill_id == _skill_id )
				{
					return (*it).skill_group_id;
				}
			}
		}
	}

	assert( 0 && "SkillTree Resource Error" );
	return 0;
}