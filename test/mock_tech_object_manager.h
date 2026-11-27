#include "includes.h"
#include "tech_def.h"

class mock_tech_object_manager : public tech_object_manager
    {
    protected:
        static mock_tech_object_manager* instance;

    public:
        mock_tech_object_manager();

        MOCK_METHOD( int, init_objects, () );

        static mock_tech_object_manager* get_instance();
        static void set_instance( mock_tech_object_manager* p );
    };