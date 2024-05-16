
#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdatomic.h>
#include <stdint.h>

#define I2C_BUS          "/dev/i2c-1"
#define MPU6050_ADDR     0x68

#define MPU6050_ACCEL_XOUT_H   0x3B
#define MPU6050_ACCEL_XOUT_L   0x3C
#define MPU6050_ACCEL_YOUT_H   0x3D
#define MPU6050_ACCEL_YOUT_L   0x3E
#define MPU6050_ACCEL_ZOUT_H   0x3F
#define MPU6050_ACCEL_ZOUT_L   0x40

#define TIME_BETWEEN_MEASURES 1

extern float ax, ay, az;       // Acceleration values
extern atomic_int acc_data_ready;
extern uint8_t accelerometer_alive;


void acceleration();
void write_acc_register(int file, unsigned char reg, unsigned char value);
void read_acceleration(int file, unsigned char* buf);
void stop_acc_measurements();

#endif /* ACCELEROMETER_H_ */
