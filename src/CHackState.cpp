#include "CHackState.h"
#include "Client.h"
#include "Panels.h"
#include <thread>

void clientModeThread()
{
	// the new not_null means that we need a temp here to help us
	PVOID tempCM;
	do
	{
		DWORD dwClientModeAddress = gSignatures.GetClientSignature(XorString("8B 0D ? ? ? ? 8B 02 D9 05"));
		XASSERT(dwClientModeAddress);
		// get clientmode from 2 bits further into the pattern.
		//gInts.ClientMode = **reinterpret_cast<ClientModeShared ***>(dwClientModeAddress + 2);
		tempCM = **reinterpret_cast<void ***>(dwClientModeAddress + 0x2);
	} while(tempCM == nullptr);

	// once we find it, assign it
	gInts.ClientMode = static_cast<ClientModeShared *>(tempCM);

	// now that clientmode is not a nullptr we can hook it!

	VMTBaseManager *clientModeHook = new VMTBaseManager(); //Setup our VMTBaseManager for ClientMode.
	clientModeHook->Init(gInts.ClientMode);
	clientModeHook->HookMethod(&Hooked_CreateMove, gOffsets.createMoveOffset); //ClientMode create move is called inside of CHLClient::CreateMove, and thus no need for hooking WriteUserCmdDelta.
	clientModeHook->Rehook();

	HState::instance()->addStatus(HState::hackStatus::clientModeInited);

	return;
}

void steam()
{
	if(gInts.steam.client == nullptr)
	{
		// get the createinterface function
		auto steamClientFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("SteamClient.dll"), "CreateInterface"));

		// get the steamclient interface
		gInts.steam.client = steamClientFactory.get<ISteamClient017 *>(STEAMCLIENT_INTERFACE_VERSION_017, NULL);

		// connect to current user
		HSteamPipe hNewPipe = gInts.steam.client->CreateSteamPipe();
		HSteamUser hNewUser = gInts.steam.client->ConnectToGlobalUser(hNewPipe);

		// get steamfriends (002 offers the best interface)
		gInts.steam.friends = reinterpret_cast<ISteamFriends002 *>(gInts.steam.client->GetISteamFriends(hNewUser, hNewPipe, STEAMFRIENDS_INTERFACE_VERSION_002));

		// get steamUser (latest)
		gInts.steam.user = reinterpret_cast<ISteamUser017 *>(gInts.steam.client->GetISteamUser(hNewUser, hNewPipe, STEAMUSER_INTERFACE_VERSION_017));

		// get userStats (latest)
		gInts.steam.userStats = reinterpret_cast<ISteamUserStats011 *>(gInts.steam.client->GetISteamUserStats(hNewUser, hNewPipe, STEAMUSERSTATS_INTERFACE_VERSION_011));
	}

	HState::instance()->addStatus(HState::hackStatus::steamInited);

	return;
}

