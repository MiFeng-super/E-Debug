
// CodeMakeDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CodeMake.h"
#include "CodeMakeDlg.h"
#include "afxdialogex.h"
#include "Elib.h"

#define BEA_ENGINE_STATIC   // 指明使用静态Lib库
#define BEA_USE_STDCALL     // 指明使用stdcall调用约定

#include "beaengine.h"
#pragma comment (lib,"legacy_stdio_definitions.lib")
#pragma comment(lib, "beaengine.lib")

#pragma warning(disable: 4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void ReadHex(ULONG addr, int len, string& text) 
{
	HANDLE hHeap = GetProcessHeap();

	byte* buffer = (byte*)HeapAlloc(hHeap, 0, len);
	char* szBuffer = (char*)HeapAlloc(hHeap, 0, len * 2 + 1);

	memset(buffer, 0, len);
	memset(szBuffer, 0, len * 2 + 1);

	memcpy(buffer, (char*)addr, len);

	for (int n = 0; n < len; n++) {
		sprintf(szBuffer + n * 2, "%02X", buffer[n]);
	}

	text = szBuffer;

	HeapFree(hHeap, 0, buffer);
	HeapFree(hHeap, 0, szBuffer);
}

// CCodeMakeDlg 对话框

CCodeMakeDlg::CCodeMakeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CODEMAKE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCodeMakeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_Outlog);
	DDX_Control(pDX, IDC_STATIC_LIBNAME, m_Static_Name);
	DDX_Control(pDX, IDC_STATIC_LIBVER, m_Static_Ver);
	DDX_Control(pDX, IDC_STATIC_GUID, m_Static_Guid);
	DDX_Control(pDX, IDC_STATIC_CMDCOU, m_Static_CmdCou);
}

BEGIN_MESSAGE_MAP(CCodeMakeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


// CCodeMakeDlg 消息处理程序

BOOL CCodeMakeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCodeMakeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCodeMakeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCodeMakeDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	WCHAR szPath[MAX_PATH] = { 0 };

	//
	// 获取拖放文件数量
	//
	int DropCount = DragQueryFileW(hDropInfo, -1, NULL, NULL);

	//
	// 对每个 fne 生成 Esig
	//
	for (int i = 0; i < DropCount; i++)
	{
		DragQueryFileW(hDropInfo, i, szPath, MAX_PATH);
		MakeCode(szPath);
	}

	DragFinish(hDropInfo);

	CDialogEx::OnDropFiles(hDropInfo);
}

