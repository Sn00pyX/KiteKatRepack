#pragma once

#include "../../toolkit/khash.h"

#include <miles/mss.h>

class PSoundFilterManager
{
public:
	PSoundFilterManager(){;}
	~PSoundFilterManager(){;}	
	
	void        Initialize();
	HPROVIDER   GetFilter( const char *szFilter);
		
protected:
	KHash<HPROVIDER, hashPr_string>      m_hashFilter;	
};



void PSoundFilterManager::Initialize()
{	
	HPROVIDER    avail;
	char *       szFilterName;

	HPROENUM next = HPROENUM_FIRST;

	while (AIL_enumerate_filters( &next,  &avail,  &szFilterName) )        
	{		
		m_hashFilter.add( szFilterName, avail);
	}
}

HPROVIDER   PSoundFilterManager::GetFilter( const char *szFilter)
{
	HPROVIDER filter;
	m_hashFilter.lookup( szFilter, filter);
	return filter;
}

PSoundFilterManager&  SOUND_FILTER()
{
	static PSoundFilterManager SoundFilter;
	return SoundFilter;
}