#include "StructTitleCondition.h"
#include "GameAllocator.h"

StructTitleCondition * StructTitleCondition::AllocTitleCondition( const int nSID, const int nType, const __int64 nCount )
{
	StructTitleCondition *pObj = allocTitleConditionStruct();
	if( !pObj )
		return NULL;

	new (pObj) StructTitleCondition( nSID, nType, nCount );

	return pObj;
}

void StructTitleCondition::FreeTitleCondition()
{
	prepareFreeTitleConditionStruct( this );
	StructTitleCondition::~StructTitleCondition();
	freeTitleConditionStruct( this );
}