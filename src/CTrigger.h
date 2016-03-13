#pragma once

//#include "Trace.h"

#include "IHack.h"
#include "Vector.h"

class CTrigger : public IHack
{
	var enabled_bool = var("Enabled", type_t::Bool);
	var hitAll_bool = var("Hit all", type_t::Bool);
	var hitbox_bool = var("Hitbox?", type_t::Bool);
	var hitbox_int = var("Hitbox Number", type_t::Int);
	var hitgroup_int = var("Hitgroup Number", type_t::Int);
	var zoomed_bool = var("Zoomed Only", type_t::Bool);

public:
	CTrigger()
	{
		hitbox_int.init(0, 17, 1, 0);
		hitgroup_int.init(0, 7, 1, 0);

		variables.push_back(enabled_bool);
		variables.push_back(hitAll_bool);
		variables.push_back(hitbox_bool);
		variables.push_back(hitbox_int);
		variables.push_back(hitgroup_int);
		variables.push_back(zoomed_bool);
	}

	const char *name() const override;
	bool paint() override;
	bool move(CUserCmd *) override;

private:
	//Vector curr_angles, curr_direction, curr_ray, curr_endpos, curr_pos;

	//int curr_hitbox, curr_hitgroup, curr_physicsbone, curr_class_id;

	//bool curr_ent_null;
};
