#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

// Required for Pico W + lwIP
#define NO_SYS                      1
#define LWIP_SOCKET                 0
#define LWIP_NETCONN                0

#define MEM_LIBC_MALLOC             0

#define LWIP_DHCP                   1
#define LWIP_IPV4                   1
#define LWIP_RAW                    1

#define LWIP_ICMP                   1
#define LWIP_TCP                    1
#define LWIP_UDP                    1

#define TCP_MSS                     1460
#define TCP_WND                     (4 * TCP_MSS)
#define TCP_SND_BUF                 (4 * TCP_MSS)

#endif // _LWIPOPTS_H
