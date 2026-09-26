
#include "../../include/toolkit/ExclusiveLogicLock.h"

__declspec( thread ) static bool	s_LockFlag = false;

//template< typename LOGIC, typename LOCK >
void _exclusive_lock_set_lock()
{
	s_LockFlag = true;
}

//template< typename LOGIC, typename LOCK >
void _exclusive_lock_unset_lock()
{
	s_LockFlag = false;
}

//template< typename LOGIC, typename LOCK >
bool _exclusive_lock_checkDoubleLock()
{
	return s_LockFlag;
}