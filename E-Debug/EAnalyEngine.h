#pragma once

#include "pch.h"
#include "E-Debug.h"


#define PAGE_ERROR (-1)

//
// 拷贝内存的信息
//
typedef struct _PAGEINFO
{
	BYTE* pAddr;		// Alloc addr
	DWORD dwOldBase;	// old page base
	DWORD dwOldSize;	// old page size
}PAGEINFO, *PPAGEINFO;

//
// 易语言入口信息
//
typedef struct _ENTRYINFO 
{
	DWORD	dwMagic;		//<- 未知
	DWORD	dwUnkown1;		//+4 未知
	DWORD	dwUnkown2;		//+8 未知
	DWORD	dwUserCodeStart;//+c 用户代码开始
	DWORD	dwEString;		//+10 字符串 资源       如果没有字符串资源,则为0
	DWORD	dwEStringSize;	//+14 字符串 资源大小   如果没有字符串资源,则为0
	DWORD	dwEWindow;		//+18 创建组件信息      包括窗口、标签等控件
	DWORD	dwEWindowSize;	//+1c 大小
	DWORD	dwLibNum;		//+20 支持库数量
	DWORD	pLibEntey;		//+24 支持库信息入口
	DWORD	dwApiCount;     //+28 Api数量
	DWORD	pLibName;		//+2C 指向库名称
	DWORD	pApiName;		//+30 指向Api名称
}*PEENTRYINFO;

class EAnalyEngine
{
public:
	EAnalyEngine(DWORD dwCodeBase);
	~EAnalyEngine();

	int AddPage(DWORD dwBase);
	int UpdatePage(DWORD dwAddr);

	int FindOldPage(DWORD dwAddr);
	int FindVirPage(DWORD dwAddr);

	DWORD GetUserCodeEnd();

	DWORD GetOldDword(DWORD dwAddr, int index);

	DWORD O2V(DWORD dwOldAddr, int index);
	DWORD V2O(DWORD dwVirAddr, int index);

public:
	BOOL m_isinit;
	vector<PAGEINFO> m_pageMap;
	PEENTRYINFO m_pEntryInfo;

	DWORD m_CallDllCmd;
	DWORD m_dwUserCodeStart;
	DWORD m_dwUserCodeEnd;

};

