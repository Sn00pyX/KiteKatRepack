
#pragma once

/*
 *	XUtility.h
 *
 *  by Testors
 */

#include <winsock2.h>
#include <string>
#include <vector>

#include "XAddr.h"

namespace XNetworkUtil
{
	bool InitNetwork();
	void DeInitNetwork();
	bool QueryDNS( XAddr & addr );
	void GetMyIPList( std::vector< std::string >& vIPs );
	bool ConvAddr( const XAddr & addr, struct sockaddr_in & addr_in );
	bool ConvAddr( const struct sockaddr_in & addr_in, XAddr & addr );
	std::string GetWin32ErrorInfo( int nErrNo );

	/**
	 * byte ordering function for 64bit variable
	 */
	unsigned __int64	htonll( unsigned __int64 host_longlong );
	unsigned __int64	ntohll( unsigned __int64 host_longlong );

	class	XNetworkInitHelper
	{
	public:

		XNetworkInitHelper()
		{
			XNetworkUtil::InitNetwork();
		}
		~XNetworkInitHelper()
		{
			XNetworkUtil::DeInitNetwork();
		}
	};

};
