/*++ HackShield SDK header file   

Copyright (c) 2003-2005  AhnLab, Inc.

Module Name:

    HShield.h

Abstract:

    This header file defines the data types and constants, and exposes APIs 
	that are defined by HackShield Library.

Tag Information:
	
	This header file is auto-generated at $$HSHIELD_BUILD_DATETIME$$.

--*/
           
#ifndef _HSHIELD_H_INC
#define _HSHIELD_H_INC

// HShield Library Version Info
#define STATE_DEV		0x00
#define STATE_ALPHA		0x01
#define STATE_BETA		0x02
#define STATE_RC_RTM	0x03

/**
	M : Major 버전
	m : Minior 버전
	p : 패치 버전
	s1 : STATE_DEV, STATE_ALPHA, STATE_BETA, STATE_RC_RTM 정보
	s2 : 각 상태별 버전 정보. e.g.) Alpha1, Alpha2
	b : 빌드 번호 
*/
#define	HSVERSION(M,m,p,s1,s2,b) (M&0x0F)<<28|(m&0x0F)<<24|(p&0x1F)<<19|(s1&0x03)<<17|(s2&0x07)<<14|(b&0x3FFF)



// Initialize Option
#define AHNHS_CHKOPT_SPEEDHACK					0x2
#define AHNHS_CHKOPT_READWRITEPROCESSMEMORY 	0x4
#define AHNHS_CHKOPT_KDTRACER					0x8
#define AHNHS_CHKOPT_OPENPROCESS				0x10
#define AHNHS_CHKOPT_AUTOMOUSE					0x20
#define AHNHS_CHKOPT_MESSAGEHOOK                0x40
#define AHNHS_CHKOPT_PROCESSSCAN                0x80


#define AHNHS_CHKOPT_ALL ( AHNHS_CHKOPT_SPEEDHACK \
						| AHNHS_CHKOPT_READWRITEPROCESSMEMORY \
						| AHNHS_CHKOPT_KDTRACER \
						| AHNHS_CHKOPT_OPENPROCESS \
						| AHNHS_CHKOPT_AUTOMOUSE \
						| AHNHS_CHKOPT_MESSAGEHOOK \
						| AHNHS_CHKOPT_PROCESSSCAN \
						| AHNHS_CHKOPT_HSMODULE_CHANGE \
						| AHNHS_USE_LOG_FILE )

#define AHNHS_USE_LOG_FILE                      0x100
#define AHNHS_ALLOW_SVCHOST_OPENPROCESS         0x400
#define AHNHS_ALLOW_LSASS_OPENPROCESS           0x800
#define AHNHS_ALLOW_CSRSS_OPENPROCESS           0x1000
#define AHNHS_DONOT_TERMINATE_PROCESS           0x2000
#define AHNHS_DISPLAY_HACKSHIELD_LOGO           0x4000
#define AHNHS_CHKOPT_HSMODULE_CHANGE			0x8000
#define AHNHS_CHKOPT_LOADLIBRARY				0x10000
// [BT: 25042][수정자: 박근영][수정일자: 2006.06.18] AHNHS_CHKOPT_PROTECTSCREEN 옵션 추가.
#define AHNHS_CHKOPT_PROTECTSCREEN				0x20000


// SpeedHack Sensing Ratio
#define	AHNHS_SPEEDHACK_SENSING_RATIO_HIGHEST	0x1
#define	AHNHS_SPEEDHACK_SENSING_RATIO_HIGH		0x2
#define	AHNHS_SPEEDHACK_SENSING_RATIO_NORMAL	0x4
#define	AHNHS_SPEEDHACK_SENSING_RATIO_LOW		0x8
#define	AHNHS_SPEEDHACK_SENSING_RATIO_LOWEST	0x10
#define	AHNHS_SPEEDHACK_SENSING_RATIO_GAME		0x20



