#ifndef modbus_client_h__
#define modbus_client_h__
#include <array>
#include "tcp_client.h"
#include "iot_base.h"
#include "device/i_tech_dev_error_device.h"

class modbus_client: public i_simple_error
    {
    protected:
        std::array<char, 50 > name{};

        tcp_client* tcpclient;
        saved_params_u_int_4* error_params = nullptr;
        int modbus_expected_length{ 0 };
        int modbus_async_result{ 0 };
        unsigned char stationid{ 1 };
        unsigned int ormask{ 0 };
        unsigned int andmask{ 0xFFFF };

        ///< Начало буфера данных для операций записи
        static const int write_buff_start = 13;
        ///< Начало буфера данных для операций чтения
        static const int read_buff_start = 9;
        void init_frame(unsigned int address, unsigned int value, unsigned int seventh_byte);

    public:
        modbus_client(unsigned int id, const char* ip, unsigned int port = 502,
            uint32_t exchangetimeout = 50, const char * name = nullptr );

        void set_error_params( saved_params_u_int_4* err_par ) override;
        const char* get_name() const override;
        const char* get_error_description() override;
        int get_error_id() override;
        int get_state() const override;
        u_int_4 get_serial_n() const override;
        int get_error_type() const override;

        int get_id();
        //реализация функций протокола modbus
        int read_discrete_inputs(unsigned int start_address, unsigned int quantity);
        int read_coils(unsigned int start_address, unsigned int quantity);
        int read_holding_registers(unsigned int address, unsigned int quantity);
        int read_input_registers(unsigned int address, unsigned int quantity);
        int write_coil(unsigned int address, unsigned char value);
        int force_multiply_coils(unsigned int address, unsigned int quantity);
        int write_multiply_registers(unsigned int address, unsigned int quantity);
        int async_read_discrete_inputs(unsigned int start_address, unsigned int quantity);
        int async_read_coils(unsigned int start_address, unsigned int quantity);
        int async_read_holding_registers(unsigned int address, unsigned int quantity);
        int async_read_input_registers(unsigned int address, unsigned int quantity);
        int async_write_coil(unsigned int address, unsigned char value);
        int async_force_multiply_coils(unsigned int address, unsigned int quantity);
        int async_write_multiply_registers(unsigned int address, unsigned int quantity);
        int async_read_write_multiply_registers(unsigned int readaddress, unsigned int readquantity, unsigned int wrireaddress, unsigned int writequantity);
        int async_mask_write_register(unsigned int writeaddress, unsigned int andmask, unsigned int ormask);
        int async_mask_write_register(unsigned int writeaddress);
        int get_async_result();
        int get_connected_state();
        void set_station(unsigned char new_station_id);
        //функции для работы с буфером из lua
        void zero_output_buff(int startpos = write_buff_start);
        void set_byte(int address, unsigned char value);
        unsigned char get_byte(int address);
        void set_int2(unsigned int address, int_2 value);
        int_2 get_int2(unsigned int address);
        void set_int4(unsigned int address, int_4 value);
        int_4 get_int4(unsigned int address) const;

        void set_int4_ab_cd(unsigned int address, int_4 value);
        int_4 get_int4_ab_cd(unsigned int address) const;
        void set_int4_cd_ab(unsigned int address, int_4 value);
        int_4 get_int4_cd_ab(unsigned int address) const;
        void set_int4_dc_ba(unsigned int address, int_4 value);
        int_4 get_int4_dc_ba(unsigned int address) const;

        void set_float(unsigned int address, float value);
        float get_float(unsigned int address);
        void set_bit(unsigned int address, int value);
        int reg_get_bit(unsigned int reg, unsigned int offset);
        void reg_set_bit(unsigned int reg, unsigned int offset, int value);
        int get_bit(unsigned int address);
        void mask_reset();
        void mask_set_bit(int pos, int value);
        unsigned char reverse(unsigned char b);
        int swapBits(int x, int p1, int p2, int n);
        ~modbus_client();

        // Explicitly delete the copy constructors.
        modbus_client( modbus_client const& ) = delete;
        modbus_client( modbus_client&& ) = delete;
        modbus_client& operator=( modbus_client const& ) = delete;
        modbus_client& operator=( modbus_client&& ) = delete;

#ifndef PTUSA_TEST
    private:
#endif
        inline static std::vector<unsigned int> ids{};

#ifdef PTUSA_TEST
        void static clear_ids()
            {
            ids.clear();
            }
#endif

        static constexpr int ERROR_TYPE = 200;
    };
#endif // modbus_client_h__
