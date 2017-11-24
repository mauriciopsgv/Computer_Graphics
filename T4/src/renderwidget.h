#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#include <vector>

#ifndef GLM_HPP
#define GLM_HPP
#include "glm/glm.hpp"
#endif

#include "camera.h"
#include "raytracingengine.h"

class RenderWidget
        : public QOpenGLWidget
        , protected QOpenGLExtraFunctions
{
Q_OBJECT

public:
    RenderWidget(QWidget* parent = 0);
    virtual ~RenderWidget();

public slots:
    void generateRayTracingImage();

private:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);

    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void wheelEvent(QWheelEvent *event);

    void createCube();
    void createSphere();
    void createVBO();
    void createTexture(const std::string& imagePath);

    QOpenGLShaderProgram* program;

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    unsigned int textureID;
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > normals;
    std::vector< glm::vec2 > texCoords;
    std::vector< unsigned int > indices;

    Camera cam;

    glm::vec2 center;  /* centro da esfera, coordenadas da tela */
    float radius;  /* raio da esfera */

    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 proj;

    bool isRotating;
    glm::vec3 lastSpherePoint;
    glm::vec3 projectOnArballSphere(float x, float y);

};

#endif // RENDERWIDGET_H
