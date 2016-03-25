#pragma once

#include <string>
#include <vector>

#include "bf_buffer.h"

#define F1_ENABLE_CONSOLE_VARS

enum class ButtonCode_t;

class CUserCmd;
class SVC_GetCvarValue;

enum class type_t
{
	Bool = 0,
	Int,
	Float,
	Switch, // uses bool to decide whether it is open or closed but also has a vector of vars inside of it
};

class var;

struct vecVars
{
	std::vector<var> vars;
	int index;
	vecVars *parent;

	vecVars()
	{
		index  = 0;
		parent = nullptr;
	}

	vecVars(vecVars *p)
	{
		index  = 0;
		parent = p;
	}

	void push_back(const var &v)
	{
		return vars.push_back(v);
	}

	size_t size()
	{
		return vars.size();
	}

	var &operator[](size_t idx)
	{
		return vars[idx];
	}

	var &at(size_t idx)
	{
		return vars[idx];
	}
	var &active()
	{
		return vars[index];
	}
};

// stupid, increadibly hacky var class for bool, int and float
// allows us to store all of these types in one type (for vectors)
class var
{
	type_t type_;

public:
	std::string name_;
	vecVars vars;

	// bool is stored in the int
	union
	{
		float fVal;
		int iVal;
		bool bVal;
	};

	// max
	union Max_t
	{
		float fMax;
		int iMax;
	};
	Max_t max_;

	// min
	union Min_t
	{
		float fMin;
		int iMin;
	};
	Min_t min_;

	// step
	union Step_t
	{
		float fStep;
		int iStep;
	};
	Step_t step_;

	union Default_t
	{
		float fDef;
		int iDef;
		bool bDef;
	};

	void increment();

	void decrement();

	const char *print() const;

	var(std::string name, type_t type)
	{
		name_ = name;
		type_ = type;

		fVal = 0.0f;
		iVal = 0;
		bVal = false;
		//vars.parent = this;
	}

	// all vars (except bools) must init!

	void init(float min, float max, float step, float def)
	{
		min_.fMin   = min;
		max_.fMax   = max;
		step_.fStep = step;

		fVal = def;
	}

	void init(int min, int max, int step, int def)
	{
		min_.iMin   = min;
		max_.iMax   = max;
		step_.iStep = step;

		iVal = def;
	}

	template <typename T>
	__declspec(deprecated("this function is replaced by the iGet bGet and fGet functions!")) T get()
	{
		if(std::is_same<T, double>() || std::is_same<T, float>())
			return fVal;
		if(std::is_same<T, int>())
			return iVal;
		if(std::is_same<T, bool>())
			return bVal;

		return T();
	}

	float fGet()
	{
		return fVal;
	}
	int iGet()
	{
		return iVal;
	}
	bool bGet()
	{
		return bVal;
	}

	type_t getType()
	{
		return type_;
	}
};

// base hack interface
class IHack
{
public:
	// vars for this hack
	vecVars variables;

	bool isOpen = false;

	virtual ~IHack()
	{
		// destroy the vecVars
		variables.~vecVars();
	}

	// each hack must impl the name function
	virtual const char *name() const = 0;

	// called on init
	virtual bool init()
	{
		return false;
	}

	// called during paint traverse
	virtual bool paint()
	{
		return false;
	}

	// called during clientmode createmove - takes a CUserCMD
	virtual bool move(CUserCmd *pUserCmd)
	{
		return false;
	}

	/*
	virtual bool chlmove(CUserCmd *pUserCmd)
	{
		return false;
	}
	*/

	// called during keyevent
	virtual bool keyEvent(ButtonCode_t keynum)
	{
		return false;
	}

	virtual bool inEntityLoop(int index)
	{
		return false;
	}

	virtual bool userMessage(int type, bf_read &data)
	{
		return false;
	}

	virtual bool emitSound(const char *sound)
	{
		return false;
	}
};
