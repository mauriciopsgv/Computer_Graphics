#ifndef RENDERAREAWIDGET_H
#define RENDERAREAWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include <vector>

#include "beziercurve.h"

class RenderAreaWidget
    : public QOpenGLWidget
    , protected QOpenGLFunctions
{
Q_OBJECT

public:
    explicit RenderAreaWidget(QWidget* parent = 0);
    ~RenderAreaWidget();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void showControlPoints(int state);

private:
    std::vector< QVector3D > previewPoints;

    std::vector< QVector3D > points;
    std::vector< BezierCurve > curves;

    QOpenGLShaderProgram* program;
    QOpenGLBuffer pointsBuffer;

    QMatrix4x4 view;
    QMatrix4x4 proj;

    bool isShowingControlPoints;
    bool isEditingPoint;
    unsigned int curveBeingEdited;
    int controlPointBeingEdited;

    void editBezier(QVector3D point);

    void updatePreviewBezier(QVector3D point);

    QVector3D * calculateBezierControlPoints(QVector3D p0, QVector3D p1, QVector3D p2, int n, QVector3D * rn1 = nullptr);
};

#endif // RENDERAREAWIDGET_H
