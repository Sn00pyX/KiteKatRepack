#ifndef _SPRITE_TML_H_
#define _SPRITE_TML_H_
#include "KFiler.h"

class KFT_bound_box
{
public:
	KFT_bound_box()
	{
		m_pObj = NULL;
		m_pMem_bound_rect = NULL;
		m_pMem_id = NULL;
	}
	KFT_bound_box( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "bound_box" );
		m_pMem_bound_rect = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "bound_rect" ));
		m_pMem_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "id" ));
	}
	KFT_bound_box( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_bound_rect = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "bound_rect" ));
		m_pMem_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "id" ));
	}
	~KFT_bound_box()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_bound_rect = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "bound_rect" ));
		m_pMem_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "id" ));
	}
	int GetArrayCount_bound_rect()
	{
		return m_pMem_bound_rect->GetDataCount();
	}
	void GetArrayValue_bound_rect( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_bound_rect->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_bound_rect()
	{
		m_pMem_bound_rect->RefreshArray();
	}
	void SetArrayValue_bound_rect( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_bound_rect->SetDataDWORD( index, data, count );
	}
	bool GetString_id( char *data, int &nLen )
	{
		return m_pMem_id->GetString( data, nLen );
	}
	bool SetString_id( const char *data, int nLen )
	{
		return m_pMem_id->SetString( data, nLen );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "bound_box" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataArrayObject*		m_pMem_bound_rect;
	KSimpleDataObject*		m_pMem_id;
};
class KFTA_bound_box
{
public:
	KFTA_bound_box()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_bound_box( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_bound_box( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "bound_box" );
	}
	~KFTA_bound_box()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_bound_box &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_old_frame_type
{
public:
	KFT_old_frame_type()
	{
		m_pObj = NULL;
		m_pMem_delay = NULL;
		m_pMem_image_origin = NULL;
		m_pMem_attr = NULL;
		m_pMem_attrlock = NULL;
		m_pMem_bound_box_size = NULL;
		m_pMem_bound_box_array = NULL;
		m_pMem_library_number = NULL;
	}
	KFT_old_frame_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "old_frame_type" );
		m_pMem_delay = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "delay" ));
		m_pMem_image_origin = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "image_origin" ));
		m_pMem_attr = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attr" ));
		m_pMem_attrlock = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attrlock" ));
		m_pMem_bound_box_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bound_box_size" ));
		m_pMem_bound_box_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bound_box_array" ));
		m_pMem_library_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_number" ));
	}
	KFT_old_frame_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_delay = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "delay" ));
		m_pMem_image_origin = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "image_origin" ));
		m_pMem_attr = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attr" ));
		m_pMem_attrlock = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attrlock" ));
		m_pMem_bound_box_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bound_box_size" ));
		m_pMem_bound_box_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bound_box_array" ));
		m_pMem_library_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_number" ));
	}
	~KFT_old_frame_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_delay = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "delay" ));
		m_pMem_image_origin = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "image_origin" ));
		m_pMem_attr = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attr" ));
		m_pMem_attrlock = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attrlock" ));
		m_pMem_bound_box_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bound_box_size" ));
		m_pMem_bound_box_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bound_box_array" ));
		m_pMem_library_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_number" ));
	}
	unsigned long GetValue_delay()
	{
		unsigned long res;
		m_pMem_delay->GetDWORD( &res );
		return res;
	}
	void SetValue_delay( unsigned long value )
	{
		m_pMem_delay->SetDWORD( value );
	}
	int GetArrayCount_image_origin()
	{
		return m_pMem_image_origin->GetDataCount();
	}
	void GetArrayValue_image_origin( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_image_origin->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_image_origin()
	{
		m_pMem_image_origin->RefreshArray();
	}
	void SetArrayValue_image_origin( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_image_origin->SetDataDWORD( index, data, count );
	}
	int GetArrayCount_attr()
	{
		return m_pMem_attr->GetDataCount();
	}
	void GetArrayValue_attr( int index, float *data, size_t data_size, int count )
	{
		m_pMem_attr->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_attr()
	{
		m_pMem_attr->RefreshArray();
	}
	void SetArrayValue_attr( int index, const float *data, int count = 1 )
	{
		m_pMem_attr->SetDataFloat( index, data, count );
	}
	int GetArrayCount_attrlock()
	{
		return m_pMem_attrlock->GetDataCount();
	}
	void GetArrayValue_attrlock( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_attrlock->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_attrlock()
	{
		m_pMem_attrlock->RefreshArray();
	}
	void SetArrayValue_attrlock( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_attrlock->SetDataDWORD( index, data, count );
	}
	unsigned long GetValue_bound_box_size()
	{
		unsigned long res;
		m_pMem_bound_box_size->GetDWORD( &res );
		return res;
	}
	void SetValue_bound_box_size( unsigned long value )
	{
		m_pMem_bound_box_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_bound_box_array( KFTA_bound_box &obj )
	{
		obj.SetData( m_pMem_bound_box_array );
	}
	unsigned long GetValue_library_number()
	{
		unsigned long res;
		m_pMem_library_number->GetDWORD( &res );
		return res;
	}
	void SetValue_library_number( unsigned long value )
	{
		m_pMem_library_number->SetDWORD( value );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "old_frame_type" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_delay;
	KSimpleDataArrayObject*		m_pMem_image_origin;
	KSimpleDataArrayObject*		m_pMem_attr;
	KSimpleDataArrayObject*		m_pMem_attrlock;
	KSimpleDataObject*		m_pMem_bound_box_size;
	KTemplateDataArrayObject*		m_pMem_bound_box_array;
	KSimpleDataObject*		m_pMem_library_number;
};
class KFTA_old_frame_type
{
public:
	KFTA_old_frame_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_old_frame_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_old_frame_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "old_frame_type" );
	}
	~KFTA_old_frame_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_old_frame_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_frame_type
{
public:
	KFT_frame_type()
	{
		m_pObj = NULL;
		m_pMem_delay = NULL;
		m_pMem_image_origin = NULL;
		m_pMem_attr = NULL;
		m_pMem_attrlock = NULL;
		m_pMem_bound_box_size = NULL;
		m_pMem_bound_box_array = NULL;
		m_pMem_library_number = NULL;
		m_pMem_flags = NULL;

		m_pMem_library_name = NULL;
	}
	KFT_frame_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "frame_type" );
		m_pMem_delay = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "delay" ));
		m_pMem_image_origin = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "image_origin" ));
		m_pMem_attr = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attr" ));
		m_pMem_attrlock = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attrlock" ));
		m_pMem_bound_box_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bound_box_size" ));
		m_pMem_bound_box_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bound_box_array" ));
		m_pMem_library_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_number" ));
		m_pMem_flags = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "flags" ));

		m_pMem_library_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject("library_name"));
	}
	KFT_frame_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_delay = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "delay" ));
		m_pMem_image_origin = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "image_origin" ));
		m_pMem_attr = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attr" ));
		m_pMem_attrlock = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attrlock" ));
		m_pMem_bound_box_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bound_box_size" ));
		m_pMem_bound_box_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bound_box_array" ));
		m_pMem_library_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_number" ));
		m_pMem_flags = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "flags" ));

		m_pMem_library_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject("library_name"));
	}
	~KFT_frame_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_delay = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "delay" ));
		m_pMem_image_origin = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "image_origin" ));
		m_pMem_attr = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attr" ));
		m_pMem_attrlock = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "attrlock" ));
		m_pMem_bound_box_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bound_box_size" ));
		m_pMem_bound_box_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bound_box_array" ));
		m_pMem_library_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_number" ));
		m_pMem_flags = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "flags" ));

		m_pMem_library_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject("library_name"));
	}
	unsigned long GetValue_delay()
	{
		unsigned long res;
		m_pMem_delay->GetDWORD( &res );
		return res;
	}
	void SetValue_delay( unsigned long value )
	{
		m_pMem_delay->SetDWORD( value );
	}
	int GetArrayCount_image_origin()
	{
		return m_pMem_image_origin->GetDataCount();
	}
	void GetArrayValue_image_origin( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_image_origin->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_image_origin()
	{
		m_pMem_image_origin->RefreshArray();
	}
	void SetArrayValue_image_origin( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_image_origin->SetDataDWORD( index, data, count );
	}
	int GetArrayCount_attr()
	{
		return m_pMem_attr->GetDataCount();
	}
	void GetArrayValue_attr( int index, float *data, size_t data_size, int count )
	{
		m_pMem_attr->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_attr()
	{
		m_pMem_attr->RefreshArray();
	}
	void SetArrayValue_attr( int index, const float *data, int count = 1 )
	{
		m_pMem_attr->SetDataFloat( index, data, count );
	}
	int GetArrayCount_attrlock()
	{
		return m_pMem_attrlock->GetDataCount();
	}
	void GetArrayValue_attrlock( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_attrlock->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_attrlock()
	{
		m_pMem_attrlock->RefreshArray();
	}
	void SetArrayValue_attrlock( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_attrlock->SetDataDWORD( index, data, count );
	}
	unsigned long GetValue_bound_box_size()
	{
		unsigned long res;
		m_pMem_bound_box_size->GetDWORD( &res );
		return res;
	}
	void SetValue_bound_box_size( unsigned long value )
	{
		m_pMem_bound_box_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_bound_box_array( KFTA_bound_box &obj )
	{
		obj.SetData( m_pMem_bound_box_array );
	}
	unsigned long GetValue_library_number()
	{
		unsigned long res;
		m_pMem_library_number->GetDWORD( &res );
		return res;
	}
	void SetValue_library_number( unsigned long value )
	{
		m_pMem_library_number->SetDWORD( value );
	}
	unsigned long GetValue_flags()
	{
		unsigned long res;
		m_pMem_flags->GetDWORD( &res );
		return res;
	}
	void SetValue_flags( unsigned long value )
	{
		m_pMem_flags->SetDWORD( value );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "frame_type" ) == 0;
	}
	bool SetString_library_name( const char* data, int nLen )
	{
		return m_pMem_library_name->SetString( data, nLen );
	}
	bool GetString_library_name( char* data, int& nLen )
	{
		return m_pMem_library_name->GetString( data, nLen );
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_delay;
	KSimpleDataArrayObject*		m_pMem_image_origin;
	KSimpleDataArrayObject*		m_pMem_attr;
	KSimpleDataArrayObject*		m_pMem_attrlock;
	KSimpleDataObject*		m_pMem_bound_box_size;
	KTemplateDataArrayObject*		m_pMem_bound_box_array;
	KSimpleDataObject*		m_pMem_library_number;
	KSimpleDataObject*		m_pMem_flags;

	KSimpleDataObject*		m_pMem_library_name;
};
class KFTA_frame_type
{
public:
	KFTA_frame_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_frame_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_frame_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "frame_type" );
	}
	~KFTA_frame_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_frame_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_note_type
{
public:
	KFT_note_type()
	{
		m_pObj = NULL;
		m_pMem_frame_number = NULL;
		m_pMem_note = NULL;
	}
	KFT_note_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "note_type" );
		m_pMem_frame_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_number" ));
		m_pMem_note = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note" ));
	}
	KFT_note_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_frame_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_number" ));
		m_pMem_note = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note" ));
	}
	~KFT_note_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_frame_number = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_number" ));
		m_pMem_note = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note" ));
	}
	unsigned long GetValue_frame_number()
	{
		unsigned long res;
		m_pMem_frame_number->GetDWORD( &res );
		return res;
	}
	void SetValue_frame_number( unsigned long value )
	{
		m_pMem_frame_number->SetDWORD( value );
	}
	bool GetString_note( char *data, int &nLen )
	{
		return m_pMem_note->GetString( data, nLen );
	}
	bool SetString_note( const char *data, int nLen )
	{
		return m_pMem_note->SetString( data, nLen );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "note_type" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_frame_number;
	KSimpleDataObject*		m_pMem_note;
};
class KFTA_note_type
{
public:
	KFTA_note_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_note_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_note_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "note_type" );
	}
	~KFTA_note_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_note_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_track_type
{
public:
	KFT_track_type()
	{
		m_pObj = NULL;
		m_pMem_track_name = NULL;
		m_pMem_note_size = NULL;
		m_pMem_note_array = NULL;
	}
	KFT_track_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "track_type" );
		m_pMem_track_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_name" ));
		m_pMem_note_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note_size" ));
		m_pMem_note_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "note_array" ));
	}
	KFT_track_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_track_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_name" ));
		m_pMem_note_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note_size" ));
		m_pMem_note_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "note_array" ));
	}
	~KFT_track_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_track_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_name" ));
		m_pMem_note_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note_size" ));
		m_pMem_note_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "note_array" ));
	}
	bool GetString_track_name( char *data, int &nLen )
	{
		return m_pMem_track_name->GetString( data, nLen );
	}
	bool SetString_track_name( const char *data, int nLen )
	{
		return m_pMem_track_name->SetString( data, nLen );
	}
	unsigned long GetValue_note_size()
	{
		unsigned long res;
		m_pMem_note_size->GetDWORD( &res );
		return res;
	}
	void SetValue_note_size( unsigned long value )
	{
		m_pMem_note_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_note_array( KFTA_note_type &obj )
	{
		obj.SetData( m_pMem_note_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "track_type" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_track_name;
	KSimpleDataObject*		m_pMem_note_size;
	KTemplateDataArrayObject*		m_pMem_note_array;
};
class KFTA_track_type
{
public:
	KFTA_track_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_track_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_track_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "track_type" );
	}
	~KFTA_track_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_track_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_old_animation_type
{
public:
	KFT_old_animation_type()
	{
		m_pObj = NULL;
		m_pMem_animation_name = NULL;
		m_pMem_frame_time = NULL;
		m_pMem_frame_size = NULL;
		m_pMem_old_frame_array = NULL;
		m_pMem_track_size = NULL;
		m_pMem_track_array = NULL;
	}
	KFT_old_animation_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "old_animation_type" );
		m_pMem_animation_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_name" ));
		m_pMem_frame_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_time" ));
		m_pMem_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_size" ));
		m_pMem_old_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "old_frame_array" ));
		m_pMem_track_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_size" ));
		m_pMem_track_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "track_array" ));
	}
	KFT_old_animation_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_animation_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_name" ));
		m_pMem_frame_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_time" ));
		m_pMem_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_size" ));
		m_pMem_old_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "old_frame_array" ));
		m_pMem_track_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_size" ));
		m_pMem_track_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "track_array" ));
	}
	~KFT_old_animation_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_animation_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_name" ));
		m_pMem_frame_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_time" ));
		m_pMem_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_size" ));
		m_pMem_old_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "old_frame_array" ));
		m_pMem_track_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_size" ));
		m_pMem_track_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "track_array" ));
	}
	bool GetString_animation_name( char *data, int &nLen )
	{
		return m_pMem_animation_name->GetString( data, nLen );
	}
	bool SetString_animation_name( const char *data, int nLen )
	{
		return m_pMem_animation_name->SetString( data, nLen );
	}
	unsigned long GetValue_frame_time()
	{
		unsigned long res;
		m_pMem_frame_time->GetDWORD( &res );
		return res;
	}
	void SetValue_frame_time( unsigned long value )
	{
		m_pMem_frame_time->SetDWORD( value );
	}
	unsigned long GetValue_frame_size()
	{
		unsigned long res;
		m_pMem_frame_size->GetDWORD( &res );
		return res;
	}
	void SetValue_frame_size( unsigned long value )
	{
		m_pMem_frame_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_old_frame_array( KFTA_old_frame_type &obj )
	{
		obj.SetData( m_pMem_old_frame_array );
	}
	unsigned long GetValue_track_size()
	{
		unsigned long res;
		m_pMem_track_size->GetDWORD( &res );
		return res;
	}
	void SetValue_track_size( unsigned long value )
	{
		m_pMem_track_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_track_array( KFTA_track_type &obj )
	{
		obj.SetData( m_pMem_track_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "old_animation_type" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_animation_name;
	KSimpleDataObject*		m_pMem_frame_time;
	KSimpleDataObject*		m_pMem_frame_size;
	KTemplateDataArrayObject*		m_pMem_old_frame_array;
	KSimpleDataObject*		m_pMem_track_size;
	KTemplateDataArrayObject*		m_pMem_track_array;
};
class KFTA_old_animation_type
{
public:
	KFTA_old_animation_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_old_animation_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_old_animation_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "old_animation_type" );
	}
	~KFTA_old_animation_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_old_animation_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_animation_type
{
public:
	KFT_animation_type()
	{
		m_pObj = NULL;
		m_pMem_animation_name = NULL;
		m_pMem_frame_time = NULL;
		m_pMem_frame_size = NULL;
		m_pMem_frame_array = NULL;
		m_pMem_track_size = NULL;
		m_pMem_track_array = NULL;
	}
	KFT_animation_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "animation_type" );
		m_pMem_animation_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_name" ));
		m_pMem_frame_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_time" ));
		m_pMem_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_size" ));
		m_pMem_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "frame_array" ));
		m_pMem_track_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_size" ));
		m_pMem_track_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "track_array" ));
	}
	KFT_animation_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_animation_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_name" ));
		m_pMem_frame_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_time" ));
		m_pMem_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_size" ));
		m_pMem_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "frame_array" ));
		m_pMem_track_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_size" ));
		m_pMem_track_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "track_array" ));
	}
	~KFT_animation_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_animation_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_name" ));
		m_pMem_frame_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_time" ));
		m_pMem_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "frame_size" ));
		m_pMem_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "frame_array" ));
		m_pMem_track_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "track_size" ));
		m_pMem_track_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "track_array" ));
	}
	bool GetString_animation_name( char *data, int &nLen )
	{
		return m_pMem_animation_name->GetString( data, nLen );
	}
	bool SetString_animation_name( const char *data, int nLen )
	{
		return m_pMem_animation_name->SetString( data, nLen );
	}
	unsigned long GetValue_frame_time()
	{
		unsigned long res;
		m_pMem_frame_time->GetDWORD( &res );
		return res;
	}
	void SetValue_frame_time( unsigned long value )
	{
		m_pMem_frame_time->SetDWORD( value );
	}
	unsigned long GetValue_frame_size()
	{
		unsigned long res;
		m_pMem_frame_size->GetDWORD( &res );
		return res;
	}
	void SetValue_frame_size( unsigned long value )
	{
		m_pMem_frame_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_frame_array( KFTA_frame_type &obj )
	{
		obj.SetData( m_pMem_frame_array );
	}
	unsigned long GetValue_track_size()
	{
		unsigned long res;
		m_pMem_track_size->GetDWORD( &res );
		return res;
	}
	void SetValue_track_size( unsigned long value )
	{
		m_pMem_track_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_track_array( KFTA_track_type &obj )
	{
		obj.SetData( m_pMem_track_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "animation_type" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_animation_name;
	KSimpleDataObject*		m_pMem_frame_time;
	KSimpleDataObject*		m_pMem_frame_size;
	KTemplateDataArrayObject*		m_pMem_frame_array;
	KSimpleDataObject*		m_pMem_track_size;
	KTemplateDataArrayObject*		m_pMem_track_array;
};
class KFTA_animation_type
{
public:
	KFTA_animation_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_animation_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_animation_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "animation_type" );
	}
	~KFTA_animation_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_animation_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_library_type
{
public:
	KFT_library_type()
	{
		m_pObj = NULL;
		m_pMem_library_name = NULL;
		m_pMem_library_path = NULL;
		m_pMem_library_rect = NULL;
	}
	KFT_library_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "library_type" );
		m_pMem_library_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_name" ));
		m_pMem_library_path = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_path" ));
		m_pMem_library_rect = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "library_rect" ));
	}
	KFT_library_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_library_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_name" ));
		m_pMem_library_path = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_path" ));
		m_pMem_library_rect = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "library_rect" ));
	}
	~KFT_library_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_library_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_name" ));
		m_pMem_library_path = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_path" ));
		m_pMem_library_rect = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "library_rect" ));
	}
	bool GetString_library_name( char *data, int &nLen )
	{
		return m_pMem_library_name->GetString( data, nLen );
	}
	bool SetString_library_name( const char *data, int nLen )
	{
		return m_pMem_library_name->SetString( data, nLen );
	}
	bool GetString_library_path( char *data, int &nLen )
	{
		return m_pMem_library_path->GetString( data, nLen );
	}
	bool SetString_library_path( const char *data, int nLen )
	{
		return m_pMem_library_path->SetString( data, nLen );
	}
	int GetArrayCount_library_rect()
	{
		return m_pMem_library_rect->GetDataCount();
	}
	void GetArrayValue_library_rect( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_library_rect->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_library_rect()
	{
		m_pMem_library_rect->RefreshArray();
	}
	void SetArrayValue_library_rect( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_library_rect->SetDataDWORD( index, data, count );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "library_type" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_library_name;
	KSimpleDataObject*		m_pMem_library_path;
	KSimpleDataArrayObject*		m_pMem_library_rect;
};
class KFTA_library_type
{
public:
	KFTA_library_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_library_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_library_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "library_type" );
	}
	~KFTA_library_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_library_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_old_sprite_type
{
public:
	KFT_old_sprite_type()
	{
		m_pObj = NULL;
		m_pMem_animation_size = NULL;
		m_pMem_old_animation_array = NULL;
		m_pMem_library_size = NULL;
		m_pMem_library_array = NULL;
	}
	KFT_old_sprite_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "old_sprite_type" );
		m_pMem_animation_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_size" ));
		m_pMem_old_animation_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "old_animation_array" ));
		m_pMem_library_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_size" ));
		m_pMem_library_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "library_array" ));
	}
	KFT_old_sprite_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_animation_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_size" ));
		m_pMem_old_animation_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "old_animation_array" ));
		m_pMem_library_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_size" ));
		m_pMem_library_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "library_array" ));
	}
	~KFT_old_sprite_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_animation_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_size" ));
		m_pMem_old_animation_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "old_animation_array" ));
		m_pMem_library_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_size" ));
		m_pMem_library_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "library_array" ));
	}
	unsigned long GetValue_animation_size()
	{
		unsigned long res;
		m_pMem_animation_size->GetDWORD( &res );
		return res;
	}
	void SetValue_animation_size( unsigned long value )
	{
		m_pMem_animation_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_old_animation_array( KFTA_old_animation_type &obj )
	{
		obj.SetData( m_pMem_old_animation_array );
	}
	unsigned long GetValue_library_size()
	{
		unsigned long res;
		m_pMem_library_size->GetDWORD( &res );
		return res;
	}
	void SetValue_library_size( unsigned long value )
	{
		m_pMem_library_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_library_array( KFTA_library_type &obj )
	{
		obj.SetData( m_pMem_library_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "old_sprite_type" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_animation_size;
	KTemplateDataArrayObject*		m_pMem_old_animation_array;
	KSimpleDataObject*		m_pMem_library_size;
	KTemplateDataArrayObject*		m_pMem_library_array;
};
class KFTA_old_sprite_type
{
public:
	KFTA_old_sprite_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_old_sprite_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_old_sprite_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "old_sprite_type" );
	}
	~KFTA_old_sprite_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_old_sprite_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_sprite_type
{
public:
	KFT_sprite_type()
	{
		m_pObj = NULL;
		m_pMem_animation_size = NULL;
		m_pMem_animation_array = NULL;
		m_pMem_library_size = NULL;
		m_pMem_library_array = NULL;
	}
	KFT_sprite_type( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "sprite_type" );
		m_pMem_animation_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_size" ));
		m_pMem_animation_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "animation_array" ));
		m_pMem_library_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_size" ));
		m_pMem_library_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "library_array" ));
	}
	KFT_sprite_type( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_animation_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_size" ));
		m_pMem_animation_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "animation_array" ));
		m_pMem_library_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_size" ));
		m_pMem_library_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "library_array" ));
	}
	~KFT_sprite_type()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_animation_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "animation_size" ));
		m_pMem_animation_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "animation_array" ));
		m_pMem_library_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "library_size" ));
		m_pMem_library_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "library_array" ));
	}
	unsigned long GetValue_animation_size()
	{
		unsigned long res;
		m_pMem_animation_size->GetDWORD( &res );
		return res;
	}
	void SetValue_animation_size( unsigned long value )
	{
		m_pMem_animation_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_animation_array( KFTA_animation_type &obj )
	{
		obj.SetData( m_pMem_animation_array );
	}
	unsigned long GetValue_library_size()
	{
		unsigned long res;
		m_pMem_library_size->GetDWORD( &res );
		return res;
	}
	void SetValue_library_size( unsigned long value )
	{
		m_pMem_library_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_library_array( KFTA_library_type &obj )
	{
		obj.SetData( m_pMem_library_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "sprite_type" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_animation_size;
	KTemplateDataArrayObject*		m_pMem_animation_array;
	KSimpleDataObject*		m_pMem_library_size;
	KTemplateDataArrayObject*		m_pMem_library_array;
};
class KFTA_sprite_type
{
public:
	KFTA_sprite_type()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_sprite_type( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_sprite_type( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "sprite_type" );
	}
	~KFTA_sprite_type()
	{
	}
	void SetData( KTemplateDataArrayObject *pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	void SetArrayInfo( int nCount, const char *varCount, KTemplateDataObject *pParent )
	{
		m_pObj->SetArrayInfo( nCount, varCount, pParent );
	}
	void RefreshArray()
	{
		m_pObj->RefreshArray();
	}
	int GetDataCount()
	{
		return m_pObj->GetDataCount();
	}
	void GetData( int index, KFT_sprite_type &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
#endif