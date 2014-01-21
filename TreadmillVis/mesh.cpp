#include "mesh.h"

void tcbBegin (GLenum data) {
    glBegin(data);
}

void tcbVertex (GLvoid *data) {

    GLdouble * vert;
    vert = (GLdouble *) data;
    //glTexCoord4dv((GLdouble *) (vert+3));
    //qDebug()<<"making vertex";
    /*
    GLdouble s,t,r,q;
    qreal len = QVector3D(*vert,*(vert+1),*(vert+2)).length();
    s= len * *(vert+3);
    t= len * *(vert+4);
    r = 0.0;
    q = len;
    glTexCoord4d(s,t,r,q);
    */
    //glColor4f(0.01 * (qrand() % 100),0.01 * (qrand() % 100),0.01 * (qrand() % 100),1);
    glVertex3dv((GLdouble *)vert);
}

void tcbEnd () {
    //qDebug()<<"ending";
    glEnd();

}
void tcbCombine( GLdouble coords[3], GLdouble *vertexData[4],GLfloat weight[4], GLdouble **dataOut ) {

    GLdouble *vertex; // new vertex
    vertex = (GLdouble *) malloc(3*sizeof(GLdouble));
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    /*
    GLdouble s,t,r,q;
    qreal len = QVector3D(coords[0],coords[1],coords[2]).length();
    s = 0.0;
    t = 0.0;
    for (int i=0; i<4; i++) {
        if (weight[i]>0) {
            s += weight[i] * vertexData[i][3];
            t += weight[i] * vertexData[i][4];
        }
    }
    r = 0.0;
    q = len;

    vertex[3] = s;
    vertex[4] = t;
    vertex[5] = r;
    vertex[6] = q;
    */
    *dataOut = vertex;

}

void tcbEdge (GLvoid *data) {
    return; // force GL_Triangles
}

Mesh::Mesh(QObject *parent) :
    QObject( parent)
{
    isLoaded = false;
    indices = new QList<int>();
    normals = new QList<QVector3D>();
    distances = new QList<double>();
    origin = QVector3D(0,0,0);
    panoBearing = 0.0;

    tess = gluNewTess();
    gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid (*)()) &tcbBegin);
    gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (*)()) &tcbVertex);
    gluTessCallback(tess, GLU_TESS_END, (GLvoid (*)()) &tcbEnd);
    gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (*)()) &tcbCombine);
    gluTessCallback(tess, GLU_TESS_EDGE_FLAG_DATA, (GLvoid (*)()) &tcbEdge);


}




int Mesh::loadFiles()
{
    qDebug()<<"reading files";

    panImg = new QImage(filePath + ".jpg");

    /*#ifdef __linux__
    iFile.setFileName( "/home/tomh/Projects/treadmill/gsv-fun/indices.json");
#else
    iFile.setFileName("/Users/tom/Programming/BBC/Treadmill/indices.json");
#endif
*/

    QJsonArray iA = loadJson(filePath + "_indices.json");
    QJsonArray::iterator i;
    for (i = iA.begin(); i != iA.end(); ++i) {
        indices->append((*i).toDouble());
    }

    QJsonArray p = loadJson(filePath+"_planes.json");
    QJsonArray::iterator j;
    for (j = p.begin(); j != p.end(); j++) {
        distances->append( (*j).toObject().value("d").toDouble() );
        normals->append(  QVector3D( (*j).toObject().value("nx").toDouble(),
                                     (*j).toObject().value("ny").toDouble(),
                                     (*j).toObject().value("nz").toDouble() ));
    }

    qDebug()<<"read files";

    isLoaded = true;
    return 0;
}

