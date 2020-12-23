#pragma once

// CDialogDll 对话框

class CDialogDll : public CDialog
{
	DECLARE_DYNAMIC(CDialogDll)

public:
	CDialogDll(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogDll();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_DLL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_List_Dll;
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnColumnclickListDll(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRclickListDll(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFindcall();
};
