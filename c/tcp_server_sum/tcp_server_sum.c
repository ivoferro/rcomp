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

int main(void) {
    struct sockaddr_storage client_addr;
    int err, new_sock, sock;
    unsigned int addr_len;
    unsigned long i, f, n, num, sum;
    unsigned char bt;
    char cli_ip_text[BUFFER_SIZE], cli_port_text[BUFFER_SIZE];
    struct addrinfo req, *list;

    bzero((char *)&req, sizeof(req));
    req.ai_family = AF_INET6; // allow both IPv4 and IPv6 clients to use this server
    req.ai_socktype = SOCK_STREAM;
    req.ai_flags = AI_PASSIVE; // local address
    err = getaddrinfo(NULL, SERVER_PORT, &req, &list);
    if(err) {
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
        perror("Bind failed");
        close(sock);
        freeaddrinfo(list);
        exit(BIND_SOCKET_FAILED);
    }
    freeaddrinfo(list);

    listen(sock, SOMAXCONN);
    puts("Accepting TCP connections (both over IPv6 or IPv4). Use CTRL+C to terminate the server");
    addr_len = sizeof(client_addr);

    while(1) {
        new_sock = accept(sock, (struct sockaddr *)&client_addr, &addr_len);
        if(fork() == 0)
        {
            close(sock);
            getnameinfo((struct sockaddr *)&client_addr, addr_len, cli_ip_text, BUFFER_SIZE,
                        cli_port_text, BUFFER_SIZE, NI_NUMERICHOST|NI_NUMERICSERV);
            printf("New connection from %s, port number %s\n", cli_ip_text, cli_port_text);
            do {
                sum = 0;
                do {
                    num = 0;
                    f = 1;
                    for(i = 0; i < 4; i++) {
                        read(new_sock, &bt, 1);
                        num = num + bt * f;
                        f = 256 * f;
                    }
                    sum = sum + num;
                }
                while(num > 0);

                n = sum;
                for(i = 0; i < 4; i++) {
                    bt = n % 256;
                    write(new_sock, &bt, 1);
                    n = n / 256;
                }
            }
            while(sum > 0);
            close(new_sock);

            printf("Connection from %s, port number %s closed\n", cli_ip_text, cli_port_text);
            exit(0);
        }
        close(new_sock);

    }
    close(sock);

    return 0;
}
