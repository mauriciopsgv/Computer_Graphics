#include "beziercurve.h"

#include <vector>


BezierCurve::BezierCurve(QVector3D p0, QVector3D p1, QVector3D p2, QVector3D p3)
{
    step = 0.01;
    controlPoints.push_back(p0);
    controlPoints.push_back(p1);
    controlPoints.push_back(p2);
    controlPoints.push_back(p3);
    UpdateCurvePoints();
}

std::vector<QVector3D> BezierCurve::getCurvePoints()
{
    return curvePoints;
}

std::vector<QVector3D> BezierCurve::getControlPoints()
{
    return controlPoints;
}

void BezierCurve::setControlPoint(int num, QVector3D point)
{
    controlPoints[num].setX(point.x());
    controlPoints[num].setY(point.y());
    controlPoints[num].setZ(point.z());
    UpdateCurvePoints();
}

QVector3D* BezierCurve::calculatePoint (float t)
{
    float x, y, z;
    x = pow(1-t,3)*controlPoints[0].x() + 3*pow(1-t,2)*t*controlPoints[1].x() + 3*(1-t)*pow(t,2)*controlPoints[2].x() + pow(t,3)*controlPoints[3].x();
    y = pow(1-t,3)*controlPoints[0].y() + 3*pow(1-t,2)*t*controlPoints[1].y() + 3*(1-t)*pow(t,2)*controlPoints[2].y() + pow(t,3)*controlPoints[3].y();
    z = 0.0f;
    return new QVector3D(x,y,z);
}

 void BezierCurve::UpdateCurvePoints()
 {
    QVector3D * newPoint;
    curvePoints.clear();
    for (float t = 0; t <= 1; t += step)
    {
        newPoint = calculatePoint(t);
        curvePoints.push_back(*newPoint);
    }
 }
