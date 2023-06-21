#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP address> <port>\n", argv[0]);
        exit(1);
    }

    int server_socket, client1_socket, client2_socket;
    struct sockaddr_in server_address, client1_address, client2_address;
    socklen_t client1_address_len = sizeof(client1_address);
    socklen_t client2_address_len = sizeof(client2_address);
    char buffer[BUFFER_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Failed to create server socket");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[2]));

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Failed to bind server socket");
        exit(1);
    }

    if (listen(server_socket, 2) < 0) {
        perror("Failed to listen on server socket");
        exit(1);
    }

    client1_socket = accept(server_socket, (struct sockaddr *)&client1_address, &client1_address_len);
    if (client1_socket < 0) {
        perror("Failed to accept client1 connection");
        exit(1);
    }
    printf("Client1 connected\n");

    client2_socket = accept(server_socket, (struct sockaddr *)&client2_address, &client2_address_len);
    if (client2_socket < 0) {
        perror("Failed to accept client2 connection");
        exit(1);
    }
    printf("Client2 connected\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        if (recv(client1_socket, buffer, BUFFER_SIZE, 0) < 0) {
            perror("Failed to receive message from client1");
            exit(1);
        }
        printf("Message from client1: %s\n", buffer);

        if (strcmp(buffer, "The End") == 0) {
            printf("Terminating...\n");
            break;
        }

        if (send(client2_socket, buffer, strlen(buffer), 0) < 0) {
            perror("Failed to send message to client2");
            exit(1);
        }
    }

    close(client1_socket);
    close(client2_socket);
    close(server_socket);

    return 0;
}
