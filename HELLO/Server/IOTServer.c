#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345

int main() {
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[1024];

    // Create a UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket to the server address
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server listening on port %d...\n", PORT);

    while (1) {
        // Receive data from client
        int bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0,
                                     (struct sockaddr*)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("Error receiving data");
            continue;
        }

        // Print received data
        buffer[bytes_received] = '\0';
        printf("Received from client: %s\n", buffer);

        // Send a reply back to the client
        char reply[] = "Hello RPI";
        if(strcmp("Hello Server",buffer)!=0){
        	 strcpy(reply, "Wrong Message");}	
        sendto(sockfd, reply, strlen(reply), 0, (struct sockaddr*)&client_addr, client_len);
    }

    close(sockfd);
    return 0;
}
