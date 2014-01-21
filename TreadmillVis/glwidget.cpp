#include "glwidget.h"
#include <QtGui>
#if defined(__APPLE__) || defined(MACOSX)
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

GLWidget::GLWidget(Controller * _controller, QList<Mesh*>* _meshes, QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    theta = 0;
     meshes = _meshes;
     controller = _controller;
    setFixedSize(1280, 720);
    camPos = QVector3D(0,0,0);
    camAt = QVector3D(1,0,0);
    lastWaypoint = 0;
    sphereRenderList = 0;

    cameraYOffset = 0;
    meshOffset = 1;

}

void GLWidget::setCamera(QVector3D position, QVector3D lookAt)
{
    camPos= position;
    camAt = lookAt;

}

void GLWidget::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    initFragmentShader("/home/tomh/Projects/treadmill/gsv-fun/TreadmillVis/shader.cg");

}


void resizeGL(int width, int height) {
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

}


void GLWidget::initFragmentShader(QString fileName)
{
    context = cgCreateContext();
    fragmentProgram = cgCreateProgramFromFile(context, CG_SOURCE,
                                              fileName.toLocal8Bit(),
                                              cgGLGetLatestProfile(CG_GL_FRAGMENT),
                                              "frag", NULL);
    qDebug() << "Fragment: "  << cgGetErrorString(cgGetError());
    qDebug() <<  cgGetLastListing(context);
    vertexProgram = cgCreateProgramFromFile(context, CG_SOURCE,
                                              fileName.toLocal8Bit(),
                                              cgGLGetLatestProfile(CG_GL_VERTEX),
                                              "vert", NULL);

    qDebug() << "Vertex: "  << cgGetErrorString(cgGetError());
    qDebug() <<  cgGetLastListing(context);

    cgGLLoadProgram(fragmentProgram);
    cgGLLoadProgram(vertexProgram);
    modelViewMatrix = cgGetNamedParameter(vertexProgram,"matModelView");
    originParam = cgGetNamedParameter(fragmentProgram,"origin");
    bearingParam = cgGetNamedParameter(fragmentProgram,"bearing");

    qDebug() << cgGetErrorString(cgGetError());
}

void GLWidget::animate()
{
    //elapsed = (elapsed + qobject_cast<QTimer*>(sender())->interval()) % 1000;
    repaint();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5,0.5,0.5,0.0);

    // Camera view
    //glViewport(400,300,400,300);
    glViewport(0,0,1280,720);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //gluPerspective(400,1.5,0.1, 1000);
    //gluPerspective(80,1.5,0.1, 1000);
    gluPerspective(50,1.5,0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear (GL_DEPTH_BUFFER_BIT);
    //gluLookAt(1,0,0,0,0,0,0,1,0);
    //gluLookAt(1,30,0,0,0,0,0,1,0);
    //if (false) {
    //gluLookAt(1 + qCos(0.1*theta),31.0,20*qSin(theta*.1),
    //          0 + qCos(0.1*theta),-1.0,20*qSin(theta*.1),
    //          0.0,1.0,0.0);
    //} else {
    //    gluLookAt(- 30* qSin(theta),0.0,8*qSin(theta),
    //           1000,0.0,0.0,
    //                        0.0,1.0,0.0);

    gluLookAt(camPos.x(), cameraYOffset + camPos.y(), camPos.z(),
              camAt.x() , camAt.y() , camAt.z() ,
              0.0       , 1.0       , 0.0       );



    renderScene();

    theta += 0.03;


}





void GLWidget::renderScene()
{
    cgGLEnableProfile( cgGLGetLatestProfile(CG_GL_VERTEX));
    cgGLBindProgram(vertexProgram);
    cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
        cgGLEnableProfile( cgGLGetLatestProfile(CG_GL_FRAGMENT));
    cgGLBindProgram(fragmentProgram);

    if (!sphereRenderList) {
        qDebug()<<"rendering sphere";
    GLUquadricObj *sphere=NULL;
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere,GLU_FILL);
    //gluQuadricDrawStyle(sphere,GLU_LINE);
    //gluQuadricTexture(sphere,true);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    gluQuadricOrientation(sphere,GLU_INSIDE);
    sphereRenderList = glGenLists(1);
    glNewList(sphereRenderList, GL_COMPILE);
    gluSphere(sphere,100.0,20,20);

    glEndList();
    gluDeleteQuadric(sphere);
    }


    QList<Mesh*>::iterator i;
    //for (i = meshes->begin(); i != meshes->end(); i++) {
    i = meshes->begin();
    i+=lastWaypoint + meshOffset;

    bindTexture(*(*i)->getPanImg());
    QVector3D meshOrigin = (*i)->getOrigin();
    cgGLSetParameter3f(originParam,meshOrigin.x(), meshOrigin.y() ,meshOrigin.z() );
    cgGLSetParameter1f(bearingParam,(*i)->getPanoBearing() );

    (*i)->drawMesh();


    glPushMatrix();
    glTranslatef((*i)->getOrigin().x(), (*i)->getOrigin().y(), (*i)->getOrigin().z());
    cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
    //cgGLSetParameter3f(originParam,-(*i)->getOrigin().x(), -(*i)->getOrigin().y(), -(*i)->getOrigin().z());
    //cgGLSetParameter3f(originParam,meshOrigin.x(), meshOrigin.y() ,meshOrigin.z() );
    cgGLSetParameter3f(originParam, 0,0,0);
    cgGLSetParameter1f(bearingParam,(*i)->getPanoBearing() );

    glCallList(sphereRenderList);

    // draw floor
    if (true) {
    glBegin(GL_POLYGON);
    qreal height = (*i)->getCameraHeight();
    glVertex3f(200,-height,200);
    glVertex3f(200,-height,-200);
    glVertex3f(-200,-height,-200);
    glVertex3f(-200,-height,200);
    glEnd();
    }

    glPopMatrix();
    cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

    cgGLDisableProfile(cgGLGetLatestProfile(CG_GL_VERTEX));
    cgGLUnbindProgram(cgGLGetLatestProfile(CG_GL_VERTEX));
    cgGLDisableProfile(cgGLGetLatestProfile(CG_GL_FRAGMENT));
    cgGLUnbindProgram(cgGLGetLatestProfile(CG_GL_FRAGMENT));

}

void GLWidget::keyPressEvent(QKeyEvent * event) {
 if (event->key() == Qt::Key_BracketRight) {
      cameraYOffset += 1.0;
 }
 else if (event->key() == Qt::Key_BracketLeft) {
      cameraYOffset -= 1.0;
 }
  else if (event->key() == Qt::Key_P) {
      cameraYOffset = 0.0;
 }
  else if (event->key() == Qt::Key_J) {
      meshOffset -= 1;
 }
  else if (event->key() == Qt::Key_K) {
      meshOffset += 1;
 }
  else if (event->key() == Qt::Key_L) {
      meshOffset = 0;
 }
   else if (event->key() == Qt::Key_Z) {
     controller->setSpeed( controller->getSpeed() - 0.1);
 }
   else if (event->key() == Qt::Key_X) {
     controller->setSpeed(0);
 }
   else if (event->key() == Qt::Key_C) {
     controller->setSpeed( controller->getSpeed() + 0.1);
 }


}
