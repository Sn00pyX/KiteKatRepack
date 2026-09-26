//===========================================================================
// Summary:
//     CXTPCommandBarsContextMenus is CXTPCommandBarList derived class,
//     represents collection of context menus
//===========================================================================
class _XTP_EXT_CLASS CXTPCommandBarsContextMenus : public CXTPCommandBarList
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPCommandBarsContextMenus object
	// Parameters:
	//     pCommandBars - Parent CXTPCommandBars object
	//-----------------------------------------------------------------------
	CXTPCommandBarsContextMenus(CXTPCommandBars* pCommandBars);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to add new context menu to collection
	// Parameters:
	//     nIDBar - Menu resource identifier
	//     pCommandBar - CommandBar to be added
	//     lpszTitle - Caption of context menu
	//     pSubMenu - Context menu to convert
	//     nSubMenu - Index of submenu to add
	// Returns:
	//     Pointer to added context menu.
	//-----------------------------------------------------------------------
	CXTPCommandBar* Add(UINT nIDBar);
	CXTPCommandBar* Add(UINT nIDBar, UINT nIDMenuResource, int nSubMenu); // <combine CXTPCommandBarsContextMenus::Add@UINT>
	CXTPCommandBar* Add(CXTPCommandBar* pCommandBar); // <combine CXTPCommandBarsContextMenus::Add@UINT>
	CXTPCommandBar* Add(UINT nIDBar, LPCTSTR lpszTitle, CMenu* pSubMenu); // <combine CXTPCommandBarsContextMenus::Add@UINT>

//{{AFX_CODEJOCK_PRIVATE
//}}AFX_CODEJOCK_PRIVATE
};
