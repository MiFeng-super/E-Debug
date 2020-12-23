// DialogWin.cpp: 实现文件
//

#include "pch.h"
#include "E-Debug.h"
#include "DialogWin.h"
#include "afxdialogex.h"

#include "EAnalyEngine.h"
extern EAnalyEngine* g_pAnalyEngine;

#include "DialogMain.h"
extern CDialogMain* g_pDiaMain;

// CDialogWin 对话框

IMPLEMENT_DYNAMIC(CDialogWin, CDialog)

CDialogWin::CDialogWin(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_DIALOG_WIN, pParent)
{

}

CDialogWin::~CDialogWin()
{
	//
	// 在退出的时候，记得释放内存
	//
	for (size_t i = 0; i < m_Control_List.size(); i++)
	{
		for (size_t j = 0; j < m_Control_List[i].ControlInfo.size(); j++)
		{
			if (m_Control_List[i].ControlInfo[j] != NULL)
			{
				free(m_Control_List[i].ControlInfo[j]);
			}
		}
	}

	for (size_t i = 0; i < m_MemTemp.size(); i++)
	{
		delete[] m_MemTemp[i].p;
	}
	m_MemTemp.clear();
}

void CDialogWin::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_WIN, m_Tree_win);
	DDX_Control(pDX, IDC_STATIC_NAME, m_Static_Name);
	DDX_Control(pDX, IDC_LIST_CTRLINFO, m_CtrlInfo);
}


BEGIN_MESSAGE_MAP(CDialogWin, CDialog)
	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE_WIN, &CDialogWin::OnSelchangingTreeWin)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_CTRLINFO, &CDialogWin::OnRclickListCtrlinfo)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_CTRLINFO, &CDialogWin::OnCustomdrawListCtrlinfo)
	ON_WM_CLOSE()
	ON_COMMAND(ID_32775, &CDialogWin::OnSaveFile)
END_MESSAGE_MAP()


// CDialogWin 消息处理程序

void CDialogWin::OnClose()
{
	CDialog::OnClose();
}


