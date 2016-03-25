#pragma once

#include "SDK.h"
#include "CMenu.h"

#ifdef __DEBUG
#include "CDumper.h"
#endif

class CNet;

class CHack
{
	CMenu men;

	struct pSilentData_t
	{
		int tickCounter = 0;

		float fMove, sMove;
		Vector view;
	};

	pSilentData_t silentData;

public:
	CHack();
	~CHack();
	// for drawing on screen
	void paintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce); // take all variables from parent...
	// for commands and movement
	bool createMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand); // take all variables from parent and return to parent (for silent aim later)
	// set up draw manager and netvars
	void intro();
	// hooked in key event
	int keyEvent(PVOID CHClient, int edx, int eventcode, ButtonCode_t keynum, const char *currentBinding);

	void runCommand(IPrediction *pPred, CBaseEntity *player, CUserCmd *ucmd, IMoveHelper *moveHelper) const;
};
