#include <QtGui>
#include "window.h"


Window::Window(GLWidget * _glWidget, QWidget *parent) :
    QWidget(parent)
{

    //setWindowTitle(tr("Street View Explorer"));

    glWidget = _glWidget;

    glWidget->show();





    /*
    mesh = new Mesh(this);
    mesh->readFiles("/home/tomh/Projects/treadmill/gsv-fun/pan1");
    mesh->buildMesh();
    */



}
