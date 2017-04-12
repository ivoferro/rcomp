/**
 * tcp_client_sum.c
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "constants.h"

#define BUFFER_SIZE 30
#define SERVER_PORT "9999"

#define GETS(B,S) {fgets(B, S, stdin); B[strlen(B)-1] = 0;}

int main(int argc, char **argv)
{
    int err, sock;
    unsigned long f, i, n, num;
    unsigned char bt;
    char linha[BUFFER_SIZE];
    struct addrinfo req, *list;

    if(argc != 2) {
        puts("Server IPv4/IPv6 address or DNS name is required as argument");
        exit(SERVER_ADDRESS_REQUIRED);
    }

    bzero((char *)&req, sizeof(req));
    // let getaddrinfo set the family depending on the supplied server address
    req.ai_family = AF_UNSPEC;
    req.ai_socktype = SOCK_STREAM;
    err = getaddrinfo(argv[1], SERVER_PORT, &req, &list);
    if(err != 0) {
        printf("Failed to get server address, error: %s\n", gai_strerror(err));
        exit(INVALID_SERVER_ADDRESS);
    }

    sock = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
    if(sock < 0) {
        perror("Failed to open socket");
        freeaddrinfo(list);
        exit(OPEN_SOCKET_FAILED);
    }

    if(connect(sock, (struct sockaddr *)list->ai_addr, list->ai_addrlen) < 0) {
        perror("Failed connect");
        freeaddrinfo(list);
        close(sock);
        exit(CONNECTION_FAILED);
    }

    do {
        do {
            printf("Enter a positive integer to SUM (zero to terminate): ");
            GETS(linha, BUFFER_SIZE);
            while(sscanf(linha, "%li", &num) != 1 || num < 0)
            {
                printf("Invalid number, please enter a positive integer to "
                     "SUM (zero to terminate): ");
                GETS(linha, BUFFER_SIZE);
            }

            n = num;
            for(i = 0; i < 4; i++)
            {
                bt = n % 256;
                write(sock, &bt, 1);
                n = n / 256;
            }
        }
        while(num > 0);

        num = 0;
        f = 1;
        for(i = 0; i < 4; i++) {
            read(sock, &bt, 1);
            num = num + bt * f;
            f = f * 256;
        }
        printf("SUM RESULT = %lu\n", num);
    }
    while(num > 0);

    close(sock);
    return 0;
}
