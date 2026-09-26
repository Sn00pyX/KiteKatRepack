class CXTPTabClientWnd::CNavigateButtonActiveFiles : public CXTPTabManagerNavigateButton
{
public:
	CNavigateButtonActiveFiles(CXTPTabManager* pManager, CXTPTabClientWnd* pTabClientWnd);

	void DrawEntry(CDC* pDC, CRect rc);

	void Reposition(CRect& rcNavigateButtons);

	void PerformClick(HWND hWnd, CPoint pt);

protected:
	BOOL m_bHiddenTabs;
	CXTPTabClientWnd* m_pTabClientWnd;

};
