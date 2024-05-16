
#ifndef COLOR_SENSOR_H
#define COLOR_SENSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdatomic.h>
#include <stdint.h>
#include <time.h>

#define I2C_BUS "/dev/i2c-1"
#define COLOR_SENSOR_ADDR   0x29
#define TIME_BETWEEN_MEASURES 1

void write_register(int file, unsigned char reg, unsigned char value);
void read_color(int file, unsigned char* buf);
void init_signals();
void restore_signals();
void toggle_flash(uint8_t *flash);

extern uint8_t color_sensor_alive;
extern atomic_int color_sensor_data_ready;

extern char color_sensor_msg[1500];


typedef struct{
	uint16_t clear;					// Clear
	uint16_t red, green, blue;		// RGB values
}t_raw_color;

typedef struct{
	float ir;
	float clear;
	float red, green, blue;		// RGB values
}t_proc_color;

// Struct for rgb data
typedef struct {
    float red;
    float green;
    float blue;
} ColorData;

void colors(ColorData *colorData);
#endif /* COLOR_SENSOR_H_ */
