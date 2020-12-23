#include "EAnalyEngine.h"

#include "DialogMain.h"
extern CDialogMain* g_pDiaMain;

EAnalyEngine* g_pAnalyEngine;

EAnalyEngine::EAnalyEngine(DWORD dwCodeBase) 
{
	byte* pResult;
	DWORD E_entry;
	int index = AddPage(dwCodeBase);

	BYTE SearchCode[17] = { 0x50,0x64,0x89,0x25,0x00,0x00,0x00,0x00,
						  0x81,0xEC,0xAC,0x01,0x00,0x00,0x53,0x56,0x57 };

	pResult = utils::Search_Bin(m_pageMap[index].pAddr, SearchCode, m_pageMap[index].dwOldSize, sizeof(SearchCode));

	if (pResult == NULL)
	{
		return;
	}

	E_entry = utils::GetDword((DWORD)pResult + 0x26);
	if (FindOldPage(E_entry) == PAGE_ERROR)
	{
		index = UpdatePage(E_entry);
		m_pEntryInfo = (PEENTRYINFO)O2V(E_entry, index);
	}
	else
	{
		m_pEntryInfo = (PEENTRYINFO)O2V(E_entry, index);
	}

	m_isinit = TRUE;

	m_dwUserCodeStart = m_pEntryInfo->dwUserCodeStart;
	m_dwUserCodeEnd = GetUserCodeEnd();
}

DWORD EAnalyEngine::GetUserCodeEnd() 
{
	byte* pResult;
	BYTE data[21] = {
		0x55, 0x8B, 0xEC, 0x51, 0x53, 0x56, 0x8B, 0xF1, 0x57, 0x8B, 0x4E, 0x68,
		0x8D, 0x86, 0xD8, 0x00, 0x00, 0x00, 0x50, 0x51, 0xE8 };


	pResult = utils::Search_Bin(m_pageMap[0].pAddr, data, m_pageMap[0].dwOldSize, sizeof(data));
	if (pResult == NULL)
	{
		return 0;
	}

	DWORD temp = (DWORD)(pResult + 0x37);
	return utils::GetDword(temp + 1) + V2O(temp, 0) + 5;
}

EAnalyEngine::~EAnalyEngine() 
{
	for (size_t i = 0; i < m_pageMap.size(); i++)
	{
		VirtualFree(m_pageMap[i].pAddr, m_pageMap[i].dwOldSize, MEM_RELEASE);
	}
}

//
// 添加 page，返回 index
//
int EAnalyEngine::AddPage(DWORD dwBase)
{
	DWORD dwReadsize;
	PAGEINFO pageInfo = { 0 };
	t_memory* pMemory = NULL;

	pMemory = Findmemory(dwBase);
	if (!pMemory)
	{
		g_pDiaMain->Output("查询内存失败！");
		return PAGE_ERROR;
	}

	//
	// 从调试进程内存中，读到调试器内存中
	//
	pageInfo.dwOldBase = pMemory->base;
	pageInfo.dwOldSize = pMemory->size;
	pageInfo.pAddr = (BYTE*)VirtualAlloc(NULL, pageInfo.dwOldSize, MEM_COMMIT, PAGE_READWRITE);
	if (pageInfo.pAddr == NULL)
	{
		g_pDiaMain->Output("申请内存失败！");
		return PAGE_ERROR;
	}

	dwReadsize = Readmemory(pageInfo.pAddr, pageInfo.dwOldBase, pageInfo.dwOldSize, MM_RESILENT);
	if (dwReadsize != pageInfo.dwOldSize)
	{
		g_pDiaMain->Output("读取内存失败！");
		return PAGE_ERROR;
	}

	m_pageMap.push_back(pageInfo);

	return m_pageMap.size() - 1;
}

//
// 更新 page，重新从调试进程中，读内存
//
int EAnalyEngine::UpdatePage(DWORD dwAddr)
{
	int i = FindOldPage(dwAddr);
	t_memory* pMemory;

	if (i != -1)
	{
		VirtualFree(m_pageMap[i].pAddr, m_pageMap[i].dwOldSize, MEM_RELEASE);
		pMemory = Findmemory(dwAddr);

		if (!pMemory)
		{
			g_pDiaMain->Output("查询内存失败！");
			return PAGE_ERROR;
		}
		
		m_pageMap[i].dwOldBase = pMemory->base;
		m_pageMap[i].dwOldSize = pMemory->size;
		m_pageMap[i].pAddr = (BYTE*)VirtualAlloc(NULL, m_pageMap[i].dwOldSize, MEM_COMMIT, PAGE_READWRITE);
		if (m_pageMap[i].pAddr == NULL)
		{
			g_pDiaMain->Output("申请内存失败！");
			return PAGE_ERROR;
		}

		DWORD dwReadsize = Readmemory(m_pageMap[i].pAddr, m_pageMap[i].dwOldBase, m_pageMap[i].dwOldSize, MM_RESILENT);
		if (dwReadsize != m_pageMap[i].dwOldSize)
		{
			g_pDiaMain->Output("读取内存失败！");
			return PAGE_ERROR;
		}

		return i;
	}

	return AddPage(dwAddr);
}

//
// 查找原始地址是否已经在 Page 表中，返回 index
//
int EAnalyEngine::FindOldPage(DWORD dwAddr) 
{
	for (size_t i = 0; i < m_pageMap.size(); i++)
	{
		if (m_pageMap[i].dwOldBase <= dwAddr 
			&& m_pageMap[i].dwOldBase + m_pageMap[i].dwOldSize > dwAddr)
		{
			return i;
		}
	}
	return PAGE_ERROR;
}

//
// 查找虚拟地址是否已经在 Page 表中，返回 index
//
int EAnalyEngine::FindVirPage(DWORD dwAddr) 
{
	for (size_t i = 0; i < m_pageMap.size(); i++)
	{
		if ((DWORD)m_pageMap[i].pAddr <= dwAddr
			&& (DWORD)m_pageMap[i].pAddr + m_pageMap[i].dwOldSize > dwAddr)
		{
			return i;
		}
	}
	return PAGE_ERROR;
}

DWORD EAnalyEngine::O2V(DWORD dwOldAddr, int index)
{
	return dwOldAddr - m_pageMap[index].dwOldBase + (DWORD)m_pageMap[index].pAddr;
}

DWORD EAnalyEngine::V2O(DWORD dwVirAddr, int index) 
{
	return dwVirAddr - (DWORD)m_pageMap[index].pAddr + m_pageMap[index].dwOldBase;
}

DWORD EAnalyEngine::GetOldDword(DWORD dwAddr, int index)
{
	return utils::GetDword(O2V(dwAddr, index));
}