#ifndef MESHPOLYGON_H
#define MESHPOLYGON_H

#include <QObject>
#include <QVector3D>

class MeshPolygon
{
public:
    explicit MeshPolygon();

    QList<QVector3D> getPoints() const;
    //int getNumPoints() { return points.length(); }

    void setPoints(const QList<QVector3D> &value);
    void appendPoint(QVector3D p) { points.append(p); }

    QVector3D getNormal() const;
    void setNormal(const QVector3D &value);

    QVector3D getCenter() const;
    void setCenter(const QVector3D &value);

    qreal getSize() const;
    void setSize(const qreal &value);

signals:

public slots:


private:
    QList<QVector3D> points;
    QVector3D normal;
    QVector3D center;
    qreal size;
};

#endif // MESHPOLYGON_H
