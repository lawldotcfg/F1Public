#include "CBunnyHop.h"

#include "SDK.h"
#include "CEntity.h"

const char* CBunnyHop::name() const
{
	return "Misc Actions";
}

bool CBunnyHop::chlmove(CUserCmd *pUserCmd)
{

	CEntity<> local{me};

	if(variables[0].bGet())
	{
		if(pUserCmd->buttons & IN_JUMP)
		{
			if(!(local.get<int>(gEntVars.iFlags) & FL_ONGROUND))
				pUserCmd->buttons &= ~IN_JUMP;

			return true;
		}
	}

	if(variables[1].bGet())
	{
		if(local.get<int>(gEntVars.iPlayerCond) & tf_cond::TFCond_Taunting)
			local.set<int>(gEntVars.iPlayerCond, local.get<int>(gEntVars.iPlayerCond) & ~tf_cond::TFCond_Taunting);

		return true;
	}

	return false;
}