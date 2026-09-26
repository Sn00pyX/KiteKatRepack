//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
#pragma once

#include <deque>
#include <string>
#include "..\Struct\StructPlayer.h"
#include "CriticalSection.h"

const int MAX_PARTY_MATCHING = 500;

class CPartyMatchingManager
{
public:
	CPartyMatchingManager();
	~CPartyMatchingManager();

	static CPartyMatchingManager* GetInstance()
	{
		return m_Instance;
	}
	inline CCriticalSection* GetCS()
	{
		return &m_cs;
	}

	void Destroy();
	CPartyMatching* Get(const int index); // 방번호로 찾기
	CPartyMatching* Get(StructPlayer* pPlayer); // 파장으로 찾기
	void Add(StructPlayer* pPlayer, void* pInfo);
	void Remove(const int index);
	void Remove(StructPlayer* pPlayer);
	void ShowList(StructPlayer* pPlayer, int page, int area);
	void SendMember(StructPlayer* pPlayer, const int index);
	void Request(StructPlayer* pPlayer);
	void SendMemberAsChange(StructPlayer* pPlayer);
	void ChangeMaster(StructPlayer* pCurrent, StructPlayer* pSuccessor);
	bool CheckNumber(const int party_id);

protected:

private:
	void SendList(StructPlayer* pPlayer, int page, std::deque<CPartyMatching *>& show_list);

	CPartyMatching** m_base;
	static CPartyMatchingManager* m_Instance;
	std::deque<CPartyMatching *> m_list;
	CCriticalSection m_cs;
};
//----------------------------------------------------------------------
