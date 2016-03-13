#pragma once

#include "IHack.h"
#include "SDK.h"
//#include "Trace.h"
#include "CUtlVector.h"

#include <unordered_map>

// straight from volvo's sdk
struct GlowObjectDefinition_t
{
	CHandle hEntity;
	float r;
	float g; // Vector m_vGlowColor;
	float b;
	float a;

	bool m_bRenderWhenOccluded;
	bool m_bRenderWhenUnoccluded;
	int m_nSplitScreenSlot;

	// Linked list of free slots
	int m_nNextFreeSlot;

	// Special values for GlowObjectDefinition_t::m_nNextFreeSlot
	static const int END_OF_FREE_LIST = -1;
	static const int ENTRY_IN_USE = -2;
};

class CGlowManager
{
public:
	//unsigned char padding[ 0x004 ];
	//CGlowObject glowObjects[ 1 ];

	CUtlVector<GlowObjectDefinition_t> glowObjects;
};

class CGlow : public IHack
{
	// the pointer to our glow object manager
	CGlowManager *pGlowObjectManger;

	var enabled_bool = var("Enabled", type_t::Bool);
	var enemy_bool = var("Enemy only", type_t::Bool);
	var team_color_bool = var("Team Colors?", type_t::Bool);

	//int s = sizeof(GlowObjectDefinition_t);

	// maps entity index to whether the glow object has been created
	std::unordered_map<int, bool> glowObjects;
	int registerGlowObject(CBaseEntity *ent, float r, float g, float b, float a, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot);

public:
	CGlow()
	{
		variables.push_back(enabled_bool);
		variables.push_back(enemy_bool);
		variables.push_back(team_color_bool);
	}

	const char *name() const override;
	bool init() override;
	bool paint() override;
	bool inEntityLoop(int index) override;

};
