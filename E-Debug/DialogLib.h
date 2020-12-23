#pragma once

// CDialogLib 对话框

class CDialogLib : public CDialog
{
	DECLARE_DYNAMIC(CDialogLib)

public:
	CDialogLib(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogLib();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LIB };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List_lib;
	CListCtrl m_List_libCmd;

	void ShowLibInfo();
	void ShowLibCmdInfo(int LibIndex);

	virtual BOOL OnInitDialog();

	afx_msg void OnClose();
	afx_msg void OnClickListLib(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDBLclkListLibcmd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickListLibcmd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListLibcmd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFindcall();
};
