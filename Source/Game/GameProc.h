#pragma once

// 대개의 게임 오퍼레이션을 행한다.
// 락은 일어나지 않는다. 미리 해둘것.

// 테이밍 세팅
bool SetTamer( struct StructMonster *pMonster, struct StructPlayer *pPlayer, struct StructSkill *pSkill );

// 테이밍 풀림
void ClearTamer( struct StructMonster *pMonster, bool bBroadcastMsg = true );

// 테이밍
bool ProcTame( struct StructMonster *pMonster );

// 스킬카드 강화
bool EnhanceSkillCard( struct StructPlayer *pPlayer, struct StructItem *pCube, struct StructItem *pSkillCard1, struct StructItem *pSkillCard2 );

// 아이템 카드화
bool TransformToCard( StructPlayer *pPlayer, StructItem *pItem, StructItem *pCard, bool bIsTransformToCard );

// 몬스터가 월드에 아이템을 떨어트림
void MonsterDropItemToWorld( struct StructCreature *pCreature, struct StructItem * pItem );

// 월드에 아이템을 떨어트림
void AddItemToWorld( struct StructItem * pItem );

// 월드에 필드프랍을 생성
void AddFieldPropToWorld( struct StructFieldProp * pProp );

// 월드에서 아이템을 제거함(ItemCollector::onProcess 에서 삭제처리 중인 아이템이면 실패함)
const bool RemoveItemFromWorld( struct StructItem * pItem );

// 월드에서 Player를 제거
void RemovePlayerFromWorld( struct StructPlayer * pPlayer );

// 월드에 몬스터를 추가
void AddMonsterToWorld( struct StructMonster * pMob );

// 월드에서 몬스터를 제거
void RemoveMonsterFromWorld( struct StructMonster * pMob );

// 월드에 NPC를 추가
void AddNPCToWorld( struct StructNPC * pMob );

// 현재 추가중인 NPC
struct StructNPC * GetCurrentInitializingNPC();

// 월드에서 NPC를 제거
void RemoveNPCFromWorld( struct StructNPC * pMob );

struct StructSummon * AllocNewSummon( struct StructPlayer* pMaster, struct StructItem* pCard );

// 월드에 소환수를 추가
void AddSummonToWorld( struct StructSummon * pSummon );

// 월드에서 소환수를 제거
void RemoveSummonFromWorld( struct StructSummon * pSummon );

struct StructPet * AllocNewPet( struct StructPlayer* pMaster, struct StructItem* pCage, const int nPetCode );

// 월드에 소환수를 추가
void AddPetToWorld( struct StructPet * pPet );

// 월드에서 소환수를 제거
void RemovePetFromWorld( struct StructPet * pPet );

// 워프
void WarpBegin( struct StructPlayer *pPlayer );
void WarpEnd( struct StructPlayer *pPlayer, const struct ArPosition * pPosition, unsigned char layer );

// 플레이어 스탯 저장
void SavePlayer( struct StructPlayer *pPlayer );

void SaveAllPlayer();
