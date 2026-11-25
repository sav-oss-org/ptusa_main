#pragma once
#include "includes.h"
#include "prj_mngr.h"

class mock_project_manager : public project_manager
{
    public:
	MOCK_METHOD(int, lua_load_configuration, ());
};

class test_project_manager
{
    public:
	static void replaceEntity(mock_project_manager* p)
	{
		project_manager::instance = p;
	}

	static void removeObject()
	{
		project_manager::instance = NULL;
	}
};