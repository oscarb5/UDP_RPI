
#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>          // For standard I/O operations like printf
#include <stdlib.h>         // For standard library functions like exit
#include <string.h>         // For string manipulation functions like memset, strlen, memcpy
#include <unistd.h>         // For POSIX operating system API, including sleep and usleep
#include <pthread.h>        // For POSIX threads
#include <arpa/inet.h>      // For Internet address manipulation functions like inet_addr
#include <sys/socket.h>

#include "color_sensor.h"
#include "accelerometer.h"

#define WRITE_BUF_SIZE	1500
#define BUF_SIZE 256
#define SERVER_IP "192.168.1.41"//Home Ip
#define PORT 12345

#define BUFFER_SIZE 1024
#define DATA_COUNT 10             // Total count of measures between sends to server
#define TIME_BETWEEN_MEASURES 1   // Time between each measures
// sensor data struct to send to the server
typedef struct {
    float acceleration_x;
    float acceleration_y;
    float acceleration_z;
    float red;
    float green;
    float blue;
} SensorData;

#endif /* MAIN_H_ */
