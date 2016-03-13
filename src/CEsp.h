#pragma once

#include "IHack.h"

class CESP : public IHack
{
	var enabled_bool = var("Enabled", type_t::Bool);
	var box_bool = var("ESP Box", type_t::Bool);
	var guid_bool = var("GUID", type_t::Bool);
	var name_bool = var("Name", type_t::Bool);
	var health_bool = var("Health", type_t::Bool);
	var friend_bool = var("Friends?", type_t::Bool);
	var index_bool = var("Index", type_t::Bool);
	var id_bool = var("Object ID", type_t::Bool);

public:
	CESP()
	{
		variables.push_back(enabled_bool);
		variables.push_back(box_bool);
		variables.push_back(guid_bool);
		variables.push_back(name_bool);
		variables.push_back(health_bool);
		variables.push_back(friend_bool);
		variables.push_back(index_bool);
		variables.push_back(id_bool);
	}

	const char *name() const override;

	bool inEntityLoop(int index) override;
};
