#include "../util.h"

int initialize_sensors(void) {
    puts("Nothing to do to initialize sensors ...[ok].");
    return 1;
}

int read_humidity(void) {
    error(-1, 0, "Sensor not present or read_humidity not implemented for this board.");    
    return -1;
}

int read_humidity_shell(int argc, char **argv) {
    (void) argc;
    (void) argv;
    
    int value = read_humidity();
    printf("Value: %4i\n", value);
    return 0;
}

int read_light(void) {
    error(-1, 0, "Sensor not present or read_light not implemented for this board.");    
    return -1;
}

int read_light_shell(int argc, char **argv) {
    (void) argc;
    (void) argv;
    
    int value = read_light();
    printf("Value: %4i\n", value);
    return 0;
}
