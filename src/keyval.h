#pragma once

#include "baseHeaders.h"
#include "CSignature.h"

#define INVALID_KEY_SYMBOL (-1)

enum types_t
{
	TYPE_NONE = 0,
	TYPE_STRING,
	TYPE_INT,
	TYPE_FLOAT,
	TYPE_PTR,
	TYPE_WSTRING,
	TYPE_COLOR,
	TYPE_UINT64,
	TYPE_NUMTYPES,
};

// keyvalues data
//class KeyValues
//{
//public:
//
//	// init function from the SDK
//	void Init()
//	{
//		m_iKeyName = INVALID_KEY_SYMBOL;
//		m_iDataType = TYPE_NONE;
//
//		m_pSub = nullptr;
//		m_pPeer = nullptr;
//		m_pChain = nullptr;
//
//		m_sValue = nullptr;
//		m_wsValue = nullptr;
//		m_pValue = nullptr;
//
//		m_bHasEscapeSequences = false;
//		m_bEvaluateConditionals = true;
//
//		// for future proof
//		memset(unused, 0, sizeof(unused));
//	}
//
//public:
//	// this is a hash of the name
//	int m_iKeyName;	// 0x0
//
//	// char values
//	char *m_sValue; // 0x4
//	wchar_t *m_wsValue; // 0x8
//
//	// other values
//	union // 0xC
//	{
//		int m_iValue;
//		float m_flValue;
//		void *m_pValue;
//		unsigned char m_Color[4];
//	};
//
//	char	   m_iDataType; //  0x10
//	char	   m_bHasEscapeSequences; //  0x14
//	char	   m_bEvaluateConditionals; // 0x18
//	char	   unused[1]; // 0x1C
//
//	KeyValues *m_pPeer;	// pointer to next key in list
//	KeyValues *m_pSub;	// pointer to Start of a new sub key list
//	KeyValues *m_pChain;// Search here if it's not in our list
//};

static int s_pfGetSymbolForString(const char *string, bool other)
{
	typedef int(__cdecl* HashFunc_t)(const char*, bool);
	static DWORD dwHashFunctionLocation = gSignatures.GetClientSignature("FF 15 ? ? ? ? 83 C4 08 89 06 8B C6");
	static HashFunc_t OriginalFn = (HashFunc_t)**(PDWORD*)(dwHashFunctionLocation + 0x2);

	return OriginalFn(string, other);
}

class keyVal
{
public:
	static PVOID InitKeyValue(void)
	{
		typedef PDWORD(__cdecl* Init_t)(int);
		// get the location of the function ( 1 byte into the sig )
		static DWORD dwInitLocation = gSignatures.GetClientSignature("E8 ? ? ? ? 83 C4 14 85 C0 74 10 68") + 0x1;
		// get the function from the pointer
		static DWORD dwInit = ((*(PDWORD)(dwInitLocation)) + dwInitLocation + 4);
		// cast the function pointer
		static Init_t InitKeyValues = (Init_t)dwInit;
		// return the value
		return InitKeyValues(32);
	}

	static void SetName(PVOID kv, const char *command, DWORD thing)
	{
		*(PDWORD)((DWORD)kv + 0x4) = 0;
		*(PDWORD)((DWORD)kv + 0x8) = 0;
		*(PDWORD)((DWORD)kv + 0xC) = 0;
		*(PDWORD)((DWORD)kv + 0x10) = thing;
		*(PDWORD)((DWORD)kv + 0x14) = 0;
		*(PDWORD)((DWORD)kv + 0x18) = 0; //Extra one the game isn't doing, but if you don't zero this out, the game crashes.
		*(PDWORD)((DWORD)kv + 0x1C) = 0;
		*(PDWORD)((DWORD)kv + 0) = s_pfGetSymbolForString(command, 1);

		//kv->m_iKeyName = s_pfGetSymbolForString(command, 1); // 0x0
		//kv->m_sValue = 0; // 0x4
		//kv->m_wsValue = 0;
		//kv->m_pValue = 0; // 0xC
		//kv->m_iDataType = 0; // 0x10
		//kv->m_bHasEscapeSequences = 0; // 0x14
		//kv->m_bEvaluateConditionals = 0; // 0x18
		//*kv->unused = 0; // 0x1C
	}
};

