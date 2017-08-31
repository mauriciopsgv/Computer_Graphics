#include "renderareawidget.h"

#include <QMouseEvent>

const char* vertexShaderSource = R"(
    #version 330 core

    layout( location = 0 ) in vec3 vertexPos;
    uniform mat4 transformMatrix;

    void main()
    {
        gl_Position = transformMatrix * vec4( vertexPos, 1 );
    }
)";


const char* fragmentShaderSource = R"(
    #version 330 core

    uniform vec3 color;
    out vec3 finalColor;

    void main()
    {
        finalColor = color;
    }
)";


RenderAreaWidget::RenderAreaWidget(QWidget* parent)
    : QOpenGLWidget(parent),
      program(nullptr)
{
    this->setMouseTracking(true);
    previewPoints.resize(2);
    isEditingPoint = false;
}


RenderAreaWidget::~RenderAreaWidget()
{
    makeCurrent();
    pointsBuffer.destroy();
    doneCurrent();
    delete program;
}


void RenderAreaWidget::initializeGL()
{
    initializeOpenGLFunctions();

    makeCurrent();

    glViewport(0,0,width(),height());

    //Layout de ponto e linha:
    glEnable(GL_POINT_SMOOTH);
    glEnable( GL_LINE_SMOOTH );
    glLineWidth(1.0f);
    glPointSize(8.0f);

    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
    program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
    program->link();

    if (!program->isLinked())
    {
        //TODO: Exibir erro de link e fechar o programa
    }

    program->bind();

    pointsBuffer.create();

    proj.ortho(-10.f,10.f,-10.f,10.f,-1.f,1.0f);

    program->setUniformValue("transformMatrix", proj*view);
}


void RenderAreaWidget::paintGL()
{
    program->bind();

    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    if (pointsBuffer.isCreated())
    {
        pointsBuffer.bind();
        program->enableAttributeArray(0);
        program->setAttributeBuffer(0,GL_FLOAT,0,3,sizeof(QVector3D));

        if(!previewPoints.empty() && !isEditingPoint)
        {
            pointsBuffer.allocate( &previewPoints[0], (int)previewPoints.size()*sizeof(QVector3D) );

            //Desenhar ponto preview
            program->setUniformValue("color", QVector3D(1,1,0)); //Amarelo
            glDrawArrays(GL_POINTS, 0, (int)previewPoints.size());

            //Desenha poligono preview
            program->setUniformValue("color", QVector3D(1,0,0)); //Vermelho
            glDrawArrays(GL_LINE_STRIP, 0, (int)previewPoints.size());
        }

        if(!points.empty())
        {
            //Desenha o poligono
            program->setUniformValue("color", QVector3D(1,0,0)); //Vermelho
            pointsBuffer.allocate( &points[0], (int)points.size()*sizeof(QVector3D) );
            glDrawArrays(GL_LINE_STRIP, 0, (int)points.size());

            //Desenha os pontos
            program->setUniformValue("color", QVector3D(1,1,0)); //Amarelo
            glDrawArrays(GL_POINTS, 0, (int)points.size());

            //Desenha Bezier e pontos de controle
            program->setUniformValue("color", QVector3D(1,1,1)); //Branco
            std::vector<QVector3D> currentCurvePoints;
            for (int i = 0; i < curves.size(); i++)
            {
//                currentCurvePoints = curves[i].getControlPoints();
//                pointsBuffer.allocate( &currentCurvePoints[0], (int)currentCurvePoints.size()*sizeof(QVector3D) );
//                glDrawArrays(GL_POINTS, 0, (int)currentCurvePoints.size());
                currentCurvePoints = curves[i].getCurvePoints();
                pointsBuffer.allocate( &currentCurvePoints[0], (int)currentCurvePoints.size()*sizeof(QVector3D) );
                glDrawArrays(GL_LINE_STRIP, 0, (int)currentCurvePoints.size());
            }

        }

        if(isEditingPoint)
        {
            //Highlight ponto editado
            pointsBuffer.allocate( &points[editedPointIndex], (int)sizeof(QVector3D) );
            program->setUniformValue("color", QVector3D(0,0,1)); //Azul
            glDrawArrays(GL_POINTS, 0, 1);
        }
    }
}


void RenderAreaWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}


void RenderAreaWidget::mousePressEvent(QMouseEvent *event)
{
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( view, proj, QRect(0,0,width(),height()));
    for (int i = 0; i < points.size(); i++)
    {
        if(point.distanceToPoint(points[i]) < 1.05)
        {
            isEditingPoint = true;
            editedPointIndex = i;
            return;
        }
    }
}


void RenderAreaWidget::mouseMoveEvent(QMouseEvent *event)
{
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( view, proj, QRect(0,0,width(),height()));
    if (points.empty())
    {
        previewPoints.front().setX(point.x());
        previewPoints.front().setY(point.y());
    }
    if (isEditingPoint)
    {
        if (editedPointIndex == points.size() - 1)
        {
            previewPoints.front().setX(point.x());
            previewPoints.front().setY(point.y());
        }
        points[editedPointIndex].setX(point.x());
        points[editedPointIndex].setY(point.y());
        if (!curves.empty())
        {
            curves.front().setControlPoint(editedPointIndex, points[editedPointIndex]);
        }
    }
    previewPoints.back().setX(point.x());
    previewPoints.back().setY(point.y());

    update();
}


void RenderAreaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (!isEditingPoint)
    {
        QVector3D point( event->x(), height()-event->y(), 0 );
        point = point.unproject( view, proj, QRect(0,0,width(),height()));
        point.setZ(0.f);

        points.push_back( point );
        previewPoints.front().setX(point.x());
        previewPoints.front().setY(point.y());

        if (points.size() == 4)
        {
            BezierCurve newCurve(points[0], points[1], points[2], points[3]);
            curves.push_back(newCurve);
        }

//        if (points.size() > 3)
//        {

//        }
//        else if (points.size() == 2)
//        {
//            BezierCurve newCurve(points[0], (1.0/3.0)*(points[1]-points[0]) + points[0], (2.0/3.0)*(points[1]-points[0]) + points[0], points[1]);
//            curves.push_back(newCurve);
//        }
//        else if (points.size() == 3)
//        {
//            BezierCurve newCurve(points[0], (1.0/3.0)*(points[1]-points[0]) + points[0], (2.0/3.0)*(points[1]-points[0]) + points[0], points[1]);
//            curves.push_back(newCurve);
//        }
    }
    else
    {
        isEditingPoint = false;
    }


    update();
}

