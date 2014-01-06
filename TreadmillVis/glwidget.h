#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QtOpenGL/QGLWidget>
#include "mesh.h"

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    GLWidget(Mesh* _mesh, QWidget *parent = 0);
    
    void initializeGL();
    void paintGL();
    void renderScene();

signals:
    
public slots:
    void animate();
    //void paintEvent(QPaintEvent *event);

private:
    //int elapsed;
    Mesh * mesh;
    qreal theta;
};

#endif