BOOL CDialogWin::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwStyle = m_CtrlInfo.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT;
	dwStyle |= LVS_EX_GRIDLINES;
	m_CtrlInfo.SetExtendedStyle(dwStyle);

	m_CtrlInfo.InsertColumn(0, L"名称", LVCFMT_CENTER, 130);
	m_CtrlInfo.InsertColumn(1, L"信息", LVCFMT_CENTER, 150);

	m_Tree_win.SetLineColor(RGB(0, 0, 255));
	m_root = m_Tree_win.InsertItem(L"易语言窗口");
	m_Tree_win.SetItemImage(m_root, 58, 58);

	m_ico.Create(16, 16, ILC_COLOR32, 255, 255);
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON1));	// IP编辑框			0
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON2));	// Unknow			1
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON3));	// Window			2
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON4));	// 按钮				3
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON5));	// 编辑框			4
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON6));	// 标签				5
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON7));	// 表格				6
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON8));	// 超级按钮			7
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON9));	// 超级编辑框		8
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON10));	// 超级链接框		9
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON11));	// 超级列表框		10
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON12));	// 超文本浏览框		11
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON13));	// 打印机			12
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON14));	// 单选框			13
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON15));	// 动画框			14
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON16));	// 动画物体			15
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON17));	// 端口				16
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON18));	// 分隔条			17
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON19));	// 分组框			18
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON20));	// 服务器			19
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON21));	// 高级选择夹		20
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON22));	// 高级影像框		21
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON23));	// 工具条			22
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON24));	// 横向滚动条		23
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON25));	// 滑块条			24
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON26));	// 画板				25
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON27));	// 进度条			26
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON28));	// 卷帘菜单			27
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON29));	// 客户				28
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON30));	// 列表框			29
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON31));	// 目录框			30
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON32));	// 气泡框			31
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON33));	// 驱动器框			32
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON34));	// 日期框			33
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON35));	// 时钟				34
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON36));	// 树型框			35
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON37));	// 数据报			36
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON38));	// 数据库提供者		37
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON39));	// 数据源			38
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON40));	// 调节器			39
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON41));	// 通用对话框		40
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON42));	// 通用提供者		41
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON43));	// 透明标签			42
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON44));	// 图片框			43
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON45));	// 图形按钮			44
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON46));	// 外部数据库		45
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON47));	// 外部数据库提供者	46
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON48));	// 外形框			47
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON49));	// 文件框			48
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON50));	// 选择夹			49
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON51));	// 选择框			50
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON52));	// 选择列表框		51
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON53));	// 颜色选择器		52
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON54));	// 影像框			53
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON55));	// 月历框			54
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON56));	// 状态条			55
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON57));	// 纵向滚动条		56
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON58));	// 组合框			57
	m_ico.Add(AfxGetApp()->LoadIcon(IDI_ICON_MAIN));// Main				58

	m_Tree_win.SetImageList(&m_ico, TVSIL_NORMAL);

	m_MousePointType[0] = "默认型";
	m_MousePointType[0x7F00] = "标准箭头型";
	m_MousePointType[0x7F01] = "文本编辑型";
	m_MousePointType[0x7F02] = "沙漏型";
	m_MousePointType[0x7F03] = "十字型";
	m_MousePointType[0x7F8B] = "箭头及问号型";
	m_MousePointType[0x7F8A] = "箭头及沙漏型";
	m_MousePointType[0x7F88] = "禁止符型";
	m_MousePointType[0x7F86] = "四向箭头型";
	m_MousePointType[0x7F83] = "北<->东箭头型";
	m_MousePointType[0x7F85] = "北<->南箭头型";
	m_MousePointType[0x7F82] = "北<->西箭头型";
	m_MousePointType[0x7F84] = "西<->东箭头型";
	m_MousePointType[0x7F04] = "向上箭头型";
	m_MousePointType[0x7F89] = "手型";
	m_MousePointType[0xFFFFFFFF] = "自定义型";

	m_borderType = { "无边框",
					"普通可调边框",
					"普通固定边框",
					"窄标题可调边框",
					"窄标题固定边框",
					"镜框式可调边框",
					"镜框式固定边框" };

	m_ImportMode = {"通常方式",
		"只读方式",
		"密码输入",
		"整数文本输入",
		"小数文本输入",
		"输入字节",
		"输入短整数",
		"输入整数",
		"输入长整数",
		"输入小数",
		"输入双精度小数",
		"输入日期时间"};


	if (!g_pAnalyEngine->m_isinit)
	{
		return TRUE;
	}

	int index;
	DWORD Curr;

	Curr = g_pAnalyEngine->m_pEntryInfo->dwEWindow;
	if (Curr == 0)
	{
		return TRUE;
	}

	index = g_pAnalyEngine->UpdatePage(Curr);
	if (index == PAGE_ERROR)
	{
		return TRUE;
	}

	Curr = g_pAnalyEngine->O2V(Curr, index);

	//
	// 没有窗口
	//
	m_WindowInfo.WindowCount = utils::GetDword(Curr) >> 3;
	if (m_WindowInfo.WindowCount == 0)
	{
		return TRUE;
	}

	CString WinID;
	CString ControlID;

	//
	// 保存窗口ID
	//
	for (size_t i = 0; i < m_WindowInfo.WindowCount; i++)
	{
		Curr += sizeof(DWORD);
		m_WindowInfo.WindowID.push_back(utils::GetDword(Curr));
		
		HTREEITEM temp;
		WinID.Format(L"0x%08X", utils::GetDword(Curr));
		temp = m_Tree_win.InsertItem(WinID, m_root);
		m_WindowInfo.WindowItem.push_back(temp);
		m_Tree_win.SetItemImage(temp, 58, 58);
	}


	Curr += (sizeof(DWORD) * (m_WindowInfo.WindowCount + 1));	// 跳过 WindowAddr

	//
	// 解析窗口信息
	//
	for (size_t i = 0; i < m_WindowInfo.WindowCount; i++)
	{
		DWORD Next = Curr;
		Curr += 0x10;
		ECONTROLLIST CotrolList = { 0 };

		CotrolList.ControlCount = utils::GetDword((Curr += 4) - 4);
		CotrolList.ControlSize = utils::GetDword((Curr += 4) - 4);

		//
		// 保存控件ID
		//
		for (size_t j = 0; j < CotrolList.ControlCount; j++)
		{
			CotrolList.ControlID.push_back(utils::GetDword(Curr));
			
			/*ControlID.Format(L"0x%08X", utils::GetDword(Curr));
			m_Tree_win.InsertItem(ControlID, m_WindowInfo.WindowItem[i]);*/
			
			Curr += 4;
		}

		//
		// 保存控件信息偏移
		//
		for (size_t j = 0; j < CotrolList.ControlCount; j++)
		{
			CotrolList.Controloffset.push_back(utils::GetDword(Curr));
			Curr += 4;
		}

		//
		// 计算控件信息大小
		//
		for (size_t j = 0; j < CotrolList.ControlCount - 1; j++)
		{
			DWORD dwSize;
			dwSize = CotrolList.Controloffset[j + 1] - CotrolList.Controloffset[j];
			CotrolList.ControlInfoSize.push_back(dwSize);
		}
		DWORD temp = CotrolList.ControlSize - 0xC;
		temp -= CotrolList.Controloffset[CotrolList.Controloffset.size() - 1];
		CotrolList.ControlInfoSize.push_back(temp);

		Curr += 4;	//跳过 Unknow3

		//
		// malloc 内存，保存控件信息
		//
		for (size_t j = 0; j < CotrolList.ControlCount; j++)
		{
			DWORD t_Curr;
			t_Curr = Curr + CotrolList.Controloffset[j];

			BYTE* p = (BYTE*)malloc(CotrolList.ControlInfoSize[j]);
			if (!p)
			{
				g_pDiaMain->Output("控件：0x%08X 申请内存失败，这可能不能显示信息");
			}
			else
			{
				memcpy(p, (VOID*)t_Curr, CotrolList.ControlInfoSize[j]);
			}
			CotrolList.ControlInfo.push_back(p);
		}
		Curr = Next + CotrolList.ControlSize + 0x18;
		m_Control_List.push_back(CotrolList);
	}

	TreeShowInfo();
	return TRUE;
}

