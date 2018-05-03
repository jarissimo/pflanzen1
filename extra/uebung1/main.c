#include <stdio.h>
#include <string.h>

#include "msg.h"
#include "shell.h"
#include "saul.h"
#include "saul_reg.h"

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int udp_send(int argc, char **argv);
extern int udp_server(int argc, char **argv);

int readlight(int argc, char **argv) {
    (void) argc;
    (void) argv;
    
    saul_reg_t* dev = saul_reg_find_name("tcs37727");
    printf("light: %p\n", dev);

    int dim;
    phydat_t res;

    dim = saul_reg_read(dev, &res);
    if (dim <= 0) {
        printf("error: failed to read from sensor\n");
        return 1;
    }

    printf("Reading from sensor (%s|%s)\n", dev->name,
           saul_class_to_str(dev->driver->type));

    phydat_dump(&res, dim);

    if ( res.scale != 0 ) {
        puts("Scaling is not implemented, go out of the sun or something");
        return 1;
    }

    if ( res.unit != UNIT_CD ) {
        printf("unknown unit returned (%i)\n", res.unit);
        return 1;
    }

    printf("values: %i %i %i\n", (int16_t)res.val[0], (int16_t)res.val[1], (int16_t)res.val[2]);
    
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "udp", "send udp packets", udp_send },
    { "udps", "start udp server", udp_server },
    { "light", "read light data", readlight },
    { NULL, NULL, NULL }
};

int main(void)
{
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("This is sensor reader (task 3)");

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
