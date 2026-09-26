#pragma once

#include "IConnection.h"

#define WM_SOCK_EVENT (WM_USER + 1001)


class XAsyncStreamConnection : public IStreamSocketConnection
{
public:
	XAsyncStreamConnection( HWND hWnd, bool bUseCipher = false ) 
		: m_hWndEvent( hWnd )
		, m_bUseCipher( bUseCipher )
		, m_pSendCipher( NULL )
		, m_pRecvCipher( NULL )
	{
	}

	XAsyncStreamConnection( XSocket sock, HWND hWnd, bool bUseCipher = false ) 
		: IStreamSocketConnection( sock ), m_hWndEvent( hWnd ), m_bUseCipher( bUseCipher )
		, m_pSendCipher( NULL )
		, m_pRecvCipher( NULL )
	{
	}

	virtual ~XAsyncStreamConnection(void);

	int		Read( void * szBuf, unsigned nLen );
	int		Write( const void * szBuf, unsigned nLen );
	int		Peek( void * szBuf, unsigned nLen );

	bool	Connect( const XAddr & addr );
	bool	Close();

	int		OnConnect( WORD wError );
	int		OnClose( WORD wError );
	int		OnReceive( WORD wError );
	int		OnWrite( WORD wError );

private:
	void	init( bool bUseCipher );

	HWND	m_hWndEvent;
	struct ICipher *m_pSendCipher;
	struct ICipher *m_pRecvCipher;
	bool	m_bUseCipher;
};
