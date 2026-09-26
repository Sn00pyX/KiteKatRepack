
#include <toolkit/KHash.h>

#include "GameAllocator.h"
#include "DBAllocator.h"
#include "StructCreature.h"
#include "StructPlayer.h"
#include "StructMonster.h"
#include "StructSummon.h"
#include "StructPet.h"
#include "StructItem.h"
#include "StructSkill.h"
#include "StructQuest.h"
#include "StructTitle.h"
#include "StructTitleCondition.h"
#include "StructProc.h"


volatile unsigned		s_UID;

volatile LONG			s_PlayerLock			= 0;
volatile LONG			s_MonsterLock			= 0;
volatile LONG			s_ItemLock				= 0;
volatile LONG			s_SummonLock			= 0;
volatile LONG			s_PetLock				= 0;
volatile LONG			s_SkillLock				= 0;
volatile LONG			s_QuestLock				= 0;
volatile LONG			s_TitleLock				= 0;
volatile LONG			s_TitleConditionLock	= 0;
volatile LONG			s_ProcLock				= 0;
volatile LONG			s_MiscLock				= 0;

typedef KHash< GameObject*, hashPr_mod_basic<AR_HANDLE> >	MISC_HASH;

#ifdef _DEBUG

static XMemoryPool & GetPlayerHeap()
{
	static XMemoryPool heap( sizeof( StructPlayer ), 64, 1, false );
	return heap;
}

static XMemoryPool & GetSummonHeap()
{
	static XMemoryPool heap( sizeof( StructSummon ), 64, 1, false );
	return heap;
}

static XMemoryPool & GetPetHeap()
{
	static XMemoryPool heap( sizeof( StructPet ), 64, 1, false );
	return heap;
}

static XMemoryPool & GetSkillHeap()
{
	static XMemoryPool heap( sizeof( StructSkill ), 4096, 1, false );
	return heap;
}

static XMemoryPool & GetQuestHeap()
{
	static XMemoryPool heap( sizeof( StructQuest ), 4096, 1, false );
	return heap;
}

static XMemoryPool & GetTitleHeap()
{
	static XMemoryPool heap( sizeof( StructTitle ), 4096, 1, false );
	return heap;
}

static XMemoryPool & GetTitleConditionHeap()
{
	static XMemoryPool heap( sizeof( StructTitleCondition ), 4096, 1, false );
	return heap;
}

struct XMemoryPool & GetProcHeap()
{
	static XMemoryPool heap(
		COMPILE_TIME_SIZE_COMPARE( StructProc,
		COMPILE_TIME_SIZE_COMPARE( StructStateProc,
		COMPILE_TIME_SIZE_COMPARE( StructAbsorbProc,
		COMPILE_TIME_SIZE_COMPARE( StructHealProc,
		COMPILE_TIME_SIZE_COMPARE( StructStealProc,
		COMPILE_TIME_SIZE_COMPARE( StructEnergyProc,
		COMPILE_TIME_SIZE_COMPARE( StructCooldownProc,
		0 ) ) ) ) ) ) )
		// 뭐 나도 이렇게 몰아서 괄호 닫고 싶진 않았는데...
		// 재귀 템플릿화에 능한 누군가가 좀 고쳐주셈;;
		, 4096, 1, false );

	return heap;
}

static XMemoryPool & GetItemHeap()
{
	static XMemoryPool heap( sizeof( StructItem ), 64, 1, false );
	return heap;
}

static XMemoryPool & GetMonsterHeap()
{
	static XMemoryPool heap( sizeof( StructMonster ), 512, 70, false );
	return heap;
}

#else

static XMemoryPool & GetPlayerHeap()
{
	static XMemoryPool heap( sizeof( StructPlayer ), 512, 4, false );
	return heap;
}

static XMemoryPool & GetSummonHeap()
{
	static XMemoryPool heap( sizeof( StructSummon ), 1024, 10, false );
	return heap;
}

static XMemoryPool & GetPetHeap()
{
	static XMemoryPool heap( sizeof( StructPet ), 128, 8, false );
	return heap;
}

static XMemoryPool & GetSkillHeap()
{
	static XMemoryPool heap( sizeof( StructSkill ), 8192, 30, false );
	return heap;
}

