
#include <string>

#include "../../include/network/XNetworkUtil.h"

namespace XNetworkUtil
{

	static int	s_nRefCount = 0;

	bool InitNetwork()
	{
		if ( s_nRefCount++ > 1 ) return true;

		WSADATA Wsa;
		if ( !WSAStartup( MAKEWORD( 2, 2 ),& Wsa ) )	return true;
		
		return false;
	}

	void DeInitNetwork()
	{
		if ( s_nRefCount-- ) return;

		WSACleanup();

		return;
	}

	bool QueryDNS( XAddr & addr )
	{
		struct hostent* i;
		
		if ( !addr.GetAddr() ) return 0;

		i = gethostbyname( addr.GetAddr() );
		
		if ( !i ) return 0;

		long nIP;
		s_memcpy( &nIP, sizeof( nIP ), i->h_addr_list[ 0 ], i->h_length );

		struct sockaddr_in temp_addr;
		ConvAddr( addr, temp_addr );
		temp_addr.sin_addr.S_un.S_addr = nIP;
		ConvAddr( temp_addr, addr );
		
		return true;
	}

	void	GetMyIPList( std::vector< std::string >& vIPs )
	{
		InitNetwork();

		char name[256] = { 0, };
		PHOSTENT pHostInfo = NULL;

		// 자신의 호스트이름 얻기
		if( gethostname( name, _countof( name ) ) == 0 )
		{
			// 호스트이름을 통하여 호스트정보 얻기
			pHostInfo = gethostbyname( name );
			if( pHostInfo )
			{
				// 반복해서 IP얻기
				for( int i = 0; pHostInfo->h_addr_list[i] != NULL; ++i )
				{
					XAddr ip;
					struct sockaddr_in addr;
					s_memcpy( &addr.sin_addr, sizeof( addr.sin_addr ), pHostInfo->h_addr_list[i], pHostInfo->h_length );

					if( XNetworkUtil::ConvAddr( addr, ip ) == true )
					{
						vIPs.push_back( ip.GetAddr() );
					}
				}
			}
		}

		DeInitNetwork();
	}

	bool ConvAddr( const XAddr & addr, struct sockaddr_in & addr_in )
	{
		unsigned long unAddr;
		
		unAddr = inet_addr( addr.GetAddr() );

		if ( INADDR_NONE == unAddr )
		{
			struct hostent FAR * hostEnt;
			hostEnt = gethostbyname( addr.GetAddr() );

			if ( !hostEnt ) return false;

			s_memcpy( &unAddr, sizeof( unAddr ), hostEnt->h_addr_list[ 0 ], hostEnt->h_length );
		}

		memset( &addr_in, 0, sizeof(addr_in) );
		addr_in.sin_family				= AF_INET;
		addr_in.sin_addr.S_un.S_addr	= unAddr;
		addr_in.sin_port				= htons( static_cast< u_short >( addr.GetPort() ) );

		return true;
	}

	bool ConvAddr( const struct sockaddr_in & addr_in, XAddr & addr )
	{
		addr.SetPortNum( ntohs( addr_in.sin_port ) );
		addr.SetAddr( inet_ntoa( addr_in.sin_addr ) );

		return true;
	}

	enum
	{
		ENDIAN_UNKNOWN	= 0,
		ENDIAN_LITTLE	= 1,
		ENDIAN_BIG		= 2,
	};

	int		GetMachineEndian()
	{
		static int	g_nMachineEndian = ENDIAN_UNKNOWN;
		if( g_nMachineEndian == ENDIAN_UNKNOWN )
		{
			const int check_endian = 1;
			const char* byte = reinterpret_cast< const char* >( &check_endian );
			if( *byte == 1 )
			{
				/* little endian */
				g_nMachineEndian = ENDIAN_LITTLE;
			}
			else
			{
				/* big endian */
				g_nMachineEndian = ENDIAN_BIG;
			}
		}

		return g_nMachineEndian;
	}

	unsigned __int64	htonll( unsigned __int64 host_longlong )
	{
		if( GetMachineEndian() == ENDIAN_LITTLE )
		{
			return	(static_cast< unsigned __int64 >( htonl( static_cast< u_long >( host_longlong ) ) ) << 32) +
					htonl( static_cast< u_long >( host_longlong >> 32 ) );
		}

		return host_longlong;
	}

	unsigned __int64	ntohll( unsigned __int64 host_longlong )
	{
		if( GetMachineEndian() == ENDIAN_LITTLE )
		{
			return	(static_cast< unsigned __int64 >( ntohl( static_cast< u_long >( host_longlong ) ) ) << 32) +
					ntohl( static_cast< u_long >( host_longlong >> 32 ) );
		}

		return host_longlong;

	}

	std::string GetWin32ErrorInfo( int nErrNo )
	{
		std::string strError = "Unknown";

		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			nErrNo,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),	// 영어인편이 디버깅 편하다. -.-
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
		);
		if ( lpMsgBuf )
		{
			strError += static_cast< char* >( lpMsgBuf );
			
			LocalFree( lpMsgBuf );
		}

		return strError;
	}

}; // namespace XUtil