//ERROR CODE
#define HS_ERR_OK                               0x00000000		///< 함수 호출 성공
#define HS_ERR_UNKNOWN                          0x00000001		///<  알 수 없는 오류가 발생했습니다.
#define HS_ERR_INVALID_PARAM                    0x00000002		///<  올바르지 않은 인자입니다.
#define HS_ERR_NOT_INITIALIZED                  0x00000003		///<  핵쉴드 모듈이 초기화되지 않은 상태입니다.
#define HS_ERR_COMPATIBILITY_MODE_RUNNING       0x00000004		///<  현재 프로세스가 호환성 모드로 실행되었습니다.
#define HS_ERR_ALREADY_UNINITIALIZED			0x00000005		///<  Uninitialize가 수행된 이후에는 게임을 재실행한후 Initialize해야 합니다.

#define HS_ERR_INVALID_LICENSE                  0x00000100		///<  올바르지 않은 라이센스 키입니다.
#define HS_ERR_INVALID_FILES                    0x00000101		///<  잘못된 파일 설치되었습니다. 프로그램을 재설치하시기 바랍니다.
#define HS_ERR_INIT_DRV_FAILED                  0x00000102
#define HS_ERR_ANOTHER_SERVICE_RUNNING          0x00000103		///<  다른 게임이나 프로세스에서 핵쉴드를 이미 사용하고 있습니다.
#define HS_ERR_ALREADY_INITIALIZED              0x00000104		///<  이미 핵쉴드 모듈이 초기화되어 있습니다.
#define HS_ERR_DEBUGGER_DETECT                  0x00000105		///<  컴퓨터에서 디버거 실행이 감지되었습니다. 디버거의 실행을 중지시킨 뒤에 다시 실행시켜주시기바랍니다.
#define HS_ERR_EXECUTABLE_FILE_CRACKED			0x00000106		///<  실행 파일의 코드가 크랙 되었습니다.
#define HS_ERR_NEED_ADMIN_RIGHTS				0x00000107		///<  ADMIN 권한이 필요합니다.

#define HS_ERR_START_ENGINE_FAILED              0x00000200		///<  해킹 프로그램 감지 엔진을 시작할 수 없습니다.
#define HS_ERR_ALREADY_SERVICE_RUNNING          0x00000201		///<  이미 핵쉴드 서비스가 실행 중입니다.
#define HS_ERR_DRV_FILE_CREATE_FAILED           0x00000202		///<  핵쉴드 드라이버 파일을 생성할 수 없습니다.
#define HS_ERR_REG_DRV_FILE_FAILED              0x00000203		///<  핵쉴드 드라이버를 등록할 수 없습니다.
#define HS_ERR_START_DRV_FAILED                 0x00000204		///<  핵쉴드 드라이버를 시작할 수 없습니다.

#define HS_ERR_SERVICE_NOT_RUNNING              0x00000301		///<  핵쉴드 서비스가 실행되고 있지 않은 상태입니다.
#define HS_ERR_SERVICE_STILL_RUNNING            0x00000302		///<  핵쉴드 서비스가 아직 실행중인 상태입니다.

#define HS_ERR_NEED_UPDATE                      0x00000401		///<  핵쉴드 모듈의 업데이트가 필요합니다.

#define HS_ERR_API_IS_HOOKED					0x00000501		///<  해당 API가 후킹되어 있는 상태입니다.

