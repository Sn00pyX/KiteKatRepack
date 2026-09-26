#pragma once


struct ArBoxCollision
{
	ArBoxCollision() : bIsGlobal( false )
	{
		left = top = right = bottom = -1;
		layer = -1;
	}

	ArBoxCollision( bool _bIsGlobal ) : bIsGlobal( _bIsGlobal )
	{
		left = top = 0;
		right = bottom = 0x7fffffff;
		layer = -1;
	}

	ArBoxCollision( AR_UNIT ax,  AR_UNIT ay,  AR_UNIT atx,  AR_UNIT aty, short _layer )
	{
		bIsGlobal = false;
		left	= GetRegionX( ax );
		top		= GetRegionY( ay );
		right	= GetRegionX( atx );
		bottom	= GetRegionY( aty );
		layer	= _layer;
	};

	ArBoxCollision( unsigned _rx1, unsigned _ry1, short _layer )
	{
		bIsGlobal = false;
		left = (int)_rx1 - VISIBLE_REGION_RANGE - 2;
		right = (int)_rx1 + VISIBLE_REGION_RANGE + 2;
		top = (int)_ry1 - VISIBLE_REGION_RANGE - 2;
		bottom = (int)_ry1 + VISIBLE_REGION_RANGE + 2;
		layer = _layer;
	};

	ArBoxCollision( unsigned _rx1, unsigned _ry1, unsigned range, short _layer )
	{
		bIsGlobal = false;
		left = (int)_rx1 - range - 2;
		right = (int)_rx1 + range + 2;
		top = (int)_ry1 - range - 2;
		bottom = (int)_ry1 + range + 2;
		layer = _layer;
	};

	ArBoxCollision( unsigned _rx1, unsigned _ry1, unsigned _rx2, unsigned _ry2, short _layer )
	{
		bIsGlobal = false;
		left = (std::min)( _rx1, _rx2 ) - VISIBLE_REGION_RANGE - 2;
		right = (std::max)( _rx1, _rx2 ) + VISIBLE_REGION_RANGE + 2;
		top = (std::min)( _ry1, _ry2 ) - VISIBLE_REGION_RANGE - 2;
		bottom = (std::max)( _ry1, _ry2 ) + VISIBLE_REGION_RANGE + 2;
		layer = _layer;
	};

	bool is_subset_of( const ArBoxCollision & c ) const
	{
		if( c.bIsGlobal ) return true;

		if( bIsGlobal && !c.bIsGlobal ) return false;
		if( c.left > left ) return false;
		if( c.right < right ) return false;
		if( c.top > top ) return false;
		if( c.bottom < bottom ) return false;

		if( c.layer != -1 && ( layer == -1 || layer != c.layer ) ) return false;

		return true;
	}

	bool is_exclusive( const ArBoxCollision & c ) const
	{
		// 전역락 충돌 판정
		if( bIsGlobal || c.bIsGlobal ) return true;

		// 전역 락이 아닌 경우에는 레이어 충돌 판정(충돌 성공의 경우가 아니라 절대로 충돌할 수 없는 경우만 체크)
		if( layer != -1 && c.layer != -1 && layer != c.layer ) return false;

		// 박스간 충돌 판정
		if( (std::min)( right, c.right ) >= (std::max)( left,  c.left ) && (std::min)( bottom,  c.bottom  ) >= (std::max)( top, c.top ) ) return true;

		return false;
	}

	bool bIsGlobal;
	int left, right, top, bottom;
	short layer;
};