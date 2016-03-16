#include "CHack.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Util.h"
#include "CHackState.h"

// include hacks
#include "CEsp.h"
#include "CBackstab.h"
#include "CGlow.h"
#include "CMenu.h"
#include "CTrigger.h"
#include "CBunnyHop.h"
#include "CNoise.h"
#include "CAimbot.h"
#include "CAutoAirblast.h"

CScreenSize gScreenSize;

CHack::CHack()
{
}
//===================================================================================

CHack::~CHack()
{
}
//===================================================================================

// for drawing on screen
void CHack::paintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	_TRY
	{
		VMTManager &hook = VMTManager::GetHook(pPanels);																									  //Get a pointer to the instance of your VMTManager with the function GetHook.
		hook.GetMethod<void(__thiscall *)(PVOID, unsigned int, bool, bool)>(gOffsets.paintTraverseOffset)(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.

		static unsigned int vguiMatSystemTopPanel;

		if(vguiMatSystemTopPanel == 0) // check for null - unable to use nullptr D:
		{
			const char *szName = gInts.Panels->GetName(vguiPanel);
			if(szName[0] == 'M' && szName[3] == 'S') //Look for MatSystemTopPanel without using slow operations like strcmp or strstr.
			{
				vguiMatSystemTopPanel = vguiPanel;
				Intro();
			}
		}

		if(vguiMatSystemTopPanel == vguiPanel) //If we're on MatSystemTopPanel, call our drawing code.
		{
			//if ( gInts.Engine->IsDrawingLoadingImage( ) || !gInts.Engine->IsInGame( ) || !gInts.Engine->IsConnected( ) || gInts.Engine->Con_IsVisible( ) || ( ( GetAsyncKeyState( VK_F12 ) || gInts.Engine->IsTakingScreenshot( ) ) ) )
			//	return; //We don't want to draw at the menu.

			//This section will be called when the player is not at the menu game and can see the screen or not taking a screenshot.
			gDrawManager.DrawString( "hud", ( gScreenSize.iScreenWidth / 2 ) - 55, 200, gDrawManager.dwGetTeamColor( gLocalPlayerVars.team ), "Welcome to F1" ); //Remove this if you want.

			// debug string stuff

			int y = 600;
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "old: %f %f %f", gLocalPlayerVars.pred.oldOrigin[0], gLocalPlayerVars.pred.oldOrigin[1], gLocalPlayerVars.pred.oldOrigin[2]);
			//y += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "new: %f %f %f", gLocalPlayerVars.pred.origin[0], gLocalPlayerVars.pred.origin[1], gLocalPlayerVars.pred.origin[2]);
			//y += gDrawManager.GetHudHeight();
			gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "dif: %f %f %f", gLocalPlayerVars.pred.origin[0] - gLocalPlayerVars.pred.oldOrigin[0], gLocalPlayerVars.pred.origin[1] - gLocalPlayerVars.pred.oldOrigin[1], gLocalPlayerVars.pred.origin[2] - gLocalPlayerVars.pred.oldOrigin[2]);
			y += gDrawManager.GetHudHeight();
			gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "curtime: %f", gInts.Globals->curtime);
			y += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "flNextAttack: %f", gLocalPlayerVars.flNextAttack);
			//y += gDrawManager.GetHudHeight();
			//gDrawManager.DrawString("hud", 0, y, COLOR_OBJ, "diff: %f", gLocalPlayerVars.flNextAttack - gInts.Globals->curtime);
			//y += gDrawManager.GetHudHeight();


			for(auto &hack : men.hacks)
			{
				if(hack != nullptr)
				{
					_TRY
					{
						hack->paint(); // call each hacks paint function
					}
					_CATCH
					{
						//Log::Error(XorString("Error with hack `%s' in paint!"), hack->name());
						_REPORT_ERROR(hack, paint());
						throw;
					}
				}
			}
			// CWorld always occupies entity index 0
			// players are reserved from 1-32
			// therefore this loop can start from 1 rather than 0
			// TODO add seperate try/catch for entity loop since it is special
			for(int i = 1; i < gInts.EntList->GetHighestEntityIndex(); i++)
			{
				for(auto &hack : men.hacks)
				{
					if(hack != nullptr)
					{
						_TRY
						{
							hack->inEntityLoop(i); // call each hacks paint in entity loop function
						}
						_CATCH
						{
							//Log::Error(XorString("Error with hack `%s' in paint!"), hack->name());
							//throw;
							_REPORT_ERROR(hack, inEntityLoop(index));
						}
					}
				}
			}
			men.menu(); // draw our cheat menu
		}
	}
	_CATCHMODULE
	{
		Log::Error("%s", e.what());
	}
	_CATCH
	{
		Log::Error("other error in CHack::paintTraverse");
	}
		//Log::Fatal(XorString("Failed PaintTraverse"));
}
//===================================================================================

