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
#define WIFI_SSID "NAME" // Name of WIFI
#define WIFI_PASS "PASSWORD" // password
#define TARGET_IP "IP"//ip adress of device, that will get data
#define PORT 5000
#define SEONSOR1 15
uint64_t time_user_is_sitting(uint64_t target_time) {
static bool user_is_sitting = false; 
static uint64_t start_time = 0;
static uint64_t last_change = 0; //time of the last change of state(used for fixing bugs, for example if user removes hand from the table pretty fast, sensor will not react)
bool sensor_act = (gpio_get(SEONSOR1) == 0); // getting current state of sensor, gets true if sensor  see user
uint64_t current_time = time_us_64();  //getting current time
if(sensor_act != user_is_sitting) { //if sensor sees user but, user is not, or user is sitting but sensor does not see
    if(current_time - last_change >= 20000) { // removing random occurances (debouce)


        if(!sensor_act && user_is_sitting) { // first case
            user_is_sitting = false; // it means that user has standed up
            uint64_t duration = current_time - start_time; //calculating duration
            last_change = current_time;
            return duration / 60000000;
        }
        if(sensor_act && !user_is_sitting) { //second case
            user_is_sitting = true; //it means user was standing and now sitted
            start_time = current_time;
            last_change = current_time; // updating last change
        }
        
    }
    
}
if(user_is_sitting && current_time - start_time >= target_time) { // if user is sitting equal or more that is stated in settings
    user_is_sitting = false;
    uint64_t duration = current_time - start_time;
    last_change = current_time;
    return duration / 60000000;
}
return 0;
}

int main(void) {
    stdio_init_all();  
    gpio_init(SEONSOR1);
    gpio_set_dir(SEONSOR1, GPIO_IN);

    uint64_t target = 1000000;
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
            uint64_t time_user_sit = time_user_is_sitting(target);
        if(time_user_sit > 0) {
        printf("User is sitting for %llu minutes\n", time_user_sit);
    } 
            const char *msg = "TEST";
            struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, strlen(msg), PBUF_RAM); //allocating data
            memcpy(p->payload, msg, strlen(msg)); //copying data
            udp_sendto(pcb, p, &addr, PORT); //sending upd package
            pbuf_free(p);   //freeing p pointer after pbuf_alloc

            printf("Data has been sent by UDP!\n");
            sleep_ms(1000); 
        }
}