void CCodeMakeDlg::MakeCode(wchar_t* pFile)
{
	USES_CONVERSION;
	CString strName;
	CString strVer;
	CString strGuid;
	CString strCmdCou;
	CString strFile(pFile);


	HMODULE hlib;
	PLIB_INFO pLibInfo;

	log(">>>>>>>> 开始生成 <<<<<<<<");

	if (strFile.Right(4).CompareNoCase(L".fne"))
	{
		log("请拖入 fne 文件");
		return;
	}

	hlib = LoadLibraryW(strFile);
	if (!hlib)
	{
		log("加载 fne 文件失败");
		return;
	}

	PFN_GET_LIB_INFO pGetNewInf = (PFN_GET_LIB_INFO)GetProcAddress(hlib, FUNCNAME_GET_LIB_INFO);
	if (!pGetNewInf)
	{
		log("导出接口 GetNewInf 不存在");
		return;
	}

	pLibInfo = pGetNewInf();

	//
	// 输出信息
	//
	strName.Format(L"%s", A2W((char*)pLibInfo->m_szName));
	m_Static_Name.SetWindowTextW(L"支持库：" + strName);
	strVer.Format(L"%1d.%1d", pLibInfo->m_nMajorVersion, pLibInfo->m_nMinorVersion);
	m_Static_Ver.SetWindowTextW(L"版本：" + strVer);
	strGuid.Format(L"%s", A2W((char*)pLibInfo->m_szGuid));
	m_Static_Guid.SetWindowTextW(L"GUID：" + strGuid);
	strCmdCou.Format(L"%d", pLibInfo->m_nCmdCount);
	m_Static_CmdCou.SetWindowTextW(L"命令数：" + strCmdCou);

	log(" ");
	log(" ");
	log("支持库：%s", pLibInfo->m_szName);
	log("版本：%ws", strVer);
	log("GUID：%ws", strGuid);
	log("命令数：%ws", strCmdCou);

	log(" ");
	log(" ");

	//
	// 开始生成 esig
	//
	string temp;

	PCMD_INFO pCmd = pLibInfo->m_pBeginCmdInfo;
	PULONG pCmdFunc = (PULONG)pLibInfo->m_pCmdsFunc;

	vector<string> vCmdName;

	for (int i = 0; i < pLibInfo->m_nCmdCount; i++)
	{
		vCmdName.push_back((char*)(pCmd + i)->m_szName);
	}

	PLIB_DATA_TYPE_INFO pDataTypeInfo = pLibInfo->m_pDataType;
	for (int i = 0; i < pLibInfo->m_nDataTypeCount; i++, pDataTypeInfo++)
	{
		LPINT pCmdIndex = pDataTypeInfo->m_pnCmdsIndex;
		if (pCmdIndex == NULL)
		{
			continue;
		}

		for (int j = 0; j < pDataTypeInfo->m_nCmdCount; j++, pCmdIndex++)
		{
			temp = (char*)pDataTypeInfo->m_szName;
			temp += ".";
			temp.append(vCmdName[*pCmdIndex]);
			vCmdName[*pCmdIndex] = temp;
		}
	}


	CString strEsig;
	CString strEsigDir;
	wchar_t wszCurPath[MAX_PATH] = { 0 };
	GetCurrentDirectoryW(MAX_PATH, wszCurPath);

	strEsigDir = wszCurPath;
	strEsigDir += L"\\Esig\\";
	strEsigDir += strGuid;
	strEsig = strEsigDir;
	strEsig += L"\\";
	strEsig += strVer + L".esig";

	//
	// 生成 重定和IAT 表，后续判断指令是否需要模糊处理
	//
	GenerateRelocaMap(hlib);
	GenerateIatMap(hlib);

	//
	// 创建 esig 文件
	//

	CreateDirectory(strEsigDir, NULL);
	HANDLE hFile;
	hFile = CreateFile(strEsig, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		log("创建 esig 文件失败。");
	}

	DWORD dwWritten;

	temp = (char*)pLibInfo->m_szName;
	temp += "\r\n";
	WriteFile(hFile, temp.c_str(), temp.size(), &dwWritten, NULL);

	temp = ">>>>Func start<<<<\r\n";
	WriteFile(hFile, temp.c_str(), temp.size(), &dwWritten, NULL);

	for (int i = 0; i < pLibInfo->m_nCmdCount; i++, pCmd++, pCmdFunc++)
	{
		//
		// 过滤掉无效命令
		//
		if (*pCmdFunc == NULL)
		{
			continue;
		}

		if (pCmd->m_wState == 32772)
		{
			continue;
		}

		if (vCmdName[i] == "打印机.取消打印")
		{
			int i = 0;
		}

		string trait;
		GenerateFuncTrait(*pCmdFunc, trait, FALSE);
		
		temp = vCmdName[i];
		temp += ":";
		temp += trait;
		temp += "\r\n";

		WriteFile(hFile, temp.c_str(), temp.size(), &dwWritten, NULL);

		log((char*)vCmdName[i].c_str());

	}

	temp = ">>>>Func end<<<<\r\n";
	WriteFile(hFile, temp.c_str(), temp.size(), &dwWritten, NULL);

	log(" ");

	temp = ">>>>Sub start<<<<\r\n";
	WriteFile(hFile, temp.c_str(), temp.size(), &dwWritten, NULL);

	for (int i = 0; i < m_subfunc.size(); i++)
	{
		string trait;
		GenerateFuncTrait(m_subfunc[i], trait, TRUE);

		temp = to_string(m_subfunc[i]);
		temp += ":";
		temp += trait;
		temp += "\r\n";

		WriteFile(hFile, temp.c_str(), temp.size(), &dwWritten, NULL);

		log("sub func:0x%X", m_subfunc[i]);
	}

	temp = ">>>>Sub end<<<<\r\n";
	WriteFile(hFile, temp.c_str(), temp.size(), &dwWritten, NULL);

	FreeLibrary(hlib);
	CloseHandle(hFile);
	m_relocation.clear();
	m_subfunc.clear();
	m_Iatmap.clear();


	log(">>>>>>>> 生成完毕 <<<<<<<<");
	log(">>>>>>>> 感谢使用 <<<<<<<<");
	log(" ");
	log(" ");


	//
	// CodeMake 基本完成，应该有BUG（后期改）。
	// 准备开发 E-Debug 
	//
}

