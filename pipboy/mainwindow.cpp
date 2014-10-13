// mainwindow.cpp

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QPushButton>
#include <QMessageBox>


/*****************************************************************************/
// CONSTRUCTOR - MainWindow
/*****************************************************************************/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainWindow){

    // Set window preferences for custom UI look
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);             //Set window to fixed size
    this->setWindowFlags(Qt::CustomizeWindowHint);                      //Set window with no title bar
    this->setWindowFlags(Qt::FramelessWindowHint);                      //Set a frameless window

    // Set main window Ui
    ui->setupUi(this);

    // Set all transition buttons transparent. *This seems easy, but without Qt experience, it was not trivial!
    ui->pushButton->setFlat(true);
    ui->pushButton->setStyleSheet("background-color: transparent;");
    ui->pushButton_2->setFlat(true);
    ui->pushButton_2->setStyleSheet("background-color: transparent;");
    ui->pushButton_3->setFlat(true);
    ui->pushButton_3->setStyleSheet("background-color: transparent;");
    ui->pushButton_4->setFlat(true);
    ui->pushButton_4->setStyleSheet("background-color: transparent;");
    ui->pushButton_5->setFlat(true);
    ui->pushButton_5->setStyleSheet("background-color: transparent;");

    // Connect pushbutton signals to the clicked slot
    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(clickedSlot()));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(clickedSlot()));
    QObject::connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(clickedSlot()));
    QObject::connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(clickedSlot()));
    QObject::connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(clickedSlot()));

    // Qt Variable defs
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

    // Timer set up and initialization
    timer = new QTimer(this);
    timer->setInterval(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    timer->start();

    // Initial I2C device setup
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


/*****************************************************************************/
// DESTRUCTOR - MainWindow
/*****************************************************************************/
MainWindow::~MainWindow(){
    delete ui;
}


/*****************************************************************************/
// SLOT - Button push handler
//        All button names were set in the UI Creator and based on a click input
//        from a user's touch, this clickedSlot handler sets the current index
//        of the Stacked Widget structure (each page containing a different
//        Pipboy screen). Essentially, this is the page turner using a Signal and
//        Slot approach.
/*****************************************************************************/
void MainWindow::clickedSlot()
{
    if(((QPushButton*)sender())->objectName() == "pushButton") {
        ui->stackedWidget->setCurrentIndex(1);
    } else if (((QPushButton*)sender())->objectName() == "pushButton_2"){
        ui->stackedWidget->setCurrentIndex(0);
    } else if (((QPushButton*)sender())->objectName() == "pushButton_3") {
        ui->stackedWidget->setCurrentIndex(2);
    } else if (((QPushButton*)sender())->objectName() == "pushButton_4") {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        // do nothing
    }
}


/*****************************************************************************/
// I2C HELPER FUNCTION - Select Device
/*****************************************************************************/
void MainWindow::selectDevice(int fd, int addr, char * name){

    if( ioctl( fd, SLAVE_I2C_ADDR, addr ) < 0 ){
        printf( "%s not present\n", name );
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("error: %s (%d)\n", strerror(errno), errno);
    }
}


/*****************************************************************************/
// I2C HELPER FUNCTION - Write To Device
/*****************************************************************************/
void MainWindow::writeToDevice( int fd, int reg, int val, char * name ){

    char buf[2];
    buf[0] = reg; buf[1] = val;

    if( write( fd, buf, 2 ) != 2 ){
        printf( "Can't write to %s\n", name);
        /* ERROR HANDLING; you can check errno to see what went wrong */
        printf("error: %s (%d)\n", strerror(errno), errno);
    }
}


/*****************************************************************************/
// QMouseEvent method
/*****************************************************************************/
void MainWindow::mousePressEvent(QMouseEvent *event) {
    // Unused method because mouse click slot handler has been invoked.
    // Unsure of the necessity of retaining this mousePressEvent for ability to click.
}



/*****************************************************************************/
// Main Function - Timer Timeout
/*****************************************************************************/
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


    // Update text labels displaying accelerometer data
    ui->label_3->setText("X:\t" + QString::number(adxl345_xa));
    ui->label_4->setText("Y:\t" + QString::number(adxl345_ya));
    ui->label_5->setText("Z:\t" + QString::number(adxl345_za));

    // Update text labels displaying thermistor data
    ui->TEMP_C->setText("TEMP (C):\t" + QString::number(tmp102_temperature * 0.0625));
    ui->TEMP_F->setText("TEMP (F):\t" + QString::number(((tmp102_temperature * 0.0625) * (1.8)) + 32));

    // Update text labels displaying magnetometer data
    ui->HEAD->setText("HEADING:\t" + QString::number(hmc5883l_head));
    ui->HEAD_X->setText("X:\t" + QString::number(hmc5883l_x));
    ui->HEAD_Y->setText("Y:\t" + QString::number(hmc5883l_y));
    ui->HEAD_Z->setText("Z:\t" + QString::number(hmc5883l_z));

    // Timer restart - may not need this?
    timer->start();
}

