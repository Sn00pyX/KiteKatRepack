class CXTPRibbonRenameDialog : public CDialog
{
public:
	CXTPRibbonRenameDialog()
		: CDialog(XTP_IDD_RIBBONCUSTOMIZE_RENAME)
	{

	}

	void DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		DDX_Text(pDX, XTP_IDC_RIBBONEDIT_RENAME, m_strName);
	}

public:
	CString m_strName;

};
