/**
 * tcp_chat_server.c
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "constants.h"

#define BUFFER_SIZE 400
#define SERVER_PORT "9999"

int main(void) {
    struct sockaddr_storage from;
    int err, new_sock, sock, i, j;
    unsigned int addr_len;
    unsigned char lsize;
    int max_fd, new_max_fd;
    char linha[BUFFER_SIZE], client_ip_text[BUFFER_SIZE], client_port_text[BUFFER_SIZE];
    struct addrinfo req, *list;
    fd_set rfds, rfds_master;

    bzero((char *)&req, sizeof(req));
    req.ai_family = AF_INET6; // allow both IPv4 and IPv6 clients
    req.ai_socktype = SOCK_STREAM; // TCP
    req.ai_flags = AI_PASSIVE; // local address
    err = getaddrinfo(NULL, SERVER_PORT, &req, &list);
    if(err) {
        printf("Failed to get local address, error: %s\n", gai_strerror(err));
        exit(INVALID_LOCAL_ADDRESS);
    }

    sock = socket(list->ai_family, list->ai_socktype, list->ai_protocol);
    if(sock < 0) {
        perror("Failed to open socket");
        freeaddrinfo(list);
        exit(OPEN_SOCKET_FAILED);
    }

    if(bind(sock, (struct sockaddr *)list->ai_addr, list->ai_addrlen) < 0) {
        perror("Bind failed");
        close(sock);
        freeaddrinfo(list);
        exit(BIND_SOCKET_FAILED);
    }
    freeaddrinfo(list);

    listen(sock, SOMAXCONN);
    FD_ZERO(&rfds_master);
    FD_SET(sock, &rfds_master); // initially only the socket accepting connections
    new_max_fd = sock;
    puts("Accepting TCP connections (both over IPv6 or IPv4). Use CTRL+C to terminate the server");
    addr_len = sizeof(from);
    while(1) {
        max_fd = new_max_fd;
        FD_ZERO(&rfds);
        for(i = 0; i <= max_fd; i++) {
            if(FD_ISSET(i, &rfds_master)) {
                FD_SET(i, &rfds);
            }
        }
        select(max_fd+1, &rfds, NULL, NULL, NULL);

        for(i = 0; i <= max_fd; i++) {
            if(FD_ISSET(i, &rfds_master) && FD_ISSET(i, &rfds))
            {
                if(i == sock)
                {
                    new_sock = accept(sock, (struct sockaddr *)&from, &addr_len);
                    getnameinfo((struct sockaddr *)&from, addr_len, client_ip_text, BUFFER_SIZE,
                                client_port_text, BUFFER_SIZE, NI_NUMERICHOST|NI_NUMERICSERV);
                    printf("New connection from %s, port %s\n", client_ip_text, client_port_text);
                    FD_SET(new_sock, &rfds_master);
                    if(new_sock > new_max_fd) {
                        new_max_fd = new_sock;
                    }
                }
                else
                {
                    read(i,&lsize,1);
                    if(!lsize) {
                        FD_CLR(i,&rfds_master);
                        write(i,&lsize,1);
                        close(i);
                        puts("One client has disconnect");
                    }
                    else
                    {
                        read(i, linha, lsize);
                        for(j = 0; j <= max_fd; j++) {
                            if(j != sock && FD_ISSET(j, &rfds_master))
                            {
                                write(j, &lsize, 1);
                                write(j, linha, lsize);

                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

