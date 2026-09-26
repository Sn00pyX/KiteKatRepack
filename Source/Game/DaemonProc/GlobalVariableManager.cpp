#include "GlobalVariableManager.h"
#include "DB_Commands.h"


GlobalVariableManager & GlobalVariableManager::Instance()
{
	static GlobalVariableManager _instance;
	return _instance;
}

GlobalVariableManager::~GlobalVariableManager()
{
	KHash< std::string *, hashPr_string >::node * pNode = NULL;

	THREAD_SYNCHRONIZE( m_cs );

	bool bNodeExist = m_hsGlobalVariable.get_first_node( pNode );
	while( bNodeExist )
	{
		bNodeExist = m_hsGlobalVariable.get_next_node( pNode );
	}
}

void GlobalVariableManager::Push( GameDBManager::DBProc* pWork )
{
	THREAD_SYNCRONIZE( m_QueryLock );

	if( m_qQueryList.empty() )
	{
		DB().Push( pWork );
	}

	m_qQueryList.push( pWork );
}

void GlobalVariableManager::onEndQuery()
{
	THREAD_SYNCRONIZE( m_QueryLock );

	m_qQueryList.pop();

	if( !m_qQueryList.empty() )
	{
		DB().Push( m_qQueryList.front() );
	}
}

void GlobalVariableManager::Set( const char * pszName, const char * pszValue, const bool bSkipDBUpdate )
{
	THREAD_SYNCHRONIZE( m_cs );

	std::string * pStrValue = NULL;

	if( m_hsGlobalVariable.lookup( pszName, pStrValue ) )
	{
		*pStrValue = pszValue;

		if( !bSkipDBUpdate )
			Push( new DB_UpdateGlobalVariable( pszName, pszValue ) );
	}
	else
	{
		pStrValue = new std::string( pszValue );
		m_hsGlobalVariable.add( pszName, pStrValue );

		if( !bSkipDBUpdate )
			Push( new DB_InsertGlobalVariable( pszName, pszValue ) );
	}
}

const char * GlobalVariableManager::Get( const char * pszName ) const
{
	THREAD_SYNCHRONIZE( m_cs );

	std::string * pStrValue = NULL;

	if( !m_hsGlobalVariable.lookup( pszName, pStrValue ) )
		return NULL;

	return pStrValue->c_str();
}

const bool GlobalVariableManager::Delete( const char * pszName )
{
	THREAD_SYNCHRONIZE( m_cs );

	if( !m_hsGlobalVariable.erase( pszName ) )
	{
		return false;
	}

	Push( new DB_DeleteGlobalVariable( pszName ) );

	return true;
}
