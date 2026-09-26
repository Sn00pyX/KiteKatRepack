#pragma once

#include <toolkit/XBossWorker.h>

struct LocationLoader : XBossWorker::XWorker
{
	bool onProcess( int nThreadNum );

	void onEnd( bool bIsCancel )	{}

protected:
	void loadLocationList();
	static void onLocationInfo( struct dbLocation * emprs );

};
