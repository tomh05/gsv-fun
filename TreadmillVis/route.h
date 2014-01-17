#ifndef ROUTE_H
#define ROUTE_H

#include <QObject>
#include <QFile>
#include <QStringList>
#include <QVector3D>
#include <QVector2D>
#include <QtCore/qmath.h>
#include <QDebug>

#define M_PI 3.14159265358979323846

class Route : public QObject
{
    Q_OBJECT
public:
    explicit Route(QObject *parent = 0);

    QString getPanoId(int i) { return panoID.at(i); }
    QVector2D getLatLon(int i) { return latLon.at(i); }
    QString getFilePath(int i) { return filePath.at(i); }
    QVector3D getOrigin(int i) { return origin.at(i); }
    qreal getHeading(int i) { return heading.at(i); }
    qreal getPanoBearing(int i) { return panoBearing.at(i); }
    int getNumPoints() { return num.length(); }

    int loadFromFolder(QString folderPath);

signals:

public slots:

private:
    QList<int> num;
    QList<QVector2D> latLon;
    QList<QString> panoID;
    QList<QVector3D> origin;
    QList<qreal> panoBearing;
    QList<QString> filePath;
    QList<qreal> heading;

    QVector3D latLonDiffToXYDiff(QVector2D latLonDiff, qreal lat0);

};

#endif // ROUTE_H
