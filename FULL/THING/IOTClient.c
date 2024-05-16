
#include "IOTClient.h"

void start_accelerometer();
void start_color_sensor(ColorData *colorData);


// Threads for sensors
pthread_t thread_color_sensor, thread_acc;
int thread_error;
uint8_t color_sensor_alive, accelerometer_alive;

atomic_int color_sensor_data_ready = 0;
atomic_int acc_data_ready = 0;


ColorData colorData = {0}; // Structure for saving rgb values

float ax, ay, az;       // Acceleration values


// Collect data with 1 second interval for 10 seconds and writing it in SensorData array
void collectSensorData(SensorData* data) {
	for (int i = 0; i < DATA_COUNT; i++) {
		data[i].red = colorData.red;
		data[i].green = colorData.green;
		data[i].blue = colorData.blue;
		data[i].acceleration_x = ax;
		data[i].acceleration_y = ay;
		data[i].acceleration_z = az;
		sleep(TIME_BETWEEN_MEASURES);
	}
}

// Print collected Sensor data values
void printSensorDataArray(const SensorData* data, int count) {
    for (int i = 0; i < count; i++) {
        printf("Data %d:\n", i + 1);
        printf("Acceleration X: %.2f\n", data[i].acceleration_x);
        printf("Acceleration Y: %.2f\n", data[i].acceleration_y);
        printf("Acceleration Z: %.2f\n", data[i].acceleration_z);
        printf("Red: %.2f\n", data[i].red);
        printf("Green: %.2f\n", data[i].green);
        printf("Blue: %.2f\n", data[i].blue);
        printf("\n");
    }
}

// Receiving and printing server response
void receiveServerResponse(int client_fd){
	char buffer[BUF_SIZE];
	int read_size;
	read_size = recvfrom(client_fd, buffer, BUF_SIZE, 0, NULL, NULL);
		if (read_size == -1) {
		    perror("recvfrom failed");
		    exit(EXIT_FAILURE);
		}

		printf("Received response from server - %s\n", buffer);
}

// Starts color sensor
void start_color_sensor(ColorData* colorData){
	colors(colorData);
}

// Starts accelerometer
void start_accelerometer(){
	acceleration();
}

// Main function
int main() {

    // Initialize Mosquitto library
    mosquitto_lib_init();

    // Create a Mosquitto instance
    struct mosquitto *mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        fprintf(stderr, "Error creating Mosquitto publisher\n");
        return 1;
    }

    // Set MQTT username and password
    mosquitto_username_pw_set(mosq, MQTT_USERNAME, NULL);

    // Connect to MQTT broker
    int mqtt_error = mosquitto_connect(mosq, MQTT_HOST, MQTT_PORT, 0);
    if (mqtt_error != MOSQ_ERR_SUCCESS) {
        fprintf(stderr, "Error: Connection not possible code: %d\n", mqtt_error);
        mosquitto_destroy(mosq);
        mosquitto_lib_cleanup();
        return 1;
    }

    // Start sensor processes in threads
    thread_error = pthread_create(&thread_color_sensor, NULL, start_accelerometer, NULL);
    accelerometer_alive = 1;
    thread_error = pthread_create(&thread_acc, NULL, start_color_sensor, &colorData);
    color_sensor_alive = 1;

    // Main loop
    while(1) {
        usleep(10000);

        // Represent data
        if(atomic_load(&acc_data_ready) && atomic_load(&color_sensor_data_ready)) {
            SensorData sensor_data[DATA_COUNT];
            collectSensorData(sensor_data);

            // Get current timestamp
            time_t current_time = time(NULL);

            // Send sensor data to the MQTT broker
            for (int i = 0; i < DATA_COUNT; i++) {
                char mqtt_message[512];
                snprintf(mqtt_message, sizeof(mqtt_message), "{\"time\": %ld, \"acceleration_x\": %.2f, \"acceleration_y\": %.2f, \"acceleration_z\": %.2f, \"red\": %.2f, \"green\": %.2f, \"blue\": %.2f}",
                    current_time, sensor_data[i].acceleration_x, sensor_data[i].acceleration_y, sensor_data[i].acceleration_z, sensor_data[i].red, sensor_data[i].green, sensor_data[i].blue);

                // Publish MQTT message
                mqtt_error = mosquitto_publish(mosq, NULL, MQTT_TOPIC, strlen(mqtt_message), mqtt_message, 1, false);
                if (mqtt_error != MOSQ_ERR_SUCCESS) {
                    fprintf(stderr, "Error: MQTT msg not published with code: %d\n", mqtt_error);
                }
            }

            fflush(stdout);
        }
    }

    // Cleanup
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();

    return 0;
}

