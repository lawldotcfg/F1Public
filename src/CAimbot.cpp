#include "CAimbot.h"
#include "SDK.h"
#include "CEntity.h"
#include "trace.h"
#include "Util.h"
#include "CDrawManager.h"

// simple define for sniper zoom
#define ZOOM_BASE_DAMAGE 150.0f

// only needed here
// TODO move if nesercary
void VectorTransform(Vector& in1, const matrix3x4 &in2, Vector &out)
{
	out[0] = (in1[0] * in2[0][0] + in1[1] * in2[0][1] + in1[2] * in2[0][2]) + in2[0][3];
	out[1] = (in1[0] * in2[1][0] + in1[1] * in2[1][1] + in1[2] * in2[1][2]) + in2[1][3];
	out[2] = (in1[0] * in2[2][0] + in1[1] * in2[2][1] + in1[2] * in2[2][2]) + in2[2][3];
}

const char *CAimbot::name() const
{
	return "Aimbot";
}

bool CAimbot::move(CUserCmd *pUserCmd)
{
	if(variables[0].bGet())
	{

		oldAngles = pUserCmd->viewangles;

		// this does the silent movement fix
		aim(pUserCmd);

		return true;
	}
	return false;
}

bool CAimbot::paint()
{
	if(!variables[0].bGet() || !variables[11].bGet())
		return false;

	int y = 500;
	gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "time: %f", time);
	y += gDrawManager.GetHudHeight();
	gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "speed: %f", speed);
	y += gDrawManager.GetHudHeight();
	gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "dist: %f", distance);
	y += gDrawManager.GetHudHeight();

	return true;
}

// update the entitys stats in the entity loop
bool CAimbot::inEntityLoop(int index)
{
	CEntity<> ent{index};

	// if its not a player
	if(!isPlayer(ent))
	{
		// TODO add world check here
		// we need to make sure we dont aim at entities that are not players
		// so remove it from the map
		stats.erase(index);
		return false;
	}

	// if its not valid then its not valid
	if(!isValidTarget(ent))
	{
		stats.erase(index);
		return false;
	}

	stats[index] = getDistance(ent);

	if(stats[index] == 8192.0f)
	{
		stats.erase(index);
		return false;
	}

	if(!checkCond(ent))
	{
		stats.erase(index);
		return false;
	}
	if(variables[6].bGet())
	{
		// we dont want to hurt friends
		if(isPlayerOnFriendsList(index))
		{
			stats.erase(index);
			return false;
		}
	}

	return true;
}

float CAimbot::getDistance(CEntity<> &ent)
{

	if(ent.isNull())
		return 8192.0f;

	return getDistanceToVector(ent->GetAbsOrigin());
}

inline float CAimbot::getDistanceToVector(Vector v)
{
	CBaseEntity *pLocal = gInts.EntList->GetClientEntity(me);

	if(!pLocal)
		return 8192.0f;

	Vector delta = pLocal->GetAbsOrigin() - v;

	float dist = sqrtf(delta.Length());

	if(dist < 1.0f)
		return 1.0f;

	return dist;
}

bool CAimbot::visible(CEntity<> &ent)
{
	trace_t trace;
	Ray_t ray;
	CBaseFilter filter;

	CEntity<> local{me};

	filter.SetIgnoreEntity(local.castToPointer<CBaseEntity>());

	// the get hitbox already attempts to predict it ( if that is enabled )
	Vector hit = getHitBoxVector(ent);

	// use the predicted origin the view offset is insignificant
	ray.Init(gLocalPlayerVars.pred.origin + local.get<Vector>(gEntVars.vecViewOffset), hit);

	gInts.EngineTrace->TraceRay(ray, MASK_AIMBOT | CONTENTS_HITBOX, &filter, &trace);

	if(trace.m_pEnt)
	{
		return (trace.m_pEnt == ent.castToPointer<CBaseEntity>() && trace.hitGroup != 0);
	}

	return true;
}

