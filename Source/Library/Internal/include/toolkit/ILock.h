// ILock.h
//
// 기본 Lock 인터페이스와 WIN32 의 Critical Section 구현을 제공.
//
// by Testors

#pragma once

#ifndef	_WIN32_WINNT
#define	_WIN32_WINNT	0x0501
#endif

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <cassert>
#include <string>

#include "../dump/XException.h"


inline void spinlock_init( volatile LONG * lock )
{
	*lock = 0;
}

inline void spinlock_enter( volatile LONG * lock ) 
{
enter:
	if( InterlockedExchange( lock, 1 ) == 0 ) return;

	goto enter;
} 

inline void spinlock_leave( volatile LONG * lock ) 
{ 
	InterlockedExchange( lock, 0 );
} 

inline bool spinlock_tryenter( volatile LONG * lock ) 
{ 
	if( InterlockedExchange( lock, 1 ) == 0 ) return true;

	return false;
}


struct ILock
{
	ILock( const char* name = "NoName" ) : m_strName( name ), m_pszFileName( "" ), m_nLineNumber(0)
	{
	}
	virtual ~ILock()		{};

	void Lock( const char* szFileName, int nLineNumber )
	{
		Lock();

		m_pszFileName = szFileName;
		m_nLineNumber = nLineNumber;
	}

	bool TryLock( const char* szFileName, int nLineNumber )
	{
		bool bRtn = TryLock();

		if( bRtn )
		{
			m_pszFileName = szFileName;
			m_nLineNumber = nLineNumber;
		}

		return bRtn;
	}

	virtual void Lock()				= 0;
	virtual void UnLock()			= 0;
	virtual bool IsLocked() const	= 0;
	virtual bool TryLock()			{ return false; }


	std::string m_strName;

	const char*	m_pszFileName;
	int			m_nLineNumber;
};

struct XDummyLock : public ILock
{
	XDummyLock( const char* name = "" ) : ILock( name ), m_bIsLocked( false ) {}

	virtual ~XDummyLock()	{};

	virtual void Lock()				{ m_bIsLocked = true;	}
	virtual void UnLock()			{ m_bIsLocked = false;	}
	virtual bool IsLocked() const	{ return m_bIsLocked;	}
	virtual bool TryLock()			{ return true;			}

	bool m_bIsLocked;
};



class XMutex : public ILock
{
public:
	XMutex( const char * szName = NULL, bool make_not_initialize_mutex = false ) : ILock( szName ? szName : "" )
	{
		m_bIsLocked = false;
		m_hMutex = 0;

		if ( !make_not_initialize_mutex )
		{
			m_hMutex = CreateMutexA( NULL, false, static_cast< LPCSTR>( szName ) );

			if ( !m_hMutex )
				m_hMutex = OpenMutexA( NULL, false, static_cast< LPCSTR>( szName ) );
		}
	};

	virtual ~XMutex()
	{
		Close();
	};

	bool Open( const char * szName )
	{
		if ( m_hMutex )
			return false;

		m_hMutex = CreateMutexA( NULL, false, static_cast< LPCSTR>( szName ) );

		if ( !m_hMutex )
			m_hMutex = OpenMutexA( NULL, false, static_cast< LPCSTR>( szName ) );

		return !!m_hMutex;
	}
	
	bool IsOpen()
	{
		return !!m_hMutex;
	}

	void Close()
	{
		if ( m_hMutex )
		{
			if ( IsLocked() )
				UnLock();

			CloseHandle( m_hMutex );

			m_hMutex = 0;
		}
	}

	virtual void Lock()
	{
		if ( !m_hMutex )
			return;
		 
		while ( true )
		{
			DWORD dwWaitResult = WaitForSingleObject( m_hMutex, INFINITE );

			if ( dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_ABANDONED_0 ) break;
		}

		m_bIsLocked = true;		
	}

	virtual void UnLock()
	{
		if ( !m_hMutex )
			return;

		m_bIsLocked = false;
		ReleaseMutex( m_hMutex );
	}

	virtual bool IsLocked() const
	{
		return m_bIsLocked;
	}

	virtual bool TryLock()
	{
		if ( !m_hMutex )
			return false;

		DWORD dwWaitResult = WaitForSingleObject( m_hMutex, 1 );

		if ( dwWaitResult == WAIT_OBJECT_0 || dwWaitResult == WAIT_ABANDONED_0 )
		{
			m_bIsLocked = true;
			return true;
		}

		return false;
	}

private:
	bool	m_bIsLocked;
	HANDLE	m_hMutex;
};

class XCriticalSection : public ILock
{
public:

	XCriticalSection( const char* name = "" ) : ILock( name )
	{
		InitializeCriticalSection( &m_cs );
	}

	XCriticalSection( DWORD dwSpinCount )
	{
		InitializeCriticalSectionAndSpinCount( &m_cs, dwSpinCount );
	}

	virtual ~XCriticalSection()
	{
		DeleteCriticalSection( &m_cs );
	}

	virtual void Lock()
	{
		EnterCriticalSection( &m_cs );
	}

