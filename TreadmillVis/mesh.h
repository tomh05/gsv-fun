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


#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <opengl/glu.h>

static const double Pi = 3.14159265358979323846264338327950288419717;

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

    QList<QList<QVector3D> >* polygons;
    QList<QList<QVector3D> >* textures;

    QPoint nextFrom(QPoint p, QPoint b);

    QString mode = "planes";
    //QString mode = "planes";
    //tesselation to deal with concave meshes;
    GLUtesselator *tess;
    //void tcbBegin(GLenum prim);
    //void tcbVertex(GLvoid *data);
    //void tcbEnd();
};
#endif // MESH_H
