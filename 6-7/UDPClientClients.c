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
    int CLIENT_PORT = atoi(argv[1]);
    char* CLIENT_IP = argv[2];
    char buffer[BUFFER_SIZE];
    char new_costumer[] = "new_costumer";
    char haircut_ready[] = "haircut_ready";

    printf("CLIENTS CLIENT IS STARTING ON PORT %d AND IP %s\n\n", CLIENT_PORT, CLIENT_IP);

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

    ssize_t sent_size = sendto(client_socket, "clients_initial", strlen("clients_initial"), 0, (struct sockaddr *)&server_address, addr_size);
    if (sent_size < 0) {
        perror("Error: failed to send message.\n");
        exit(-1);
    }

    while (1) {
        printf("PRESS ENTER IF NEW COSTUMER CAME: ");
        fgets(buffer, sizeof(buffer), stdin);
        printf("New costumer came in.\n\n");
        // send message to server
        ssize_t sentLen = sendto(client_socket, new_costumer, strlen(new_costumer), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        if (sentLen < 0) {
            perror("Error: failed to send message.\n");
            break;
        }
    }

    // close socket
    close(client_socket);
    return 0;

}
