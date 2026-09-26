#pragma once

#include <string>
#include <list>


// 스크립트 파일
#define NFSFILE_SIGN "nFlavor Script"
const DWORD c_dwNFSCurrentVer = 2;

#pragma pack(push, 1)
struct NFS_HEADER_V02
{
	char szSign[16];
	DWORD dwVersion;
	DWORD dwEventLocationOffset; // 이벤트 영역 정보 시작 위치
	DWORD dwEventScriptOffset; // 이벤트 영역 스크립트 정보 시작 위치
	DWORD dwPropScriptOffset; // 장식물 스크립트 정보 시작 위치
};
typedef NFS_HEADER_V02 NFS_HEADER_V01;
typedef NFS_HEADER_V02 NFS_LATESTHEADER;
#pragma pack(pop)

// Trigger 정의 (대상 타입 -NPC, EventLoc등- 에 따라 이중 일부만을 사용하게 될 것이다.)
enum SCRIPT_TRIGGER
{
	SCRTRG_INITIALIZE = 0,
	SCRTRG_CONTACT = 1,
	SCRTRG_ENTER = 2,
	SCRTRG_LEAVE = 3,
	SCRTRG_CLIENTWATER = 4,
	SCRTRG_MINIMAPINFO = 5,
};

// Trigger와 Function이름
struct ScriptFunction
{
	SCRIPT_TRIGGER m_Trigger;
	std::string m_strFuncName;

	ScriptFunction( SCRIPT_TRIGGER Trigger, const char* szFuncName )
		: m_Trigger( Trigger ), m_strFuncName( szFuncName ) { }
};
typedef std::list<ScriptFunction> SCRIPTFUNC_LIST;
