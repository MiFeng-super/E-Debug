#pragma once

#include "framework.h"
#include "E-Debug.h"

#define TYPE_NULL	 0
#define TYPE_PASS	 1
#define TYPE_CALL	 2
#define TYPE_JMP	 3
#define TYPE_CALLIAT 4


class TrieTreeNode 
{
public:
	TrieTreeNode();

public:
	TrieTreeNode** m_Node;

	vector<TrieTreeNode*> m_SpecialNode;
	DWORD m_SpecialType;

	char* m_EsigText;	
	char* m_FuncName;
	BOOL m_IsMatch;
};

class TrieTree
{
public:
	TrieTree();
	~TrieTree();

	void Free(TrieTreeNode* p);

	BOOL LoadEsig(const char* lpPath);
	BOOL InsertFunc(string& FuncTxt, string& FuncName);

	TrieTreeNode* AddNode(TrieTreeNode* p, string hex);
	TrieTreeNode* AddSpecialNode(TrieTreeNode* p, DWORD type, string Text);

	char* MatchEsig(DWORD dwFunc);
	TrieTreeNode* Match(TrieTreeNode* p, BYTE* dwFunc);
	BOOL CheckNode(TrieTreeNode* p, BYTE** dwFunc);

	BOOL CmpCode(BYTE* dwFunc, string& FunText);
	BOOL Match_Fast(BYTE* dwFunc, BYTE* code, DWORD dwSize);

public:
	TrieTreeNode* m_root;

	map<string, string> m_Subfun;
	map<DWORD, string> m_RSubfun;
};

