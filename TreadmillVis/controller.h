#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include <window.h>
#include <glwidget.h>
#include <mesh.h>
#include <route.h>

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = 0);
    void run();

signals:

public slots:

private:
    GLWidget * glWidget;
    QList<Mesh*> * meshes;

};

#endif // CONTROLLER_H
