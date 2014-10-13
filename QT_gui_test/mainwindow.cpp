#include "mainwindow.h" 
#include "ui_mainwindow.h"

//constructor
MainWindow::MainWindow(QWidget *parent) : 
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{

	//temp layout nfo

	//set main window Ui
	ui->setupUi(this);
	
	//QT var defs
	temp = new QLabel("temp");
	temp->show();
	heading = new QLabel("heading");
	heading->show();
	xaccl = new QLabel("xaccl");
	xaccl->show();
	yaccl = new QLabel("yaccl");
	yaccl->show();
	zaccl = new QLabel("zaccl");
	zaccl->show();
	screenonLabel = new QLabel("screenon");
	screenonLabel->show();
	eventcountLabel = new QLabel("eventcount");
	eventcountLabel->show();
	timer = new QTimer(this);
	timer->setInterval(100);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
	timer->start();
	//QLabel initial setup
	
	
	
	//initial I2C device setup
	tmp102_buf[1] = 0;
	eventcount = 0;
	screenon = 1;
	
	/* initialize HMC5883l */
	if(( hmc5883l = open(HMC5883L_I2C_BUS, O_RDWR)) < 0 ){
		printf("Failed to open i2c bus for tmp102.");
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
		exit(1);
	}
	
	selectDevice(hmc5883l, HMC5883L_I2C_ADDR, "hmc5883l");
	
	writeToDevice(hmc5883l, HMC5883L_I2C_CNFGA, 0x74, "hmc5883l");//00010000 --> wrote to config RegA (pointer updated) (default)
	writeToDevice(hmc5883l, HMC5883L_I2C_CNFGB, 0x20, "hmc5883l");//00100000 --> wrote to config RegB (pointer updated) (default)
	writeToDevice(hmc5883l, HMC5883L_I2C_MODE, 0x00, "hmc5883l");//00000000 --> wrote to config RegC (pointer updated) (continuous measure mode)
	usleep(67000);//hold
	//writeToDevice(hmc5883l, HMC5883L_I2C_WRITE, HMC5883L_I2C_XMSB, "hmc5883l");//00000011 --> move to data output x msb reg
	
	/* initialize TMP102 */
	if( ( tmp102 = open(TMP102_I2C_BUS, O_RDWR) ) < 0 ){
		printf("Failed to open i2c bus for tmp102.");
		/* ERROR HANDLING; you can check errno to see what went wrong */	
		printf("error: %s (%d)\n", strerror(errno), errno);
		exit(1);
	}
	
	selectDevice(tmp102, TMP102_I2C_ADDR, "tmp102");
	
	writeToDevice(tmp102, 0x00, 1 , "tmp102");
	
	/* initialize ADXL345 */
	if( ( adxl345 = open(ADXL345_I2C_BUS, O_RDWR) ) < 0 ){
		//open port for read/write
		printf( "Failed to open i2c bus for adxl345\n" );
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
		exit(1);
	}
	
	selectDevice(adxl345, ADXL345_I2C_ADDR, "ADXL345");
	
	//why do all these?
	writeToDevice( adxl345, ADXL345_POWER_CTL, 0, "adxl345" );
	writeToDevice( adxl345, ADXL345_POWER_CTL, 16, "adxl345"  );
	writeToDevice( adxl345, ADXL345_POWER_CTL, 8, "adxl345"  );
	writeToDevice( adxl345, ADXL345_POWER_CTL, 11, "adxl345"  );
}


MainWindow::~MainWindow()
{

    delete ui;
}

/* I2C FUNCTIONS */
void MainWindow::selectDevice(int fd, int addr, char * name){

	if( ioctl( fd, SLAVE_I2C_ADDR, addr ) < 0 ){
		printf( "%s not present\n", name );
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
	}
}

void MainWindow::writeToDevice( int fd, int reg, int val, char * name ){
	
	char buf[2];
	buf[0] = reg; buf[1] = val;
	
	if( write( fd, buf, 2 ) != 2 ){
		printf( "Can't write to %s\n", name);
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
	}
}