#define HS_ERR_GET_SDKVERSION_FAILED			0x00000601		///<  버전값을 가져오는데 실패
// Server-Client 연동 버전  ERROR CODE
#define HS_ERR_ANTICPCNT_MAKEACKMSG_INVALIDPARAM				0x00010000
#define HS_ERR_ANTICPCNT_MAKEACKMSG_MAKESESSIONKEY_FAIL			0x00010001
#define HS_ERR_ANTICPCNT_MAKEACKMSG_INITCRYPT_FAIL				0x00010002
#define HS_ERR_ANTICPCNT_MAKEACKMSG_DECRYPTMESSAGE_FAIL			0x00010003
#define HS_ERR_ANTICPCNT_MAKEACKMSG_GETMEMHASH_FAIL				0x00010004
#define HS_ERR_ANTICPCNT_MAKEACKMSG_GETMODNAME_FAIL				0x00010005
#define HS_ERR_ANTICPCNT_MAKEACKMSG_GETFILEHASH_FAIL			0x00010006
#define HS_ERR_ANTICPCNT_MAKEACKMSG_GETHSHIELDHASH_FAIL			0x00010007
#define HS_ERR_ANTICPCNT_MAKEACKMSG_MAKEHSHIELDFLAG_FAIL		0x00010008
#define HS_ERR_ANTICPCNT_MAKEACKMSG_MAKEACKSEQUENCENUM_FAIL		0x00010009
#define HS_ERR_ANTICPCNT_MAKEACKMSG_ENCRYPTMESSAGE_FAIL			0x0001000A

#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_INVALIDPARAM			0x00010010
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_MAKESESSIONKEY_FAIL		0x00010011
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_INITCRYPT_FAIL			0x00010012
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_DECRYPTMESSAGE_FAIL		0x00010013
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_GETGUIDFROMFILE_FAIL	0x00010014
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_MAKEACKSEQUENCENUM_FAIL	0x00010015
#define HS_ERR_ANTICPCNT_MAKEGUIDACKMSG_ENCRYPTMESSAGE_FAIL		0x00010016	

#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_INVALIDPARAM			0x00010020
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_OPENFILEMAPPING_FAIL	0x00010021
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_MAPVIEWOFFILE_FAIL		0x00010022
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_INITCRYPT_FAIL			0x00010023
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_DECRYPTMESSAGE_FAIL	0x00010024
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_INVALIDMMF				0x00010025
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_OPENEVENT_FAIL			0x00010026
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_GETMODFILENAME_FAIL	0x00010027
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_GETINSTRUCTION_FAIL	0x00010028
#define HS_ERR_ANTICPCNT_SAVEFUNCADDRESS_ENCRYPTMESSAGE_FAIL	0x00010029

#define HS_ERR_ANTICPCNT_DEBUGGER_DETECTED						0x00010030

//////////////////////////////////////////////////////////////////////////
// 에러 코드 정의
//////////////////////////////////////////////////////////////////////////

#define HS_ERR_ANTICPXCNT_BASECODE								0xE4010000

