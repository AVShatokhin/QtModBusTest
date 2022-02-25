#include <QtCore/QCoreApplication>
#include <QTimer>

#include "mb_client.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    mb_client* mb = new mb_client("192.168.0.180:502", 1000, 1, 1);

    //QObject::connect(mb, &mb_client::connected, [=] {
    //    QTimer* timer = new QTimer();
    //    timer->setInterval(40);
    //    timer->start();        
    //    QObject::connect(timer, &QTimer::timeout, mb, &mb_client::readData);
    //});

    //QObject::connect(mb, &mb_client::readDataFinished, [=] {
    //    qDebug() << mb->stopMs - mb->startMs << " : " << mb->value;
    //});

    QObject::connect(mb, &mb_client::connected, [=] {
        QTimer* timer = new QTimer();        
        timer->setInterval(40);        
        timer->setTimerType(Qt::PreciseTimer);
        QObject::connect(timer, &QTimer::timeout, mb, &mb_client::writeData);
        timer->start();

        QTimer* bigTimer = new QTimer();
        bigTimer->setInterval(60000);
        QObject::connect(bigTimer, &QTimer::timeout, mb, &mb_client::flashStats);
        bigTimer->start();
    });



    //QObject::connect(mb, &mb_client::writeDataFinished, [=] {
        //qDebug() << mb->stopMs - mb->startMs << " : " << mb->valueToWrite;
    //});



    mb->mb_connect();

    return a.exec();
}