void MainWindow::timerTimeout(){
	/* ADXL345 */
	selectDevice( adxl345, ADXL345_I2C_ADDR, "ADXL345" );
	
	adxl345_buf[0] = ADXL345_DATAX0; 
		
	if( ( write(adxl345, adxl345_buf, 1) ) != 1 ){
		//send register to read from
		printf( "Error writing to I2C slave\n" );
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
		//exit(1);
	}
		
	if( read( adxl345, adxl345_buf, 6 ) !=6 ){
		//xyz accels
		printf( "Unable to read from ADXL345\n" );
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
	}
		
	else{
		adxl345_x = adxl345_buf[1]<<8| adxl345_buf[0];
		adxl345_y = adxl345_buf[3]<<8| adxl345_buf[2];
		adxl345_z = adxl345_buf[5]<<8| adxl345_buf[4];
		adxl345_xa = (90.0 / 256.0) * (float) adxl345_x;
		adxl345_ya = (90.0 / 256.0) * (float) adxl345_y;
		adxl345_za = (90.0 / 256.0) * (float) adxl345_z;
		//printf("adxl345_x=%d, adxl345_y=%d, adxl345_z=%d\n", adxl345_x, adxl345_y, adxl345_z);
		//printf("%4.0f x %4.0f y %4.0f z, ", adxl345_xa, adxl345_ya, adxl345_za);
		//for (b=0; b<6; b++) printf("%02x ",adxl345_buf[b]); printf("\n");
	}
	/* Gesture Recognition */
	adxl345_xa_ya = adxl345_xa/adxl345_ya;
	adxl345_xa_za = adxl345_xa/adxl345_za;
	adxl345_ya_za = adxl345_ya/adxl345_za;
	
	if( !screenon ){
		if( (adxl345_xa_ya > -1.0) && (adxl345_xa_ya < 1.0) && (adxl345_xa_za > -1.0) 
		 && (adxl345_xa_za < 1.0) && (adxl345_ya_za > 0) && (adxl345_ya_za < 5.0) ){
			eventcount++;
			//printf( "If1");//DEBUG
		} else {
			eventcount = 0;
			//printf( "Else1");//DEBUG
		}
		if(eventcount > 15){
			screenon = 1;
			//printf( "I'm on bithces!\n");
			eventcount = 0;
			system("pxaregs LCCRO 0x003008f9");
			system("echo 0 > /sys/class/graphics/fb0/blank");
		}
	}
	if( screenon ){
		if( !( (adxl345_xa_ya > -1.0) && (adxl345_xa_ya < 1.0) && (adxl345_xa_za > -1.0)
			&& (adxl345_xa_za < 1.0) && (adxl345_ya_za > 0) && (adxl345_ya_za < 5.0) ) ){
			printf("If2");
			eventcount++;
		} else {
			eventcount = 0;
			//printf( "Else2");//DEBUG
		}
		if(eventcount > 25){
			screenon = 0;
			//printf( "I'm off bithces!\n");
			eventcount = 0;
			system("echo 1 > /sys/class/graphics/fb0/blank");
			system("pxaregs LCCR0 0x00300cf8");//experimental
		}
	}
		
	/* TMP102 */
	//float data;
	//read 2 uint8 using I2C read
	int k = read(tmp102, tmp102_buf, 2);
	if((k != 2)){
		printf( "tmp102 failed to report data" );
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
	} else {
		
		tmp102_temperature = ((tmp102_buf[0])<<8)|(tmp102_buf[1]);
		tmp102_temperature >>= 4;
		
		//the tmp102 does twos complement but has the neg bit in the wrong spot
		//so test for it and correct if needed
		if(tmp102_temperature & (1 << 11)){
			tmp102_temperature |= 0xF800; 	//set bits 11 to 15 to 1s to get this 
									//reading into real twos complement. 
		}
		printf( "temp: %04f, ", tmp102_temperature * 0.0625);
	}
	/* HMC5883L */
	selectDevice(hmc5883l, HMC5883L_I2C_ADDR, "hmc5883l");
	hmc5883l_buf[0] = HMC5883L_I2C_XMSB;
	if( ( write(hmc5883l, hmc5883l_buf, 1 ) ) != 1 ){
		//send register to read from
		printf( "Error writing to I2C slave\n" );
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
		//exit(1);
	}
	if( read( hmc5883l, hmc5883l_buf, 6 ) !=6 ){
		//xyz mags
		printf( "Unable to read from hmc5883l\n" );
		/* ERROR HANDLING; you can check errno to see what went wrong */
		printf("error: %s (%d)\n", strerror(errno), errno);
	} else {
		//printf(" IN ELSE OF MAG\n");//DEBUG
		hmc5883l_x = hmc5883l_buf[1]<<8| hmc5883l_buf[0];
		hmc5883l_z = hmc5883l_buf[3]<<8| hmc5883l_buf[2];
		hmc5883l_y = hmc5883l_buf[5]<<8| hmc5883l_buf[4];
		hmc5883l_head = atan2 (hmc5883l_y,hmc5883l_x);
		if( hmc5883l_head < 0 ){
			hmc5883l_head += 2*3.14159265358979323846; 
		}
		hmc5883l_head = hmc5883l_head * 180/3.14159265358979323846;
		
		/*hmc5883l_x = hmc5883l_buf[0] * 256 + hmc5883l_buf[1];
		hmc5883l_y = hmc5883l_buf[4] * 256 + hmc5883l_buf[5];
		hmc5883l_head = atan2((double)hmc5883l_y,(double)hmc5883l_x)*180/3.14+180;*/

		printf(" Compass x: %d, y: %d, heading: %4.0f\n", hmc5883l_x, hmc5883l_y, hmc5883l_head);
		//writeToDevice(hmc5883l, HMC5883L_I2C_WRITE, HMC5883L_I2C_XMSB, "hmc5883l"); // resets pointer back to the first data register
	}
	//printf(" REACHED END\n" );//DEBUG
	printf(" x/y: %4.4f x/z: %4.4f y/z: %4.4f :::: eventcount %d, screenon %d \n", adxl345_xa_ya, adxl345_xa_za, adxl345_ya_za, eventcount, screenon);
	
	//set all label values. 
	/*
	temp = new QLabel("temp");
	heading = new QLabel("heading");
	xaccl = new QLabel("xaccl");
	yaccl = new QLabel("yaccl");
	zaccl = new QLabel("zaccl");
	timer = new QTimer(this);
	
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

	int eventcount = 0;//track the number of gesture recognized events
	int screenon = 1;//is the screen activated?
	*/
	
	ui->textEdit->setText("Heading: " + QString::number(hmc5883l_head));
	ui->textEdit_2->setText("Temp: " + QString::number(tmp102_temperature * 0.0625));
	ui->textEdit_3->setText("Xaccel: " + QString::number(adxl345_xa));
	ui->textEdit_4->setText("Yaccel: " + QString::number(adxl345_ya));
	ui->textEdit_5->setText("Zaccel: " + QString::number(adxl345_za));
	ui->textEdit_6->setText("SO: " + QString::number(screenon));
	ui->textEdit_7->setText("EC: " + QString::number(eventcount));
	timer->start();
}