void CDialogWin::TreeShowInfo() 
{
	HTREEITEM temp;
	CString ControlID;

	for (size_t i = 0; i < m_WindowInfo.WindowCount; i++)
	{
		for (size_t j = 0; j < m_Control_List[i].ControlID.size(); j++)
		{
			//
			// 插入节点
			//
			ControlID.Format(L"0x%08X", m_Control_List[i].ControlID[j]);
			temp = m_Tree_win.InsertItem(ControlID, m_WindowInfo.WindowItem[i]);
			m_Control_List[i].ControlItem.push_back(temp);

			//
			// 设置图标
			//

			DWORD CtrlType = utils::GetDword((DWORD)m_Control_List[i].ControlInfo[j]);

			//
			// 对节点设置上数据，方便后面显示数据
			//

			m_Tree_win.SetItemData(temp, (i << 16) + j);

			TreeSetIco(CtrlType, temp);
		}
	}



	//
	// 展开
	//
	for (size_t i = 0; i < m_WindowInfo.WindowItem.size(); i++)
	{
		m_Tree_win.Expand(m_WindowInfo.WindowItem[i], TVE_EXPAND);
	}
	m_Tree_win.Expand(m_root, TVE_EXPAND);
}

void CDialogWin::TreeSetIco(DWORD CtrlType, HTREEITEM node) 
{
	switch (CtrlType)
	{
	case 0x10001:  //窗体
		m_Tree_win.SetItemImage(node, 2, 2);
		break;
	case 0x10005: //编辑框
		m_Tree_win.SetItemImage(node, 4, 4);
		break;
	case 0x10006: //图片框
		m_Tree_win.SetItemImage(node, 43, 43);
		break;
	case 0x10007: //外形框
		m_Tree_win.SetItemImage(node, 47, 47);
		break;
	case 0x10008: //画板
		m_Tree_win.SetItemImage(node, 25, 25);
		break;
	case 0x10009: //分组框
		m_Tree_win.SetItemImage(node, 18, 18);
		break;
	case 0x1000A: //标签
		m_Tree_win.SetItemImage(node, 5, 5);
		break;
	case 0x1000B: //按钮
		m_Tree_win.SetItemImage(node, 3, 3);
		break;
	case 0x1000C: //选择框
		m_Tree_win.SetItemImage(node, 50, 50);
		break;
	case 0x1000D://单选框
		m_Tree_win.SetItemImage(node, 13, 13);
		break;
	case 0x1000E://组合框
		m_Tree_win.SetItemImage(node, 57, 57);
		break;
	case 0x1000F://列表框
		m_Tree_win.SetItemImage(node, 29, 29);
		break;
	case 0x10010://选择列表框
		m_Tree_win.SetItemImage(node, 51, 51);
		break;
	case 0x10011://横向滚动条
		m_Tree_win.SetItemImage(node, 23, 23);
		break;
	case 0x10012://纵向滚动条
		m_Tree_win.SetItemImage(node, 56, 56);
		break;
	case 0x10013://进度条
		m_Tree_win.SetItemImage(node, 26, 26);
		break;
	case 0x10014://滑块条
		m_Tree_win.SetItemImage(node, 24, 24);
		break;
	case 0x10015://选择夹
		m_Tree_win.SetItemImage(node, 49, 49);
		break;
	case 0x10016://影像框
		m_Tree_win.SetItemImage(node, 53, 53);
		break;
	case 0x10017://日期框
		m_Tree_win.SetItemImage(node, 33, 33);
		break;
	case 0x10018://月历
		m_Tree_win.SetItemImage(node, 54, 54);
		break;
	case 0x10019://驱动器框
		m_Tree_win.SetItemImage(node, 32, 32);
		break;
	case 0x1001A://目录框
		m_Tree_win.SetItemImage(node, 30, 30);
		break;
	case 0x1001B://文件框
		m_Tree_win.SetItemImage(node, 48, 48);
		break;
	case 0x1001C://颜色选择器
		m_Tree_win.SetItemImage(node, 52, 52);
		break;
	case 0x1001D://超级链接框
		m_Tree_win.SetItemImage(node, 9, 9);
		break;
	case 0x1001E://调节器
		m_Tree_win.SetItemImage(node, 39, 39);
		break;
	case 0x1001F://通用对话框
		m_Tree_win.SetItemImage(node, 40, 40);
		break;
	case 0x10020://时钟
		m_Tree_win.SetItemImage(node, 34, 34);
		break;
	case 0x10021://打印机
		m_Tree_win.SetItemImage(node, 12, 12);
		break;
	case 0x10024://数据报
		m_Tree_win.SetItemImage(node, 36, 36);
		break;
	case 0x10025://客户
		m_Tree_win.SetItemImage(node, 28, 28);
		break;
	case 0x10026://服务器
		m_Tree_win.SetItemImage(node, 19, 19);
		break;
	case 0x10027://端口
		m_Tree_win.SetItemImage(node, 16, 16);
		break;
	case 0x10029://表格
		m_Tree_win.SetItemImage(node, 6, 6);
		break;
	case 0x1002A://数据源
		m_Tree_win.SetItemImage(node, 38, 38);
		break;
	case 0x1002B://通用提供者
		m_Tree_win.SetItemImage(node, 41, 41);
		break;
	case 0x1002C://数据库提供者
		m_Tree_win.SetItemImage(node, 37, 37);
		break;
	case 0x1002D://图形按钮
		m_Tree_win.SetItemImage(node, 44, 44);
		break;
	case 0x1002E://外部数据库
		m_Tree_win.SetItemImage(node, 45, 45);
		break;
	case 0x1002F://外部数据库提供者
		m_Tree_win.SetItemImage(node, 46, 46);
		break;

	default://未知
		m_Tree_win.SetItemImage(node, 1, 1);
		break;
	}
}

