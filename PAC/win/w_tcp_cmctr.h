/// @file w_tcp_cmctr.h
/// @brief Совместимость: ссылается на единую реализацию
/// tcp_communicator_impl.

#ifndef TCP_CMCTR_WIN
#define TCP_CMCTR_WIN

#include "cmn_tcp_cmctr.h"

using tcp_communicator_win = tcp_communicator_impl;

#endif //TCP_CMCTR_WIN
