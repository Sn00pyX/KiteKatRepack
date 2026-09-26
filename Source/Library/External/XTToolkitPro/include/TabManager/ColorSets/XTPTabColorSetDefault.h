#if !defined(_XTPTABCOLORSETDEFAULT_H__)
#define _XTPTABCOLORSETDEFAULT_H__

//-------------------------------------------------------------------------
// Summary:
//     CXTPTabColorSetDefault is a CXTPTabPaintManagerColorSet derived class that represents the
//     default tab color set.
// Remarks:
//     To use the default color set, SetColor is used to apply
//     the xtpTabColorDefault XTPTabColorStyle.
//
// See Also: XTPTabColorStyle, XTPTabAppearanceStyle, SetAppearance, GetAppearance, GetAppearanceSet,
//           SetColor, GetColor, GetColorSet, SetColorSet, SetAppearanceSet
//-------------------------------------------------------------------------
class _XTP_EXT_CLASS CXTPTabColorSetDefault : public CXTPTabPaintManagerColorSet
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
	// See Also: CXTPTabManager::GetNavigateButton, CXTPTabManagerNavigateButton
	//-----------------------------------------------------------------------
	void FillNavigateButton(CDC* pDC, CXTPTabManagerNavigateButton* pButton, CRect& rc);
};


class CXTPTabPaintManager::CColorSetDefault : public CXTPTabColorSetDefault
{

};

#endif // !defined(_XTPTABCOLORSETDEFAULT_H__)
