#include "UIDManager.h"

UIDManager::~UIDManager()
{
	m_hsUIDCounter.clear();
}

UIDManager & UIDManager::Instance()
{
	static UIDManager _instance;

	return _instance;
}

const int UIDManager::GetNewUID( const char *szTypeName )
{
	THREAD_SYNCHRONIZE( m_UIDLock );

	int nUIDCounter = 0;

	if( !m_hsUIDCounter.lookup( szTypeName, nUIDCounter ) )
	{
		m_hsUIDCounter.add( szTypeName, 0 );
	}

	m_hsUIDCounter.modify( szTypeName, ++nUIDCounter );

	return nUIDCounter;
}

const bool UIDManager::ClearUID( const char *szTypeName )
{
	THREAD_SYNCHRONIZE( m_UIDLock );

	return m_hsUIDCounter.erase( szTypeName );
}

const bool UIDManager::SetMaxUsedUID( const char *szTypeName, const int nNewMaxUsedID )
{
	THREAD_SYNCHRONIZE( m_UIDLock );

	int nMaxUsedID = 0;

	if( !m_hsUIDCounter.lookup( szTypeName, nMaxUsedID ) )
	{
		m_hsUIDCounter.add( szTypeName, nNewMaxUsedID );
	}
	else if( nMaxUsedID < nNewMaxUsedID )
	{
		m_hsUIDCounter.modify( szTypeName, nNewMaxUsedID );
	}

	return true;
}
