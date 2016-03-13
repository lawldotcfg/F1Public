#pragma once

#include "IHack.h"

class CNoise : public IHack
{
	var enabled_bool = var("Enabled", type_t::Bool);

public:
	CNoise()
	{
		variables.push_back(enabled_bool);
	}

	// Inherited via IHack
	virtual const char *name() const override;
	virtual bool move(CUserCmd *) override;

};