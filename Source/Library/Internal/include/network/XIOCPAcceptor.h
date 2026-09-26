
#pragma once

/*
 *	XIOCPAcceptor.h
 *
 *  by Testors
 */

#include <list>

#include "IAcceptor.h"
#include "../toolkit/ILock.h"

class XIOCPAcceptor : public IAcceptor
{
public:
	XIOCPAcceptor();
	virtual ~XIOCPAcceptor();

protected:

	struct XOVERLAPPED*	m_pAcceptOverlapped;

	friend struct XIOCP;

	bool		pendAcceptRequest();
	void		deleteFromPendingList( SOCKET hSocket );

	XCriticalSection		m_Lock;
	std::list< XSocket* >	m_lstPendingSocket;
};

