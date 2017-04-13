/**
 * tcp_chat_client.c
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

#define BUFFER_SIZE 300
#define SERVER_PORT "9999"
#define GETS(B, S) {fgets(B, S, stdin); B[strlen(B)-1] = 0;}

///
/// Initiates a chat client receiving server address as parameter.
///
int main(int argc, char **argv) {
    int err, sock;
    unsigned char msg_size;
    char nick[BUFFER_SIZE], line[BUFFER_SIZE], buff[BUFFER_SIZE];
    struct addrinfo req, *list;
    fd_set rfds;
    if(argc != 2) {
        puts("Server IPv4/IPv6 address or DNS name is required as argument");
        exit(SERVER_ADDRESS_REQUIRED);
    }

    bzero((char *)&req,sizeof(req));
    // let getaddrinfo set the family depending on the supplied server address
    req.ai_family = AF_UNSPEC;
    req.ai_socktype = SOCK_STREAM;
    err = getaddrinfo(argv[1], SERVER_PORT, &req, &list);
    if(err) {
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
    freeaddrinfo(list);

    printf("Connected, enter nickname: ");
    GETS(nick, BUFFER_SIZE);
    while(1) {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(sock, &rfds);
        select(sock + 1, &rfds, NULL, NULL, NULL);
        if(FD_ISSET(0, &rfds))
        {
            GETS(line, BUFFER_SIZE);
            if(!strcmp(line, "exit"))
            {
                msg_size = 0;
                write(sock, &msg_size, 1);
                read(sock, &msg_size, 1);
                break;
            }
            sprintf(buff, "(%s) %s", nick, line);
            msg_size = strlen(buff);
            write(sock, &msg_size, 1);
            write(sock, buff, msg_size);
        }

        if(FD_ISSET(sock, &rfds))
        {
            read(sock, &msg_size, sizeof(msg_size));
            read(sock, buff, msg_size);
            buff[msg_size] = 0;
            puts(buff);
        }
    }
    close(sock);

    return 0;
}
