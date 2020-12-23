#include "TrieTree.h"

#include "EAnalyEngine.h"
extern EAnalyEngine* g_pAnalyEngine;

#include "DialogMain.h"
extern CDialogMain* g_pDiaMain;

TrieTreeNode::TrieTreeNode() 
{
	m_Node = new TrieTreeNode* [256];
	for (size_t i = 0; i < 256; i++)
	{
		m_Node[i] = NULL;
	}

	m_IsMatch = FALSE;
	m_EsigText = NULL;
	m_FuncName = NULL;
	m_SpecialType = TYPE_NULL;
	m_SpecialNode.clear();
}

void TrieTree::Free(TrieTreeNode* p)
{
	if (!p)
	{
		return;
	}

	for (size_t i = 0; i < p->m_SpecialNode.size(); i++)
	{
		Free(p->m_SpecialNode[i]);
	}

	for (size_t i = 0; i < 256; i++)
	{
		if (p->m_Node[i])
		{
			Free(p->m_Node[i]);
		}
	}

	delete[] p->m_Node;

	if (p->m_EsigText)
	{
		delete[] p->m_EsigText;
		p->m_EsigText = NULL;
	}

	if (p->m_FuncName)
	{
		delete[] p->m_FuncName;
		p->m_FuncName = NULL;
	}

	delete p;
}

TrieTree::TrieTree() 
{
	m_root = new TrieTreeNode;
}

TrieTree::~TrieTree() 
{
	Free(m_root);
}

