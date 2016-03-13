#pragma once

#include "SDK.h"
#include "CEntity.h"

// takes an index and sees if it relates to anyone on their friends list
inline bool isPlayerOnFriendsList(int index)
{
	player_info_t pInfo;

	if(!gInts.Engine->GetPlayerInfo(index, &pInfo))
		return false;

	for(int i = 0; i < gInts.steam.friends->GetFriendCount(k_EFriendFlagImmediate); i++)
	{
		CSteamID friendID = gInts.steam.friends->GetFriendByIndex(i, k_EFriendFlagImmediate);

		// this makes the assumption that it is an individual account and it is in the public universe ( the most common type )
		CSteamID pInfoFriendID{pInfo.friendsID, k_EUniversePublic, k_EAccountTypeIndividual};

		if(friendID == pInfoFriendID)
			return true;
	}
	return false;
}

inline CRC32_t GetChecksumForCmd(CUserCmd userCmd)
{
	CRC32_t crc;

	CRC32_Init(&crc);
	CRC32_ProcessBuffer(&crc, &userCmd.command_number,	sizeof(userCmd.command_number));
	CRC32_ProcessBuffer(&crc, &userCmd.tick_count,		sizeof(userCmd.tick_count));
	CRC32_ProcessBuffer(&crc, &userCmd.viewangles,		sizeof(userCmd.viewangles));
	CRC32_ProcessBuffer(&crc, &userCmd.forwardmove,		sizeof(userCmd.forwardmove));
	CRC32_ProcessBuffer(&crc, &userCmd.sidemove,		sizeof(userCmd.sidemove));
	CRC32_ProcessBuffer(&crc, &userCmd.upmove,			sizeof(userCmd.upmove));
	CRC32_ProcessBuffer(&crc, &userCmd.buttons,			sizeof(userCmd.buttons));
	CRC32_ProcessBuffer(&crc, &userCmd.impulse,			sizeof(userCmd.impulse));
	CRC32_ProcessBuffer(&crc, &userCmd.weaponselect,	sizeof(userCmd.weaponselect));
	CRC32_ProcessBuffer(&crc, &userCmd.weaponsubtype,	sizeof(userCmd.weaponsubtype));
	CRC32_ProcessBuffer(&crc, &userCmd.random_seed,		sizeof(userCmd.random_seed));
	CRC32_ProcessBuffer(&crc, &userCmd.mousedx,			sizeof(userCmd.mousedx));
	CRC32_ProcessBuffer(&crc, &userCmd.mousedy,			sizeof(userCmd.mousedy));
	CRC32_Final(&crc);

	return crc;
}

inline bool bulletTime(CEntity<> &ent, bool shouldUseIntervals)
{
	if(ent.isNull())
		return false;

	CEntity<> wep{HANDLE2INDEX(ent.get<int>(gEntVars.hActiveWeapon))};
	if(wep.isNull())
		return false;


	// either use intervals or use 1
	float interval = shouldUseIntervals ? shouldUseIntervals * gInts.Globals->interval_per_tick : 1;
	

	float tickBase = static_cast<float>(ent.get<int>(gEntVars.nTickBase)) * interval;

	float nextAttack = gLocalPlayerVars.flNextAttack;


	bool canTickbase = nextAttack > tickBase;

	bool canCurTime = nextAttack > gInts.Globals->curtime * interval;

	//Log::Console("bullettime? tickbase: %s curTime: %s", canTickbase ? "true" : "false", canCurTime ? "true" : "false");

	return canTickbase;
}

inline void generateItemList()
{
	static bool alloced;

	if(!alloced)
	{
		if(!AllocConsole())
			Log::Fatal("We Failed men...");
		else
			alloced = true;
	}

	freopen("CONOUT$", "w", stdout);

	printf("enum class classId : int\n{\n");
	for(ClientClass* pCC = gInts.Client->GetAllClasses(); pCC; pCC = pCC->pNextClass)
	{
		printf("\t%s = %i,\n", pCC->chName, pCC->iClassID);
	}

	printf("};");
}

inline Vector EstimateAbsVelocity(CBaseEntity *ent)
{
	typedef void(__thiscall* EstimateAbsVelocityFn)(CBaseEntity *, Vector &);

	static DWORD dwFn = gSignatures.GetClientSignature("E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07") + 0x1;

	static DWORD dwEstimate = ((*(PDWORD)(dwFn)) + dwFn + 4);

	EstimateAbsVelocityFn vel = (EstimateAbsVelocityFn)dwEstimate;

	Vector v;
	vel(ent, v);

	return v;
}

//__declspec(noinline) inline void maskCvar(const char *pszPrefix, ConVar *pVar)
//{
//	// New variables prefix string length
//	const unsigned int PrefixSize = strlen(pszPrefix);
//
//	// Calculate string length of new variable name
//	unsigned int OldVarSize = strlen(pVar->m_pszName);
//	unsigned int NewVarSize = PrefixSize + OldVarSize + 0x2;
//
//	// Create and initialize a pointer to hold the new variable
//	ConVar *pDummyVariable = (ConVar *)LocalAlloc(LMEM_ZEROINIT, sizeof ConVar);
//
//	// Backup original variable name, flags, and value
//	const char *pszOldName = pVar->m_pszName;
//	const char *pszOldValue = pVar->m_pszDefaultValue;
//	unsigned int OldFlags = pVar->m_nFlags;
//
//	// Change original variables name and flags
//	pVar->m_nFlags = 0;
//	pVar->m_pszName = (PCHAR)LocalAlloc(LMEM_ZEROINIT, NewVarSize);
//	sprintf((char *)pVar->m_pszName, "%s_%s", pszPrefix, pszOldName);
//
//	// Create a new variable with the original name, flags, and default value
//	//registerNewCvar(pDummyVariable, pszOldName, pszOldValue, OldFlags);
//}

