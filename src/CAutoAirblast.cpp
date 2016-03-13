#include "CAutoAirblast.h"

#include "SDK.h"
#include "CEntity.h"
#include "Util.h"

CAutoAirblast::CAutoAirblast()
{
	variables.push_back(enabled_bool);
	targets[-1] = 8192.0f;
}

const char * CAutoAirblast::name() const
{
	return "Auto Airblast";
}

bool CAutoAirblast::move(CUserCmd *pUserCmd)
{
	if(!variables[0].bGet())
		return false;

	if(gLocalPlayerVars.activeWeapon == classId::CTFFlameThrower)
	{
		int closeIndex = findBestTarget();

		if(closeIndex == -1)
			return false;

		if(targets[closeIndex] <= 185.0f)
		{
			pUserCmd->buttons |= IN_ATTACK2;
		}

		targets.erase(closeIndex);
	}
	return true;
}

bool CAutoAirblast::inEntityLoop(int index)
{
	if(!variables[0].bGet())
		return false;

	CEntity<> ent{index};

	if(ent.isNull())
	{
		targets.erase(index);
		return false;
	}

	if(ent->IsDormant())
	{
		targets.erase(index);
		return false;
	}

	// if the projectile is from our own team we dont want or need to reflect it
	if(ent.get<int>(gEntVars.iTeam) == gLocalPlayerVars.team)
	{
		targets.erase(index);
		return false;
	}

	classId id = ent->GetClientClass()->iClassID;

	Vector vel;

	switch(id)
	{
	case classId::CTFProjectile_Arrow:
		//case classId::CTFProjectile_Cleaver:
		//case classId::CTFProjectile_EnergyBall:
	case classId::CTFProjectile_Flare:
		//case classId::CTFProjectile_GrapplingHook:
		//case classId::CTFProjectile_HealingBolt:
	case classId::CTFProjectile_Jar:
	case classId::CTFProjectile_JarMilk:
	case classId::CTFProjectile_Rocket:
	case classId::CTFProjectile_SentryRocket:
		//case classId::CTFProjectile_SpellBats:
		//case classId::CTFProjectile_SpellFireball:
		//case classId::CTFProjectile_SpellKartBats:
		//case classId::CTFProjectile_SpellKartOrb:
		//case classId::CTFProjectile_SpellLightningOrb:
		//case classId::CTFProjectile_SpellMeteorShower:
		//case classId::CTFProjectile_SpellMirv:
		//case classId::CTFProjectile_SpellPumpkin:
		//case classId::CTFProjectile_SpellSpawnBoss:
		//case classId::CTFProjectile_SpellSpawnHorde:
		//case classId::CTFProjectile_SpellSpawnZombie:
		//case classId::CTFProjectile_SpellTransposeTeleport:
		//case classId::CTFProjectile_Throwable:
		//case classId::CTFProjectile_ThrowableBreadMonster:
		//case classId::CTFProjectile_ThrowableBrick:
		//case classId::CTFProjectile_ThrowableRepel:
	case classId::CTFGrenadePipebombProjectile:
		// since it is a projectile, we can get its velocity
		vel = EstimateAbsVelocity(ent.castToPointer<CBaseEntity>());
		break;
	default:
		targets.erase(index);
		return false;
	}

	Vector origin;
	ent->GetWorldSpaceCenter(origin);
	
	Vector eyePos = gLocalPlayerVars.pred.origin + CEntity<>{ me }.get<Vector>(gEntVars.vecViewOffset);

	float latency = gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING) + gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);

	Vector target = origin + (vel * latency);

	targets[index] = (target - eyePos).Length();

	return true;
}

int CAutoAirblast::findBestTarget()
{
	float bestDist = 8192.0f;
	int bestIndex = -1;

	for(auto& targ : targets)
	{
		if(targ.second < bestDist)
		{
			bestDist = targ.second;
			bestIndex = targ.first;
		}
	}

	return bestIndex;
}