void CAimbot::aim(CUserCmd *pUserCmd)
{
	Vector angles;

	int target = selectTarget();

	if(target == -1)
	{
		valid = false;
		return;
	}

	CEntity<> local{me};
	CEntity<> other{target};

	CEntity<> localWeapon{HANDLE2INDEX(local.get<int>(gEntVars.hActiveWeapon))};

	if(local.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE)
		return;

	if(gLocalPlayerVars.Class == TF2_Sniper)
	{
		if(gLocalPlayerVars.cond & tf_cond::TFCond_Zoomed)
		{
			float damage = localWeapon.get<float>(gEntVars.flChargedDamage);
			// minium charge to be able to headshot
			if(damage < variables[4].fGet())
			{
				return;
			}
			// respect zoom damage
			if(variables[3].bGet() && ZOOM_BASE_DAMAGE + damage < other.get<int>(gEntVars.iHealth))
			{
				return;
			}
		}
		else
		{
			if(variables[2].bGet())
			{
				return;
			}
		}
	}
	else if(gLocalPlayerVars.Class == TF2_Spy)
	{
		if(gLocalPlayerVars.activeWeapon == classId::CTFRevolver)
		{
			CTFBaseWeaponGun *tfWeap = localWeapon.castToPointer<CTFBaseWeaponGun>();

			float spread = tfWeap->WeaponGetSpread();

			//Log::Console("%f", spread);

			if(spread > 0)
				return;
		}
		else
		{
			// if we cant fire, dont aim (bullettime is a little backwards)
			if(bulletTime(local, true))
				return;
		}
	}
	else
	{
		// if we cant fire, dont aim (bullettime is a little backwards)
		//if(bulletTime(local, true))
		//	return;
	}

	Vector hit = getHitBoxVector(other);

	// once again use the predicted origin of our local player
	VectorAngles(hit - (gLocalPlayerVars.pred.origin + local.get<Vector>(gEntVars.vecViewOffset)), angles);

	ClampAngle(angles);

	if(variables[8].bGet() && !(pUserCmd->buttons & IN_ATTACK))
		return;

	//Vector oldView = pUserCmd->viewangles;

	if(variables[9].bGet())
	{
		silentMovementFix(pUserCmd, angles);
	}

	pUserCmd->viewangles = angles;

	if(variables[13].bGet())
		gInts.Engine->SetViewAngles(angles);
	//else
	//	gInts.Engine->SetViewAngles(oldAngles);

	// also attack
	pUserCmd->buttons |= IN_ATTACK;
	return;
}

bool CAimbot::isValidTarget(CEntity<> &ent)
{
	if(ent.isNull())
		return false;

	if(ent->IsDormant())
		return false;

	if(!isPlayer(ent))
		return false;

	if(ent.index() == me)
		return false;

	if(ent->GetClientClass()->iClassID != classId::CTFPlayer)
		return false;

	if(ent.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE)
		return false;

	if(ent.get<int>(gEntVars.iTeam) == gLocalPlayerVars.team)
		return false;

	if(!visible(ent))
		return false;

	return true;
}

Vector CAimbot::getHitBoxVector(CEntity<> &ent)
{
	Vector vHitbox;

	if(ent.isNull())
		return vHitbox;

	if(ent->IsDormant())
		return vHitbox;

	PDWORD model = ent->GetModel();

	if(!model)
		return vHitbox;

	PDWORD pStudioHdr = gInts.ModelInfo->GetStudiomodel(model);

	if(!pStudioHdr)
		return vHitbox;

	if(!ent->SetupBones(BoneToWorld, 128, 0x100, 0))
		return vHitbox;

	box = GetHitbox(variables[1].iGet(), pStudioHdr);

	if(!box)
		return vHitbox;

	VectorTransform(box->bbmin, BoneToWorld[box->bone], Min);
	VectorTransform(box->bbmax, BoneToWorld[box->bone], Max);

	vHitbox = (Min + Max) / 2;

	// this either will predict or just return the same thing back
	Vector pred = predict(ent, vHitbox);

	return pred;
}

inline mstudiobbox_t *CAimbot::GetHitbox(int iHitbox, DWORD *pHeader)
{
	int HitboxSetIndex = *(int*)((DWORD)pHeader + 0xB0);
	mstudiohitboxset_t* pSet = (mstudiohitboxset_t*)(((PBYTE)pHeader) + HitboxSetIndex);

	return pSet->pHitbox(iHitbox);
}

inline bool CAimbot::isPlayer(CEntity<> &ent)
{
	if(!ent.isNull())
		return ent->GetClientClass()->iClassID == classId::CTFPlayer;

	return false;
}

