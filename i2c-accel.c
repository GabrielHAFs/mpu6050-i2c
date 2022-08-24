#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include "smbus.h"

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include "i2c-accel.h"

#define MPU6050_I2C_ADDR 0x69
#define WHO_AM_I_REG 0x75
#define WHO_AM_I 0x68
#define REG_ACCEL_ZOUT_H 0x3F
#define REG_ACCEL_ZOUT_L 0x40
#define REG_PWR_MGMT_1 0x6B
#define REG_ACCEL_CONFIG 0x1C
#define REG_SMPRT_DIV 0x19
#define REG_CONFIG 0x1A
#define REG_FIFO_EN 0x23
#define REG_USER_CTRL 0x6A
#define REG_FIFO_COUNT_L 0x72
#define REG_FIFO_COUNT_H 0x73
#define REG_FIFO 0x74
#define REG_WHO_AM_I 0x75

#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40

void i2c_write(__u8 reg_address, __u8 val, int file);
char i2c_read(uint8_t reg_address, int file);
uint16_t merge_bytes( uint8_t LSB, uint8_t MSB);
int16_t two_complement_to_int( uint8_t LSB, uint8_t MSB);

int main(int argc, char const *argv[])
{
	int file;
	int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
	float Ax, Ay, Az;
	int adapter_nr = 3;
	char filename[250];	
	

	snprintf(filename, 250, "/dev/i2c-%d", adapter_nr);
	file = open(filename, O_RDWR);
	if (file < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}

	if (ioctl(file, I2C_SLAVE, MPU6050_I2C_ADDR) < 0) {
		/* ERROR HANDLING; you can check errno to see what went wrong */
		exit(1);
	}

	if (i2c_smbus_read_word_data(file, WHO_AM_I_REG) != WHO_AM_I) {
	/* ERROR HANDLING: i2c transaction failed */
		exit(1);
	} else {
		i2c_write(REG_PWR_MGMT_1, 0x01, file);
		i2c_write(REG_ACCEL_CONFIG, 0x00, file);
		i2c_write(REG_SMPRT_DIV, 0x07, file);
		i2c_write(REG_CONFIG, 0x00, file);
		i2c_write(REG_FIFO_EN, 0x08, file);
		i2c_write(REG_USER_CTRL, 0x44, file);
	}

	Accel_X_RAW = (int16_t)((i2c_read(ACCEL_XOUT_H, file) & 0xFF) << 8 | i2c_read(ACCEL_XOUT_L, file));
	Accel_Y_RAW = (int16_t)((i2c_read(ACCEL_YOUT_H, file) & 0xFF) << 8 | i2c_read(ACCEL_YOUT_L, file));
	Accel_Z_RAW = (int16_t)((i2c_read(ACCEL_ZOUT_H, file) & 0xFF) << 8 | i2c_read(ACCEL_ZOUT_L, file));

	Ax = Accel_X_RAW/16384.0;
	Ay = Accel_Y_RAW/16384.0;
	Az = Accel_Z_RAW/16384.0;

	printf("x_accel %.3fg	y_accel %.3fg	z_accel %.3fg         \r", Ax, Ay, Az);

	return 0;
}

void i2c_write(__u8 reg_address, __u8 val, int file) {
	char buf[2];

	buf[0] = reg_address;
	buf[1] = val;

	if(file < 0) {
		printf("Error, i2c bus is not available\n");
		exit(1);
	}

	if (write(file, buf, 2) != 2) {
		printf("Error, unable to write to i2c device\n");
		exit(1);
	}

}

char i2c_read(uint8_t reg_address, int file) {
	char buf[1];
	if(file < 0) {
		printf("Error, i2c bus is not available\n");
		exit(1);
	}

	buf[0] = reg_address;

	if (write(file, buf, 1) != 1) {
		printf("Error, unable to write to i2c device\n");
		exit(1);
	}


	if (read(file, buf, 1) != 1) {
		printf("Error, unable to read from i2c device\n");
		exit(1);
	}

	return buf[0];
}
