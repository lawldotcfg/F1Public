#include "CDrawManager.h"

//===================================================================================
CDrawManager gDrawManager;

#define ESP_HEIGHT 12
#define HUD_HEIGHT 14
//===================================================================================
void CDrawManager::Initialize()
{
	if(gInts.Surface == nullptr)
		return;

	gInts.Engine->GetScreenSize(gScreenSize.iScreenWidth, gScreenSize.iScreenHeight);

	font_t font;

	font.setFont("Arial", ESP_HEIGHT, 400, CDrawManager::FONTFLAG_OUTLINE, "esp");
	addFont(font);
	font.setFont("Arial", HUD_HEIGHT, 500, CDrawManager::FONTFLAG_ANTIALIAS, "hud");
	addFont(font);
}
//===================================================================================
//void CDrawManager::DrawString( int x, int y, DWORD dwColor, const wchar_t *pszText )
//{
//	if ( pszText == NULL )
//		return;
//
//	gInts.Surface->DrawSetTextPos( x, y );
//	gInts.Surface->DrawSetTextFont( m_Font );
//	gInts.Surface->DrawSetTextColor( RED( dwColor ), GREEN( dwColor ), BLUE( dwColor ), ALPHA( dwColor ) );
//	gInts.Surface->DrawPrintText( pszText, wcslen( pszText ) );
//}
////===================================================================================
//void CDrawManager::DrawString( int x, int y, DWORD dwColor, const char *pszText, ... )
//{
//	if ( pszText == NULL )
//		return;
//
//	va_list va_alist;
//	char szBuffer[ 1024 ]	= {'\0'};
//	wchar_t szString[ 1024 ] = {'\0'};
//
//	va_start( va_alist, pszText );
//	vsprintf_s( szBuffer, pszText, va_alist );
//	va_end( va_alist );
//
//	wsprintfW( szString, L"%S", szBuffer );
//
//	gInts.Surface->DrawSetTextPos( x, y );
//	gInts.Surface->DrawSetTextFont( m_Font );
//	gInts.Surface->DrawSetTextColor( RED( dwColor ), GREEN( dwColor ), BLUE( dwColor ), ALPHA( dwColor ) );
//	gInts.Surface->DrawPrintText( szString, wcslen( szString ) );
//}
//void CDrawManager::DrawString( font_t font, int x, int y, DWORD dwColor, const char *pszText, ... )
//{
//	if ( pszText == NULL )
//		return;
//
//	va_list va_alist;
//	char szBuffer[ 1024 ]	= {'\0'};
//	wchar_t szString[ 1024 ] = {'\0'};
//
//	va_start( va_alist, pszText );
//	vsprintf_s( szBuffer, pszText, va_alist );
//	va_end( va_alist );
//
//	wsprintfW( szString, L"%S", szBuffer );
//
//	gInts.Surface->DrawSetTextPos( x, y );
//	gInts.Surface->DrawSetTextFont( font.font );
//	gInts.Surface->DrawSetTextColor( RED( dwColor ), GREEN( dwColor ), BLUE( dwColor ), ALPHA( dwColor ) );
//	gInts.Surface->DrawPrintText( szString, wcslen( szString ) );
//}
void CDrawManager::DrawString(const char *fontName, int x, int y, DWORD dwColor, const char *pszText, ...)
{
	if(pszText == NULL || fontName == NULL)
		return;

	va_list va_alist;
	char szBuffer[1024] = {'\0'};
	wchar_t szString[1024] = {'\0'};

	va_start(va_alist, pszText);
	vsprintf_s(szBuffer, pszText, va_alist);
	va_end(va_alist);

	wsprintfW(szString, L"%S", szBuffer);

	gInts.Surface->DrawSetTextPos(x, y);
	gInts.Surface->DrawSetTextFont(fonts[fontName].font);
	gInts.Surface->DrawSetTextColor(RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor));
	gInts.Surface->DrawPrintText(szString, wcslen(szString));
}
//===================================================================================
BYTE CDrawManager::GetESPHeight()
{
	return ESP_HEIGHT;
}
//===================================================================================
BYTE CDrawManager::GetHudHeight()
{
	return HUD_HEIGHT;
}
//===================================================================================
CDrawManager::font_size_t CDrawManager::GetPixelTextSize(const char *fontName, const char *pszText)
{
	font_size_t temp;
	wchar_t buffer[1024] = {'/0'};

	wsprintfW(buffer, L"%s", pszText);

	gInts.Surface->GetTextSize(fonts[fontName].font, buffer, temp.length, temp.height);

	//temp.length *= 1.1f;

	return temp;
}
//===================================================================================
void CDrawManager::DrawRect(int x, int y, int w, int h, DWORD dwColor)
{
	gInts.Surface->DrawSetColor(RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor));
	gInts.Surface->DrawFilledRect(x, y, x + w, y + h);
}
//===================================================================================
void CDrawManager::OutlineRect(int x, int y, int w, int h, DWORD dwColor)
{
	gInts.Surface->DrawSetColor(RED(dwColor), GREEN(dwColor), BLUE(dwColor), ALPHA(dwColor));
	gInts.Surface->DrawOutlinedRect(x, y, x + w, y + h);
}
//===================================================================================
void CDrawManager::DrawBox(Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius)
{
	Vector vScreen;

	if(!WorldToScreen(vOrigin, vScreen))
		return;

	int radius2 = radius << 1;

	OutlineRect(vScreen.x - radius + box_width, vScreen.y - radius + box_width, radius2 - box_width, radius2 - box_width, 0x000000FF);
	OutlineRect(vScreen.x - radius - 1, vScreen.y - radius - 1, radius2 + (box_width + 2), radius2 + (box_width + 2), 0x000000FF);
	DrawRect(vScreen.x - radius + box_width, vScreen.y - radius, radius2 - box_width, box_width, COLORCODE(r, g, b, alpha));
	DrawRect(vScreen.x - radius, vScreen.y + radius, radius2, box_width, COLORCODE(r, g, b, alpha));
	DrawRect(vScreen.x - radius, vScreen.y - radius, box_width, radius2, COLORCODE(r, g, b, alpha));
	DrawRect(vScreen.x + radius, vScreen.y - radius, box_width, radius2 + box_width, COLORCODE(r, g, b, alpha));
}
//===================================================================================
bool CDrawManager::WorldToScreen(Vector &vOrigin, Vector &vScreen)
{
	const matrix3x4 &worldToScreen = gInts.Engine->WorldToScreenMatrix(); //Grab the world to screen matrix from CEngineClient::WorldToScreenMatrix

	float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3]; //Calculate the angle in compareson to the player's camera.
	vScreen.z = 0;																																					//Screen doesn't have a 3rd dimension.

	if(w > 0.001) //If the object is within view.
	{
		float fl1DBw = 1 / w;																																																												//Divide 1 by the angle.
		vScreen.x = (gScreenSize.iScreenWidth / 2) + (0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * fl1DBw) * gScreenSize.iScreenWidth + 0.5);   //Get the X dimension and push it in to the Vector.
		vScreen.y = (gScreenSize.iScreenHeight / 2) - (0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * fl1DBw) * gScreenSize.iScreenHeight + 0.5); //Get the Y dimension and push it in to the Vector.
		return true;
	}

	return false;
}

CDrawManager::font_t CDrawManager::getFont(const char *windowsFontName)
{
	if(windowsFontName == NULL)
		return font_t();

	return fonts[windowsFontName];
}

void CDrawManager::addFont(font_t font)
{
	fonts[font.name] = font;
}

void CDrawManager::drawLine(int startx, int starty, int endx, int endy, DWORD color)
{
	gInts.Surface->DrawSetColor(RED(color), GREEN(color), BLUE(color), ALPHA(color));
	gInts.Surface->DrawLine(startx, starty, endx, endy);
}
