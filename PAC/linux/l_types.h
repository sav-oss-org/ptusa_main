/// @file l_types.h
/// @brief Некоторые переименования типов простых чисел для обеспечения
/// совместимости при сериализации вне зависимости от разрядности процессора
/// для ОС Linux.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev: 223 $.\n
/// @$Author: id $.\n
/// @$Date:: 2011-02-17 09:39:32#$.

#ifndef L_TYPES_H
#define L_TYPES_H

typedef short int int_2;
typedef int int_4;
typedef unsigned short int u_int_2;

typedef unsigned int u_int_4;

#endif // L_TYPES_H