// for commands and movement
bool CHack::createMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pUserCmd)
{
	auto &hook = VMTManager::GetHook(ClientMode);
	bool bReturn = hook.GetMethod<bool(__thiscall *)(PVOID, float, CUserCmd *)>(gOffsets.createMoveOffset)(ClientMode, input_sample_frametime, pUserCmd);

	CEntity<> local{me};
	CBaseEntity *localEnt = local.castToPointer<CBaseEntity>();

	// this should NEVER happen
	if(local.isNull())
		throw;

	#pragma region player vars

	// update our stats every game tick
	gLocalPlayerVars.Class	= local.get<tf_classes>(gEntVars.iClass);
	gLocalPlayerVars.cond	= local.get<int>(gEntVars.iPlayerCond);
	gLocalPlayerVars.condEx = local.get<int>(gEntVars.iPlayerCondEx);
	gLocalPlayerVars.health = local.get<int>(gEntVars.iHealth);
	gLocalPlayerVars.team	= local.get<int>(gEntVars.iTeam);
	gLocalPlayerVars.cmdNum = pUserCmd->command_number;

	CBaseEntity *pLocalWep = gInts.EntList->GetClientEntity(HANDLE2INDEX(local.get<int>(gEntVars.hActiveWeapon)));

	if(pLocalWep)
	{
		gLocalPlayerVars.activeWeapon = pLocalWep->GetClientClass()->iClassID;

		gLocalPlayerVars.flNextAttack = CEntity<>{pLocalWep->GetIndex()}.get<float>(gEntVars.flNextPrimaryAttack);
	}
	else
		gLocalPlayerVars.activeWeapon = static_cast<classId>(-1);

	#pragma endregion

	#pragma region player prediction
	// back up globals as the prediction increments them
	float frameTime = gInts.Globals->frametime;
	float currTime = gInts.Globals->curtime;

	gLocalPlayerVars.pred.oldOrigin = localEnt->GetAbsOrigin();
	CUtilMove::runSimulation(gInts.Prediction, pUserCmd->command_number, gInts.Globals->curtime, pUserCmd, localEnt);
	gLocalPlayerVars.pred.origin = localEnt->GetAbsOrigin();

	// restore backups
	gInts.Globals->frametime = frameTime;
	gInts.Globals->curtime = currTime;
	#pragma endregion

	// set these before the hacks run
	// we cant have these in chlmove as by that point they have already run

	silentData.fMove = pUserCmd->forwardmove;
	silentData.sMove = pUserCmd->sidemove;
	silentData.view = pUserCmd->viewangles;

	_TRY
	{
		for(int i = 0; i < men.hacks.size(); i++)
		{
			if(men.hacks[i] != nullptr)
			{
				_TRY
				{
					men.hacks[i]->move(pUserCmd); // call each hacks move function
				}
				_CATCH
				{
					//Log::Error(XorString("Error with hack `%s' in move!"), men.hacks[i]->name());
					//throw;
					_REPORT_ERROR(men.hacks[i], move(pUserCmd));
				}
			}
			else
				break;
		}
	}
	_CATCHMODULE
	{
		Log::Error("%s", e.what());
	}

	return false;
}
//===================================================================================

