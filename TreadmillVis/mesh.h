#ifndef MESH_H
#define MESH_H

#include <QVector3D>
#include <QList>
#include <QDebug>
#include <QTextStream>
#include <QtOpenGL/QGLWidget>
#include <QtCore/qmath.h>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <meshpolygon.h>


// Shaders
#include <QGLShaderProgram>



#ifdef __linux__
#include <GL/glu.h>
#else
#include <opengl/glu.h>
#endif
static const double Pi = 3.14159265358979323846264338327950288419717;

class Mesh : public QObject
{
    Q_OBJECT
public:
    explicit Mesh(QObject *parent = 0);
    int loadFiles();
    int buildMesh();
    int drawMesh();

    void setOrigin(QVector3D o) { origin = o; }
    void setPanoBearing(qreal b) { panoBearing = b; }

    QVector3D getOrigin() { return origin; }
    qreal getPanoBearing() { return panoBearing; }
    void setFilePath(QString fp) { filePath = fp; }
    bool loaded() { return isLoaded; }
    QImage * getPanImg() { return panImg; }

    qreal getCameraHeight() { return distances->at(1); };

signals:

public slots:

private:
    // GSV data
    QList<int> * indices;
    QList<QVector3D> * normals;
    QList<double> * distances;

    bool isLoaded;

    GLuint texID;
    GLuint sphereRenderList;
    QImage* panImg;

    QVector3D origin;
    qreal panoBearing;

    QString filePath;

    QList<MeshPolygon>* polygons;

    //QPoint nextFrom(QPoint p, QPoint b);
    //QList<QVector3D>* meshVertices;
    //QList<QVector3D>* meshTexCoords;
    //QList<QList<QVector3D> >* textures;

    //tesselation to deal with concave meshes;
    GLUtesselator *tess;


    QJsonArray loadJson(QString path);
    QVector3D unitVectorFromPx(int x, int y, int w, int h);
};
#endif // MESH_H