static XMemoryPool & GetQuestHeap()
{
	static XMemoryPool heap( sizeof( StructQuest ), 4096, 15, false );
	return heap;
}

static XMemoryPool & GetTitleHeap()
{
	static XMemoryPool heap( sizeof( StructTitle ), 4096, 15, false );
	return heap;
}

static XMemoryPool & GetTitleConditionHeap()
{
	static XMemoryPool heap( sizeof( StructTitleCondition ), 4096, 15, false );
	return heap;
}

struct XMemoryPool & GetProcHeap()
{
	static XMemoryPool heap(
		COMPILE_TIME_SIZE_COMPARE( StructProc,
		COMPILE_TIME_SIZE_COMPARE( StructStateProc,
		COMPILE_TIME_SIZE_COMPARE( StructAbsorbProc,
		COMPILE_TIME_SIZE_COMPARE( StructHealProc,
		COMPILE_TIME_SIZE_COMPARE( StructStealProc,
		COMPILE_TIME_SIZE_COMPARE( StructEnergyProc,
		COMPILE_TIME_SIZE_COMPARE( StructCooldownProc,
		0 ) ) ) ) ) ) )
		// 뭐 나도 이렇게 몰아서 괄호 닫고 싶진 않았는데...
		// 재귀 템플릿화에 능한 누군가가 좀 고쳐주셈;;
		, 4096, 15, false );

	return heap;
}

static XMemoryPool & GetItemHeap()
{
	static XMemoryPool heap( sizeof( StructItem ), 16384, 26, false );
	return heap;
}

static XMemoryPool & GetMonsterHeap()
{
	static XMemoryPool heap( sizeof( StructMonster ), 512, 70, false );
	return heap;
}

#endif

static MISC_HASH & GetMiscHash()
{
	static MISC_HASH hash;
	return hash;
}

int GetPlayerHeapSize()
{
	return static_cast< int >( GetPlayerHeap().GetAllocSize() >> 10 );
}

int GetMonsterHeapSize()
{
	return static_cast< int >( GetMonsterHeap().GetAllocSize() >> 10 );
}

int GetItemHeapSize()
{
	return static_cast< int >( GetItemHeap().GetAllocSize() >> 10 );
}

int GetSummonHeapSize()
{
	return static_cast< int >( GetSummonHeap().GetAllocSize() >> 10 );
}

int GetPetHeapSize()
{
	return static_cast< int >( GetPetHeap().GetAllocSize() >> 10 );
}

int GetSkillHeapSize()
{
	return static_cast< int >( GetSkillHeap().GetAllocSize() >> 10 );
}

int GetQuestHeapSize()
{
	return static_cast< int >( GetQuestHeap().GetAllocSize() >> 10 );
}

int	GetTitleHeapSize()
{
	return static_cast< int >( GetTitleHeap().GetAllocSize() >> 10 );
}

int	GetTitleConditionHeapSize()
{
	return static_cast< int >( GetTitleConditionHeap().GetAllocSize() >> 10 );
}

int GetProcHeapSize()
{
	return static_cast< int >( GetProcHeap().GetAllocSize() >> 10 );
}

void InitGameHeap()
{
	s_UID = 0;

	spinlock_init( &s_PlayerLock );
	spinlock_init( &s_MonsterLock );
	spinlock_init( &s_ItemLock );
	spinlock_init( &s_SummonLock );
	spinlock_init( &s_PetLock );
	spinlock_init( &s_MiscLock );

	// 싱글톤 초기화. (안해주면 난리남)
	GetPlayerHeap();
	GetMonsterHeap();
	GetItemHeap();
	GetSummonHeap();
	GetPetHeap();
	GetSkillHeap();
	GetQuestHeap();
	GetProcHeap();
	GetTitleHeap();
	GetTitleConditionHeap();
	GetMiscHash();
	//
}

static unsigned int g_misc_cnt = 0;

AR_HANDLE AllocMiscHandle( struct GameObject* pObj )
{
	spinlock_enter( &s_MiscLock );

	//AR_HANDLE tmp = (++cnt << 32 ) | TYPE_MISC;

	AR_HANDLE tmp = 0;
	do
	{
		tmp = ++g_misc_cnt | TYPE_MISC;
	}
	while( GetMiscHash().has( tmp ) );

	GetMiscHash().add( tmp, pObj );

	spinlock_leave( &s_MiscLock );

	return tmp;
}

