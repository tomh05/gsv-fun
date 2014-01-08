#ifndef MESH_H
#define MESH_H

#include <QVector3D>
#include <QList>
#include <QDebug>
#include <QTextStream>
//#include <QRegularExpression>
#include <QtOpenGL/QGLWidget>
#include <QtCore/qmath.h>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class Mesh : public QGLWidget
{
    Q_OBJECT
public:
    explicit Mesh(QWidget *parent = 0);
    int readDepthFiles();
    int buildMesh();
    int drawMesh();
signals:

public slots:



private:
    QList<int> * indices;
    QList<QVector3D> * normals;
    QList<double> * distances;


    QVector3D unitVectorFromPx(int x, int y, int w, int h);
    GLuint texID;
    QImage* panImg;
    QList<QVector3D>* meshVertices;
    QList<QVector3D>* meshTexCoords;

};

#endif // MESH_H
