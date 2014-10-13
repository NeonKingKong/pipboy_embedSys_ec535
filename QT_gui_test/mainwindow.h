#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/i2c-dev.h>
#include <errno.h>
#include <math.h>
#include <asm/arch/pxa-regs.h>
#include <asm/hardware.h>

#include <QtGui>
#include <QLabel>
#include <QString>

#include <QMainWindow>

#define ADXL345_OFSX 0x1e //X offset
#define ADXL345_OFSY 0x1f //Y offset
#define ADXL345_OFSZ 0x20 //Z offset
#define ADXL345_DATAX0 0x32 //X data least sig bit
#define ADXL345_DATAX1 0x33 //X data most sig bit
#define ADXL345_DATAY0 0x34 //Y data least sig bit
#define ADXL345_DATAY1 0x35 //Y data msb
#define ADXL345_DATAZ0 0x36 //Y data msb
#define ADXL345_DATAZ1 0x37 //Y data msb
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_POWER_CTL 0x2d
#define ADXL345_I2C_ADDR 0x53
#define ADXL345_I2C_BUS "/dev/i2c-0"

/* TMP102 */
#define TMP102_I2C_ADDR 0x49
#define TMP102_I2C_BUS "/dev/i2c-0"

/* HMC5883L */
#define HMC5883L_I2C_ADDR 0x1e
#define HMC5883L_I2C_BUS "/dev/i2c-0"
#define HMC5883L_I2C_WRITE 0x3c
#define HMC5883L_I2C_READ 0x3d
#define HMC5883L_I2C_XMSB 0x03
#define HMC5883L_I2C_CNFGA 0x00
#define HMC5883L_I2C_CNFGB 0x01
#define HMC5883L_I2C_MODE 0x02

/* I2C SLAVE */
#define SLAVE_I2C_ADDR 0x0703  /* Use this slave address */	

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
	
	public:
		explicit MainWindow(QWidget *parent=0);
		~MainWindow();
		
	private slots:
		void timerTimeout();
		
	private:
		//user interface
		Ui::MainWindow *ui;

		//QT vars
		QTimer *timer;
		QLabel *temp;
		QLabel *heading;
		QLabel *xaccl, *yaccl, *zaccl;
		QLabel *screenonLabel;
		QLabel *eventcountLabel;
		
		//layout
		void createHorizontalGroupBox();

		//I2C FNCs
		void writeToDevice( int fd, int reg, int val, char * name );
		void selectDevice(int fd, int addr, char * name);
		
		//int b;//b not currently used.
		short adxl345_x, adxl345_y, adxl345_z;//adxl345 raw accelerations
		float adxl345_xa, adxl345_ya, adxl345_za;//adxl345 normalized accelerations
		float adxl345_xa_ya, adxl345_xa_za, adxl345_ya_za;//adxl345 acceleration ratios for gesture recognition
		int adxl345, tmp102, hmc5883l;//device pointers
		int tmp102_temperature;//tmp102 temperature degrees c
		unsigned char adxl345_buf[16];
		char tmp102_buf[1];
		short hmc5883l_x, hmc5883l_y, hmc5883l_z;
		float hmc5883l_head;
		unsigned char hmc5883l_buf[16];
	
		int eventcount;//track the number of gesture recognized events
		int screenon;//is the screen activated?
};
#endif //WINDOW_H
