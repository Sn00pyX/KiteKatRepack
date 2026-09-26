#if !defined(_KBINARYFILER_H_INCLUDED_)
#define _KBINARYFILER_H_INCLUDED_

#pragma once


#include "KFiler.h"

class KAsciiFiler;
class KBinaryFiler : public KFiler
{
public:
	KBinaryFiler();
	~KBinaryFiler();

	virtual bool Save( KStream &stream );
	virtual bool Load( KStream &stream );
	KAsciiFiler* GetAsciiFiler();	

};

#endif // !defined(_KBINARYFILER_H_INCLUDED_)