//
// 控件树被点击
//
void CDialogWin::OnSelchangingTreeWin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	
	int i;
	int j;
	DWORD ControlType;
	static HTREEITEM CurrItem;

	if (!g_pAnalyEngine->m_isinit)
	{
		return;
	}

	if (m_Tree_win.GetChildItem(pNMTreeView->itemNew.hItem) == 0
		&& CurrItem != pNMTreeView->itemNew.hItem)
	{
		DWORD data = m_Tree_win.GetItemData(pNMTreeView->itemNew.hItem);
		i = HIWORD(data);
		j = LOWORD(data);

		m_CtrlInfo.DeleteAllItems();

		if (m_Control_List.empty())
		{
			return;
		}

		//
		// 没有信息，则直接返回
		//
		if (m_Control_List[i].ControlInfo[j] == NULL)
		{
			return;
		}
		
		//
		// 保存一下，当前解析控件。防止重复点击
		//
		CurrItem = pNMTreeView->itemNew.hItem;

		//
		// 在开始解析前，要把上次解析的临时数据，清空
		//
		for (size_t i = 0; i < m_MemTemp.size(); i++)
		{
			delete[] m_MemTemp[i].p;
		}
		m_MemTemp.clear();

		ControlType = utils::GetDword((DWORD)m_Control_List[i].ControlInfo[j]);
		switch (ControlType)
		{
		case 0x10001:	//窗体
			m_Static_Name.SetWindowText(L"窗体");
			Analy_Window(m_Control_List[i].ControlInfo[j], data);
			break;
		case 0x10005:	//编辑框
			m_Static_Name.SetWindowText(L"编辑框");
			Analy_Edit(m_Control_List[i].ControlInfo[j], data);
		default:
			break;
		}
	}
}

//
// 弹出菜单
//
void CDialogWin::OnRclickListCtrlinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	static CMenu Menu;
	static CMenu* pSubMenu;

	if (pSubMenu == NULL)
	{
		Menu.LoadMenu(IDR_MENU_WIN);
		pSubMenu = Menu.GetSubMenu(0);
	}

	DWORD data = m_CtrlInfo.GetItemData(pNMItemActivate->iItem);

	if (data != 0 && data <= m_MemTemp.size())
	{
		DWORD dwPos = GetMessagePos();
		CPoint p(LOWORD(dwPos), HIWORD(dwPos));
		pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, p.x, p.y, this);
	}

}

void CDialogWin::OnSaveFile()
{
	OPENFILENAME ofn = { 0 };
	TCHAR szFile[MAX_PATH] = { 0 };

	int pos = m_CtrlInfo.GetSelectionMark();
	if (pos == -1)
	{
		return;
	}

	DWORD data = m_CtrlInfo.GetItemData(pos) - 1;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner	= m_hWnd;
	ofn.lpstrFile	= szFile;
	ofn.nMaxFile	= MAX_PATH;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn))
	{
		CString szTemp;
		//
		// 保存文件
		//
		HANDLE hFile;
		hFile = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			szTemp.Format(L"保存文件失败 Error Code: 0x%X", GetLastError());
			::MessageBox(m_hWnd, szTemp, L"Error", MB_ICONERROR);
			return;
		}
		DWORD dwWritten;
		WriteFile(hFile, m_MemTemp[data].p, m_MemTemp[data].size, &dwWritten, NULL);
		CloseHandle(hFile);
	}
}


void CDialogWin::OnCustomdrawListCtrlinfo(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	if (CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		//奇数行   
		if (pLVCD->nmcd.dwItemSpec % 2)
			pLVCD->clrTextBk = RGB(255, 255, 128);
		//偶数行   
		else
			pLVCD->clrTextBk = RGB(128, 255, 255);
		//继续   
		*pResult = CDRF_DODEFAULT;
	}


}

