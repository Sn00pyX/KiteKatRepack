#include "../../include/sound/XMusicPlayer.h"

#define _WIN32_DCOM

#include <windows.h>
#include <process.h>
#include <memory>
#include <assert.h>

struct _XMusicPlayerScopedLock
{
	_XMusicPlayerScopedLock( void * lock )
	{
		m_pLock = (LPCRITICAL_SECTION)lock;
		EnterCriticalSection( m_pLock );
	}

	~_XMusicPlayerScopedLock()
	{
		LeaveCriticalSection( m_pLock );
	}

	LPCRITICAL_SECTION m_pLock;

};

#define THREAD_SYNC( a )  _XMusicPlayerScopedLock __music_lock( a )


inline void SAFE_RELEASE( IUnknown* p) { if(p) { (p)->Release(); (p)=NULL; } }

struct _XMusicPlayerImpl
{
	_XMusicPlayerImpl( audiere::AudioDevicePtr _d )
	{
		device = _d;
		fVolume			= 0.0f;
		pFadeArg       = NULL;

//		_oprint( "PLAY !! 0x%08X\n", this );
	}

	virtual ~_XMusicPlayerImpl()
	{	
//		_oprint( "DELETE !! 0x%08X\n", this );
	}

	float GetVolume()
	{
		return fVolume;
	}
	
	void SetVolume( float _fVolume )
	{
		fVolume = _fVolume;
		if( stream ) stream->setVolume( fVolume );
	}

	bool Init()
	{
		return device != NULL;
	}

	bool Load( const char *szFileName )
	{
		if( !device ) return false;

		stream = audiere::OpenSound( device, szFileName, true, audiere::FF_OGG);
		if( !stream ) return false;

		stream->setVolume( fVolume );

		return stream != NULL;   
	}

	bool Play( bool bLoop = true )
	{
		if( !stream ) return false;
		stream->setRepeat( bLoop );
		stream->play();
		return true;
	}

	bool Stop()
	{
		if( !stream ) return false;
		stream->stop();
		stream->reset();
		return true;
	}

	bool Pause()
	{
		if( !stream ) return false;
		stream->stop();
		return true;
	}

	
	audiere::AudioDevicePtr		device;
	audiere::OutputStreamPtr	stream;

	float            fVolume;

	struct _FadeArgument * pFadeArg;
};

XMusicPlayer::XMusicPlayer()
{
	bThreadStopSignal = false;
	nThreadCount	  = 0;
	cVolume			  = 100;
	m_pLock			  = new CRITICAL_SECTION;
	InitializeCriticalSection( (LPCRITICAL_SECTION) m_pLock );
}

XMusicPlayer::~XMusicPlayer()
{	
	bThreadStopSignal = true;

	while( nThreadCount ) Sleep( 100 );

	DeInit();

	DeleteCriticalSection( (LPCRITICAL_SECTION) m_pLock );
	delete m_pLock;
}

bool XMusicPlayer::Init()
{
   m_Device = audiere::OpenDevice("autodetect", "");
   if( m_Device == NULL ) return false;

	if( !m_vList.empty() ) return false;

	m_vList.push_back( new _XMusicPlayerImpl(m_Device) );

	return m_vList.back()->Init();
}

bool XMusicPlayer::DeInit()
{
	if( m_Device == NULL ) return false;

	THREAD_SYNC( m_pLock );
	for( size_t i = 0; i < m_vList.size(); i++ )
	{
		delete m_vList[i];
	}
	m_vList.clear();

	return 0;
}

bool XMusicPlayer::Load( const char *szFileName )
{	
	if( m_Device == NULL ) return false;

	THREAD_SYNC( m_pLock );
	return m_vList.back()->Load( szFileName );
}

bool XMusicPlayer::Pause()
{
	if( m_Device == NULL ) return false;

	THREAD_SYNC( m_pLock );
	return m_vList.back()->Pause();
}

bool XMusicPlayer::Play( bool bLoop )
{
	if( m_Device == NULL ) return false;

	THREAD_SYNC( m_pLock );
	return m_vList.back()->Play( bLoop );
}

bool XMusicPlayer::Stop()
{
	if( m_Device == NULL ) return false;

	THREAD_SYNC( m_pLock );
	return m_vList.back()->Stop();
}

void XMusicPlayer::SetVolume( char cPercentage )
{

	if( m_Device == NULL ) return;
	THREAD_SYNC( m_pLock );
	cVolume = cPercentage;
	//-5000 정도 되면 거의 안 들림. 약간 개선 해야 할 듯
	//-10000 ~ 0 (min, max)
	m_vList.back()->SetVolume( (cPercentage/100.0f) );
}

void XMusicPlayer::SetLoop( bool bLoop )
{
	if( m_Device == NULL ) return;

	THREAD_SYNC( m_pLock );
	if( !m_vList.empty() ) 
	{
		if( m_vList.back()->stream )
			m_vList.back()->stream->setRepeat( bLoop );
	}
}


