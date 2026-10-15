/// @file tcp_cmctr.h
/// @brief Содержит описания классов, которые предназначены для обмена данными
/// PAC-сервер по протоколу TCP.
///
/// @author  Иванюк Дмитрий Сергеевич.
///
/// @par Описание директив препроцессора:
///
/// @par Текущая версия:
/// @$Rev$.\n
/// @$Author$.\n
/// @$Date::                     $.

#ifndef TCP_CMCTR_H
#define TCP_CMCTR_H

#include <stdio.h>
#include <map>

#include "smart_ptr.h"
#include "dtime.h"

class tcp_client;

#ifdef WIN_OS
const char* WSA_Last_Err_Decode();
#endif // WIN_OS

//-----------------------------------------------------------------------------
/// @brief Базовый класс коммуникатор - обмен данными PAC-сервер.
class tcp_communicator
    {
    // Friendly класс предназначен только для тестирования
    // и не должен использоваться в других целях
#ifdef PTUSA_TEST
    friend class test_tcp_communicator;
#endif
    public:
        /// @brief Определение функции сервиса.
        typedef long int srv_proc( long int, u_char *, u_char * );
        typedef srv_proc *srv_ptr;

        /// @brief Получение единственного экземпляра класса для работы с
        /// коммуникатором.
        ///
        /// @return - указатель на единственный объект класса @ref
        /// tcp_communicator.
        static tcp_communicator* get_instance();

#ifdef PTUSA_TEST
        static bool is_init;

        static void clear_instance();
#endif //PTUSA_TEST

        static void init_instance( const char *name_rus, const char *name_eng );

        /// @brief Итерация обмена данными с сервером.
        virtual int evaluate() = 0;

        /// @brief Добавление пользовательской функции по обмену данными -
        /// сервиса.
        ///
        /// @param srv_id - номер, за которым будет закреплен сервис.
        /// @param fk     - указатель на объект выделенного блока памяти.
        virtual srv_ptr reg_service( u_char srv_id, srv_ptr fk );

        /// @brief Получение сетевого имени PAC.
        ///
        /// @return - сетевое имя PAC на русском языке.
        char* get_host_name_rus();

        /// @brief Получение сетевого имени PAC.
        ///
        /// @return - сетевое имя PAC на английском языке.
        char* get_host_name_eng();

        virtual int add_async_client(tcp_client* client);
        virtual int remove_async_client(tcp_client* client);

        virtual ~tcp_communicator();

        static void set_port( int new_port, int new_port_modbus );

        static int get_port();

        static int get_modbus_port();

        static int get_master_socket();

        enum CONSTANTS
            {
            BUFSIZE = 500 * 1024,   ///< Размер буфера.
            MAX_SOCKETS = 64,       ///< Максимальное количество сокетов.

            QLEN = MAX_SOCKETS - 1, ///< Максимальное количество соединений.

            TC_MAX_HOST_NAME = 70,
            TC_MAX_SERVICE_NUMBER = 16,
            };

        static bool checkBuff( int s );

        static int recvtimeout( int s, u_char* buf,
            int len, long int sec, long int usec,
            const char* IP = nullptr, const char* name = nullptr,
            stat_time* stat = nullptr );

        static int sendall( int sockfd, u_char* buf, int len,
            int sec, int usec,
            const char* IP = nullptr, const char* name = nullptr,
            stat_time* stat = nullptr );

    protected:
        static int master_socket;   ///< Мастер-сокет для прослушивания.

        static int port;            ///< Порт.
        static int port_modbus;

        tcp_communicator();

        //ERRORS DEFINITION
        enum ERRORS
            {
            ERR_RETRIVE       = 1,
            ERR_WRONG_SERVICE = 3,
            ERR_TRANSMIT      = 4,
            ERR_WRONG_CMD     = 5,
            };

        //COMMANDS DEFINITION
        enum COMMANDS
            {
            FRAME_SINGLE = 1,
            AKN_ERR      = 7,
            AKN_DATA     = 8,
            AKN_OK       = 12,
            };

        static auto_smart_ptr < tcp_communicator > instance;///< Экземпляр класса.

        srv_ptr services[ TC_MAX_SERVICE_NUMBER ];  ///< Массив сервисов.

        char host_name_rus[ TC_MAX_HOST_NAME + 1] = { 0 }; ///< Сетевое имя PAC.
        char host_name_eng[ TC_MAX_HOST_NAME + 1] = { 0 }; ///< Сетевое eng имя PAC.

        int max_cycles;         ///< Максимальное количество циклов обработки состояний сокетов за 1 проход.
        int glob_cmctr_ok;      ///< Флаг активности обмена с сервером.

        u_int   in_buffer_count;        ///< Количество данных в буфере.
        u_char  buf[ BUFSIZE ] = { 0 }; ///< Буфер.

        u_char pidx;            ///< Номер ответа.
        int    net_id;          ///< Номер PAC.

        std::map<int, tcp_client*> *clients;

        void _ErrorAkn( u_char error );
        void _AknData( u_long len );
        void _AknOK();
    };
//-----------------------------------------------------------------------------
#define G_CMMCTR tcp_communicator::get_instance()

#endif //TCP_CMCTR_H
