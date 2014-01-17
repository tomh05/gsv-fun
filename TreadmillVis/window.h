#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include "glwidget.h"
#include "mesh.h"


class Window : public QWidget
{
    Q_OBJECT
public:
    explicit Window(GLWidget *_glWidget, QWidget *parent = 0);
    
signals:
    
public slots:

private:
    GLWidget *glWidget;
    //Mesh *mesh;
    
};

#endif // WINDOW_H