void FreeMiscHandle( AR_HANDLE handle )
{
	spinlock_enter( &s_MiscLock );

	GetMiscHash().erase( handle );

	spinlock_leave( &s_MiscLock );
}

struct GameObject *	getMiscPtrFromId( AR_HANDLE uid )
{
	spinlock_enter( &s_MiscLock );

	GameObject *ptr = NULL;
	GetMiscHash().lookup( uid, ptr );

	spinlock_leave( &s_MiscLock );

	return ptr;
}

//template< typename T >
inline AR_HANDLE allocFromUserHeap( int type, XMemoryPool & heap, volatile LONG & lock, void ** p, GameAllocateFunctor & _fo )
{	
	AR_HANDLE id;
	spinlock_enter( &lock );
	*p = heap.Alloc();
	// id = ((__int64)(++s_UID)<<32) | heap.GetIndexFromPtr( *p ) | type;	// 64 bit
	unsigned int index = static_cast< unsigned int >( heap.GetIndexFromPtr( *p ) );

	if( index & TYPE_MASK ) 
	{
		MessageBox( NULL, "TOO BIG INDEX!", "CRITICAL ERROR", MB_OK );
		exit( 0 );
	}

	id = index | type;	// 32 bit	

	_fo( *p, id );
	spinlock_leave( &lock );
	return id;
}

//template< typename T >
inline void freeFromUserHeap( XMemoryPool & heap, volatile LONG & lock, void * p )
{
	// _oprint( "FREE  : 0x%08X\n", p );
	spinlock_enter( &lock );
	heap.Free( p );
	spinlock_leave( &lock );
}

inline void prepareFreeFromUserHeap( XMemoryPool & heap, volatile LONG & lock, void * p )
{
	// _oprint( "FREE  : 0x%08X\n", p );
	spinlock_enter( &lock );
	heap.PrepareFree( p );
	spinlock_leave( &lock );
}

inline void* getPtrFromIdWithoutLock( XMemoryPool & heap, unsigned id )
{
	void *p = heap.GetPtrFromIndex( id & ~TYPE_MASK );
	return p;
}

inline void* getPtrFromId( XMemoryPool & heap, volatile LONG & lock, unsigned id )
{
	spinlock_enter( &lock );
	void *p = heap.GetPtrFromIndex( id & ~TYPE_MASK );
	spinlock_leave( &lock );
	return p;
}

// { player alloc/free

AR_HANDLE allocPlayerStruct( StructPlayer** ppStruct, GameAllocateFunctor & _fo )
{
	return allocFromUserHeap( TYPE_PLAYER, GetPlayerHeap(), s_PlayerLock, (void**)ppStruct, _fo );
}

void prepareFreePlayerStruct( StructPlayer* pStruct )
{
	return prepareFreeFromUserHeap( GetPlayerHeap(), s_PlayerLock, (void*)pStruct );
}

void freePlayerStruct( StructPlayer* pStruct )
{
	return freeFromUserHeap( GetPlayerHeap(), s_PlayerLock, (void*)pStruct );
}

// }

// { summon alloc/free

AR_HANDLE allocSummonStruct( StructSummon** ppStruct, GameAllocateFunctor & _fo )
{
	return allocFromUserHeap( TYPE_SUMMON, GetSummonHeap(), s_SummonLock, (void**)ppStruct, _fo );
}

void prepareFreeSummonStruct( StructSummon* pStruct )
{
	return prepareFreeFromUserHeap( GetSummonHeap(), s_SummonLock, (void*)pStruct );
}

void freeSummonStruct( StructSummon* pStruct )
{
	return freeFromUserHeap( GetSummonHeap(), s_SummonLock, (void*)pStruct );
}

// }

// { pet alloc/free

AR_HANDLE allocPetStruct( StructPet** ppStruct, GameAllocateFunctor & _fo )
{
	return allocFromUserHeap( TYPE_PET, GetPetHeap(), s_PetLock, (void**)ppStruct, _fo );
}

