#include "meshpolygon.h"

MeshPolygon::MeshPolygon()

{
}
QList<QVector3D> MeshPolygon::getPoints() const
{
    return points;
}

void MeshPolygon::setPoints(const QList<QVector3D> &value)
{
    points = value;
}
QVector3D MeshPolygon::getNormal() const
{
    return normal;
}

void MeshPolygon::setNormal(const QVector3D &value)
{
    normal = value;
}
QVector3D MeshPolygon::getCenter() const
{
    return center;
}

void MeshPolygon::setCenter(const QVector3D &value)
{
    center = value;
}
qreal MeshPolygon::getSize() const
{
    return size;
}

void MeshPolygon::setSize(const qreal &value)
{
    size = value;
}




