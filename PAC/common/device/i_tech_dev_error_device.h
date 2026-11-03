#ifndef I_TECH_DEV_ERROR_DEVICE_H
#define I_TECH_DEV_ERROR_DEVICE_H

#include "param_ex.h"

/// @brief Интерфейс объекта, необходимый для обработки его ошибки.
class i_simple_error
    {
    public:
        virtual ~i_simple_error() = default;

        virtual void set_error_params( saved_params_u_int_4* err_par ) = 0;
        virtual const char* get_name() const = 0;
        virtual const char* get_error_description() = 0;
        virtual int get_error_id() = 0;
        virtual int get_state() const = 0;
        virtual u_int_4 get_serial_n() const = 0;
        virtual int get_error_type() const = 0;
    };

#endif // I_TECH_DEV_ERROR_DEVICE_H