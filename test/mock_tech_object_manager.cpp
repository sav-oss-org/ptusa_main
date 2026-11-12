#include "mock_tech_object_manager.h"

mock_tech_object_manager* mock_tech_object_manager::instance = NULL;

mock_tech_object_manager::mock_tech_object_manager()
    {
    }

mock_tech_object_manager* mock_tech_object_manager::get_instance()
    {
    return mock_tech_object_manager::instance;
    }

void mock_tech_object_manager::set_instance( mock_tech_object_manager* p )
    {
    mock_tech_object_manager::instance = p;
    }