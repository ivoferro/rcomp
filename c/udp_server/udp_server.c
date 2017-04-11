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
#define SERVER_PORT "9999"

int main(void) {
    struct sockaddr_storage client;
    int err, sock, res, i;
    unsigned int addr_len;
    char line[BUFFER_SIZE], line_aux[BUFFER_SIZE];
    char client_ip_text[BUFFER_SIZE], client_port_text[BUFFER_SIZE];
    struct addrinfo req, *list;

    bzero((char *)&req,sizeof(req));
    // requesting a IPv6 local address will allow both IPv4 and IPv6 clients
    req.ai_family = AF_INET6;
    req.ai_socktype = SOCK_DGRAM;
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

    puts("Listening for UDP requests (both over IPv6 or IPv4). Use CTRL+C to terminate the server");
    addr_len = sizeof(client);
    while(1)
    {
        res = recvfrom(sock, line, BUFFER_SIZE, 0, (struct sockaddr *)&client, &addr_len);
        getnameinfo((struct sockaddr *)&client, addr_len, client_ip_text, BUFFER_SIZE,
                    client_port_text, BUFFER_SIZE, NI_NUMERICHOST|NI_NUMERICSERV);
        printf("Request from node %s, port number %s\n", client_ip_text, client_port_text);
        for(i = 0; i < res; i++) {
            line_aux[res-1-i]=line[i]; // mirror the string
        }
        sendto(sock, line_aux, res, 0, (struct sockaddr *)&client, addr_len);
    }
    close(sock);

    return 0;
}
