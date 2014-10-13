//pipboy5.cpp


#include <QApplication>
#include <QLabel>
#include <QString>
#include <QTimer>
#include <QtGui>
#include "mainwindow.h"


int main(int argc, char *argv[]){
	
	QApplication app(argc,argv);
	
	MainWindow Mwin;
	Mwin.show();
	
	return app.exec();
}
