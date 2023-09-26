#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
    int CLIENT_PORT;
    char* CLIENT_IP;

    if (argc == 1) {
        CLIENT_PORT = 8000;
        CLIENT_IP = "127.0.0.1";

        printf("\033[0;33m"); //yellow
        printf("VIEWER CLIENT IS STARTING ON DEFAULT PORT %d AND DEFAULT IP %s", CLIENT_PORT, CLIENT_IP);
        printf("\033[0m\n");
    } else if (argc == 2) {
        CLIENT_PORT = atoi(argv[1]);;
        CLIENT_IP = "127.0.0.1";

        printf("\033[0;33m"); //yellow
        printf("VIEWER CLIENT IS STARTING ON PORT %d AND DEFAULT IP %s", CLIENT_PORT, CLIENT_IP);
        printf("\033[0m\n");
    } else if (argc == 3) {
        CLIENT_PORT = atoi(argv[1]);
        CLIENT_IP = argv[2];

        printf("\033[0;33m"); //yellow
        printf("VIEWER CLIENT IS STARTING ON PORT %d AND IP %s", CLIENT_PORT, CLIENT_IP);
        printf("\033[0m\n");
    } else {
        perror("Error: wrong client CLIENTS start parameters.\n");
        exit(1);
    }

    char buffer[BUFFER_SIZE];

    printf("VIEWER CLIENT IS STARTING ON PORT %d AND IP %s\n\n", CLIENT_PORT, CLIENT_IP);

    // creating client`s socket
    int client_socket;
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error: socket() function has failed.\n");
        exit(1);
    }

    // saving server`s address
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(CLIENT_IP);
    server_address.sin_port = htons(CLIENT_PORT);
    socklen_t addr_size = sizeof(server_address);

    ssize_t sent_size = sendto(client_socket, "viewer_initial", strlen("viewer_initial"), 0, (struct sockaddr *)&server_address, addr_size);
    if (sent_size < 0) {
        perror("Error: failed to send message.\n");
        exit(-1);
    }

    while (1) {
        // receive message from server
        memset(buffer, '\0', BUFFER_SIZE);
        ssize_t recv_size = recvfrom(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, &addr_size);
        if (recv_size < 0) {
            perror("Failed to receive message.\n");
            break;
        }
        buffer[recv_size] = '\0';

        printf("%s\n\n", buffer);
    }

    // close socket
    close(client_socket);
    return 0;

}
