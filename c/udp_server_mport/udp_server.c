/**
 * udp_server.c
 */

#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "constants.h"

#define BUFFER_SIZE 300
#define BASE_PORT 9009
#define PORT_STEP 100
#define NUM_PORTS 6

int main(void) {
    struct sockaddr_storage client;
    int err, sock[NUM_PORTS], res, i, j, max;
    unsigned int addr_len;
    char line[BUFFER_SIZE], line_aux[BUFFER_SIZE];
    char ip_text[BUFFER_SIZE], port_text[BUFFER_SIZE];
    struct addrinfo req, *list;
    fd_set read_socks;

    max = 0;
    for(i = 0; i < NUM_PORTS; i++) {
        bzero((char *)&req, sizeof(req));
        req.ai_family = AF_INET6; // will allow both IPv4 and IPv6 clients to use it
        req.ai_socktype = SOCK_DGRAM; // UDP
        req.ai_flags = AI_PASSIVE; // this is a local address
        sprintf(port_text, "%i", BASE_PORT + PORT_STEP * i);
        err = getaddrinfo(NULL, port_text, &req, &list);
        if(err) {
            printf("Getaddrinfo failed for port %s, error: %s\n", port_text, gai_strerror(err));
            exit(INVALID_LOCAL_ADDRESS);
        }

        sock[i] = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
        if(sock[i] < 0) {
            perror("Failed to open socket");
            freeaddrinfo(list);
            i--;
            do {
                close(sock[i]);
                i--;
            }
            while(i > -1);
            exit(OPEN_SOCKET_FAILED);
        }

        if(bind(sock[i],(struct sockaddr *)list->ai_addr, list->ai_addrlen) < 0) {
            perror("Bind failed");
            freeaddrinfo(list);
            do {
                close(sock[i]);
                i--;
            }
            while(i > -1);
            exit(BIND_SOCKET_FAILED);
        }
        freeaddrinfo(list);

        if(sock[i] > max) {
            max = sock[i]; // the greatest number socket}
        }
    }

    puts("Listening for UDP requests (both over IPv6 or IPv4). Use CTRL+C to terminate the server");
    addr_len = sizeof(client);
    while(1) {
        FD_ZERO(&read_socks);
        for(i = 0; i < NUM_PORTS; i++) {
            FD_SET(sock[i], &read_socks);
        }
        select(max + 1, &read_socks, NULL, NULL, NULL);

        for(i = 0; i < NUM_PORTS; i++) {
            if(FD_ISSET(sock[i], &read_socks))
            {
                res = recvfrom(sock[i], line, BUFFER_SIZE, 0,(struct sockaddr *)&client, &addr_len);
                getnameinfo((struct sockaddr *)&client, addr_len,
                            ip_text, BUFFER_SIZE, port_text, BUFFER_SIZE, NI_NUMERICHOST|NI_NUMERICSERV);
                printf("Request from node %s, port number %s\n", ip_text, port_text);
                for(j = 0; j < res; j++) {
                    line_aux[res-1-j] = line[j];
                }
                sendto(sock[i], line_aux, res, 0, (struct sockaddr *)&client, addr_len);
            }
        }
    }

    return 0;
}
