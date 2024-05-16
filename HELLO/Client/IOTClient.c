#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "192.168.1.41"  // Change this to the server's IP address
#define SERVER_PORT 12345      // Change this to the server's port

void receive_message_from_server(int sockfd) { //Reads message sent by server
    char buffer[1024];
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    // Receive message from the server
    ssize_t bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&server_addr, &addr_len);
    if (bytes_received == -1) {
        perror("Error receiving message");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';  // Null-terminate the received message
    printf("Received message from server: %s\n", buffer);
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char message[] = "Hello Server";  // Message to send

    // Create a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send the message to the server
    if (sendto(sockfd, message, strlen(message), 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error sending message");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Message sent to server: %s\n", message);

    //Read message from server

    receive_message_from_server(sockfd);

    close(sockfd);
    return 0;
}
