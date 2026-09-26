#ifndef XTPTabThemeOffice2013_h__
#define XTPTabThemeOffice2013_h__

class _XTP_EXT_CLASS CXTPTabThemeOffice2013 : public CXTPTabThemeVisualStudio2012
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPTabThemeOffice2013 object.
	//-------------------------------------------------------------------------
	CXTPTabThemeOffice2013();

	virtual void RefreshMetrics();

	virtual XTPTabColorStyle GetDefaultColorSet() { return xtpTabColorOffice2013; }

};

#endif // XTPTabThemeOffice2013_h__
