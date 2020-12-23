// DialogLib.cpp: 实现文件
//

#include "pch.h"
#include "E-Debug.h"
#include "DialogLib.h"
#include "afxdialogex.h"

#include "TrieTree.h"

#include "EAnalyEngine.h"
extern EAnalyEngine* g_pAnalyEngine;

#include "DialogMain.h"
extern CDialogMain* g_pDiaMain;

//
// 当前显示的支持库信息索引，方便 SetCpu
//
DWORD g_ShowLibindex = -1;

// CDialogLib 对话框

IMPLEMENT_DYNAMIC(CDialogLib, CDialog)

CDialogLib::CDialogLib(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_LIB, pParent)
{

}

CDialogLib::~CDialogLib()
{

}

void CDialogLib::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LIB, m_List_lib);
	DDX_Control(pDX, IDC_LIST_LIBCMD, m_List_libCmd);
}


BEGIN_MESSAGE_MAP(CDialogLib, CDialog)
	ON_WM_CLOSE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_LIB, &CDialogLib::OnClickListLib)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_LIBCMD, &CDialogLib::OnDBLclkListLibcmd)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_LIBCMD, &CDialogLib::OnColumnclickListLibcmd)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_LIBCMD, &CDialogLib::OnRclickListLibcmd)
	ON_COMMAND(ID_FINDCALL, &CDialogLib::OnFindcall)
END_MESSAGE_MAP()


// CDialogLib 消息处理程序