void prepareFreePetStruct( StructPet* pStruct )
{
	return prepareFreeFromUserHeap( GetPetHeap(), s_PetLock, (void*)pStruct );
}

void freePetStruct( StructPet* pStruct )
{
	return freeFromUserHeap( GetPetHeap(), s_PetLock, (void*)pStruct );
}

// }

// { monster alloc/free

AR_HANDLE allocMonsterStruct( StructMonster ** ppStruct, GameAllocateFunctor & _fo )
{
	return allocFromUserHeap( TYPE_MONSTER, GetMonsterHeap(), s_MonsterLock, (void**)ppStruct, _fo );
}

void prepareFreeMonsterStruct( StructMonster* pStruct )
{
	return prepareFreeFromUserHeap( GetMonsterHeap(), s_MonsterLock, (void*)pStruct );
}

void freeMonsterStruct( StructMonster *pStruct )
{
	return freeFromUserHeap( GetMonsterHeap(), s_MonsterLock, (void*)pStruct );
}

// }

// { item alloc/free

AR_HANDLE allocItemStruct( StructItem ** ppStruct, GameAllocateFunctor & _fo )
{
	return allocFromUserHeap( TYPE_ITEM, GetItemHeap(), s_ItemLock, (void**)ppStruct, _fo );
}

void prepareFreeItemStruct( struct StructItem *pStruct )
{
	return prepareFreeFromUserHeap( GetItemHeap(), s_ItemLock, (void*)pStruct );
}

void freeItemStruct( struct StructItem *pStruct )
{
	return freeFromUserHeap( GetItemHeap(), s_ItemLock, (void*)pStruct );
}

// }

// { Skill alloc/free

struct StructSkill * allocSkillStruct()
{
	spinlock_enter( &s_SkillLock );
	StructSkill *pStruct = reinterpret_cast< StructSkill * >( GetSkillHeap().Alloc() );
	spinlock_leave( &s_SkillLock );

	return pStruct;
}

void prepareFreeSkillStruct( struct StructSkill *pStruct )
{
	return prepareFreeFromUserHeap( GetSkillHeap(), s_SkillLock, (void*)pStruct );
}

void freeSkillStruct( struct StructSkill *pStruct )
{
	return freeFromUserHeap( GetSkillHeap(), s_SkillLock, (void*)pStruct );
}

// }

// { Quest alloc/free

struct StructQuest * allocQuestStruct()
{
	spinlock_enter( &s_QuestLock );
	StructQuest *pStruct = reinterpret_cast< StructQuest * >( GetQuestHeap().Alloc() );
	spinlock_leave( &s_QuestLock );

	return pStruct;
}

void prepareFreeQuestStruct( struct StructQuest *pStruct )
{
	return prepareFreeFromUserHeap( GetQuestHeap(), s_QuestLock, (void*)pStruct );
}

void freeQuestStruct( struct StructQuest *pStruct )
{
	return freeFromUserHeap( GetQuestHeap(), s_QuestLock, (void*)pStruct );
}

// }

// { Title alloc/free

struct StructTitle * allocTitleStruct()
{
	spinlock_enter( &s_TitleLock );
	StructTitle *pStruct = reinterpret_cast< StructTitle * >( GetTitleHeap().Alloc() );
	spinlock_leave( &s_TitleLock );

	return pStruct;
}

void prepareFreeTitleStruct( struct StructTitle* pStruct )
{
	return prepareFreeFromUserHeap( GetTitleHeap(), s_TitleLock, (void*)pStruct );
}

void freeTitleStruct( StructTitle *pStruct )
{
	return freeFromUserHeap( GetTitleHeap(), s_TitleLock, (void*)pStruct );
}

// }

// { TitleCondition alloc/free

struct StructTitleCondition * allocTitleConditionStruct()
{
	spinlock_enter( &s_TitleConditionLock );
	StructTitleCondition *pStruct = reinterpret_cast< StructTitleCondition * >( GetTitleConditionHeap().Alloc() );
	spinlock_leave( &s_TitleConditionLock );

	return pStruct;
}

void prepareFreeTitleConditionStruct( struct StructTitleCondition* pStruct )
{
	return prepareFreeFromUserHeap( GetTitleConditionHeap(), s_TitleConditionLock, (void*)pStruct );
}

