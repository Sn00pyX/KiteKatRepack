
#pragma once

#include <map>
#include <vector>
#include <string>


#include <lua/lua.hpp>

#include <toolkit/ILock.h>


typedef int (*luaFunction) (struct lua_State *L);

extern int time_test;

class LuaVM
{
public:
	bool Init();
	bool DeInit();

	void BindLogOutputFunction( void (*fp)( const char* ) );
	void Log( const char *szString );
	void LogPrintf( const char *szString, ... );


	void		ResetLoadedScriptList();
	const bool	LoadScript( const char* szFileName, const bool bEncrypted );
	bool		RunString( const char* szString, std::string * pstrReturnValue = NULL, int id = 0 );
	bool		RegisterFunction( const char* szFunctionName, luaFunction fp );

	static LuaVM*	GlobalInst();
	static LuaVM*	Inst();

	struct LUA_INFO
	{
		LUA_INFO() : pState( NULL ), cs( "LUA_INFO" )
		{
			prev_stack_top	= 0;
			arg_cnt			= 0;
		}

		int					prev_stack_top;
		int					arg_cnt;
		lua_State*			pState;
		XCriticalSection	cs;
	};

	static const char *			LUA_FILE_FORMAT_TAG;
	static const unsigned int	LUA_FILE_VERSION		= 0x00020000;
	static const unsigned int	LUA_FILE_VERSION2		= 0x00030000;
	static const unsigned int	LUA_LATEST_FILE_VERSION	= LUA_FILE_VERSION2;

	// 로딩되었던 스크립트 파일과 등록되었던 함수들만 다시 로딩/등록함
	bool Reload();

	bool InitThread( LuaVM::LUA_INFO** pLuaLocalInfo );
	bool DeInitThread();

private:

	LuaVM( bool bGlobalInstance = false );
	~LuaVM();

	struct _LOG_FUNC*	m_pLogFunc;

	struct LUA_INFO *	getLuaInfo();

	void				reload( lua_State* pState );
	const bool			doFile( lua_State* pState, const char * pszFilename, const bool bEncrypted, const unsigned int nStrictFileVersion = LUA_LATEST_FILE_VERSION );

	static std::vector< struct LUA_INFO * >		m_vStateList;
	static std::map< std::string, luaFunction >	m_mapFuncList;

	struct LuaScriptInfo
	{
		LuaScriptInfo( const char * pszFilename, const bool bEncrypted )
			: m_strFilename( pszFilename )
			, m_bEncrypted( bEncrypted )
		{}

		const bool operator ==( const LuaScriptInfo & rhs )
		{
			return m_strFilename == rhs.m_strFilename;
		}

		std::string		m_strFilename;
		bool			m_bEncrypted;
	};

	static std::vector< LuaScriptInfo >		m_vScriptList;

	const bool	m_bIsGlobalInstance;
	bool		m_bInit;
};

inline LuaVM*	LUA()
{
	return LuaVM::Inst();
}

void lua_pushstring_utf8( lua_State * L, const char * s );
std::string lua_tostring_utf8(lua_State *L, int idx);