int CAimbot::selectTarget()
{
	std::pair<int, float> target;

	// this is the furthest our gun can shoot
	// therefore it should be reasonable that all other entitys should be closer than this
	target.second = 8192.0f;
	target.first = -1;

	// loop through data
	for(auto &t : stats)
	{
		// if we have already got the lowest distance, stop looping
		if(target.second == 1.0f)
			break;

		float currDist = t.second;

		// if priority is enabled
		if(variables[10].vars[0].bGet())
		{
			vecVars &vars = variables[10].vars;
			// scale the distances by priority
			
			CEntity<> tar{t.first};

			if(!tar.isNull())
			{
				if(!tar->IsDormant())
				{
					tf_classes Class = (tf_classes)tar.get<int>(gEntVars.iClass);
					currDist /= vars[Class].iGet();
				}
			}
			
		}

		if(currDist < target.second)
		{
			target = t;
		}
	}

	return target.first;
}

inline bool CAimbot::checkCond(CEntity<> &ent)
{
	if(!variables[5].bGet())
		return true;

	int cond = ent.get<int>(gEntVars.iPlayerCond);

	// people we shouldnt hit
	if(cond & tf_cond::TFCond_Cloaked || cond & tf_cond::TFCond_Ubercharged || cond & tf_cond::TFCond_UberchargeFading || cond & tf_cond::TFCond_CloakFlicker || cond & tf_cond::TFCond_DeadRingered)
		return false;

	// TODO add netvars for cond ex and ex2

	return true;
}

// this doesnt really predict latency, it just compensates for velocity
inline Vector CAimbot::predict(CEntity<> &ent, Vector v)
{
	//Vector vel{0.f,0.f,0.f};

	if(variables[11].bGet())
	{
		float latency = gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) + gInts.Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
		Vector velocity = EstimateAbsVelocity(ent.castToPointer<CBaseEntity>());

		currPos = v;

		// prevent devide by 0 errors later on
		speed = 1.0f;

		CEntity<> local{me};
		CEntity<> localwep{HANDLE2INDEX(local.get<int>(gEntVars.hActiveWeapon))};

		if(!localwep.isNull())
		{
			classId wepID = localwep->GetClientClass()->iClassID;

			switch(wepID)
			{
			case classId::CTFRocketLauncher:
			case classId::CTFRocketLauncher_AirStrike:
			case classId::CTFRocketLauncher_Mortar:
				speed = 1100.0f;
				break;
			case classId::CTFRocketLauncher_DirectHit:
				speed = 1980.0f;
				break;
			default:
				break;
			}
		}

		//CUtilMove::predictVectorForPlayer(v, ent.castToPointer<CBaseEntity>());

		//(pos + (vel * time)) + (1/2 * (grav * (time ^ 2)))


		Vector oldVector = (local->GetAbsOrigin() - v);
		distance = oldVector.Length();
		time = ((distance / speed) + latency) * variables[12].fGet();

		if(ent.get<int>(gEntVars.iFlags) & FL_ONGROUND)
		{
			// if they are on the ground then we can use simple methods to predict where they will be
			// TODO maybe call walkmove to determine this 

			v += (velocity * time);
		}
		else
		{
			
			// set up some constants
			// speed is defined further up
			float grav = 800.0f;

			v -= (0.5 * grav * powf(time, 2) * gInts.Globals->frametime);

			v += (velocity * time);

		}

		// if they are not on the ground
		//v -= (0.5 * (grav * (powf(time, 2))) * gInts.Globals->frametime);	
	}

	if(variables[7].bGet())
	{
		// scale the velocity by the latency
		//vel *= latency;
	}

	// no prediction is enabled just return the input

	//v += vel;
	predPos = v;
	return v;
}

inline void CAimbot::silentMovementFix(CUserCmd *pUserCmd, Vector angles)
{
	Vector vecSilent(pUserCmd->forwardmove, pUserCmd->sidemove, pUserCmd->upmove);
	float flSpeed = sqrt(vecSilent.x * vecSilent.x + vecSilent.y * vecSilent.y);
	Vector angMove;
	VectorAngles(vecSilent, angMove);
	float flYaw = DEG2RAD(angles.y - pUserCmd->viewangles.y + angMove.y);
	pUserCmd->forwardmove = cos(flYaw) * flSpeed;
	pUserCmd->sidemove = sin(flYaw) * flSpeed;
	//pUserCmd->viewangles = angles;
}