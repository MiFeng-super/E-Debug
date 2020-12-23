// DialogDll.cpp: 实现文件
//

#include "pch.h"
#include "E-Debug.h"
#include "DialogDll.h"
#include "afxdialogex.h"

#include "EAnalyEngine.h"
extern EAnalyEngine* g_pAnalyEngine;

#include "DialogMain.h"
extern CDialogMain* g_pDiaMain;

// CDialogDll 对话框

IMPLEMENT_DYNAMIC(CDialogDll, CDialog)

CDialogDll::CDialogDll(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_DLL, pParent)
{

}

CDialogDll::~CDialogDll()
{
}

void CDialogDll::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DLL, m_List_Dll);
}


BEGIN_MESSAGE_MAP(CDialogDll, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DLL, &CDialogDll::OnColumnclickListDll)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_DLL, &CDialogDll::OnRclickListDll)
	ON_COMMAND(ID_FINDCALL, &CDialogDll::OnFindcall)
END_MESSAGE_MAP()


// CDialogDll 消息处理程序


void CDialogDll::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}


BOOL CDialogDll::OnInitDialog()
{
	USES_CONVERSION;
	CDialog::OnInitDialog();

	//
	// 设置列表样式
	//
	DWORD dwStyle = m_List_Dll.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_List_Dll.SetExtendedStyle(dwStyle);

	dwStyle = m_List_Dll.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_List_Dll.SetExtendedStyle(dwStyle);

	m_List_Dll.InsertColumn(0, L"序号", LVCFMT_LEFT, 50);
	m_List_Dll.InsertColumn(1, L"DLL库", LVCFMT_CENTER, 90);
	m_List_Dll.InsertColumn(2, L"命令名称", LVCFMT_CENTER, 280);
	m_List_Dll.InsertColumn(3, L"引用次数", LVCFMT_CENTER, 80);

	//
	// 特征码
	//
	byte TraitCode[] = { 0xB8, 0, 0, 0, 0, 0xE8 };

	if (g_pAnalyEngine->m_isinit)
	{
		if (g_pAnalyEngine->m_pEntryInfo->pApiName != NULL)
		{
			char* pDllName;
			char* pApiName;
			DWORD DllName;
			DWORD ApiName;
			CString str;

			int index = g_pAnalyEngine->UpdatePage(g_pAnalyEngine->m_pEntryInfo->pLibName);
			if (index == PAGE_ERROR)
			{
				g_pDiaMain->Output("API 命令,,,致命错误！！！");
				return TRUE;
			}

			DllName = g_pAnalyEngine->O2V(g_pAnalyEngine->m_pEntryInfo->pLibName, index);
			ApiName = g_pAnalyEngine->O2V(g_pAnalyEngine->m_pEntryInfo->pApiName, index);

			for (size_t i = 0; i < g_pAnalyEngine->m_pEntryInfo->dwApiCount; i++)
			{
				pDllName = (char*)g_pAnalyEngine->O2V(utils::GetDword(DllName), index);
				pApiName = (char*)g_pAnalyEngine->O2V(utils::GetDword(ApiName), index);

				if (*(byte*)pDllName == NULL)
				{
					pDllName = "NULL";
				}
				str.Empty();
				str.Format(L"%d", i);
				m_List_Dll.InsertItem(i, str);
				m_List_Dll.SetItemData(i, i);
				m_List_Dll.SetItemText(i, 1, A2W(pDllName));
				m_List_Dll.SetItemText(i, 2, A2W(pApiName));


				//
				// 查找引用
				//
				DWORD dwCount = 0;
				DWORD dwStart = g_pAnalyEngine->m_dwUserCodeStart;
				DWORD dwEnd = g_pAnalyEngine->m_dwUserCodeEnd;
				DWORD dwSize = dwEnd - dwStart;
				int index = g_pAnalyEngine->FindOldPage(dwStart);
				vector<DWORD > vAddr;

				//
				// 填充特征码
				//
				*(int*)(TraitCode + 1) = i;


				byte* pCurr = (byte*)g_pAnalyEngine->O2V(dwStart, index);
				while (1)
				{
					pCurr = utils::Search_Bin_Ex(pCurr, TraitCode, dwSize, sizeof(TraitCode));
					if (!pCurr)
					{
						break;
					}

					//
					// 判断一下 call xxxxx 目标地址是否正确
					//
					DWORD dwCallDest = utils::GetDword((DWORD)pCurr + sizeof(TraitCode)) + 
						(DWORD)pCurr + sizeof(TraitCode) - 1 + 5;

					// jmp [xxxx] 获取xxx
					dwCallDest = utils::GetDword(dwCallDest + 2);
					index = g_pAnalyEngine->FindOldPage(dwCallDest);

					//
					// 过滤掉非法的
					//
					if (index == PAGE_ERROR)
					{
						pCurr += sizeof(TraitCode);
						continue;
					}

					dwCallDest = utils::GetDword(g_pAnalyEngine->O2V(dwCallDest, index));

					if (dwCallDest == g_pAnalyEngine->m_CallDllCmd)
					{
						dwCount++;
						DWORD dwAddr = g_pAnalyEngine->V2O((DWORD)pCurr, 0);
						Insertname(dwAddr, NM_LABEL, pApiName);
						vAddr.push_back(dwAddr);
					}

					pCurr += sizeof(TraitCode);
				}

				//
				// 保存调用地址
				//
				g_CallApiMap[i] = vAddr;

				//
				// 设置 引用次数
				//
				str.Empty();
				str.Format(L"%d", dwCount);
				m_List_Dll.SetItemText(i, 3, str);

				DllName += sizeof(DWORD);
				ApiName += sizeof(DWORD);
			}
		}
		g_pDiaMain->Output("-> 获取易语言<DllCmd>完毕...");

	}
	return TRUE;  
}

