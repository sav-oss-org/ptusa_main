/// @file l_tcp_cmctr.h
/// @brief Совместимость: ссылается на единую реализацию
/// tcp_communicator_impl.

#ifndef TCP_CMCTR_LINUX
#define TCP_CMCTR_LINUX

#include "cmn_tcp_cmctr.h"

using tcp_communicator_linux = tcp_communicator_impl;

#endif //TCP_CMCTR_LINUX

