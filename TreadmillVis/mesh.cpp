#include "mesh.h"

void tcbBegin (GLenum data) {
    //qDebug()<<"beginning";

    //glBegin(data);
    //glBegin(GL_TRIANGLE_STRIP);
    glBegin(GL_TRIANGLES);

}

void tcbVertex (GLvoid *data) {

    GLdouble * vert;
    vert = (GLdouble *) data;
    //glTexCoord4dv((GLdouble *) (vert+3));
    //qDebug()<<"making vertex";

    GLdouble s,t,r,q;
    qreal len = QVector3D(*vert,*(vert+1),*(vert+2)).length();
    s= len * *(vert+3);
    t= len * *(vert+4);
    r = 0.0;
    q = len;
    glTexCoord4d(s,t,r,q);

    //glColor4f(0.01 * (qrand() % 100),0.01 * (qrand() % 100),0.01 * (qrand() % 100),1);
    glVertex3dv((GLdouble *)vert);
}

void tcbEnd (GLvoid *data) {
    //qDebug()<<"ending";
    glEnd();

}
void tcbCombine( GLdouble coords[3], GLdouble *vertexData[4],GLfloat weight[4], GLdouble **dataOut ) {

    GLdouble *vertex; // new vertex

    vertex = (GLdouble *) malloc(7*sizeof(GLdouble));
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
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

    *dataOut = vertex;

}

void tcbEdge (GLvoid *data) {
    return; // force GL_Triangles
}

Mesh::Mesh(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    indices = new QList<int>();
    normals = new QList<QVector3D>();
    distances = new QList<double>();

    tess = gluNewTess();
    //gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid (*)()) &glBegin);
    gluTessCallback(tess, GLU_TESS_BEGIN, (GLvoid (*)()) &tcbBegin);
    //gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (*)()) &(tcbVertex));
    gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (*)()) &tcbVertex);
    //gluTessCallback(tess, GLU_TESS_VERTEX, (GLvoid (*)()) glVertex3dv);
    //gluTessCallback(tess, GLU_TESS_END, &glEnd);
    gluTessCallback(tess, GLU_TESS_END, (GLvoid (*)()) &tcbEnd);
    gluTessCallback(tess, GLU_TESS_COMBINE, (GLvoid (*)()) &tcbCombine);
    gluTessCallback(tess, GLU_TESS_EDGE_FLAG_DATA, (GLvoid (*)()) &tcbEdge);

#ifdef __linux__
    panImg = new QImage("/home/tomh/Projects/treadmill/gsv-fun/pan1.jpg");
#else
    panImg = new QImage("/Users/tom/Programming/BBC/Treadmill/pan1.jpg");
#endif

    /*
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

void Mesh::initFragmentShader(QString fileName)
{
    context = cgCreateContext();
    //for some reason, selecting the 'bestProfile' didnt work properly
    //on my linux box. So I am setting the profile manually
    bestProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
    qDebug()<<"best"<<bestProfile;

    fragmentProgram = cgCreateProgramFromFile(context, CG_SOURCE,
                                              fileName.toLocal8Bit(),
                                              bestProfile /*CG_PROFILE_ARBFP1*/,
                                              "frag", NULL);
    qDebug() << "Fragment: "  << cgGetErrorString(cgGetError());
    qDebug() <<  cgGetLastListing(context);

    bestProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
    vertexProgram = cgCreateProgramFromFile(context, CG_SOURCE,
                                              fileName.toLocal8Bit(),
                                              bestProfile,
                                              "vert", NULL);

    qDebug() << "Vertex: "  << cgGetErrorString(cgGetError());
    qDebug() <<  cgGetLastListing(context);

    cgGLLoadProgram(fragmentProgram);
    cgGLLoadProgram(vertexProgram);
    modelViewMatrix = cgGetNamedParameter(vertexProgram,"matModelView");
    qDebug() << cgGetErrorString(cgGetError());


}


