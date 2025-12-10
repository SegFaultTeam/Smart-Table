#include <dht.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <tusb.h>
#include "ask_user.h"
#include <stdlib.h>
#include "hardware/pwm.h"
#include <stdbool.h>
#include "hardware/uart.h"
#include <string.h>
#include "ssd1306.h"
#include "hardware/i2c.h"

ssd1306_t disp;

void setup_gpios(void) { // initialing pins for I2c
    i2c_init(i2c1, 400000);
    gpio_set_function(2, GPIO_FUNC_I2C);
    gpio_set_function(3, GPIO_FUNC_I2C);
    gpio_pull_up(2);
    gpio_pull_up(3);
}

//function for displaying status
void display_status(float temp, float hum, int fan_percent) {
    char buf[32];
    
    ssd1306_clear(&disp);
    
    sprintf(buf, "Temp:%.1fC", temp);
    ssd1306_draw_string(&disp, 5, 5, 1, buf);
    
    sprintf(buf, "Hum :%.1f%%", hum);
    ssd1306_draw_string(&disp, 5, 20, 1, buf);
    
    sprintf(buf, "Fan :%d%%", fan_percent);
    ssd1306_draw_string(&disp, 5, 35, 1, buf);

    ssd1306_show(&disp);
}

#define PWM_PIN 14 
static const dht_model_t DHT_MODEL = DHT22;
static const uint DATA_PIN = 15;

int main() {
    stdio_init_all();
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);
    gpio_set_function(1, GPIO_FUNC_UART);
    
    setup_gpios();
    
    sleep_ms(100);

    disp.external_vcc = false;
    if (!ssd1306_init(&disp, 128, 64, 0x3C, i2c1)) { //checking for display for using
        printf("Display init failed!\n");
        if (!ssd1306_init(&disp, 128, 64, 0x3D, i2c1)) {
            printf("Display init failed on both addresses!\n");
        }
    }
    
    ssd1306_clear(&disp);//clearing display(prevent white noises)
    ssd1306_draw_string(&disp, 5, 5, 1, "Starting...");
    ssd1306_show(&disp); //show starting
    sleep_ms(1000);
    
    dht_t dht;
    dht_init(&dht, DHT_MODEL, pio0, DATA_PIN, true);

    while(!stdio_usb_connected()){ //function waits before user go inside serial monitor
        sleep_ms(10);
    }
    sleep_ms(1500);
    uart_puts(uart0, "test\n"); // "garbage to uart to prevent bug of first uart"

    char *min_comfort_temp_cels = minimum_temp();
    float min_temp = atof(min_comfort_temp_cels);
    float gip_stazis = min_temp - 1.5; 
    fflush(stdout);

    char *freq_of_not = frequence_of_moving();
    freq_of_not[strcspn(freq_of_not, "\0")] = '\n';
    uart_puts(uart0, freq_of_not); //sending time to move for second pico

    fflush(stdout);
    printf("\n");
    bool fan_turn = false; 

    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slices = pwm_gpio_to_slice_num(PWM_PIN);
    pwm_set_wrap(slices, 100);
    pwm_set_gpio_level(PWM_PIN, 0);
    pwm_set_enabled(slices, 1); //initialing pwm for fan

    char buffull[200];
    int difference = 0;

    for(;;){
        if (!tud_cdc_connected()) { //if user disconnected by usb free memory
            free(min_comfort_temp_cels); 
            min_comfort_temp_cels = NULL;
            free(freq_of_not);
            freq_of_not = NULL;
            ssd1306_deinit(&disp);
            break;
        }

        dht_start_measurement(&dht);
        
        float humidity;
        float temperature_c;
        
        dht_result_t result = dht_finish_measurement_blocking(&dht, &humidity, &temperature_c);

        if(result == DHT_RESULT_OK){
            sprintf(buffull, "%.2f %.2f\n", humidity, temperature_c);
            uart_puts(uart0, buffull); //sending statuses for second pico
            if (fan_turn) { //logic: if fan is turn on, we can turn it off just after decreasing lower than giperstazis
                if (temperature_c <= gip_stazis) {
                    fan_turn = false;
                    difference = 0;
                } else {
                    int raw_calc = (int)((temperature_c - min_temp) * 10);
                    if (raw_calc < 80) { //80 is minimum for turning on fan:(
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
            display_status(temperature_c, humidity, difference);
        } else if(result == DHT_RESULT_TIMEOUT){
            uart_putc(uart0, 'D'); //code error for pico check wiring
        } else {
            assert(result == DHT_RESULT_BAD_CHECKSUM);
            uart_putc(uart0, 'B'); //code error for pico bad checksum
        }
        
        sleep_ms(2000);
    }
}