// set up draw manager and netvars
void CHack::intro()
{
	_TRY
	{
		// wait until we have completely inited before running intro
		while(!HState::instance()->checkStatus(HState::FullyInited))
		{
			HState::instance()->think();
		}

		gDrawManager.Initialize(); //Initalize the drawing class.

		gEntVars.find(); // find our netvars

		// find gameResource offsets
		// these are seperate from the entVars due to the number of
		// netvars that there are
		//gInts.GameResource->findOffsets();

		LOGDEBUG(XorString("==========================================================="));

		//{
		//	CDumper nDumper;
		//	nDumper.SaveDump( );
		//}

		// TODO - MOVE ELSEWHERE

		//men.addHack( new CNoPunch( ) );
		
		// TODO the hacks should add themselves
		// we should NOT add them

		_TRY
		{
			men.addHack(new CESP());
			men.addHack(new CBackstab());
			men.addHack(new CGlow());
			men.addHack(new CTrigger());
			men.addHack(new CBunnyHop());
			men.addHack(new CNoise());
			men.addHack(new CAimbot());
			men.addHack(new CAutoAirblast());
		}
		_CATCH
		{
			moduleException e{"Error initialing hacks in intro"};
			throw e;
		}

		for(auto &hack : men.hacks)
		{
			_TRY
			{
				if(hack != nullptr)
					hack->init(); // call each hacks init function
			}
			_CATCH
			{
				_REPORT_ERROR(hack, init());
			}
		}

		Color c(255, 0, 0, 255);
		gInts.Cvar->ConsolePrintf(XorString("_____________________________________________\n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("  __| _ |  _ \\       |     | _)      \n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("  _|    |  __/ |  |   _ \\  |  |   _| \n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString(" _|    _| _|  \\_,_| _.__/ _| _| \\__| \n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("F1Public hack loaded successfully.\n"));
		gInts.Cvar->ConsoleColorPrintf(c, XorString("Have Fun!\n"));
		gInts.Cvar->ConsolePrintf(XorString("_____________________________________________\n"));

		CSteamID localID = gInts.steam.user->GetSteamID();

		gInts.Cvar->ConsoleColorPrintf(c, "SteamID:     %s\n", localID.Render());
		gInts.Cvar->ConsoleColorPrintf(c, "newSteamID:  %s\n", localID.SteamRender());
		gInts.Cvar->ConsoleColorPrintf(c, "PersonaName: %s\n", gInts.steam.friends->GetPersonaName());
		gInts.Cvar->ConsolePrintf(XorString("_____________________________________________\n"));

		//for ( int i = 0; i < gInts.steam.friends->GetFriendCount( k_EFriendFlagImmediate ); i++ )
		//{
		//	CSteamID friendID = gInts.steam.friends->GetFriendByIndex( i, k_EFriendFlagImmediate );
		//	const char *name = gInts.steam.friends->GetFriendPersonaName( friendID );
		//	gInts.Cvar->ConsoleColorPrintf( c, "Friend: %s, %s\n", friendID.Render( ), name );
		//	if ( !strcmp( name, "Aeix" ) )
		//	{
		//		const char *msg = "Cyka Blyat Idi Nahui";
		//		bool test = gInts.steam.friends->SendMsgToFriend( friendID, EChatEntryType::k_EChatEntryTypeChatMsg, (void *) msg, strlen( msg ) );
		//	}
		//}

		for(auto& hack : men.hacks)
		{
			gInts.Cvar->ConsoleColorPrintf(c, "Loaded %s\n", hack->name());
		}

		// unprotect / remove mins and maxes of all cvars
		// causes plenty of problems on some smac servers
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)killCvars, nullptr, 0, nullptr);

		Log::Msg(XorString("Injection Successful")); //If the module got here without crashing, it is good day.
	}
	_CATCHMODULE
	{
		//Log::Fatal("%s", XorString("Failed Intro"));

		// failing intro is kinda fatal
		Log::Fatal("%s", e.what());
	}
	_CATCH
	{
		Log::Fatal("Undefined exception during intro!\nExiting now!");
	}
}
//===================================================================================

