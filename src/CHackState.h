#pragma once

#include "SDK.h"

class HState
{

public:

	// this is a flag system
	enum hackStatus
	{
		loaded = 0x0, // the dll is first loaded
		baseInited = 0x1, // we have completed the init function
		steamInited = 0x2,
		clientModeInited = 0x4,
		FullyInited = 0x8,
		introComplete = 0x10, // the intro completes
		inGame = 0x20, // we are inGame
		outOfGame = 0x40, // we have been in a game but are no longer in one
	};
	
	HState(HState &other) = delete;

	HState operator=(HState &other) = delete;

	int getStatus() const
	{
		return status;
	}
	void addStatus(hackStatus s)
	{
		status |= s;
	}
	bool checkStatus(hackStatus s)
	{
		return status & s;
	}

	static HState *instance()
	{
		static HState state;
		return &state;
	}

	static void init();

	void think();

private:

	int status;

	HState()
	{
		// when we are constructed the hack has just been loaded
		status = loaded;
	}
};
