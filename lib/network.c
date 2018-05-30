#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "checksum/crc16_ccitt.h"
#include "net/sock/udp.h"
#include "net/ipv6/addr.h"

#include "util.h"

typedef void (*receive_handler) (uint8_t* buf, uint16_t buflen);
typedef uint16_t nodeid_t;

#define H2OP_PORT (44555)
#define H2OP_MAGIC (0xAC)
#define H2OP_VERSION (1)
typedef enum {
    H2OP_DATA_TEMPERATURE = 0x11,
    H2OP_DATA_HUMIDITY = 0x12,
    H2OP_WARN_BUCKET_EMPTY = 0x99,
} H2OP_MSGTYPE;
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
#define H2OP_HEADER_LENGTH (sizeof(H2OP_HEADER))
#define H2OP_MAX_LENGTH (127) // 802.15.4's mtu

#define SERVER_MSG_QUEUE_SIZE (8)
#define SERVER_BUFFER_SIZE (H2OP_MAX_LENGTH)

void h2op_nodeid_to_addr ( nodeid_t nodeid, ipv6_addr_t *addr) {
    switch(nodeid) {
        case 0: // reserved
            ipv6_addr_set_unspecified(addr);
            error(0, 0, "Invalid value for nodeid: 0000");
            break;
        case 0xffff: // special address meaning broadcast (ff02::1)
            ipv6_addr_set_all_nodes_multicast(addr, 2);
            break;
        default:
            addr->u64[0] = byteorder_htonll((uint64_t) H2O_NETWORK_PREFIX);
            addr->u64[1] = byteorder_htonll((uint64_t) nodeid);
            break;
    }
}

int rv; // generic declaration for return value (use locally only)

void udp_server(uint16_t port, receive_handler handler) {
    /* Create a UDP server that calls the receive_handler for each incoming message.
     *
     * @param port: the port to listen at
     * @param handler: the function to call. It must accept two parameters:
     * - a pointer to a memory area where the received data is stored.
     * - the length of this data.
     */
    static sock_udp_t sock;
    static uint8_t server_buffer[SERVER_BUFFER_SIZE];
    static msg_t server_msg_queue[SERVER_MSG_QUEUE_SIZE];

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
        res = sock_udp_recv(&sock, server_buffer, sizeof(server_buffer),
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

ssize_t udp_send ( const ipv6_addr_t *recipient, uint16_t port,
                   const uint8_t *data, size_t data_len ) {
    /* send an udp datagram. */
    sock_udp_ep_t remote = {
        .family = AF_INET6,
        .port = port,
    };
    memcpy(&remote.addr, recipient, sizeof(*recipient));

    if (ipv6_addr_is_link_local((ipv6_addr_t *)&remote.addr)) {
        /* choose first interface when address is link local */
        gnrc_netif_t *netif = gnrc_netif_iter(NULL);
        remote.netif = (uint16_t)netif->pid;
    }

    ssize_t res = sock_udp_send(NULL, data, data_len, &remote);
    // TODO error handling
    return res;
}

void h2op_header_hton ( H2OP_HEADER *header ) {
    header->node = htons(header->node);
    header->crc = htons(header->crc);
}

void h2op_header_ntoh ( H2OP_HEADER *header ) {
    header->node = ntohs(header->node);
    header->crc = ntohs(header->crc);
}

ssize_t h2op_send ( const nodeid_t recipient, H2OP_MSGTYPE type,
                    const char *data, size_t len, uint16_t source ) {
    /* Send a H2OP message.
     * @param recipient: recipient, node id. 0xffff is broadcast.
     * @param type: message type, as per protocol definition
     * @param data: message data
     * @param len: length of data
     * @param source: node id of data source
     */
#pragma pack(push, 1)
    struct {
        H2OP_HEADER header;
        uint8_t data[H2OP_MAX_LENGTH-H2OP_HEADER_LENGTH];
    } buf;
#pragma pack(pop)
    size_t buflen = H2OP_HEADER_LENGTH + len;

    ipv6_addr_t recipient_ip;
    h2op_nodeid_to_addr(recipient, &recipient_ip);

    buf.header = (H2OP_HEADER) {
        .magic = H2OP_MAGIC,
        .version = H2OP_VERSION,
        .len = H2OP_HEADER_LENGTH+len,
        .type = type,
        .node = source,
        .crc = 0,
    };
    memcpy(buf.data, data, len);
    buf.header.crc = crc16_ccitt_calc((uint8_t*) &buf, buflen);

    h2op_header_hton(&buf.header);
    rv = udp_send(&recipient_ip, H2OP_PORT, (uint8_t*) &buf, buflen);
    printf("%d bytes sent to ", rv); ipv6_addr_print(&recipient_ip); putchar('\n');
    return rv;
}

void h2op_debug_receive_handler ( uint8_t *buf, uint16_t packetlen ) {
    /* print contents of h2op package to stdout */
    if ( packetlen < H2OP_HEADER_LENGTH ) {
        error(0,0, "invalid packet received: length < %u", H2OP_HEADER_LENGTH);
        return;
    }
    H2OP_HEADER *header = (H2OP_HEADER*) buf;
    h2op_header_ntoh(header);

    if ( header->magic != H2OP_MAGIC ) {
        error(0,0, "invalid magic number (got 0x%02X, want 0xAC)", header->magic);
        return;
    }
    if ( header->version != H2OP_VERSION ) {
        error(0,0, "unknown version (got 0x%02X, want 0x01)", header->version);
        return;
    }

    uint16_t crc_want = header->crc;
    header->crc = 0;
    uint16_t crc_have = crc16_ccitt_calc(buf, packetlen);
    if ( crc_want != crc_have ) {
        error(0,0, "crc mismatch (got 0x%04X, calculated 0x%04X)",
              crc_want, crc_have);
        return;
    }

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
    puts("printf '\\xac\\x01\\x0c\\x00\\x12\\x34\\x4e\\x67DATA' "
         "| nc -6u ff02::1%tapbr0 44555");

    udp_server(H2OP_PORT, &h2op_debug_receive_handler);
    return 0;
}

int h2o_send_data_shell ( int argc, char *argv[]) {
    if ( argc != 5 ) {
        printf("Usage: %s from(nodeid) to(nodeid) "
             "[temperature|humidity] value(0..255)\n", argv[0]);
        return 1;
    }
    nodeid_t from = strtoul(argv[1], NULL, 16);
    nodeid_t to = strtoul(argv[2], NULL, 16);
    int16_t value = strtol(argv[4], NULL, 10);

    H2OP_MSGTYPE type;
    if ( argv[3][0] == 't' ) {
        type = H2OP_DATA_TEMPERATURE;
    } else if ( argv[3][0] == 'h' ) {
        type = H2OP_DATA_HUMIDITY;
    } else {
        fprintf(stderr, "Invalid argument: %s (must be one of temperature, "
                "humidity)\n", argv[3]);
    }

    //XXX: why is there no network_int16_t in RIOT?
    int16_t netval = htons(value);

    rv = h2op_send(to, type, (char*) &netval, sizeof(value), from);
    if ( rv <= 0 ) {
        error(0,-rv, "could not send data");
        return 1;
    } else {
        puts("Data sent.");
        return 0;
    }
}
