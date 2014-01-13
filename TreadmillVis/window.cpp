#include <QtGui>
#include "window.h"


Window::Window(QWidget *parent) :
    QWidget(parent)
{
    int startFrame = 700;
    int endFrame = 849;
    //Widget *native = new Widget(&helper, this);
    mesh = new Mesh(this);

    mesh->readDepthFiles();
    mesh->buildMesh();


    openGL = new GLWidget(mesh,this);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(openGL, 0, 0);
    setLayout(layout);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), openGL, SLOT(animate()));
    timer->start(50);

    setWindowTitle(tr("Visualisation"));



}
