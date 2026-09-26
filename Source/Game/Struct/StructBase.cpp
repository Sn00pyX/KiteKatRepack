
#include <toolkit/ILock.h>

#include "StructBase.h"

static XCriticalSection s_Lock( "s_Lock" );

extern struct GameObject*		getPtrFromId( AR_HANDLE uid );

GameObject*		GameObject::raw_get( AR_HANDLE handle )
{
	if( !handle ) return NULL;
	return getPtrFromId( handle );	
}

GameObject::iterator GameObject::get( AR_HANDLE handle )
{
	THREAD_SYNCRONIZE( s_Lock );

	iterator It;
	GameObject* pPtr = NULL; 
	if( handle ) pPtr = getPtrFromId( handle );

	// 삭제 대기 중인것은 KIN
	if( pPtr && !pPtr->IsDeleteRequested() ) It.set( pPtr );
	
	return It;
}

