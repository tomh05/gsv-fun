#include "controller.h"
Controller::Controller(QObject *parent) :
    QObject(parent)
{

}

void Controller::run()
{
    distanceTravelled = 0.0;
    lastWaypoint = 0;
    setSpeed(0.1);

    qDebug()<<"loading routes";
    route = new Route();
    //route->loadFromFolder("/home/tomh/Projects/treadmill/gsv-fun/googleGrabber/mancity/");
    //route->loadFromFolder("/home/tomh/Projects/treadmill/gsv-fun/googleGrabber/newcastle/");
    route->loadFromFolder("/home/tomh/Projects/treadmill/gsv-fun/googleGrabber/newroute/");
    //route->loadFromFolder("/home/tomh/Projects/treadmill/gsv-fun/googleGrabber/mcuk_trafford/");

    qDebug()<<"loading meshes";
    meshes = new QList<Mesh*>();

    for (int i = 0; i < route->getNumPoints(); i++ ) {

    qDebug()<<"mesh "<<i;
        Mesh * m = new Mesh();
        m->setOrigin(route->getOrigin(i));
        m->setFilePath(route->getFilePath(i));
        m->setPanoBearing(route->getPanoBearing(i));
        m->loadFiles();
        m->buildMesh();
        meshes->append(m);
    }


    qDebug()<<"creating widget";
    glWidget = new GLWidget(this,meshes);

    qDebug()<<"created widget";

    QTimer *distTimer = new QTimer(this);
    connect(distTimer, SIGNAL(timeout()), this, SLOT(distanceTimer()));
    distTimer->start(30);


    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), glWidget, SLOT(animate()));
    timer->start(30);

    glWidget->show();
    //Window window(glWidget);
    //window.show();

}


void Controller::distanceTimer() {

    distanceTravelled += getSpeed(); // meters
    if (lastWaypoint >0 && distanceTravelled > route->getCumulativeDistance(route->getNumPoints() -2 )) {
        distanceTravelled = 0;
        lastWaypoint = 0;
    }

    // find next waypoint
    while (distanceTravelled > route->getCumulativeDistance(lastWaypoint+1)) {
        lastWaypoint ++;
    }

        // compute x,y,z
    QVector3D runnerPosition, runnerLookAt;
    qreal alongLine = distanceTravelled - route->getCumulativeDistance(lastWaypoint);
    alongLine /= route->getCumulativeDistance(lastWaypoint + 1) - route->getCumulativeDistance(lastWaypoint);

    runnerPosition = alongLine * route->getOrigin(lastWaypoint + 1) + (1-alongLine) * route->getOrigin(lastWaypoint);
    runnerLookAt = alongLine * route->getOrigin(lastWaypoint + 2) + (1-alongLine) * route->getOrigin(lastWaypoint+1);


        // set camera

    glWidget->setCamera(runnerPosition, runnerLookAt);
    glWidget->setWaypoint(lastWaypoint);


}
