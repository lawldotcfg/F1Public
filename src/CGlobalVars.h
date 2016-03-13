#pragma once

class CPlayerVariables
{
public:
	void find();

	DWORD iWeaponID;
	DWORD bHasMeleeWeapon;
	DWORD bHasFlameThrower;

	DWORD bGlowEnabled;
	DWORD iTeam;
	DWORD iLifeState;
	DWORD iHealth;
	DWORD iClass;
	DWORD bReadyToBackstab;

	DWORD iWeaponSlot;
	DWORD iPlayerCond;
	DWORD iPlayerCondEx;
	DWORD iFlags;
	DWORD chName;

	DWORD vecViewOffset;
	DWORD vecVelocity;
	DWORD vecBaseVelocity;
	DWORD vecPunchAngle;

	DWORD flChargedDamage;
	DWORD nTickBase;
	DWORD flNextPrimaryAttack;

	DWORD hActiveWeapon;
	DWORD hOwner;

	DWORD iItemDefinitionIndex;
};

class COffsets
{
public:
	DWORD dwWriteUserCmd;
	enum
	{
		createMoveOffset = 21, 
		paintTraverseOffset = 41, 
		keyEvent = 20,
		writeUserCmdToBufferOffset = 23,
		processGetCvarValue = 28, 
		processSetConVar = 3,
		getUserCmdOffset = 8,
		runCommandOffset = 17,
		dispatchUserMessageOffset = 36,
		emitSound = 4,
		frameStageNotify = 35,
	};

};
