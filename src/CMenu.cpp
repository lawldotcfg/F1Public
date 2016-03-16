#include "SDK.h"
#include "CDrawManager.h"

int CMenu::keyEvent(ButtonCode_t keynum)
{
	if(keynum == ButtonCode_t::KEY_INSERT) //insert
	{
		flipMenu();
		if(activeVars == nullptr)
			if(hacks.size() != 0)
				activeVars = &hacks[0]->variables;
				
		return 0;
	}

	if(bMenuActive)
	{
		if(keynum == ButtonCode_t::KEY_UP || keynum == ButtonCode_t::MOUSE_WHEEL_UP) // Up
		{
			if(activeVars->index > 0)
				activeVars->index--;
			else
				activeVars->index = activeVars->size() - 1;
			return 0;
		}
		else if(keynum == ButtonCode_t::KEY_DOWN || keynum == ButtonCode_t::MOUSE_WHEEL_DOWN) // Down
		{
			if(activeVars->index < activeVars->size() - 1)
				activeVars->index++;
			else
				activeVars->index = 0;
			return 0;
		}
		else if(keynum == ButtonCode_t::KEY_LEFT || keynum == ButtonCode_t::MOUSE_LEFT) // Left
		{
			activeVars->active().decrement();
			return 0;
		}
		else if(keynum == ButtonCode_t::KEY_RIGHT || keynum == ButtonCode_t::MOUSE_RIGHT) // Right
		{
			activeVars->active().increment();
			return 0;
		}
		else if(keynum == ButtonCode_t::KEY_TAB)
		{
			if(iIndex < hacks.size() - 1)
				iIndex++;
			else if(iIndex == hacks.size() - 1)
				iIndex = 0;

			// close all active switches
			closeSwitch(*activeVars);

			// set the active vars to the next
			activeVars = &hacks[iIndex]->variables;

			return 0;
		}
		else if(keynum == ButtonCode_t::KEY_ENTER)
		{
			Log::Console("Enter pressed!!");
			// switches can only be opened using the enter key
			if(activeVars->active().getType() == type_t::Switch)
			{
				activeVars->active().bVal = true;
				//Log::Console("var %s is now %s", activeVars->active().name_.c_str(), activeVars->active().bVal ? "true" : "false");
				activeVars = &activeVars->active().vars;
			}
			else
			{
				// deactivate this switch via its parent
				if(activeVars->parent)
				{
					activeVars = activeVars->parent;
					activeVars->active().bVal = false;
					//Log::Console("var %s is now %s", activeVars->active().name_.c_str(), activeVars->active().bVal ? "true" : "false");
				}
			}
			return 0;
		}
	}
	// by default get the engine to process the key press
	return 1;
}

void CMenu::menu()
{
	if(!bMenuActive)
		return;
	if(hacks.empty()) // if cheats list is empty
		return;
	try
	{
		int x = 0, menux = 300,
			//xx = x + 105,
			y = 300, yy = 0,
			h = gDrawManager.GetPixelTextSize("hud", "A").height,
			menuw = 230;
			
		DWORD menuColor = gDrawManager.dwGetTeamColor(gLocalPlayerVars.team);

		if(yy == 0)
		{
			int curr = 0;

			for(auto &hack : hacks)
			{
				CDrawManager::font_size_t size = gDrawManager.GetPixelTextSize("hud", hack->name());
				size.length += 2;
				size.height += 2;
				if(curr == iIndex)
				{
					gDrawManager.DrawRect(x, y + yy, size.length, size.height, COLOR_BLACK);
					gDrawManager.OutlineRect(x, y + yy, size.length, size.height, COLOR_MENU_OFF);
					gDrawManager.DrawString("hud", x + 2, y + yy + 1, COLOR_MENU_OFF, hack->name());
				}
				else
				{
					gDrawManager.DrawRect(x, y + yy, size.length, size.height, COLOR_BLACK);
					gDrawManager.OutlineRect(x, y + yy, size.length, size.height, menuColor);
					gDrawManager.DrawString("hud", x + 2, y + yy + 2, menuColor, hack->name());
				}
				yy += size.height + 4;
				curr++;

				menux = max(menux, size.length);
			}
		}
		gDrawManager.DrawString(XorString("hud"), x + 12, y - (h + 2), menuColor, XorString("F1Public Hack \"github.com/josh33901/F1Public\""));

		y += 32;

		int xx = menux + 105;

		drawList(hacks[iIndex]->variables, menuColor, menux, y, xx, menuw, h);

		return;
	}
	catch(...)
	{
		Log::Debug(XorString("Error with cheat menu!"));
		throw;
	}
}

void CMenu::drawList(vecVars &list, DWORD color, int x, int y, int xx, int w, int h)
{

	// TODO maybe check first if there is an open switch to resolve highlighting issues

	// posibility for this is that every var vector has its own iiIndex that it can set to and arbitary value
	// outside its range in order to have no index, therefore eliminating the problem

	int iMenuItems = list.size();


	// draw the background box
	gDrawManager.DrawRect(x, y, w, iMenuItems * h, COLOR_BACK);
	gDrawManager.OutlineRect(x, y, w, iMenuItems * h, color);

	for(int i = 0; i < iMenuItems; i++)
	{
		var &curr = list[i];

		if(curr.getType() == type_t::Switch)
		{
			if(curr.bGet())
			{
				int newx = x + w + 4;
				int newxx = w + 4 + xx;

				drawList(curr.vars, color, newx, y, newxx, w, h);
			}
		}

		if(i != list.index)
		{
			gDrawManager.DrawString(XorString("hud"), x + 2, y + (h * i), COLOR_MENU_OFF, curr.name_.c_str());
			gDrawManager.DrawString(XorString("hud"), xx, y + (h * i), COLOR_MENU_OFF, curr.print()); // call print for types
		}
		else
		{
			gDrawManager.DrawRect(x + 1, y + (h * i), w - 2, h, COLORCODE(255, 255, 255, 80));
			gDrawManager.DrawString(XorString("hud"), x + 2, y + (h * i), color, curr.name_.c_str());
			gDrawManager.DrawString(XorString("hud"), xx, y + (h * i), color, curr.print()); // call print for types
		}
	}
}

void CMenu::addHack(IHack *hack)
{
	hacks.emplace_back(hack);
}

void CMenu::get()
{}
