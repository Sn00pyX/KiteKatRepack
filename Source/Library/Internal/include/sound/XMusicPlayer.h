#pragma once

#include <vector>
#include <audiere/audiere.h>

#pragma comment(lib, "audiere.lib")

struct XMusicPlayer
{
	XMusicPlayer();
	virtual ~XMusicPlayer();

	bool		Init();
	bool		DeInit();

	bool		Load( const char *szFileName );

	void		SetLoop( bool bLoop = false );
	bool		Play( bool bLoop = false );
	bool		Swap( const char *szFileName, unsigned ms = 100, unsigned count = 50 );
	bool		Stop();
	bool		Pause();
	bool		IsPlaying();

	void		SetVolume( char cPercentage );
	char		GetVolume()	{ return cVolume; }
	
	bool        onGraphEvent();

private:

	XMusicPlayer( const XMusicPlayer& );
	XMusicPlayer& operator=( const XMusicPlayer& );

	char		  cVolume;
	volatile bool bThreadStopSignal;
	volatile int  nThreadCount;

	audiere::AudioDevicePtr m_Device;
	void * m_pLock;
	
	std::vector<struct _XMusicPlayerImpl*> m_vList;
};