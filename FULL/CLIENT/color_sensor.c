
#include "color_sensor.h"
int i2c_fd;
uint8_t fd_open = 0;
uint8_t light = 0;




void write_register(int i2c_fd, unsigned char reg, unsigned char value){
    struct i2c_rdwr_ioctl_data i2c_data;
    struct i2c_msg msgs[1];

    unsigned char wr_buf[2];

    wr_buf[0] = reg;
    wr_buf[1] = value;

    msgs[0].addr = COLOR_SENSOR_ADDR;
    msgs[0].flags = 0;                  // Write operation
    msgs[0].len = 2;                    // 1st byte dir, 2nd value
    msgs[0].buf = wr_buf;

    // Set up I2C transaction
    i2c_data.msgs = msgs;
    i2c_data.nmsgs = 1;

    // Send the I2C transaction
    ioctl(i2c_fd, I2C_RDWR, &i2c_data);
}

void read_color(int i2c_fd, unsigned char* buf){
    struct i2c_msg messages[2];
    struct i2c_rdwr_ioctl_data ioctl_data;

    unsigned char start_reg = 0x94;			// Clear low byte
    messages[0].addr  = COLOR_SENSOR_ADDR;  // device address
    messages[0].flags = 0;                  // Write operation
    messages[0].len   = 1;                  // One byte to write
    messages[0].buf   = &start_reg;

    messages[1].addr  = COLOR_SENSOR_ADDR;
    messages[1].flags = I2C_M_RD;
    messages[1].len   = 8;                  // Number of bytes to read (clear, red, green, blue)
    messages[1].buf   = buf;

    ioctl_data.msgs  = messages;
    ioctl_data.nmsgs = 2;

    ioctl(i2c_fd, I2C_RDWR, &ioctl_data);
}

void colors(ColorData *colorData) {
	// passing colorData structure for writing rgb values in it
	t_raw_color		raw_colors 	= {0};
	t_proc_color	proc_colors	= {0};
    unsigned char color[8];					// clear + RGB
    uint8_t error_measure = 0;

    time_t start, end;
    double elapsed_time;

    i2c_fd = open(I2C_BUS, O_RDWR);   // init file descriptor
    if (i2c_fd < 0) {
        perror("Failed to open I2C bus");
        exit(1);
    } else fd_open = 1;

    // Set the I2C slave address
    if (ioctl(i2c_fd, I2C_SLAVE, COLOR_SENSOR_ADDR) < 0) {
        perror("Failed to set the I2C slave address");
        exit(1);
    }

    write_register(i2c_fd, 0x80, 0x03); //  sets the sensor's operating mode to active data collection mode.
    usleep(5000);
    write_register(i2c_fd, 0x81, 0x00); //  disables interrupts for the sensor.
    usleep(500);
    write_register(i2c_fd, 0x83, 0x07); //  sets the prescaler to 1, which means the sensor will operate at the maximum data acquisition rate.
    usleep(500);
    write_register(i2c_fd, 0x8F, 0x00); //  disables interrupt signal locking for the sensor.
    usleep(500);
    write_register(i2c_fd, 0x8D, 0x11); // sets the integration time delay to 12 milliseconds.
    usleep(500);
    write_register(i2c_fd, 0x80, 0x43); // sets the sensor's operating mode to active data collection mode and enables light integration.

    start = time(NULL);
    while(color_sensor_alive){
    	sleep(TIME_BETWEEN_MEASURES);
    	if(!fd_open){
    		i2c_fd = open(I2C_BUS, O_RDWR);
    		    if (i2c_fd < 0) {
    		        perror("Failed to open I2C bus");
    		        exit(1);
    		    }
    	}
            end = time(NULL);
            elapsed_time = difftime(end, start);

            if (elapsed_time >= 1.0) {
            	error_measure = 0;
            	// Old data
            	atomic_exchange(&color_sensor_data_ready,0);
                read_color(i2c_fd, color);
                // saves 16 bit results of reading into separate variables
                raw_colors.clear	=	color[1] << 8 | color[0];
                raw_colors.red		= 	color[3] << 8 | color[2];
                raw_colors.green	= 	color[5] << 8 | color[4];
                raw_colors.blue		= 	color[7] << 8 | color[6];

                //Calculates the infrared (IR) component
                proc_colors.ir 		= (float)(raw_colors.red + raw_colors.green + raw_colors.blue - raw_colors.clear)/2.0;
                proc_colors.clear 	= (float)(raw_colors.clear); // Converts the channel value to a floating-point value
                proc_colors.red 	= (float)(raw_colors.red);
                proc_colors.green 	= (float)(raw_colors.green);
                proc_colors.blue 	= (float)(raw_colors.blue);

                if(proc_colors.clear != 0){
					proc_colors.red 	= proc_colors.red  * 255/65535.0;;
					proc_colors.blue 	= proc_colors.blue * 255/65535.0;;
					proc_colors.green	= proc_colors.green * 255/65535.0;;
                } else error_measure |= 0x8;

                // saving results into color data structure
                colorData->red = proc_colors.red;
                colorData->blue = proc_colors.blue;
                colorData->green = proc_colors.green;

                atomic_exchange(&color_sensor_data_ready,1);
                start = end; // reset the start time
        }
    }
    close(i2c_fd);
}





