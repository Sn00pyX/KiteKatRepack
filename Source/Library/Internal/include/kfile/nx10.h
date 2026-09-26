#ifndef _NX10_H_
#define _NX10_H_
#include "KFiler.h"

class KFT_nx3_mtl_block
{
public:
	KFT_nx3_mtl_block()
	{
		m_pObj = NULL;
		m_pMem_mtl_name = NULL;
		m_pMem_texture_name = NULL;
		m_pMem_mtl_id = NULL;
		m_pMem_channel_id = NULL;
		m_pMem_power = NULL;
		m_pMem_self_illumi = NULL;
		m_pMem_smoothing = NULL;
		m_pMem_ambient = NULL;
		m_pMem_diffuse = NULL;
		m_pMem_specular = NULL;
	}
	KFT_nx3_mtl_block( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_mtl_block" );
		m_pMem_mtl_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_name" ));
		m_pMem_texture_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texture_name" ));
		m_pMem_mtl_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_id" ));
		m_pMem_channel_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_id" ));
		m_pMem_power = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "power" ));
		m_pMem_self_illumi = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "self_illumi" ));
		m_pMem_smoothing = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "smoothing" ));
		m_pMem_ambient = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ambient" ));
		m_pMem_diffuse = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "diffuse" ));
		m_pMem_specular = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "specular" ));
	}
	KFT_nx3_mtl_block( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mtl_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_name" ));
		m_pMem_texture_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texture_name" ));
		m_pMem_mtl_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_id" ));
		m_pMem_channel_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_id" ));
		m_pMem_power = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "power" ));
		m_pMem_self_illumi = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "self_illumi" ));
		m_pMem_smoothing = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "smoothing" ));
		m_pMem_ambient = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ambient" ));
		m_pMem_diffuse = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "diffuse" ));
		m_pMem_specular = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "specular" ));
	}
	~KFT_nx3_mtl_block()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mtl_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_name" ));
		m_pMem_texture_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texture_name" ));
		m_pMem_mtl_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_id" ));
		m_pMem_channel_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_id" ));
		m_pMem_power = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "power" ));
		m_pMem_self_illumi = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "self_illumi" ));
		m_pMem_smoothing = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "smoothing" ));
		m_pMem_ambient = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ambient" ));
		m_pMem_diffuse = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "diffuse" ));
		m_pMem_specular = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "specular" ));
	}
	bool GetString_mtl_name( char *data, int &nLen )
	{
		return m_pMem_mtl_name->GetString( data, nLen );
	}
	bool SetString_mtl_name( const char *data, int nLen )
	{
		return m_pMem_mtl_name->SetString( data, nLen );
	}
	bool GetString_texture_name( char *data, int &nLen )
	{
		return m_pMem_texture_name->GetString( data, nLen );
	}
	bool SetString_texture_name( const char *data, int nLen )
	{
		return m_pMem_texture_name->SetString( data, nLen );
	}
	unsigned long GetValue_mtl_id()
	{
		unsigned long res;
		m_pMem_mtl_id->GetDWORD( &res );
		return res;
	}
	void SetValue_mtl_id( unsigned long value )
	{
		m_pMem_mtl_id->SetDWORD( value );
	}
	unsigned long GetValue_channel_id()
	{
		unsigned long res;
		m_pMem_channel_id->GetDWORD( &res );
		return res;
	}
	void SetValue_channel_id( unsigned long value )
	{
		m_pMem_channel_id->SetDWORD( value );
	}
	float GetValue_power()
	{
		float res;
		m_pMem_power->GetFloat( &res );
		return res;
	}
	void SetValue_power( float value )
	{
		m_pMem_power->SetFloat( value );
	}
	float GetValue_self_illumi()
	{
		float res;
		m_pMem_self_illumi->GetFloat( &res );
		return res;
	}
	void SetValue_self_illumi( float value )
	{
		m_pMem_self_illumi->SetFloat( value );
	}
	char GetValue_smoothing()
	{
		char res;
		m_pMem_smoothing->GetChar( &res );
		return res;
	}
	void SetValue_smoothing( char value )
	{
		m_pMem_smoothing->SetChar( value );
	}
	unsigned long GetValue_ambient()
	{
		unsigned long res;
		m_pMem_ambient->GetDWORD( &res );
		return res;
	}
	void SetValue_ambient( unsigned long value )
	{
		m_pMem_ambient->SetDWORD( value );
	}
	unsigned long GetValue_diffuse()
	{
		unsigned long res;
		m_pMem_diffuse->GetDWORD( &res );
		return res;
	}
	void SetValue_diffuse( unsigned long value )
	{
		m_pMem_diffuse->SetDWORD( value );
	}
	unsigned long GetValue_specular()
	{
		unsigned long res;
		m_pMem_specular->GetDWORD( &res );
		return res;
	}
	void SetValue_specular( unsigned long value )
	{
		m_pMem_specular->SetDWORD( value );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_mtl_block" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_mtl_name;
	KSimpleDataObject*		m_pMem_texture_name;
	KSimpleDataObject*		m_pMem_mtl_id;
	KSimpleDataObject*		m_pMem_channel_id;
	KSimpleDataObject*		m_pMem_power;
	KSimpleDataObject*		m_pMem_self_illumi;
	KSimpleDataObject*		m_pMem_smoothing;
	KSimpleDataObject*		m_pMem_ambient;
	KSimpleDataObject*		m_pMem_diffuse;
	KSimpleDataObject*		m_pMem_specular;
};
class KFTA_nx3_mtl_block
{
public:
	KFTA_nx3_mtl_block()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_mtl_block( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_mtl_block( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_mtl_block" );
	}
	~KFTA_nx3_mtl_block()
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
	void GetData( int index, KFT_nx3_mtl_block &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_mtl
{
public:
	KFT_nx3_mtl()
	{
		m_pObj = NULL;
		m_pMem_sub_mtl_block_size = NULL;
		m_pMem_sub_mtl_block_array = NULL;
	}
	KFT_nx3_mtl( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_mtl" );
		m_pMem_sub_mtl_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "sub_mtl_block_size" ));
		m_pMem_sub_mtl_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "sub_mtl_block_array" ));
	}
	KFT_nx3_mtl( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_sub_mtl_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "sub_mtl_block_size" ));
		m_pMem_sub_mtl_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "sub_mtl_block_array" ));
	}
	~KFT_nx3_mtl()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_sub_mtl_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "sub_mtl_block_size" ));
		m_pMem_sub_mtl_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "sub_mtl_block_array" ));
	}
	unsigned long GetValue_sub_mtl_block_size()
	{
		unsigned long res;
		m_pMem_sub_mtl_block_size->GetDWORD( &res );
		return res;
	}
	void SetValue_sub_mtl_block_size( unsigned long value )
	{
		m_pMem_sub_mtl_block_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_sub_mtl_block_array( KFTA_nx3_mtl_block &obj )
	{
		obj.SetData( m_pMem_sub_mtl_block_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_mtl" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_sub_mtl_block_size;
	KTemplateDataArrayObject*		m_pMem_sub_mtl_block_array;
};
class KFTA_nx3_mtl
{
public:
	KFTA_nx3_mtl()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_mtl( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_mtl( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_mtl" );
	}
	~KFTA_nx3_mtl()
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
	void GetData( int index, KFT_nx3_mtl &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_mtl_header
{
public:
	KFT_nx3_mtl_header()
	{
		m_pObj = NULL;
		m_pMem_mtl_size = NULL;
		m_pMem_mtl_array = NULL;
	}
	KFT_nx3_mtl_header( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_mtl_header" );
		m_pMem_mtl_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_size" ));
		m_pMem_mtl_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mtl_array" ));
	}
	KFT_nx3_mtl_header( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mtl_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_size" ));
		m_pMem_mtl_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mtl_array" ));
	}
	~KFT_nx3_mtl_header()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mtl_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mtl_size" ));
		m_pMem_mtl_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mtl_array" ));
	}
	unsigned long GetValue_mtl_size()
	{
		unsigned long res;
		m_pMem_mtl_size->GetDWORD( &res );
		return res;
	}
	void SetValue_mtl_size( unsigned long value )
	{
		m_pMem_mtl_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_mtl_array( KFTA_nx3_mtl &obj )
	{
		obj.SetData( m_pMem_mtl_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_mtl_header" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_mtl_size;
	KTemplateDataArrayObject*		m_pMem_mtl_array;
};
class KFTA_nx3_mtl_header
{
public:
	KFTA_nx3_mtl_header()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_mtl_header( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_mtl_header( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_mtl_header" );
	}
	~KFTA_nx3_mtl_header()
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
	void GetData( int index, KFT_nx3_mtl_header &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_old_nx3_mesh_frame
{
public:
	KFT_old_nx3_mesh_frame()
	{
		m_pObj = NULL;
		m_pMem_time_value = NULL;
		m_pMem_vertex_size = NULL;
		m_pMem_vertex_array = NULL;
		m_pMem_normal_size = NULL;
		m_pMem_normal_array = NULL;
		m_pMem_texel_size = NULL;
		m_pMem_texel_array = NULL;
		m_pMem_color_size = NULL;
		m_pMem_color_array = NULL;
	}
	KFT_old_nx3_mesh_frame( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "old_nx3_mesh_frame" );
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_vertex_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "vertex_size" ));
		m_pMem_vertex_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "vertex_array" ));
		m_pMem_normal_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "normal_size" ));
		m_pMem_normal_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "normal_array" ));
		m_pMem_texel_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texel_size" ));
		m_pMem_texel_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "texel_array" ));
		m_pMem_color_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "color_size" ));
		m_pMem_color_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "color_array" ));
	}
	KFT_old_nx3_mesh_frame( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_vertex_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "vertex_size" ));
		m_pMem_vertex_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "vertex_array" ));
		m_pMem_normal_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "normal_size" ));
		m_pMem_normal_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "normal_array" ));
		m_pMem_texel_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texel_size" ));
		m_pMem_texel_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "texel_array" ));
		m_pMem_color_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "color_size" ));
		m_pMem_color_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "color_array" ));
	}
	~KFT_old_nx3_mesh_frame()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_vertex_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "vertex_size" ));
		m_pMem_vertex_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "vertex_array" ));
		m_pMem_normal_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "normal_size" ));
		m_pMem_normal_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "normal_array" ));
		m_pMem_texel_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texel_size" ));
		m_pMem_texel_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "texel_array" ));
		m_pMem_color_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "color_size" ));
		m_pMem_color_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "color_array" ));
	}
	unsigned long GetValue_time_value()
	{
		unsigned long res;
		m_pMem_time_value->GetDWORD( &res );
		return res;
	}
	void SetValue_time_value( unsigned long value )
	{
		m_pMem_time_value->SetDWORD( value );
	}
	unsigned long GetValue_vertex_size()
	{
		unsigned long res;
		m_pMem_vertex_size->GetDWORD( &res );
		return res;
	}
	void SetValue_vertex_size( unsigned long value )
	{
		m_pMem_vertex_size->SetDWORD( value );
	}
	int GetArrayCount_vertex_array()
	{
		return m_pMem_vertex_array->GetDataCount();
	}
	void GetArrayValue_vertex_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_vertex_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_vertex_array()
	{
		m_pMem_vertex_array->RefreshArray();
	}
	void SetArrayValue_vertex_array( int index, const float *data, int count = 1 )
	{
		m_pMem_vertex_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_normal_size()
	{
		unsigned long res;
		m_pMem_normal_size->GetDWORD( &res );
		return res;
	}
	void SetValue_normal_size( unsigned long value )
	{
		m_pMem_normal_size->SetDWORD( value );
	}
	int GetArrayCount_normal_array()
	{
		return m_pMem_normal_array->GetDataCount();
	}
	void GetArrayValue_normal_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_normal_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_normal_array()
	{
		m_pMem_normal_array->RefreshArray();
	}
	void SetArrayValue_normal_array( int index, const float *data, int count = 1 )
	{
		m_pMem_normal_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_texel_size()
	{
		unsigned long res;
		m_pMem_texel_size->GetDWORD( &res );
		return res;
	}
	void SetValue_texel_size( unsigned long value )
	{
		m_pMem_texel_size->SetDWORD( value );
	}
	int GetArrayCount_texel_array()
	{
		return m_pMem_texel_array->GetDataCount();
	}
	void GetArrayValue_texel_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_texel_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_texel_array()
	{
		m_pMem_texel_array->RefreshArray();
	}
	void SetArrayValue_texel_array( int index, const float *data, int count = 1 )
	{
		m_pMem_texel_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_color_size()
	{
		unsigned long res;
		m_pMem_color_size->GetDWORD( &res );
		return res;
	}
	void SetValue_color_size( unsigned long value )
	{
		m_pMem_color_size->SetDWORD( value );
	}
	int GetArrayCount_color_array()
	{
		return m_pMem_color_array->GetDataCount();
	}
	void GetArrayValue_color_array( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_color_array->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_color_array()
	{
		m_pMem_color_array->RefreshArray();
	}
	void SetArrayValue_color_array( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_color_array->SetDataDWORD( index, data, count );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "old_nx3_mesh_frame" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_time_value;
	KSimpleDataObject*		m_pMem_vertex_size;
	KSimpleDataArrayObject*		m_pMem_vertex_array;
	KSimpleDataObject*		m_pMem_normal_size;
	KSimpleDataArrayObject*		m_pMem_normal_array;
	KSimpleDataObject*		m_pMem_texel_size;
	KSimpleDataArrayObject*		m_pMem_texel_array;
	KSimpleDataObject*		m_pMem_color_size;
	KSimpleDataArrayObject*		m_pMem_color_array;
};
class KFTA_old_nx3_mesh_frame
{
public:
	KFTA_old_nx3_mesh_frame()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_old_nx3_mesh_frame( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_old_nx3_mesh_frame( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "old_nx3_mesh_frame" );
	}
	~KFTA_old_nx3_mesh_frame()
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
	void GetData( int index, KFT_old_nx3_mesh_frame &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_weight_frame
{
public:
	KFT_nx3_weight_frame()
	{
		m_pObj = NULL;
		m_pMem_bone_name = NULL;
		m_pMem_weight_size = NULL;
		m_pMem_weight_array = NULL;
		m_pMem_offset_vector_size = NULL;
		m_pMem_offset_vector_array = NULL;
	}
	KFT_nx3_weight_frame( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_weight_frame" );
		m_pMem_bone_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_name" ));
		m_pMem_weight_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "weight_size" ));
		m_pMem_weight_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "weight_array" ));
		m_pMem_offset_vector_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "offset_vector_size" ));
		m_pMem_offset_vector_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "offset_vector_array" ));
	}
	KFT_nx3_weight_frame( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_bone_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_name" ));
		m_pMem_weight_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "weight_size" ));
		m_pMem_weight_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "weight_array" ));
		m_pMem_offset_vector_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "offset_vector_size" ));
		m_pMem_offset_vector_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "offset_vector_array" ));
	}
	~KFT_nx3_weight_frame()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_bone_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_name" ));
		m_pMem_weight_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "weight_size" ));
		m_pMem_weight_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "weight_array" ));
		m_pMem_offset_vector_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "offset_vector_size" ));
		m_pMem_offset_vector_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "offset_vector_array" ));
	}
	bool GetString_bone_name( char *data, int &nLen )
	{
		return m_pMem_bone_name->GetString( data, nLen );
	}
	bool SetString_bone_name( const char *data, int nLen )
	{
		return m_pMem_bone_name->SetString( data, nLen );
	}
	unsigned long GetValue_weight_size()
	{
		unsigned long res;
		m_pMem_weight_size->GetDWORD( &res );
		return res;
	}
	void SetValue_weight_size( unsigned long value )
	{
		m_pMem_weight_size->SetDWORD( value );
	}
	int GetArrayCount_weight_array()
	{
		return m_pMem_weight_array->GetDataCount();
	}
	void GetArrayValue_weight_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_weight_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_weight_array()
	{
		m_pMem_weight_array->RefreshArray();
	}
	void SetArrayValue_weight_array( int index, const float *data, int count = 1 )
	{
		m_pMem_weight_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_offset_vector_size()
	{
		unsigned long res;
		m_pMem_offset_vector_size->GetDWORD( &res );
		return res;
	}
	void SetValue_offset_vector_size( unsigned long value )
	{
		m_pMem_offset_vector_size->SetDWORD( value );
	}
	int GetArrayCount_offset_vector_array()
	{
		return m_pMem_offset_vector_array->GetDataCount();
	}
	void GetArrayValue_offset_vector_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_offset_vector_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_offset_vector_array()
	{
		m_pMem_offset_vector_array->RefreshArray();
	}
	void SetArrayValue_offset_vector_array( int index, const float *data, int count = 1 )
	{
		m_pMem_offset_vector_array->SetDataFloat( index, data, count );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_weight_frame" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_bone_name;
	KSimpleDataObject*		m_pMem_weight_size;
	KSimpleDataArrayObject*		m_pMem_weight_array;
	KSimpleDataObject*		m_pMem_offset_vector_size;
	KSimpleDataArrayObject*		m_pMem_offset_vector_array;
};
class KFTA_nx3_weight_frame
{
public:
	KFTA_nx3_weight_frame()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_weight_frame( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_weight_frame( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_weight_frame" );
	}
	~KFTA_nx3_weight_frame()
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
	void GetData( int index, KFT_nx3_weight_frame &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_mesh_frame
{
public:
	KFT_nx3_mesh_frame()
	{
		m_pObj = NULL;
		m_pMem_time_value = NULL;
		m_pMem_vertex_size = NULL;
		m_pMem_vertex_array = NULL;
		m_pMem_normal_size = NULL;
		m_pMem_normal_array = NULL;
		m_pMem_texel_size = NULL;
		m_pMem_texel_array = NULL;
		m_pMem_color_size = NULL;
		m_pMem_color_array = NULL;
		m_pMem_bone_size = NULL;
		m_pMem_bone_block = NULL;
		m_pMem_mesh_tm = NULL;
	}
	KFT_nx3_mesh_frame( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_mesh_frame" );
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_vertex_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "vertex_size" ));
		m_pMem_vertex_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "vertex_array" ));
		m_pMem_normal_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "normal_size" ));
		m_pMem_normal_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "normal_array" ));
		m_pMem_texel_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texel_size" ));
		m_pMem_texel_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "texel_array" ));
		m_pMem_color_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "color_size" ));
		m_pMem_color_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "color_array" ));
		m_pMem_bone_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_size" ));
		m_pMem_bone_block = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bone_block" ));
		m_pMem_mesh_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mesh_tm" ));
	}
	KFT_nx3_mesh_frame( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_vertex_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "vertex_size" ));
		m_pMem_vertex_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "vertex_array" ));
		m_pMem_normal_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "normal_size" ));
		m_pMem_normal_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "normal_array" ));
		m_pMem_texel_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texel_size" ));
		m_pMem_texel_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "texel_array" ));
		m_pMem_color_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "color_size" ));
		m_pMem_color_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "color_array" ));
		m_pMem_bone_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_size" ));
		m_pMem_bone_block = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bone_block" ));
		m_pMem_mesh_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mesh_tm" ));
	}
	~KFT_nx3_mesh_frame()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_vertex_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "vertex_size" ));
		m_pMem_vertex_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "vertex_array" ));
		m_pMem_normal_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "normal_size" ));
		m_pMem_normal_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "normal_array" ));
		m_pMem_texel_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texel_size" ));
		m_pMem_texel_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "texel_array" ));
		m_pMem_color_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "color_size" ));
		m_pMem_color_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "color_array" ));
		m_pMem_bone_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_size" ));
		m_pMem_bone_block = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bone_block" ));
		m_pMem_mesh_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mesh_tm" ));
	}
	unsigned long GetValue_time_value()
	{
		unsigned long res;
		m_pMem_time_value->GetDWORD( &res );
		return res;
	}
	void SetValue_time_value( unsigned long value )
	{
		m_pMem_time_value->SetDWORD( value );
	}
	unsigned long GetValue_vertex_size()
	{
		unsigned long res;
		m_pMem_vertex_size->GetDWORD( &res );
		return res;
	}
	void SetValue_vertex_size( unsigned long value )
	{
		m_pMem_vertex_size->SetDWORD( value );
	}
	int GetArrayCount_vertex_array()
	{
		return m_pMem_vertex_array->GetDataCount();
	}
	void GetArrayValue_vertex_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_vertex_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_vertex_array()
	{
		m_pMem_vertex_array->RefreshArray();
	}
	void SetArrayValue_vertex_array( int index, const float *data, int count = 1 )
	{
		m_pMem_vertex_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_normal_size()
	{
		unsigned long res;
		m_pMem_normal_size->GetDWORD( &res );
		return res;
	}
	void SetValue_normal_size( unsigned long value )
	{
		m_pMem_normal_size->SetDWORD( value );
	}
	int GetArrayCount_normal_array()
	{
		return m_pMem_normal_array->GetDataCount();
	}
	void GetArrayValue_normal_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_normal_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_normal_array()
	{
		m_pMem_normal_array->RefreshArray();
	}
	void SetArrayValue_normal_array( int index, const float *data, int count = 1 )
	{
		m_pMem_normal_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_texel_size()
	{
		unsigned long res;
		m_pMem_texel_size->GetDWORD( &res );
		return res;
	}
	void SetValue_texel_size( unsigned long value )
	{
		m_pMem_texel_size->SetDWORD( value );
	}
	int GetArrayCount_texel_array()
	{
		return m_pMem_texel_array->GetDataCount();
	}
	void GetArrayValue_texel_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_texel_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_texel_array()
	{
		m_pMem_texel_array->RefreshArray();
	}
	void SetArrayValue_texel_array( int index, const float *data, int count = 1 )
	{
		m_pMem_texel_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_color_size()
	{
		unsigned long res;
		m_pMem_color_size->GetDWORD( &res );
		return res;
	}
	void SetValue_color_size( unsigned long value )
	{
		m_pMem_color_size->SetDWORD( value );
	}
	int GetArrayCount_color_array()
	{
		return m_pMem_color_array->GetDataCount();
	}
	void GetArrayValue_color_array( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_color_array->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_color_array()
	{
		m_pMem_color_array->RefreshArray();
	}
	void SetArrayValue_color_array( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_color_array->SetDataDWORD( index, data, count );
	}
	unsigned long GetValue_bone_size()
	{
		unsigned long res;
		m_pMem_bone_size->GetDWORD( &res );
		return res;
	}
	void SetValue_bone_size( unsigned long value )
	{
		m_pMem_bone_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_bone_block( KFTA_nx3_weight_frame &obj )
	{
		obj.SetData( m_pMem_bone_block );
	}
	int GetArrayCount_mesh_tm()
	{
		return m_pMem_mesh_tm->GetDataCount();
	}
	void GetArrayValue_mesh_tm( int index, float *data, size_t data_size, int count )
	{
		m_pMem_mesh_tm->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_mesh_tm()
	{
		m_pMem_mesh_tm->RefreshArray();
	}
	void SetArrayValue_mesh_tm( int index, const float *data, int count = 1 )
	{
		m_pMem_mesh_tm->SetDataFloat( index, data, count );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_mesh_frame" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_time_value;
	KSimpleDataObject*		m_pMem_vertex_size;
	KSimpleDataArrayObject*		m_pMem_vertex_array;
	KSimpleDataObject*		m_pMem_normal_size;
	KSimpleDataArrayObject*		m_pMem_normal_array;
	KSimpleDataObject*		m_pMem_texel_size;
	KSimpleDataArrayObject*		m_pMem_texel_array;
	KSimpleDataObject*		m_pMem_color_size;
	KSimpleDataArrayObject*		m_pMem_color_array;
	KSimpleDataObject*		m_pMem_bone_size;
	KTemplateDataArrayObject*		m_pMem_bone_block;
	KSimpleDataArrayObject*		m_pMem_mesh_tm;
};
class KFTA_nx3_mesh_frame
{
public:
	KFTA_nx3_mesh_frame()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_mesh_frame( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_mesh_frame( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_mesh_frame" );
	}
	~KFTA_nx3_mesh_frame()
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
	void GetData( int index, KFT_nx3_mesh_frame &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_mesh_block
{
public:
	KFT_nx3_mesh_block()
	{
		m_pObj = NULL;
		m_pMem_texture_index = NULL;
		m_pMem_mesh_frame_size = NULL;
		m_pMem_mesh_frame_array = NULL;
		m_pMem_index_buffer_size = NULL;
		m_pMem_index_buffer_array = NULL;
	}
	KFT_nx3_mesh_block( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_mesh_block" );
		m_pMem_texture_index = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texture_index" ));
		m_pMem_mesh_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_frame_size" ));
		m_pMem_mesh_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_frame_array" ));
		m_pMem_index_buffer_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "index_buffer_size" ));
		m_pMem_index_buffer_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "index_buffer_array" ));
	}
	KFT_nx3_mesh_block( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_texture_index = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texture_index" ));
		m_pMem_mesh_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_frame_size" ));
		m_pMem_mesh_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_frame_array" ));
		m_pMem_index_buffer_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "index_buffer_size" ));
		m_pMem_index_buffer_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "index_buffer_array" ));
	}
	~KFT_nx3_mesh_block()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_texture_index = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "texture_index" ));
		m_pMem_mesh_frame_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_frame_size" ));
		m_pMem_mesh_frame_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_frame_array" ));
		m_pMem_index_buffer_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "index_buffer_size" ));
		m_pMem_index_buffer_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "index_buffer_array" ));
	}
	unsigned long GetValue_texture_index()
	{
		unsigned long res;
		m_pMem_texture_index->GetDWORD( &res );
		return res;
	}
	void SetValue_texture_index( unsigned long value )
	{
		m_pMem_texture_index->SetDWORD( value );
	}
	unsigned long GetValue_mesh_frame_size()
	{
		unsigned long res;
		m_pMem_mesh_frame_size->GetDWORD( &res );
		return res;
	}
	void SetValue_mesh_frame_size( unsigned long value )
	{
		m_pMem_mesh_frame_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_mesh_frame_array( KFTA_nx3_mesh_frame &obj )
	{
		obj.SetData( m_pMem_mesh_frame_array );
	}
	unsigned long GetValue_index_buffer_size()
	{
		unsigned long res;
		m_pMem_index_buffer_size->GetDWORD( &res );
		return res;
	}
	void SetValue_index_buffer_size( unsigned long value )
	{
		m_pMem_index_buffer_size->SetDWORD( value );
	}
	int GetArrayCount_index_buffer_array()
	{
		return m_pMem_index_buffer_array->GetDataCount();
	}
	void GetArrayValue_index_buffer_array( int index, unsigned short *data, size_t data_size, int count )
	{
		m_pMem_index_buffer_array->GetDataWORD( index, data, data_size, count );
	}
	void RefreshArray_index_buffer_array()
	{
		m_pMem_index_buffer_array->RefreshArray();
	}
	void SetArrayValue_index_buffer_array( int index, const unsigned short *data, int count = 1 )
	{
		m_pMem_index_buffer_array->SetDataWORD( index, data, count );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_mesh_block" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_texture_index;
	KSimpleDataObject*		m_pMem_mesh_frame_size;
	KTemplateDataArrayObject*		m_pMem_mesh_frame_array;
	KSimpleDataObject*		m_pMem_index_buffer_size;
	KSimpleDataArrayObject*		m_pMem_index_buffer_array;
};
class KFTA_nx3_mesh_block
{
public:
	KFTA_nx3_mesh_block()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_mesh_block( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_mesh_block( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_mesh_block" );
	}
	~KFTA_nx3_mesh_block()
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
	void GetData( int index, KFT_nx3_mesh_block &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_ani_block
{
public:
	KFT_nx3_ani_block()
	{
		m_pObj = NULL;
		m_pMem_time = NULL;
		m_pMem_matrix = NULL;
	}
	KFT_nx3_ani_block( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_ani_block" );
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_matrix = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "matrix" ));
	}
	KFT_nx3_ani_block( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_matrix = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "matrix" ));
	}
	~KFT_nx3_ani_block()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_matrix = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "matrix" ));
	}
	unsigned long GetValue_time()
	{
		unsigned long res;
		m_pMem_time->GetDWORD( &res );
		return res;
	}
	void SetValue_time( unsigned long value )
	{
		m_pMem_time->SetDWORD( value );
	}
	int GetArrayCount_matrix()
	{
		return m_pMem_matrix->GetDataCount();
	}
	void GetArrayValue_matrix( int index, float *data, size_t data_size, int count )
	{
		m_pMem_matrix->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_matrix()
	{
		m_pMem_matrix->RefreshArray();
	}
	void SetArrayValue_matrix( int index, const float *data, int count = 1 )
	{
		m_pMem_matrix->SetDataFloat( index, data, count );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_ani_block" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_time;
	KSimpleDataArrayObject*		m_pMem_matrix;
};
class KFTA_nx3_ani_block
{
public:
	KFTA_nx3_ani_block()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_ani_block( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_ani_block( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_ani_block" );
	}
	~KFTA_nx3_ani_block()
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
	void GetData( int index, KFT_nx3_ani_block &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_visi_block
{
public:
	KFT_nx3_visi_block()
	{
		m_pObj = NULL;
		m_pMem_time = NULL;
		m_pMem_visibility = NULL;
	}
	KFT_nx3_visi_block( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_visi_block" );
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_visibility = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "visibility" ));
	}
	KFT_nx3_visi_block( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_visibility = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "visibility" ));
	}
	~KFT_nx3_visi_block()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_visibility = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "visibility" ));
	}
	unsigned long GetValue_time()
	{
		unsigned long res;
		m_pMem_time->GetDWORD( &res );
		return res;
	}
	void SetValue_time( unsigned long value )
	{
		m_pMem_time->SetDWORD( value );
	}
	float GetValue_visibility()
	{
		float res;
		m_pMem_visibility->GetFloat( &res );
		return res;
	}
	void SetValue_visibility( float value )
	{
		m_pMem_visibility->SetFloat( value );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_visi_block" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_time;
	KSimpleDataObject*		m_pMem_visibility;
};
class KFTA_nx3_visi_block
{
public:
	KFTA_nx3_visi_block()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_visi_block( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_visi_block( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_visi_block" );
	}
	~KFTA_nx3_visi_block()
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
	void GetData( int index, KFT_nx3_visi_block &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_fx_block
{
public:
	KFT_nx3_fx_block()
	{
		m_pObj = NULL;
		m_pMem_time = NULL;
		m_pMem_note = NULL;
	}
	KFT_nx3_fx_block( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_fx_block" );
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_note = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note" ));
	}
	KFT_nx3_fx_block( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_note = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note" ));
	}
	~KFT_nx3_fx_block()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_time = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time" ));
		m_pMem_note = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "note" ));
	}
	unsigned long GetValue_time()
	{
		unsigned long res;
		m_pMem_time->GetDWORD( &res );
		return res;
	}
	void SetValue_time( unsigned long value )
	{
		m_pMem_time->SetDWORD( value );
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
		return strcmp( pObj->GetTemplateName(), "nx3_fx_block" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_time;
	KSimpleDataObject*		m_pMem_note;
};
class KFTA_nx3_fx_block
{
public:
	KFTA_nx3_fx_block()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_fx_block( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_fx_block( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_fx_block" );
	}
	~KFTA_nx3_fx_block()
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
	void GetData( int index, KFT_nx3_fx_block &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_fx
{
public:
	KFT_nx3_fx()
	{
		m_pObj = NULL;
		m_pMem_fx_block_size = NULL;
		m_pMem_fx_block_array = NULL;
	}
	KFT_nx3_fx( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_fx" );
		m_pMem_fx_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fx_block_size" ));
		m_pMem_fx_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "fx_block_array" ));
	}
	KFT_nx3_fx( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_fx_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fx_block_size" ));
		m_pMem_fx_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "fx_block_array" ));
	}
	~KFT_nx3_fx()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_fx_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fx_block_size" ));
		m_pMem_fx_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "fx_block_array" ));
	}
	unsigned long GetValue_fx_block_size()
	{
		unsigned long res;
		m_pMem_fx_block_size->GetDWORD( &res );
		return res;
	}
	void SetValue_fx_block_size( unsigned long value )
	{
		m_pMem_fx_block_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_fx_block_array( KFTA_nx3_fx_block &obj )
	{
		obj.SetData( m_pMem_fx_block_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_fx" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_fx_block_size;
	KTemplateDataArrayObject*		m_pMem_fx_block_array;
};
class KFTA_nx3_fx
{
public:
	KFTA_nx3_fx()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_fx( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_fx( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_fx" );
	}
	~KFTA_nx3_fx()
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
	void GetData( int index, KFT_nx3_fx &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};

class KFTA_nx3_mesh;

class KFT_nx3_mesh
{
public:
	KFT_nx3_mesh()
	{
		m_pObj = NULL;
		m_pMem_mesh_name = NULL;
		m_pMem_material_id = NULL;
		m_pMem_channel_id = NULL;
		m_pMem_mesh_block_size = NULL;
		m_pMem_mesh_block_array = NULL;
		m_pMem_ani_block_size = NULL;
		m_pMem_ani_block_array = NULL;
		m_pMem_visi_block_size = NULL;
		m_pMem_visi_block_array = NULL;
		m_pMem_fx_size = NULL;
		m_pMem_fx_array = NULL;
		m_pMem_mesh_children_size = NULL;
		m_pMem_mesh_children_array = NULL;
	}
	KFT_nx3_mesh( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_mesh" );
		m_pMem_mesh_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_name" ));
		m_pMem_material_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "material_id" ));
		m_pMem_channel_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_id" ));
		m_pMem_mesh_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_block_size" ));
		m_pMem_mesh_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_block_array" ));
		m_pMem_ani_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ani_block_size" ));
		m_pMem_ani_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "ani_block_array" ));
		m_pMem_visi_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "visi_block_size" ));
		m_pMem_visi_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "visi_block_array" ));
		m_pMem_fx_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fx_size" ));
		m_pMem_fx_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "fx_array" ));
		m_pMem_mesh_children_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_children_size" ));
		m_pMem_mesh_children_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_children_array" ));
	}
	KFT_nx3_mesh( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mesh_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_name" ));
		m_pMem_material_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "material_id" ));
		m_pMem_channel_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_id" ));
		m_pMem_mesh_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_block_size" ));
		m_pMem_mesh_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_block_array" ));
		m_pMem_ani_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ani_block_size" ));
		m_pMem_ani_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "ani_block_array" ));
		m_pMem_visi_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "visi_block_size" ));
		m_pMem_visi_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "visi_block_array" ));
		m_pMem_fx_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fx_size" ));
		m_pMem_fx_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "fx_array" ));
		m_pMem_mesh_children_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_children_size" ));
		m_pMem_mesh_children_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_children_array" ));
	}
	~KFT_nx3_mesh()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mesh_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_name" ));
		m_pMem_material_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "material_id" ));
		m_pMem_channel_id = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_id" ));
		m_pMem_mesh_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_block_size" ));
		m_pMem_mesh_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_block_array" ));
		m_pMem_ani_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ani_block_size" ));
		m_pMem_ani_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "ani_block_array" ));
		m_pMem_visi_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "visi_block_size" ));
		m_pMem_visi_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "visi_block_array" ));
		m_pMem_fx_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fx_size" ));
		m_pMem_fx_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "fx_array" ));
		m_pMem_mesh_children_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_children_size" ));
		m_pMem_mesh_children_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_children_array" ));
	}
	bool GetString_mesh_name( char *data, int &nLen )
	{
		return m_pMem_mesh_name->GetString( data, nLen );
	}
	bool SetString_mesh_name( const char *data, int nLen )
	{
		return m_pMem_mesh_name->SetString( data, nLen );
	}
	unsigned long GetValue_material_id()
	{
		unsigned long res;
		m_pMem_material_id->GetDWORD( &res );
		return res;
	}
	void SetValue_material_id( unsigned long value )
	{
		m_pMem_material_id->SetDWORD( value );
	}
	unsigned long GetValue_channel_id()
	{
		unsigned long res;
		m_pMem_channel_id->GetDWORD( &res );
		return res;
	}
	void SetValue_channel_id( unsigned long value )
	{
		m_pMem_channel_id->SetDWORD( value );
	}
	unsigned long GetValue_mesh_block_size()
	{
		unsigned long res;
		m_pMem_mesh_block_size->GetDWORD( &res );
		return res;
	}
	void SetValue_mesh_block_size( unsigned long value )
	{
		m_pMem_mesh_block_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_mesh_block_array( KFTA_nx3_mesh_block &obj )
	{
		obj.SetData( m_pMem_mesh_block_array );
	}
	unsigned long GetValue_ani_block_size()
	{
		unsigned long res;
		m_pMem_ani_block_size->GetDWORD( &res );
		return res;
	}
	void SetValue_ani_block_size( unsigned long value )
	{
		m_pMem_ani_block_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_ani_block_array( KFTA_nx3_ani_block &obj )
	{
		obj.SetData( m_pMem_ani_block_array );
	}
	unsigned long GetValue_visi_block_size()
	{
		unsigned long res;
		m_pMem_visi_block_size->GetDWORD( &res );
		return res;
	}
	void SetValue_visi_block_size( unsigned long value )
	{
		m_pMem_visi_block_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_visi_block_array( KFTA_nx3_visi_block &obj )
	{
		obj.SetData( m_pMem_visi_block_array );
	}
	unsigned long GetValue_fx_size()
	{
		unsigned long res;
		m_pMem_fx_size->GetDWORD( &res );
		return res;
	}
	void SetValue_fx_size( unsigned long value )
	{
		m_pMem_fx_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_fx_array( KFTA_nx3_fx &obj )
	{
		obj.SetData( m_pMem_fx_array );
	}
	unsigned long GetValue_mesh_children_size()
	{
		unsigned long res;
		m_pMem_mesh_children_size->GetDWORD( &res );
		return res;
	}
	void SetValue_mesh_children_size( unsigned long value )
	{
		m_pMem_mesh_children_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_mesh_children_array( KFTA_nx3_mesh &obj );

	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_mesh" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_mesh_name;
	KSimpleDataObject*		m_pMem_material_id;
	KSimpleDataObject*		m_pMem_channel_id;
	KSimpleDataObject*		m_pMem_mesh_block_size;
	KTemplateDataArrayObject*		m_pMem_mesh_block_array;
	KSimpleDataObject*		m_pMem_ani_block_size;
	KTemplateDataArrayObject*		m_pMem_ani_block_array;
	KSimpleDataObject*		m_pMem_visi_block_size;
	KTemplateDataArrayObject*		m_pMem_visi_block_array;
	KSimpleDataObject*		m_pMem_fx_size;
	KTemplateDataArrayObject*		m_pMem_fx_array;
	KSimpleDataObject*		m_pMem_mesh_children_size;
	KTemplateDataArrayObject*		m_pMem_mesh_children_array;
};
class KFTA_nx3_mesh
{
public:
	KFTA_nx3_mesh()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_mesh( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_mesh( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_mesh" );
	}
	~KFTA_nx3_mesh()
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
	void GetData( int index, KFT_nx3_mesh &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_mesh_tm
{
public:
	KFT_nx3_mesh_tm()
	{
		m_pObj = NULL;
		m_pMem_name = NULL;
		m_pMem_tm = NULL;
	}
	KFT_nx3_mesh_tm( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_mesh_tm" );
		m_pMem_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "name" ));
		m_pMem_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "tm" ));
	}
	KFT_nx3_mesh_tm( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "name" ));
		m_pMem_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "tm" ));
	}
	~KFT_nx3_mesh_tm()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "name" ));
		m_pMem_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "tm" ));
	}
	bool GetString_name( char *data, int &nLen )
	{
		return m_pMem_name->GetString( data, nLen );
	}
	bool SetString_name( const char *data, int nLen )
	{
		return m_pMem_name->SetString( data, nLen );
	}
	int GetArrayCount_tm()
	{
		return m_pMem_tm->GetDataCount();
	}
	void GetArrayValue_tm( int index, float *data, size_t data_size, int count )
	{
		m_pMem_tm->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_tm()
	{
		m_pMem_tm->RefreshArray();
	}
	void SetArrayValue_tm( int index, const float *data, int count = 1 )
	{
		m_pMem_tm->SetDataFloat( index, data, count );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_mesh_tm" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_name;
	KSimpleDataArrayObject*		m_pMem_tm;
};
class KFTA_nx3_mesh_tm
{
public:
	KFTA_nx3_mesh_tm()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_mesh_tm( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_mesh_tm( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_mesh_tm" );
	}
	~KFTA_nx3_mesh_tm()
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
	void GetData( int index, KFT_nx3_mesh_tm &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_old_nx3_mesh_header
{
public:
	KFT_old_nx3_mesh_header()
	{
		m_pObj = NULL;
		m_pMem_mesh_size = NULL;
		m_pMem_mesh_array = NULL;
	}
	KFT_old_nx3_mesh_header( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "old_nx3_mesh_header" );
		m_pMem_mesh_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_size" ));
		m_pMem_mesh_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_array" ));
	}
	KFT_old_nx3_mesh_header( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mesh_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_size" ));
		m_pMem_mesh_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_array" ));
	}
	~KFT_old_nx3_mesh_header()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mesh_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_size" ));
		m_pMem_mesh_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_array" ));
	}
	unsigned long GetValue_mesh_size()
	{
		unsigned long res;
		m_pMem_mesh_size->GetDWORD( &res );
		return res;
	}
	void SetValue_mesh_size( unsigned long value )
	{
		m_pMem_mesh_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_mesh_array( KFTA_nx3_mesh &obj )
	{
		obj.SetData( m_pMem_mesh_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "old_nx3_mesh_header" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_mesh_size;
	KTemplateDataArrayObject*		m_pMem_mesh_array;
};
class KFTA_old_nx3_mesh_header
{
public:
	KFTA_old_nx3_mesh_header()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_old_nx3_mesh_header( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_old_nx3_mesh_header( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "old_nx3_mesh_header" );
	}
	~KFTA_old_nx3_mesh_header()
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
	void GetData( int index, KFT_old_nx3_mesh_header &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};

inline void KFT_nx3_mesh::GetMemberTemplateArray_mesh_children_array( KFTA_nx3_mesh &obj )
{
	obj.SetData( m_pMem_mesh_children_array );
}

class KFT_nx3_mesh_header
{
public:
	KFT_nx3_mesh_header()
	{
		m_pObj = NULL;
		m_pMem_mesh_size = NULL;
		m_pMem_mesh_array = NULL;
		m_pMem_mesh_tm_size = NULL;
		m_pMem_mesh_tm_array = NULL;
	}
	KFT_nx3_mesh_header( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_mesh_header" );
		m_pMem_mesh_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_size" ));
		m_pMem_mesh_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_array" ));
		m_pMem_mesh_tm_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_tm_size" ));
		m_pMem_mesh_tm_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_tm_array" ));
	}
	KFT_nx3_mesh_header( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mesh_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_size" ));
		m_pMem_mesh_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_array" ));
		m_pMem_mesh_tm_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_tm_size" ));
		m_pMem_mesh_tm_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_tm_array" ));
	}
	~KFT_nx3_mesh_header()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mesh_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_size" ));
		m_pMem_mesh_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_array" ));
		m_pMem_mesh_tm_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "mesh_tm_size" ));
		m_pMem_mesh_tm_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "mesh_tm_array" ));
	}
	unsigned long GetValue_mesh_size()
	{
		unsigned long res;
		m_pMem_mesh_size->GetDWORD( &res );
		return res;
	}
	void SetValue_mesh_size( unsigned long value )
	{
		m_pMem_mesh_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_mesh_array( KFTA_nx3_mesh &obj )
	{
		obj.SetData( m_pMem_mesh_array );
	}
	unsigned long GetValue_mesh_tm_size()
	{
		unsigned long res;
		m_pMem_mesh_tm_size->GetDWORD( &res );
		return res;
	}
	void SetValue_mesh_tm_size( unsigned long value )
	{
		m_pMem_mesh_tm_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_mesh_tm_array( KFTA_nx3_mesh_tm &obj )
	{
		obj.SetData( m_pMem_mesh_tm_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_mesh_header" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_mesh_size;
	KTemplateDataArrayObject*		m_pMem_mesh_array;
	KSimpleDataObject*		m_pMem_mesh_tm_size;
	KTemplateDataArrayObject*		m_pMem_mesh_tm_array;
};
class KFTA_nx3_mesh_header
{
public:
	KFTA_nx3_mesh_header()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_mesh_header( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_mesh_header( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_mesh_header" );
	}
	~KFTA_nx3_mesh_header()
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
	void GetData( int index, KFT_nx3_mesh_header &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_camera_block
{
public:
	KFT_nx3_camera_block()
	{
		m_pObj = NULL;
		m_pMem_mat_object = NULL;
		m_pMem_mat_target = NULL;
		m_pMem_has_target = NULL;
		m_pMem_time_value = NULL;
		m_pMem_fov = NULL;
		m_pMem_dist = NULL;
		m_pMem_hither = NULL;
		m_pMem_yon = NULL;
		m_pMem_near_range = NULL;
		m_pMem_far_range = NULL;
	}
	KFT_nx3_camera_block( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_camera_block" );
		m_pMem_mat_object = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_object" ));
		m_pMem_mat_target = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_target" ));
		m_pMem_has_target = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "has_target" ));
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_fov = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fov" ));
		m_pMem_dist = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "dist" ));
		m_pMem_hither = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "hither" ));
		m_pMem_yon = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "yon" ));
		m_pMem_near_range = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "near_range" ));
		m_pMem_far_range = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "far_range" ));
	}
	KFT_nx3_camera_block( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mat_object = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_object" ));
		m_pMem_mat_target = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_target" ));
		m_pMem_has_target = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "has_target" ));
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_fov = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fov" ));
		m_pMem_dist = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "dist" ));
		m_pMem_hither = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "hither" ));
		m_pMem_yon = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "yon" ));
		m_pMem_near_range = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "near_range" ));
		m_pMem_far_range = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "far_range" ));
	}
	~KFT_nx3_camera_block()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mat_object = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_object" ));
		m_pMem_mat_target = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_target" ));
		m_pMem_has_target = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "has_target" ));
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_fov = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "fov" ));
		m_pMem_dist = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "dist" ));
		m_pMem_hither = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "hither" ));
		m_pMem_yon = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "yon" ));
		m_pMem_near_range = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "near_range" ));
		m_pMem_far_range = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "far_range" ));
	}
	int GetArrayCount_mat_object()
	{
		return m_pMem_mat_object->GetDataCount();
	}
	void GetArrayValue_mat_object( int index, float *data, size_t data_size, int count )
	{
		m_pMem_mat_object->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_mat_object()
	{
		m_pMem_mat_object->RefreshArray();
	}
	void SetArrayValue_mat_object( int index, const float *data, int count = 1 )
	{
		m_pMem_mat_object->SetDataFloat( index, data, count );
	}
	int GetArrayCount_mat_target()
	{
		return m_pMem_mat_target->GetDataCount();
	}
	void GetArrayValue_mat_target( int index, float *data, size_t data_size, int count )
	{
		m_pMem_mat_target->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_mat_target()
	{
		m_pMem_mat_target->RefreshArray();
	}
	void SetArrayValue_mat_target( int index, const float *data, int count = 1 )
	{
		m_pMem_mat_target->SetDataFloat( index, data, count );
	}
	char GetValue_has_target()
	{
		char res;
		m_pMem_has_target->GetChar( &res );
		return res;
	}
	void SetValue_has_target( char value )
	{
		m_pMem_has_target->SetChar( value );
	}
	unsigned long GetValue_time_value()
	{
		unsigned long res;
		m_pMem_time_value->GetDWORD( &res );
		return res;
	}
	void SetValue_time_value( unsigned long value )
	{
		m_pMem_time_value->SetDWORD( value );
	}
	float GetValue_fov()
	{
		float res;
		m_pMem_fov->GetFloat( &res );
		return res;
	}
	void SetValue_fov( float value )
	{
		m_pMem_fov->SetFloat( value );
	}
	float GetValue_dist()
	{
		float res;
		m_pMem_dist->GetFloat( &res );
		return res;
	}
	void SetValue_dist( float value )
	{
		m_pMem_dist->SetFloat( value );
	}
	float GetValue_hither()
	{
		float res;
		m_pMem_hither->GetFloat( &res );
		return res;
	}
	void SetValue_hither( float value )
	{
		m_pMem_hither->SetFloat( value );
	}
	float GetValue_yon()
	{
		float res;
		m_pMem_yon->GetFloat( &res );
		return res;
	}
	void SetValue_yon( float value )
	{
		m_pMem_yon->SetFloat( value );
	}
	float GetValue_near_range()
	{
		float res;
		m_pMem_near_range->GetFloat( &res );
		return res;
	}
	void SetValue_near_range( float value )
	{
		m_pMem_near_range->SetFloat( value );
	}
	float GetValue_far_range()
	{
		float res;
		m_pMem_far_range->GetFloat( &res );
		return res;
	}
	void SetValue_far_range( float value )
	{
		m_pMem_far_range->SetFloat( value );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_camera_block" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataArrayObject*		m_pMem_mat_object;
	KSimpleDataArrayObject*		m_pMem_mat_target;
	KSimpleDataObject*		m_pMem_has_target;
	KSimpleDataObject*		m_pMem_time_value;
	KSimpleDataObject*		m_pMem_fov;
	KSimpleDataObject*		m_pMem_dist;
	KSimpleDataObject*		m_pMem_hither;
	KSimpleDataObject*		m_pMem_yon;
	KSimpleDataObject*		m_pMem_near_range;
	KSimpleDataObject*		m_pMem_far_range;
};
class KFTA_nx3_camera_block
{
public:
	KFTA_nx3_camera_block()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_camera_block( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_camera_block( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_camera_block" );
	}
	~KFTA_nx3_camera_block()
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
	void GetData( int index, KFT_nx3_camera_block &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_camera
{
public:
	KFT_nx3_camera()
	{
		m_pObj = NULL;
		m_pMem_block_name = NULL;
		m_pMem_camera_block_size = NULL;
		m_pMem_camera_block_array = NULL;
	}
	KFT_nx3_camera( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_camera" );
		m_pMem_block_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "block_name" ));
		m_pMem_camera_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "camera_block_size" ));
		m_pMem_camera_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "camera_block_array" ));
	}
	KFT_nx3_camera( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_block_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "block_name" ));
		m_pMem_camera_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "camera_block_size" ));
		m_pMem_camera_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "camera_block_array" ));
	}
	~KFT_nx3_camera()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_block_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "block_name" ));
		m_pMem_camera_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "camera_block_size" ));
		m_pMem_camera_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "camera_block_array" ));
	}
	bool GetString_block_name( char *data, int &nLen )
	{
		return m_pMem_block_name->GetString( data, nLen );
	}
	bool SetString_block_name( const char *data, int nLen )
	{
		return m_pMem_block_name->SetString( data, nLen );
	}
	unsigned long GetValue_camera_block_size()
	{
		unsigned long res;
		m_pMem_camera_block_size->GetDWORD( &res );
		return res;
	}
	void SetValue_camera_block_size( unsigned long value )
	{
		m_pMem_camera_block_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_camera_block_array( KFTA_nx3_camera_block &obj )
	{
		obj.SetData( m_pMem_camera_block_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_camera" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_block_name;
	KSimpleDataObject*		m_pMem_camera_block_size;
	KTemplateDataArrayObject*		m_pMem_camera_block_array;
};
class KFTA_nx3_camera
{
public:
	KFTA_nx3_camera()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_camera( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_camera( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_camera" );
	}
	~KFTA_nx3_camera()
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
	void GetData( int index, KFT_nx3_camera &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_light_block
{
public:
	KFT_nx3_light_block()
	{
		m_pObj = NULL;
		m_pMem_mat_object = NULL;
		m_pMem_mat_target = NULL;
		m_pMem_time_value = NULL;
		m_pMem_clr_light = NULL;
		m_pMem_intens = NULL;
		m_pMem_atten_start = NULL;
		m_pMem_atten_end = NULL;
		m_pMem_onoff = NULL;
	}
	KFT_nx3_light_block( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_light_block" );
		m_pMem_mat_object = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_object" ));
		m_pMem_mat_target = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_target" ));
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_clr_light = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "clr_light" ));
		m_pMem_intens = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "intens" ));
		m_pMem_atten_start = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "atten_start" ));
		m_pMem_atten_end = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "atten_end" ));
		m_pMem_onoff = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "onoff" ));
	}
	KFT_nx3_light_block( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mat_object = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_object" ));
		m_pMem_mat_target = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_target" ));
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_clr_light = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "clr_light" ));
		m_pMem_intens = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "intens" ));
		m_pMem_atten_start = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "atten_start" ));
		m_pMem_atten_end = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "atten_end" ));
		m_pMem_onoff = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "onoff" ));
	}
	~KFT_nx3_light_block()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_mat_object = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_object" ));
		m_pMem_mat_target = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "mat_target" ));
		m_pMem_time_value = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "time_value" ));
		m_pMem_clr_light = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "clr_light" ));
		m_pMem_intens = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "intens" ));
		m_pMem_atten_start = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "atten_start" ));
		m_pMem_atten_end = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "atten_end" ));
		m_pMem_onoff = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "onoff" ));
	}
	int GetArrayCount_mat_object()
	{
		return m_pMem_mat_object->GetDataCount();
	}
	void GetArrayValue_mat_object( int index, float *data, size_t data_size, int count )
	{
		m_pMem_mat_object->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_mat_object()
	{
		m_pMem_mat_object->RefreshArray();
	}
	void SetArrayValue_mat_object( int index, const float *data, int count = 1 )
	{
		m_pMem_mat_object->SetDataFloat( index, data, count );
	}
	int GetArrayCount_mat_target()
	{
		return m_pMem_mat_target->GetDataCount();
	}
	void GetArrayValue_mat_target( int index, float *data, size_t data_size, int count )
	{
		m_pMem_mat_target->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_mat_target()
	{
		m_pMem_mat_target->RefreshArray();
	}
	void SetArrayValue_mat_target( int index, const float *data, int count = 1 )
	{
		m_pMem_mat_target->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_time_value()
	{
		unsigned long res;
		m_pMem_time_value->GetDWORD( &res );
		return res;
	}
	void SetValue_time_value( unsigned long value )
	{
		m_pMem_time_value->SetDWORD( value );
	}
	unsigned long GetValue_clr_light()
	{
		unsigned long res;
		m_pMem_clr_light->GetDWORD( &res );
		return res;
	}
	void SetValue_clr_light( unsigned long value )
	{
		m_pMem_clr_light->SetDWORD( value );
	}
	float GetValue_intens()
	{
		float res;
		m_pMem_intens->GetFloat( &res );
		return res;
	}
	void SetValue_intens( float value )
	{
		m_pMem_intens->SetFloat( value );
	}
	float GetValue_atten_start()
	{
		float res;
		m_pMem_atten_start->GetFloat( &res );
		return res;
	}
	void SetValue_atten_start( float value )
	{
		m_pMem_atten_start->SetFloat( value );
	}
	float GetValue_atten_end()
	{
		float res;
		m_pMem_atten_end->GetFloat( &res );
		return res;
	}
	void SetValue_atten_end( float value )
	{
		m_pMem_atten_end->SetFloat( value );
	}
	char GetValue_onoff()
	{
		char res;
		m_pMem_onoff->GetChar( &res );
		return res;
	}
	void SetValue_onoff( char value )
	{
		m_pMem_onoff->SetChar( value );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_light_block" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataArrayObject*		m_pMem_mat_object;
	KSimpleDataArrayObject*		m_pMem_mat_target;
	KSimpleDataObject*		m_pMem_time_value;
	KSimpleDataObject*		m_pMem_clr_light;
	KSimpleDataObject*		m_pMem_intens;
	KSimpleDataObject*		m_pMem_atten_start;
	KSimpleDataObject*		m_pMem_atten_end;
	KSimpleDataObject*		m_pMem_onoff;
};
class KFTA_nx3_light_block
{
public:
	KFTA_nx3_light_block()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_light_block( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_light_block( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_light_block" );
	}
	~KFTA_nx3_light_block()
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
	void GetData( int index, KFT_nx3_light_block &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_light
{
public:
	KFT_nx3_light()
	{
		m_pObj = NULL;
		m_pMem_block_name = NULL;
		m_pMem_light_type = NULL;
		m_pMem_ambient_light = NULL;
		m_pMem_light_block_size = NULL;
		m_pMem_light_block_array = NULL;
	}
	KFT_nx3_light( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_light" );
		m_pMem_block_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "block_name" ));
		m_pMem_light_type = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_type" ));
		m_pMem_ambient_light = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ambient_light" ));
		m_pMem_light_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_block_size" ));
		m_pMem_light_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "light_block_array" ));
	}
	KFT_nx3_light( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_block_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "block_name" ));
		m_pMem_light_type = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_type" ));
		m_pMem_ambient_light = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ambient_light" ));
		m_pMem_light_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_block_size" ));
		m_pMem_light_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "light_block_array" ));
	}
	~KFT_nx3_light()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_block_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "block_name" ));
		m_pMem_light_type = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_type" ));
		m_pMem_ambient_light = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "ambient_light" ));
		m_pMem_light_block_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_block_size" ));
		m_pMem_light_block_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "light_block_array" ));
	}
	bool GetString_block_name( char *data, int &nLen )
	{
		return m_pMem_block_name->GetString( data, nLen );
	}
	bool SetString_block_name( const char *data, int nLen )
	{
		return m_pMem_block_name->SetString( data, nLen );
	}
	unsigned long GetValue_light_type()
	{
		unsigned long res;
		m_pMem_light_type->GetDWORD( &res );
		return res;
	}
	void SetValue_light_type( unsigned long value )
	{
		m_pMem_light_type->SetDWORD( value );
	}
	unsigned long GetValue_ambient_light()
	{
		unsigned long res;
		m_pMem_ambient_light->GetDWORD( &res );
		return res;
	}
	void SetValue_ambient_light( unsigned long value )
	{
		m_pMem_ambient_light->SetDWORD( value );
	}
	unsigned long GetValue_light_block_size()
	{
		unsigned long res;
		m_pMem_light_block_size->GetDWORD( &res );
		return res;
	}
	void SetValue_light_block_size( unsigned long value )
	{
		m_pMem_light_block_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_light_block_array( KFTA_nx3_light_block &obj )
	{
		obj.SetData( m_pMem_light_block_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_light" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_block_name;
	KSimpleDataObject*		m_pMem_light_type;
	KSimpleDataObject*		m_pMem_ambient_light;
	KSimpleDataObject*		m_pMem_light_block_size;
	KTemplateDataArrayObject*		m_pMem_light_block_array;
};
class KFTA_nx3_light
{
public:
	KFTA_nx3_light()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_light( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_light( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_light" );
	}
	~KFTA_nx3_light()
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
	void GetData( int index, KFT_nx3_light &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_light_header
{
public:
	KFT_nx3_light_header()
	{
		m_pObj = NULL;
		m_pMem_light_size = NULL;
		m_pMem_light_array = NULL;
	}
	KFT_nx3_light_header( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_light_header" );
		m_pMem_light_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_size" ));
		m_pMem_light_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "light_array" ));
	}
	KFT_nx3_light_header( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_light_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_size" ));
		m_pMem_light_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "light_array" ));
	}
	~KFT_nx3_light_header()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_light_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "light_size" ));
		m_pMem_light_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "light_array" ));
	}
	unsigned long GetValue_light_size()
	{
		unsigned long res;
		m_pMem_light_size->GetDWORD( &res );
		return res;
	}
	void SetValue_light_size( unsigned long value )
	{
		m_pMem_light_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_light_array( KFTA_nx3_light &obj )
	{
		obj.SetData( m_pMem_light_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_light_header" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_light_size;
	KTemplateDataArrayObject*		m_pMem_light_array;
};
class KFTA_nx3_light_header
{
public:
	KFTA_nx3_light_header()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_light_header( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_light_header( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_light_header" );
	}
	~KFTA_nx3_light_header()
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
	void GetData( int index, KFT_nx3_light_header &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_bone_ani
{
public:
	KFT_nx3_bone_ani()
	{
		m_pObj = NULL;
		m_pMem_szName = NULL;
		m_pMem_parent_Index = NULL;
		m_pMem_base_tm = NULL;
		m_pMem_flag = NULL;
		m_pMem_pos_time_size = NULL;
		m_pMem_pos_time_array = NULL;
		m_pMem_pos_key_size = NULL;
		m_pMem_pos_key_array = NULL;
		m_pMem_rot_time_size = NULL;
		m_pMem_rot_time_array = NULL;
		m_pMem_rot_key_size = NULL;
		m_pMem_rot_key_array = NULL;
		m_pMem_child_size = NULL;
		m_pMem_child_Index_array = NULL;
	}
	KFT_nx3_bone_ani( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_bone_ani" );
		m_pMem_szName = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "szName" ));
		m_pMem_parent_Index = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "parent_Index" ));
		m_pMem_base_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "base_tm" ));
		m_pMem_flag = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "flag" ));
		m_pMem_pos_time_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "pos_time_size" ));
		m_pMem_pos_time_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "pos_time_array" ));
		m_pMem_pos_key_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "pos_key_size" ));
		m_pMem_pos_key_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "pos_key_array" ));
		m_pMem_rot_time_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "rot_time_size" ));
		m_pMem_rot_time_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "rot_time_array" ));
		m_pMem_rot_key_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "rot_key_size" ));
		m_pMem_rot_key_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "rot_key_array" ));
		m_pMem_child_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "child_size" ));
		m_pMem_child_Index_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "child_Index_array" ));
	}
	KFT_nx3_bone_ani( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_szName = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "szName" ));
		m_pMem_parent_Index = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "parent_Index" ));
		m_pMem_base_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "base_tm" ));
		m_pMem_flag = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "flag" ));
		m_pMem_pos_time_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "pos_time_size" ));
		m_pMem_pos_time_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "pos_time_array" ));
		m_pMem_pos_key_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "pos_key_size" ));
		m_pMem_pos_key_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "pos_key_array" ));
		m_pMem_rot_time_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "rot_time_size" ));
		m_pMem_rot_time_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "rot_time_array" ));
		m_pMem_rot_key_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "rot_key_size" ));
		m_pMem_rot_key_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "rot_key_array" ));
		m_pMem_child_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "child_size" ));
		m_pMem_child_Index_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "child_Index_array" ));
	}
	~KFT_nx3_bone_ani()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_szName = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "szName" ));
		m_pMem_parent_Index = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "parent_Index" ));
		m_pMem_base_tm = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "base_tm" ));
		m_pMem_flag = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "flag" ));
		m_pMem_pos_time_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "pos_time_size" ));
		m_pMem_pos_time_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "pos_time_array" ));
		m_pMem_pos_key_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "pos_key_size" ));
		m_pMem_pos_key_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "pos_key_array" ));
		m_pMem_rot_time_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "rot_time_size" ));
		m_pMem_rot_time_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "rot_time_array" ));
		m_pMem_rot_key_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "rot_key_size" ));
		m_pMem_rot_key_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "rot_key_array" ));
		m_pMem_child_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "child_size" ));
		m_pMem_child_Index_array = reinterpret_cast<KSimpleDataArrayObject*>(m_pObj->GetMemberObject( "child_Index_array" ));
	}
	bool GetString_szName( char *data, int &nLen )
	{
		return m_pMem_szName->GetString( data, nLen );
	}
	bool SetString_szName( const char *data, int nLen )
	{
		return m_pMem_szName->SetString( data, nLen );
	}
	unsigned long GetValue_parent_Index()
	{
		unsigned long res;
		m_pMem_parent_Index->GetDWORD( &res );
		return res;
	}
	void SetValue_parent_Index( unsigned long value )
	{
		m_pMem_parent_Index->SetDWORD( value );
	}
	int GetArrayCount_base_tm()
	{
		return m_pMem_base_tm->GetDataCount();
	}
	void GetArrayValue_base_tm( int index, float *data, size_t data_size, int count )
	{
		m_pMem_base_tm->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_base_tm()
	{
		m_pMem_base_tm->RefreshArray();
	}
	void SetArrayValue_base_tm( int index, const float *data, int count = 1 )
	{
		m_pMem_base_tm->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_flag()
	{
		unsigned long res;
		m_pMem_flag->GetDWORD( &res );
		return res;
	}
	void SetValue_flag( unsigned long value )
	{
		m_pMem_flag->SetDWORD( value );
	}
	unsigned long GetValue_pos_time_size()
	{
		unsigned long res;
		m_pMem_pos_time_size->GetDWORD( &res );
		return res;
	}
	void SetValue_pos_time_size( unsigned long value )
	{
		m_pMem_pos_time_size->SetDWORD( value );
	}
	int GetArrayCount_pos_time_array()
	{
		return m_pMem_pos_time_array->GetDataCount();
	}
	void GetArrayValue_pos_time_array( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_pos_time_array->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_pos_time_array()
	{
		m_pMem_pos_time_array->RefreshArray();
	}
	void SetArrayValue_pos_time_array( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_pos_time_array->SetDataDWORD( index, data, count );
	}
	unsigned long GetValue_pos_key_size()
	{
		unsigned long res;
		m_pMem_pos_key_size->GetDWORD( &res );
		return res;
	}
	void SetValue_pos_key_size( unsigned long value )
	{
		m_pMem_pos_key_size->SetDWORD( value );
	}
	int GetArrayCount_pos_key_array()
	{
		return m_pMem_pos_key_array->GetDataCount();
	}
	void GetArrayValue_pos_key_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_pos_key_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_pos_key_array()
	{
		m_pMem_pos_key_array->RefreshArray();
	}
	void SetArrayValue_pos_key_array( int index, const float *data, int count = 1 )
	{
		m_pMem_pos_key_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_rot_time_size()
	{
		unsigned long res;
		m_pMem_rot_time_size->GetDWORD( &res );
		return res;
	}
	void SetValue_rot_time_size( unsigned long value )
	{
		m_pMem_rot_time_size->SetDWORD( value );
	}
	int GetArrayCount_rot_time_array()
	{
		return m_pMem_rot_time_array->GetDataCount();
	}
	void GetArrayValue_rot_time_array( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_rot_time_array->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_rot_time_array()
	{
		m_pMem_rot_time_array->RefreshArray();
	}
	void SetArrayValue_rot_time_array( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_rot_time_array->SetDataDWORD( index, data, count );
	}
	unsigned long GetValue_rot_key_size()
	{
		unsigned long res;
		m_pMem_rot_key_size->GetDWORD( &res );
		return res;
	}
	void SetValue_rot_key_size( unsigned long value )
	{
		m_pMem_rot_key_size->SetDWORD( value );
	}
	int GetArrayCount_rot_key_array()
	{
		return m_pMem_rot_key_array->GetDataCount();
	}
	void GetArrayValue_rot_key_array( int index, float *data, size_t data_size, int count )
	{
		m_pMem_rot_key_array->GetDataFloat( index, data, data_size, count );
	}
	void RefreshArray_rot_key_array()
	{
		m_pMem_rot_key_array->RefreshArray();
	}
	void SetArrayValue_rot_key_array( int index, const float *data, int count = 1 )
	{
		m_pMem_rot_key_array->SetDataFloat( index, data, count );
	}
	unsigned long GetValue_child_size()
	{
		unsigned long res;
		m_pMem_child_size->GetDWORD( &res );
		return res;
	}
	void SetValue_child_size( unsigned long value )
	{
		m_pMem_child_size->SetDWORD( value );
	}
	int GetArrayCount_child_Index_array()
	{
		return m_pMem_child_Index_array->GetDataCount();
	}
	void GetArrayValue_child_Index_array( int index, unsigned long *data, size_t data_size, int count )
	{
		m_pMem_child_Index_array->GetDataDWORD( index, data, data_size, count );
	}
	void RefreshArray_child_Index_array()
	{
		m_pMem_child_Index_array->RefreshArray();
	}
	void SetArrayValue_child_Index_array( int index, const unsigned long *data, int count = 1 )
	{
		m_pMem_child_Index_array->SetDataDWORD( index, data, count );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_bone_ani" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_szName;
	KSimpleDataObject*		m_pMem_parent_Index;
	KSimpleDataArrayObject*		m_pMem_base_tm;
	KSimpleDataObject*		m_pMem_flag;
	KSimpleDataObject*		m_pMem_pos_time_size;
	KSimpleDataArrayObject*		m_pMem_pos_time_array;
	KSimpleDataObject*		m_pMem_pos_key_size;
	KSimpleDataArrayObject*		m_pMem_pos_key_array;
	KSimpleDataObject*		m_pMem_rot_time_size;
	KSimpleDataArrayObject*		m_pMem_rot_time_array;
	KSimpleDataObject*		m_pMem_rot_key_size;
	KSimpleDataArrayObject*		m_pMem_rot_key_array;
	KSimpleDataObject*		m_pMem_child_size;
	KSimpleDataArrayObject*		m_pMem_child_Index_array;
};
class KFTA_nx3_bone_ani
{
public:
	KFTA_nx3_bone_ani()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_bone_ani( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_bone_ani( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_bone_ani" );
	}
	~KFTA_nx3_bone_ani()
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
	void GetData( int index, KFT_nx3_bone_ani &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_bone_ani_channel
{
public:
	KFT_nx3_bone_ani_channel()
	{
		m_pObj = NULL;
		m_pMem_channel_name = NULL;
		m_pMem_channel_flag = NULL;
		m_pMem_channel_time_span = NULL;
		m_pMem_channel_frame_rate = NULL;
		m_pMem_bone_ani_size = NULL;
		m_pMem_bone_ani_array = NULL;
	}
	KFT_nx3_bone_ani_channel( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_bone_ani_channel" );
		m_pMem_channel_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_name" ));
		m_pMem_channel_flag = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_flag" ));
		m_pMem_channel_time_span = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_time_span" ));
		m_pMem_channel_frame_rate = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_frame_rate" ));
		m_pMem_bone_ani_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_ani_size" ));
		m_pMem_bone_ani_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bone_ani_array" ));
	}
	KFT_nx3_bone_ani_channel( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_channel_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_name" ));
		m_pMem_channel_flag = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_flag" ));
		m_pMem_channel_time_span = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_time_span" ));
		m_pMem_channel_frame_rate = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_frame_rate" ));
		m_pMem_bone_ani_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_ani_size" ));
		m_pMem_bone_ani_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bone_ani_array" ));
	}
	~KFT_nx3_bone_ani_channel()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_channel_name = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_name" ));
		m_pMem_channel_flag = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_flag" ));
		m_pMem_channel_time_span = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_time_span" ));
		m_pMem_channel_frame_rate = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_frame_rate" ));
		m_pMem_bone_ani_size = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_ani_size" ));
		m_pMem_bone_ani_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "bone_ani_array" ));
	}
	bool GetString_channel_name( char *data, int &nLen )
	{
		return m_pMem_channel_name->GetString( data, nLen );
	}
	bool SetString_channel_name( const char *data, int nLen )
	{
		return m_pMem_channel_name->SetString( data, nLen );
	}
	unsigned long GetValue_channel_flag()
	{
		unsigned long res;
		m_pMem_channel_flag->GetDWORD( &res );
		return res;
	}
	void SetValue_channel_flag( unsigned long value )
	{
		m_pMem_channel_flag->SetDWORD( value );
	}
	unsigned long GetValue_channel_time_span()
	{
		unsigned long res;
		m_pMem_channel_time_span->GetDWORD( &res );
		return res;
	}
	void SetValue_channel_time_span( unsigned long value )
	{
		m_pMem_channel_time_span->SetDWORD( value );
	}
	unsigned long GetValue_channel_frame_rate()
	{
		unsigned long res;
		m_pMem_channel_frame_rate->GetDWORD( &res );
		return res;
	}
	void SetValue_channel_frame_rate( unsigned long value )
	{
		m_pMem_channel_frame_rate->SetDWORD( value );
	}
	unsigned long GetValue_bone_ani_size()
	{
		unsigned long res;
		m_pMem_bone_ani_size->GetDWORD( &res );
		return res;
	}
	void SetValue_bone_ani_size( unsigned long value )
	{
		m_pMem_bone_ani_size->SetDWORD( value );
	}
	void GetMemberTemplateArray_bone_ani_array( KFTA_nx3_bone_ani &obj )
	{
		obj.SetData( m_pMem_bone_ani_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_bone_ani_channel" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_channel_name;
	KSimpleDataObject*		m_pMem_channel_flag;
	KSimpleDataObject*		m_pMem_channel_time_span;
	KSimpleDataObject*		m_pMem_channel_frame_rate;
	KSimpleDataObject*		m_pMem_bone_ani_size;
	KTemplateDataArrayObject*		m_pMem_bone_ani_array;
};
class KFTA_nx3_bone_ani_channel
{
public:
	KFTA_nx3_bone_ani_channel()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_bone_ani_channel( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_bone_ani_channel( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_bone_ani_channel" );
	}
	~KFTA_nx3_bone_ani_channel()
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
	void GetData( int index, KFT_nx3_bone_ani_channel &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
class KFT_nx3_bone_ani_header
{
public:
	KFT_nx3_bone_ani_header()
	{
		m_pObj = NULL;
		m_pMem_bone_count = NULL;
		m_pMem_channel_count = NULL;
		m_pMem_channel_array = NULL;
	}
	KFT_nx3_bone_ani_header( KFiler *filer )
	{
		m_pObj = filer->CreateTemplateObject( "nx3_bone_ani_header" );
		m_pMem_bone_count = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_count" ));
		m_pMem_channel_count = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_count" ));
		m_pMem_channel_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "channel_array" ));
	}
	KFT_nx3_bone_ani_header( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_bone_count = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_count" ));
		m_pMem_channel_count = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_count" ));
		m_pMem_channel_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "channel_array" ));
	}
	~KFT_nx3_bone_ani_header()
	{
	}
	void SetDataObject( KTemplateDataObject *pObj )
	{
		m_pObj = pObj;
		m_pMem_bone_count = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "bone_count" ));
		m_pMem_channel_count = reinterpret_cast<KSimpleDataObject*>(m_pObj->GetMemberObject( "channel_count" ));
		m_pMem_channel_array = reinterpret_cast<KTemplateDataArrayObject*>(m_pObj->GetMemberObject( "channel_array" ));
	}
	unsigned long GetValue_bone_count()
	{
		unsigned long res;
		m_pMem_bone_count->GetDWORD( &res );
		return res;
	}
	void SetValue_bone_count( unsigned long value )
	{
		m_pMem_bone_count->SetDWORD( value );
	}
	unsigned long GetValue_channel_count()
	{
		unsigned long res;
		m_pMem_channel_count->GetDWORD( &res );
		return res;
	}
	void SetValue_channel_count( unsigned long value )
	{
		m_pMem_channel_count->SetDWORD( value );
	}
	void GetMemberTemplateArray_channel_array( KFTA_nx3_bone_ani_channel &obj )
	{
		obj.SetData( m_pMem_channel_array );
	}
	void Save( KFiler &filer )
	{
		filer.AddDataObject( m_pObj );
	}
	static bool Check( KTemplateDataObject *pObj)
	{
		return strcmp( pObj->GetTemplateName(), "nx3_bone_ani_header" ) == 0;
	}
private:
	KTemplateDataObject*		m_pObj;
	KSimpleDataObject*		m_pMem_bone_count;
	KSimpleDataObject*		m_pMem_channel_count;
	KTemplateDataArrayObject*		m_pMem_channel_array;
};
class KFTA_nx3_bone_ani_header
{
public:
	KFTA_nx3_bone_ani_header()
	{
		m_pObj = NULL;
		m_pParent = NULL;
	}
	KFTA_nx3_bone_ani_header( KTemplateDataArrayObject* pObj )
	{
		m_pObj = pObj;
		m_pParent = m_pObj->GetParent();
	}
	KFTA_nx3_bone_ani_header( KTemplateDataObject *pParent )
	{
		m_pParent = pParent;
		m_pObj = pParent->GetTemplateDataArrayMemberObject( "nx3_bone_ani_header" );
	}
	~KFTA_nx3_bone_ani_header()
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
	void GetData( int index, KFT_nx3_bone_ani_header &obj )
	{
		obj.SetDataObject( m_pObj->GetData( index ) );
	}

private:
	KTemplateDataObject*		m_pParent;
	KTemplateDataArrayObject*		m_pObj;
};
#endif