//
// 对比DLL库
//
int CALLBACK CompareDll(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{
	CString a = g_pDiaMain->m_Dialog_Dll.m_List_Dll.GetItemText(lParam1, 1);
	CString b = g_pDiaMain->m_Dialog_Dll.m_List_Dll.GetItemText(lParam2, 1);

	return lParamSort == FALSE ? a.GetLength() > b.GetLength() : a.GetLength() < b.GetLength();
}

//
// 对比命令名称
//
int CALLBACK CompareCmd(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{
	CString a = g_pDiaMain->m_Dialog_Dll.m_List_Dll.GetItemText(lParam1, 2);
	CString b = g_pDiaMain->m_Dialog_Dll.m_List_Dll.GetItemText(lParam2, 2);

	return lParamSort == FALSE ? a.GetLength() > b.GetLength() : a.GetLength() < b.GetLength();
}

//
// 对比引用次数
//
int CALLBACK CompareCount(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) 
{
	int a = _ttoi(g_pDiaMain->m_Dialog_Dll.m_List_Dll.GetItemText(lParam1, 3));
	int b = _ttoi(g_pDiaMain->m_Dialog_Dll.m_List_Dll.GetItemText(lParam2, 3));
	return lParamSort == FALSE ? a > b : a < b;
}

//
// 排序
//
void CDialogDll::OnColumnclickListDll(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	static BOOL CountType;
	static BOOL CmdType;
	static BOOL DllType;

	switch (pNMLV->iSubItem)
	{
		//
		// 序号
		//
	case 0:
		break;

		//
		// DLL库
		//
	case 1:
		m_List_Dll.SortItemsEx(CompareDll, DllType);
		DllType = !DllType;
		break;

		//
		// 命令名称
		//
	case 2:
		m_List_Dll.SortItemsEx(CompareCmd, CmdType);
		CmdType = !CmdType;
		break;

		//
		// 引用次数
		//
	case 3:
		m_List_Dll.SortItemsEx(CompareCount, CountType);
		CountType = !CountType;
		break;
	}
}

//
// 弹出菜单
//
void CDialogDll::OnRclickListDll(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	static CMenu Menu;
	static CMenu* pSubMenu;

	if (pSubMenu == NULL)
	{
		Menu.LoadMenu(IDR_MENU_lib);
		pSubMenu = Menu.GetSubMenu(0);
	}

	if (pNMItemActivate->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint p(LOWORD(dwPos), HIWORD(dwPos));
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, p.x, p.y, this);
	}

}

//
// 查找引用
//
void CDialogDll::OnFindcall()
{
	USES_CONVERSION;
	int pos = m_List_Dll.GetSelectionMark();
	if (pos == -1)
	{
		return;
	}

	//
	// 清空 log bufs
	//
	g_pDiaMain->m_List_log.ResetContent();
	g_pDiaMain->Output("->执行命令   --==查找引用==--");

	//
	// api id
	//
	int id = m_List_Dll.GetItemData(pos);
	
	for (size_t i = 0; i < g_CallApiMap[id].size(); i++)
	{
		int InsertPos = g_pDiaMain->Output("%08X    mov eax,%08X    //%s",
			g_CallApiMap[id][i],
			id,
			W2A(m_List_Dll.GetItemText(pos, 2)));

		g_pDiaMain->m_List_log.SetItemData(InsertPos, g_CallApiMap[id][i]);

	}
}
