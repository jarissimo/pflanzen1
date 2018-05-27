#include "thread.h"
#include "xtimer.h"

/* interval of measurements in microseconds*/
#define MEASUREMENTS_INTERVAL   (5 * 1000000)

void *sensor_thread(void *arg)
{
    (void) arg;

    initialize_sensors();

    xtimer_ticks32_t last_wakeup = xtimer_now();
    uint32_t period = MEASUREMENTS_INTERVAL;

    while (1) {
        puts("here is the sensor thread");
        char **argv = 0;
        read_humidity_shell(0, argv);

        /* wakes up periodically, this should get us the interval */ 
        /* as long as it is longer as we take for our measurements */
        xtimer_periodic_wakeup(&last_wakeup, period); 
        
    }

    return NULL;
}