BOOL CDialogLib::OnInitDialog()
{
	CDialog::OnInitDialog();

	//
	// 设置列表样式
	//
	DWORD dwStyle = m_List_lib.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	m_List_lib.SetExtendedStyle(dwStyle);

	dwStyle = m_List_libCmd.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_List_libCmd.SetExtendedStyle(dwStyle);


	m_List_lib.InsertColumn(0, L"支持库信息", LVCFMT_LEFT, 250);
	m_List_libCmd.InsertColumn(0, L"地址", LVCFMT_LEFT, 80);
	m_List_libCmd.InsertColumn(1, L"命令名称", LVCFMT_CENTER, 205);


	if (g_pAnalyEngine->m_isinit == TRUE)
	{
		//
		// 标识 用户代码开始
		//

		Insertname(g_pAnalyEngine->m_dwUserCodeStart, NM_LABEL, "用户代码段开始");
		Insertname(g_pAnalyEngine->m_dwUserCodeEnd, NM_LABEL, "易语言程序入口");

		int temp;

		temp = g_pDiaMain->Output("->  用户自写代码段开始 : %08X", g_pAnalyEngine->m_dwUserCodeStart);
		g_pDiaMain->m_List_log.SetItemData(temp, g_pAnalyEngine->m_dwUserCodeStart);

		temp = g_pDiaMain->Output("->  用户自写代码段结束 : %08X", g_pAnalyEngine->m_dwUserCodeEnd);
		g_pDiaMain->m_List_log.SetItemData(temp, g_pAnalyEngine->m_dwUserCodeEnd);

		//////////////////////////////////////////////////////////////////////////

		//
		// 标识 FF25
		//

		vector<string> krnlCmd =
		{ "错误回调", "DLL命令", "三方支持库命令", "核心支持库命令",
			"读取组件属性", "设置组件属性", "分配内存", "重新分配内存",
			"释放内存", "结束", "窗口消息循环", "载入启动窗口", "初始化" };

		DWORD dwkrnlCmd;
		DWORD dwKrnlEntry = g_pAnalyEngine->m_pEntryInfo->dwEString;
		if (dwKrnlEntry == 0)
		{
			dwKrnlEntry = g_pAnalyEngine->m_pEntryInfo->dwEWindow;
		}

		int index = g_pAnalyEngine->UpdatePage(dwKrnlEntry);
		if (index == PAGE_ERROR)
		{
			return TRUE;
		}

		for (int i = krnlCmd.size() - 1; i >= 0; i--)
		{
			dwKrnlEntry -= sizeof(DWORD);
			dwkrnlCmd = utils::GetDword(g_pAnalyEngine->O2V(dwKrnlEntry, index));
			//
			// 获取到 krnl_MCallDllCmd
			//
			if (i == 1)
			{
				g_pAnalyEngine->m_CallDllCmd = dwkrnlCmd;
			}
			Insertname(dwkrnlCmd, NM_LABEL, (char*)krnlCmd[i].c_str());
		}
		g_pDiaMain->Output("-> 枚举易语言<_krln_func_>核心支持命令完毕...");
		//////////////////////////////////////////////////////////////////////////

		//
		// 开始识别命令
		//

		PLIB_INFO pLibInfo;
		DWORD dwLibEntry = g_pAnalyEngine->m_pEntryInfo->pLibEntey;
		index = g_pAnalyEngine->UpdatePage(dwLibEntry);

		if (index == PAGE_ERROR)
		{
			g_pDiaMain->Output("致命错误");
			return TRUE;
		}

		//
		// 循环对每个支持库处理
		//

		for (size_t i = 0; i < g_pAnalyEngine->m_pEntryInfo->dwLibNum; i++, dwLibEntry += 4)
		{
			pLibInfo = (PLIB_INFO)g_pAnalyEngine->O2V(g_pAnalyEngine->GetOldDword(dwLibEntry, index), index);
			string Name = (char*)g_pAnalyEngine->O2V((DWORD)pLibInfo->m_szName, index);
			string Guid = (char*)g_pAnalyEngine->O2V((DWORD)pLibInfo->m_szGuid, index);
			string Ver = to_string(pLibInfo->m_nMajorVersion) + "." + to_string(pLibInfo->m_nMinorVersion);

			LIBINFO libInfo;
			libInfo.Name = Name;
			libInfo.Guid = Guid;
			libInfo.Ver = Ver;

			string Path;
			for (size_t i = 0; i < g_EsigList.size(); i++)
			{
				if (g_EsigList[i].Guid == Guid &&
					g_EsigList[i].Ver == Ver)
				{
					Path = g_EsigList[i].Path;
					break;
				}
			}

			//
			// 加载 Esig
			//
			BOOL isLoad = FALSE;
			TrieTree Tree;
			if (!Path.empty())
			{
				isLoad = Tree.LoadEsig(Path.c_str());
			}

			DWORD pFunc = g_pAnalyEngine->O2V((DWORD)pLibInfo->m_pCmdsFunc, index);
			DWORD dwFunc;

			for (int i = 0; i < pLibInfo->m_nCmdCount; i++, pFunc += 4)
			{
				dwFunc = utils::GetDword(pFunc);
				CMDINFO cmd;
				cmd.FuncAddr = dwFunc;

				if (!isLoad)
				{
					cmd.FuncName = "Esig Not Founded";
					libInfo.cmd.push_back(cmd);
					continue;
				}
				//
				// 匹配，还存在问题。后续要修复，支持库文件有一些函数编译出来，偏移不一样，
				// 导致匹配失败，目前方法是：匹配失败的，按模糊匹配再匹配一次
				//
				char* pFunName = Tree.MatchEsig(g_pAnalyEngine->O2V(dwFunc, 0));

				if (pFunName)
				{
					cmd.FuncName = pFunName;
					Insertname(dwFunc, NM_LABEL, pFunName);
				}
				else
				{
					cmd.FuncName = "Error";
					Insertname(dwFunc, NM_LABEL, "未知命令");
				}
				libInfo.cmd.push_back(cmd);
			}

			g_LibInfo.push_back(libInfo);
		}
		g_pDiaMain->Output("-> 分析易语言<KrnlLib_Cmd>&&<Lib_Cmd>完毕...");
		ShowLibInfo();

	}
	else
	{
		//
		// 不是 静态编译
		//
		g_pDiaMain->Output("-> 该程序可能不是易语言静态编译程序");
	}


	return TRUE; 
}


//
// 显示支持库信息
//
void CDialogLib::ShowLibInfo()
{
	USES_CONVERSION;
	m_List_lib.DeleteAllItems();
	m_List_libCmd.DeleteAllItems();

	for (int i = g_LibInfo.size() - 1; i >= 0; i--)
	{
		CString Name;
		CString Guid;
		CString Cmd;
		Name.Format(L"--->%s(Ver:%s)", A2W(g_LibInfo[i].Name.c_str()), A2W(g_LibInfo[i].Ver.c_str()));
		Guid.Format(L"    %s", A2W(g_LibInfo[i].Guid.c_str()));
		Cmd.Format(L"  ->调用命令(总数:%d)", g_LibInfo[i].cmd.size());
		m_List_lib.InsertItem(0, Name);
		m_List_lib.InsertItem(1, Guid);
		m_List_lib.InsertItem(2, Cmd);
		m_List_lib.InsertItem(3, L"==============================");
	}

}

