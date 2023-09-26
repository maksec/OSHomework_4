#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

typedef struct {
    int costumer_cnt;
    int barber_sleep;
    int done_haircuts;
} Queue;

int main(int argc, char **argv) {
    int SERVER_PORT;
    char* SERVER_IP;

    if (argc == 1) {
        SERVER_PORT = 8000;
        SERVER_IP = "127.0.0.1";

        printf("\033[0;33m"); //yellow
        printf("CLIENTS CLIENT IS STARTING ON DEFAULT PORT %d AND DEFAULT IP %s", SERVER_PORT, SERVER_IP);
        printf("\033[0m\n");
    } else if (argc == 2) {
        SERVER_PORT = atoi(argv[1]);;
        SERVER_IP = "127.0.0.1";

        printf("\033[0;33m"); //yellow
        printf("CLIENTS CLIENT IS STARTING ON PORT %d AND DEFAULT IP %s", SERVER_PORT, SERVER_IP);
        printf("\033[0m\n");
    } else if (argc == 3) {
        SERVER_PORT = atoi(argv[1]);
        SERVER_IP = argv[2];

        printf("\033[0;33m"); //yellow
        printf("CLIENTS CLIENT IS STARTING ON PORT %d AND IP %s", SERVER_PORT, SERVER_IP);
        printf("\033[0m\n");
    } else {
        perror("Error: wrong client CLIENTS start parameters.\n");
        exit(1);
    }
    char buffer[BUFFER_SIZE];
    Queue queue;
    queue.done_haircuts = 0;
    queue.costumer_cnt = 0;
    queue.barber_sleep = 1;

    printf("SERVER IS STARTING ON PORT %d AND IP %s\n", SERVER_PORT, SERVER_IP);

    // creating server`s socket
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Error: socket() function has failed.\n");
        exit(1);
    }

    // saving server`s address
    struct sockaddr_in server_address, barber_client_address, clients_client_address, viewer_client_address, client_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address.sin_port = htons(SERVER_PORT);
    socklen_t addr_size = sizeof(client_address);

    // bind the socket to our specified IP and port
    if (bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        perror("Error: bind() function has failed.\n");
        exit(1);
    }

    printf("SERVER HAS STARTED.\n\n");

    int keep_server_alive = 1;
    int viewer_connected = 0;
    while (keep_server_alive) {
        // getting info from clients
        ssize_t recv_size = recvfrom(server_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_address, &addr_size);
        if (recv_size < 0) {
            perror("Failed to receive message.\n");
            break;
        }
        buffer[recv_size] = '\0';

        if (strcmp(buffer, "barber_initial") == 0) {
            printf("BARBER CONNECTED!\n\n");
            barber_client_address = client_address;
        } else if (strcmp(buffer, "clients_initial") == 0) {
            printf("CLIENTS CONNECTED!\n\n");
            clients_client_address = client_address;
        } else if (strcmp(buffer, "viewer_initial") == 0) {
            printf("VIEWER CONNECTED!\n\n");
            viewer_client_address = client_address;
            viewer_connected = 1;
        } else if (strcmp(buffer, "new_costumer") == 0) {
            printf("New costumer came in.\n\n");
            ++queue.costumer_cnt;
            if (queue.costumer_cnt == 1) {
                if (queue.barber_sleep == 1) {
                    ssize_t sent_size = sendto(server_socket, "barber_wake_up", strlen("barber_wake_up"), 0, (struct sockaddr *)&barber_client_address, addr_size);
                    if (sent_size < 0) {
                        perror("Failed to send message");
                        break;
                    }
                    if (viewer_connected) {
                        sent_size = sendto(server_socket, "new costumer. barber_wake_up.", strlen("new costumer. barber_wake_up."), 0, (struct sockaddr *)&viewer_client_address, addr_size);
                        if (sent_size < 0) {
                            perror("Failed to send message");
                            break;
                        }
                    }
                    printf("Barber woke up and start working.\n\n");
                } else {
                    if (viewer_connected) {
                        ssize_t sent_size = sendto(server_socket, "new costumer.", strlen("new costumer."), 0, (struct sockaddr *)&viewer_client_address, addr_size);
                        if (sent_size < 0) {
                            perror("Failed to send message");
                            break;
                        }
                    }
                    printf("Barber already working. Costumers in queue: %d\n\n", queue.costumer_cnt);
                }
            } else {
                if (viewer_connected) {
                    ssize_t sent_size = sendto(server_socket, "new costumer.", strlen("new costumer."), 0, (struct sockaddr *)&viewer_client_address, addr_size);
                    if (sent_size < 0) {
                        perror("Failed to send message");
                        break;
                    }
                }
                printf("Barber already working. Costumers in queue: %d\n\n", queue.costumer_cnt);
            }
        } else if (strcmp(buffer, "haircut_ready") == 0) {
            --queue.costumer_cnt;
            ++queue.done_haircuts;
            if (viewer_connected) {
                ssize_t sent_size = sendto(server_socket, "haircut done.", strlen("haircut done."), 0, (struct sockaddr *)&viewer_client_address, addr_size);
                if (sent_size < 0) {
                    perror("Failed to send message");
                    break;
                }
            }
            if (queue.costumer_cnt > 0) {
                ssize_t sent_size = sendto(server_socket, "barber_one_more_costumer", strlen("barber_one_more_costumer"), 0, (struct sockaddr *)&barber_client_address, addr_size);
                if (sent_size < 0) {
                    perror("Failed to send message");
                    break;
                }
                if (viewer_connected) {
                    sent_size = sendto(server_socket, "barber_one_more_costumer", strlen("barber_one_more_costumer"), 0, (struct sockaddr *)&viewer_client_address, addr_size);
                    if (sent_size < 0) {
                        perror("Failed to send message");
                        break;
                    }
                }
                printf("Barber start working on new haircut who was in queue.\n\n");
            } else {
                queue.barber_sleep = 1;
                if (viewer_connected) {
                    ssize_t sent_size = sendto(server_socket, "barber_go_sleep", strlen("barber_go_sleep"), 0, (struct sockaddr *)&viewer_client_address, addr_size);
                    if (sent_size < 0) {
                        perror("Failed to send message");
                        break;
                    }
                }
                printf("Barber go sleep.\n\n");
            }
        }
            // Логика поведения медведя
        else if (strcmp(buffer, "bear") == 0) {

        }
    }

    // close socket of server
    close(server_socket);
    return 0;
}