int Mesh::readDepthFiles()
{
    qDebug()<<"reading depth files";
    // indices file
    QFile iFile;
    QString iContents;
#ifdef __linux__
    iFile.setFileName("/home/tomh/Projects/treadmill/gsv-fun/indices.json");
#else
    iFile.setFileName("/Users/tom/Programming/BBC/Treadmill/indices.json");
#endif
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
#ifdef __linux__
    pFile.setFileName("/home/tomh/Projects/treadmill/gsv-fun/planes.json");
#else
    pFile.setFileName("/Users/tom/Programming/BBC/Treadmill/planes.json");
#endif
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

    qDebug()<<"read depth files";
    return 0;
}

// Image coordinates to position on unit sphere
QVector3D Mesh::unitVectorFromPx(int x, int y, int w, int h)
{
    // projection: x=0 is 0 degrees longitude, x=w is 360 degrees longitude
    //             y=0 is 180 lat, y= h is -180 lat

    qreal alpha = Pi * (qreal) (y) / (qreal) h;
    qreal beta = 2 * Pi* (qreal) (x) / (qreal) w -  Pi * 0.5;

    qreal vy = qCos( alpha);
    qreal yScale = qSin(alpha);

    qreal vx = qCos(beta) * yScale;
    qreal vz = qSin(beta) * yScale;

    return QVector3D(vx,vy,vz).normalized();
}


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

