#include "header/login.h"

#include <QApplication>
//#include <QSplashScreen>
#include <QTimer>
#include <QMovie>
#include <QLabel>
#include<QLayout>
#include<QBoxLayout>
#include <QStyle>
#include <QDesktopWidget>
//#include <QGuiApplication>
//#include <QStyleOptionTitleBar>
#include <QIcon>
#include<QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login w;
    QFontDatabase::addApplicationFont("C:/Users/97798/Desktop/QTP/Expenditure_Traversing_system/Sf-Pro.ttf");
    QFontDatabase db;
    for(int i=0; i<db.families().size(); i++)
    {
        qDebug() << db.families().at(i);
    }

    QMovie *movie = new QMovie("C:/Users/97798/Desktop/QTP/Expenditure_Traversing_system/img/splash_screen.gif");
    QLabel *processLabel = new QLabel(NULL);
    processLabel->resize(675,450);
    processLabel->setMovie(movie);
    movie->start();
    processLabel->setWindowIcon(QIcon("C:/Users/97798/Desktop/QTP/Expenditure_Traversing_system/img/logo.ico"));
    processLabel->setWindowFlags(Qt::FramelessWindowHint);
    processLabel->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, processLabel->size(), qApp->desktop()->availableGeometry()/*QGuiApplication::screens()*/));
    processLabel->show();
    w.setWindowTitle("Expenditure Traversing System");

    QTimer::singleShot(2500, processLabel, SLOT(close()));
    QTimer::singleShot(2500, &w, SLOT(show()));

    return a.exec();
}
