#include "CBackstab.h"

#include "SDK.h"
#include "CEntity.h"
#include "Trace.h"

#define __USENETVAR

const char *CBackstab::name() const
{
	return "Auto Backstab";
}

bool CBackstab::move(CUserCmd *pCommand)
{
	// if the hack is not enabled
	if(!variables[0].get<bool>())
		return false;

	// testing auto backstabb - works
	CEntity<> localPlayer(me);

	if(localPlayer.isNull())
		return false;

	DWORD hActiveWeapon = *localPlayer.getPtr<DWORD>(gEntVars.hActiveWeapon);

	// if the handle is not valid
	if(!hActiveWeapon || hActiveWeapon == -1)
		return false;

	// get entity index from handle
	int localWeaponIndex = hActiveWeapon & 0xFFF;

	// set up entity from index of handle
	CEntity<CBaseCombatWeapon> weapon(localWeaponIndex);

	if(weapon.isNull()) // is not null
		return false;

	if(weapon->GetMaxClip1() != -1) // easier check than strcmp - less time wasted
		return false;

	//if ( !weapon.get<bool>( gEntVars.bReadyToBackstab ) ) // is ready to backstab
	//	return false;

	char *name = weapon->GetName();

	if(name[10] == 'k' && name[11] == 'n') // check the important characters - there is only one weapon with a [k] and an [n] at pos 10 and 11
	{
		// TODO fixme
		if(!canBackstab(weapon, localPlayer))
			return false;

		pCommand->buttons |= IN_ATTACK;
	}
	else
	{
		return false;
	}

	return true;
}

// backstab helper
bool CBackstab::canBackstab(CEntity<CBaseCombatWeapon> &weap_entity, CEntity<> &local_entity)
{
	trace_t trace;

	CEntity<CTFBaseWeaponMelee> tfweap(weap_entity.castTo<CTFBaseWeaponMelee>());

	bool istrace = tfweap->DoSwingTrace(trace);

	if(!istrace)
		return false;

	if(!trace.m_pEnt)
		return false;

	if(trace.m_pEnt->IsDormant())
		return false;

	CEntity<> other_entity(trace.m_pEnt->GetIndex());

	if(other_entity.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE)
		return false;

	classId classId = other_entity->GetClientClass()->iClassID;

	if(classId != classId::CTFPlayer)
		return false;

	int other_team = other_entity.get<int>(gEntVars.iTeam); // so we dont have to get the netvar every time

	if(other_team == gLocalPlayerVars.team || (other_team < 2 || other_team > 3)) // check team is not our team or invalid team
		return false;

	if(engineCanBackstab(tfweap.castToPointer<CBaseCombatWeapon>(), other_entity.castToPointer<CBaseEntity>()))
	{
		//Log::Console( "canBackstab!!" );
		return true;
	}

	return false;
}
// we cannot get the viewangles of another player so this function is defunct
// maybe look into how the engine does it
bool CBackstab::isBehind(CEntity<> &other_entity, CEntity<> &local_entity)
{
	//if ( other_entity.isNull( ) )
	//	return false;

	//if ( local_entity.isNull( ) )
	//	return false;

	//// up here because the thisptr becomes null (somehow)
	//float maxDot = variables[ 1 ].fGet( );

	//// Get the forward view vector of the target, ignore Z
	//Vector vecVictimForward = Vector( );
	////vecVictimForward.AngleVectors( other_entity->GetAbsAngles() ); // <---- TODO returns 0, 0, 0 sometimes, maybe check this
	////vecVictimForward[0] = 0.0f; // no pitch
	//vecVictimForward[ 2 ] = 0.0f; // no roll
	////vecVictimForward.NormalizeInPlace();

	//Vector vecLocalForward = local_entity->GetAbsAngles( );
	////vecLocalForward.AngleVectors( local_entity->GetAbsAngles() );
	////vecLocalForward[0] = 0.0f; // no pitch
	//vecLocalForward[ 2 ] = 0.0f; // no roll
	////vecLocalForward.NormalizeInPlace();

	////Log::Console( "%f %f %f", vecLocalForward[0], vecLocalForward[1], vecLocalForward[2] );
	////Log::Console( "%f %f %f", vecVictimForward[0], vecVictimForward[1], vecVictimForward[2] );

	//// Get a vector from my origin to my targets origin
	//Vector vecToTarget, vecTarget, vecLocal;
	//other_entity->GetWorldSpaceCenter( vecTarget );
	//local_entity->GetWorldSpaceCenter( vecLocal );
	//vecToTarget = vecTarget - vecLocal;
	////vecToTarget[0] = 0.0f; // no pitch
	//vecToTarget[ 2 ] = 0.0f; // no roll

	//vecToTarget.NormalizeInPlace( );

	//float flDot = vecVictimForward.Dot( vecToTarget );

	//float normal = ( 1.0f / flDot + FLT_EPSILON );

	////flDot *= (1.0f / ( flDot + FLT_EPSILON ));
	////maxDot *= (1.0f / ( maxDot + FLT_EPSILON ));

	////Log::LogToConsole( "Dot: %f", flDot );
	////Log::LogToConsole( "maxDot: %f", maxDot );
	////Log::LogToConsole( "Normal: %f", normal );

	//return ( normal < maxDot );

	typedef bool(__thiscall * IsBehindFn)(CBaseCombatWeapon *, CBaseEntity *);

	static DWORD dwLoc = gSignatures.GetClientSignature("E8 ? ? ? ? 84 C0 74 08 5F B0 01 5E 5D C2 04 00 A1") + 0x1;

	static DWORD dwIsBehind = ((*(PDWORD)(dwLoc)) + dwLoc + 4);

	static IsBehindFn isBehind = (IsBehindFn)dwIsBehind;

	return isBehind(local_entity.castToPointer<CBaseCombatWeapon>(), other_entity.castToPointer<CBaseEntity>());
}

bool CBackstab::engineCanBackstab(CBaseCombatWeapon *weapon, CBaseEntity *target)
{
	typedef bool(__thiscall * BackstabFn)(CBaseCombatWeapon *, CBaseEntity *);

	static DWORD dwBackstabLoc = gSignatures.GetClientSignature("E8 ? ? ? ? 84 C0 74 36 80 BF") + 0x1;

	static DWORD dwBackstab = ((*(PDWORD)(dwBackstabLoc)) + dwBackstabLoc + 4);

	static BackstabFn backstab = (BackstabFn)dwBackstab;

	if(!weapon || !target)
		return false;

	bool r = backstab(weapon, target);

	return r;
}

bool CBackstab::predicts(CEntity<> &local, CEntity<> &other)
{
	return false;
}
