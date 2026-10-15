/// @file cmn_tcp_cmctr.h
/// @brief Содержит описание класса для обмена данными PAC<->сервер по
/// протоколу TCP (единая реализация для Linux и Windows).
///
/// @author  Иванюк Дмитрий Сергеевич.

#ifndef CMN_TCP_CMCTR_H
#define CMN_TCP_CMCTR_H

#ifdef WIN_OS
#include <winsock2.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#endif

#include <vector>

#include "tcp_cmctr.h"
#include "dtime.h"

//-----------------------------------------------------------------------------
/// @brief Cостояние сокета.
struct socket_state
    {
    int socket;
    int active;      ///< Сокет активен.
    int init;        ///< Сокет только что был активирован.
    int is_listener; ///< Сокет является инициатором соединения ( = 0 ) /
                     ///< сокет является слушателем ( != 0 ).
    int evaluated;   ///< В данном цикле уже произошел обмен информацией
                     ///< по данному сокету.
    int ismodbus;    ///< Сокет принадлежит к сервису modbus.
    sockaddr_in sin; ///< Адрес клиента.

    stat_time recv_stat;  ///< Статистика работы с сокетом.
    stat_time send_stat;  ///< Статистика работы с сокетом.
    };
//-----------------------------------------------------------------------------
/// @brief Коммуникатор - обмен данными PAC<->сервер (единая реализация
/// для Linux и Windows).
class tcp_communicator_impl : public tcp_communicator
    {
    public:
        tcp_communicator_impl( const char *name_rus,
            const char *name_eng );
        virtual ~tcp_communicator_impl();

        /// @brief Итерация обмена данными с сервером.
        int evaluate() override;

    private:
        sockaddr_in ssin;       ///< Адрес клиента.
        u_int sin_len;          ///< Длина адреса.

        int modbus_socket = 0;  ///< Модбас-сокет.
        int slave_socket = 0;   ///< Слейв-сокет при подключении клиента.
        int rc = 0;             ///< Код возврата select.

        /// @brief Посылка ответных данных на сервер.
        ///
        /// @param idx - индекс сокета в таблице.
        int do_echo( int idx );

        uint32_t glob_last_transfer_time;  ///< Время последней успешной
                                           ///< передачи данных.

        timeval tv;                       ///< Задержка ожидания функции
                                          ///< опроса состояний сокетов.
        fd_set rfds;                      ///< Набор дескрипторов сокетных
                                          ///< файлов для чтения.
        std::vector< socket_state > sst;  ///< Таблица состояния сокетов.
        int netOK;                        ///< Признак успешной
                                          ///< инициализации сети.

        /// @brief Уничтожение сокетов.
        void killsockets();

        /// @brief Инициализация сети.
        int net_init();

        /// @brief Закрытие сети.
        void net_terminate();
    };
//-----------------------------------------------------------------------------
#endif // CMN_TCP_CMCTR_H
