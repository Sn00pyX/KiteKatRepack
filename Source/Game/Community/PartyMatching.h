//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
#pragma once

#include <string>

struct StructPlayer;

class CPartyMatching
{
public:
	struct tagINFO
	{
		int id; // party id
		int num;
		int party;
		short level;
		short area;
		short max_member;
		std::string chant;
	};

	CPartyMatching();
	virtual ~CPartyMatching();

	inline const int GetIndex() const
	{
		return m_index;
	}
	inline void SetMaster(StructPlayer* pPlayer)
	{
		m_master = pPlayer;
	}
	inline StructPlayer* GetMaster()
	{
		return m_master;
	}
	inline const bool Empty() const
	{
		return !m_use;
	}
	inline void SetUse(const bool flag)
	{
		m_use = flag;
	}
	inline CPartyMatching::tagINFO& GetInfo()
	{
		return m_info;
	}
	void Clear();

protected:

private:
	bool m_use;
	int m_index;
	StructPlayer* m_master;
	struct tagINFO m_info;
};
//----------------------------------------------------------------------
