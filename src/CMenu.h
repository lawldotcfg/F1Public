#pragma once

#include <vector>
#include "IHack.h"

class CMenu
{
	//enum class MenuType
	//{
	//	HACK = 0,
	//	VAR = 1
	//};

	//MenuType currType;

	inline void closeSwitch(vecVars &vv)
	{
		for(auto &v : vv.vars)
		{
			// if there is an open switch
			if(v.getType() == type_t::Switch && v.bGet())
			{
				// see if there are any open switches in that
				closeSwitch(v.vars);

				// close this current switch
				v.bVal = false;
			}
		}
	}

public:
	CMenu()
	{
		bMenuActive = false;
		iIndex = 0;
		activeVars = nullptr;
	}

	void menu();				 // draw the menu
	void drawList(vecVars &list, DWORD color, int x, int y, int xx, int w, int h);
	void addHack(IHack *hack); // add a IHack derived hack to the hacks vector

	void get(); // returns vector of hacks

	inline void flipMenu() // enable / disable the menu
	{
		bMenuActive = !bMenuActive;
	}

	std::vector<IHack *> hacks; // TODO refactor later
	int iIndex;					// current hack vector index
	//int iiIndex;				// inside i index
	bool bMenuActive;			// menu active
	vecVars *activeVars;		// the vars that are active to be modified

	int keyEvent(ButtonCode_t keynum);
};
