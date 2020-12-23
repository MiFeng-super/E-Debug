// E-Debug.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"
#include "E-Debug.h"
#include "DialogMain.h"
#include "EAnalyEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

map<int, vector<DWORD>> g_CallApiMap;
vector<ESIGINFO> g_EsigList;
vector<LIBINFO> g_LibInfo;
CDialogMain* g_pDiaMain;

//
// 工具
//

//
// 搜索二进制，0x90 模糊搜索，返回地址
//
byte* utils::Search_Bin(byte* pSrc, byte* pTrait, int nSrcLen, int nTraitLen) 
{
	if (IsBadReadPtr(pSrc, 4) == TRUE)
	{
		return 0;
	}
	int i, j, k;
	for (i = 0; i <= (nSrcLen - nTraitLen); i++)
	{
		if (pSrc[i] == pTrait[0])
		{
			k = i;
			j = 0;
			while (j < nTraitLen)
			{
				k++; j++;
				if (pTrait[j] == 0x90)
				{
					continue;
				}
				if (pSrc[k] != pTrait[j])
				{
					break;
				}
			}

			if (j == nTraitLen)
			{
				return pSrc + i;
			}
		}
	}
	return 0;
}

//
// 搜索二进制，返回地址
//
byte* utils::Search_Bin_Ex(byte* pSrc, byte* pTrait, int nSrcLen, int nTraitLen)
{
	if (IsBadReadPtr(pSrc, 4) == TRUE)
	{
		return 0;
	}
	int i, j, k;
	for (i = 0; i <= (nSrcLen - nTraitLen); i++)
	{
		if (pSrc[i] == pTrait[0])
		{
			k = i;
			j = 0;
			while (j < nTraitLen)
			{
				k++; j++;
				if (pSrc[k] != pTrait[j])
				{
					break;
				}
			}

			if (j == nTraitLen)
			{
				return pSrc + i;
			}
		}
	}
	return 0;
}

//
// 读 DWORD
//
DWORD utils::GetDword(DWORD addr) 
{
	return *(DWORD*)addr;
}

//
// 取文本中间
//
string utils::GetStrMid(string& src, char* left, char* right, int offset) 
{
	int start = src.find(left, offset);
	if (start == -1)
	{
		return "";
	}

	int end = src.find(right, start);
	if (end == -1)
	{
		return "";
	}
	
	return src.substr(start + strlen(left), end - start - strlen(left));
}

//
// Hex2Bin
//

