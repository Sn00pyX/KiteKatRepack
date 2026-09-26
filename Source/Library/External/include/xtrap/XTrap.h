
/******************************************************************************
// XTrap.h Version 0xD5000001
// By WiseLogic
//****************************************************************************/

#ifdef __XTRAPMODULE_H

#define		CREATEKF_OUT_SIZE	544
#define		CREATEKF_IN_SIZE	264

extern HANDLE	g_XTrapMsgEvent;

///////////////////////////////////////////////////////////////////////////////
// XTrap Interface External Variable
///////////////////////////////////////////////////////////////////////////////

extern BOOL g_bApiMal;			///< XTRAP_API_MDL		 : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bMemoryMdl;		///< XTRAP_MEMORY_MDL    : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bAutoMousMdl;		///< XTRAP_AUTOMOUS_MDL  : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bAutoKeybMdl;		///< XTRAP_AUTOKEYB_MDL  : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bMalMdl;			///< XTRAP_MAL_MDL       : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bSpeedMdl;		///< XTRAP_SPEED_MDL     : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bFileMdl;			///< XTRAP_FILE_MDL      : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bApiHookMdl;		///< XTRAP_APIHOOK_MDL   : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bDebugModMdl;		///< XTRAP_DEBUGMOD_MDL  : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bOsMdl;			///< XTRAP_OS_MDL        : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bPatchMdl;		///< XTRAP_PATCH_MDL	   : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE

extern BOOL g_bMemoryCrack;		///< XTRAP_MEMORY_CRACK  : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bFileCrack;		///< XTRAP_FILE_CRACK    : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bApiHookCrack;	///< XTRAP_APIHOOK_CRACK : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE
extern BOOL g_bApiCrack;		///< XTRAP_API_CRACK	   : ACTIVE_DETECT_HACK : ACTIVE_FLAG_NONE

/** XTrap cannot find the file/path specified
	XTrap program reguires a newer version of Games */
extern BOOL g_bStartXTrap;

extern char	g_sXTrapCode[MAX_PATH];		///< XTrap ErrorCode XX-XX-XXXX

///////////////////////////////////////////////////////////////////////////////
// XTrap Interface External Function
///////////////////////////////////////////////////////////////////////////////

/*required*/void SetXTrapStartInfo		(CHAR *pGameName, 
										 DWORD ApiVersion, DWORD VendorCode, DWORD KeyboardType, DWORD PatchType, DWORD ModuleType);
/*required*/void XTrapStart				();
/*required*/void XTrapStop				();
/*required*/void XTrapKeepAlive			();

/*optional*/void SetXTrapMgrInfo		(IN CHAR *pMgrIp);
/*optional*/void SetXTrapPatchHttpUrl	(IN CHAR *pHttpUrl);
/*optional*/void SetXTrapPatchFtpUrl	(IN CHAR *pFtpUrl);
/*optional*/void SetOptGameInfo			(IN CHAR *pUserName,  IN CHAR *pGameServerName, IN CHAR *pCharacterName, 
									     IN CHAR *pClassName, IN long UserLevel);

/*optional*/DWORD CreateKFEx			(IN  PUCHAR	pVerHash,	/* XTrapCC Version Verify Hash Value					*/
										 IN  PUCHAR	pInBuf,		/* Encyption[AddressTable(256byte) + RandomKey(8byte)]	*/
										 IN	 UCHAR	CMF_Cnt,	/* ServerSide CMF Count									*/
										 OUT PUCHAR	pOutBuf);	/* Generate Key											*/
/*Optional*/void SendLogToMgr();

void XTrapStartFunc();
void XTrapStopFunc();

#endif