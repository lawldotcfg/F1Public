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

// no checks here ;)
CUserCmd *__fastcall Hooked_GetUserCmd(PVOID pInput, int edx, int sequence_number)
{
	return &(*(CUserCmd **)((DWORD)gInts.Input.get() + 0xF4))[sequence_number % 90];
}
