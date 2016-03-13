#pragma once

#include "IHack.h"
#include <unordered_map>

class ConVar;

class CAutoAirblast : public IHack
{
	var enabled_bool{"Enabled", type_t::Bool};

	// maps entity indexes to their distance
	std::unordered_map<int, float> targets;

public:

	CAutoAirblast();

	// Inherited via IHack
	virtual const char *name() const override;
	virtual bool move(CUserCmd *pUserCmd) override;
	virtual bool inEntityLoop(int index) override;

private:
	int findBestTarget();
};