BOOL TrieTree::LoadEsig(const char* lpPath) 
{
	HANDLE hFile = CreateFileA(lpPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	DWORD dwRead;
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	char* pBuf = (char*)malloc(dwFileSize);
	if (!pBuf)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	ReadFile(hFile, pBuf, dwFileSize, &dwRead, NULL);
	string strEsig = pBuf;

	//
	// load sub func
	//
	string strSub = utils::GetStrMid(strEsig, ">>>>Sub start<<<<\r\n", ">>>>Sub end<<<<", 0);

	int pos = strSub.find("\r\n");
	while (pos != -1)
	{
		string temp = strSub.substr(0, pos);
		int tpos = temp.find(":");
		if (tpos == -1)
		{
			break;
		}

		m_Subfun[temp.substr(0, tpos)] = temp.substr(tpos + 1);

		strSub = strSub.substr(pos + 2);
		pos = strSub.find("\r\n");
	}

	//
	// load lib cmd func
	//
	string strFun = utils::GetStrMid(strEsig, ">>>>Func start<<<<\r\n", ">>>>Func end<<<<", 0);

	pos = strFun.find("\r\n");
	while (pos != -1)
	{
		string temp = strFun.substr(0, pos);
		int tpos = temp.find(":");
		if (tpos == -1)
		{
			break;
		}

		//
		// lib cmd func 插入匹配树中
		//
		if (!InsertFunc(temp.substr(tpos + 1), temp.substr(0, tpos)))
		{
			//MessageBoxA(NULL, "插入函数失败", "Error", MB_ICONERROR);
		}

		

		strFun = strFun.substr(pos + 2);
		pos = strFun.find("\r\n");
	}

	if (pBuf)
	{
		free(pBuf);
	}

	CloseHandle(hFile);
	return TRUE;
}

TrieTreeNode* TrieTree::AddNode(TrieTreeNode* p, string hex) 
{
	BYTE index;
	utils::Hex2Bin(hex, &index);
	if (p->m_Node[index])
	{
		return p->m_Node[index];
	}

	TrieTreeNode* Node = new TrieTreeNode;
	p->m_Node[index] = Node;
	return Node;
}

TrieTreeNode* TrieTree::AddSpecialNode(TrieTreeNode* p, DWORD type, string Text) 
{
	for (size_t i = 0; i < p->m_SpecialNode.size(); i++)
	{
		if (p->m_SpecialNode[i]->m_SpecialType == type &&
			p->m_SpecialNode[i]->m_EsigText == Text)
		{
			return p->m_SpecialNode[i];
		}
	}

	TrieTreeNode* Node = new TrieTreeNode;
	p->m_SpecialNode.push_back(Node);
	Node->m_EsigText = new char[Text.size() + 1]; strcpy(Node->m_EsigText, Text.c_str());
	Node->m_SpecialType = type;
	return Node;
}


BOOL TrieTree::InsertFunc(string& FuncTxt, string& FuncName)
{
	TrieTreeNode* p = m_root;

	int pos = 0;

	string hex;
	string SpecialText;

	for (size_t i = 0; i < FuncTxt.size();)
	{
		switch (FuncTxt[i])
		{
		case '<':
			pos = FuncTxt.find(">", i);
			if (pos == -1)
			{
				return FALSE;
			}
			hex = "E8";
			SpecialText = FuncTxt.substr(i + 1, pos - i - 1);

			p = AddNode(p, hex);
			p = AddSpecialNode(p, TYPE_CALL, SpecialText);
			i = pos + 1;
			continue;

		case '[':
			pos = FuncTxt.find("]", i);
			if (pos == -1)
			{
				return FALSE;
			}
			SpecialText = FuncTxt.substr(i + 1, pos - i - 1);

			hex = "FF";
			p = AddNode(p, hex);
			hex = "15";
			p = AddNode(p, hex);
			p = AddSpecialNode(p, TYPE_CALLIAT, SpecialText);
			i = pos + 1;
			continue;

		case '?':
			if (FuncTxt[i + 1] == '?')
			{
				p = AddSpecialNode(p, TYPE_PASS, FuncTxt.substr(i, 2));
				i += 2;
			}
			continue;

			//
			// E9????????
			//
		case 'E':
			if (FuncTxt[i + 1] == '9' && FuncTxt[i + 2] == '?')
			{
				hex = "E9";
				p = AddNode(p, hex);
				p = AddSpecialNode(p, TYPE_JMP, "????????");
				i += 10;
				continue;
			}

		default:
			hex = FuncTxt.substr(i, 2);
			p = AddNode(p, hex);
			i += 2;
		}
	}

	if (p->m_FuncName)
	{
		//
		// 函数重复打印出来
		//
		g_pDiaMain->Output("函数重复： %s --> %s", p->m_FuncName, FuncName.c_str());
		return FALSE;
	}

	p->m_FuncName = new char[FuncName.size() + 1]; strcpy(p->m_FuncName, FuncName.c_str());
	return TRUE;
}

char* TrieTree::MatchEsig(DWORD dwFunc)
{
	TrieTreeNode* Node = Match(m_root, (BYTE*)dwFunc);
	if (Node != NULL)
	{
		Node->m_IsMatch = TRUE;
		return Node->m_FuncName;
	}
	return NULL;
}

TrieTreeNode* TrieTree::Match(TrieTreeNode* p, BYTE* dwFunc)
{
	stack<TrieTreeNode*> stackNode;
	stack<BYTE*> stackFunc;

	stackNode.push(p);
	stackFunc.push(dwFunc);

	while (!stackNode.empty())
	{
		p = stackNode.top();
		dwFunc = stackFunc.top();
		stackNode.pop();
		stackFunc.pop();

		if (p->m_FuncName)
		{
			return p;
		}

		if (!CheckNode(p, &dwFunc))
		{
			continue;
		}

		int x = p->m_SpecialNode.size();
		for (size_t i = 0; i < p->m_SpecialNode.size(); i++)
		{
			stackNode.push(p->m_SpecialNode[i]);
			stackFunc.push(dwFunc);
		}


		//
		// 为了识别，加密 call iat
		//
		if (*dwFunc == 0xE8 || *(dwFunc + 1) == 0xE8)
		{
			//
			// 可能是 call 到了壳端
			//
			int index;
			DWORD oldFunc;
			DWORD CallDest;

			if (*dwFunc == 0xE8)
			{
				index = g_pAnalyEngine->FindVirPage((DWORD)dwFunc);
				oldFunc = g_pAnalyEngine->V2O((DWORD)dwFunc, index);
				CallDest = oldFunc - utils::GetDword((DWORD)dwFunc + 1) - 5;
			}
			else
			{
				index = g_pAnalyEngine->FindVirPage((DWORD)dwFunc + 1);
				oldFunc = g_pAnalyEngine->V2O((DWORD)dwFunc + 1, index);
				CallDest = oldFunc - utils::GetDword((DWORD)dwFunc + 2) - 5;
			}

			if (g_pAnalyEngine->FindOldPage(CallDest) == PAGE_ERROR)
			{
				//
				// 说明后面有 CALL IAT，则按 加密iat的 call。处理
				//
				if (p->m_Node[0xFF] != NULL && p->m_Node[0xFF]->m_Node[0x15] != NULL)
				{
					//
					// 可能存在多个函数名称，但是只能返回一个了
					//

					dwFunc += 6;			//跳过 FF15 字节
					TrieTreeNode* temp = NULL;
					p = p->m_Node[0xFF]->m_Node[0x15];
					
					for (size_t i = 0; i < p->m_SpecialNode.size(); i++)
					{
						if (p->m_SpecialNode[i]->m_Node[*dwFunc])
						{
							temp = Match(p->m_SpecialNode[i]->m_Node[*dwFunc], dwFunc + 1);
							if (temp != NULL && temp->m_IsMatch == FALSE)
							{
								return temp;
							}
						}
					}
					return NULL;
				}
			}
		}

		if (p->m_Node[*dwFunc])
		{
			stackNode.push(p->m_Node[*dwFunc]);
			stackFunc.push(dwFunc + 1);
		}

	}
	return NULL;
}


BOOL TrieTree::CheckNode(TrieTreeNode* p, BYTE** dwFunc)
{
	int index = -1;
	string temp;
	DWORD Calldest = 0;
	DWORD OldCalldest = 0;

	switch (p->m_SpecialType)
	{
	case TYPE_NULL:
		return TRUE;

	case TYPE_PASS:
		//
		// ?? 直接跳过
		//
		++(*dwFunc);
		return TRUE;

	case TYPE_CALL:
		//
		// 获取 call xxx 的立即数
		//
		Calldest = (DWORD)*dwFunc - 1 + utils::GetDword((DWORD)*dwFunc) + 5;
		index = g_pAnalyEngine->FindVirPage(Calldest);
		
		OldCalldest = g_pAnalyEngine->V2O(Calldest, index);
		if (m_RSubfun[OldCalldest] == p->m_EsigText)
		{
			(*dwFunc) += 4;
			return TRUE;
		}
		
		index = g_pAnalyEngine->FindOldPage(OldCalldest);
		if (index == PAGE_ERROR)
		{
			index = g_pAnalyEngine->UpdatePage(OldCalldest);
			if (index == PAGE_ERROR)
			{
				return FALSE;
			}
		}

		//
		// 匹配 sub func
		//
		if (!CmpCode((BYTE*)g_pAnalyEngine->O2V(OldCalldest, index), m_Subfun[p->m_EsigText]))
		{
			return FALSE;
		}

		//
		// 匹配过的sub func , 保存起来。第二次就不匹配了
		//
		m_RSubfun[OldCalldest] = p->m_EsigText;

		(*dwFunc) += 4;
		return TRUE;

	case TYPE_JMP:
		//
		// jmp 处理
		//
		Calldest = (DWORD)*dwFunc - 1 + utils::GetDword((DWORD)*dwFunc) + 5;
		index = g_pAnalyEngine->FindVirPage(Calldest);
		if (index == PAGE_ERROR)
		{
			return FALSE;
		}

		(*dwFunc) = (BYTE*)Calldest;
		return TRUE;

	case TYPE_CALLIAT:
		DWORD IatAddr;
		char buf[MAX_PATH] = { 0 };
		string Iat = p->m_EsigText;
		
		//
		// 从 FF15 xxxxx 中，获取立即数
		//
		IatAddr = *(DWORD*)(*dwFunc);
		//
		// 这里用OD，自带的功能，查
		//
		if (Findname(IatAddr, NM_IMPORT, buf) != 0)
		{
			if (!strcmp(buf, Iat.c_str()))
			{
				(*dwFunc) += 4;
				return TRUE;
			}
		}

		//
		// 如果没查到，则直接 GetProcAddress or 其他方案 判断吧
		//

		int index;
		string DllName;
		string FunName;

		DllName = Iat.substr(0, Iat.find(".") + 1) + "dll";
		FunName = Iat.substr(Iat.find(".") + 1);

		//
		// 第一种方案
		//
		DWORD  FuncAddr;
		HMODULE hModule;
		hModule = GetModuleHandleA(DllName.c_str());
		if (hModule)
		{
			FuncAddr = (DWORD)GetProcAddress(hModule, FunName.c_str());
			index = g_pAnalyEngine->UpdatePage(IatAddr);
			if (index != PAGE_ERROR)
			{
				if (utils::GetDword(g_pAnalyEngine->O2V(IatAddr, index)) == FuncAddr)
				{
					(*dwFunc) += 4;
					return TRUE;
				}
			}
		}


	}

	return FALSE;
}

BOOL TrieTree::CmpCode(BYTE* dwFunc, string& FunText)
{
	BOOL result;
	DWORD len = FunText.size() / 2;
	BYTE* pBuf = new BYTE[len];

	utils::Hex2Bin(FunText, pBuf);
	result = Match_Fast(dwFunc, pBuf, len);

	delete[] pBuf;
	return result;
}

BOOL TrieTree::Match_Fast(BYTE* dwFunc, BYTE* code, DWORD dwSize)
{
	//
	// 这里是 sub func 的匹配，sub func 也可能存在 FF15 加密。
	// 所以，这里也要处理
	//
	for (size_t i = 0; i < dwSize; i++, dwFunc++, code++)
	{
		if (*code == 0x90)
		{
			continue;
		}

		if (i < dwSize - 1)
		{
			if (*dwFunc == 0xE8 && *(WORD*)code == 0x15FF)
			{
				i += 6;
				code += 6;
				dwFunc += 6;
				continue;
			}

			if (*(dwFunc + 1) == 0xE8 && *(WORD*)code == 0x15FF)
			{
				i += 6;
				code += 6;
				dwFunc += 6;
				continue;
			}
		}

		if (*code != *dwFunc)
		{
			return FALSE;
		}

	}
	return TRUE;
}