#include "SDK.h"
#include "Panels.h"
#include "CDrawManager.h"

//===================================================================================
void __fastcall Hooked_PaintTraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	return gHack.paintTraverse(pPanels, edx, vguiPanel, forceRepaint, allowForce);
}
//===================================================================================
void Intro(void)
{
	return gHack.intro();
}