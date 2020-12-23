// E-Debug.h: E-Debug DLL 的主标头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号

#define _CHAR_UNSIGNED

#include "Elib.h"
#include "Plugin.h"

#include <vector>
#include <string>
#include <stack>
#include <map>
#include <algorithm>

#pragma warning(disable: 4996)
#pragma comment(lib, "Ollydbg.lib")

using namespace std;

//
// Esig 信息
//
typedef struct _ESIGINFO
{
	string Guid;	//支持库GUID
	string Ver;		//支持库版本
	string Path;	//支持库完整路径
}ESIGINFO, *PESIGINFO;

typedef struct _CMDINFO
{
	DWORD FuncAddr;
	string FuncName;
}CMDINFO;

typedef struct _LIBINFO
{
	string Name;
	string Guid;
	string Ver;
	vector<CMDINFO> cmd;
}LIBINFO, * PLIBINFO;


//
// 全局变量
//
extern map<int, vector<DWORD>> g_CallApiMap;
extern vector<ESIGINFO> g_EsigList;
extern vector<LIBINFO> g_LibInfo;



// CEDebugApp
// 有关此类实现的信息，请参阅 E-Debug.cpp
//

class CEDebugApp : public CWinApp
{
public:
	CEDebugApp();

// 重写
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

namespace utils
{
	byte* Search_Bin(byte* pSrc, byte* pTrait, int nSrcLen, int nTraitLen);
	byte* Search_Bin_Ex(byte* pSrc, byte* pTrait, int nSrcLen, int nTraitLen);
	string GetStrMid(string& src, char* left, char* right, int offset);
	DWORD GetDword(DWORD addr);
	void Hex2Bin(string& Hex, BYTE* Bin);
}