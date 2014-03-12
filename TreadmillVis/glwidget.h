#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include "mesh.h"
#include "controller.h"

class Controller;

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget(Controller* _controller, QList<Mesh *> *_meshes, QWidget *parent = 0);
    void setCamera(QVector3D position, QVector3D lookAt);
    void setWaypoint(int l) { lastWaypoint = l; }

    void initializeGL();
    void paintGL();
    void renderScene();

signals:
    
public slots:
    void animate();
    //void paintEvent(QPaintEvent *event);

    void keyPressEvent(QKeyEvent *event);
private:
    //int elapsed;
    QList<Mesh*> * meshes;
    qreal theta;

    Controller * controller;
    QVector3D camPos, camAt;
    int lastWaypoint;



    // shaders
    CGcontext context;
    CGprogram vertexProgram;
    CGprogram fragmentProgram;
    CGparameter modelViewMatrix;
    CGparameter originParam;
    CGparameter bearingParam;
    void initFragmentShader(QString fileName);

    GLuint sphereRenderList;

    qreal cameraYOffset;
    int   meshOffset;


};

#endif
