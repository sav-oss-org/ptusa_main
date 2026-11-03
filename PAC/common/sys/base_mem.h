/// @file base_mem.h
/// @brief Содержит описания классов, которые реализуют аппаратную часть PAC -
/// различную память - на абстрактном уровне.
///

#pragma once

#include <cstddef>

#include "console.h"
#include "s_types.h"

#include "smart_ptr.h"
#include <filesystem>
//-----------------------------------------------------------------------------
/// @brief Интерфейс доступа к памяти.
class i_memory
    {
    public:
        virtual ~i_memory() = default;

        /// @brief Загрузка массива байт.
        ///
        /// @return > 0 - ошибка.
        /// @return = 0 - ОК.
        virtual int load_data() = 0;

        /// @brief Безопасное сохранение данных.
        /// @return - результат.
        virtual int safe_save() = 0;

        /// @brief Сохранение неизменяемого снимка вместо рабочего массива.
        /// Не пишет в общий журнал: вызывается из фонового потока.
        virtual int safe_save( const std::byte* data ) = 0;

        /// @brief Получение размера памяти в байтах.
        ///
        /// @return - размер памяти в байтах.
        virtual u_int get_size() const = 0;

        virtual void zero_fill() = 0;

        virtual std::byte* get_data() = 0;
    };
//-----------------------------------------------------------------------------
/// @brief Работа с энергонезависимой ОЗУ (Static Memory).
///
/// Имеет ограничения на количество циклов записи/чтения - 1 миллион.
class SRAM : public i_memory
    {
    public:
        SRAM( const std::filesystem::path& file_name, u_int size );

        ~SRAM() override;

        /// @brief Метод интерфейса @ref i_memory.
        int load_data() override;

        int safe_save() override;

        int safe_save( const std::byte* data ) override;

        std::byte* get_data() override;

        void zero_fill() override;

        u_int get_size() const override;
    private:

    SRAM( const SRAM& ) = delete;
    SRAM( SRAM&& ) = delete;
    SRAM& operator=( const SRAM& ) = delete;
    SRAM& operator=( SRAM&& ) = delete;

    std::filesystem::path file_path;
    std::filesystem::path tmp_path;

    u_int total_size;

    /// Рабочий массив параметров.
    std::byte* params_data{};

    int safe_save_buffer( const std::byte* data, bool log_messages );
    };