// ff's kv class
// TODO fix up
class KeyValues
{
public:
	//KeyValues(const char* setName)
	//{
	//	reinterpret_cast<void(__thiscall*)(PVOID, const char*)>(g_Offsets.dwKeyValuesConstructor)(this, setName);
	//}

	//bool LoadFromFile(DWORD filesystem, const char* resourceName, const char* pathID = NULL, DWORD pfnEvaluateSymbolProc = NULL)
	//{
	//	return reinterpret_cast<bool(__thiscall*)(PVOID, DWORD, const char*, const char*, DWORD)>(g_Offsets.dwKeyValuesLoadFromFile)(this, filesystem ? filesystem + 4 : NULL, resourceName, pathID, pfnEvaluateSymbolProc);
	//}

	//KeyValues* FindKey(const char* keyName, bool bCreate = false)
	//{
	//	return reinterpret_cast<KeyValues*(__thiscall*)(PVOID, const char*, bool)>(g_Offsets.dwKeyValuesFindKey)(this, keyName, bCreate);
	//}

	//void SetInt(const char* keyName, int value)
	//{
	//	KeyValues* dat = FindKey(keyName, true);

	//	if(dat)
	//	{
	//		dat->m_iValue = value;
	//		dat->m_iDataType = TYPE_INT;
	//	}
	//}

	//void SetFloat(const char* keyName, float value)
	//{
	//	KeyValues* dat = FindKey(keyName, true);

	//	if(dat)
	//	{
	//		dat->m_flValue = value;
	//		dat->m_iDataType = TYPE_FLOAT;
	//	}
	//}

	//PVOID operator new(size_t iAllocSize)
	//{
	//	return getvfunc<PVOID(__thiscall*)(PVOID, size_t)>(KeyValuesSystem(), 1)(KeyValuesSystem(), iAllocSize);
	//}

	//void operator delete(PVOID pMem)
	//{
	//	getvfunc<void(__thiscall*)(PVOID, PVOID)>(KeyValuesSystem(), 2)(KeyValuesSystem(), pMem);
	//}

	//types_t GetDataType( const char *keyName = NULL ); 

	enum MergeKeyValuesOp_t
	{
		MERGE_KV_ALL,
		MERGE_KV_UPDATE,
		MERGE_KV_DELETE,
		MERGE_KV_BORROW
	};
	//void MergeFrom(KeyValues* kvMerge, MergeKeyValuesOp_t eOp = MERGE_KV_ALL)
	//{
	//	reinterpret_cast<void(__thiscall*)(PVOID, KeyValues*, MergeKeyValuesOp_t)>(g_Offsets.dwKeyValuesMergeFrom)(this, kvMerge, eOp);
	//}

private:
	//static PVOID KeyValuesSystem()
	//{
	//	return reinterpret_cast<PVOID(__cdecl*)()>(g_Offsets.dwKeyValuesKeyValuesSystem)();
	//}

	DWORD m_iKeyName : 24;
	DWORD m_iKeyNameCaseSensitive1 : 8;

	char* m_sValue;
	wchar_t* m_wsValue;

	union
	{
		int m_iValue;
		float m_flValue;
		PVOID m_pValue;
		BYTE m_Color[4];
	};

	char m_iDataType;
	char m_bHasEscapeSequences;
	WORD m_iKeyNameCaseSensitive2;

	KeyValues* m_pPeer;
	KeyValues* m_pSub;
	KeyValues* m_pChain;

	DWORD pad0; // cba to figure out if it's at the bottom of the class or somewhere else. it works, i don't care. 
};