//
// 窗口控件分析
//
void CDialogWin::Analy_Window(BYTE* CtrlInf, DWORD ItemData)
{
	USES_CONVERSION;
	string HelpFileName;
	string ClassName;
	string Caption;
	BYTE* BackPic = NULL;
	BYTE* icon = NULL;
	BYTE* BackMusic = NULL;
	BYTE* pMousePointer = NULL;

	MEMTEMP MemTemp;

	int i = HIWORD(ItemData);
	int j = LOWORD(ItemData);
	BYTE* t_CtrlInf = CtrlInf;

	Analy_WINDOW Analy = { 0 };

	//
	// 前面有 0x42 字节可以直接拷贝
	//
	memcpy(&Analy, t_CtrlInf, 0x42);	
	t_CtrlInf += 0x42;

	//
	// 鼠标指针
	//
	if (Analy.MousePointerSize > 4)
	{
		pMousePointer = new BYTE[Analy.MousePointerSize];
		memcpy(pMousePointer, t_CtrlInf, Analy.MousePointerSize);
		t_CtrlInf += Analy.MousePointerSize - 4;

		//
		// 这里 - 4，因为 MousePointerSize = sizeof(MousePointer) + customMousePointer
		// MousePointer 前面 memcpy 已经跳过了。所以这里 - 4
		//

	}


	//
	// 说明有 tag 标识
	//
	if (*t_CtrlInf != 0)
	{
		Analy.tag = (char*)t_CtrlInf;
		t_CtrlInf += Analy.tag.size();
	}
	t_CtrlInf++;		//跳过字符串后面的字节，即使没有 tag ，也要加 1

	t_CtrlInf += 4;		//跳过 ZeroByte1

	Analy.visible_disable = utils::GetDword((DWORD)t_CtrlInf) & 0x3;
	t_CtrlInf += 4;

	t_CtrlInf += 4;		//跳过 ZeroByte2

	Analy.UnknowSize = utils::GetDword((DWORD)t_CtrlInf);
	Analy.UnknowSize = Analy.UnknowSize * 8 + 0x18;

	t_CtrlInf += Analy.UnknowSize + 4;	//跳过 UnknowByte2

	if ((DWORD)t_CtrlInf - (DWORD)CtrlInf >= m_Control_List[i].ControlInfoSize[j])
	{
		//
		// 默认配置
		//
		Analy.border = 2;
		Analy.BackColor = 0xFF000000;
		Analy.BackPicMode = 1;
		Analy.MinBtn = 1;
		Analy.CtrlBtn = 1;
		Analy.pos = 1;
		Analy.CanMove = 1;
		Analy.EscClose = 1;
		Analy.F1OpenHelp = 0;
		Analy.ShowInTaskBar = 1;

		goto labelShow;
	}

	//
	// 固定的，直接拷贝
	//
	memcpy(&Analy.border, t_CtrlInf, 0x48);
	t_CtrlInf += 0x48;


	Analy.ClassNameSize = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 0x4;
	if (Analy.ClassNameSize != 0)
	{
		//
		// 有窗口类名
		//
		char* p = new char[Analy.ClassNameSize + 1];
		memset(p, 0, Analy.ClassNameSize + 1);
		memcpy(p, t_CtrlInf, Analy.ClassNameSize);
		ClassName = p;
		delete[] p;
		t_CtrlInf += Analy.ClassNameSize;
	}

	Analy.BackPicSize = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 0x4;
	if (Analy.BackPicSize != 0)
	{
		//
		// 有底图
		//
		BackPic = new BYTE[Analy.BackPicSize];
		memcpy(BackPic, t_CtrlInf, Analy.BackPicSize);
		t_CtrlInf += Analy.BackPicSize;
	}

	Analy.iconSize = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 0x4;
	if (Analy.iconSize != 0)
	{
		//
		// 有图标
		//
		icon = new BYTE[Analy.iconSize];
		memcpy(icon, t_CtrlInf, Analy.iconSize);
		t_CtrlInf += Analy.iconSize;
	}

	Analy.BackMusicSize = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 0x4;
	if (Analy.BackMusicSize != 0)
	{
		//
		// 有背景音乐
		//
		BackMusic = new BYTE[Analy.BackMusicSize];
		memcpy(BackMusic, t_CtrlInf, Analy.BackMusicSize);
		t_CtrlInf += Analy.BackMusicSize;
	}

	Analy.captionSize = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 0x4;
	if (Analy.captionSize != 0)
	{
		//
		// 有窗口标题
		//
		char* p = new char[Analy.captionSize + 1];
		memset(p, 0, Analy.captionSize + 1);
		memcpy(p, t_CtrlInf, Analy.captionSize);
		Caption = p;
		delete[] p;
		t_CtrlInf += Analy.captionSize;
	}

	Analy.HelpFileNameSize = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 0x4;
	if (Analy.HelpFileNameSize != 0)
	{
		//
		// 有帮助文件
		//
		char* p = new char[Analy.HelpFileNameSize + 1];
		memset(p, 0, Analy.HelpFileNameSize + 1);
		memcpy(p, t_CtrlInf, Analy.HelpFileNameSize);
		HelpFileName = p;
		delete[] p;
		t_CtrlInf += Analy.HelpFileNameSize;
	}
labelShow:
	m_CtrlInfo.InsertItem(0, L"名称");
	m_CtrlInfo.InsertItem(1, L"备注");
	m_CtrlInfo.InsertItem(2, L"左边");
	m_CtrlInfo.InsertItem(3, L"顶边");
	m_CtrlInfo.InsertItem(4, L"宽度");
	m_CtrlInfo.InsertItem(5, L"高度");
	m_CtrlInfo.InsertItem(6, L"标记");
	m_CtrlInfo.InsertItem(7, L"可视");
	m_CtrlInfo.InsertItem(8, L"禁止");
	m_CtrlInfo.InsertItem(9, L"鼠标指针");
	m_CtrlInfo.InsertItem(10, L"标题");
	m_CtrlInfo.InsertItem(11, L"边框");
	m_CtrlInfo.InsertItem(12, L"底色");
	m_CtrlInfo.InsertItem(13, L"底图");
	m_CtrlInfo.InsertItem(14, L"    底图方式");
	m_CtrlInfo.InsertItem(15, L"背景音乐");
	m_CtrlInfo.InsertItem(16, L"    播放次数");
	m_CtrlInfo.InsertItem(17, L"控制按钮");
	m_CtrlInfo.InsertItem(18, L"    最大化按钮");
	m_CtrlInfo.InsertItem(19, L"    最小化按钮");
	m_CtrlInfo.InsertItem(20, L"位置");
	m_CtrlInfo.InsertItem(21, L"可否移动");
	m_CtrlInfo.InsertItem(22, L"图标");
	m_CtrlInfo.InsertItem(23, L"回车下移焦点");
	m_CtrlInfo.InsertItem(24, L"Esc键关闭");
	m_CtrlInfo.InsertItem(25, L"F1打开帮助");
	m_CtrlInfo.InsertItem(26, L"    帮助文件名");
	m_CtrlInfo.InsertItem(27, L"    帮助标志值");
	m_CtrlInfo.InsertItem(28, L"在任务条中显示");
	m_CtrlInfo.InsertItem(29, L"随意移动");
	m_CtrlInfo.InsertItem(30, L"外形");
	m_CtrlInfo.InsertItem(31, L"总在最前");
	m_CtrlInfo.InsertItem(32, L"保持标题条激活");
	m_CtrlInfo.InsertItem(33, L"窗口类名");
	
	CString num;
	num.Format(L"%d", Analy.left);
	m_CtrlInfo.SetItemText(2, 1, num);
	num.Format(L"%d", Analy.top);
	m_CtrlInfo.SetItemText(3, 1, num);
	num.Format(L"%d", Analy.width);
	m_CtrlInfo.SetItemText(4, 1, num);
	num.Format(L"%d", Analy.height);
	m_CtrlInfo.SetItemText(5, 1, num);

	if (Analy.tag.size() > 0)
	{
		m_CtrlInfo.SetItemText(6, 1, A2W(Analy.tag.c_str()));
	}
	

	m_CtrlInfo.SetItemText(7, 1, Analy.visible_disable & 1 ? L"真" : L"假");
	m_CtrlInfo.SetItemText(8, 1, Analy.visible_disable & 2 ? L"真" : L"假");

	if (!m_MousePointType[Analy.MousePointer].empty())
	{
		m_CtrlInfo.SetItemText(9, 1, 
			A2W(m_MousePointType[Analy.MousePointer].c_str()));

		if (Analy.MousePointer == 0xFFFFFFFF)
		{
			MemTemp.size = Analy.MousePointerSize - 4;
			MemTemp.p = pMousePointer;
			m_MemTemp.push_back(MemTemp);
			m_CtrlInfo.SetItemData(9, m_MemTemp.size());
		}

	}

	if (!Caption.empty())
	{
		m_CtrlInfo.SetItemText(10, 1,
			A2W(Caption.c_str()));
	}

	if (Analy.border <= 6)
	{
		m_CtrlInfo.SetItemText(11, 1,
			A2W(m_borderType[Analy.border].c_str()));
	}

	CString Temp;
	Temp.Format(L"0x%08X", Analy.BackColor);
	m_CtrlInfo.SetItemText(12, 1,Temp);

	if (BackPic)
	{
		MemTemp.size = Analy.BackColor;
		MemTemp.p = BackPic;
		m_CtrlInfo.SetItemText(13, 1, L"有数据");
		m_MemTemp.push_back(MemTemp);
		m_CtrlInfo.SetItemData(13, m_MemTemp.size());
	}

	Temp = "";
	switch (Analy.BackPicMode)
	{
	case 0:
		Temp = "图片居左上";
		break;
	case 1:
		Temp = "图片平铺";
		break;
	case 2:
		Temp = "图片居中";
		break;
	case 3:
		Temp = "缩放图片";
		break;
	}
	m_CtrlInfo.SetItemText(14, 1, Temp);

	if (BackMusic)
	{
		MemTemp.size = Analy.BackMusicSize;
		MemTemp.p = BackMusic;
		m_CtrlInfo.SetItemText(15, 1, L"有数据");
		m_MemTemp.push_back(MemTemp);
		m_CtrlInfo.SetItemData(15, m_MemTemp.size());
	}

	Temp = "";
	switch (Analy.PlayCount)
	{
	case 0:
		Temp = "循环播放";
		break;
	case 1:
		Temp = "仅播放1次";
		break;
	case 2:
		Temp = "不播放";
		break;
	}
	m_CtrlInfo.SetItemText(16, 1, Temp);

	m_CtrlInfo.SetItemText(17, 1, Analy.CtrlBtn ? L"真" : L"假");
	m_CtrlInfo.SetItemText(18, 1, Analy.MaxBtn ? L"真" : L"假");
	m_CtrlInfo.SetItemText(19, 1, Analy.MinBtn ? L"真" : L"假");

	Temp = "";
	switch (Analy.pos)
	{
	case 0:
		Temp = "通常";
		break;
	case 1:
		Temp = "居中";
		break;
	case 2:
		Temp = "最小化";
		break;
	case 3:
		Temp = "最大化";
		break;
	}
	m_CtrlInfo.SetItemText(20, 1, Temp);

	m_CtrlInfo.SetItemText(21, 1, Analy.CanMove ? L"真" : L"假");

	if (icon)
	{
		MemTemp.size = Analy.iconSize;
		MemTemp.p = icon;
		m_CtrlInfo.SetItemText(22, 1, L"有数据");
		m_MemTemp.push_back(MemTemp);
		m_CtrlInfo.SetItemData(22, m_MemTemp.size());
	}

	m_CtrlInfo.SetItemText(23, 1, Analy.EnterToNext ? L"真" : L"假");
	m_CtrlInfo.SetItemText(24, 1, Analy.EscClose ? L"真" : L"假");
	m_CtrlInfo.SetItemText(25, 1, Analy.F1OpenHelp ? L"真" : L"假");

	if (Analy.F1OpenHelp)
	{
		if (!HelpFileName.empty())
		{
			m_CtrlInfo.SetItemText(26, 1, A2W(HelpFileName.c_str()));
		}
		Temp.Format(L"%d", Analy.HelpContext);
		m_CtrlInfo.SetItemText(27, 1, Temp);
	}

	m_CtrlInfo.SetItemText(28, 1, Analy.ShowInTaskBar ? L"真" : L"假");
	m_CtrlInfo.SetItemText(29, 1, Analy.HitMove ? L"真" : L"假");
	Temp.Format(L"%d", Analy.shape);
	m_CtrlInfo.SetItemText(30, 1, Temp);

	m_CtrlInfo.SetItemText(31, 1, Analy.Topmost ? L"真" : L"假");
	m_CtrlInfo.SetItemText(32, 1, Analy.KeepTitleBarActive ? L"真" : L"假");

	if (!ClassName.empty())
	{
		m_CtrlInfo.SetItemText(33, 1, A2W(ClassName.c_str()));
	}

}

