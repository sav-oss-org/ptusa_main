/// @file linux\PAC_PC\wago_PC.h
/// @brief Работа с Wago для PC.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
/// @c DEBUG   - компиляция c выводом отладочной информации в консоль.
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef IO_PC_H
#define IO_PC_H

#include "bus_coupler_io.h"

//-----------------------------------------------------------------------------
/// @brief Работа с модулями ввода/вывода Wago для PC.
///
///
class io_manager_PC : public io_manager
    {
    public:
        io_manager_PC();

        virtual ~io_manager_PC();

        int read_inputs();
        int write_outputs();
    };
//-----------------------------------------------------------------------------
#endif // IO_PC_H
