//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------
#pragma once
class CCriticalSection
{
public:

	CCriticalSection()
	{
		//InitializeCriticalSection(&m_csLock);
		DWORD spin_count = 4000;
		::InitializeCriticalSectionAndSpinCount(&m_csLock, spin_count);
	}

	~CCriticalSection()
	{
		DeleteCriticalSection(&m_csLock);
	}

	void Lock(void)
	{
		EnterCriticalSection(&m_csLock);
	}

	void Unlock(void)
	{
		LeaveCriticalSection(&m_csLock);
	}

	void SetSpinCount(DWORD spin_count)
	{
		::SetCriticalSectionSpinCount(&m_csLock, spin_count);
	}

private:

	CRITICAL_SECTION m_csLock;
};


class CLock
{
public:
	CLock(CCriticalSection* pCS) : m_cs(pCS)
	{
		m_cs->Lock();
	}
	virtual ~CLock()
	{
		m_cs->Unlock();
	}

protected:

private:
	CCriticalSection* m_cs;
};
//----------------------------------------------------------------------