//
// 生成函数特征，并记录 sub func
//
void CCodeMakeDlg::GenerateFuncTrait(DWORD func, string& trait, BOOL isSubFunc)
{
	DISASM asmdata = { 0 };
	asmdata.EIP = func;

	string strtemp;

	//
	// 特征长度，限制在 128
	//
	DWORD traitlen = 0;
	DWORD maxaddr = 0;

	for (int len = 0;; asmdata.EIP += len, traitlen += len)
	{
		strtemp = "";
		len = Disasm(&asmdata);

		if (len <= 0)
		{
			break;
		}

		if (traitlen >= 128)
		{
			return;
		}

		if (len >= 5)
		{
			int offset = IsRelocation(asmdata.EIP, len);
			if (offset != -1)
			{
				//
				// call iat
				//
				if (asmdata.Instruction.Opcode == 0xFF &&
					*(char*)(asmdata.EIP + 1) == 0x15)
				{
					if (!m_Iatmap[asmdata.Argument1.Memory.Displacement].empty()
						&& isSubFunc == FALSE)
					{
						trait.append("[" + m_Iatmap[asmdata.Argument1.Memory.Displacement] + "]");
						continue;
					}
				}


				//
				// jmp iat
				//
				if (asmdata.Instruction.Opcode == 0xFF &&
					*(char*)(asmdata.EIP + 1) == 0x25)
				{
					break;
				}

				ReadHex(asmdata.EIP, offset, strtemp);
				trait.append(strtemp + "????????");

				if (len - offset - 4 != 0)
				{
					ReadHex(asmdata.EIP + offset + 4, len - offset - 4, strtemp);
					trait.append(strtemp);
				}

				continue;
			}

			//
			// jmp je call 指令，长跳转
			//
			if(asmdata.Instruction.BranchType != 0 && asmdata.Instruction.AddrValue != 0)
			{
				//jmp 
				if (asmdata.Instruction.BranchType == JmpType)
				{
					trait.append(strtemp + "E9????????");
					if (isSubFunc)
					{
						traitlen += len;
						break;
					}
					//
					// jmp 则继续跟踪
					//
					asmdata.EIP = asmdata.Instruction.AddrValue - len;
					continue;
				}

				//call
				if (asmdata.Instruction.BranchType == CallType)
				{
					if (isSubFunc)
					{
						trait.append(strtemp + "E8????????");
						continue;
					}

					
					if (find(m_subfunc.begin(), m_subfunc.end(), asmdata.Instruction.AddrValue)
						== m_subfunc.end())
					{
						m_subfunc.push_back(asmdata.Instruction.AddrValue);
					}

					trait.append("<" + to_string(asmdata.Instruction.AddrValue) + ">");
					continue;
				}

				//
				// je 这类跳转
				//
				maxaddr = max(maxaddr, asmdata.Instruction.AddrValue);
				ReadHex(asmdata.EIP, len, strtemp);
				trait.append(strtemp);
				continue;
			}
		}



		if (asmdata.Instruction.BranchType != 0 && asmdata.Instruction.AddrValue != 0)
		{
			maxaddr = max(maxaddr, asmdata.Instruction.AddrValue);
			ReadHex(asmdata.EIP, len, strtemp);
			trait.append(strtemp);
			continue;
		}


		//
		// ret 
		//
		if (asmdata.Instruction.BranchType == RetType && asmdata.EIP >= maxaddr)
		{
			ReadHex(asmdata.EIP, len, strtemp);
			trait.append(strtemp);
			traitlen += len;
			break;
		}


		ReadHex(asmdata.EIP, len, strtemp);
		trait.append(strtemp);
		continue;
	}
}

