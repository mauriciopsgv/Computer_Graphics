#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <QVector3D>

class BezierCurve
{
public:
    BezierCurve(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3);

    std::vector<QVector3D> getCurvePoints();

    std::vector<QVector3D> getControlPoints();
    void setControlPoint(int num, QVector3D point);

private:

    float step;

    std::vector<QVector3D> controlPoints;

    std::vector<QVector3D> curvePoints;

    QVector3D* calculatePoint(float t);
    void UpdateCurvePoints();
};

#endif // BEZIERCURVE_H
