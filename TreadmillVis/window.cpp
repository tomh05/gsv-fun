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


    openGL = new GLWidget(mesh,this);

    QSlider *frameSlider = new QSlider(Qt::Horizontal);
    frameSlider->setMinimum(1);
    frameSlider->setMaximum(endFrame - startFrame);
    frameSlider->setTickPosition(QSlider::TicksBelow);
    frameSlider->setTickInterval(1);
    //connect(frameSlider,SIGNAL(sliderMoved(int)),
    //        openGL, SLOT(changeFrame(int)));

    QLineEdit *frameEdit = new QLineEdit();
    //frameEdit->textEdited();

    QGridLayout *layout = new QGridLayout;
    //layout->addWidget(native, 0, 0);
    layout->addWidget(frameSlider, 0, 0);
    layout->addWidget(frameEdit, 0, 1);
    layout->addWidget(openGL, 1, 0,1,2);
    //layout->addWidget(nativeLabel, 1, 0);
    //layout->addWidget(openGLLabel, 1, 1);
    setLayout(layout);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), openGL, SLOT(animate()));
    timer->start(50);

    setWindowTitle(tr("Visualisation"));



}
