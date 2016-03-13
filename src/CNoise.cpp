#include "CNoise.h"
#include "SDK.h"

const char *CNoise::name() const
{
	return "Noisemaker";
}

bool CNoise::move(CUserCmd *pUserCmd)
{
	if(!variables[0].bGet())
		return false;

	PVOID kv = keyVal::InitKeyValue();
	if(kv)
	{
		// the thing here doesnt matter.
		keyVal::SetName(kv, "use_action_slot_item_server", 0xDEADBEEF);
		gInts.Engine->ServerCmdKeyValues(kv);
		return true;
	}
	else
	{
		return false;
	}
}