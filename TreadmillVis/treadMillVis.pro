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
    camera.h \
    mesh.h

INCLUDEPATH += ../shared

macx: LIBS += -framework GLUT
