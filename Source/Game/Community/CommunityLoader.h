#pragma once

#include <toolkit/XBossWorker.h>

struct PartyLoader : XBossWorker::XWorker
{
	bool onProcess( int nThreadNum );

	void onEnd( bool bIsCancel )	{}
};

struct GuildLoader : XBossWorker::XWorker
{
	bool onProcess( int nThreadNum );

	void onEnd( bool bIsCancel )	{}
};
