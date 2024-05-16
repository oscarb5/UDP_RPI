
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

int main()
{
	int client_fd;
	struct sockaddr_in server_addr;


	 // Array for storing sensor data
	 SensorData sensor_data[DATA_COUNT];

	 char sensor_data_buffer[BUFFER_SIZE];

	// Create socket
	if ((client_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
	    perror("socket failed");
	    exit(EXIT_FAILURE);
	}

	// Prepare the server address structure
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);
	printf("Send\n");

	// Send "hello" message to server
	if (sendto(client_fd, "Hello Server", strlen("Hello Server"), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
	    perror("sendto failed");
	    exit(EXIT_FAILURE);
	}

	// Receive response message from server
	receiveServerResponse(client_fd);


    while(1)
    {
    	usleep(10000);
    	// Starting sensors processes in threads
    	thread_error = pthread_create(&thread_acc, NULL, &start_accelerometer, NULL);
    	accelerometer_alive = 1;
    	thread_error = pthread_create(&thread_color_sensor, NULL, &start_color_sensor, &colorData);
    	color_sensor_alive =1;;

    	// Represent data
    	if(atomic_load(&acc_data_ready) && atomic_load(&color_sensor_data_ready)){
    		collectSensorData(&sensor_data);
    		printSensorDataArray(&sensor_data, DATA_COUNT);
    	    memcpy(sensor_data_buffer, sensor_data, sizeof(sensor_data)); // serializing data
    	    // Sending sensor data to the server
    		if (sendto(client_fd, sensor_data_buffer, sizeof(sensor_data), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    		    perror("sendto failed");
    		    exit(EXIT_FAILURE);
    		}
    		receiveServerResponse(client_fd);

    		fflush(stdout);
    	}

    }
    // Close the socket
    accelerometer_alive = 0;
    color_sensor_alive = 0;
    close(client_fd);


    return 0;
}

// Starts color sensor
void start_color_sensor(ColorData* colorData){
	colors(colorData);
}

// Starts accelerometer
void start_accelerometer(){
	acceleration();
}




