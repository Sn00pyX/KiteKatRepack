#if !defined(_KASCIIFILER_H_INCLUDED_)
#define _KASCIIFILER_H_INCLUDED_

#pragma once

#include "KStream.h"
#include "KFiler.h"
#include "KBinaryFiler.h"

class KBinaryFiler;

class KAsciiFiler : public KFiler
{
public:
	KAsciiFiler();
	~KAsciiFiler();

	virtual bool Save( KStream &stream );
	virtual bool Load( KStream &stream );
	KBinaryFiler* GetBinaryFiler( );
};

#endif // !defined(_KASCIIFILER_H_INCLUDED_)
