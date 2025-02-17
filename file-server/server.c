#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define BACKLOG 3

int main() {
    int sockfd, new_sockfd;
    int addrlen = sizeof(struct sockaddr);
    struct sockaddr_in my_addr, their_addr;
    char buffer[BUFFER_SIZE] = {0};

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(PORT);
    my_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        close(sockfd);
        exit(1);
    }

    printf("\nServer listening on port %d\n", PORT);

    if ((new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, (socklen_t*)&addrlen)) == -1) {
            perror("accept");
            close(sockfd);
            exit(1);
        }

    printf("\nServer: Got connection from %s\n", inet_ntoa(their_addr.sin_addr));

    while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int bytes_received = recv(new_sockfd,buffer,BUFFER_SIZE,0);
            if (bytes_received <= 0) {
                printf("Client disconnected or error occurred.\n");
                break;
            }
            buffer[bytes_received] = '\0'; 
            printf("Client requested file: %s\n", buffer);

            FILE *file = fopen(buffer, "r");
            if (file == NULL) {
                perror("File not found!");
                const char *msg = "file not found";
                send(new_sockfd, msg, strlen(msg), 0);
                break;
            } else {
                while(fgets(buffer,BUFFER_SIZE,file)!=NULL) {
                    send(new_sockfd,buffer,BUFFER_SIZE,0);
                }
                break;
            }
        }
    close(new_sockfd);
    close(sockfd); 
    return 0;
}
