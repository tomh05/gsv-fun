#include "serialtalker.h"

SerialTalker::SerialTalker(QObject *parent) :
    QObject(parent)
{
}

int SerialTalker::init()
{


        qDebug() << "Init" ;
    QList<QSerialPortInfo> portsInfo = QSerialPortInfo::availablePorts();


    QList<QSerialPortInfo>::iterator i;

    QSerialPortInfo treadmillPort;
    for (i = portsInfo.begin(); i != portsInfo.end(); i++) {
        qDebug() << "Name" << (*i).portName();
        qDebug() << "Desc" << (*i).description();
        qDebug() << "Manf" << (*i).manufacturer();

        if ((*i).description()=="Arduino Leonardo") {
           treadmillPort = (*i);
            break;
        }

    }

    if (treadmillPort.isNull()) {
    qDebug()<<"couldnt find" ;
       return -1;
    }


   serial.setPort(treadmillPort);

   if (serial.open((QIODevice::ReadWrite))) {
   serial.setBaudRate(9600);
   serial.clear();

   } else {
       qDebug()<<"Couldn't open port:" <<serial.errorString();
       return -1;
   }
}


int SerialTalker::close() {
    serial.close();
    return 1;
}



qreal SerialTalker::readSpeed() {

    QByteArray bytes;
    serial.waitForReadyRead(500);
    while (serial.canReadLine()) {
        bytes = (serial.readLine());
    }
    QString string = QTextCodec::codecForMib(106)->toUnicode(bytes).simplified();

    qDebug()<<"string"<<string;

    if (string != "") {
        qreal speed = string.toFloat();
        qDebug()<<"speed"<<speed;

        return speed;

    }
}
