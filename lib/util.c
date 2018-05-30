#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

// from https://stackoverflow.com/a/7776146/196244 , slightly adapted
void hexdump (char *desc, void *addr, int len) {
    /* Print some data as a hexdump (like `hexdump -C`)
     * @param desc: if not NULL, printed before the dump (plus colon and newline)
     * @param addr: start address of the data to dump
     * @param len: data length
     */

    int i;
    unsigned char buff[17];
    unsigned char *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %i\n",len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of 16 means new line (with line offset).

        if ((i % 16) == 0) {
            // Just don't print ASCII for the zeroth line.
            if (i != 0)
                printf ("  %s\n", buff);

            // Output the offset.
            printf ("  %04x ", i);
        } else if ((i % 16) == 8) {
            printf (" ");
        }

        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);

        // And store a printable ASCII character for later.
        if (pc[i] == 0)
            buff[i % 16] = ':';
        else if ((pc[i] < 0x20) || (pc[i] > 0x7e))
            buff[i % 16] = '.';
        else
            buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }

    // Pad out last line if not exactly 16 characters.
    while ((i % 16) != 0) {
        if ((i % 16) == 8 )
            printf (" ");
        printf ("   ");
        i++;
    }

    // And print the final ASCII bit.
    printf ("  %s\n", buff);
}

#ifdef USE_LIBC_ERRORH
#include <error.h>
#else
void error(int status, int errnum, const char *format, ...) {
    fflush(stdout);

    fprintf(stderr, "error: ");

    if ( errnum != 0 ) {
/* maybe use this again later conditionally on architecturess that have it
        char buf[1024];
        strerror_r(errnum, buf, 1024);
        fprintf(stderr, "%s: ", buf);
*/
        fprintf(stderr, "%d: ", errnum);
    }

    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);

    fprintf(stderr, "\n");
    fflush(stderr);
    if ( status != 0 ) {
        _exit(status);
    }
}
#endif // USE_LIBC_ERRORH
