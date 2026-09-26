class _XTP_EXT_CLASS CXTPResourceImageList : public CXTPResourceImages
{
	DECLARE_DYNCREATE(CXTPResourceImageList)
	friend class CXTPResourceImage;
public:
	CXTPResourceImageList();
	virtual ~CXTPResourceImageList();

	CXTPResourceImage* GetBitmap(UINT nID);

	CXTPResourceImage* SetBitmap(HBITMAP hBitmap, UINT nID, BOOL bAlptha, BOOL bCopyBitmap = TRUE);

	BOOL SetBitmap(CXTPResourceImage* pImage, UINT nID, BOOL bCallAddRef);

	BOOL LoadBitmap(LPCTSTR lpcszPath, UINT nID);

	BOOL Remove(UINT nID);
	void RemoveAll();

protected:
	CMap<UINT, UINT, CXTPResourceImage*, CXTPResourceImage*> m_mapID2Image;

protected:
};
