#pragma once

#include "SDK.h"

template <typename T = CBaseEntity> // default entity is CBaseEntity
class CEntity
{
	int ent;

	inline T *getEnt()
	{
		return reinterpret_cast<T *>(gInts.EntList->GetClientEntity(ent));
	}

public:
	// must take an entity
	CEntity(int index)
	{
		ent = index;
	}

	CEntity(CBaseEntity *entity)
	{
		if(!entity)
			Log::Fatal("Invalid entity passed to CEntity Constructer");
		ent = entity->GetIndex();
	}

	CEntity(CEntity &other)
	{
		this->ent = other.index();
	}

	template <typename T>
	CEntity(CEntity<T> &other)
	{
		this->ent = other.index();
	}

	// is entity null?
	inline bool isNull()
	{
		return getEnt() ? false : true;
	}

	// -> function returns ent-> for easier access
	inline T *operator->()
	{
		return getEnt();
	}

	// get index of entity
	inline int index()
	{
		return ent;
	}

	template <typename t>
	inline t get(DWORD offset)
	{
		return *(t *)((DWORD)(getEnt()) + ((DWORD)(offset)));
	}

	template <typename t>
	inline t *getPtr(DWORD offset)
	{
		return (t *)((DWORD)(getEnt()) + ((DWORD)(offset)));
	}

	template <typename t>
	inline void set(DWORD offset, t val)
	{
		*(t *)((DWORD)(getEnt()) + ((DWORD)(offset))) = val;
	}

	template <typename t>
	inline void setPtr(DWORD offset, t *val)
	{
		(t *)((DWORD)(getEnt()) + ((DWORD)(offset))) = val;
	}

        // use this to get the entity pointer
	template <typename t>
	inline t *castToPointer()
	{
		return reinterpret_cast<t *>(getEnt());
	}

        // convert to a different CEntity type
	template <typename t>
	inline CEntity<t> castTo()
	{
		return CEntity<t>(ent);
	}
};
