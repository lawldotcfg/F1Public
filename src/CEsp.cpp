#include "CEsp.h"

#include "SDK.h"
#include "CDrawManager.h"
#include "CEntity.h"
#include "Util.h"

const char *CESP::name() const
{
	return "Extra-sensory perception";
}

bool CESP::inEntityLoop(int index)
{
	if(!variables[0].get<bool>())
		return false;

	if(index == me) // we have no reason to perform esp on ourselves
		return false;

	// get the player
	CEntity<> player(index);

	// no nulls
	if(player.isNull())
		return false;

	// no dormants
	if(player->IsDormant())
		return false;

	Vector vecWorld, vecScreen;
	Vector min, max, origin, localOrigin;
	Vector bot, top;

	player->GetRenderBounds(min, max);

	origin = player->GetAbsOrigin();

	player->GetWorldSpaceCenter(vecWorld);

	if(!gDrawManager.WorldToScreen(vecWorld, vecScreen))
		return false;

	if(!gDrawManager.WorldToScreen(Vector(origin.x, origin.y, origin.z + min.z), bot) || !gDrawManager.WorldToScreen(Vector(origin.x, origin.y, origin.z + max.z), top))
		return false;

	float flHeight = bot.y - top.y;
	float flWidth = flHeight / 4.0f;
	DWORD teamColor;

	if(variables[5].bGet())
	{
		if(isPlayerOnFriendsList(index))
		{
			teamColor = COLORCODE(0, 255, 0, 255);
		}
		else
		{
			teamColor = gDrawManager.dwGetTeamColor(player.get<int>(gEntVars.iTeam));
		}
	}
	else
	{
		teamColor = gDrawManager.dwGetTeamColor(player.get<int>(gEntVars.iTeam));
	}

	//Draw on the player.

	classId id = player->GetClientClass()->iClassID;

	if(id == classId::CTFPlayer)
	{

		// no deads
		if(player.get<BYTE>(gEntVars.iLifeState) != LIFE_ALIVE)
			return false;

		player_info_t info;
		if(!gInts.Engine->GetPlayerInfo(index, &info))
			return false;

		if(variables[1].bGet())
		{
			gDrawManager.OutlineRect(top.x - flWidth, top.y, flWidth * 2, flHeight, teamColor); // player box.
		}

		if(variables[2].bGet())
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, COLOR_OBJ, XorString("%s"), info.guid);
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(variables[3].bGet())
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%s"), info.name);
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(variables[4].bGet())
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%i"), player.get<int>(gEntVars.iHealth) /*gInts.GameResource->getHealth(index)*/); //Draw on the player.
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(variables[6].bGet())
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%i"), player.index()); //Draw on the player.
			vecScreen.y += gDrawManager.GetESPHeight();
		}

		if(variables[8].bGet())
		{
			
		}
	}
	else if(id == classId::CObjectDispenser || id == classId::CObjectSapper || id == classId::CObjectSentrygun || id == classId::CObjectTeleporter)
	{
		// just draw the name now

		if(variables[7].bGet())
		{
			gDrawManager.DrawString("esp", vecScreen.x, vecScreen.y, teamColor, XorString("%s"), player->GetClientClass()->chName);
			vecScreen.y += gDrawManager.GetESPHeight();
		}
	}
	return true;
}