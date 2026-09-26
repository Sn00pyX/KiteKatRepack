#pragma once

#include <cstddef>
#include <numeric>


namespace SimpleCipher
{
	template < class _T1, class _T2 > struct MagicNumOp
	{
		_T1 operator () ( const _T1& _Left, const _T2& _Right ) const
		{ return ( _Left + static_cast <_T1>( _Right ) * 65539 ); }
	};

	template <class S, class T> int Encrypt( size_t nOriginalLength, const S& src, T& dest )
	{
		static_assert( sizeof( S::value_type ) == 1, "size is too big" );
		static_assert( sizeof( T::value_type ) == 1, "size is too big" );

		size_t sz = src.size();

		// 1 바이트 이하의 데이터는 원본 그대로 적용
		{
			if( !sz )
				return 0;
			else if( sz == 1 )
			{
				dest.resize( 1 );
				dest[ 0 ] = src[ 0 ];

				return 0;
			}
		}

		// MagicNum은 4바이트 양수이며, 오버플로우는 허용되므로 정상적으로 역산이 가능하다.
		unsigned int magicnum = unsigned int( std::accumulate( src.begin(), src.end(), unsigned int( sz ), MagicNumOp < unsigned int, S::value_type > () ) );

		// nOriginalLength와 magicnum을 위하여 8바이트를 사용한다.
		dest.resize( sz + 4 * 2 );

		int nStart = magicnum % int( sz );
		int nJump = (magicnum & 0xfff0) + 1;

		// nJump 값 보정
		// * 공약수 계산으로 인해 sz값이 커지면 커질 수록 연산 시간이 오래 걸릴 수 있음(최대 sz값과 비례하여 연산 시간 증가)
		// * 단순히 생각하면 nJump가 소수이면 sz 범위 내에서 아무리 nJump 씩 인덱스를 이동시켜도 문제가 없을 것 같지만
		//   sz가 특정 소수의 배수(약수 아님)일 경우에는 nJump가 소수여도 nJump의 약수일 수 있으며, 인덱스 중첩이 발생함.
		//   따라서 nJump는 소수 여부와는 무관하게 sz와 공약수가 1 외에는 없는 숫자여야 함.
		// * 주석이 코드보다 많아서 그렇지 절대 긴 코드가 아님 -,. -;;
		{
			// SimpleCipher의 nJump 계산 결과값을 sz로 나눈 나머지를 2로 나누고 -2 하여 최대 sz / 2 - 2 보다 작은 수가 나오게 함.
			nJump = int( ( nJump % sz ) / 2 - 2 );
			// 계산된 nJump가 1 이하이면 별도의 룰에 따라 nJump(의 최대값)를 다시 계산.
			if( nJump <= 1 )
			{
				// 데이터가 4 바이트 이하일 경우에는 nJump는 무조건 sz-1
				// sz == 4 : 1, 3이 유효 nJump가 될 수 있는데 1은 피하는 편이 좋으므로 3.
				// sz == 3 : 1, 2가 유효 nJump가 될 수 있는데 1은 피하는 편이 좋으므로 2.
				// sz == 2 : 1 밖에 유효 nJump가 없음 -,. -;
				// 결론: 4바이트 이하는 nJump = sz - 1 이 최적(이라기보다는 그나마 나은)의 답임
				if( sz <= 4 )
					nJump = int( sz - 1 );
				// 데이터가 16 바이트 이하일 경우에는 nJump를 sz-2 이하이면서 sz와 공약수가 없는 수를 구하도록 함
				// * 여기서는 nJump의 최대값만 세팅하고 그 이하의 숫자 중 sz와 공약수가 없는 수는 아래쪽에서 구함
				else if( sz <= 16 )
					nJump = int( sz - 2 );
				// 데이터가 17 바이트 이상일 경우에는(위의 두 조건에 해당 안 되는 모든 경우) nJump는 sz / 2 - 1 보다 작은 sz와 공약수가 없는 수를 구함
				else
					nJump = int( sz / 2 - 1 );
			}

			// 현재 계산된 nJump 이하의 값들 중 sz와 공약수가 없는 수를 구하기...
			// sz의 약수 목록 미리 구해두기
			// sz를 나눌 수 있는 약수 목록
			std::vector< int > vDivisorList;
			{
				int nMaxValue = int( sz );
				// vDivisorList에 n, (Num/n)의 2개의 숫자가 동시에 들어가면 정렬을 시킬 수 없으므로 (Num/n)을 몫(quotient) 리스트에 따로 넣었다가 나중에 vDivisorList로 합침
				std::vector< int > vQuotientList;
				for( int nDivisor = 2 ; nDivisor < nMaxValue ; ++nDivisor )
				{
					// 약수가 아니면 패스
					if( sz % nDivisor )
						continue;

					// Num이 n으로 나누어 떨어지는 경우에 약수는 n, (Num/n) 2개이며, n이 (Num/n)보다 크거나 같아지면 해당 n보다 큰 n은 이미 (Num/n)으로 등장했던 약수가 됨.
					vDivisorList.push_back( nDivisor );
					nMaxValue = int( sz / nDivisor );
					// sz가 특정 수의 제곱수일 경우 nDivisor == nMaxValue가 됨
					if( nDivisor != nMaxValue )
						vQuotientList.push_back( nMaxValue );
				}

				// 몫 리스트를 약수 리스트에 합치기
				// * 몫은 큰 수부터 들어있으므로 역순으로 vDivisorList에 추가함
				vDivisorList.reserve( vDivisorList.size() + vQuotientList.size() );
				for( std::vector< int >::const_reverse_iterator it = vQuotientList.rbegin() ; it != vQuotientList.rend() ; ++it )
				{
					vDivisorList.push_back( (*it) );
				}
			}

			// nJump 보다 같거나 작은 숫자 중 가장 큰 sz와의 공약수 없는 수 구하기
			for( ; nJump > 1 ; --nJump )
			{
				std::vector< int >::const_iterator it;
				for( it = vDivisorList.begin() ; it != vDivisorList.end() ; ++it )
				{
					// sz의 약수가 nJump의 약수인 경우 이후 약수에 대해서는 검사 중지
					if( !( nJump % (*it) ) )
						break;
				}

				// 공약수가 없었던 경우 현재 nJump 값을 사용하도록 루프 중지
				if( it == vDivisorList.end() )
					break;
			}
		}

		BYTE ucValue = *src.begin();

		dest[ nStart ] = static_cast< T::value_type >( ucValue ^ magicnum );
		nStart += nJump;
		nStart %= sz;

		for ( size_t i = 1; i < sz; ++i )
		{
			assert( !dest[ nStart ] );
			dest[ nStart ] = static_cast< T::value_type >( ( src[i] - ucValue ) ^ magicnum );
			ucValue = src[i];

			nStart += nJump;
			nStart %= sz;
		}
		dest[sz + 0] = T::value_type( ( ( nOriginalLength >> 24 ) & 0xff ) ^ magicnum );
		dest[sz + 1] = T::value_type( ( ( nOriginalLength >> 16 ) & 0xff ) ^ magicnum );
		dest[sz + 2] = T::value_type( ( ( nOriginalLength >>  8 ) & 0xff ) ^ magicnum );
		dest[sz + 3] = T::value_type( ( ( nOriginalLength       ) & 0xff ) ^ magicnum );

		dest[sz + 4] = T::value_type( ( magicnum >> 24 ) & 0xff );
		dest[sz + 5] = T::value_type( ( magicnum >> 16 ) & 0xff );
		dest[sz + 6] = T::value_type( ( magicnum >>  8 ) & 0xff );
		dest[sz + 7] = T::value_type( ( magicnum       ) & 0xff );

		return 0;
	};

