#include "mesh.h"

Mesh::Mesh(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    indices = new QList<int>();
    normals = new QList<QVector3D>();
    distances = new QList<double>();

    //panImg = new QImage("/Users/tom/Programming/BBC/Treadmill/pan1.jpg");
    panImg = new QImage("/home/tomh/Projects/treadmill/gsv-fun/pan1.jpg");
}

int Mesh::readDepthFiles()
{
    // indices file
    QFile iFile;
    QString iContents;
    //iFile.setFileName("/Users/tom/Programming/BBC/Treadmill/indices.json");
    iFile.setFileName("/home/tomh/Projects/treadmill/gsv-fun/indices.json");
    iFile.open(QIODevice::ReadOnly | QIODevice::Text);
    iContents = iFile.readAll();
    iFile.close();

    QJsonDocument iD = QJsonDocument::fromJson(iContents.toUtf8());
    if (!iD.isArray()) {
        qDebug()<<"error: expected array in index file";
        return -1;
    }
    QJsonArray iA = iD.array();

    QJsonArray::iterator i;
    for (i = iA.begin(); i != iA.end(); ++i) {
        indices->append((*i).toDouble());
    }



    // planes file
    QFile pFile;
    QString pContents;
    //pFile.setFileName("/Users/tom/Programming/BBC/Treadmill/planes.json");
    pFile.setFileName("/home/tomh/Projects/treadmill/gsv-fun/planes.json");
    pFile.open(QIODevice::ReadOnly | QIODevice::Text);
    pContents = pFile.readAll();
    pFile.close();

    QJsonDocument pD = QJsonDocument::fromJson(pContents.toUtf8());

    if (!pD.isArray()) {
        qDebug()<<"error: expected array in planes file";
        return -1;
    }


    QJsonArray a = pD.array();


    QJsonArray::iterator j;
    for (j = a.begin(); j != a.end(); j++) {
        distances->append( (*j).toObject().value("d").toDouble() );
        //distances->append(1.0);

        //normals->append( QVector3D(1,0,0));

        normals->append(  QVector3D( (*j).toObject().value("nx").toDouble(),
                                     (*j).toObject().value("ny").toDouble(),
                                     (*j).toObject().value("nz").toDouble() ));

    }

    return 0;
}

// Image coordinates to position on unit sphere
QVector3D Mesh::unitVectorFromPx(int x, int y, int w, int h)
{
    // projection: x=0 is 0 degrees longitude, x=w is 360 degrees longitude
    //             y=0 is 180 lat, y= h is -180 lat

    qreal alpha = 3.14159265359 * (qreal) (y) / (qreal) h;
    qreal beta = 2 * 3.14159265359 * (qreal) (x) / (qreal) w -  3.14159265359 * 0.5;

    qreal vy = qCos( alpha);
    qreal yScale = qSin(alpha);

    qreal vx = qCos(beta) * yScale;
    qreal vz = qSin(beta) * yScale;

    return QVector3D(vx,vy,vz).normalized();
}

// build a mesh composed of 512x256 quads, each corresponding to a plane defined in the GSV depth map.
int Mesh::buildMesh() {
    int xCells = 512;
    int yCells = 256;

    meshVertices = new QList<QVector3D>();
    meshTexCoords = new QList<QVector3D>();

    for (int j = 0;j<yCells-20;j++){
        for (int i = 0;i<xCells;i++) {

            // get unit vectors pointing towards quad corners
            // top left, bottom left, bottom right, top right
            QVector3D c1 = unitVectorFromPx( i    , j, xCells , yCells );
            QVector3D c2 = unitVectorFromPx( i   , (j+1), xCells , yCells );
            QVector3D c3 = unitVectorFromPx( (i+1), (j+1), xCells , yCells );
            QVector3D c4 = unitVectorFromPx( (i+1), j, xCells , yCells );
            qreal d1,d2,d3,d4;


            // get index for current cell
            int index = indices->at(i+j*xCells);
            if (index> distances->length()-1) index=0; // catch bad indices

            // Deal with regions that don't have a plane: just project them onto a sphere
            if (index == 0) {

                d1 = 100.0;
                d2 = d1;
                d3 = d1;
                d4 = d1;

                // For regions with a plane:
            } else {
                //index = 5;
                //qDebug()<<"i"<<i;
                //qDebug()<<" j"<<j;
                //if (index != 6 && index !=5 && index != 4) continue;
                //if (index > 58) continue;
                //if (index < 28) continue;

                qreal scale = -1.0;
                qreal n1 = normals->at(index).x();
                qreal n2 = normals->at(index).y();
                qreal n3 = normals->at(index).z();

                // convert from GSV to OpenGL coordinate system
                QVector3D n = QVector3D(n1,-n3,-n2);

                // intersection of line from origin to quad vertex, with the plane defined by normal n and distance in distances[]
                d1 = scale * distances->at(index) / ( QVector3D::dotProduct(c1, n));
                d2 = scale * distances->at(index) / ( QVector3D::dotProduct(c2, n));
                d3 = scale * distances->at(index) / ( QVector3D::dotProduct(c3, n));
                d4 = scale * distances->at(index) / ( QVector3D::dotProduct(c4, n));

                /*
                 if (d1 < 0) {
                    qDebug()<<"skipping index "<<index;
                    qDebug()<<"distances "<<d1;
                    qDebug()<<"normal "<<n;
                    qDebug()<<"c1 "<<c1;
                    continue;
                }
                */

            }

            // scale quad corner unit vectors by computed distance
            c1 = d1 * c1;
            c2 = d2 * c2;
            c3 = d3 * c3;
            c4 = d4 * c4;

            // Compute texture coordinates
            float offset = 0.;
            //float si =  (offset + 1.0f - ((float)i ) / xCells);
            float si =  (offset + ((float)i ) / xCells);
            if (si>1.0) si -= 1.0;
            //float sii =  (offset + 1.0f - ((float)i+1 ) / xCells);
            float sii =  (offset + ((float)i-1 ) / xCells);
            if (sii>1.0) sii -= 1.0;
            float sj = ((float) yCells - j) / yCells;
            float sjj = ((float) yCells - j-1) / yCells;

            float l1 = c1.length();
            float l2 = c2.length();
            float l3 = c3.length();
            float l4 = c4.length();

            meshVertices->append(QVector3D(c1));
            meshVertices->append(QVector3D(c2));
            meshVertices->append(QVector3D(c3));
            meshVertices->append(QVector3D(c4));

            meshTexCoords->append(QVector3D(l1*sii,l1*sjj,l1));
            meshTexCoords->append(QVector3D(l2*sii,l2*sjj,l2));
            meshTexCoords->append(QVector3D(l3*si,l3*sjj,l3));
            meshTexCoords->append(QVector3D(l4*si,l4*sj,l4));
        }
    }
    return 0;
}


int Mesh::drawMesh()
{
    glEnable(GL_TEXTURE_2D);
    texID = bindTexture(*panImg);
                glColor4f(1,1,1,1);


        glBegin(GL_QUADS);
    for (int i=0; i<meshVertices->length();i++)
    {
        // Draw Quads
        glTexCoord4f(meshTexCoords->at(i).x(),meshTexCoords->at(i).y(), 0, meshTexCoords->at(i).z());
        glVertex3f(meshVertices->at(i).x(), meshVertices->at(i).y(), meshVertices->at(i).z());
    }

        glEnd();

    return 0;
}
