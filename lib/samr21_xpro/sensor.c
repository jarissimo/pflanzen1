#include <stdio.h>
#include <math.h>

#include "cpu.h"
#include "board.h"
#include "xtimer.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "../util.h"

#define RES                         ADC_RES_12BIT

#define ADC_USED_LINE               ADC_LINE(0)
#define ADC_SLEEP1                  (1)
#define ADC_SLEEP2                  (1)

#define GPIO_POWER_PORT		        (PA)
#define GPIO_POWER_PIN 		        (13)
#define GPIO_POWER                  GPIO_PIN(GPIO_POWER_PORT, GPIO_POWER_PIN)


int initialize_sensors(void) {
    
    puts("\nRIOT test for a moisture sensor\n");
    printf("\nusing GPIO port %d, pin %d", GPIO_POWER_PORT, GPIO_POWER_PIN);

    /* initialize a GPIO that powers the sensor just during a measure */
    if (gpio_init(GPIO_POWER, GPIO_OUT)== 0) {
        puts("    ...[ok]");
    }
    else {
        puts("    ...[failed]");
        return -1;
    }
    puts("\n");
    /* first set LOW */  
    gpio_clear(GPIO_POWER);
    
    /* initialize the adc device */ 
    adc_t line = ADC_LINE(0);
    printf("Initializing ADC_%i @ %i bit resolution", 0, (6 + (2* RES)));
    if (adc_init(line) == 0) {
        puts("    ...[ok]");
    }
    else {
        puts("    ...[failed]");
        return -1;
    }
    return 1;
}


int read_humidity(int argc, char **argv) {
    
    (void) argc;
    (void) argv;
    
    gpio_set(GPIO_POWER);
    xtimer_sleep(ADC_SLEEP1);

    int value = adc_sample(ADC_USED_LINE, RES);
    printf("Value: ADC_%i: %4i\n", ADC_USED_LINE, value);
    xtimer_sleep(ADC_SLEEP2);
    gpio_clear(GPIO_POWER);

    return 0;
}

int read_light(int argc, char **argv) {

    (void) argc;
    (void) argv;

    error(-1, 0, "Sensor not present or read light not implemented for this board.");    
    return -1;
}


