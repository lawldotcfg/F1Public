#include "SDK.h"

// TODO XorString these

void CPlayerVariables::find()
{
	try
	{
		// initialise netvars
		gNetvars.init();

		// CTFPlayer
		iHealth = gNetvars.get_offset("DT_BasePlayer", "m_iHealth"); // 0xa8
		iLifeState = gNetvars.get_offset("DT_BasePlayer", "m_lifeState");
		iTeam = gNetvars.get_offset("DT_BaseEntity", "m_iTeamNum");
		iClass = gNetvars.get_offset("DT_TFPlayer", "m_PlayerClass", "m_iClass"); // 0xA5
		iFlags = gNetvars.get_offset("DT_BasePlayer", "m_fFlags");
		bGlowEnabled = gNetvars.get_offset("DT_TFPlayer", "m_bGlowEnabled");
		hActiveWeapon = gNetvars.get_offset("DT_BaseCombatCharacter", "m_hActiveWeapon");
		bReadyToBackstab = gNetvars.get_offset("DT_TFWeaponKnife", "m_bReadyToBackstab");
		vecViewOffset = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
		flChargedDamage = gNetvars.get_offset("DT_TFSniperRifle", "SniperRifleLocalData", "m_flChargedDamage");
		nTickBase = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_nTickBase");
		flNextPrimaryAttack = gNetvars.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
		vecVelocity = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecVelocity[0]");
		vecVelocity = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecBaseVelocity");
		iPlayerCond = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
		vecPunchAngle = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_Local", "m_vecPunchAngle");
		hOwner = gNetvars.get_offset("DT_BaseCombatWeapon", "m_hOwner");

		iItemDefinitionIndex = gNetvars.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
		// TODO fixme
		//iPlayerCondEx = gNetvars.get_offset( "DT_TFPlayer", "m_Shared" "m_nPlayerCondEx" );
	}
	catch(...)
	{
		Log::Fatal("Netvars Failed!");
	}
}