	virtual bool TryLock()
	{
		if ( TryEnterCriticalSection( &m_cs ) )
		{
			return true;
		}
		return false;
	}

	virtual void UnLock()
	{
		assert( m_cs.RecursionCount > 0 );
		LeaveCriticalSection( &m_cs );
	}

	virtual bool IsLocked() const
	{
		// Refer to http://msdn.microsoft.com/en-us/magazine/cc164040.aspx
		return m_cs.RecursionCount != 0;
	}

	bool IsLockedByCurrentThread() const
	{
		// Refer to http://msdn.microsoft.com/en-us/magazine/cc164040.aspx
		return m_cs.RecursionCount != 0 && m_cs.OwningThread == ULongToHandle( GetCurrentThreadId() );
	}

private:

	CRITICAL_SECTION	m_cs;
};

// 나중에 인라인 어셈으로 바꾸자.
// 그런데 가상함수 쓰는데 어셈으로 바꾼다고 얼마나 빨라질까나..? ㅡ.ㅡ

class XSpinLock : public ILock
{
public:

	XSpinLock( const char* name = "" ) : ILock( name )
	{
		m_bIsLocked = false;
		spinlock_init( &m_nLockCount );
	}

	virtual ~XSpinLock()
	{
	}

	virtual void Lock()
	{
		spinlock_enter( &m_nLockCount );
		m_bIsLocked = true;
	}

	virtual void UnLock()
	{
		spinlock_leave( &m_nLockCount );
		m_bIsLocked = false;
	}

	virtual bool TryLock()
	{
		if ( spinlock_tryenter( &m_nLockCount ) ) 
		{
			m_bIsLocked = true;
			return true;
		}
		
		return false;
	}

	virtual bool IsLocked() const
	{
		return m_bIsLocked;
	}

private:

	volatile LONG	m_nLockCount;
	volatile bool	 m_bIsLocked;
};

class XScopedLock_CS
{
public:

	XScopedLock_CS( CRITICAL_SECTION	*pCS )
	{
		m_pCS = pCS;
		EnterCriticalSection( m_pCS );
	}

	~XScopedLock_CS()
	{
		LeaveCriticalSection( m_pCS );
	}

private:

	CRITICAL_SECTION	*m_pCS;
};

#define THREAD_SYNCRONIZE_CS( SyncObject ) XScopedLock_CS __scoped_lock_cs( SyncObject )


class XScopedLock
{
public:

	XScopedLock( ILock * lock, const char* szFileName, int nLineNumber )
	{
		lock->Lock( szFileName, nLineNumber );
		m_lock = lock;
	}

	XScopedLock( ILock * lock )
	{
		lock->Lock();
		m_lock = lock;
	}

	XScopedLock( ILock & lock, const char* szFileName, int nLineNumber )
	{
		lock.Lock( szFileName, nLineNumber );
		m_lock = &lock;
	}

	XScopedLock( ILock & lock )
	{
		lock.Lock();
		m_lock = &lock;
	}

	~XScopedLock()
	{
		m_lock->UnLock();
	}

private:

	ILock * m_lock;
};

class XScopedSpinLock
{
public:

	XScopedSpinLock( volatile LONG * lock )
	{
		spinlock_enter( lock );
		m_lock = lock;
	}

	XScopedSpinLock( volatile LONG & lock )
	{
		spinlock_enter( &lock );
		m_lock = &lock;
	}

	~XScopedSpinLock()
	{
		spinlock_leave( m_lock );
	}

private:

	volatile LONG * m_lock;
};

#define LOCK( SyncObject ) SyncObject.Lock( __FILE__" : "__FUNCTION__, __LINE__ );
#define UNLOCK( SyncObject ) SyncObject.UnLock();

#define THREAD_SYNCRONIZE( SyncObject ) XScopedLock __scoped_lock( SyncObject, __FILE__" : "__FUNCTION__, __LINE__ );
#define THREAD_SYNCRONIZE1( SyncObject ) XScopedLock __scoped_lock_1( SyncObject, __FILE__" : "__FUNCTION__, __LINE__ );
#define THREAD_SYNCRONIZE2( SyncObject ) XScopedLock __scoped_lock_2( SyncObject, __FILE__" : "__FUNCTION__, __LINE__ );
#define SPINLOCK_SYNCRONIZE( SyncObject ) XScopedSpinLock __scoped_spinlock( SyncObject );

#define THREAD_SYNCHRONIZE( SyncObject ) XScopedLock __scoped_lock( SyncObject, __FILE__" : "__FUNCTION__, __LINE__ );
#define THREAD_SYNCHRONIZE1( SyncObject ) XScopedLock __scoped_lock_1( SyncObject, __FILE__" : "__FUNCTION__, __LINE__ );
#define THREAD_SYNCHRONIZE2( SyncObject ) XScopedLock __scoped_lock_2( SyncObject, __FILE__" : "__FUNCTION__, __LINE__ );
#define SPINLOCK_SYNCHRONIZE( SyncObject ) XScopedSpinLock __scoped_spinlock( SyncObject );

