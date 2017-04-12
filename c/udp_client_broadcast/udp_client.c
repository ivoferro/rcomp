/**
 * udp_client.c
 */

#include <strings.h>
#include <string.h>
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
#define SERVER_PORT "9999"
#define BCAST_ADDRESS "255.255.255.255"

// read a string from stdin protecting buffer overflow
#define GETS(B,S) {fgets(B, S, stdin); B[strlen(B)-1] = '\0';}

///
/// Given an UDP server as first parameter, sends UDP requests.
///
int main(int argc, char *argv[]) {
    struct sockaddr_storage server_addr;
    int sock, val, res, err;
    unsigned int server_addr_len;
    char line[BUFFER_SIZE];
    struct addrinfo req, *list;
    struct timeval to;

    bzero(&req, sizeof(req));
    req.ai_family = AF_INET; // there is no broadcast in IPv6, so we request a IPv4 address
    req.ai_socktype = SOCK_DGRAM;
    err = getaddrinfo(BCAST_ADDRESS, SERVER_PORT, &req, &list);
    if(err != 0) {
        printf("Failed to get server address, error: %s\n", gai_strerror(err));
        exit(INVALID_SERVER_ADDRESS);
    }

    server_addr_len = list->ai_addrlen;
    // stores the server address for later use when sending requests
    memcpy(&server_addr, list->ai_addr, server_addr_len);
    freeaddrinfo(list);

    bzero((char *)&req, sizeof(req));
    req.ai_family = AF_INET;
    req.ai_socktype = SOCK_DGRAM;
    req.ai_flags = AI_PASSIVE; // local address
    err = getaddrinfo(NULL, "0", &req, &list);  // port 0 = auto assign
    if(err != 0) {
        printf("Failed to get local address, error: %s\n",gai_strerror(err));
        exit(INVALID_LOCAL_ADDRESS);
    }

    sock = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
    if(sock < 0) {
        perror("Failed to open socket");
        freeaddrinfo(list);
        exit(OPEN_SOCKET_FAILED);
    }

    val=1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val)); // enables broadcast

    if(bind(sock, (struct sockaddr *)list->ai_addr, list->ai_addrlen) < 0) {
        perror("Failed to bind socket");
        close(sock);
        freeaddrinfo(list);
        exit(BIND_SOCKET_FAILED);
    }

    freeaddrinfo(list);

    to.tv_sec = TIMEOUT;
    to.tv_usec = 0;
    setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&to, sizeof(to));

    while(1) {
        printf("Request sentence to send (\"exit\" to quit): ");
        GETS(line, BUFFER_SIZE);
        if(strcmp(line, "exit") == 0) {
            break;
        }

        sendto(sock, line, strlen(line), 0, (struct sockaddr *)&server_addr, server_addr_len);
        res = recvfrom(sock, line, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, &server_addr_len);
        if (res > 0) {
            line[res] = '\0'; /* NULL terminate the string */
            printf("Received reply: %s\n", line);
        } else {
            printf("No reply from server\n");
        }
    }
    close(sock);

    return 0;
}