// build a mesh composed of 512x256 quads, each corresponding to a plane defined in the GSV depth map.
int Mesh::buildMesh() {
    int xCells = 512;
    int yCells = 256;

    meshVertices = new QList<QVector3D>();
    meshTexCoords = new QList<QVector3D>();

    if (mode=="planes") {
    /////////////////////////
/*
    for (int i = 1;i< distances->length();i++){ // for each plane (depth map intensity)
        // find start pixel
        int xStart = 0;
        int yStart = 0;
        for (int y = 0;y<yCells-20;y++){   // xy search
            for (int x = 0;x<xCells;x++) {
                if (indices->at(x+y*xCells) ==i) {
                    xStart = x;
                    yStart = y;
                    break;
                }
            }
            if (xStart !=0) break;
        }

       // contour trace
        QPoint M[] = { QPoint(-1,-1),QPoint(0,-1),QPoint(1,-1),
                        QPoint(1,0), QPoint(1,1), QPoint(0,1),
                        QPoint(-1,1),QPoint(-1,0) };

        QPoint s = QPoint(xStart,yStart);

        qDebug()<<"start "<<s;
        QList<QPoint> B; // boundary pixels.
        B.append(s);
        QPoint p = s;
        //int b = 7; // b = left
        QPoint b;
        b = s + QPoint(-1,0);
        QPoint c;
        //c = p + M[(b+1)%7];
        c = nextFrom(p,b);
        int limit = 80;
        int test = 0;
        while (c !=s && test < limit) {
        test++;
        //qDebug()<<"indices is"<<indices->at(c.x()+c.y()*xCells) ;
            if (indices->at(c.x()+c.y()*xCells) == i) {
                //qDebug()<<"yes!";
        qDebug()<<"c is"<<c;
                B.append(c);
                //b = p;
                //b = (b+1)%7;
                p = c;
                //c = p + M[(b+1)%7];
                //c = nextFrom(p,b);
                c = b;

            } else {
                //qDebug()<<"no!";
                b = c;
                //b = (b+1)%7;
                //c = p + M[(b+1)%7];
                c = nextFrom(p,b);
            }

        }

        qDebug()<<"B long"<<B.length();
        //append start pixel to contour list

        // look
        //http://www.imageprocessingplace.com/downloads_V3/root_downloads/tutorials/contour_tracing_Abeer_George_Ghuneim/moore.html


    }
*/


    polygons = new QList<QList<QVector3D> >();
    textures = new QList<QList<QVector3D> >();
    for (int i = 1;i< distances->length();i++){ // for each plane (depth map intensity)
    //for (int i = 4;i< 5;i++){ // for each plane (depth map intensity)

        // create binary image, white = current plane level
        cv::Mat binMat = cv::Mat(256,512,CV_8U,0.0);
        for (int y = 9;y<256-9;y++) {
            for (int x = 9;x<512-9;x++) {
                if (indices->at(x+y*xCells) == i || indices->at(x-1+y*xCells) ==i || indices->at(x+(y-1)*xCells) == i )  {
                //if (indices->at(x+y*xCells) == i )  {
                    binMat.at<char>(x+y*xCells) = 255;
                }
            }
        }
                cv::namedWindow("window2");
        cv::imshow("window2",binMat);

        // get contours of image
        cv::vector<cv::vector<cv::Point> > contours;
        cv::findContours(binMat,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

        //for (int ci=0;ci<contours.size();ci++) {
        //    qDebug()<<"got a contour "<<ci;
        //}
        cv::drawContours(binMat,contours,0,cv::Scalar(255,0,0));
        cv::namedWindow("window");
        cv::imshow("window",binMat);

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
            textures->append(newTex);
        }
        //qDebug()<<"made it alive";
    }

        for (int j = 0;j<yCells-20;j++){
        for (int i = 0;i<xCells;i++) {

            if (indices->at(i+j*xCells) !=0) continue;
  // get unit vectors pointing towards quad corners
            // top left, bottom left, bottom right, top right
            QVector3D c1 = unitVectorFromPx( i    , j, xCells , yCells );
            QVector3D c2 = unitVectorFromPx( i   , (j+1), xCells , yCells );
            QVector3D c3 = unitVectorFromPx( (i+1), (j+1), xCells , yCells );
            QVector3D c4 = unitVectorFromPx( (i+1), j, xCells , yCells );
            qreal d1,d2,d3,d4;

                d1 = 100.0;
                d2 = d1;
                d3 = d1;
                d4 = d1;

            // scale quad corner unit vectors by computed distance
            c1 = d1 * c1;
            c2 = d2 * c2;
            c3 = d3 * c3;
            c4 = d4 * c4;

            // Compute texture coordinates
            qreal offset = 0.;
            //qreal si =  (offset + 1.0f - ((qreal)i ) / xCells);
            qreal si =  (offset + ((qreal)i ) / (qreal)xCells);
            if (si>1.0) si -= 1.0;
            //qreal sii =  (offset + 1.0f - ((qreal)i+1 ) / xCells);
            qreal sii =  (offset + ((qreal)i-1 ) / (qreal)xCells);
            if (sii>1.0) sii -= 1.0;
            qreal sj = ((qreal) yCells - j) / (qreal)yCells;
            qreal sjj = ((qreal) yCells - j-1) / (qreal)yCells;

            qreal l1 = c1.length();
            qreal l2 = c2.length();
            qreal l3 = c3.length();
            qreal l4 = c4.length();

            meshVertices->append(QVector3D(c1));
            meshVertices->append(QVector3D(c2));
            meshVertices->append(QVector3D(c3));
            meshVertices->append(QVector3D(c4));

            meshTexCoords->append(QVector3D(sii,l1*sjj,l1));
            meshTexCoords->append(QVector3D(sii,l2*sjj,l2));
            meshTexCoords->append(QVector3D(si,l3*sjj,l3));
            meshTexCoords->append(QVector3D(si,l4*sj,l4));

        }
        }

        //qDebug()<<"made it alive 2";
    return 0;
} else {
    /////////////////////////
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

                if (d1<0 || d2<0 || d3<0 || d4<0) continue;

            }

            // scale quad corner unit vectors by computed distance
            c1 = d1 * c1;
            c2 = d2 * c2;
            c3 = d3 * c3;
            c4 = d4 * c4;

            // Compute texture coordinates
            qreal offset = 0.;
            //qreal si =  (offset + 1.0f - ((qreal)i ) / xCells);
            qreal si =  (offset + ((qreal)i ) / (qreal)xCells);
            if (si>1.0) si -= 1.0;
            //qreal sii =  (offset + 1.0f - ((qreal)i+1 ) / xCells);
            qreal sii =  (offset + ((qreal)i-1 ) / (qreal)xCells);
            if (sii>1.0) sii -= 1.0;
            qreal sj = ((qreal) yCells - j) / (qreal)yCells;
            qreal sjj = ((qreal) yCells - j-1) / (qreal)yCells;

            qreal l1 = c1.length();
            qreal l2 = c2.length();
            qreal l3 = c3.length();
            qreal l4 = c4.length();

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
}


