// KTemplateInfo.h: interface for the KTemplateInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_KTEMPLATEINFO_H__INCLUDED_)
#define _KTEMPLATEINFO_H__INCLUDED_

#pragma once

//#include <vector>
//#include <string>
#include <guiddef.h>
#include "../toolkit/khash.h"
#include "KStream.h"

class KTemplateInfo
{
public:
	KTemplateInfo();
	~KTemplateInfo();
	void SetTemplateName( const char *name )	{ m_strTemplateName = name; }
	void SetTemplateGUID( const GUID &uuid )	{ m_uuidTemplate = uuid; }
	
	const char *GetTemplateName()	const		{ return m_strTemplateName.c_str(); }
	const GUID &GetTemplateGUID()	const		{ return m_uuidTemplate; }
	int GetMemberCount()						{ return m_nMemberCount; }
	
	class KDataObject *CreateMember( int index, class KTemplateDataObject *pParent );
	
	const char *GetMemberName( int index )		{ return m_members[index].name.c_str(); }
	const GUID &GetMemberGUID( int index )		{ return m_members[index].uuidType; }
	int		    GetMemberClassID( int index )   { return m_members[index].classType; }
	
	void GenerateSource( KStream &stream );
	
	inline int GetMemberIndex( const char *name );
	inline bool IsMember( const char *name );

	void AddMember( const char *name, const char *classname, const GUID *uuidType, int classType, int nArrayCount, const char *strArrayCountVar );
private:
	void generateClassHeader( KStream &stream );
	void generateMemberSource( KStream &stream );
	void generateClassFooter( KStream &stream );
	void generateArrayClassHeader( KStream &stream );
	void generateArrayMemberSource( KStream &stream );
	void generateArrayClassFooter( KStream &stream );
	
	void setTypeInfo( int type );
	void replaceGenSrc( std::string &src );

	std::string m_gen_classname;		///< %cn%
	std::string m_gen_membername;		///< %mn%
	std::string m_gen_membertype;		///< %mt%
	std::string m_gen_membervartype;	///< %mv%
	std::string m_gen_membervarmethod;	///< %mm%
	std::string m_gen_memberclassname;	///< %mc%
	
private:
	int m_nMemberCount;
	GUID m_uuidTemplate;
	std::string m_strTemplateName;
	struct MEMBER
	{
		std::string name;
		std::string classname;
		GUID		uuidType;
		int			classType;
		int			arraycount;
		std::string arrayvarname;
	};
	std::vector<MEMBER> m_members;
	KHash<int, hashPr_string> m_hashIndexByName;
};

inline int KTemplateInfo::GetMemberIndex( const char *name )
{
	int index;
	if ( m_hashIndexByName.lookup( name, index ) == true )
		return index;
	return -1;
}


inline bool KTemplateInfo::IsMember( const char *name )
{
	return GetMemberIndex( name ) != -1;
}


#endif // !defined(_KTEMPLATEINFO_H__INCLUDED_)

