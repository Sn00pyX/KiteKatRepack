#pragma once

#include <toolkit/ILock.h>
#include <toolkit/KHash.h>

class UIDManager
{
public:
	virtual ~UIDManager();

	static UIDManager & Instance();

	const int GetNewUID( const char *szTypeName );
	const bool ClearUID( const char *szTypeName );

	// UIDManager가 가지고 있는 이미 사용된 최대 ID값과 nNewMaxUsedID 중 큰 것을 이미 사용된 최대 ID값으로 설정
	const bool SetMaxUsedUID( const char *szTypeName, const int nNewMaxUsedID );

private:
	UIDManager()
		: m_UIDLock( "UIDManager::m_UIDLock" )
	{}

private:
	XCriticalSection				m_UIDLock;
	KHash< int, hashPr_string >		m_hsUIDCounter;
};
