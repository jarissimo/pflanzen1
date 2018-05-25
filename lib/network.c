#include <stdint.h>
#include <stdio.h>

#include "net/sock/udp.h"
#include "net/ipv6/addr.h"
#include "util.h"

#define SERVER_MSG_QUEUE_SIZE (8)
#define SERVER_BUFFER_SIZE (128) // 802.15.4's mtu plus one to add a null byte

static sock_udp_t sock;
static char server_buffer[SERVER_BUFFER_SIZE];
static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];

typedef void (*receive_handler) (char* buf, uint16_t buflen);

#define H2OP_SERVER_PORT (44555)
static size_t H2OP_HEADER_LENGTH = 8;
#pragma pack(push, 1)
typedef struct {
    uint8_t magic;
    uint8_t version;
    uint8_t len;
    uint8_t type;
    uint16_t node;
    uint16_t crc;
} H2OP_HEADER;
#pragma pack(pop)

int rv; // generic declaration for return value (use locally only)

void udp_server(uint16_t port, receive_handler handler) {
    /* Create a UDP server that calls the receive_handler for each incoming message.
     *
     * @param port: the port to listen at
     * @param handler: the function to call. It must accept two parameters:
     * - a pointer to a memory area where the received data is stored.
     * - the length of this data.
     */

    sock_udp_ep_t server = { .port = port, .family = AF_INET6, };
    msg_init_queue(server_msg_queue, SERVER_MSG_QUEUE_SIZE);

    if((rv = sock_udp_create(&sock, &server, NULL, 0)) < 0) {
        error(-rv, 0, "Could not start UDP server");
        return;
    }

    printf("UDP server running on port %u.\n", port);

    while (true) {
        int res;

        //TODO: store sender address
        res = sock_udp_recv(&sock, server_buffer, sizeof(server_buffer)-1,
                            SOCK_NO_TIMEOUT, NULL);
        if ( res < 0 ) {
            error(0, -res, "Error while receiving");
            return;
        } else if ( res == 0 ) {
            puts("no data received");
        } else {
            handler(server_buffer, res);
        }
    }
}

void h2op_header_ntoh ( H2OP_HEADER *header ) {
    header->node = ntohs(header->node);
    header->crc = ntohs(header->crc);
}

void h2op_debug_receive_handler ( char *buf, uint16_t packetlen ) {
    if ( packetlen < H2OP_HEADER_LENGTH ) {
        error(0,0, "invalid packet received: length < %u", H2OP_HEADER_LENGTH);
        return;
    }
    H2OP_HEADER *header = (H2OP_HEADER*) buf;
    h2op_header_ntoh(header);

    if ( header->magic != 0xac ) {
        error(0,0, "invalid magic number (got 0x%02X, want 0xAC)", header->magic);
        return;
    }
    if ( header->version != 0x01) {
        error(0,0, "invalid version (got 0x%02X, want 0x01)", header->version);
        return;
    }
    // TODO check crc
    if ( packetlen < header->len ) {
        error(0,0, "packet length (%u) < length field in header (%u)",
              packetlen, header->len);
        return;
    }
    else if ( packetlen > header->len ) {
        printf("INFO: packet length (%u) > length field in header (%u)\n",
               packetlen, header->len);
    }

    printf("Packet received.       Type: %02x             Author: %04x  ",
            header->type, header->node);
    hexdump("Data", buf+H2OP_HEADER_LENGTH, (header->len)-H2OP_HEADER_LENGTH);
}

int h2o_dump_server ( int argc, char *argv[]) {
    (void) argc;
    (void) argv;

    puts("Starting Server. Example usage:");
    puts("printf '\\xac\\x01\\x0c\\x00\\x12\\x34\\x00\\x00DATA' "
         "| nc -6u ff02::1%tapbr0 44555");

    udp_server(H2OP_SERVER_PORT, &h2op_debug_receive_handler);
    return 0;
}