void HState::init()
{
	// spawn these threads first
	std::thread{clientModeThread}.detach();

	std::thread{steam}.detach();

	if(gInts.Client == nullptr) //Prevent repeat callings.
	{
		// == CLIENT ==
		auto ClientFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("client.dll"), "CreateInterface"));

		gInts.Client = ClientFactory.get<CHLClient *>("VClient017");

		gInts.EntList = ClientFactory.get<CEntList *>("VClientEntityList003");

		gInts.Prediction = ClientFactory.get<CPrediction *>("VClientPrediction001");

		// == ENGINE ==
		auto EngineFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface"));

		gInts.Engine = EngineFactory.get<EngineClient *>("VEngineClient013");

		gInts.ModelInfo = EngineFactory.get<CModelInfo *>("VModelInfoClient006");

		gInts.EngineTrace = EngineFactory.get<CEngineTrace *>("EngineTraceClient003");

		gInts.RandomStream = EngineFactory.get<CUniformRandomStream *>("VEngineRandom001");

		// == VGUI ==
		auto VGUIFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vguimatsurface.dll")), XorString("CreateInterface")));

		gInts.Surface = VGUIFactory.get<ISurface *>("VGUI_Surface030");

		// == VSTDLIB ==
		auto CvarFactory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vstdlib.dll")), XorString("CreateInterface")));

		gInts.Cvar = CvarFactory.get<ICvar *>("VEngineCvar004");

		// == VGUI2 ==
		auto VGUI2Factory = srcFactory(GetProcAddress(gSignatures.GetModuleHandleSafe(XorString("vgui2.dll")), XorString("CreateInterface")));
		gInts.Panels = VGUI2Factory.get<IPanel *>("VGUI_Panel009");

		// with new not_nulls, panels is garenteed not to be null
		VMTBaseManager *panelHook = new VMTBaseManager(); //Setup our VMTBaseManager for Panels.
		panelHook->Init(gInts.Panels);
		panelHook->HookMethod(&Hooked_PaintTraverse, gOffsets.paintTraverseOffset);
		panelHook->Rehook();

		// TODO change this if we use its
		CreateInterfaceFn AppSysFactory = nullptr;
		DWORD dwAppSystem = gSignatures.GetEngineSignature("A1 ? ? ? ? 8B 11 68");
		XASSERT(dwAppSystem);
		if(dwAppSystem)
		{
			gInts.Globals = *reinterpret_cast<CGlobals**>(dwAppSystem + 8);

			DWORD dwAppSystemAddress = **reinterpret_cast<PDWORD*>((dwAppSystem)+1);
			AppSysFactory = reinterpret_cast<CreateInterfaceFn>(dwAppSystemAddress);
		}

		// get dwWriteUserCmd
		// do this before we hook it
		gOffsets.dwWriteUserCmd = gSignatures.GetClientSignature("55 8B EC 8B 45 10 83 EC 08 8B 40 04"); //Grab WriteUserCmd from client.dll.
		XASSERT(gOffsets.dwWriteUserCmd); //Make sure it's not 0.

		VMTBaseManager *CHLClientHook = new VMTBaseManager(); // set up for chlclient.
		CHLClientHook->Init(gInts.Client);
		CHLClientHook->HookMethod(&Hooked_Key_Event, gOffsets.keyEvent); // hook in key event.
		CHLClientHook->HookMethod(&Hooked_CHLClient_CreateMove, gOffsets.createMoveOffset); // hook chlclient createmove
		CHLClientHook->HookMethod(&Hooked_WriteUserCmdDeltaToBuffer, gOffsets.writeUserCmdToBufferOffset); //This is required so it doesn't CRC the CUserCmd.  We're not going to call the original, because we're reconstructing the function.
		CHLClientHook->Rehook();

		DWORD dwInputPointer = (gSignatures.dwFindPattern((DWORD)CHLClientHook->GetMethod<DWORD>(gOffsets.createMoveOffset), ((DWORD)CHLClientHook->GetMethod<DWORD>(gOffsets.createMoveOffset)) + 0x100, "8B 0D")) + (0x2); //Find the pointer to CInput in CHLClient::CreateMove.
		gInts.Input = **reinterpret_cast<CInput***>(dwInputPointer);

		// hook getusercmd from CInput
		VMTBaseManager *inputHook = new VMTBaseManager();
		inputHook->Init(gInts.Input);
		inputHook->HookMethod(&Hooked_GetUserCmd, gOffsets.getUserCmdOffset);
		inputHook->Rehook();

		// update the status
		HState::instance()->addStatus(hackStatus::baseInited);
	}
	return; //The thread has been completed, and we do not need to call anything once we're done. The call to Hooked_PaintTraverse is now our main thread.
}

void HState::think()
{
	if(checkStatus(baseInited) && checkStatus(clientModeInited) && checkStatus(steamInited))
	{
		addStatus(FullyInited);
	}
}
