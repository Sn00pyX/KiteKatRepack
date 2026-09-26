#pragma once

#include <string>
#include <queue>

#include <toolkit/ILock.h>
#include <toolkit/KHash.h>

#include "GameDBManager.h"



struct GlobalVariableManager
{
public:
	~GlobalVariableManager();

	static GlobalVariableManager & Instance();

	void			Push( GameDBManager::DBProc * pWork );
	void			onEndQuery();

	void			Set( const char * pszName, const char * pszValue, const bool bSkipDBUpdate = false );
	const char *	Get( const char * pszName ) const;
	const bool		Delete( const char * pszName );

protected:
	GlobalVariableManager()
		: m_cs( "GlobalVariableManager::m_cs" )
	{}

private:
	// 모든 락의 말단에서 걸림(GlobalVariableManager 안에서 타 락을 걸지 않음)
	mutable XCriticalSection				m_cs;

	KHash< std::string *, hashPr_string >	m_hsGlobalVariable;

	XCriticalSection						m_QueryLock;
	std::queue< GameDBManager::DBProc* >	m_qQueryList;
};

#define GVM GlobalVariableManager::Instance
