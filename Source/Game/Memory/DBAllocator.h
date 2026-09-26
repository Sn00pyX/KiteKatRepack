#pragma once

#include <toolkit/XMemoryPool.h>
#include "GameDBManager.h"


template< typename NewType, size_t CurrentMaxSize >
class SizeComparer
{
public:
	enum
	{
		LARGER_SIZE		= ( CurrentMaxSize > sizeof( NewType ) ) ? CurrentMaxSize : sizeof( NewType ),
	};
};

#define COMPILE_TIME_SIZE_COMPARE( ARG1, ARG2 ) SizeComparer< ARG1, ARG2 >::LARGER_SIZE


XMemoryPool &					GetDBHeap();

void							InitDBHeap();

struct GameDBManager::DBProc *	allocDBProcStruct();
void							prepareFreeDBProcStruct( struct GameDBManager::DBProc * pStruct );
void							freeDBProcStruct( struct GameDBManager::DBProc * pStruct );
