#pragma once

#include <mmo/ArOption.h>

const unsigned int TYPE_MASK		= 0xE0000000;
const unsigned int TYPE_PET			= 0xE0000000;
const unsigned int TYPE_SUMMON		= 0xC0000000;
const unsigned int TYPE_PLAYER		= 0x80000000;
const unsigned int TYPE_MONSTER		= 0x40000000;
const unsigned int TYPE_MISC		= 0x20000000;
const unsigned int TYPE_ITEM		= 0x00000000;

inline bool						IsPet( AR_HANDLE uid )		{ return TYPE_PET == ( TYPE_MASK & uid ); }
inline bool						IsSummon( AR_HANDLE uid )	{ return TYPE_SUMMON == ( TYPE_MASK & uid ); }
inline bool						IsPlayer( AR_HANDLE uid )	{ return TYPE_PLAYER == ( TYPE_MASK & uid ); }
inline bool						IsItem( AR_HANDLE uid )		{ return TYPE_ITEM == ( TYPE_MASK & uid ); }
inline bool						IsMonster( AR_HANDLE uid )	{ return TYPE_MONSTER == ( TYPE_MASK & uid ); }

extern struct XMemoryPool &		GetProcHeap();

extern int						GetPlayerHeapSize();
extern int						GetMonsterHeapSize();
extern int						GetItemHeapSize();
extern int						GetSummonHeapSize();
extern int						GetPetHeapSize();
extern int						GetSkillHeapSize();
extern int						GetQuestHeapSize();
extern int						GetTitleHeapSize();
extern int						GetTitleConditionHeapSize();
extern int						GetProcHeapSize();
extern void						InitGameHeap();

inline unsigned					GetObjectType( AR_HANDLE uid )		{ return ( static_cast< unsigned >( uid ) & TYPE_MASK ); }

AR_HANDLE						AllocMiscHandle( struct GameObject* pObj );
void							FreeMiscHandle( AR_HANDLE handle );

struct GameAllocateFunctor
{
	virtual void operator()( void * pObj, AR_HANDLE handle ) = 0;
};

extern AR_HANDLE allocPlayerStruct( struct StructPlayer** ppStruct, GameAllocateFunctor & _fo );
extern void prepareFreePlayerStruct( struct StructPlayer* pStruct );
extern void freePlayerStruct( struct StructPlayer* pStruct );

extern AR_HANDLE allocSummonStruct( struct StructSummon** ppStruct, GameAllocateFunctor & _fo );
extern void prepareFreeSummonStruct( struct StructSummon* pStruct );
extern void freeSummonStruct( struct StructSummon* pStruct );

extern AR_HANDLE allocPetStruct( struct StructPet** ppStruct, GameAllocateFunctor & _fo );
extern void prepareFreePetStruct( struct StructPet* pStruct );
extern void freePetStruct( struct StructPet* pStruct );

extern AR_HANDLE allocMonsterStruct( struct StructMonster** ppStruct, GameAllocateFunctor & _fo );
extern void prepareFreeMonsterStruct( struct StructMonster* pStruct );
extern void freeMonsterStruct( struct StructMonster* pStruct );

extern AR_HANDLE allocItemStruct( struct StructItem** ppStruct, GameAllocateFunctor & _fo );
extern void prepareFreeItemStruct( struct StructItem* pStruct );
extern void freeItemStruct( struct StructItem* pStruct );

extern struct StructSkill * allocSkillStruct();
extern void prepareFreeSkillStruct( struct StructSkill* pStruct );
extern void freeSkillStruct( struct StructSkill* pStruct );

extern struct StructQuest * allocQuestStruct();
extern void prepareFreeQuestStruct( struct StructQuest* pStruct );
extern void freeQuestStruct( struct StructQuest* pStruct );

extern struct StructTitle * allocTitleStruct();
extern void prepareFreeTitleStruct( struct StructTitle* pStruct );
extern void freeTitleStruct( struct StructTitle* pStruct );

extern struct StructTitleCondition * allocTitleConditionStruct();
extern void prepareFreeTitleConditionStruct( struct StructTitleCondition* pStruct );
extern void freeTitleConditionStruct( struct StructTitleCondition* pStruct );

extern struct StructProc * allocProcStruct();
extern void prepareFreeProcStruct( struct StructProc * pStruct );
extern void freeProcStruct( struct StructProc * pStruct );