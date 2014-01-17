#include "controller.h"
Controller::Controller(QObject *parent) :
    QObject(parent)
{

}

void Controller::run()
{
       qDebug()<<"loading routes";
    Route route;
    route.loadFromFolder("/home/tomh/Projects/treadmill/gsv-fun/googleGrabber/mancity/");

    qDebug()<<"loading meshes";
    meshes = new QList<Mesh*>();

    for (int i = 0; i < route.getNumPoints(); i++ ) {

    qDebug()<<"mesh "<<i;
        Mesh * m = new Mesh();
        m->setOrigin(route.getOrigin(i));
        m->setFilePath(route.getFilePath(i));
        m->setPanoBearing(route.getPanoBearing(i));
        m->loadFiles();
        m->buildMesh();
        meshes->append(m);
    }


    qDebug()<<"creating widget";
    glWidget = new GLWidget(meshes);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), glWidget, SLOT(animate()));
    timer->start(30);

    glWidget->show();
    //Window window(glWidget);
    //window.show();

}
