#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "pico/time.h"
#include <stdbool.h>
#include <stdlib.h>
#include "hardware/uart.h"
#define ECHO 15
#define TRIGGER 14
#define DISTANCE_CM 50 //target distance, can be changed.
#define UART_ID uart0
#define UART_TX 0
#define UART_RX 1
uint64_t time_user_is_sitting(uint64_t target_time) { //function that returns time that user was sitting
    static bool user_is_sitting = false; //state init
    static uint64_t start_time = 0; //start time initting
    static uint64_t lost_time = 0; //lost time is used for fixing situations when user stood up fast enough for instance, and then sit back, so time will not stop in this case
    uint64_t current_time = time_us_64();  // getting current time

    gpio_put(TRIGGER, 1);
    sleep_us(10);
    gpio_put(TRIGGER, 0);

    while(gpio_get(ECHO) == 0) tight_loop_contents();
    uint64_t time1 = time_us_64();
    while(gpio_get(ECHO) == 1) tight_loop_contents();
    uint64_t time2 = time_us_64();

    float distance_cm = ((time2 - time1) / 2.0) * 0.0343; //calculating distance
    bool sensor_act = distance_cm < DISTANCE_CM; //gives true if user is in range

    if(!sensor_act && user_is_sitting) {  //first case
        if(lost_time == 0) lost_time = current_time; //updating lost time
        if(current_time - lost_time < 500000) { //giving 0.5s as temporary lost signal, if user returns in range, time will not stop
            printf("[INFO] Temporary lost signal, ignoring\n");
            return 0;
        }

        user_is_sitting = false;
        uint64_t duration = current_time - start_time;  //calculating duration
        lost_time = 0; //updating lost time
        printf("[STATE] User stand, user was sitting for %llu\n", duration / 1000000);
        return duration / 1000000;
    }

    if(sensor_act && !user_is_sitting) {  //second case, if user was standing and now sitted
        user_is_sitting = true; //updating state
        lost_time = 0; //resetting lost time
        printf("[STATE] User sitted\n");
        return 0;
    }

    if(sensor_act) { //if user is in range
        lost_time = 0; //resetting lost time
        printf("[INFO] User is still sitting, distance is %.1f cm\n", distance_cm);
    }
    // if user is sitting more or equal than user stated in settings
    if(user_is_sitting && current_time - start_time >= target_time) {
        user_is_sitting = false; 
        uint64_t duration = current_time - start_time;
        printf("[STATE] Target time was reached! User was sitting for %llu seconds\n", duration / 1000000);
        return duration / 1000000;
    }
    return 0;
}


int main(void) {
    stdio_init_all();  

    sleep_ms(4000);
    puts("CONNECTED");
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART); 
    gpio_set_function(1, GPIO_FUNC_UART);
    gpio_init(TRIGGER);
    gpio_set_dir(TRIGGER, GPIO_OUT);

    gpio_init(ECHO);
    gpio_set_dir(ECHO, GPIO_IN);
           char buf[100];
        for(;;) {
            int i = 1;
            int c = uart_getc(uart0);
        int index = 0;
        while(c != '\n' && c != '\r' && index != 100){
            buf[index++] = c;
            c = uart_getc(uart0);
        }
        buf[index] = '\0';
        fflush(stdout);
        printf("%s\n", buf);
            uint64_t time_user_sit = time_user_is_sitting(10000000);
        if(time_user_sit > 0) {
        printf("User is sitting for %llu seconds\n", time_user_sit);
        

        }

    sleep_ms(100); 
        }
    }