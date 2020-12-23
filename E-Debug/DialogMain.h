#pragma once

#include "DialogLib.h"
#include "DialogDll.h"
#include "DialogWin.h"

// CDialogMain 对话框

class CDialogMain : public CDialog
{
	DECLARE_DYNAMIC(CDialogMain)

public:
	CDialogMain(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDialogMain();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MAIN };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CDialogLib m_Dialog_lib;
	CDialogDll m_Dialog_Dll;
	CDialogWin m_Dialog_Win;

	CTabCtrl m_Tab_1;
	CListBox m_List_log;
	virtual BOOL OnInitDialog();
	afx_msg void OnTrnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLbnSelListLog();
	afx_msg void OnClose();

	int Output(char* format, ...);

};
