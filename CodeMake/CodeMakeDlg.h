
// CodeMakeDlg.h: 头文件
//

#pragma once

#include <map>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;
// CCodeMakeDlg 对话框
class CCodeMakeDlg : public CDialogEx
{
// 构造
public:
	CCodeMakeDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CODEMAKE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
	void log(char* format, ...);
	void MakeCode(wchar_t* pFile);
	void GenerateRelocaMap(HMODULE hLib);
	void GenerateIatMap(HANDLE hLib);
	void GenerateFuncTrait(DWORD func, string& trait, BOOL isSubFunc);
	int  IsRelocation(ULONG address, int len);

	map<DWORD, BOOL> m_relocation;
	map<DWORD, string>m_Iatmap;
	vector<DWORD> m_subfunc;

	CListBox m_Outlog;
	CStatic m_Static_Name;
	CStatic m_Static_Ver;
	CStatic m_Static_Guid;
	CStatic m_Static_CmdCou;
};