void utils::Hex2Bin(string& Hex, BYTE* Bin)
{
	static UCHAR BinMap[256] = {
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,		//123456789
		0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,	//ABCDEF
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,	//abcdef
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	for (size_t i = 0, j = 0; i < Hex.size() / 2; i++, j += 2)
	{
		if (Hex[j] == '?' && Hex[j + 1] == '?')
		{
			Bin[i] = 0x90;
			continue;
		}
		Bin[i] = (BinMap[Hex[j]] << 4) + BinMap[Hex[j + 1]];
	}
}


//
// 枚举 Esig 文件
//
BOOL EnumEsig() 
{
	g_EsigList.clear();

	char path[MAX_PATH] = { 0 };
	char path_1[MAX_PATH] = { 0 };
	GetCurrentDirectoryA(MAX_PATH, path);
	strcpy(path_1, path);
	strcat(path, "\\Esig\\*.*");
	strcat(path_1, "\\Esig\\");

	WIN32_FIND_DATAA FindDir;
	HANDLE hFindDir = FindFirstFileA(path, &FindDir);
	if (hFindDir == INVALID_HANDLE_VALUE)
	{
		MessageBoxA(NULL, "枚举 Esig 文件失败", "Error", MB_ICONERROR);
		return FALSE;
	}

	do 
	{
		if (FindDir.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			string strPath;
			string strFind;
			HANDLE hFindEsig;
			WIN32_FIND_DATAA FindEsig;
			string strname = FindDir.cFileName;
			if (strname != "." && strname != "..")
			{
				strFind = path_1;
				strFind += strname;
				strPath = strFind + "\\";
				strFind += "\\*.esig";
				
				hFindEsig = FindFirstFileA(strFind.c_str(), &FindEsig);

				if (hFindEsig != INVALID_HANDLE_VALUE)
				{
					do
					{
						ESIGINFO Esiginfo;
						string EsigPath = strPath;
						EsigPath += FindEsig.cFileName;

						Esiginfo.Ver = FindEsig.cFileName;
						Esiginfo.Guid = FindDir.cFileName;
						Esiginfo.Path = EsigPath;

						Esiginfo.Ver = Esiginfo.Ver.substr(0, Esiginfo.Ver.size() - 5);

						g_EsigList.push_back(Esiginfo);

					} while (FindNextFileA(hFindEsig, &FindEsig));

#ifndef _DEBUG
					CloseHandle(hFindEsig);
#endif
					
				}
			}
		}
	} while (FindNextFileA(hFindDir, &FindDir));

#ifndef _DEBUG
	CloseHandle(hFindDir);
#endif

	return TRUE;
}




//
// OD插件导出函数
//

extc int _export cdecl ODBG_Plugindata(char* shortname)
{
	strcpy(shortname, "E-Debug");
	return PLUGIN_VERSION;
}

extc int _export cdecl ODBG_Plugininit(int ollydbgversion, HWND hw, ulong* features)
{
	if (ollydbgversion != PLUGIN_VERSION)
	{
		Addtolist(0, 0, "E-Debug：插件版本与OD不匹配!");
		return -1;
	}

	Addtolist(0, 0, "E-Debug - Ver 4.1 by 蜜蜂 ");
	Addtolist(0, -1, "");

	return 0;
}

extc int _export cdecl ODBG_Pluginmenu(int origin, char data[4096], void* item)
{
	if (origin == PM_MAIN)
	{
		strcpy(data, "0&开始分析|1&关于");
	}
	return 1;
}

extc void _export cdecl ODBG_Pluginaction(int origin, int action, void* item)
{
	if (origin == PM_MAIN)
	{
		if (action == 1)
		{
			CString str;
			str += "E-debug 4.1\r\n\r\n";
			str += "by:蜜蜂\r\n";
			str += "Thanks:\r\n";
			str += "      xjun 、fjqisba\r\n";
			str += "      以及各位大佬……\r\n";
			MessageBox(NULL, str, L"写得很菜，望各位大佬别嫌弃", MB_ICONINFORMATION);
			return;
		}

		if (action == 0)
		{

			if (Getcputhreadid() == 0)
			{
				MessageBoxA(NULL, "未调试程序！请调试程序后，跳转到程序领空重试。", "Error", MB_ICONERROR);
				return;
			}

			g_LibInfo.clear();
			g_EsigList.clear();
			g_CallApiMap.clear();

			g_CallApiMap.swap(map<int, vector<DWORD>>());
			g_EsigList.swap(vector<ESIGINFO>());
			g_LibInfo.swap(vector<LIBINFO>());

			//
			// 枚举 Esig 文件
			//
			EnumEsig();

			if (g_pDiaMain != NULL)
			{
				delete g_pDiaMain;
			}

			g_pDiaMain = new CDialogMain;
			g_pDiaMain->Create(IDD_DIALOG_MAIN);
			g_pDiaMain->ShowWindow(SW_SHOW);
			return;
		}
	}
}

// CEDebugApp

BEGIN_MESSAGE_MAP(CEDebugApp, CWinApp)
END_MESSAGE_MAP()


// CEDebugApp 构造

CEDebugApp::CEDebugApp()
{
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CEDebugApp 对象

CEDebugApp theApp;


// CEDebugApp 初始化

BOOL CEDebugApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}
