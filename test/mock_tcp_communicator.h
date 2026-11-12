#pragma once
#include "includes.h"
#include "tcp_cmctr.h"

class mock_tcp_communicator : public tcp_communicator
    {
    public:
        MOCK_METHOD( srv_ptr, reg_service, ( u_char srv_id, srv_ptr fk ) );

        int evaluate()
            {
            return 0;
            };
    };

class test_tcp_communicator
    {
    public:
        static void replaceEntity( mock_tcp_communicator* p )
            {
            tcp_communicator::is_init = true;
            tcp_communicator::instance = p;
            }

        static void removeObject()
            {
            tcp_communicator::instance = NULL;
            tcp_communicator::is_init = false;
            }
    };
