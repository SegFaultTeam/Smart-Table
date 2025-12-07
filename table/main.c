#include <dht.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <tusb.h>
#include <ask_user.h>
#include <stdlib.h>
#include "hardware/pwm.h"
#include <stdbool.h>

#define PWM_PIN 14 //change me please
static const dht_model_t DHT_MODEL = DHT22;
static const uint DATA_PIN = 15;
int main() {
    stdio_init_all();
    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true /* pull_up */);
    while(!stdio_usb_connected()){ //wait before user go inside serial monitot
        sleep_ms(10);
    }
    sleep_ms(1500);
    char *min_comfort_temp_cels = minimum_temp();
    float min_temp = atof(min_comfort_temp_cels);
    float gip_stazis = min_temp-1.5; //to not turn on and off too ofthen 
    char *freq_of_not = frequence_of_moving();
    bool fan_turn = false;
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slices = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_set_wrap(slices, 100);
    pwm_set_gpio_level(PWM_PIN, 0);
    pwm_set_enabled(slices, 1);
    for(;;){
        if (!tud_cdc_connected()) { //if user disconnect free mem
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
            int difference = (temperature_c - min_temp)*10;
            if(difference < 0) difference = 0;
            if(difference>100) difference = 100;
            if(temperature_c <= gip_stazis){
                fan_turn = false;
                pwm_set_gpio_level(PWM_PIN, 0);
            }
            else if(difference >= 1 && !fan_turn){
                pwm_set_gpio_level(PWM_PIN, difference);
            }
        }else if(result == DHT_RESULT_TIMEOUT){
            printf("DSNR\n"); //prototype
        }else{
            assert(result == DHT_RESULT_BAD_CHECKSUM);
            printf("BCS\n"); //prototype
        }
        
        sleep_ms(2000);
    }
}
