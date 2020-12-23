#pragma once

#include "pch.h"
#include "E-Debug.h"


#define PAGE_ERROR (-1)

//
// �����ڴ����Ϣ
//
typedef struct _PAGEINFO
{
	BYTE* pAddr;		// Alloc addr
	DWORD dwOldBase;	// old page base
	DWORD dwOldSize;	// old page size
}PAGEINFO, *PPAGEINFO;

//
// �����������Ϣ
//
typedef struct _ENTRYINFO 
{
	DWORD	dwMagic;		//<- δ֪
	DWORD	dwUnkown1;		//+4 δ֪
	DWORD	dwUnkown2;		//+8 δ֪
	DWORD	dwUserCodeStart;//+c �û����뿪ʼ
	DWORD	dwEString;		//+10 �ַ��� ��Դ       ���û���ַ�����Դ,��Ϊ0
	DWORD	dwEStringSize;	//+14 �ַ��� ��Դ��С   ���û���ַ�����Դ,��Ϊ0
	DWORD	dwEWindow;		//+18 ���������Ϣ      �������ڡ���ǩ�ȿؼ�
	DWORD	dwEWindowSize;	//+1c ��С
	DWORD	dwLibNum;		//+20 ֧�ֿ�����
	DWORD	pLibEntey;		//+24 ֧�ֿ���Ϣ���
	DWORD	dwApiCount;     //+28 Api����
	DWORD	pLibName;		//+2C ָ�������
	DWORD	pApiName;		//+30 ָ��Api����
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

