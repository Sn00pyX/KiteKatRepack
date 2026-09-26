#if !defined(_XTPTABCOLORSETOFFICE2007_H__)
#define _XTPTABCOLORSETOFFICE2007_H__

//===========================================================================
// Summary:
//     CXTPTabColorSetOffice2007 is a CXTPTabColorSetOffice2003 derived class that represents the
//     Office 2007 tab color set.
//===========================================================================
class _XTP_EXT_CLASS CXTPTabColorSetOffice2007 : public CXTPTabColorSetOffice2003
{
public:
	//-------------------------------------------------------------------------
	// Summary:
	//     This member is called to refresh the visual metrics of the tabs.
	// Remarks:
	//     All of the color members are refreshed when this is called.
	//     This member can be override this member to change the colors of
	//     the color members.
	//-------------------------------------------------------------------------
	virtual void RefreshMetrics();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to fill the tab navigation buttons.
	// Parameters:
	//     pDC     - Pointer to a valid device context.
	//     pButton - Tab navigation button to fill.
	//     rc      - Bounding rectangle of the tab navigation button.
	// Remarks:
	//     This member takes care of filling the tab navigation buttons
	//     that are in the header of the TabClient.
	//     The XTPTabColorStyle CXTPTabPaintManagerColorSet classes override this to perform
	//     actions such as painting the highlighting, pressed, and normal
	//     versions of the tab navigation buttons.
	//
	//     If IsAppThemed is FALSE, then CXTPTabColorSetDefault::FillNavigationButton
	//     is used.
	//
	// See Also: CXTPTabManager::GetNavigateButton, CXTPTabManagerNavigateButton
	//-----------------------------------------------------------------------
	void FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc);
protected:
	CXTPPaintManagerColor  m_clrButtonText;         // Text color
};

#endif // !defined(_XTPTABCOLORSETOFFICE2007_H__)
