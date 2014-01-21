#QT       += core opengl
QT += widgets
QT += opengl
QT += serialport

SOURCES += \
    main.cpp \
    window.cpp \
    glwidget.cpp \
    mesh.cpp \
    route.cpp \
    controller.cpp \
    meshpolygon.cpp \
    serialtalker.cpp

#mac hack
unix:!macx {
LIBS += -lglut  \
        -lGLU \
         # CG shader libraries
        -lCg \
        -lCgGL
}

HEADERS += \
    window.h \
    glwidget.h \
    mesh.h \
    route.h \
    controller.h \
    meshpolygon.h \
    serialtalker.h

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

OTHER_FILES += \
    shader.cg
