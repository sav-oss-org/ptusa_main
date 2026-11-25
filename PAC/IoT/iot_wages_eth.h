#pragma once

#include <memory>
#include <array>

#include "iot_base.h"
#include "tcp_client.h"

class iot_wages_eth : public i_iot_node
    {
    public:
        iot_wages_eth( unsigned int id, const char* ip, unsigned int port,
            const char* name );

        void evaluate();

        int get_wages_state() const;

        float get_wages_value() const;

        void set_wages_value( float new_value );

        void set_wages_state( int new_state );

        void convert_value();

        void direct_set_tcp_buff( const char* new_value, size_t size,
            int new_status );

    private:
        enum class CONSTANTS
            {
            BUFF_SIZE = 256,
            SEND_RECEIVE_TIMEOUT = 150,
            };

        int status = 0;
        int state = 0;
        float value = .0f;
        std::unique_ptr < tcp_client > tc;

        std::array<char, 50 > name{};
    };
