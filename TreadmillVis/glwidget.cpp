#include "glwidget.h"
#include <QtGui>
#if defined(__APPLE__) || defined(MACOSX)
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

GLWidget::GLWidget(Mesh *_mesh, QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    theta = 0;
     mesh = _mesh;
    //elapsed = 0;
    setFixedSize(800, 600);
    //setAutoFillBackground(false);



}

void GLWidget::initializeGL()
{
    //glFrustrum
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
}


void resizeGL(int width, int height) {
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);


    //glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
}

void GLWidget::animate()
{
    //elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 1000;
    repaint();
}

void GLWidget::paintGL()
{
    //qDebug()<<"paint";

    //QPainter painter;
    //painter.begin(this);
    //painter.setRenderHint(QPainter::Antialiasing);
    //helper->paint(&painter, event, elapsed);
    //painter.end();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(250,250,250,0);

    // Left: Plan View
    glViewport(0,0,400,600);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float zoom = 20.0;
    glOrtho(-400/zoom,400/zoom,-600/zoom,600/zoom,-100,100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear (GL_DEPTH_BUFFER_BIT);
    gluLookAt(0.0,1.0,0.0,
              0.0,0.0,0.0,
              -1.0,0.0,0.0);
    renderScene();

    // Top right: Camera view
    glViewport(400,300,400,300);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(400,1.5,0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear (GL_DEPTH_BUFFER_BIT);
    gluLookAt(100*qCos(theta),100,100.0*qSin(theta),
              0.0,0.0,0.0,
              0.0,1.0,0.0);
    renderScene();

    // Bottom right: Moveable view
    glViewport(400,0,400,300);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(10,1.5,0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glClear (GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0,0.0,0.0,
              qCos(theta),0.0,qSin(theta),
              0.0,1.0,0.0);
    renderScene();

    theta += 0.2;


}


void GLWidget::renderScene()
{
    mesh->drawMesh();

     glColor3f(0,0.5,0);
    glBegin(GL_LINES);
    for (int i = -10; i <= 10; i++) {
      //  for (int j = -10; j <= 10, j++) {
            glVertex3f(-10,0,i);
            glVertex3f(10,0,i);
            glVertex3f(i,0,10);
            glVertex3f(i,0,-10);
    //}
    }

    glEnd();
/*

    return;

    //draw pitch
    glColor3f(0,0.5,0);
    glBegin(GL_POLYGON);
    float halfLength = 50;
    float halfWidth = 35;
    glVertex3f(-halfLength,0,-halfWidth);
    glVertex3f(-halfLength,0,halfWidth);
    glVertex3f(halfLength,0,halfWidth);
    glVertex3f(halfLength,0,-halfWidth);
    glEnd();
    //draw a shape
    glPushMatrix();
    glTranslatef(10,10,10);
    //glRotatef(90,1,0,0);
    glScalef(10,10,10);
    glColor3f(1,0,0);
    glBegin(GL_POLYGON);
    glVertex3f(-1,0,-1);
    glVertex3f(-1,0,1);
    glVertex3f(1,0,1);
    glVertex3f(1,0,-1);
    glEnd();
    glPopMatrix();
    */


}
