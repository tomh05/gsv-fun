#include "mesh.h"

Mesh::Mesh(QWidget *parent) :
      QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    indices = new QList<int>();
    normals = new QList<QVector3D>();
    distances = new QList<double>();

    panImg = new QImage("/Users/tom/Programming/BBC/Treadmill/pan1.jpg");
}

int Mesh::readDepthFiles()
{
   // indices file
   QFile iFile;
   QString iContents;
   iFile.setFileName("/Users/tom/Programming/BBC/Treadmill/indices.json");
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
   pFile.setFileName("/Users/tom/Programming/BBC/Treadmill/planes.json");
   pFile.open(QIODevice::ReadOnly | QIODevice::Text);
   pContents = pFile.readAll();
   pFile.close();

   QJsonDocument pD = QJsonDocument::fromJson(pContents.toUtf8());

   if (!pD.isArray()) {
       qDebug()<<"error: expected array in planes file";
       return -1;
}


   QJsonArray a = pD.array();

   qDebug()<<a.at(1);

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


int Mesh::drawMesh() {
    int xCells = 512;
    int yCells = 256;

    for (int j = 0;j<yCells-20;j++){
        for (int i = 0;i<xCells;i++) {

            // compute corners
            // top left, bottom left, bottom right, top right
            QVector3D c1 = unitVectorFromPx( i    , j, xCells , yCells );
            QVector3D c2 = unitVectorFromPx( i   , (j+1), xCells , yCells );
            QVector3D c3 = unitVectorFromPx( (i+1), (j+1), xCells , yCells );
            QVector3D c4 = unitVectorFromPx( (i+1), j, xCells , yCells );
            /*
            qDebug()<<"i"<<i;
            qDebug()<<"j"<<j;
            qDebug()<<"l"<<indices->length();
            qDebug()<<"a---";
            */
            int index = indices->at(i+j*xCells);
            if (index>70) index=0;
            /*qDebug()<<"index"<<index;
            qDebug()<<"nl"<<normals->length();
            qDebug()<<"dl"<<distances->at(0);
            qDebug()<<"dl"<<distances->at(1);
            qDebug()<<"dl"<<distances->at(69);
            qDebug()<<"dl"<<distances->at(70);
            qDebug()<<"m---";
            */
            qreal d1,d2,d3,d4;
            index = 0;
            if (index == 0) {

                glColor4f(0.8,0.8,0.8,1);
                d1 = 100.0;
                d2 = d1;
                d3 = d1;
                d4 = d1;
            } else {
                glColor4f(1,1,1,1);
                //index = 5;
                //qDebug()<<"i"<<i;
                //qDebug()<<" j"<<j;
                //if (index != 6 && index !=5 && index != 4) continue;
                if (index > 58) continue;

                qreal scale = -1.0;
                qreal n1 = normals->at(index).x();
                qreal n2 = normals->at(index).y();
                qreal n3 = normals->at(index).z();

                QVector3D n = QVector3D(n1,n3,n2);

                d1 = scale * distances->at(index) / ( QVector3D::dotProduct(c1, n));
                if (false) {
                    qDebug()<<"distance "<<d1;
                qDebug()<<"c1 "<<c1;
                qDebug()<<"n "<<n;
                qDebug()<<"dot "<<QVector3D::dotProduct(c1,n);
                qDebug()<<"---";
                }
                d2 = scale * distances->at(index) / ( QVector3D::dotProduct(c2, n));
                d3 = scale * distances->at(index) / ( QVector3D::dotProduct(c3, n));
                d4 = scale * distances->at(index) / ( QVector3D::dotProduct(c4, n));
                /*
                qreal d1 = scale * - distances->at(index) / ( QVector3D::dotProduct(c1, normals->at(index) ));
                qreal d2 = scale * - distances->at(index) / ( QVector3D::dotProduct(c2, normals->at(index) ));
                qreal d3 = scale * - distances->at(index) / ( QVector3D::dotProduct(c3, normals->at(index) ));
                qreal d4 = scale * - distances->at(index) / ( QVector3D::dotProduct(c4, normals->at(index) ));
                */


                if (false){

                    qDebug()<<"dist"<<distances->at(index);
                    qDebug()<<"c1"<<c1;
                    qDebug()<<"n"<<normals->at(index);
                    qDebug()<<"dot"<< QVector3D::dotProduct(c1, normals->at(index));
                    qDebug()<<"d1"<<d1;
                }
                /*
            d1 = 10.0+ 0.1 * i;
            d2 = d1+1;
            d3 = d1+2;
            d4 = d1+1;
*/

            }
            c1 = d1 * c1;
            c2 = d2 * c2;
            c3 = d3 * c3;
            c4 = d4 * c4;

            glEnable(GL_TEXTURE_2D);
            texID = bindTexture(*panImg);

            //draw quad
            //draw pitch

            glBegin(GL_POLYGON);

            float imWidth = 418;
            float imHeight = 208;
            float si = ((float) xCells - i) / xCells;
            float sii = ((float) xCells - i+1) / xCells;
            float sj = ((float) yCells - j) / yCells;
            float sjj = ((float) yCells - j-1) / yCells;

            float l1 = c1.length();
            glTexCoord4f(l1*sii,l1*sj,0,l1);
            glVertex3f(c1.x(),c1.y(),c1.z());

            float l2 = c2.length();
            glTexCoord4f(l2*sii,l2*sjj,0,l2);
            glVertex3f(c2.x(),c2.y(),c2.z());

            float l3 = c3.length();
            glTexCoord4f(l3*si,l3*sjj,0,l3);
            glVertex3f(c3.x(),c3.y(),c3.z());

            float l4 = c4.length();
            glTexCoord4f(l4*si,l4*sj,0,l4);
            glVertex3f(c4.x(),c4.y(),c4.z());




            glEnd();

        }
    }
    return 0;
}
