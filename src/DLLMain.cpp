#include "SDK.h"
#include "Client.h"
#include "Panels.h"
#include "unlinkpeb.h"
#include "CHackState.h"

// maybe move these out of here and into their respective sections
COffsets gOffsets;
CHack gHack;
CInterfaces gInts;
CPlayerVariables gEntVars;
playerVars_t gLocalPlayerVars;

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	//If you manually map, make sure you setup this function properly.
	if(dwReason == DLL_PROCESS_ATTACH)
	{

#ifndef ALT_BUILD
		//DisableThreadLibraryCalls(hInstance);
		UnlinkModuleFromPEB(hInstance);
		RemovePEHeader((DWORD)hInstance);
#else
#pragma message("ALT_BUILD ENABLED")
#endif
		Log::Init(hInstance);

		gInts.thisDll = hInstance;

		// call our init
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)&HState::init, 0, 0, 0);
	}
	else if(dwReason == DLL_PROCESS_DETACH)
	{
		// doesnt ever get called :3
		// maybe hook the exit function and then relink to peb?
		gHack.~CHack();
	}
	return true;
}