// global functions are stupid
// but i see no better way of doing this
// TODO MOVE THIS INTO CANTISMAC
inline DWORD WINAPI killCvars(LPVOID param)
{
	ConCommandBase *base = gInts.Cvar->GetCommands();

	int count = 0;

	// set all flags to 0
	while(base)
	{
		// TODO fix masking
		//maskCvar("tf", (ConVar *)base);

		if(base->m_nFlags & (int)ConvarFlags::FCVAR_CHEAT)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_CHEAT;

		if(base->m_nFlags & (int)ConvarFlags::FCVAR_REPLICATED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_REPLICATED;

		if(base->m_nFlags & (int)ConvarFlags::FCVAR_PROTECTED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_PROTECTED;

		if(base->m_nFlags & (int)ConvarFlags::FCVAR_SPONLY) // single player only
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_SPONLY;

		if(base->m_nFlags & (int)ConvarFlags::FCVAR_HIDDEN)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_HIDDEN;

		if(base->m_nFlags & (int)ConvarFlags::FCVAR_DEVELOPMENTONLY)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_DEVELOPMENTONLY;

		if(base->m_nFlags & (int)ConvarFlags::FCVAR_NOT_CONNECTED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_NOT_CONNECTED;

		// if we can convert it into a convar
		if(ConVar *c = dynamic_cast<ConVar *>(base))
		{
			c->m_bHasMax = false;
			c->m_bHasMin = false;
		}

		base = base->m_pNext;
		count++;
	}

	gInts.Cvar->ConsolePrintf("Unprotected %d cvars\n", count);

	return 0;
}

// static move helper functions
class CUtilMove
{
public:
	static void runSimulation(CPrediction *pred, int curr_cmd, float currTime, CUserCmd *cmd, CBaseEntity *pBaseEnt)
	{
		typedef void(__thiscall *runSimulationfn)(CPrediction *, int, float, CUserCmd *, CBaseEntity *);
		static DWORD dwRunSim = gSignatures.GetClientSignature("55 8B EC 51 53 56 8B 75 14 57");
		
		static runSimulationfn func = (runSimulationfn)dwRunSim;
		func(pred, curr_cmd, currTime, cmd, pBaseEnt);

		return;
	}

	static void predictVectorForPlayer(Vector &v, CBaseEntity *ent)
	{
		predictVectorForPlayer(v, ent, gInts.Globals->curtime);
		return;
	}

	static void predictVectorForPlayer(Vector &v, CBaseEntity *ent, float time)
	{
		auto cmd = std::make_unique<CUserCmd>();

		float frameTime = gInts.Globals->frametime;
		float currTime = gInts.Globals->curtime;

		Vector oldOrigin = ent->GetAbsOrigin();

		//gInts.Prediction->SetupMove(ent, cmd, gInts.MoveHelper, pMoveData);
		//gInts.Prediction->RunCommand(ent, cmd, gInts.MoveHelper);
		runSimulation(gInts.Prediction, gLocalPlayerVars.cmdNum, time, cmd.get(), ent);
		//gInts.Prediction->FinishMove(ent, cmd, pMoveData);

		Vector delta = ent->GetAbsOrigin() - oldOrigin;

		// relate that to the vector
		v += delta;

		// restore globals
		gInts.Globals->frametime = frameTime;
		gInts.Globals->curtime = currTime;

		// v is returned by reference
		return;
	}
};

inline int getMaxHealth(CEntity<> &ent)
{
	if(ent.isNull())
		return 0;

	if(ent->GetClientClass()->iClassID != classId::CTFPlayer)
		return 0;

	switch(ent.get<tf_classes>(gEntVars.iClass))
	{
	case TF2_Scout:
	case TF2_Spy:
	case TF2_Sniper:
	case TF2_Engineer:
		return 125;
		break;
	case TF2_Soldier:
		return 200;
		break;
	case TF2_Medic:
		return 150;
	case TF2_Heavy:
		return 300;
		break;
	case TF2_Demoman:
	case TF2_Pyro:
		return 175;
		break;

	default:
		return 0;
		break;
	}

	return 0;
}

inline std::string getPathForDll(HMODULE module)
{
	std::string ret;

	char path[MAX_PATH];

	if(GetModuleFileNameA(module, path, MAX_PATH) != 0)
	{
		size_t slash = -1;

		for(size_t i = 0; i < strlen(path); i++)
		{
			if(path[i] == '/' || path[i] == '\\')
			{
				slash = i;
			}
		}

		if(slash != -1)
		{
			path[slash + 1] = '\0';
			ret = path;
		}
		else
		{
			Log::Error("unable to get path for dll selected");
		}
	}
	return ret;
}