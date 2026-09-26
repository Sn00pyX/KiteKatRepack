//
//	The Readers/Writers lock
//
//		by Testors (testors@welovebeer.org)
//
//		Jeffrey Richter 의 것으로 C&P 했음. *^^*
//
//					2003/09/23

#pragma once

#include "ILock.h"


template< typename TLOCK = XCriticalSection >
class XRWLock
{
public:
	// Constructor
	XRWLock()                  
	{
		// Initially no readers want access, no writers want access, and 
		// no threads are accessing the resource
		m_nWaitingReaders = m_nWaitingWriters = m_nActive = 0;
		m_hsemReaders = CreateSemaphore(NULL, 0, MAXLONG, NULL);
		m_hsemWriters = CreateSemaphore(NULL, 0, MAXLONG, NULL);
		InitializeCriticalSection(&m_cs);
	}

	// Destructor
	~XRWLock()
	{
		m_nWaitingReaders = m_nWaitingWriters = m_nActive = 0;
		DeleteCriticalSection(&m_cs);
		CloseHandle(m_hsemReaders);
		CloseHandle(m_hsemWriters);
	}

	// Call this to gain shared read access
	void WaitToRead()      
	{
		// Ensure exclusive access to the member variables
		EnterCriticalSection(&m_cs);

		// Are there writers waiting or is a writer writing?
		BOOL fResourceWritePending = (m_nWaitingWriters || (m_nActive < 0));

		if (fResourceWritePending) {

			// This reader must wait, increment the count of waiting readers
			m_nWaitingReaders++;
		} else {

			// This reader can read, increment the count of active readers
			m_nActive++;
		}

		// Allow other threads to attempt reading/writing
		LeaveCriticalSection(&m_cs);

		if (fResourceWritePending) {

			// This thread must wait
			WaitForSingleObject(m_hsemReaders, INFINITE);
		}

	}

	// Call this to gain exclusive write access
	void WaitToWrite()
	{
		// Ensure exclusive access to the member variables
		EnterCriticalSection(&m_cs);
	 
		// Are there any threads accessing the resource?
		BOOL fResourceOwned = (m_nActive != 0);
	 
		if (fResourceOwned) {
	 
			// This writer must wait, increment the count of waiting writers
			m_nWaitingWriters++;

		} else {
	 
			// This writer can write, decrement the count of active writers
			m_nActive = -1;
		}
	 
		// Allow other threads to attempt reading/writing
		LeaveCriticalSection(&m_cs);
	 
		if (fResourceOwned) {
	 
			// This thread must wait
			WaitForSingleObject(m_hsemWriters, INFINITE);
		}
	}

	// Call this when done accessing the resource
	void Done()            
	{
		// Ensure exclusive access to the member variables
		EnterCriticalSection(&m_cs);
	 
		if (m_nActive > 0) {

			// Readers have control so a reader must be done
			m_nActive--;
		} else {
	 
			// Writers have control so a writer must be done
			m_nActive++;
		}
	 
		HANDLE hsem = NULL;  // Assume no threads are waiting
		LONG lCount = 1;     // Assume only 1 waiter wakes; always true for writers
	 
		if (m_nActive == 0) {
	 
		// No thread has access, who should wake up?
		// NOTE: It is possible that readers could never get access
		//       if there are always writers wanting to write
	 
			if (m_nWaitingWriters > 0) {
		 
				// Writers are waiting and they take priority over readers
				m_nActive = -1;         // A writer will get access
				m_nWaitingWriters--;    // One less writer will be waiting
				hsem = m_hsemWriters;   // Writers wait on this semaphore
				// NOTE: The semaphore will release only 1 writer thread
		 
			} else if (m_nWaitingReaders > 0) {
		 
				// Readers are waiting and no writers are waiting
				m_nActive = m_nWaitingReaders;   // All readers will get access
				m_nWaitingReaders = 0;           // No readers will be waiting
				hsem = m_hsemReaders;            // Readers wait on this semaphore
				lCount = m_nActive;              // Semaphore releases all readers
			} else {
		 
				// There are no threads waiting at all; no semaphore gets released
			}
		}
	 
		// Allow other threads to attempt reading/writing
		LeaveCriticalSection(&m_cs);
	 
		if (hsem != NULL) {
	 
			// Some threads are to be released
			ReleaseSemaphore(hsem, lCount, NULL);
		}

	}

private:

   CRITICAL_SECTION m_cs;    // Permits exclusive access to other members
   HANDLE m_hsemReaders;     // Readers wait on this if a writer has access
   HANDLE m_hsemWriters;     // Writers wait on this if a reader has access
   int    m_nWaitingReaders; // Number of readers waiting for access
   int    m_nWaitingWriters; // Number of writers waiting for access
   int    m_nActive;         // Number of threads currently with access
                             //   (0=no threads, >0=# of readers, -1=1 writer)
};


class rwlock
{
public:

	rwlock()
		: m_write_mutex( NULL )
		, m_read_event( NULL )
		, m_readers( 0 )
	{
		m_write_mutex = ::CreateMutex( NULL, FALSE, NULL );
		m_read_event = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	}

	~rwlock()
	{
		::CloseHandle( m_read_event );
		::CloseHandle( m_write_mutex );
	}

	bool	rdlock( DWORD milliseconds = INFINITE )
	{
		DWORD code = ::WaitForSingleObject( m_write_mutex, milliseconds );
		if( code == WAIT_FAILED || code == WAIT_TIMEOUT )
			return false;

		/* We've successfully acquired the writer mutex, we can't be locked
		* for write, so it's OK to add the reader lock.  The writer mutex
		* doubles as race condition protection for the readers counter.   
		*/
		::InterlockedIncrement( &m_readers );
    
		if( ::ResetEvent( m_read_event ) == FALSE )
			return false;
    
		if( ::ReleaseMutex( m_write_mutex ) == FALSE )
			return false;

		return true;
	}

	bool	wrlock( DWORD milliseconds = INFINITE )
	{
		DWORD code = ::WaitForSingleObject( m_write_mutex, milliseconds );
		if( code == WAIT_FAILED || code == WAIT_TIMEOUT )
		return false;

		/* We've got the writer lock but we have to wait for all readers to
		* unlock before it's ok to use it.
		*/
		if( m_readers )
		{
			/* Must wait for readers to finish before returning, unless this
			* is an trywrlock (milliseconds == 0):
			*/
			if( milliseconds > 0 )
			{
				code = ::WaitForSingleObject( m_read_event, milliseconds );
			}
			else
			{
				code = WAIT_TIMEOUT;
			}

			if( code == WAIT_FAILED || code == WAIT_TIMEOUT )
			{
				/* Unable to wait for readers to finish, release write lock: */
				if( ::ReleaseMutex( m_write_mutex ) == FALSE )
					return false;
            
				return false;
			}
		}

		return true;
	}

	bool	unlock()            
	{
		DWORD rv = 0;

		/* First, guess that we're unlocking a writer */
		if( ::ReleaseMutex( m_write_mutex ) == FALSE )
			rv = ::GetLastError();

		if( rv == ERROR_NOT_OWNER )
		{
			/* Nope, we must have a read lock */
			if( m_readers &&
				! ::InterlockedDecrement( &m_readers ) &&
				! ::SetEvent( m_read_event ) )
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		return (rv == 0);
	}

private:

	HANDLE	m_write_mutex;
	HANDLE	m_read_event;
	LONG	m_readers;

};