// DialogMain.cpp: 实现文件
//
#include "pch.h"
#include "E-Debug.h"
#include "DialogWin.h"
#include "DialogMain.h"
#include "afxdialogex.h"

#include "EAnalyEngine.h"
extern EAnalyEngine* g_pAnalyEngine;

// CDialogMain 对话框

vector<CDialog*> g_Teb_List;

IMPLEMENT_DYNAMIC(CDialogMain, CDialog)

CDialogMain::CDialogMain(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_MAIN, pParent)
{

}

CDialogMain::~CDialogMain()
{
}

void CDialogMain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_Tab_1);
	DDX_Control(pDX, IDC_LIST_LOG, m_List_log);
}


BEGIN_MESSAGE_MAP(CDialogMain, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CDialogMain::OnTrnSelchangeTab1)
	ON_LBN_SELCHANGE(IDC_LIST_LOG, &CDialogMain::OnLbnSelListLog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDialogMain 消息处理程序


BOOL CDialogMain::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	// 加载上图标
	//
	HICON icon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_MAIN));
	SendMessage(WM_SETICON, FALSE, (LPARAM)icon);
	SendMessage(WM_SETICON, TRUE, (LPARAM)icon);

	if (g_pAnalyEngine != NULL)
	{
		delete g_pAnalyEngine;
	}

	DWORD dwBase, dwSize;
	Getdisassemblerrange(&dwBase, &dwSize);

	Output("->开始分析当前区段....  分析地址: %08X  内存大小: %08X", dwBase, dwSize);

	g_pAnalyEngine = new EAnalyEngine(dwBase);


	CRect rc;
	m_Tab_1.GetClientRect(&rc);
	rc.top += 26;
	rc.left += 4;
	rc.right -= 6;
	rc.bottom -= 6;

	g_Teb_List.clear();

	m_Tab_1.InsertItem(0, L"支持库命令");
	m_Dialog_lib.Create(IDD_DIALOG_LIB, &m_Tab_1);
	m_Dialog_lib.MoveWindow(&rc);
	g_Teb_List.push_back(&m_Dialog_lib);

	m_Tab_1.InsertItem(1, L"API命令");
	m_Dialog_Dll.Create(IDD_DIALOG_DLL, &m_Tab_1);
	m_Dialog_Dll.MoveWindow(&rc);
	g_Teb_List.push_back(&m_Dialog_Dll);

	m_Tab_1.InsertItem(2, L"窗口组件");
	m_Dialog_Win.Create(IDD_DIALOG_WIN, &m_Tab_1);
	m_Dialog_Win.MoveWindow(&rc);
	g_Teb_List.push_back(&m_Dialog_Win);

	m_Dialog_lib.ShowWindow(SW_SHOW);
	m_Tab_1.SetCurSel(0);

	return TRUE;
}

//
// 切换窗口
//
void CDialogMain::OnTrnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	int CurSel = m_Tab_1.GetCurSel();

	for (size_t i = 0; i < g_Teb_List.size(); i++)
	{
		g_Teb_List[i]->ShowWindow(SW_HIDE);
	}
	g_Teb_List[CurSel]->ShowWindow(SW_SHOW);

	*pResult = 0;
}


void CDialogMain::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}

//
// LOG 单击
//
void CDialogMain::OnLbnSelListLog()
{
	int pos;
	DWORD data;

	pos = m_List_log.GetCurSel();
	data = m_List_log.GetItemData(pos);
	if (data != 0)
	{
		Setcpu(NULL, data, NULL, NULL, CPU_NOFOCUS);
	}
}

int CDialogMain::Output(char* format, ...) 
{
	USES_CONVERSION;

	va_list va;
	char buf[1000] = { 0 };

	va_start(va, format);
	vsprintf(buf, format, va);
	va_end(va);

	return m_List_log.InsertString(-1, A2W(buf));
}