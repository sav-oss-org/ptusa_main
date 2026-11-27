/// @file i_tech_def.h
/// @brief Реализована интерфейс следующих объектов, описывающих технологический
/// процесс - танк (технологическая емкость), гребенка (совокупность клапанов)
/// и т.д.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 868 $.\n
/// @$Author: id $.\n
/// @$Date:: 2013-12-26 10:29:25#$.
#ifndef I_TECH_H
#define I_TECH_H

#include "s_types.h"
#include "param_ex.h"
//-----------------------------------------------------------------------------
/// @brief Работа с технологическим объектом.
///
/// Базовый интерфейсный класс для технологического объекта (танка, гребенки).
/// Содержит основные методы работы - работа с режимами и т.д.
///
class i_tech_object
    {
#ifndef __GNUC__
#pragma region Удобная отладочная печать с учётом вложенности вызовов.
#endif

    public:
        static const char* get_prefix()
            {
            return white_spaces;
            }

        enum ERR_MSG_TYPES
            {
            ERR_CANT_ON,
            ERR_ON_WITH_ERRORS,
            ERR_OFF,
            ERR_OFF_AND_ON,
            ERR_DURING_WORK,
            ERR_ALARM,

            ERR_TO_FAIL_STATE,
            ERR_CANT_ON_2_OPER,  //Уже включена блокирующая операция.
            ERR_CANT_ON_2_OBJ,  //Уже включена блокирующая операция другого объекта.
            };

    protected:
        static char white_spaces[ 256 ];

#ifndef __GNUC__
#pragma endregion
#endif

    public:
        /// @brief Включение/выключение режима.
        ///
        /// @param mode      - режим.
        /// @param new_state - новое состояние режима.
        ///
        /// @return  0 - режим включен (отключен).
        /// @return  1 - режим ранее был включен (отключен).
        /// @return  3 - нет такого режима.
        virtual int set_mode( u_int mode, int new_state ) = 0;

        virtual const char* get_name() const = 0;

        virtual float get_step_param( u_int idx ) const = 0;

        virtual const saved_params_float* get_params() const = 0;

        virtual int set_err_msg( const char* err_msg, int mode,
            int new_mode = 0, ERR_MSG_TYPES type = ERR_CANT_ON ) = 0;

        virtual int check_operation_on(
            u_int operation_n, bool show_error = true ) = 0;
    };

#endif  // I_TECH_H
