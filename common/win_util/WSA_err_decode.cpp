#include "WSA_err_decode.h"
#include <windows.h>
//-----------------------------------------------------------------------------
const char* WSA_Last_Err_Decode()
    {
    return WSA_Err_Decode( WSAGetLastError() );
    }
//-----------------------------------------------------------------------------
const char* WSA_Err_Decode( int err_code )
    {
    switch ( err_code )
        {
        case ERROR_SUCCESS:
            return "The operation completed successfully.";

        case WSANOTINITIALISED:
            return "A successful WSAStartup must occur before using this function.";
        case WSAENETDOWN:
            return "The network subsystem has failed.";
        case WSAHOST_NOT_FOUND:
            return "Authoritative answer host not found.";
        case WSATRY_AGAIN:
            return "Non authoritative host not found, or server failure.";
        case WSANO_RECOVERY:
            return "A nonrecoverable error occurred.";
        case WSANO_DATA:
            return "Valid name, no data record of requested type.";
        case WSAEADDRINUSE:
            return "The specified address is already in use.";
        case WSAEINTR:
            return "The (blocking) call was canceled through WSACancelBlockingCall.";
        case WSAEAFNOSUPPORT:
            return "The specified address family is not supported.";
        case WSAECONNREFUSED:
            return "The attempt to connect was forcefully rejected.";
        case WSAEFAULT:
            return "The name or the namelen argument is not a valid part of the user "
                   "address space, the namelen argument is too small, or the name "
                   "argument contains incorrect address format for the associated "
                   "address family. In case WSAStartup - lpWSAData is not a valid pointer";
        case WSAENOTCONN:
            return "The socket is not connected.";
        case WSAEINPROGRESS:
            return "A blocking Windows Sockets 1.1 call is in progress, or the service "
                   "provider is still processing a callback function.";
        case WSAENETRESET:
            return "The connection has been broken due to the remote host resetting.";
        case WSAEALREADY:
            return "A nonblocking connect call is in progress on the specified socket. "
                   "Note  In order to preserve backward compatibility, this error is "
                   "reported as WSAEINVAL to Windows Sockets 1.1 applications that link "
                   "to either WINSOCK.DLL or WSOCK32.DLL.";
        case WSAEADDRNOTAVAIL:
            return "The specified address is not available from the local machine.";
        case WSAEMFILE:
            return "No more socket descriptors are available.";
        case WSAENOBUFS:
            return "No buffer space is available. The socket cannot be created.";
        case WSAENOTSOCK:
            return "The descriptor is not a socket.";
        case WSAEOPNOTSUPP:
            return "MSG_OOB was specified, but the socket is not stream style such "
                   "as type SOCK_STREAM, out-of-band data is not supported in the "
                   "communication domain associated with this socket, or the socket "
                   "is unidirectional and supports only send operations.";
        case WSAESHUTDOWN:
            return "The socket has been shut down; it is not possible to recv on a "
                   "socket after shutdown has been invoked with how set to SD_RECEIVE "
                   "or SD_BOTH.";
        case WSAETIMEDOUT:
            return "Attempt to connect timed out without establishing a connection.";
        case WSAECONNRESET:
            return "The virtual circuit was reset by the remote side "
                   "executing a "
                   "hard"
                   " or "
                   "abortive"
                   " close. The "
                   "application should close the socket as it is no "
                   "longer usable. On a UDP datagram socket this error "
                   "would indicate that a previous send operation resulted "
                   "in an ICMP "
                   "Port Unreachable"
                   " message.";
        case WSAEWOULDBLOCK:
            return "The socket is marked as nonblocking and the connection cannot "
                   "be completed immediately. It is possible to select the socket "
                   "while it is connecting by selecting it for writing.";
        case WSAEMSGSIZE:
            return "The message was too large to fit into the specified buffer and "
                   "was truncated.";
        case WSAEACCES:
            return "Attempt to connect datagram socket to broadcast address failed "
                   "because setsockopt SO_BROADCAST is not enabled.";
        case WSAEPROTONOSUPPORT:
            return "The specified protocol is not supported.";
        case WSAEPROTOTYPE:
            return "The specified protocol is the wrong type for this socket.";
        case WSAESOCKTNOSUPPORT:
            return "The specified socket type is not supported in this address family.";
        case WSAEINVAL:
            return "The parameter g specified is not valid.";
        case WSAECONNABORTED:
            return "The virtual circuit was terminated due to a time-out or other "
                   "failure. The application should close the socket as it is no "
                   "longer usable.";
        case WSAEISCONN:
            return "The socket is already connected (connection-oriented sockets only).";
        case WSAENETUNREACH:
            return "The network cannot be reached from this host at this time.";
            //Errors, returned WSAStartup
        case WSASYSNOTREADY:
            return "Underlying network subsystem is not ready for network communication.";
        case WSAVERNOTSUPPORTED:
            return "The version of Windows Sockets support requested is not "
                   "provided by this particular Windows Sockets implementation.";
        case WSAEPROCLIM:
            return "Limit on the number of tasks supported by the Windows Sockets "
                   "implementation has been reached.";
        }
    return "Unknown error.";
    }
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