/// 파라미터 값이 올바르지 않습니다
#define HS_ERR_ANTICPXCNT_INVALID_PARAMETER						(HS_ERR_ANTICPXCNT_BASECODE + 0x1)
/// 잘못된 메모리 주소를 접근하였습니다
#define HS_ERR_ANTICPXCNT_INVALID_ADDRESS						(HS_ERR_ANTICPXCNT_BASECODE + 0x2)
/// 메모리가 부족합니다
#define HS_ERR_ANTICPXCNT_NOT_ENOUGH_MEMORY						(HS_ERR_ANTICPXCNT_BASECODE + 0x3)
/// CRC Table을 초기화 하는데 실패하였습니다
#define HS_ERR_ANTICPXCNT_CRC_TABLE_INIT_FAILED					(HS_ERR_ANTICPXCNT_BASECODE + 0x4)
/// 메시지 길이가 올바르지 않습니다
#define HS_ERR_ANTICPXCNT_BAD_LENGTH							(HS_ERR_ANTICPXCNT_BASECODE + 0x5)
/// 버퍼의 크기가 작습니다
#define HS_ERR_ANTICPXCNT_INSUFFICIENT_BUFFER					(HS_ERR_ANTICPXCNT_BASECODE + 0x6)
/// 현재 버전에서 지원하지 않는 기능이다
#define HS_ERR_ANTICPXCNT_NOT_SUPPORTED							(HS_ERR_ANTICPXCNT_BASECODE + 0x7)
/// 파일을 찾을 수 없습니다
#define HS_ERR_ANTICPXCNT_FILE_NOT_FOUND						(HS_ERR_ANTICPXCNT_BASECODE + 0x8)
/// 입력받은 메시지의 크기가 올바르지 않습니다
#define HS_ERR_ANTICPXCNT_INVALID_MESSAGE_SIZE					(HS_ERR_ANTICPXCNT_BASECODE + 0x9)
/// 올바른 포멧이 아닙니다
#define HS_ERR_ANTICPXCNT_BAD_FORMAT							(HS_ERR_ANTICPXCNT_BASECODE + 0xA)
/// 디버그 상황을 감지하였습니다
#define HS_ERR_ANTICPXCNT_DEBUGGER_DETECTED						(HS_ERR_ANTICPXCNT_BASECODE + 0xB)
/// 핵쉴드 모듈 경로가 잘 못되었거나 핵쉴드 모듈이 올바르지 않습니다
#define HS_ERR_ANTICPXCNT_BAD_HSHIELD_MODULE					(HS_ERR_ANTICPXCNT_BASECODE + 0xC)
/// 클라이언트 모듈이 올바르지 않습니다
#define HS_ERR_ANTICPXCNT_BAD_CLIENT_FILE						(HS_ERR_ANTICPXCNT_BASECODE + 0xD)
/// 서버로 부터 전달 받은 요청 메시지가 올바르지 않습니다
#define HS_ERR_ANTICPXCNT_BAD_REQUEST							(HS_ERR_ANTICPXCNT_BASECODE + 0xE)
/// 핵쉴드 코어 엔진이 정상적으로 동작하지 않습니다 (디바이스 드라이버)
#define HS_ERR_ANTICPXCNT_HSHIELD_CORE_ENGINE_NOT_WORKING		(HS_ERR_ANTICPXCNT_BASECODE + 0xF)
/// 해킹 시도에 의해 시스템이 오동작 할 경우 발생
#define HS_ERR_ANTICPXCNT_UNKNOWN								(HS_ERR_ANTICPXCNT_BASECODE + 0xFF)


// CallBack Code
#define AHNHS_ACTAPC_DETECT_ALREADYHOOKED		0x010101			///< 일부 API가 이미 후킹되어 있는 상태입니다. (그러나 실제로는 이를 차단하고 있기 때문에 후킹프로그램은 동작하지 않습니다.)
#define AHNHS_ACTAPC_DETECT_AUTOMOUSE			0x010102			///< 오토마우스 행위가 감지되었습니다.	
#define AHNHS_ACTAPC_DETECT_AUTOMACRO			0x010104			///< 오토메크로 행위가 감지되었습니다.	
#define AHNHS_ACTAPC_DETECT_HOOKFUNCTION		0x010301			///< 보호 API에 대한 후킹 행위가 감지되었습니다.
#define AHNHS_ACTAPC_DETECT_DRIVERFAILED		0x010302			///< 해킹 차단 드라이버가 로드되지 않았습니다.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK			0x010303			///< 스피드핵류의 프로그램에 의해 시스템 시간이 변경되었습니다.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK_APP		0x010304			///< 스피드핵류의 프로그램에 의해 시스템 시간이 변경되었습니다.
#define AHNHS_ACTAPC_DETECT_MESSAGEHOOK			0x010306			///< 메시지 후킹이 시도되었으며 이를 차단하지 못했습니다.
#define AHNHS_ACTAPC_DETECT_KDTRACE				0x010307			///< 디버거 트래이싱이 발생했다.(커널 디버거 활성화, 이후 브레이크 포인터 처리)
#define AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED		0x010308			///< 설치된 디버거 트래이싱이 변경되었다.
#define AHNHS_ACTAPC_DETECT_SPEEDHACK_RATIO		0x01030B			///< 스피드핵 감지 옵션이 'GAME'이 경우 이 콜백으로 최근 5초동안의 시간정보가 전달됩니다.
#define AHNHS_ENGINE_DETECT_GAME_HACK           0x010501			///< 게임 해킹툴의 실행이 발견되었습니다.
#define AHNHS_ENGINE_DETECT_GENERAL_HACK        0x010502			///< 일반 해킹툴(트로이목마 종류)이 발견되었습니다.
#define AHNHS_ENGINE_DETECT_WINDOWED_HACK		0x010503			///< 창모드 해킹툴
#define AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS	0x010601		///< 코드 패치 시도가 감지되었습니다.
#define AHNHS_ACTAPC_DETECT_MODULE_CHANGE       0x010701			///< 핵쉴드 관련모듈이 변경되었습니다.
#define AHNHS_ACTAPC_DETECT_ENGINEFAILED	    0x010702			///< 엔진기능이 정상적이지 않습니다.
#define AHNHS_ACTAPC_DETECT_CODEMISMATCH		0x010703			///< 코드가 일치하지 않습니다. 



