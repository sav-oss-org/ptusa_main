# PHOENIX BK ETH: Modbus/UDP

The `PHOENIX_BK_ETH` coupler (2702177) can be polled over UDP port 502. TCP is
the default. UDP affects only PHOENIX nodes; other I/O nodes continue using TCP.
The coupler and network must permit Modbus/UDP traffic before enabling the mode.

Start the console application with `--phoenix_modbus_udp` to enable UDP, or
`--phoenix_modbus_udp=false` to explicitly use TCP. The setting can be changed
at runtime:

```lua
G_PAC_INFO():set_phoenix_modbus_udp(true)  -- UDP
G_PAC_INFO():set_phoenix_modbus_udp(false) -- TCP
local udp_enabled = G_PAC_INFO():is_phoenix_modbus_udp()
```

The UDP response timeout defaults to 25 ms. Set
`--phoenix_modbus_udp_timeout_ms=40` to change it (valid range: 1..60000 ms).
This timeout starts after a UDP datagram is sent and does not change the TCP
I/O timeout. It can also be adjusted at runtime using
`G_PAC_INFO():set_phoenix_modbus_udp_timeout_ms(40)` or by writing
`SYSTEM.PHOENIX_MODBUS_UDP_TIMEOUT_MS=40` through the control server. The
current value is available through
`G_PAC_INFO():get_phoenix_modbus_udp_timeout_ms()` and the SYSTEM snapshot.

The control server publishes `SYSTEM.PHOENIX_MODBUS_UDP` as `0` or `1`. Write
`SYSTEM.PHOENIX_MODBUS_UDP=1` or `0` to change it, or issue `SYSTEM.CMD=301`
to enable and `SYSTEM.CMD=302` to disable UDP. The Lua debugger offers the same
commands in its controller command selector. The setting is runtime only and
does not modify saved controller parameters.

Each request gets a per-node 16-bit Modbus transaction ID. Counters are retained
across reconnects and wrap from 65535 to 0. At most one request per node
is in flight. A UDP response is accepted only when the transaction ID, MBAP
protocol/length, unit, function and payload match the request. Late responses
from earlier transactions are discarded. Switching transport closes PHOENIX
sockets and reconnects them in the selected mode at the next exchange boundary. Writes are confirmed
before the output image is updated.

A lost response fails the current exchange without closing the UDP socket or
waiting for the TCP reconnect backoff. The next cycle sends a fresh request;
old writes are not retransmitted. Empty, oversized, malformed and mismatched
datagrams are discarded without extending the response deadline. Socket errors
still trigger reconnection. A failed PHOENIX status-register read also fails the
read phase for that node and prevents output writes until a successful read.

Transaction matching cannot distinguish packets delayed for a full 65536-request
sequence wrap on the same socket. UDP also cannot guarantee the arrival order or
exactly-once execution of writes at the device; MBAP IDs correlate replies but
do not make the coupler reject an old request.

## MarkServerTester loopback stand

Use the **Modbus UDP server** with port **502**, **Unit ID 0** and
`max_addresses = 10000` (increase it for larger configured I/O images).
The tester defaults to Unit ID 1 and deliberately drops requests addressed to
another unit. PHOENIX requests use FC04 for inputs at 8000 and status at
7996/7997, and FC16 for outputs at 9000. The tester supplies a generic register
map; it does not emulate the physical coupler's bus or diagnostics.

Regression coverage includes packet loss followed by immediate recovery, late
responses, transaction wrap, invalid/empty/oversized datagrams, 25/40 ms response
deadlines, and a lost status reply preventing output writes. Hardware cycle-time
measurements remain necessary to quantify any improvement on the actual network.