//
// 判断地址是否存在重定位表中，存在返回偏移，不存在返回 -1
//
int CCodeMakeDlg::IsRelocation(ULONG address, int len)
{
	for (int i = 0; i < len - 3; i++)
	{
		//
		// 直接判断地址是否存在表中
		//
		if (m_relocation.find(address + i) != m_relocation.end())
		{
			return i;
		}
	}
	return -1;
}

//
// 生成重定位表
//
void CCodeMakeDlg::GenerateRelocaMap(HMODULE hLib) 
{
	if (!hLib)
	{
		return;
	}

	DWORD LibBase = (DWORD)hLib;
	PIMAGE_DOS_HEADER pDosHead = (PIMAGE_DOS_HEADER)hLib;
	PIMAGE_NT_HEADERS pNtHead = (PIMAGE_NT_HEADERS)(LibBase + pDosHead->e_lfanew);
	PIMAGE_BASE_RELOCATION pRelocation;

	if (pDosHead->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return;
	}

	if (pNtHead->Signature != IMAGE_NT_SIGNATURE)
	{
		return;
	}

	pRelocation = (PIMAGE_BASE_RELOCATION)(LibBase +
		pNtHead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);

	for (; pRelocation->VirtualAddress > 0;)
	{
		DWORD RelocDest = LibBase + pRelocation->VirtualAddress;
		PWORD RelocInfo = (PWORD)((DWORD)pRelocation + sizeof(IMAGE_BASE_RELOCATION));

		for (size_t i = 0; i < (pRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2; i++)
		{
			if (*RelocInfo >> 12 == 3)
			{
				WORD offset = *RelocInfo & 0xFFF;
				m_relocation[RelocDest + offset] = TRUE;
			}
			RelocInfo++;
		}
		pRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)pRelocation + pRelocation->SizeOfBlock);
	}
}

//
// 生成IAT表
//
void CCodeMakeDlg::GenerateIatMap(HANDLE hLib)
{
	DWORD LibBase = (DWORD)hLib;
	PIMAGE_DOS_HEADER pDosHead = (PIMAGE_DOS_HEADER)LibBase;
	PIMAGE_NT_HEADERS pNtHead = (PIMAGE_NT_HEADERS)(LibBase + pDosHead->e_lfanew);

	PIMAGE_IMPORT_DESCRIPTOR pImport;

	if (pDosHead->e_magic != IMAGE_DOS_SIGNATURE)
	{
		return;
	}

	if (pNtHead->Signature != IMAGE_NT_SIGNATURE)
	{
		return;
	}

	pImport = (PIMAGE_IMPORT_DESCRIPTOR)(LibBase +
		pNtHead->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	while (pImport->OriginalFirstThunk > 0)
	{
		string DllName = (char*)(pImport->Name + LibBase);
		DllName.erase(DllName.find(".") + 1);
		
		int* pOrigFirst = (int*)(pImport->OriginalFirstThunk + LibBase);
		DWORD IatAddr = pImport->FirstThunk + LibBase;

		for (; *pOrigFirst > 0; pOrigFirst++, IatAddr += 4)
		{
			string FuncName = (char*)(*pOrigFirst + LibBase + sizeof(WORD));
			m_Iatmap[IatAddr] = DllName + FuncName;
		}

		pImport++;
	}
	return;
}

//
// 输出
//
void CCodeMakeDlg::log(char* format, ...) 
{
	USES_CONVERSION;
	char buf[MAX_PATH] = { 0 };

	va_list va;
	va_start(va, format);
	vsprintf_s(buf, format, va);
	va_end(va);
	
#if _DEBUG
	OutputDebugStringA(buf);
#endif

	m_Outlog.SetCurSel(m_Outlog.InsertString(-1, A2W(buf)));
}