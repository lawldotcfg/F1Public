#pragma once

#include "IHack.h"

class CBunnyHop : public IHack
{
	var bunny_bool = var("Bunny hop", type_t::Bool);
	var taunt_bool = var("Taunt slide", type_t::Bool);
public:

	CBunnyHop()
	{
		variables.push_back(bunny_bool);
		variables.push_back(taunt_bool);
	}

	const char *name() const override;
	bool chlmove(CUserCmd*) override;
};
