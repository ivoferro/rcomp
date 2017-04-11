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

// read a string from stdin protecting buffer overflow
#define GETS(B,S) {fgets(B, S, stdin); B[strlen(B)-1] = '\0';}

///
/// Given an UDP server as first parameter, sends UDP requests.
///
int main(int argc, char *argv[]) {
    struct sockaddr_storage server_addr;
    int sock, res, err;
    unsigned int server_addr_len;
    char line[BUFFER_SIZE];
    struct addrinfo req, *list;
    struct timeval to;

    if(argc != 2) {
        puts("Server IPv4/IPv6 address or DNS name is required as argument");
        exit(SERVER_ADDRESS_REQUIRED);
    }

    bzero(&req, sizeof(req));
    // let getaddrinfo set the family depending on the supplied server address
    req.ai_family = AF_UNSPEC;
    req.ai_socktype = SOCK_DGRAM;
    err = getaddrinfo(argv[1], SERVER_PORT, &req, &list);
    if(err != 0) {
        printf("Failed to get server address, error: %s\n", gai_strerror(err));
        exit(INVALID_SERVER_ADDRESS);
    }

    server_addr_len = list->ai_addrlen;
    // store the server address for later use when sending requests
    memcpy(&server_addr, list->ai_addr, server_addr_len);
    freeaddrinfo(list);
    bzero((char *)&req, sizeof(req));

    // for the local address, request the same family determined for the server address
    req.ai_family = server_addr.ss_family;
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

    if(bind(sock, (struct sockaddr *)list->ai_addr, list->ai_addrlen) < 0) {
        perror("Failed to bind socket");
        close(sock);
        freeaddrinfo(list);
        exit(BIND_SOCKET_FAILED);
    }

    freeaddrinfo(list);

    to.tv_sec = TIMEOUT;
    to.tv_usec = 0;
    setsockopt (sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&to, sizeof(to));

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
