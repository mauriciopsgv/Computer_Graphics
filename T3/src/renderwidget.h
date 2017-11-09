#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QOpenGLShaderProgram>
#include <QVector3D>
#include <QMatrix4x4>

#include <vector>

#include "glm/glm.hpp"

class RenderWidget
        : public QOpenGLWidget
        , protected QOpenGLExtraFunctions
{
public:
    RenderWidget(QWidget* parent);
    virtual ~RenderWidget();

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
    std::vector< glm::vec3 > vertices;
    std::vector< glm::vec3 > normals;
    std::vector< glm::vec2 > texCoords;
    std::vector< unsigned int > indices;

    struct Camera {
       glm::vec3 eye;      /* posicao do olho ou centro de projecao conica */
       glm::vec3 at;       /* ponto para onde a camera aponta              */
       glm::vec3 up;       /* orientacao da direcao "para cima" da camera  */
       float fovy;         /* angulo de abertura da camera                 */
       float zNear,zFar;   /* distancia do plano proximo e distante        */
       float width,height; /* largura e altura da janela em pixels         */
    };

    Camera cam;
    glm::mat4x4 model;
    glm::mat4x4 view;
    glm::mat4x4 proj;

    unsigned int textureID;
};

#endif // RENDERWIDGET_H
