/// @file prj_mngr.h
/// @brief Работа с загрузкой конфигурации проекта, инициализация объектов и
/// т.д.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef PRJ_MANAGER_H
#define PRJ_MANAGER_H

#include "string"

#include "smart_ptr.h"
#include "base_mem.h"

//-----------------------------------------------------------------------------
/// @brief Инициализация проекта.
///
///
class project_manager
    {
    // Friendly класс предназначен только для тестирования
    // и не должен использоваться в других целях
#ifdef PTUSA_TEST
    friend class test_project_manager;
#endif
    public:
        /// @brief Обработка параметров командной строки.
        ///
        /// @param argc - количество параметров.
        /// @param argv - массив параметров.
        int proc_main_params( int argc, const char *argv[] );

        /// @brief Применение сохраненного режима OPC UA.
        ///
        /// @param show_msg - выводить ли сообщение о примененном режиме.
        ///
        /// @return 0 - ОК.
        int apply_opc_mode( bool show_msg = true ) const;

        /// @brief Загрузка системной конфигурации проекта на основе скрипта.
        ///
        /// Системная конфигурация была предварительно загружена из файла
        /// wago.main.plua.
        virtual int lua_load_configuration();

        /// @brief Получение единственного экземпляра класса.
        static project_manager* get_instance();

        virtual ~project_manager() = default;

        /// @brief Установка путей к файлам Lua.
        int init_path( const char* path );

        /// @brief Установка путей к системным файлам Lua.
        int init_sys_path( const char* sys_path );

        /// @brief Установка путей к пользовательским файлам Lua.
        int init_extra_paths( const char* paths );

        std::string main_script = "";//Скрипт Lua.
        std::string sys_path = "";   //Путь к системным скриптам Lua.
        std::string path = "";       //Путь к описывающим проект скриптам Lua.
        std::string extra_paths = "";//Дополнительный путь к user-скриптам Lua.

        unsigned int sleep_time_ms = 0;

    protected:
        void log_opc_mode() const;

        enum class OPC_MODE
            {
            UNDEFINED,
            OFF,
            READ_ONLY,
            READ_WRITE
            };

        OPC_MODE opc_mode = OPC_MODE::UNDEFINED;

        /// @brief Единственный экземпляр класса.
        static auto_smart_ptr < project_manager > instance;
    };
//-----------------------------------------------------------------------------
#define G_PROJECT_MANAGER project_manager::get_instance()
//-----------------------------------------------------------------------------
#endif // PRJ_MANAGER_H