struct _FadeArgument
{
	_FadeArgument(	_XMusicPlayerImpl *_pPlayer,
					volatile bool * _bStopSignal,
					volatile int *_nThreadCount,
					void* _pLock,
					std::vector<struct _XMusicPlayerImpl*> * _pList )
	{
		pPlayer = _pPlayer;
		pbGlobalStopSignal = _bStopSignal;
		nThreadCount = _nThreadCount;
		pLock = _pLock;
		pList = _pList;

		nCurrent = 0;
		fAdd = 0;
		bFadeOut = false;
		bStopSignal = false;
		nCount = 0;
		nTime = 0;

//		_oprint( "START THREAD !! 0x%08X\n", this );
	}

	virtual ~_FadeArgument()
	{
		THREAD_SYNC( pLock );
		InterlockedDecrement( (volatile LONG *)nThreadCount );

		pPlayer->pFadeArg = NULL;

		if( bFadeOut )
		{	
			for( size_t i = 0; i < pList->size(); i++ )
			{
				if( (*pList)[i] != pPlayer ) continue;
				pList->erase( pList->begin() + i );
				break;
			}
			delete pPlayer;
		}

//		_oprint( "END THREAD !! 0x%08X\n", this );
	}

	std::vector<struct _XMusicPlayerImpl*> * pList;
	void*				pLock;
	_XMusicPlayerImpl*	pPlayer;
	volatile bool*		pbGlobalStopSignal;
	volatile int*		nThreadCount;

	volatile bool		bStopSignal;
	volatile float		fAdd;
	volatile int		nCount;
	volatile int		nTime;
	volatile int		nCurrent;
	volatile bool		bFadeOut;
};

unsigned __stdcall fadeEffect( void *pPtr )
{
	_FadeArgument *pArg = reinterpret_cast< _FadeArgument * >( pPtr );

	for( pArg->nCurrent = 0; pArg->nCurrent < pArg->nCount; pArg->nCurrent++ )
	{
		if( (*pArg->pbGlobalStopSignal) || pArg->bStopSignal ) break;

		float fNewVolume = pArg->pPlayer->GetVolume() + pArg->fAdd;

		//_oprint( "%08X : %f : %f\n", pArg->pPlayer, pArg->pPlayer->GetVolume(), fNewVolume );

		if( fNewVolume < 0 ) fNewVolume = 0.0f;

		pArg->pPlayer->SetVolume( fNewVolume );

		Sleep( pArg->nTime );
	}

	delete pArg;

	return 0;
}

static void initFadeOut( _FadeArgument* p, float cur_vol, unsigned ms, int count )
{
	p->bFadeOut = true;

	p->nTime = ms;
	p->nCount = count;
	p->fAdd = 0 - cur_vol / count;
}

static void initFadeIn( _FadeArgument* p, float cur_vol, unsigned ms, int count )
{
	p->bFadeOut = false;

	p->nTime = ms;
	p->nCount = count;
	p->fAdd = cur_vol / count;
}

bool XMusicPlayer::IsPlaying()
{
	THREAD_SYNC( m_pLock );
	if( m_vList.empty() || !m_vList.back()->stream ) return false;
	return m_vList.back()->stream->isPlaying();
}

bool XMusicPlayer::Swap( const char *szFileName, unsigned ms, unsigned count )
{
	THREAD_SYNC( m_pLock );

	if( m_vList.back()->pFadeArg && !m_vList.back()->pFadeArg->bFadeOut )
	{
		m_vList.back()->pFadeArg->bStopSignal = true;
	}

	_XMusicPlayerImpl* pNewMusic = new _XMusicPlayerImpl( m_Device );;
	if( !pNewMusic->Init() ) assert( !"11" );
	pNewMusic->SetVolume( 0 );
	if( !pNewMusic->Load( szFileName ) )
	{
		delete pNewMusic;

		assert( !"22" );
		return false;
	}

	if( !pNewMusic->Play() ) assert( !"33" );
	
	_FadeArgument *pFadeOut = new _FadeArgument( m_vList.back(), &bThreadStopSignal, &nThreadCount, m_pLock, &m_vList );
	_FadeArgument *pFadeIn  = new _FadeArgument( pNewMusic, &bThreadStopSignal, &nThreadCount, m_pLock, &m_vList );

	m_vList.back()->pFadeArg = pFadeOut;
	pNewMusic->pFadeArg = pFadeIn;
	
	InterlockedIncrement( (volatile LONG *) &nThreadCount );
	InterlockedIncrement( (volatile LONG *) &nThreadCount );

	initFadeOut( pFadeOut, GetVolume() / 100.0f, ms, count );
	initFadeIn( pFadeIn, GetVolume() / 100.0f, ms, count );

	unsigned id;
	_beginthreadex( NULL, 0, fadeEffect, pFadeOut, 0, &id );
	_beginthreadex( NULL, 0, fadeEffect, pFadeIn , 0, &id );

	m_vList.push_back( pNewMusic );
	
	return true;
}