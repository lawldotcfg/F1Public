#pragma once

// source engine factory wrapper

typedef void *(__cdecl *CreateInterface_t)(const char *, int *);
typedef void *(*CreateInterfaceFn)(const char *pName, int *pReturnCode);

class srcFactory
{
public:
	srcFactory(void *fac)
	{
		factory = reinterpret_cast<CreateInterface_t>(fac);
	}

	template<typename T>
	T get(const char *name, int *ret = nullptr)
	{
		return static_cast<T>(factory(name, ret));
	}

	// backwards compatability
	// remove when done
	void *operator()(const char *name, int *ret)
	{
		return factory(name, ret);
	}

private:
	CreateInterface_t factory;
};