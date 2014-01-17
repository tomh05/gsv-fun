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
    vertex = (GLdouble *) malloc(7*sizeof(GLdouble));
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

Mesh::Mesh(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
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


    /*
     *OpenGL shaders. not used now.
    shader = new QGLShader(QGLShader::Fragment,this);
    qDebug()<<"shaer compile"<<shader->compileSourceCode("uniform mediump vec4 color;\n"
                              "void main(void)\n"
                              "{\n"
                              "     gl_FragColor = color;\n"
                              "} \n");
    qDebug()<<"log"<<shader->log();

    qDebug()<<"stat"<<glGetString(GL_SHADING_LANGUAGE_VERSION);
    bool result = shader->compileSourceFile("/home/tomh/Projects/treadmill/gsv-fun/TreadmillVis/shader.fsh");
    if (!result) {
        qDebug()<<"shader compilation failed: "<<shader->log();
    }

    shaderProgram = new QGLShaderProgram(this);
    shaderProgram->addShaderFromSourceCode(QGLShader::Vertex,
                                           "varying vec4 worldCoord;\n"
                                           "void main(void)\n"
                                           "{\n"
                                           "  worldCoord = gl_ModelViewMatrix * gl_Vertex ;\n"
                                           "} \n");
    // fragment shader:
    // get x,y,z from varying vec4 worldCoord
    // apply sin, cos to get tex coords
    // gl_FragColor (or something else?) = texture(x_img,y_img)
//http://stackoverflow.com/questions/12414708/correct-glsl-affine-texture-mapping
//http://www.ozone3d.net/tutorials/glsl_texturing_p02.php
   // http://qt-project.org/doc/qt-4.8/qglshaderprogram.html
    // TODO: apply the actual shader, get it working
    //http://stackoverflow.com/questions/16563234/opengl-texture-mapping-in-vertex-or-fragment-shader
   qDebug()<<"compile" << shaderProgram->addShaderFromSourceCode(QGLShader::Fragment,
                                           "varying vec4 worldCoord;\n"
                                           "uniform sampler2D _MainTex;\n"
                                           "void main(void)\n"
                                           "{\n"
                                           "     vec2 position;\n"
                                           "     position = vec2(0,0);\n"
                                           "     gl_FragColor = texture2D(_MainTex, vec2 position);\n"
                                           "} \n");
   qDebug()<<shaderProgram->log();
    qDebug()<< "link "<< shaderProgram->link();
    qDebug()<< "link "<< shaderProgram->log();
    qDebug()<< "bind "<< shaderProgram->bind();

*/


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

/*
 * contour tracing - deprecated.
QPoint Mesh::nextFrom(QPoint p, QPoint b) {
    QPoint r;

    QPoint d = b-p;
    if      (d == QPoint(-1,-1)) r = p + QPoint( 0,-1);
    else if (d == QPoint( 0,-1)) r = p + QPoint( 1,-1);
    else if (d == QPoint( 1,-1)) r = p + QPoint( 1, 0);
    else if (d == QPoint( 1, 0)) r = p + QPoint( 1, 1);
    else if (d == QPoint( 1, 1)) r = p + QPoint( 0, 1);
    else if (d == QPoint( 0, 1)) r = p + QPoint(-1, 1);
    else if (d == QPoint(-1, 1)) r = p + QPoint(-1, 0);
    else if (d == QPoint(-1, 0)) r = p + QPoint(-1,-1);
    else {
        qDebug()<<"ERROR!!!";
    }
    return r;
}
*/

// build a mesh composed of 512x256 quads, each corresponding to a plane defined in the GSV depth map.
int Mesh::buildMesh() {
    qDebug()<<"building mesh";
    int xCells = 512;
    int yCells = 256;


    //textures = new QList<QList<QVector3D> >();

    polygons = new QList<QList<QVector3D> >();

    for (int i = 1;i< distances->length();i++){ // for each plane (depth map intensity)

        // create binary image, white = current plane level
        cv::Mat binMat = cv::Mat(256,512,CV_8U,0.0);
        for (int y = 1;y<256;y++) {
            for (int x = 1;x<512;x++) {
                if (indices->at(x+y*xCells) == i || indices->at(x-1+y*xCells) ==i || indices->at(x+(y-1)*xCells) == i )  {
                //if (indices->at(x+y*xCells) == i )  {
                    binMat.at<char>(x+y*xCells) = 255;
                }
            }
        }

        // get contours of image
        cv::vector<cv::vector<cv::Point> > contours;
        cv::findContours(binMat,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

       /*
        for (int ci=0;ci<contours.size();ci++) {
            qDebug()<<"got a contour "<<ci;
        }
        cv::drawContours(binMat,contours,0,cv::Scalar(255,0,0));
        cv::namedWindow("window");
        cv::imshow("window",binMat);
        */

        // for each contour found, convert each contour point to 3d and append
        for (int k=0; k<contours.size(); k++) {

            //QList<QVector3D> newPoly = new QList<QVector3D>();
            QList<QVector3D> newPoly;
            QList<QVector3D> newTex;
            for (int m = 0; m < contours.at(k).size(); m++) {

                // get new polygon in 3d
                int curX = contours.at(k).at(m).x;
                int curY = contours.at(k).at(m).y;

                QVector3D viewVector = unitVectorFromPx(curX, curY, xCells , yCells );
                //int index = indices->at(curX+curY*xCells);
                int index = i;
                qreal n1 = normals->at(index).x();
                qreal n2 = normals->at(index).y();
                qreal n3 = normals->at(index).z();
                // convert from GSV to OpenGL coordinate system
                QVector3D n = QVector3D(n1,-n3,-n2);
                QMatrix4x4 normalRot;
                normalRot.rotate(-panoBearing-90,0.0,1.0,0.0);
                n = normalRot * n;

                // intersection of line from origin to quad vertex, with the plane defined by normal n and distance in distances[]
                qreal d1 = - distances->at(index) / ( QVector3D::dotProduct(viewVector, n));

                viewVector *= d1;

                qreal offset = 0.;
                qreal si =  offset + ((qreal) curX ) / ((qreal) xCells);
                if (si>1.0) si -= 1.0;
                qreal sj = ((qreal) (yCells - curY)) / ((qreal)yCells);

                qreal len = viewVector.length();

                newPoly.append(viewVector);
                newTex.append(QVector3D(si,sj,len));


            }
            polygons->append(newPoly);
            //textures->append(newTex);
        }
    }


    GLUquadricObj *sphere=NULL;
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere,GLU_FILL);
    gluQuadricTexture(sphere,true);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    sphereRenderList = glGenLists(1);
    glNewList(sphereRenderList, GL_COMPILE);
    gluSphere(sphere,200.0,20,20);
        glBegin(GL_QUADS);
    glVertex3d(0,0,4);
    glVertex3d(0,10,4);
    glVertex3d(10,10,4);
    glVertex3d(10,0,4);
    glEnd(); // these quads don't seem to render, so the sphere isn't working either

    glEndList();
    gluDeleteQuadric(sphere);


    return 0;
}


int Mesh::drawMesh()
{

    glEnable(GL_TEXTURE_2D);
    texID = bindTexture(*panImg);
    glColor4f(1,1,1,1);

    for (int i=0; i<polygons->length();i++) {

        gluTessProperty(tess,GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_POSITIVE);
        gluTessBeginPolygon (tess, NULL);
        gluTessBeginContour (tess);
        GLdouble vert[7*polygons->at(i).length()];

        for (int j=0; j<polygons->at(i).length();j++) {
            // Draw Quads
            vert[0 + j*7] =  polygons->at(i).at(j).x() + origin.x();
            vert[1 + j*7] =  polygons->at(i).at(j).y() + origin.y();
            vert[2 + j*7] =  polygons->at(i).at(j).z() + origin.z();
            /*
            vert[3 + j*7] =  textures->at(i).at(j).x();
            vert[4 + j*7] =  textures->at(i).at(j).y();
            vert[5 + j*7] =  0;
            vert[6 + j*7] =  textures->at(i).at(j).z();
            */
            gluTessVertex (tess, vert +j*7 , vert + j*7);
        }

        gluTessEndContour(tess);
        gluTessEndPolygon(tess);
    }

    qDebug()<<"calling"<<sphereRenderList;
    glCallList(sphereRenderList);


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