//
// 显示支持库命令信息
//
void CDialogLib::ShowLibCmdInfo(int LibIndex)
{
	USES_CONVERSION;

	m_List_libCmd.DeleteAllItems();

	CString str;
	for (size_t i = 0; i < g_LibInfo[LibIndex].cmd.size(); i++)
	{
		str.Format(L"%08X", g_LibInfo[LibIndex].cmd[i].FuncAddr);
		int npos = m_List_libCmd.InsertItem(i, str);
		m_List_libCmd.SetItemText(npos, 1, A2W(g_LibInfo[LibIndex].cmd[i].FuncName.c_str()));
	}

}

void CDialogLib::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialog::OnClose();
}

//
// 支持库列表点击
//
void CDialogLib::OnClickListLib(NMHDR* pNMHDR, LRESULT* pResult)
{
	USES_CONVERSION;
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	int pos = m_List_lib.GetSelectionMark();
	if (pos == -1)
	{
		return;
	}

	if (pos % 2 != 0)
	{
		return;
	}


	pos /= 4;
	
	//
	// 判断一下，当前show。防止重复显示
	//
	if (g_ShowLibindex != pos)
	{
		g_ShowLibindex = pos;
		//
		// 显示命令地址
		//
		ShowLibCmdInfo(pos);
	}
}

//
// 命令列表双击
//
void CDialogLib::OnDBLclkListLibcmd(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	int pos = m_List_libCmd.GetSelectionMark();
	if (pos == -1)
	{
		return;
	}

	//
	// 根据地址，控制 OD 反汇编窗口
	//
	Setcpu(0, g_LibInfo[g_ShowLibindex].cmd[pos].FuncAddr, 0, 0, CPU_NOFOCUS);

}

//
// 排序函数
//
BOOL g_SortWay;
bool SortCmdInfo(const CMDINFO& v1, const CMDINFO& v2)
{
	//
	// 升序 or 降序
	//
	return g_SortWay != FALSE ? (v1.FuncName.size() < v2.FuncName.size()) : (v1.FuncName.size() > v2.FuncName.size());
}

//
// 排序，按文本长度吧
//
void CDialogLib::OnColumnclickListLibcmd(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if (pNMLV->iSubItem != 1)
	{
		return;
	}

	g_SortWay = !g_SortWay;

	sort(g_LibInfo[g_ShowLibindex].cmd.begin(), g_LibInfo[g_ShowLibindex].cmd.end(), SortCmdInfo);

	ShowLibCmdInfo(g_ShowLibindex);
}

//
// 弹出菜单
//
void CDialogLib::OnRclickListLibcmd(NMHDR* pNMHDR, LRESULT* pResult)
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
void CDialogLib::OnFindcall()
{
	// TODO: 在此添加命令处理程序代码
	int pos = m_List_libCmd.GetSelectionMark();
	if (pos == -1)
	{
		return;
	}

	//
	// mov edx, xxxx
	//
	BYTE TraitCode[] = { 0xBB, 0, 0, 0, 0 };
	*(DWORD*)(TraitCode + 1) = g_LibInfo[g_ShowLibindex].cmd[pos].FuncAddr;

	//
	// 命令信息
	//
	CMDINFO cmdinfo;
	cmdinfo.FuncAddr = g_LibInfo[g_ShowLibindex].cmd[pos].FuncAddr;
	cmdinfo.FuncName = g_LibInfo[g_ShowLibindex].cmd[pos].FuncName;

	//
	// 特征码搜索信息
	//
	DWORD dwStart = g_pAnalyEngine->m_dwUserCodeStart;
	DWORD dwEnd = g_pAnalyEngine->m_dwUserCodeEnd;
	DWORD dwSize = dwEnd - dwStart;
	int index = g_pAnalyEngine->FindOldPage(dwStart);


	//
	// 清空 log bufs
	//
	g_pDiaMain->m_List_log.ResetContent();
	g_pDiaMain->Output("->执行命令   --==查找引用==--");


	byte* pCurr = (byte*)g_pAnalyEngine->O2V(dwStart, index);
	while (1)
	{
		pCurr = utils::Search_Bin_Ex(pCurr, TraitCode, dwSize, sizeof(TraitCode));
		if (!pCurr)
		{
			break;
		}

		DWORD dwAddr = g_pAnalyEngine->V2O((DWORD)pCurr, index);

		int insertpos = g_pDiaMain->Output(
			"%08X    mov ebx,%08X    //%s",
			dwAddr,
			cmdinfo.FuncAddr,
			cmdinfo.FuncName.c_str());

		g_pDiaMain->m_List_log.SetItemData(insertpos, dwAddr);

		pCurr += sizeof(TraitCode);
	}

}
