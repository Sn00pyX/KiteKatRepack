//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
#include "PartyMatchingManager.h"
#include "Resource/KTypes.h"
#include "../Message/GameMessage.h"
#include "../Message/SendMessage.h"
#include "../Resource/GameContent.h"
#include "PartyManager.h"

CPartyMatchingManager* CPartyMatchingManager::m_Instance = NULL;
CPartyMatchingManager party_matching_manager;
//----------------------------------------------------------------------
CPartyMatchingManager::CPartyMatchingManager()
{
	int n;
	m_Instance = this;
	m_list.clear();

	m_base = (CPartyMatching **) ::new CPartyMatching* [MAX_PARTY_MATCHING];
	for(n = 0; n < MAX_PARTY_MATCHING; n++)
	{
		m_base[n] = new CPartyMatching();
	}
}
//----------------------------------------------------------------------
CPartyMatchingManager::~CPartyMatchingManager()
{
}
//----------------------------------------------------------------------
void CPartyMatchingManager::Destroy()
{
	int n;
	for(n = 0; n < MAX_PARTY_MATCHING; n++)
	{
		SAFE_DELETE(m_base[n]);
	}
	SAFE_DELETE_ARRAY(m_base);

	m_Instance = NULL;
}
//----------------------------------------------------------------------
CPartyMatching* CPartyMatchingManager::Get(const int index)
{
	CLock lock(&m_cs);
	auto iter = m_list.begin();
	for(; iter != m_list.end(); ++iter)
	{
		if((*iter)->GetIndex() == index)
		{
			return *iter;
		}
	}
	return NULL;
}
//----------------------------------------------------------------------
CPartyMatching* CPartyMatchingManager::Get(StructPlayer* pPlayer)
{
	CLock lock(&m_cs);
	auto iter = m_list.begin();
	for(; iter != m_list.end(); ++iter)
	{
		if((*iter)->GetMaster() == pPlayer)
		{
			return *iter;
		}
	}
	return NULL;
}
//----------------------------------------------------------------------
void CPartyMatchingManager::Add(StructPlayer* pPlayer, void* pInfo)
{
	CLock lock(&m_cs);
	struct STRUCT_PARTYMATCH_ROOM* pData = reinterpret_cast<struct STRUCT_PARTYMATCH_ROOM *>(pInfo);
	pData->szText[PARTYMATCH_ROOM_TITLE - 1] = '\0';

	if(pPlayer->GetPartyMatching())
	{
		// 이미 파티매칭에 속해있다
		return;
	}
	if(false == PartyManager::GetInstance().IsLeader(pPlayer->GetPartyID(), pPlayer->GetPlayerUID()))
	{
		// 파장이 아니다
		return;
	}

	int n;
	CPartyMatching* pPartyMatching = NULL;
	for(n = 0; n < MAX_PARTY_MATCHING; n++)
	{
		if(m_base[n]->Empty())
		{
			pPartyMatching = m_base[n];
			break;
		}
	}
	if(NULL == pPartyMatching)
	{
		return;
	}
	CPartyMatching::tagINFO& info = pPartyMatching->GetInfo();
	info.id = pPlayer->GetPartyID();
	info.area = pData->nArea;
	info.level = pData->nLevel;
	info.num = pData->nParty;
	info.chant = pData->szText;
	info.max_member = pData->nMaxMember;
	info.party = pData->nParty;
	pPartyMatching->SetUse(true);
	pPartyMatching->SetMaster(pPlayer);
	pPlayer->SetPartyMatching(pPartyMatching);
	m_list.push_front(pPartyMatching);
}
//----------------------------------------------------------------------
void CPartyMatchingManager::Remove(const int index)
{
	CLock lock(&m_cs);
	auto iter = m_list.begin();
	for(; iter != m_list.end(); ++iter)
	{
		if((*iter)->GetIndex() == index)
		{
			(*iter)->GetMaster()->SetPartyMatching(NULL);
			(*iter)->Clear();
			m_list.erase(iter);
			return;
		}
	}
}
//----------------------------------------------------------------------
void CPartyMatchingManager::Remove(StructPlayer* pPlayer)
{
	CLock lock(&m_cs);
	auto iter = m_list.begin();
	for(; iter != m_list.end(); ++iter)
	{
		if((*iter)->GetMaster() == pPlayer)
		{
			(*iter)->GetMaster()->SetPartyMatching(NULL);
			(*iter)->Clear();
			m_list.erase(iter);
			return;
		}
	}
}
//----------------------------------------------------------------------
void CPartyMatchingManager::ShowList(StructPlayer* pPlayer, int page, int area)
{
	CLock lock(&m_cs);

	if(0 == area)
	{
		CPartyMatchingManager::GetInstance()->SendList(pPlayer, page, m_list);
	}
	else
	{
		std::deque<CPartyMatching *> show_list;
		auto iter = m_list.begin();
		for(; iter != m_list.end(); ++iter)
		{
			if((*iter)->GetInfo().area == area)
			{
				show_list.push_back(*iter);
			}
		}
		CPartyMatchingManager::GetInstance()->SendList(pPlayer, page, show_list);
	}
}
//----------------------------------------------------------------------
void CPartyMatchingManager::SendList(StructPlayer* pPlayer, int page, std::deque<CPartyMatching *>& show_list)
{
	if(page < 1)
	{
		page = 1;
	}
	int start = (page - 1) * PARTYMATCH_ROOM_PAGE;
	TS_SC_PARTYMATCH_LIST msg;

	msg.nRoomTotal = static_cast<int>(show_list.size());
	if(start >= msg.nRoomTotal)
	{
		::ZeroMemory(msg.pRoom, sizeof(msg.pRoom));
	}
	else
	{
		int end = msg.nRoomTotal - start;
		int n, cnt = 0;
		auto iter = show_list.begin() + start;
		for(n = 0; n < end; n++)
		{
			StructPlayer* pMaster = (*iter)->GetMaster();
			if(NULL == pMaster)
			{
				iter = show_list.erase(iter);
				continue;
			}
			int member_count = PartyManager::GetInstance().GetMemberCount(pMaster->GetPartyID());
			if(member_count <= 0)
			{
				iter = show_list.erase(iter);
				continue;
			}
			if(member_count >= (*iter)->GetInfo().max_member)
			{
				continue;
			}
			CPartyMatching* pMatching = *iter;
			STRUCT_PARTYMATCH_ROOM& pRoom = msg.pRoom[cnt];
			pRoom.nRoom = pMatching->GetIndex();
			pRoom.nParty = member_count;
			pRoom.nArea = pMatching->GetInfo().area;
			pRoom.nLevel = pMatching->GetInfo().level;
			pRoom.nMaxMember = pMatching->GetInfo().max_member;
			pRoom.nMember = static_cast<short>(member_count);
			pRoom.nParty = pMatching->GetInfo().party;
			sprintf_s(pRoom.szText, _countof(pRoom.szText), "%s", pMatching->GetInfo().chant.c_str());
			cnt++;
			if(cnt >= PARTYMATCH_ROOM_PAGE)
			{
				break;
			}
			iter++;
		}
	}
	msg.nPage = page;
	PendMessage(pPlayer, &msg);
}
//----------------------------------------------------------------------
void CPartyMatchingManager::SendMember(StructPlayer* pPlayer, const int index)
{
	CLock lock(&m_cs);

	TS_SC_PARTYMATCH_MEMBER msg;
	::ZeroMemory(msg.pMember, sizeof(msg.pMember));

	CPartyMatching* pMatching = Get(index);
	if(NULL == pMatching)
	{
		return;
	}
	if(NULL == pMatching->GetMaster())
	{
		Remove(index);
		return;
	}
	msg.nMaster = StructPlayer::FindPlayer(pMatching->GetMaster()->GetName());
	if(0 == msg.nMaster)
	{
		Remove(index);
		return;
	}
	msg.nRoom = index;
	STRUCT_PARTYMATCH_MEMBER* pMember = msg.pMember;

	const PartyManager::PartyInfo* pPartyInfo = PartyManager::GetInstance().getPartyInfo(pMatching->GetMaster()->GetPartyID());
	if(NULL == pPartyInfo)
	{
		return;
	}

	int cnt = 0;
	std::vector<PartyManager::PartyMemberTag>::const_iterator iter;
	for(iter = pPartyInfo->vMemberNameList.begin(); iter != pPartyInfo->vMemberNameList.end(); ++iter)
	{
		pMember[cnt].nMode = 0x80000000 | (iter->nLevel << 16) | iter->nJobId;
		pMember[cnt].nMember = StructPlayer::FindPlayer(iter->strName.c_str());
		sprintf_s(pMember[cnt].szName, _countof(pMember[cnt].szName), "%s", iter->strName.c_str());
		cnt++;
	}
	PendMessage(pPlayer, &msg);
	pPlayer->SetPartyMatchingLeaderName(pPartyInfo->strLeaderName.c_str());
}
//----------------------------------------------------------------------
void CPartyMatchingManager::Request(StructPlayer* pPlayer)
{
	CLock lock(&m_cs);

	if(pPlayer->GetPartyMatchingLeaderName().length() <= 0)
	{
		return;
	}

	if(!_stricmp(pPlayer->GetName(), pPlayer->GetPartyMatchingLeaderName().c_str()))
	{
		return;
	}

	char chat_msg[256];
	sprintf_s(chat_msg, sizeof(chat_msg), "/requestparty %s %d %d", pPlayer->GetName(), pPlayer->GetLevel(), pPlayer->GetJobId());

	// 메세지 헤더
	TS_SC_CHAT msg;
	msg.type = CHAT_PARTY_SYSTEM;
	sprintf_s(msg.szSender, sizeof(msg.szSender), "%s", pPlayer->GetName());
	msg.len = (unsigned char)(strlen(chat_msg) + 1);

	// 문자열 크기만큼 사이즈 늘림
	msg.size += static_cast<unsigned int>(msg.len);

	if(msg.size > SENDMSG_BUFFER_SIZE)
	{
		FILELOG("SendLocalChatMessage too big : %d", msg.size);
		return;
	}

	// 메모리 할당
	char pBuffer[512];

	// 버퍼에 헤더 복사
	s_memcpy(pBuffer, sizeof(pBuffer), &msg, sizeof(msg));

	// 버퍼에 문자열 복사
	s_memcpy(pBuffer + sizeof(msg), sizeof(pBuffer) - sizeof(msg), chat_msg, msg.len);

	// '\0' 추가
	pBuffer[msg.size - 1] = '\0';

	StructPlayer* pSender = *StructPlayer::get(StructPlayer::FindPlayer(pPlayer->GetPartyMatchingLeaderName().c_str()));
	if(pSender)
	{
		PendMessage(pSender, reinterpret_cast<TS_MESSAGE *>(pBuffer));
	}
}
//----------------------------------------------------------------------
void CPartyMatchingManager::SendMemberAsChange(StructPlayer* pPlayer)
{
	// pPlayer는 파티장.

	CLock lock(&m_cs);

	CPartyMatching* pMatching = Get(pPlayer);
	if(NULL == pMatching)
	{
		return;
	}

	TS_SC_PARTYMATCH_MEMBER msg;
	::ZeroMemory(msg.pMember, sizeof(msg.pMember));

	msg.nMaster = StructPlayer::FindPlayer(pPlayer->GetName());
	if(0 == msg.nMaster)
	{
		Remove(pMatching->GetIndex());
		return;
	}
	msg.nRoom = pMatching->GetIndex();
	STRUCT_PARTYMATCH_MEMBER* pMember = msg.pMember;

	const PartyManager::PartyInfo* pPartyInfo = PartyManager::GetInstance().getPartyInfo(pPlayer->GetPartyID());
	if(NULL == pPartyInfo)
	{
		return;
	}

	// setup packet
	int cnt = 0;
	std::vector<PartyManager::PartyMemberTag>::const_iterator iter;
	for(iter = pPartyInfo->vMemberNameList.begin(); iter != pPartyInfo->vMemberNameList.end(); ++iter)
	{
		pMember[cnt].nMode = 0x80000000 | (iter->nLevel << 16) | iter->nJobId;
		pMember[cnt].nMember = StructPlayer::FindPlayer(iter->strName.c_str());
		sprintf_s(pMember[cnt].szName, _countof(pMember[cnt].szName), "%s", iter->strName.c_str());
		cnt++;
	}

	// send packet to members
	for(iter = pPartyInfo->vMemberNameList.begin(); iter != pPartyInfo->vMemberNameList.end(); ++iter)
	{
		StructPlayer* pSender = *StructPlayer::get(StructPlayer::FindPlayer(iter->strName.c_str()));
		PendMessage(pSender, &msg);
	}
}
//----------------------------------------------------------------------
void CPartyMatchingManager::ChangeMaster(StructPlayer* pCurrent, StructPlayer* pSuccessor)
{
	CLock lock(&m_cs);
	auto iter = m_list.begin();
	for(; iter != m_list.end(); ++iter)
	{
		if((*iter)->GetMaster() == pCurrent)
		{
			(*iter)->SetMaster(pSuccessor);
			return;
		}
	}
}
//----------------------------------------------------------------------
bool CPartyMatchingManager::CheckNumber(const int party_id)
{
	CLock lock(&m_cs);
	auto iter = m_list.begin();
	for(; iter != m_list.end(); ++iter)
	{
		if((*iter)->GetInfo().id == party_id)
		{
			const PartyManager::PartyInfo* pPartyInfo = PartyManager::GetInstance().getPartyInfo(party_id);
			if(pPartyInfo)
			{
				if((*iter)->GetInfo().max_member >= static_cast<int>(pPartyInfo->vMemberNameList.size()))
				{
					return false;
				}
			}
		}
	}
	return true;
}
//----------------------------------------------------------------------
