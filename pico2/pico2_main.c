#include "pico/stdlib.h"
#include <stdio.h>
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include <string.h>
#include "pico/time.h"
#include <stdbool.h>
#include <stdlib.h>
#define WIFI_SSID // Name of WIFI
#define WIFI_PASS // password
#define TARGET_IP //ip adress of device, that will get data
#define PORT 5000
#define SEONSOR1 15
gpio_init(SEONSOR1);
gpio_set_dir(SEONSOR1, GPIO_IN);

uint64_t time_user_is_sitting(void) {
static bool user_is_present = false;
static bool prev_state = false;
bool raw = (gpio_get(SEONSOR1) == 0);
uint64_t prev = time_us_64();
while(gpio_get(SEONSOR1) == 0);
uint64_t current = time_us_64();
return (current - prev) / 1000 / 60;
}

int main(void) {
    stdio_init_all();  
    
    if(cyw43_arch_init()) {     //initting wifi module of pico, returns 1 if error
        printf("Wifi init failed\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode(); //enabling pico as a client
    printf("Connecting to WIFI ...\n");

    if(cyw43_arch_connect_timeout_ms(WIFI_SSID, WIFI_PASS, CYW43_AUTH_WPA2_AES_PSK, 20000)) { //trying to connect to wifi, returns 1 if error
        printf("Error connecting to WIFI\n");
        return 1;
    }
    printf("Successfully connected to WIFI!\n");
    static struct udp_pcb * pcb; //initting new udp package
    pcb = udp_new();
    if(!pcb) {
        printf("UDP new method has failed\n");
        return 1;
    }

    static ip_addr_t addr;
    ipaddr_aton(TARGET_IP, &addr); //setting addr to target ip address

        for(;;) {
            const char *msg = "TEST";
            struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, strlen(msg), PBUF_RAM); //allocating data
            memcpy(p->payload, msg, strlen(msg)); //copying data
            udp_sendto(pcb, p, &addr, PORT); //sending upd package
            pbuf_free(p);   //freeing p pointer after pbuf_alloc

            printf("Data has been sent by UDP!\n");
            sleep_ms(1000); 
        }
}