#ifndef __ANTICPX_TRANS_BUFFER__
#define __ANTICPX_TRANS_BUFFER__

#define ANTICPX_TRANS_BUFFER_MAX			400

// [2008.1.3][남성일][BT35349]
// 기존 bytealignment정보를 스택에 저장.
#pragma pack(push) //기존 alignment 저장
#pragma pack(8)

typedef struct _AHNHS_TRANS_BUFFER
{
	unsigned short nLength;
	unsigned char byBuffer[ANTICPX_TRANS_BUFFER_MAX/* 송수신 패킷의 최대 크기 */];
} AHNHS_TRANS_BUFFER, *PAHNHS_TRANS_BUFFER;

#pragma pack(pop) // 기존 alignment 복구.

#endif // __ANTICPX_TRANS_BUFFER__


// CallBack시 전달되는 데이터
#pragma pack(push) //기존 alignment 저장
#pragma pack(1)

typedef struct tagACTAPCPARAM_DETECT_HOOKFUNCTION
{	
	char szFunctionName[128];		///< file path
	char szModuleName[128];
	
} ACTAPCPARAM_DETECT_HOOKFUNCTION, *PACTAPCPARAM_DETECT_HOOKFUNCTION;


/// 오토마우스 관련 APC Structure
typedef struct tagACTAPCPARAM_DETECT_AUTOMOUSE
{
	BYTE	byDetectType;			///< AutoMouse 관련 API 호출 1, AutoMouse 관련 API 변조 2
	DWORD	dwPID;					///< AutoMouse 프로세스 또는 API 가 변조된 프로세스
	CHAR	szProcessName[16+1];	///< 프로세스명
	CHAR	szAPIName[128];			///< 호출된 API 명 또는 변조된 API 명

} ACTAPCPARAM_DETECT_AUTOMOUSE, *PACTAPCPARAM_DETECT_AUTOMOUSE;


#define	EAGLE_AUTOMOUSE_APCTYPE_API_CALLED			1
#define	EAGLE_AUTOMOUSE_APCTYPE_API_ALTERATION		2
#define	EAGLE_AUTOMOUSE_APCTYPE_SHAREDMEMORY_ALTERATION	3


/// AHNHS_ENGINE_DETECT_GAME_HACK	Callback Structure
typedef struct tagAHNHS_ENGINE_DETECT_GAME_HACK_CALLBACK
{
	CHAR	szMessageA[128+1];		///< Detect game hack name
	DWORD	cbMessageLen;			///< name length
	DWORD	dwHackNo;

} AHNHS_ENGINE_DETECT_GAME_HACK_CALLBACK, *PAHNHS_ENGINE_DETECT_GAME_HACK_CALLBACK;


#pragma pack(pop)	// 기존 alignment 복구.


