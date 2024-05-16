
#ifndef MAIN_H
#define MAIN_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <mosquitto.h>


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

#define MQTT_HOST "192.168.1.41"
#define MQTT_PORT 1883
#define MQTT_TOPIC "v1/devices/me/telemetry"
#define MQTT_USERNAME "ecIHjSgQoEOv8r4rxAkU"

typedef struct {
    float acceleration_x;
    float acceleration_y;
    float acceleration_z;
    float red;
    float green;
    float blue;
} SensorData;

#endif /* MAIN_H_ */
