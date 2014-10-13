// main.cpp

#include <QApplication>
#include <QLabel>
#include <QString>
#include <QTimer>
#include <QtGui>
#include "mainwindow.h"


int main(int argc, char *argv[]){

    QApplication app(argc,argv);

    // Set touch screen cursor to invisible for better user experience
    app.setOverrideCursor(QCursor(Qt::BlankCursor));

    MainWindow Mwin;
    Mwin.show();

    return app.exec();
}