//
// 编辑框分析
//
void CDialogWin::Analy_Edit(BYTE* CtrlInf, DWORD ItemData)
{
	USES_CONVERSION;
	string Content;
	BYTE* pMousePointer = NULL;

	int i = HIWORD(ItemData);
	int j = LOWORD(ItemData);
	BYTE* t_CtrlInf = CtrlInf;

	Analy_EDIT Analy = { 0 };

	memcpy(&Analy, t_CtrlInf, 0x18);
	t_CtrlInf += 0x18;

	Analy.CtrlName = (char*)t_CtrlInf;
	t_CtrlInf += Analy.CtrlName.size() + 1;

	t_CtrlInf += 0x5;	//跳过 UnknowByte1 

	memcpy(&Analy.left, t_CtrlInf, 0x24);
	t_CtrlInf += 0x24;

	if (Analy.MousePointerSize > 4)
	{
		pMousePointer = new BYTE[Analy.MousePointerSize];
		memcpy(pMousePointer, t_CtrlInf, Analy.MousePointerSize);
		t_CtrlInf += Analy.MousePointerSize - 4;
	}

	if (*(char*)t_CtrlInf != 0)
	{
		Analy.tag = (char*)t_CtrlInf;
		t_CtrlInf += Analy.tag.size();
	}
	t_CtrlInf++;

	t_CtrlInf += 4;	//跳过 ZeroByte1

	Analy.vis_dis_stopfoc = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 4;

	Analy.Stoprder = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 4;

	Analy.UnknowSize = utils::GetDword((DWORD)t_CtrlInf);
	Analy.UnknowSize = Analy.UnknowSize * 8 + 0x18;

	t_CtrlInf += Analy.UnknowSize + 4;	//跳过 UnknowByte3

	memcpy(&Analy.borde, t_CtrlInf, 0x3A);
	t_CtrlInf += 0x3A;

	Analy.FontSize = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 4;

	//
	// 先不分析，字体
	//
	if (Analy.FontSize != 0)
	{
		t_CtrlInf += Analy.FontSize;
	}

	Analy.ContentSize = utils::GetDword((DWORD)t_CtrlInf);
	t_CtrlInf += 4;
	if (Analy.ContentSize != 0)
	{
		char* p = new char[Analy.ContentSize + 1];
		memset(p, 0, Analy.ContentSize + 1);
		memcpy(p, t_CtrlInf, Analy.ContentSize);
		Content = p;
		delete[] p;
		t_CtrlInf += Analy.ContentSize;
	}

	m_CtrlInfo.InsertItem(0, L"名称");
	m_CtrlInfo.InsertItem(1, L"备注");
	m_CtrlInfo.InsertItem(2, L"左边");
	m_CtrlInfo.InsertItem(3, L"顶边");
	m_CtrlInfo.InsertItem(4, L"宽度");
	m_CtrlInfo.InsertItem(5, L"高度");
	m_CtrlInfo.InsertItem(6, L"标记");
	m_CtrlInfo.InsertItem(7, L"可视");
	m_CtrlInfo.InsertItem(8, L"禁止");
	m_CtrlInfo.InsertItem(9, L"鼠标指针");
	m_CtrlInfo.InsertItem(10, L"可停留焦点");
	m_CtrlInfo.InsertItem(11, L"    停留顺序");
	m_CtrlInfo.InsertItem(12, L"内容");
	m_CtrlInfo.InsertItem(13, L"边框");
	m_CtrlInfo.InsertItem(14, L"文本颜色");
	m_CtrlInfo.InsertItem(15, L"背景颜色");
	m_CtrlInfo.InsertItem(16, L"字体");
	m_CtrlInfo.InsertItem(17, L"隐藏选择");
	m_CtrlInfo.InsertItem(18, L"最大允许长度");
	m_CtrlInfo.InsertItem(19, L"是否允许多行");
	m_CtrlInfo.InsertItem(20, L"滚动条");
	m_CtrlInfo.InsertItem(21, L"对齐方式");
	m_CtrlInfo.InsertItem(22, L"输入方式");
	m_CtrlInfo.InsertItem(23, L"    密码遮盖字符");
	m_CtrlInfo.InsertItem(24, L"转换方式");
	m_CtrlInfo.InsertItem(25, L"调节器方式");
	m_CtrlInfo.InsertItem(26, L"调节器底限值");
	m_CtrlInfo.InsertItem(27, L"调节器上限值");
	m_CtrlInfo.InsertItem(28, L"起始选择位置");
	m_CtrlInfo.InsertItem(29, L"被选择字符数");


	if (!Analy.CtrlName.empty())
	{
		m_CtrlInfo.SetItemText(0, 1, A2W(Analy.CtrlName.c_str()));
	}

	CString temp;
	temp.Format(L"%d", Analy.left);
	m_CtrlInfo.SetItemText(2, 1, temp);
	temp.Format(L"%d", Analy.top);
	m_CtrlInfo.SetItemText(3, 1, temp);
	temp.Format(L"%d", Analy.width);
	m_CtrlInfo.SetItemText(4, 1, temp);
	temp.Format(L"%d", Analy.height);
	m_CtrlInfo.SetItemText(5, 1, temp);

	if (!Analy.tag.empty())
	{
		m_CtrlInfo.SetItemText(6, 1, A2W(Analy.tag.c_str()));
	}

	m_CtrlInfo.SetItemText(7, 1, Analy.vis_dis_stopfoc & 1 ? L"真" : L"假");
	m_CtrlInfo.SetItemText(8, 1, Analy.vis_dis_stopfoc & 2 ? L"真" : L"假");
	m_CtrlInfo.SetItemText(10, 1, Analy.vis_dis_stopfoc & 4 ? L"真" : L"假");

	if (Analy.MousePointer != 0xFFFFFFFF)
	{
		m_CtrlInfo.SetItemText(9, 1,
			A2W(m_MousePointType[Analy.MousePointer].c_str()));
	}

	temp.Format(L"%d", Analy.Stoprder);
	m_CtrlInfo.SetItemText(11, 1, temp);

	if (!Content.empty())
	{
		m_CtrlInfo.SetItemText(12, 1, A2W(Content.c_str()));
	}

	m_CtrlInfo.SetItemText(13, 1, A2W(m_borderType[Analy.borde].c_str()));

	temp.Format(L"0x%08X", Analy.TextColor);
	m_CtrlInfo.SetItemText(14, 1, temp);
	temp.Format(L"0x%08X", Analy.BackColor);
	m_CtrlInfo.SetItemText(15, 1, temp);

	//
	// 字体跳过
	//
	
	m_CtrlInfo.SetItemText(17, 1, Analy.HideSelect ? L"真" : L"假");

	temp.Format(L"%d", Analy.MaxSize);
	m_CtrlInfo.SetItemText(18, 1, temp);

	m_CtrlInfo.SetItemText(19, 1, Analy.IsMoreLine ? L"真" : L"假");

	temp = "";
	switch (Analy.ScrollBar)
	{
	case 0:
		temp = "无";
		break;
	case 1:
		temp = "横向滚动条";
		break;
	case 2:
		temp = "纵向滚动条";
		break;
	case 3:
		temp = "横线及纵向";
		break;
	}
	m_CtrlInfo.SetItemText(20, 1, temp);

	temp = "";
	switch (Analy.Alignment)
	{
	case 0:
		temp = "左对齐";
		break;
	case 1:
		temp = "居中";
		break;
	case 2:
		temp = "右对齐";
		break;
	}
	m_CtrlInfo.SetItemText(21, 1, temp);

	m_CtrlInfo.SetItemText(22, 1, A2W(m_ImportMode[Analy.ImportMode].c_str()));

	m_CtrlInfo.SetItemText(23, 1, (WCHAR*)&Analy.PassCoverChar);

	temp = "";
	switch (Analy.transition)
	{
	case 0:
		temp = "无";
		break;
	case 1:
		temp = "大写 -> 小写";
		break;
	case 2:
		temp = "小写 -> 大写";
		break;
	}
	m_CtrlInfo.SetItemText(24, 1, temp);

	temp = "";
	switch (Analy.Regulator)
	{
	case 0:
		temp = "无调节器";
		break;
	case 1:
		temp = "自动调节器";
		break;
	case 2:
		temp = "手动调节器";
		break;
	}
	m_CtrlInfo.SetItemText(25, 1, temp);

	temp.Format(L"%d", Analy.RegulatorMin);
	m_CtrlInfo.SetItemText(26, 1, temp);
	temp.Format(L"%d", Analy.RegulatorMax);
	m_CtrlInfo.SetItemText(27, 1, temp);
	temp.Format(L"%d", Analy.StartSelPos);
	m_CtrlInfo.SetItemText(28, 1, temp);
	temp.Format(L"%d", Analy.BeSelCharNum);
	m_CtrlInfo.SetItemText(29, 1, temp);

}