	template <class S, class T> int Decrypt( const S& src, T& dest, size_t* pnOriginalLength )
	{
		S temp;

		// 1 바이트 이하의 데이터는 원본 그대로 적용
		size_t nSrcSize = src.size();
		if( nSrcSize == 1 )
		{
			if( !nSrcSize )
			{
				*pnOriginalLength = 0;

				return 0;
			}
			else if( nSrcSize == 1 )
			{
				dest.resize( 1 );
				dest[ 0 ] = src[ 0 ];
				*pnOriginalLength = 1;

				return 0;
			}
		}
		// 원본 데이터가 2바이트 이상이면 8 바이트의 추가 정보(nOriginalLength, magicnum)도 붙으므로 10 바이트 이상으로 길이가 늘어남.
		else if( nSrcSize < 10 )
		{
			assert( 0 );
			return 0;
		}

		// nOriginalLength와 magicnum을 위하여 8바이트를 사용한다.
		size_t sz = nSrcSize - 4 * 2;

		unsigned int magicnum;

		temp.resize( src.size() + 4 );

		magicnum = static_cast<BYTE> ( src[sz + 4 ] );
		magicnum = magicnum * 256 + static_cast<BYTE> ( src[sz + 5] );
		magicnum = magicnum * 256 + static_cast<BYTE> ( src[sz + 6] );
		magicnum = magicnum * 256 + static_cast<BYTE> ( src[sz + 7] );

		for ( size_t i = 0; i < sz + 4; ++i )
			temp[i] = static_cast< S::value_type >( src[i] ^ magicnum );

		*pnOriginalLength =                           static_cast<BYTE> ( temp[ sz + 0 ] );
		*pnOriginalLength = *pnOriginalLength * 256 + static_cast<BYTE> ( temp[ sz + 1 ] );
		*pnOriginalLength = *pnOriginalLength * 256 + static_cast<BYTE> ( temp[ sz + 2 ] );
		*pnOriginalLength = *pnOriginalLength * 256 + static_cast<BYTE> ( temp[ sz + 3 ] );

		int nStart = magicnum % int( sz );
		int nJump = (magicnum & 0xfff0) + 1;

		// nJump 값 보정
		// * 공약수 계산으로 인해 sz값이 커지면 커질 수록 연산 시간이 오래 걸릴 수 있음(최대 sz값과 비례하여 연산 시간 증가)
		// * 단순히 생각하면 nJump가 소수이면 sz 범위 내에서 아무리 nJump 씩 인덱스를 이동시켜도 문제가 없을 것 같지만
		//   sz가 특정 소수의 배수(약수 아님)일 경우에는 nJump가 소수여도 nJump의 약수일 수 있으며, 인덱스 중첩이 발생함.
		//   따라서 nJump는 소수 여부와는 무관하게 sz와 공약수가 1 외에는 없는 숫자여야 함.
		// * 주석이 코드보다 많아서 그렇지 절대 긴 코드가 아님 -,. -;;
		{
			// SimpleCipher의 nJump 계산 결과값을 sz로 나눈 나머지를 2로 나누고 -2 하여 최대 sz / 2 - 2 보다 작은 수가 나오게 함.
			nJump = int( ( nJump % sz ) / 2 - 2 );
			// 계산된 nJump가 1 이하이면 별도의 룰에 따라 nJump(의 최대값)를 다시 계산.
			if( nJump <= 1 )
			{
				// 데이터가 4 바이트 이하일 경우에는 nJump는 무조건 sz-1
				// sz == 4 : 1, 3이 유효 nJump가 될 수 있는데 1은 피하는 편이 좋으므로 3.
				// sz == 3 : 1, 2가 유효 nJump가 될 수 있는데 1은 피하는 편이 좋으므로 2.
				// sz == 2 : 1 밖에 유효 nJump가 없음 -,. -;
				// 결론: 4바이트 이하는 nJump = sz - 1 이 최적(이라기보다는 그나마 나은)의 답임
				if( sz <= 4 )
					nJump = int( sz - 1 );
				// 데이터가 16 바이트 이하일 경우에는 nJump를 sz-2 이하이면서 sz와 공약수가 없는 수를 구하도록 함
				// * 여기서는 nJump의 최대값만 세팅하고 그 이하의 숫자 중 sz와 공약수가 없는 수는 아래쪽에서 구함
				else if( sz <= 16 )
					nJump = int( sz - 2 );
				// 데이터가 17 바이트 이상일 경우에는(위의 두 조건에 해당 안 되는 모든 경우) nJump는 sz / 2 - 1 보다 작은 sz와 공약수가 없는 수를 구함
				else
					nJump = int( sz / 2 - 1 );
			}

			// 현재 계산된 nJump 이하의 값들 중 sz와 공약수가 없는 수를 구하기...
			// sz의 약수 목록 미리 구해두기
			// sz를 나눌 수 있는 약수 목록
			std::vector< int > vDivisorList;
			{
				int nMaxValue = int( sz );
				// vDivisorList에 n, (Num/n)의 2개의 숫자가 동시에 들어가면 정렬을 시킬 수 없으므로 (Num/n)을 몫(quotient) 리스트에 따로 넣었다가 나중에 vDivisorList로 합침
				std::vector< int > vQuotientList;
				for( int nDivisor = 2 ; nDivisor < nMaxValue ; ++nDivisor )
				{
					// 약수가 아니면 패스
					if( sz % nDivisor )
						continue;

					// Num이 n으로 나누어 떨어지는 경우에 약수는 n, (Num/n) 2개이며, n이 (Num/n)보다 크거나 같아지면 해당 n보다 큰 n은 이미 (Num/n)으로 등장했던 약수가 됨.
					vDivisorList.push_back( nDivisor );
					nMaxValue = int( sz / nDivisor );
					// sz가 특정 수의 제곱수일 경우 nDivisor == nMaxValue가 됨
					if( nDivisor != nMaxValue )
						vQuotientList.push_back( nMaxValue );
				}

				// 몫 리스트를 약수 리스트에 합치기
				// * 몫은 큰 수부터 들어있으므로 역순으로 vDivisorList에 추가함
				vDivisorList.reserve( vDivisorList.size() + vQuotientList.size() );
				for( std::vector< int >::const_reverse_iterator it = vQuotientList.rbegin() ; it != vQuotientList.rend() ; ++it )
				{
					vDivisorList.push_back( (*it) );
				}
			}

			// nJump 보다 같거나 작은 숫자 중 가장 큰 sz와의 공약수 없는 수 구하기
			for( ; nJump > 1 ; --nJump )
			{
				std::vector< int >::const_iterator it;
				for( it = vDivisorList.begin() ; it != vDivisorList.end() ; ++it )
				{
					// sz의 약수가 nJump의 약수인 경우 이후 약수에 대해서는 검사 중지
					if( !( nJump % (*it) ) )
						break;
				}

				// 공약수가 없었던 경우 현재 nJump 값을 사용하도록 루프 중지
				if( it == vDivisorList.end() )
					break;
			}
		}

		BYTE ucValue = 0;

		dest.resize( src.size() );

		for ( size_t i = 0; i < sz; ++i )
		{
			ucValue += temp[ nStart ];
			dest[ i ] = ucValue;
			nStart += nJump;
			nStart %= sz;
		}

		return 0;
	}
};
