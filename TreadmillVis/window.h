#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include "glwidget.h"
#include "imgwidget.h"
#include "mesh.h"


class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = 0);
    
signals:
    
public slots:

private:
    GLWidget *openGL;
    Mesh *mesh;
    
};

#endif // WINDOW_H
