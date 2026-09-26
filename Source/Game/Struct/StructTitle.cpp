#include "StructTitle.h"
#include "GameAllocator.h"


StructTitle * StructTitle::AllocTitle( TitleEventHandler *pHandler, int nSID, int nCode, int nStatus )
{
	StructTitle *pObj = allocTitleStruct();
	if( !pObj )
		return NULL;

	new (pObj) StructTitle( pHandler, nSID, nCode, nStatus );

	return pObj;
}

void StructTitle::FreeTitle()
{
	prepareFreeTitleStruct( this );
	StructTitle::~StructTitle();
	freeTitleStruct( this );
}