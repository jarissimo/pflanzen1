#include "../util.h"

int initialize_sensors(void) {
    puts("Nothing to do to initialize sensors ...[ok].");
    return 1;
}

int read_humidity(int argc, char **argv) {
    (void) argc;
    (void) argv;
    error(-1, 0, "Sensor not present or read_humidity not implemented for this board.");    
    return -1;
}

int read_light(int argc, char **argv) {
    (void) argc;
    (void) argv;
    error(-1, 0, "Sensor not present or read_light not implemented for this board.");    
    return -1;
}
