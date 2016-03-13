#pragma once
//===================================================================================
#include "SDK.h"
#include "Panels.h"

#define RED( COLORCODE ) ( (int)( COLORCODE >> 24 ) )
#define BLUE( COLORCODE ) ( (int)( COLORCODE >> 8 ) & 0xFF )
#define GREEN( COLORCODE ) ( (int)( COLORCODE >> 16 ) & 0xFF )
#define ALPHA( COLORCODE ) ( (int)COLORCODE & 0xFF )
#define COLORCODE( r, g, b, a ) ( ( DWORD )( ( ( (r)&0xff ) << 24 ) | ( ( (g)&0xff ) << 16 ) | ( ( (b)&0xff ) << 8 ) | ( (a)&0xff ) ) )

//===================================================================================
// messy fix later
#define COLOR_OBJ COLORCODE( 255, 255, 255, 255 )			// 0
#define COLOR_TEAMONE COLORCODE( 0, 128, 255, 255 )			// 1
#define COLOR_TEAMTWO COLORCODE( 255, 10, 0, 255 )			// 2
#define COLOR_TEAMTHREE COLORCODE( 0, 128, 255, 255 )		// 3
#define COLOR_TEAMFOUR COLORCODE( 255, 128, 0, 255 )		// 4
#define COLOR_BACK COLORCODE( 30, 30, 30, 128 )				// 5
#define COLOR_LINE COLORCODE( 255, 120, 0, 255 )			// 6
#define COLOR_OUTLINE COLORCODE( 255, 100, 000, 255 )		// 7
#define COLOR_SELECT COLORCODE( 255, 100, 000, 255 )		// 8
#define COLOR_HUD_TEXT COLORCODE( 255, 255, 255, 255 )		// 9
#define COLOR_CROSS COLORCODE( 255, 255, 255, 255 )			// 10
#define COLOR_CROSS_SEC COLORCODE( 255, 0, 0, 255 )			// 11
#define COLOR_INVIS_TEAMTWO COLORCODE( 110, 180, 255, 255 ) // 12
#define COLOR_INVIS_TEAMTHREE COLORCODE( 255, 0, 0, 255 )   // 13
#define COLOR_MENU_ON COLORCODE( 0, 128, 255, 255 )			// 14
#define COLOR_MENU_OFF COLORCODE( 255, 255, 255, 255 )		// 15
#define COLOR_AIM_TARGET COLORCODE( 0, 255, 0, 255 )		// 16
#define COLOR_BLACK COLORCODE( 0, 0, 0, 255 )				// 17
//===================================================================================
class CDrawManager
{
public:
	enum EFontFlags
	{
		FONTFLAG_NONE,
		FONTFLAG_ITALIC = 0x001,
		FONTFLAG_UNDERLINE = 0x002,
		FONTFLAG_STRIKEOUT = 0x004,
		FONTFLAG_SYMBOL = 0x008,
		FONTFLAG_ANTIALIAS = 0x010,
		FONTFLAG_GAUSSIANBLUR = 0x020,
		FONTFLAG_ROTARY = 0x040,
		FONTFLAG_DROPSHADOW = 0x080,
		FONTFLAG_ADDITIVE = 0x100,
		FONTFLAG_OUTLINE = 0x200,
		FONTFLAG_CUSTOM = 0x400, // custom generated font - never fall back to asian compatibility mode
	};

	struct font_t
	{
		unsigned long font;
		std::string name;
		std::string windowsName;
		int tall_;
		int weight_;
		int flags_;

		void setFont(std::string windowsFontName, int tall, int weight, int flags, std::string mapName)
		{
			name = mapName;
			windowsName = windowsFontName;
			tall_ = tall;
			weight_ = weight;
			flags_ = flags;
			font = gInts.Surface->CreateFont();
			gInts.Surface->SetFontGlyphSet(font, windowsFontName.c_str(), tall, weight, 0, 0, flags);
		}
	};

	struct font_size_t
	{
		int height;
		int length;
	};

	void Initialize();
	//void DrawString( int x, int y, DWORD dwColor, const wchar_t *pszText );
	//void DrawString( int x, int y, DWORD dwColor, const char *pszText, ... );
	//void DrawString( font_t font, int x, int y, DWORD dwColor, const char *pszText, ... );
	void DrawString(const char *fontName, int x, int y, DWORD dwColor, const char *pszText, ...);
	//void DrawString( const char *fontName, int x, int y, int z, DWORD dwColor, const char *pszText, ... );
	BYTE GetESPHeight();
	BYTE GetHudHeight();
	font_size_t GetPixelTextSize(const char *fontName, const char *pszText);
	//int GetPixelTextSize ( wchar_t *pszText );
	void DrawBox(Vector vOrigin, int r, int g, int b, int alpha, int box_width, int radius);
	void DrawRect(int x, int y, int w, int h, DWORD dwColor);
	void OutlineRect(int x, int y, int w, int h, DWORD dwColor);
	bool WorldToScreen(Vector &vOrigin, Vector &vScreen);
	font_t getFont(const char *windowsFontName);
	void addFont(font_t font);
	DWORD dwGetTeamColor(int iIndex)
	{
		static DWORD dwColors[] = {
			0,								//Dummy
			0,								// 1 Teamone (UNUSED)
			/*0xFF8000FF*/ COLOR_TEAMTWO,   // 2 Teamtwo (RED)
			/*0x0080FFFF*/ COLOR_TEAMTHREE, // 3 teamthree (BLUE)
			0								// 4 Teamfour (UNUSED)
		};
		return dwColors[iIndex];
	}

	void drawLine(int startx, int starty, int endx, int endy, DWORD color);

private:
	unsigned long m_Font;
	std::unordered_map<std::string, font_t> fonts;
};
//===================================================================================
extern CDrawManager gDrawManager;
//===================================================================================
