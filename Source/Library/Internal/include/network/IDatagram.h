
#pragma once

#include "XAddr.h"
#include "ISocketObject.h"

//--------------------------------------------------------------------------
// File Name: IDatagram.h
// Description: IDataram를 정의합니다.
// Created: 2006. 02. 21
// Author: Myung-Gon Park <myunggoni@nflavor.com>
//--------------------------------------------------------------------------

struct IDatagram : public IBaseObject
{
	virtual ~IDatagram() {}

	virtual bool Open(const XAddr& addr) = 0;
	virtual bool Close() = 0;
	virtual bool IsOpened() = 0;

	virtual void GetMyAddress(XAddr &addr) = 0;
	virtual const XAddr& GetMyAddress() = 0;
};

struct ISocketDatagram : public IDatagram, public ISocketObject
{
	ISocketDatagram()
	{
		m_bIsOpened = false;

		m_socket.CreateDatagramSocket();
	}

	ISocketDatagram(XSocket sock)
	{
		m_bIsOpened = false;

		m_socket = sock;
	}

	~ISocketDatagram()
	{
	}

	virtual bool Open(const XAddr& addr) = 0;
	virtual bool Close() = 0;
	virtual bool IsOpened() { return m_bIsOpened; }

	virtual void GetMyAddress(XAddr & addr) { addr = m_MyAddr; }
	virtual const XAddr& GetMyAddress() { return m_MyAddr; }

	virtual int SendTo(const XAddr& addr, const void *buf, unsigned len) = 0;
	virtual int RecvFrom(XAddr& addr, void *buf, unsigned len) = 0;

protected:
	XAddr m_MyAddr;
	bool m_bIsOpened;
};