// hooked in key event
int CHack::keyEvent(PVOID CHLClient, int edx, int eventcode, ButtonCode_t keynum, const char *currentBinding)
{
	int ret;
	_TRY
	{
		VMTManager &hook = VMTManager::GetHook(CHLClient); // Get a pointer to the instance of your VMTManager with the function GetHook.
		ret = hook.GetMethod<int(__thiscall *)(PVOID, int, int, const char *)>(gOffsets.keyEvent)(CHLClient, eventcode, static_cast<int>(keynum), currentBinding); // Call the original.

		if(eventcode == 1)
		{
			for(auto &hack : men.hacks)
			{
				if(hack != nullptr)
				{
					_TRY
					{
						hack->keyEvent(keynum); // call each hacks keyEvent function
					}
					_CATCH
					{
						//Log::Error(XorString("Error with hack `%s' in keyEvent!"), hack->name());
						//throw;
						_REPORT_ERROR(hack, keyEvent());
					}
				}
			}

			return men.keyEvent(keynum);
		}
	}
	_CATCHMODULE
	{
		//Log::Fatal(XorString("Error with key event!"));
		Log::Error("%s", e.what());
	}

	return ret;
}
//===================================================================================
void CHack::CHLCreateMove(PVOID CHLClient, int sequence_number, float input_sample_time, bool active, bool &sendPacket)
{
	_TRY
	{
		// use this cmd accross all hacks
		CUserCmd *pUserCmd = gInts.Input->GetUserCmd(sequence_number);

		VMTManager& hook = VMTManager::GetHook(CHLClient); //Get a pointer to the instance of your VMTManager with the function GetHook.
		hook.GetMethod<void(__thiscall *)(PVOID, int, float, bool)>(gOffsets.createMoveOffset)(CHLClient, sequence_number, input_sample_time, active); //Call the original.

		CEntity<> local{me};

		for(auto& hack : men.hacks)
		{
			_TRY
			{
				hack->chlmove(pUserCmd);
			}
			_CATCH
			{
				//Log::Error(XorString("Error with hack `%s' in chlmove!"), hack->name());
				//throw;
				_REPORT_ERROR(hack, chlMove(pUserCmd));
			}
		}


		// this should always happen, but we can never be sure
		if(!local.isNull())
		{
			bool shooting = true;

			if(bulletTime(local, true))
				shooting = false;

			if(pUserCmd->buttons & IN_ATTACK && shooting)
			{
				sendPacket = false;
			}
			else if(!(pUserCmd->buttons & IN_ATTACK))
			{
				// only reset the angles if we are not attacking
				sendPacket = true;

				pUserCmd->viewangles = silentData.view;
				//gInts.Engine->SetViewAngles(silentData.view);

				// dont set these
				//pUserCmd->sidemove = silentData.sMove;
				//pUserCmd->forwardmove = silentData.fMove;
			}
			else
			{
				sendPacket = true;
			}
		}

		CVerifiedUserCmd *pSafeCommand = (CVerifiedUserCmd *)(*(DWORD *)(gInts.Input.get() + 0xF8) + (sizeof(CVerifiedUserCmd) * (sequence_number % 90)));
		pSafeCommand->m_cmd = *pUserCmd;
		pSafeCommand->m_crc = GetChecksumForCmd(pSafeCommand->m_cmd);

	}
	_CATCHMODULE
	{
		//Log::Fatal(XorString("Error with CHLMove!"));
		Log::Error("%s", e.what());
	}

	return;
}
//===================================================================================