void freeTitleConditionStruct( StructTitleCondition *pStruct )
{
	return freeFromUserHeap( GetTitleConditionHeap(), s_TitleConditionLock, (void*)pStruct );
}

// }

// { Proc tag alloc/free

struct StructProc * allocProcStruct()
{
	spinlock_enter( &s_ProcLock );
	StructProc *pStruct = reinterpret_cast< StructProc * >( GetProcHeap().Alloc() );
	spinlock_leave( &s_ProcLock );

	return pStruct;
}

void prepareFreeProcStruct( struct StructProc * pStruct )
{
	return prepareFreeFromUserHeap( GetProcHeap(), s_ProcLock, (void*)pStruct );
}

void freeProcStruct( struct StructProc * pStruct )
{
	return freeFromUserHeap( GetProcHeap(), s_ProcLock, (void*)pStruct );
}

// }

StructPlayer*	getPlayerPtrFromId( AR_HANDLE uid )
{
	spinlock_enter( &s_PlayerLock );

	StructPlayer* pTmp = reinterpret_cast< StructPlayer* >( getPtrFromIdWithoutLock( GetPlayerHeap(), uid ) );

	if( pTmp && pTmp->GetHandle() == uid && !pTmp->IsDeleteRequested() )
	{
		spinlock_leave( &s_PlayerLock );
		return pTmp;
	}

	spinlock_leave( &s_PlayerLock );
	return NULL;
}

StructMonster*	getMonsterPtrFromId( AR_HANDLE uid )
{
	spinlock_enter( &s_MonsterLock );

	StructMonster* pTmp = reinterpret_cast< StructMonster* >( getPtrFromIdWithoutLock( GetMonsterHeap(), uid ) );

	if( pTmp && pTmp->GetHandle() == uid && !pTmp->IsDeleteRequested() ) 
	{
		spinlock_leave( &s_MonsterLock );
		return static_cast< StructMonster* >( pTmp );
	}

	spinlock_leave( &s_MonsterLock );
	return NULL;
}

StructItem*		getItemPtrFromId( AR_HANDLE uid )
{
	spinlock_enter( &s_ItemLock );

	StructItem* pTmp = reinterpret_cast< StructItem* >( getPtrFromIdWithoutLock( GetItemHeap(), uid ) );

	if( pTmp && pTmp->GetHandle() == uid && !pTmp->IsDeleteRequested() )
	{
		spinlock_leave( &s_ItemLock );
		return pTmp;
	}

	spinlock_leave( &s_ItemLock );
	return NULL;
}

StructSummon*		getSummonPtrFromId( AR_HANDLE uid )
{
	spinlock_enter( &s_SummonLock );

	StructSummon* pTmp = reinterpret_cast< StructSummon* >( getPtrFromIdWithoutLock( GetSummonHeap(), uid ) );

	if( pTmp && pTmp->GetHandle() == uid && !pTmp->IsDeleteRequested() )
	{
		spinlock_leave( &s_SummonLock );
		return pTmp;
	}

	spinlock_leave( &s_SummonLock );
	return NULL;
}

StructPet*		getPetPtrFromId( AR_HANDLE uid )
{
	spinlock_enter( &s_PetLock );

	StructPet* pTmp = reinterpret_cast< StructPet* >( getPtrFromIdWithoutLock( GetPetHeap(), uid ) );

	if( pTmp && pTmp->GetHandle() == uid && !pTmp->IsDeleteRequested() )
	{
		spinlock_leave( &s_PetLock );
		return pTmp;
	}

	spinlock_leave( &s_PetLock );
	return NULL;
}

GameObject* getPtrFromId( AR_HANDLE uid )
{
	switch( ( uid & TYPE_MASK ) )
	{
		case TYPE_PLAYER:	return getPlayerPtrFromId( uid );
		case TYPE_MONSTER:	return getMonsterPtrFromId( uid );
		case TYPE_ITEM:		return getItemPtrFromId( uid );
		case TYPE_SUMMON:	return getSummonPtrFromId( uid );
		case TYPE_PET:		return getPetPtrFromId( uid );
		case TYPE_MISC:		return getMiscPtrFromId( uid );
		default:			return NULL;
	}

	return NULL;
}