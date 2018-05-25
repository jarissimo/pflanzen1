/* #define NATIVE 0 */
/* #define PHYNODE 1 */
/* #define SAMR 2 */

/* #define BOARD 0 */

/* #if BOARD == NATIVE */
/* #include "sensor/native/sensor.c" */
/* #elif BOARD == PHYNODE */
/* #include "sensor/pba_d_01_kw2x/sensor.c" */
/* #elif BOARD == SAMR */
/* #include "sensor/samr21_xpro/sensor.c" */
/* #else */
/* #error */
/* #endif */

int read_sensor(char *sensor_name);

int read_humidity(int argc, char **argv);

int read_light(int argc, char **argv);

