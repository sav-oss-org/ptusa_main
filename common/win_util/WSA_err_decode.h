/// @file WSA_err_decode.h.h
/// @brief Содержит функцию расшифровки ошибок библиотеки работы с сокетами
/// Windows.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#pragma once
/// @brief Расшифровка последней ошибки библиотеки WSA.
const char* WSA_Last_Err_Decode();

/// @brief Расшифровка ошибок библиотеки WSA.
///
/// @param err_code [ in ] - код ошибки.
///
/// @return Строка с расшифровкой ошибки.
const char* WSA_Err_Decode( int err_code );