QJsonArray Mesh::loadJson(QString path) {
    QFile file;
    QString contents;
    file.setFileName(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    contents = file.readAll();
    file.close();

    QJsonDocument d = QJsonDocument::fromJson(contents.toUtf8());

    if (!d.isArray()) {
        qDebug()<<"Problem with JSON file at "+ path + ": expected it to contain an array";
        return QJsonArray();
    }
    return d.array();
}

// Image coordinates to position on unit sphere
QVector3D Mesh::unitVectorFromPx(int x, int y, int w, int h)
{
    // projection: x=0 is 0 degrees longitude, x=w is 360 degrees longitude
    //             y=0 is 180 lat, y= h is -180 lat
    qreal alpha = Pi * (qreal) (y) / (qreal) h;
    qreal beta = 2 * Pi* (qreal) (x) / (qreal) w + (Pi * panoBearing / 180.0);
    //qreal beta = 2 * Pi* (qreal) (x) / (qreal) w;


    qreal vy = qCos( alpha);
    qreal yScale = qSin(alpha);

    qreal vx = qCos(beta) * yScale;
    qreal vz = qSin(beta) * yScale;

    return QVector3D(vx,vy,vz).normalized();
}


// build a mesh composed of 512x256 quads, each corresponding to a plane defined in the GSV depth map.
int Mesh::buildMesh() {
    qDebug()<<"building mesh";
    int xCells = 512;
    int yCells = 256;


    //textures = new QList<QList<QVector3D> >();

    polygons = new QList<MeshPolygon>();

    for (int i = 1;i< distances->length();i++){ // for each plane (depth map intensity)

        if (QVector3D::dotProduct(normals->at(i),QVector3D(0,0,-1)) > 0.9) {
           continue;
        }


        // create binary image, white = current plane level
        int binWidth = xCells +3 ;
        int binHeight = yCells +1;
        cv::Mat binMat = cv::Mat(binHeight,binWidth,CV_8U,0.0);

        for (int y = 0;y<binHeight;y++) {
            for (int x = 0;x<binWidth;x++) {
                //qDebug()<< "x"<<x<<"y"<<y;
                if ((x<(xCells-1) && y<(yCells-1) && indices->at(x+y*xCells) ==i )
                        || (x>0 && y<(yCells-1) && indices->at(x-1+y*xCells) ==i )
                        || (x<(xCells-1) && y>0 && indices->at(x+(y-1)*xCells) ==i ) ) {
                    binMat.at<char>(x+y*binWidth) = 255;
                }

            }
        }
        /*if (i == 1) {
        cv::namedWindow("window2");
        cv::imshow("window2",binMat);
        }
        */

        // get contours of image
        cv::vector<cv::vector<cv::Point> > contours;
        //cv::findContours(binMat,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);
        cv::findContours(binMat,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

        /*
        if (i == 1) {
        for (int ci=0;ci<contours.size();ci++) {
            qDebug()<<"got a contour "<<ci;
        }
        cv::drawContours(binMat,contours,0,cv::Scalar(255,0,0));
        cv::namedWindow("window");
        cv::imshow("window",binMat);
        }
        */

        // compute normal of plane
        int index = i;
        qreal n1 = normals->at(index).x();
        qreal n2 = normals->at(index).y();
        qreal n3 = normals->at(index).z();
        // convert from GSV to OpenGL coordinate system
        QVector3D n = QVector3D(n1,-n3,-n2);
        QMatrix4x4 normalRot;
        normalRot.rotate(-panoBearing-90,0.0,1.0,0.0);
        n = normalRot * n;

        // for each contour found, convert each contour point to 3d and append
        for (int k=0; k<contours.size(); k++) {

            //QList<QVector3D> newPoly = new QList<QVector3D>();
            MeshPolygon newPoly;
            //QList<QVector3D> newPoly;
            QList<QVector3D> newTex;
            for (int m = 0; m < contours.at(k).size(); m++) {

                // get new polygon in 3d
                int curX = contours.at(k).at(m).x;
                int curY = contours.at(k).at(m).y;

                QVector3D viewVector = unitVectorFromPx(curX, curY, xCells , yCells );
                //int index = indices->at(curX+curY*xCells);

                // intersection of line from origin to quad vertex, with the plane defined by normal n and distance in distances[]
                qreal d1 = - distances->at(index) / ( QVector3D::dotProduct(viewVector, n));

                if (d1<0) continue;

                viewVector *= d1;

                //qreal offset = 0.;
                //qreal si =  offset + ((qreal) curX ) / ((qreal) xCells);
                //if (si>1.0) si -= 1.0;
                //qreal sj = ((qreal) (yCells - curY)) / ((qreal) yCells);

                //qreal len = viewVector.length();

                // extend low vertecies down to the ground
                if (viewVector.y()<0) {
                    viewVector.setY(-10);
                }

                newPoly.appendPoint(viewVector);


            }
            newPoly.setNormal(n);

            //cv::Moments mom = cv::moments(contours.at(k));
            //float centX = mom.m10 / mom.m00;
            //float centY = mom.m01 / mom.m00;
            //QVector3D centerVector = unitVectorFromPx(centX,centY,xCells,yCells);
            //centerVector *= - distances->at(index) / QVector3D::dotProduct(centerVector,n) ;
            //newPoly.setCenter(centerVector);
            //newPoly.setSize();
            polygons->append(newPoly);
            //textures->append(newTex);
        }
    }



    return 0;
}


int Mesh::drawMesh()
{

    glEnable(GL_TEXTURE_2D);
    //texID = bindTexture(*panImg);
    glColor4f(1,1,1,1);

    for (int i=0; i<polygons->length();i++) {
        gluTessProperty(tess,GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_POSITIVE);
        gluTessBeginPolygon (tess, NULL);
        gluTessBeginContour (tess);

        int numPoints = polygons->at(i).getPoints().length();
        GLdouble vert[3*numPoints];

        for (int j=0; j< numPoints;j++) {
            // Draw Quads
            vert[0 + j*3] =  polygons->at(i).getPoints().at(j).x() + origin.x();
            vert[1 + j*3] =  polygons->at(i).getPoints().at(j).y() + origin.y();
            vert[2 + j*3] =  polygons->at(i).getPoints().at(j).z() + origin.z();
            gluTessVertex (tess, vert +j*3 , vert + j*3);
        }

        gluTessEndContour(tess);
        gluTessEndPolygon(tess);
    }


    return 0;

    /*
    /////////////////////////////////////////////////////////
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
*/
}

