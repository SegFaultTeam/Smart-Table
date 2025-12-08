#include <dht.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <tusb.h>
#include "ask_user.h"
#include <stdlib.h>
#include "hardware/pwm.h"
#include <stdbool.h>
#include "hardware/uart.h"

#define PWM_PIN 14 
static const dht_model_t DHT_MODEL = DHT22;
static const uint DATA_PIN = 15;

int main() {
    stdio_init_all();
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);

    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true);

    while(!stdio_usb_connected()){
        sleep_ms(10);
    }
    sleep_ms(1500);

    char *min_comfort_temp_cels = minimum_temp();
    float min_temp = atof(min_comfort_temp_cels);
    float gip_stazis = min_temp - 1.5; 

    fflush(stdin);
    fflush(stdout);

    char *freq_of_not = frequence_of_moving();
    
    fflush(stdin);
    fflush(stdout);

    bool fan_turn = false; 

    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slices = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_set_wrap(slices, 100);
    pwm_set_gpio_level(PWM_PIN, 0);
    pwm_set_enabled(slices, 1);

    char buffull[200];
    int difference = 0;

    for(;;){
        if (!tud_cdc_connected()) { 
            free(min_comfort_temp_cels); 
            min_comfort_temp_cels = NULL;
            free(freq_of_not);
            freq_of_not = NULL;
            break;
        }

        dht_start_measurement(&dht);
        
        float humidity;
        float temperature_c;
        
        dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);

        if(result == DHT_RESULT_OK){
            sprintf(buffull, "%.2f %.2f\n", humidity, temperature_c);
            uart_puts(uart0, buffull);
            printf("\n%f\n", temperature_c);

            if (fan_turn) {
                if (temperature_c <= gip_stazis) {
                    fan_turn = false;
                    difference = 0;
                } else {
                    int raw_calc = (int)((temperature_c - min_temp) * 10);
                    if (raw_calc < 80) {
                        difference = 80;
                    } else if (raw_calc > 100) {
                        difference = 100;
                    } else {
                        difference = raw_calc;
                    }
                }
            } else {
                if (temperature_c > min_temp) {
                    fan_turn = true;
                    int raw_calc = (int)((temperature_c - min_temp) * 10);
                    if (raw_calc < 80) {
                        difference = 80;
                    } else if (raw_calc > 100) {
                        difference = 100;
                    } else {
                        difference = raw_calc;
                    }
                } else {
                    difference = 0;
                }
            }
            pwm_set_gpio_level(PWM_PIN, difference);
            printf("DIF%d", difference);

        } else if(result == DHT_RESULT_TIMEOUT){
            printf("DSNR\n");
        } else {
            printf("BCS\n");
        }
        
        sleep_ms(2000);
    }
}