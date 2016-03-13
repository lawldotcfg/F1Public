#include "SDK.h"
#include "Client.h"

//============================================================================================
bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand)
{
	return gHack.createMove(ClientMode, edx, input_sample_frametime, pCommand);
}
//============================================================================================
int __fastcall Hooked_Key_Event(PVOID CHLClient, int edx, int eventcode, ButtonCode_t keynum, const char *currentBinding)
{
	return gHack.keyEvent(CHLClient, edx, eventcode, keynum, currentBinding);
}

void __fastcall Hooked_CHLClient_CreateMove(PVOID CHLClient, int edx, int sequence_number, float input_sample_time, bool active)
{
	DWORD createMoveEBP = NULL;

	__asm mov createMoveEBP, ebp

	bool *bSendPacket = (bool *)(*(char **)createMoveEBP - 0x1);


	gHack.CHLCreateMove(CHLClient, sequence_number, input_sample_time, active, *bSendPacket);

	return;
}

bool __fastcall Hooked_WriteUserCmdDeltaToBuffer(PVOID pClient, int edx, PDWORD buf, int from, int to, bool isnewcommand)
{
	try
	{
		typedef void(__cdecl *WriteUserCmdFn)(PDWORD, CUserCmd *, CUserCmd *);
		static WriteUserCmdFn CallWriteUserCmd = (WriteUserCmdFn)gOffsets.dwWriteUserCmd;

		CUserCmd nullcmd, *pFrom, *pTo;

		if(from == -1)
		{
			pFrom = &nullcmd;
		}
		else
		{
			pFrom = gInts.Input->GetUserCmd(from);

			if(!pFrom)
				pFrom = &nullcmd;
		}

		pTo = gInts.Input->GetUserCmd(to);

		if(!pTo)
			pTo = &nullcmd;

		CallWriteUserCmd(buf, pTo, pFrom);

		if(*(bool*)((DWORD)buf + 0x10)) //IsOverflowed
			return false;

		return true;
	}
	catch(...)
	{
		Log::Fatal("Failed Hooked_WriteUserCmdDeltaToBuffer");
		return false;
	}
}

// no checks here ;)
CUserCmd *__fastcall Hooked_GetUserCmd(PVOID pInput, int edx, int sequence_number)
{
	return &(*(CUserCmd **)((DWORD)gInts.Input.get() + 0xF4))[sequence_number % 90];
}