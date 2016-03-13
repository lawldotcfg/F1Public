#pragma once

#include "SDK.h"

bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, CUserCmd *pCommand);

int __fastcall Hooked_Key_Event(PVOID CHLClient, int edx, int eventcode, ButtonCode_t keynum, const char *pszCurrentBinding);

void __fastcall Hooked_CHLClient_CreateMove(PVOID CHLClient, int edx, int sequence_number, float input_sample_time, bool active);

bool __fastcall Hooked_WriteUserCmdDeltaToBuffer(PVOID pClient, int edx, PDWORD buf, int from, int to, bool isnewcommand);

void __fastcall Hooked_RunCommand(IPrediction *pPred, int edx, CBaseEntity *player, CUserCmd *ucmd, IMoveHelper *moveHelper);

CUserCmd *__fastcall Hooked_GetUserCmd(PVOID pInput, int edx, int sequence_number);
