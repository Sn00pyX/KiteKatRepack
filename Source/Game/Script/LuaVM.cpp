	
#include <cstdarg>
#include <algorithm>

#include <toolkit/XConsole.h>
#include <toolkit/XSTLUtil.h>
#include <toolkit/XENV.h>
#include <cipher/XEncrypt.h>
#include <logging/FileLog.h>

#include "LuaVM.h"


static XCriticalSection						s_CS( "LuaVMLock" );

LuaVM::LUA_INFO*							g_pGlobalLuaLocalInfo = NULL;	
__declspec( thread ) LuaVM::LUA_INFO*		g_pLuaLocalInfo = NULL;

const char *								LuaVM::LUA_FILE_FORMAT_TAG		= "LA";
std::vector< struct LuaVM::LUA_INFO * >		LuaVM::m_vStateList;
std::map< std::string, luaFunction >		LuaVM::m_mapFuncList;
std::vector< LuaVM::LuaScriptInfo >			LuaVM::m_vScriptList;
int time_test = 0;

static void enum_lua_stack(lua_State *L)
{
	int top = lua_gettop(L);
	_cprint( "Stack: %d", top);
	for( int i = 1; i <= lua_gettop(L); ++i )
	{
		switch( lua_type( L, i ) )
		{
		case LUA_TNIL:
			_cprint( "  %s", lua_typename(L, lua_type(L, i)));
			break;
		case LUA_TBOOLEAN:
			_cprint( "  %s: %s", lua_typename(L, lua_type(L, i)), lua_toboolean(L, i)?"true":"false");
			break;
		case LUA_TLIGHTUSERDATA:
			_cprint("  %s: 0x%p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TNUMBER:
			_cprint("  %s: %f", lua_typename(L, lua_type(L, i)), lua_tonumber(L, i));
			break;
		case LUA_TSTRING:
			_cprint("  %s: %s", lua_typename(L, lua_type(L, i)), lua_tostring(L, i));
			break;
		case LUA_TTABLE:
			_cprint("  %s: 0x%p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TFUNCTION:
			_cprint("  %s(): 0x%p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TUSERDATA:
			_cprint("  %s: 0x%p", lua_typename(L, lua_type(L, i)), lua_topointer(L, i));
			break;
		case LUA_TTHREAD:
			_cprint("  %s", lua_typename(L, lua_type(L, i)));
			break;
		}
	}

	_cprint("\n");
}

static void lua_call_stack(lua_State* L, int n=0)
{
	lua_Debug ar;
	if(lua_getstack(L, n, &ar) == 1)
	{
		lua_getinfo(L, "nSlu", &ar);

		if(n == 0)
		{
			_cprint("\t<call stack>\n");
		}

		if(ar.name)
			_cprint("  %s() : line %d [%s : line %d]\n", ar.name, ar.currentline, ar.source, ar.linedefined);
		else
			_cprint("  unknown : line %d [%s : line %d]\n", ar.currentline, ar.source, ar.linedefined);

		lua_call_stack(L, n+1);
	}
}

static int	lua_on_error(lua_State *L)
{
	_cprint("error: %s\n", lua_tostring(L, -1));
	lua_call_stack(L, 0);
	return 0;
}

static int _outputConsole(lua_State *L)
{
	int n = lua_gettop(L); /* number of arguments */
	for (int i = 1; i <= n; i++)
	{
		if( lua_isstring(L, i) )
		{
			LUA()->Log( lua_tostring( L, i ) );
			continue;
		}

		if( lua_isnumber(L, i) )
		{
			char szTmp[30];
			s_sprintf( szTmp, _countof( szTmp ), "%f", lua_tonumber( L, i ) );
			LUA()->Log( szTmp );
			continue;
		}
	}
	return 0;
}

/*
static int _lua_alert(lua_State *L)
{
	_outputConsole( L );
	return 0;
}*/

struct _LOG_FUNC
{
	void (*fp)( const char* );
};

LuaVM::LuaVM( bool bGlobalInstance )
	: m_bIsGlobalInstance( bGlobalInstance )
{
	m_pLogFunc = new _LOG_FUNC;

	m_pLogFunc->fp	= NULL;

	m_bInit = false;
}

LuaVM::~LuaVM()
{
	delete m_pLogFunc;
}

bool LuaVM::Init()
{
	THREAD_SYNCRONIZE( s_CS );

	if( m_bInit )
		return false;

	GlobalInst();
	Inst();

	m_bInit = true;
	return true;
}

bool LuaVM::DeInit()
{
	THREAD_SYNCRONIZE( s_CS );

	std::vector< struct LUA_INFO * >::iterator it;

	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		{
			THREAD_SYNCRONIZE( (*it)->cs );
			lua_close( (*it)->pState );
		}

		delete (*it);
	}

	m_vStateList.clear();

	return true;
}

void LuaVM::reload( lua_State* pState )
{
	{ // 함수들 등록
		std::map< std::string, luaFunction >::iterator it;

		for( it = m_mapFuncList.begin(); it != m_mapFuncList.end(); ++it )
		{
			lua_register( pState, it->first.c_str(), it->second );
		}
	}

	{ // 스크립트 파일 로딩
		std::vector< LuaScriptInfo >::iterator it;

		for( it = m_vScriptList.begin(); it != m_vScriptList.end(); ++it )
		{
			doFile( pState, (*it).m_strFilename.c_str(), (*it).m_bEncrypted );
		}
	}
}

const bool LuaVM::doFile( lua_State * pState, const char * pszFilename, const bool bEncrypted, const unsigned int nStrictFileVersion )
{
	FILE * fp = NULL;
	if( fopen_s( &fp, pszFilename, "rb" ) )
		return false;

	struct FileCloser
	{
		FileCloser( FILE * _fp ) : fp( _fp )	{}
		~FileCloser()							{ fclose( fp ); }

		FILE * fp;
	} fc( fp );

	size_t nContentsLength = 0;

	fseek( fp, 0, SEEK_END );
	nContentsLength = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	// No header
	if( !nContentsLength )
	{
		assert( 0 );
		return false;
	}

	BYTE * pBuffer = new BYTE[ nContentsLength+1 ];
	struct MemoryDeallocator
	{
		MemoryDeallocator( BYTE * _pBuffer ) : pBuffer( _pBuffer )	{}
		~MemoryDeallocator()										{ delete [] pBuffer; }

		BYTE * pBuffer;
	} md( pBuffer );

	size_t nReadNum = fread( pBuffer, sizeof( BYTE ), nContentsLength, fp );
	if( nReadNum != nContentsLength )
	{
		assert( 0 );
		return false;
	}

	pBuffer[nContentsLength] = 0;
	const char* pScript = reinterpret_cast< const char* >( pBuffer );
	std::vector< BYTE > vDecrypted;
	if( bEncrypted )
	{
		const unsigned int nVersion = XEncrypt::GetFormatVersion( LUA_FILE_FORMAT_TAG, pBuffer, nContentsLength );
		if( nVersion == XEncrypt::INVALID_VERSION || nStrictFileVersion && nVersion != nStrictFileVersion )
		{
			assert( 0 );
			return false;
		}

		switch( nVersion )
		{
		case LUA_FILE_VERSION:
			if( !XEncrypt::Decrypt( XEncrypt::ET_ZLIB64, LUA_FILE_FORMAT_TAG, LUA_FILE_VERSION, pBuffer, nContentsLength, vDecrypted ) )
			{
				assert( 0 );
				return false;
			}
			break;
		case LUA_FILE_VERSION2:
			if( !XEncrypt::Decrypt( XEncrypt::ET_ZLIB_WITH_SIMPLE_CIPHER, LUA_FILE_FORMAT_TAG, LUA_FILE_VERSION2, pBuffer, nContentsLength, vDecrypted ) )
			{
				assert( 0 );
				return false;
			}
			break;
		default:
			assert( 0 );
			return false;
		}

		if( vDecrypted.empty() )
			return true;

		vDecrypted.push_back( NULL );
		nContentsLength = vDecrypted.size();
		pScript = reinterpret_cast< char * >( &vDecrypted.front() );
	}

	const BYTE* pBOMCheck = reinterpret_cast< const BYTE* >( pScript );
	if( nContentsLength > 3 && pBOMCheck[0] == 0xEF && pBOMCheck[1] == 0xBB && pBOMCheck[2] == 0xBF )
	{
		pScript = &pScript[3];
	}

	int pre_idx = lua_gettop( pState );
	if( luaL_dostring( pState, pScript ) != 0 )
	{
		const char* err_msg = lua_tostring( pState, -1 );
		FILELOG( "Lua run doFile failed. %s(%s)", pszFilename, err_msg );
		_cprint( "Lua run doFile failed. %s(%s)\n", pszFilename, err_msg );

		lua_pop( pState, 1 );
		return false;
	}

	lua_settop( pState, pre_idx );	// 스텍 원상복구
	return true;
}

bool LuaVM::Reload()
{
	THREAD_SYNCRONIZE( s_CS );

	for( std::vector< struct LUA_INFO * >::iterator it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		THREAD_SYNCHRONIZE1( (*it)->cs );

		reload( (*it)->pState );
	}

	return true;
}

bool LuaVM::InitThread( LuaVM::LUA_INFO** pLuaLocalInfo )
{
	if( *pLuaLocalInfo ) return false;

	LUA_INFO * pLuaInfo = new LUA_INFO;

	pLuaInfo->pState = luaL_newstate();

	if( !pLuaInfo->pState )
	{
		assert( 0 && "lua_open() FAILED!!!" );
		delete pLuaInfo;
		return false;
	}

	luaopen_base( pLuaInfo->pState );
	luaopen_math( pLuaInfo->pState );
	luaopen_table( pLuaInfo->pState );

	lua_register( pLuaInfo->pState, "_ALERT", _outputConsole );
	lua_register( pLuaInfo->pState, "cprint", _outputConsole );


	THREAD_SYNCRONIZE( s_CS );

	reload( pLuaInfo->pState );
	lua_settop( pLuaInfo->pState, 0 );

	m_vStateList.push_back( pLuaInfo );
	*pLuaLocalInfo = pLuaInfo;
	return true;
}

bool LuaVM::DeInitThread()
{
	if( !g_pLuaLocalInfo ) return false;

	{
		THREAD_SYNCRONIZE( s_CS );

		std::vector< struct LUA_INFO * >::iterator it = std::find( m_vStateList.begin(), m_vStateList.end(), g_pLuaLocalInfo );

		if( it != m_vStateList.end() )
		{
			vector_fast_erase( &m_vStateList, it );
		}
	}

	{
		THREAD_SYNCRONIZE( g_pLuaLocalInfo->cs );
		lua_close( g_pLuaLocalInfo->pState );
	}

	delete g_pLuaLocalInfo;
	g_pLuaLocalInfo = NULL;

	return true;
}

void LuaVM::BindLogOutputFunction( void (*fp)( const char* ) )
{
	m_pLogFunc->fp = fp;
}

void LuaVM::Log( const char *szString )
{
	if( m_pLogFunc->fp ) 
	{
		m_pLogFunc->fp( szString );
		m_pLogFunc->fp( "\n" );
	}
	else 
	{
#ifndef _STOOL
	#ifndef _RAC
	//	FILELOG( "%s", szString );
		_cprint( "%s\n", szString );
	#endif
#endif
	}
}

void LuaVM::LogPrintf( const char *szString, ... )
{
	char szBuf[2048];
	va_list va;
	va_start( va, szString );
	s_vsprintf( szBuf, _countof(szBuf), szString, va );
	va_end( va );

	this->Log( szBuf );
}

void	LuaVM::ResetLoadedScriptList()
{
	THREAD_SYNCRONIZE( s_CS );
	m_vScriptList.clear();
}

const bool LuaVM::LoadScript( const char* szFileName, const bool bEncrypted )
{
	THREAD_SYNCRONIZE( s_CS );

	bool bResult = true;

	for( std::vector< struct LUA_INFO * >::iterator it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		THREAD_SYNCRONIZE( (*it)->cs );

		if( doFile( (*it)->pState, szFileName, bEncrypted ) == false )
		{
			bResult = false;
		}
	}

	LuaScriptInfo scriptInfo( szFileName, bEncrypted );

	std::vector< LuaScriptInfo >::iterator itScript = std::find( m_vScriptList.begin(), m_vScriptList.end(), scriptInfo );
	if( itScript == m_vScriptList.end() )
		m_vScriptList.push_back( scriptInfo );
	else
		(*itScript).m_bEncrypted = bEncrypted;

	return bResult;
}

bool LuaVM::RunString( const char* szString, std::string * pstrReturnValue, int id )
{
	LUA_INFO *pLuaInfo = getLuaInfo();

	if( !pLuaInfo ) return false;

	THREAD_SYNCRONIZE( pLuaInfo->cs );

	int pre_idx = lua_gettop( pLuaInfo->pState );
	int code_page = ENV().GetInt( "CodePage", CP_ACP );
	if( code_page >= 0 )
	{
		assert( strlen( szString ) < 1024 );

		wchar_t wszString[1024];
		MultiByteToWideChar( code_page, 0, szString, -1, wszString, _countof( wszString ) );

		char szUTFString[3*1024];
		WideCharToMultiByte( CP_UTF8, 0, wszString, -1, szUTFString, _countof( szUTFString ), NULL, NULL );

		if( luaL_dostring( pLuaInfo->pState, szUTFString ) != 0 )
		{
			const char* err_msg = lua_tostring( pLuaInfo->pState, -1 );
			FILELOG( "Lua run string failed. %d %s(%s)", id, szUTFString, err_msg );
			_cprint( "Lua run string failed. %d %s(%s)\n", id, szUTFString, err_msg );

			lua_pop( pLuaInfo->pState, 1 );
			return false;
		}
	}
	else
	{
		if( luaL_dostring( pLuaInfo->pState, szString ) != 0 )
		{
			const char* err_msg = lua_tostring( pLuaInfo->pState, -1 );
			FILELOG( "Lua run string failed. %d %s(%s)", id, szString, err_msg );
			_cprint( "Lua run string failed. %d %s(%s)\n", id, szString, err_msg );

			lua_pop( pLuaInfo->pState, 1 );
			return false;
		}
	}

	int ret_count = lua_gettop( pLuaInfo->pState );
	if( pstrReturnValue && pre_idx < ret_count )	// 리턴값이 있다면
	{
		int type = ::lua_type( pLuaInfo->pState, -1 );
		if( type == LUA_TBOOLEAN )
		{
			if( lua_toboolean( pLuaInfo->pState, -1 ) )
				*pstrReturnValue = "true";
			else
				*pstrReturnValue = "false";
		}
		else if( type == LUA_TNIL )
		{
			*pstrReturnValue = "nil";
		}
		else
		{
			*pstrReturnValue = lua_tostring_utf8( pLuaInfo->pState, -1 );
		}
	}

	lua_settop( pLuaInfo->pState, pre_idx );	// 스텍 원상복구
	return true;
}

bool LuaVM::RegisterFunction( const char* szFunctionName, luaFunction fp )
{
	THREAD_SYNCRONIZE( s_CS );

	std::vector< struct LUA_INFO * >::iterator it;

	for( it = m_vStateList.begin(); it != m_vStateList.end(); ++it )
	{
		THREAD_SYNCRONIZE( (*it)->cs );

		lua_register( (*it)->pState, szFunctionName, fp );
	}

	m_mapFuncList[szFunctionName] = fp;

	return true;
}

LuaVM::LUA_INFO *	LuaVM::getLuaInfo()
{
	if( m_bIsGlobalInstance )
	{
		if( !g_pGlobalLuaLocalInfo ) InitThread( &g_pGlobalLuaLocalInfo );
		return g_pGlobalLuaLocalInfo;
	}

	if( !g_pLuaLocalInfo ) InitThread( &g_pLuaLocalInfo );
	return g_pLuaLocalInfo;
}

LuaVM*	LuaVM::GlobalInst()
{
	static LuaVM inst( true );
	return &inst;
}

LuaVM*	LuaVM::Inst()
{
	static LuaVM inst;
	return &inst;
}

void lua_pushstring_utf8( lua_State * L, const char * s )
{
	assert( strlen( s ) < 1024 );

	wchar_t wszString[1024];
	MultiByteToWideChar( ENV().GetInt( "CodePage", CP_ACP ), 0, s, -1, wszString, _countof( wszString ) );

	char szUTFString[3*1024];
	WideCharToMultiByte( CP_UTF8, 0, wszString, -1, szUTFString, _countof( szUTFString ), NULL, NULL );

	lua_pushstring( L, szUTFString );
}

std::string lua_tostring_utf8( lua_State *L, int idx )
{
	// 문자열로 인식할 수 없는 파라미터(따옴표 빼먹는게 주 원인 -_ -)를 가져오려고 했다면 빈 문자열 반환
	if( !lua_isstring( L, idx ) ) return "";

	const char * s = lua_tostring( L, idx );

	assert( strlen( s ) < 1024 );

	wchar_t wszString[1024];
	if( MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, s, -1, wszString, _countof( wszString ) ) <= 0 )
	{
		return s;
	}

	char szMBCSString[3*1024];
	WideCharToMultiByte( ENV().GetInt( "CodePage", CP_ACP ), 0, wszString, -1, szMBCSString, _countof( szMBCSString ), NULL, NULL );

	return szMBCSString;
}