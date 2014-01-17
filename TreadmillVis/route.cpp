#include "route.h"

Route::Route(QObject *parent) :
    QObject(parent)
{

}

int Route::loadFromFolder(QString folderPath)
{
    QFile file;
    QString contents;
    file.setFileName(folderPath + "route.dat");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (! file.atEnd() ) {
            QString fullLine = file.readLine();
            QStringList line= fullLine.split(",");
            if (line.length()==9) {
                num.append(line.at(0).toInt());
                panoID.append(line.at(1));
                latLon.append(QVector2D(line.at(2).toDouble(),line.at(3).toDouble()));    // one of these is a sligtly different
                // latLon.append(QVector2D(line.at(4).toDouble(),line.at(5).toDouble())); // coordinate provided by google.
                // which one is best?
                filePath.append(folderPath + line.at(0) + "_" + line.at(1) );
                panoBearing.append(line.at(6).toDouble());
                heading.append(line.at(8).toDouble());

                if (origin.length() == 0) {
                    origin.append(QVector3D(0.0,0.0,0.0));
                } else {
                    QVector3D newVec = latLonDiffToXYDiff(latLon.last() - latLon.at(latLon.length()-2), latLon.at(0).x()) ;
                    origin.append( origin.last() + newVec );
                }
            }


        }
        file.close();
        file.readLine();
    } else { qDebug()<<"can't open"; }
    qDebug()<<"o"<<origin.at(0);
    qDebug()<<"o"<<origin.at(1);
    qDebug()<<"route len"<<panoID.length();
}


QVector3D Route::latLonDiffToXYDiff(QVector2D latLonDiff, qreal lat0) {

    qDebug()<<"latlon"<< latLonDiff;
    qDebug()<<"lat0"<< lat0;
    qreal twoPiRad = 2 * M_PI * 6731000.0 / 360.0; // earth radius, m

    qreal xDiff = latLonDiff.y() * twoPiRad * qCos(lat0 * (M_PI/180.0) );
    qDebug()<<"qcos " << qCos(lat0 * (M_PI/180.0) );
    qreal yDiff = latLonDiff.x() * twoPiRad;

    qDebug()<<"x"<< xDiff;
    qDebug()<<"y"<< yDiff;
    //return QVector3D(xDiff,0,yDiff);
    return QVector3D(-yDiff,0,-xDiff);
    //return QVector3D(0,0,-yDiff);
}
