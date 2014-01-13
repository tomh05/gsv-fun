#QT       += core opengl
QT += widgets
QT += opengl

SOURCES += \
    main.cpp \
    window.cpp \
    glwidget.cpp \
    imgwidget.cpp \
    mesh.cpp

#mac hack
unix:!macx {
LIBS += -lglut  \
        -lGLU
}

HEADERS += \
    window.h \
    glwidget.h \
    imgwidget.h \
    mesh.h

INCLUDEPATH += ../shared

macx: LIBS += -framework GLUT

INCLUDEPATH += /opt/local/include
LIBS += -L/opt/local/lib
LIBS += -lopencv_calib3d \
-lopencv_contrib \
-lopencv_core \
-lopencv_features2d \
-lopencv_flann \
-lopencv_gpu \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_legacy \
-lopencv_ml \
-lopencv_objdetect \
-lopencv_video
