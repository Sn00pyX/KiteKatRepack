#pragma once

// È£°¨µµ
struct FAVOR_INFO
{
	FAVOR_INFO( int _favor_id, int _favor ) : favor_id( _favor_id ), favor( _favor ), bNeedToUpdate( false ) {}

	int favor_id;
	int favor;

	bool bNeedToUpdate;
};