int Mesh::drawMesh()
{
    /*    bool result = shader->compileSourceFile("/home/tomh/Projects/treadmill/gsv-fun/TreadmillVis/shader.fsh");
    if (!result) {
        qDebug()<<"shader compilation failed: "<<shader->log();
    }
    */

    if (mode == "planes") {
      //  qDebug()<<"drawing planes";
    glEnable(GL_TEXTURE_2D);
    texID = bindTexture(*panImg);
    glColor4f(1,1,1,1);




    cgGLEnableProfile( cgGLGetLatestProfile(CG_GL_VERTEX));
    cgGLBindProgram(vertexProgram);
    cgGLSetStateMatrixParameter(modelViewMatrix, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);

    cgGLEnableProfile( cgGLGetLatestProfile(CG_GL_FRAGMENT));
    cgGLBindProgram(fragmentProgram);

    //glTexGeni(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
    //glTexGeni(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);

    for (int i=0; i<polygons->length();i++) {
        gluTessProperty(tess,GLU_TESS_WINDING_RULE,GLU_TESS_WINDING_POSITIVE);
        gluTessBeginPolygon (tess, NULL);
        gluTessBeginContour (tess);
        //glBegin(GL_POLYGON);
        GLdouble vert[7*polygons->at(i).length()];

        for (int j=0; j<polygons->at(i).length();j++) {
            // Draw Quads
            //glTexCoord4f(textures->at(i).at(j).x(),textures->at(i).at(j).y(), 0, textures->at(i).at(j).z());
            //glVertex3f(polygons->at(i).at(j).x(), polygons->at(i).at(j).y(), polygons->at(i).at(j).z());
            vert[0 + j*7] =  polygons->at(i).at(j).x();
            vert[1 + j*7] =  polygons->at(i).at(j).y();
            vert[2 + j*7] =  polygons->at(i).at(j).z();
            vert[3 + j*7] =  textures->at(i).at(j).x();
            vert[4 + j*7] =  textures->at(i).at(j).y();
            vert[5 + j*7] =  0;
            vert[6 + j*7] =  textures->at(i).at(j).z();
            //            GLdouble vert[7] = { polygons->at(i).at(j).x(),  polygons->at(i).at(j).y(),  polygons->at(i).at(j).z(),
            //                  textures->at(i).at(j).x(),  textures->at(i).at(j).y(), 0.0,
             //                 textures->at(i).at(j).z() };
            //qDebug()<<"vert"<<vert[j*7];
            //qDebug()<<"vert"<<vert[j*7+1];
            //qDebug()<<"vert"<<vert[j*7+2];
            gluTessVertex (tess, vert +j*7 , vert + j*7);
        }

        gluTessEndContour (tess);
        //gluEndPolygon (tess);
        gluTessEndPolygon(tess);
        //  glEnd();
    }


    //qDebug()<<"drawn planes";
  glBegin(GL_QUADS);
    for (int i=0; i<meshVertices->length();i++) {
        // Draw Quads
        glTexCoord4f(meshTexCoords->at(i).x(),meshTexCoords->at(i).y(), 0, meshTexCoords->at(i).z());
        glVertex3f(meshVertices->at(i).x(), meshVertices->at(i).y(), meshVertices->at(i).z());
}

    glEnd();

    //qDebug()<<"returning";
    return 0;
    } else {
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
}

