#ifndef SERIALTALKER_H
#define SERIALTALKER_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include <QTextCodec>

class SerialTalker : public QObject
{
    Q_OBJECT
public:
    explicit SerialTalker(QObject *parent = 0);
    int init();

    int close();

signals:

public slots:

    qreal readSpeed();
private:
    QSerialPort serial;

};

#endif // SERIALTALKER_H
