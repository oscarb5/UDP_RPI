#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>

#define BUF_SIZE 256
#define SERVER_IP "192.168.1.41"
#define PORT 12345
#define BUFFER_SIZE 1024
#define DATA_COUNT 10

// Struct for sensor data received from client
typedef struct {
    float acceleration_x;
    float acceleration_y;
    float acceleration_z;
    float red;
    float green;
    float blue;
} SensorData;


void receiveSensorData(int sockfd, const struct sockaddr* clientAddr, socklen_t addrLen) {
    char buffer[BUFFER_SIZE];

    // Receive data from the client
    ssize_t bytesRead = recvfrom(sockfd, buffer, sizeof(SensorData) * DATA_COUNT, 0, (struct sockaddr*)clientAddr, &addrLen);
    if (bytesRead < 0) {
        perror("Failed to receive data");
        return;
    }

    // Deserialize the received data
    SensorData* data = (SensorData*)buffer;

    SensorData min_data = data[0];
    SensorData max_data = data[0];
    SensorData average_data = {0};
    SensorData deviation_data = {0};

    // Process the received data
    printf("||********** VALUES RECEIVED **********||\n\n");
    for (int i = 0; i < DATA_COUNT; i++) {
        printf("Received SensorData[%d]: accel_x=%.2f, accel_y=%.2f, accel_z=%.2f, red=%.2f, green=%.2f, blue=%.2f\n",
            i, data[i].acceleration_x, data[i].acceleration_y, data[i].acceleration_z, data[i].red, data[i].green, data[i].blue);

        // Max and min calc
        if (data[i].red > max_data.red) {
        	max_data.red = data[i].red;
        }
        if (data[i].blue > max_data.blue) {
        	max_data.blue = data[i].blue;
        }
        if (data[i].green > max_data.green) {
        	max_data.green = data[i].green;
        }
        if (data[i].acceleration_x > max_data.acceleration_x) {
            max_data.acceleration_x = data[i].acceleration_x;
        }
        if (data[i].acceleration_y > max_data.acceleration_y) {
            max_data.acceleration_y = data[i].acceleration_y;
        }
        if (data[i].acceleration_z > max_data.acceleration_z) {
            max_data.acceleration_z = data[i].acceleration_z;
        }

        if (data[i].red < min_data.red) {
        	min_data.red = data[i].red;
        }
        if (data[i].blue < min_data.blue) {
        	min_data.blue = data[i].blue;
        }
        if (data[i].green < min_data.green) {
        	min_data.green = data[i].green;
        }
        if (data[i].acceleration_x < min_data.acceleration_x) {
            min_data.acceleration_x = data[i].acceleration_x;
        }
        if (data[i].acceleration_y < min_data.acceleration_y) {
            min_data.acceleration_y = data[i].acceleration_y;
        }
        if (data[i].acceleration_z < min_data.acceleration_z) {
            min_data.acceleration_z = data[i].acceleration_z;
        }

        // Mean calc
        average_data.acceleration_x += data[i].acceleration_x;
        average_data.acceleration_y += data[i].acceleration_y;
        average_data.acceleration_z += data[i].acceleration_z;
        average_data.red += data[i].red;
        average_data.green += data[i].green;
        average_data.blue += data[i].blue;
    }

    average_data.acceleration_x = average_data.acceleration_x / DATA_COUNT;
    average_data.acceleration_y = average_data.acceleration_y / DATA_COUNT;
    average_data.acceleration_z = average_data.acceleration_z / DATA_COUNT;
    average_data.red = average_data.red / DATA_COUNT;
    average_data.green = average_data.green / DATA_COUNT;
    average_data.blue = average_data.blue / DATA_COUNT;

    // Deviation calc
    for (int i = 0; i < DATA_COUNT; i++) {
    	float diff_ax = data[i].acceleration_x - average_data.acceleration_x;
    	float diff_ay = data[i].acceleration_y - average_data.acceleration_y;
    	float diff_az = data[i].acceleration_z - average_data.acceleration_z;
    	float diff_red = data[i].red - average_data.red;
    	float diff_green = data[i].green - average_data.green;
    	float diff_blue = data[i].blue - average_data.blue;

    	deviation_data.acceleration_x += diff_ax * diff_ax;
    	deviation_data.acceleration_y += diff_ay * diff_ay;
    	deviation_data.acceleration_z += diff_az * diff_az;
    	deviation_data.red += diff_red * diff_red;
    	deviation_data.green += diff_green * diff_green;
    	deviation_data.blue += diff_blue * diff_blue;
    }

    deviation_data.acceleration_x = sqrt(deviation_data.acceleration_x / DATA_COUNT);
    deviation_data.acceleration_y = sqrt(deviation_data.acceleration_y / DATA_COUNT);
    deviation_data.acceleration_z = sqrt(deviation_data.acceleration_z / DATA_COUNT);
    deviation_data.red = sqrt(deviation_data.red / DATA_COUNT);
    deviation_data.green = sqrt(deviation_data.green / DATA_COUNT);
    deviation_data.blue = sqrt(deviation_data.blue / DATA_COUNT);

    // Output of all received and computed data
    printf("||********** STATISTICS **********||\n\n");
    printf("Mean Sensor data: accel_x=%.2f, accel_y=%.2f, accel_z=%.2f, red=%.2f, green=%.2f, blue=%.2f\n",
    		average_data.acceleration_x, average_data.acceleration_y, average_data.acceleration_z,
			average_data.red, average_data.green, average_data.blue);
    printf("Maximum Sensor data: accel_x=%.2f, accel_y=%.2f, accel_z=%.2f, red=%.2f, green=%.2f, blue=%.2f\n",
        		max_data.acceleration_x, max_data.acceleration_y, max_data.acceleration_z,
				max_data.red, max_data.green, max_data.blue);
    printf("Minimum Sensor data: accel_x=%.2f, accel_y=%.2f, accel_z=%.2f, red=%.2f, green=%.2f, blue=%.2f\n",
           		min_data.acceleration_x, min_data.acceleration_y, min_data.acceleration_z,
				min_data.red, min_data.green, min_data.blue);
    printf("Standard deviation of Sensor data: accel_x=%.2f, accel_y=%.2f, accel_z=%.2f, red=%.2f, green=%.2f, blue=%.2f\n",
    		deviation_data.acceleration_x, deviation_data.acceleration_y, deviation_data.acceleration_z,
			deviation_data.red, deviation_data.green, deviation_data.blue);
    printf("||********** END OF DATA OUTPUT **********||\n\n");
}


