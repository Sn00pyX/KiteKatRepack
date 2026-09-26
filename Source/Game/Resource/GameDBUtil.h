#pragma once

#include <cassert>

#import "Common Files\System\ADO\msado60_Backcompat.tlb" no_namespace rename("EOF", "EndOfFile")
#include <oledb.h>
#include <icrsint.h>

#include <toolkit/XEnv.h>


extern bool InitUserDbConnection( _ConnectionPtr & ConnPtr );
extern bool InitContentDbConnection( _ConnectionPtr & ConnPtr );
extern bool InitAdoConnection( _ConnectionPtr & ConnPtr, const char *szConnectionString, const char *szAccount, const char *szPassword );
extern bool InitAdoConnection( _ConnectionPtr & ConnPtr, const char *szDbName, const char *szServer, const char *szAccount, const char *szPassword );

template< typename STRUCT, typename FUNCTOR >
size_t LoadDbResource( const char *szCommand, _ConnectionPtr & connection, FUNCTOR & handler, long nCommandTypeEnum = adCmdTable )
{
	// Define ADO object pointers.
	// Initialize pointers on define.
	// These are in the ADODB::  namespace
	_RecordsetPtr		pRstItem = NULL;
	IADORecordBinding   *picRs  = NULL;  // Interface Pointer declared.

	STRUCT emprs;       // C++ Class object

	HRESULT hr = pRstItem.CreateInstance( __uuidof(Recordset) );

	if( hr != S_OK ) throw XException( "ADO ERROR : Can't create recordset!" );

	hr = pRstItem->Open(szCommand, 
						_variant_t((IDispatch *)connection,true),
						adOpenForwardOnly, adLockReadOnly, nCommandTypeEnum);

	if( hr != S_OK ) throw XException( "ADO ERROR : Can't open database!" );

	// Open an IADORecordBinding interface pointer which we'll 
	// use for Binding Recordset to a class.
	pRstItem->QueryInterface( __uuidof(IADORecordBinding),(LPVOID*)&picRs );

	picRs->BindToRecordset(&emprs);

	size_t cnt = 0;
	while( hr == S_OK && pRstItem->State != adStateClosed && !pRstItem->EndOfFile )
	{	
		handler( &emprs );
		++cnt;

		hr = pRstItem->MoveNext();
	}

	picRs->Release();
	pRstItem->Close();	

	return cnt;
}

// 나름대로 분석해본 decimal/numeric 타입의 바이너리 구조.. 를 DECIMAL 을 발견해서 고쳤다. ㅠ.ㅠ
struct _decimal_variant : public DECIMAL
{
	__int64 getMultipleInteger( const int & nMulti )
	{
		int s = 1;
		for( unsigned i = 0; i < scale; ++i )
			s *= 10;

		if( sign )
			s = -s;

		return static_cast< __int64 >( Lo64 ) * nMulti / s;
	}

	void setMultipleInteger( const __int64 & value, const int & nMulti )
	{
		assert( nMulti > 0 && ( nMulti >= 10 || nMulti % 10 == 0 ) );

		wReserved = 0;
		sign = ( value < 0 ) ? 0x80 : 0;

		scale = 0;
		int nMultiBuf = nMulti;
		while( nMultiBuf >= 10 )
		{
			assert( nMultiBuf % 10 == 0 );

			++scale;
			nMultiBuf /= 10;
		}
		
		Hi32 = 0;
		Lo64 = static_cast< ULONGLONG >( value * ( sign ? -1 : 1 ) );
	}

	float getFloat()
	{
		float f;

		VarR4FromDec( this, &f );

		return f;
	}
	
	double getDouble()
	{
		double f;

		VarR8FromDec( this, &f );

		return f;
	}
};