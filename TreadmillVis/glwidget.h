#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include "mesh.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget(QList<Mesh *> *_meshes, QWidget *parent = 0);

    void initializeGL();
    void paintGL();
    void renderScene();

signals:
    
public slots:
    void animate();
    //void paintEvent(QPaintEvent *event);

private:
    //int elapsed;
    QList<Mesh*> * meshes;
    qreal theta;


    // shaders
    CGcontext context;
    CGprogram vertexProgram;
    CGprogram fragmentProgram;
    CGparameter modelViewMatrix;
    CGparameter originParam;
    CGparameter bearingParam;
    void initFragmentShader(QString fileName);

    qreal isSecond;



};

#endif