int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUF_SIZE];
    int read_size;
    socklen_t client_addr_len = sizeof(client_addr);


    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Prepare the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Bind socket to server address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        printf("Error binding socket");
        exit(EXIT_FAILURE);
    }

    printf("UDP server listening on port %d.... \n", PORT);
    // Receive message from client
    read_size = recvfrom(server_fd, buffer, BUF_SIZE, 0, (struct sockaddr*)&client_addr, &client_addr_len);
    if (read_size == -1) {
        printf("recvfrom failed");
        exit(EXIT_FAILURE);
    }

    printf("Received message from client - %s\n", buffer);

    // Send response message to client
    if (sendto(server_fd, "Hello Client", strlen("Hello Client"), 0, (struct sockaddr*)&client_addr, client_addr_len) == -1) {
        perror("sendto failed");
        exit(EXIT_FAILURE);
    }

    while (1) {

    	receiveSensorData(server_fd, (struct sockaddr*)&client_addr, client_addr_len);
    	// Sends the acknowledge message to the client
    	if (sendto(server_fd, "Server got sensor data!", strlen("Server got sensor data!"), 0, (struct sockaddr*)&client_addr, client_addr_len) == -1) {
    	        perror("sendto failed");
    	        exit(EXIT_FAILURE);
    	}
    }

    // Close the sockets
    close(server_fd);

    return 0;
}

