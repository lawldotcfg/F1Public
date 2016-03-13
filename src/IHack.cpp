#include "IHack.h"

#include "SDK.h"

void var::increment()
{
	if(type_ == type_t::Bool)
		bVal = true;
	if(type_ == type_t::Float)
	{
		if(fVal <= max_.fMax)
		{
			if((fVal + step_.fStep) <= max_.fMax)
			{
				fVal += step_.fStep;
			}
		}
	}
	if(type_ == type_t::Int)
	{
		if(iVal <= max_.iMax)
		{
			if((iVal + step_.iStep) <= max_.iMax)
			{
				iVal += step_.iStep;
			}
		}
	}
}

void var::decrement()
{
	if(type_ == type_t::Bool)
		bVal = false;
	else if(type_ == type_t::Float)
	{
		if(fVal >= min_.fMin)
		{
			if((fVal - step_.fStep) >= min_.fMin)
			{
				fVal -= step_.fStep;
			}
		}
	}
	if(type_ == type_t::Int)
	{
		if(iVal >= min_.iMin)
		{
			if((iVal - step_.iStep) >= min_.iMin)
			{
				iVal -= step_.iStep;
			}
		}
	}
}

const char *var::print() const
{
	char *buf;
	// bool
	if(type_ == type_t::Bool)
	{
		return bVal ? "true" : "false";
	}

	if(type_ == type_t::Switch)
	{
		return bVal ? "open" : "close";
	}

	// float
	// TODO maybe allow manual precision setting
	if(type_ == type_t::Float)
	{
		buf = new char[127];
		sprintf_s(buf, 127, "%2.2f", fVal);
		return buf;
	}

	// int
	buf = new char[127];
	sprintf_s(buf, 127, "%i", iVal);
	return buf;
}