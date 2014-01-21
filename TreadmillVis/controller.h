#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QTimer>

//#include <window.h>
#include <glwidget.h>
#include <mesh.h>
#include <route.h>

class GLWidget;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    void run();
    qreal getDistanceTravelled() { return distanceTravelled; }
    void setSpeed(qreal s) { speed = s;}
    qreal getSpeed() { return speed; }

signals:

public slots:
    void distanceTimer();

private:
    GLWidget * glWidget;
    QList<Mesh*> * meshes;
    Route * route;
    qreal distanceTravelled;
    qreal speed;
    int lastWaypoint;

};

#endif // CONTROLLER_H
