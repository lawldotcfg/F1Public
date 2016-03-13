#pragma once

#include "IHack.h"
#include "Vector.h"
#include <unordered_map>
#include "CEntity.h"

class CBaseEntity;

typedef unsigned long DWORD;
struct mstudiobbox_t;
typedef float matrix3x4[3][4];

class CAimbot : public IHack
{
	var enabled_bool = var("Enabled", type_t::Bool);
	var hitbox_int = var("Hitbox Number", type_t::Int);
	var zoomed_bool = var("Zoomed Only", type_t::Bool);
	var charged_bool = var("Charged damage", type_t::Bool);
	var zoom_low_float = var("Lowest damage", type_t::Float);
	var cond_bool = var("Ignore cond", type_t::Bool);
	var friend_bool = var("Ignore Friends", type_t::Bool);
	var vel_pred_bool = var("Velocity Pred", type_t::Bool);
	var click_bool = var("Aim on click", type_t::Bool);
	var silent_bool = var("Silent fix", type_t::Bool);
	var proj_bool = var("Projectile", type_t::Bool);
	var proj_mult_float = var("projMult", type_t::Float);
	var noSilent_bool = var("No Silent", type_t::Bool);
	//var proj_iter_int = var("Proj iter", type_t::Int);

	// vars for priority
	var priority_list = var("Priority", type_t::Switch);

	var enable_pri = var("Enable priority", type_t::Bool);
	var scout_pri = var("Scout", type_t::Int);
	var sniper_pri = var("Sniper", type_t::Int);
	var soldier_pri = var("Soldier", type_t::Int);
	var demo_pri = var("Demoman", type_t::Int);
	var medic_pri = var("Medic", type_t::Int);
	var heavy_pri = var("Heavy", type_t::Int);
	var pyro_pri = var("Pyro", type_t::Int);
	var spy_pri = var("Spy", type_t::Int);
	var engi_pri = var("Engi", type_t::Int);


	matrix3x4 BoneToWorld[128];
	Vector	Min, Max;
	mstudiobbox_t *box;
	bool valid;
	Vector predPos, currPos;
	float speed, time, distance;
	Vector oldAngles;

	// maps entitys to their distance
	// if an entity is on the list it has to both be valid and visible
	std::unordered_map<int, float> stats;

public:

	CAimbot()
	{
		hitbox_int.init(0, 16, 1, 0);
		zoom_low_float.init(0.0f, 100.0f, 5.0f, 20.0f);
		proj_mult_float.init(0.0f, 100.0f, 1.0f, 1.0f);
		//proj_iter_int.init(0, 100, 1, 0);

		// init the prioritys
		scout_pri.init(1, 10, 1, 1);
		sniper_pri.init(1, 10, 1, 1);
		soldier_pri.init(1, 10, 1, 1);
		demo_pri.init(1, 10, 1, 1);
		medic_pri.init(1, 10, 1, 1);
		heavy_pri.init(1, 10, 1, 1);
		pyro_pri.init(1, 10, 1, 1);
		spy_pri.init(1, 10, 1, 1);
		engi_pri.init(1, 10, 1, 1);

		variables.push_back(enabled_bool);
		variables.push_back(hitbox_int);
		variables.push_back(zoomed_bool);
		variables.push_back(charged_bool);
		variables.push_back(zoom_low_float);
		variables.push_back(cond_bool);
		variables.push_back(friend_bool);
		variables.push_back(vel_pred_bool);
		variables.push_back(click_bool);
		variables.push_back(silent_bool);

		// add the priority ones
		priority_list.vars.push_back(enable_pri);
		priority_list.vars.push_back(scout_pri);
		priority_list.vars.push_back(sniper_pri);
		priority_list.vars.push_back(soldier_pri);
		priority_list.vars.push_back(demo_pri);
		priority_list.vars.push_back(medic_pri);
		priority_list.vars.push_back(heavy_pri);
		priority_list.vars.push_back(pyro_pri);
		priority_list.vars.push_back(spy_pri);
		priority_list.vars.push_back(engi_pri);

		// always set up the parent
		priority_list.vars.parent = &variables;
		variables.push_back(priority_list);
		variables.push_back(proj_bool);
		variables.push_back(proj_mult_float);
		variables.push_back(noSilent_bool);
		//variables.push_back(proj_iter_int);

		valid = false;
		predPos = currPos = Vector{0.0f,0.0f,0.0f};

		//extrapPos = new Vector[maxExtrap];
	}

	// Inherited via IHack
	virtual const char *name() const override;
	virtual bool move(CUserCmd *) override;
	virtual bool paint() override;
	virtual bool inEntityLoop(int index) override;

private:

	inline float getDistance(CEntity<> &ent);

	inline float getDistanceToVector(Vector v);

	inline bool visible(CEntity<> &ent);

	inline void aim(CUserCmd *pUserCmd);

	inline bool isValidTarget(CEntity<> &ent);

	// this assumes the index is the target
	inline Vector getHitBoxVector(CEntity<> &ent);

	inline mstudiobbox_t *GetHitbox(int iHitbox, DWORD *pHeader);

	inline bool isPlayer(CEntity<> &ent);

	inline int selectTarget();

	inline bool checkCond(CEntity<> &ent);

	// takes a vector and entity and scales the vector by the entitys velocity
	inline Vector predict(CEntity<> &ent, Vector v);

	inline void silentMovementFix(CUserCmd *pUserCmd, Vector angles);
};