// Server-Client 연동 버전에 사용되는 버퍼의 크기
#define SIZEOF_REQMSG		160
#define SIZEOF_ACKMSG		72
#define SIZEOF_GUIDREQMSG	20
#define SIZEOF_GUIDACKMSG	340



/// Callback Function Prototype
typedef int (__stdcall* PFN_AhnHS_Callback)(
	long lCode,
	long lParamSize,
	void* pParam 
);


// Export API
#if defined(__cplusplus)
extern "C"
{
#endif

/**
 * 
 * @remarks	핵쉴드 초기화 함수
 *
 * @param	szFileName : [IN] Ehsvc.dll의 Full Path
 * @param	pfn_Callback : [OUT] Callback Function Pointer
 * @param	nGameCode : [IN] Game Code
 * @param	szLicenseKey : [IN] License Code
 * @param	unOption : [IN] Initialize Options
 * @param	unSHackSensingRatio : [IN] SpeedHack Sensing Ratio
 *
 */
int
__stdcall
_AhnHS_Initialize (
	const char* szFileName,
	PFN_AhnHS_Callback pfn_Callback,
	int nGameCode,
	const char* szLicenseKey,
	unsigned int unOption,
	unsigned int unSHackSensingRatio
);


/**
 * 
 * @remarks	핵쉴드 시작 함수
 *
 */
int
__stdcall
_AhnHS_StartService ();


/**
 * 
 * @remarks	핵쉴드 종료 함수
 *
 */
int
__stdcall
_AhnHS_StopService ();


/**
 * 
 * @remarks	핵쉴드 기능 중지 함수로 현재는 키보드 관련 메시지 훅관련 기능만 중지 시킨다.
 *
 * @param	unPauseOption : [IN] 중지 하려는 기능의 옵션 ( 현재는 AHNHS_CHKOPT_MESSAGEHOOK만 가능)
 *
 */
int
__stdcall
_AhnHS_PauseService (
	unsigned int unPauseOption
	);


/**
 * 
 * @remarks	핵쉴드 기능 재개 함수로 현재는 키보드 관련 메시지 훅관련 기능만 재개 시킨다.
 *
 * @param	unResumeOption : [IN] 재개 하려는 기능의 옵션 ( 현재는 AHNHS_CHKOPT_MESSAGEHOOK만 가능)
 *
 */
int
__stdcall
_AhnHS_ResumeService (
	unsigned int unResumeOption
	);


/**
 * 
 * @remarks	핵쉴드 완전 종료 함수
 *
 */
int
__stdcall
_AhnHS_Uninitialize ();


/**
 * @remarks	핵쉴드 SDK 버전을 읽는 함수 
 */
int 
__stdcall 
_AhnHS_GetSDKVersion( DWORD* dwSDKver );

/**
 * 
 * @remarks	서버로부터 CRC를 요청받을 경우 해당 메시지에 대한 응답메시지를 만든다.
 *
 * @param	pbyReqMsg : [IN] 암호화된 Request Message, 이 버퍼의 크기는 위에 정의된 SIZEOF_REQMSG이다.
 * @param	pbyAckMsg : [OUT] 암호화된 Ack Message, 이 버퍼의 크기는 위에 정의된 SIZEOF_ACKMSG이다.
 *
 */
int
__stdcall
_AhnHS_MakeAckMsg (
	unsigned char *pbyReqMsg,
	unsigned char *pbyAckMsg
	);


/**
 * 
 * @remarks	서버로부터 GUID를 요청받을 경우 해당 메시지에 대한 응답메시지를 만든다.
 *
 * @param	pbyGuidReqMsg : [IN] 암호화된 GUID Request Message, 이 버퍼의 크기는 위에 정의된 SIZEOF_GUIDREQMSG이다.
 * @param	pbyGuidAckMsg : [OUT] 암호화된 GUID Ack Message, 이 버퍼의 크기는 위에 정의된 SIZEOF_GUIDACKMSG이다.
 *
 */
int
__stdcall
_AhnHS_MakeGuidAckMsg (
	unsigned char *pbyGuidReqMsg,
	unsigned char *pbyGuidAckMsg
	);
							

/**
 * 
 * @remarks	함수 포인터 리스트를 받아들여 만약 게임사에 제공한 툴이 실행중이라면
 *			메모리에 대한 CRC 파일이 만들어 진다.
 *
 * @author	Heean Park
 *
 * @date	2005/11/21
 *
 * @param	unNumberOfFunc : [IN] 함수 포인터 개수
 * @param	... : [IN] 함수 포인터 리스트
 *
 */
int
__stdcall
_AhnHS_SaveFuncAddress (
	unsigned int unNumberOfFunc,
	...
	);


int 
__stdcall
_AhnHS_CheckAPIHooked (
	const char*	szModuleName,
	const char*	szFunctionName,
	const char*	szSpecificPath
	);


/**
 * 
 * @remarks	서버로부터 요청받을 경우 해당 메시지에 대한 응답메시지를 만든다.
 *
 * @param	pbyRequest : [IN] 요청 메시지 버퍼
 * @param	ulRequestLength : [IN] 요청 메시지 길이
 * @param	pResponseBuffer : [OUT] 응답 메시지 버퍼
 *
 */
int
__stdcall
_AhnHS_MakeResponse (
	unsigned char *pbyRequest,
	unsigned long ulRequestLength, 
	PAHNHS_TRANS_BUFFER pResponseBuffer
	);

/**
 * 
 * @remarks	DirectInput의 Buffered 방식에서의 Scan 코드 처리.
 *
 * @param	pbyRequest : [IN] 요청 메시지 버퍼
 *
 */
int
__stdcall
_AhnHS_PreTranslateBufferedDIMessage ( 
	IN USHORT usMakeCode 
	);

/**
 * 
 * @remarks	DirectInput의 Buffered 방식에서의 Scan 코드 처리.
 *
 * @param	dwCnt : [IN] 입력으로 처리하는 개수 ( key down, up을 따로 처리하는 경우는 1 사용)
 *
 */
int
__stdcall
_AhnHS_PreTranslateBufferedDICounter(DWORD dwCnt = 2);

/**
 * 
 * @remarks	DirectInput의 Immediate 방식에서의 Scan 코드 처리.
 *
 * @param	pbyRequest : [IN] 요청 메시지 버퍼
 * @param	ulRequestLength : [IN] 요청 메시지 길이
 * @param	pResponseBuffer : [OUT] 응답 메시지 버퍼
 *
 */
int
__stdcall
_AhnHS_PreTranslateImmediateDIMessage ( 
   IN PBYTE pBitmap, 
   IN UINT unBitmapSize 
   );

/**
 * 
 * @remarks	안전한 Direct3DCreate9 API를 위한 Wrapper Function
 *
 * @param	SDKVersion : [IN] Direct3DCreate9 API의 인자로 들어가는 SDK Version
 *
 */
PVOID
__stdcall
_AhnHS_Direct3DCreate9 (
	IN UINT SDKVersion
	);


/**
 * 
 * @remarks	안전한 sendto API를 위한 Wrapper Function (sendto API와 동일하게 사용)
 *
 * @param	s : [IN] 소켓
 * @param	buf : [IN] 전송될 버퍼
 * @param	len : [IN] 버퍼의 길이
 * @param	flags : [IN] 함수 역활을 나타내는 플레그
 * @param	to : [IN] 버퍼(데이터)가 전송될 원격 호스트의 어드레스
 * @param	tolen : to 변수의 크기
 *
 */
int
__stdcall
_AhnHS_sendto (			
	IN  int s,
	IN  char* buf,
	IN  int len,
	IN  int flags,
	IN  const struct sockaddr* to,
	IN  int tolen
	);

#if defined(__cplusplus)
}
#endif

#endif _HSHIELD_H_INC