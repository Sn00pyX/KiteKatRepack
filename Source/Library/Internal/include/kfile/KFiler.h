// KFiler.h: interface for the KFiler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_KFILER_H_INCLUDED_)
#define _KFILER_H_INCLUDED_

#pragma once


#include "KStream.h"
#include "KDataObject.h"

class KFiler
{
public:
	KFiler();
	virtual ~KFiler();
	
	void Clear();

	virtual bool Save( KStream &stream ) = 0;
	virtual bool Load( KStream &stream ) = 0;
	

	// create basic object
	KTemplateDataObject* CreateTemplateObject( const GUID &uuidType );
	KTemplateDataObject* CreateTemplateObject( const char *template_name );
	//KSimpleDataObject* CreateSimpleDataObject( const KDataObject::CLASSTYPEID &classtype, void *initial_pData, int nSize );

	// create array object
	//KTemplateDataArrayObject* CreateTemplateArrayObject( const GUID &uuidType, int nCount, const char *countVarName, KTemplateDataObject *pParent );
	//KSimpleDataArrayObject* CreateSimpleDataArrayObject( const KDataObject::CLASSTYPEID &classtype, void *initial_pData, int nSize, int nCount, const char *countVarName, KTemplateDataObject *pParent );

	/// add new object
	void AddDataObject( KTemplateDataObject *dataobj );

	// enumeration
	int GetDataObjectCount();
	KTemplateDataObject* GetDataObjectAt(int index);


protected:
	std::vector<KTemplateDataObject*> m_vectorData;
	std::vector<KTemplateDataObject*> m_vectorCreated;

public:
	static void RegisterTemplates( KStream &stream );
	static void ClearRegisteredTemplates();
	static KTemplateInfo *GetTemplateInfo( const GUID &uuidType );
	static KTemplateInfo *GetTemplateInfo( const char *template_name );
	static void GenerateSource( KStream &stream, const char *filename );
	
private:
	static void addTemplate( KTemplateInfo *memberinfo );
	static KHash<KTemplateInfo*, hashPr_GUID> m_hashByGUID;
	static KHash<KTemplateInfo*, hashPr_string> m_hashByName;
	static std::vector<KTemplateInfo*> m_vectorTemplateMember;
};

#endif // !defined(_KFILER_H_INCLUDED_)
