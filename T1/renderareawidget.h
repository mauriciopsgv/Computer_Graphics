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

private:
    std::vector< QVector3D > previewPoints;
    std::vector< QVector3D > points;
    std::vector< BezierCurve > curves;

    QOpenGLShaderProgram* program;
    QOpenGLBuffer pointsBuffer;

    QMatrix4x4 view;
    QMatrix4x4 proj;

    bool isEditingPoint;
    int editedPointIndex;
};

#endif // RENDERAREAWIDGET_H
