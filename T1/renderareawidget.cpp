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
    isShowingControlPoints = false;
}

void RenderAreaWidget::showControlPoints(int state)
{
    isShowingControlPoints = !isShowingControlPoints;
    update();
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

            //Desenha Bezier preview
            program->setUniformValue("color", QVector3D(1,1,1)); //Branco
            if (!curves.empty())
            {
                std::vector<QVector3D> previewCurvePoints = curves.back().getCurvePoints();
                pointsBuffer.allocate( &previewCurvePoints[0], (int)previewCurvePoints.size()*sizeof(QVector3D) );
                glDrawArrays(GL_LINE_STRIP, 0, (int)previewCurvePoints.size());

                // Desenha pontos de controle da Bezier de Preview
                if (isShowingControlPoints)
                {
                    program->setUniformValue("color", QVector3D(0,0,1)); //Azul
                    std::vector<QVector3D> previewControlPoints = curves.back().getControlPoints();
                    pointsBuffer.allocate( &previewControlPoints[1], (int)2*sizeof(QVector3D) );
                    glDrawArrays(GL_POINTS, 0, 2);
                }
            }
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

            //Desenha Bezier
            program->setUniformValue("color", QVector3D(1,1,1)); //Branco
            std::vector<QVector3D> currentCurvePoints;
            int size = curves.size();
            for (int i = 0; i < size - 1; i++)
            {
                currentCurvePoints = curves[i].getCurvePoints();
                pointsBuffer.allocate( &currentCurvePoints[0], (int)currentCurvePoints.size()*sizeof(QVector3D) );
                glDrawArrays(GL_LINE_STRIP, 0, (int)currentCurvePoints.size());
            }

            //Desenha pontos de controle das Beziers
            if (isShowingControlPoints)
            {
                program->setUniformValue("color", QVector3D(0,0,1)); //Azul
                std::vector<QVector3D> currentControlPoints;
                int size = curves.size();
                for (int i = 0; i < size - 1; i++)
                {
                    currentControlPoints = curves[i].getControlPoints();
                    pointsBuffer.allocate( &currentControlPoints[1], (int)2*sizeof(QVector3D) );
                    glDrawArrays(GL_POINTS, 0, 2);
                }
            }
        }

        if(isEditingPoint)
        {
            //Highlight ponto editado
            pointsBuffer.allocate( &curves[curveBeingEdited].getControlPoints()[controlPointBeingEdited], (int)sizeof(QVector3D) );
            program->setUniformValue("color", QVector3D(1,0,1)); //Roxo
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
    std::vector<QVector3D> controlPoints;
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( view, proj, QRect(0,0,width(),height()));

    // curves - 1 because the last curve os this vector is the preview curve
    int size = curves.size();
    for (int i = 0; i < size - 1; i++)
    {
        controlPoints = curves[i].getControlPoints();
        for (int j = 0; j < 4; j++)
        {
            if(point.distanceToPoint(controlPoints[j]) < 1.05)
            {
                isEditingPoint = true;
                curveBeingEdited = i;
                controlPointBeingEdited = j;
                return;
            }
        }
    }
}


void RenderAreaWidget::mouseMoveEvent(QMouseEvent *event)
{
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( view, proj, QRect(0,0,width(),height()));
    point.setZ(0.f);

    if (points.empty())
    {
        previewPoints.front().setX(point.x());
        previewPoints.front().setY(point.y());
    }
    if (isEditingPoint)
    {
        // if the last point inputted is being edited we need to edit the preview point as well
        if (curveBeingEdited == curves.size() - 2 &&
            controlPointBeingEdited == 3)
        {
            previewPoints.front().setX(point.x());
            previewPoints.front().setY(point.y());
        }
        if (!curves.empty())
        {
            if (controlPointBeingEdited == 0 || controlPointBeingEdited == 3)
            {
                points[curveBeingEdited + controlPointBeingEdited/3].setX(point.x());
                points[curveBeingEdited + controlPointBeingEdited/3].setY(point.y());
            }
            editBezier(point);
        }
    }

    updatePreviewBezier(point);

    previewPoints.back().setX(point.x());
    previewPoints.back().setY(point.y());

    update();
}


void RenderAreaWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QVector3D point( event->x(), height()-event->y(), 0 );
    point = point.unproject( view, proj, QRect(0,0,width(),height()));
    point.setZ(0.f);
    if (event->button() == Qt::LeftButton)
    {
        if (!isEditingPoint)
        {
            points.push_back( point );
            previewPoints.front().setX(point.x());
            previewPoints.front().setY(point.y());

            QVector3D * newControlPoints;
            int size = points.size();

            if (size > 2)
            {
                newControlPoints = calculateBezierControlPoints(points.rbegin()[1], points.rbegin()[0], point, size + 1, &curves.back().getControlPoints()[1]);
                curves.back().setControlPoint(2, newControlPoints[0]);
                curves.push_back(*new BezierCurve(points.back(), newControlPoints[1], newControlPoints[2], point));
            }
            else if (size == 2)
            {
                newControlPoints = calculateBezierControlPoints(points.rbegin()[1], points.rbegin()[0], point, size + 1);
                curves.back().setControlPoint(1, newControlPoints[0]);
                curves.back().setControlPoint(2, newControlPoints[1]);
                curves.push_back(*new BezierCurve(points.back(), newControlPoints[2], newControlPoints[3], point));
            }
            else if (size == 1)
            {
                BezierCurve newCurve(points.front(), (1.0/3.0)*(point-points.front()) + points.front(), (2.0/3.0)*(point-points.front()) + points.front(), point);
                curves.push_back(newCurve);
            }
            delete []newControlPoints;

        }
        else
        {
            isEditingPoint = false;
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        curves.pop_back();
        points.pop_back();
        previewPoints.front() = points.back();
        updatePreviewBezier(point);
    }

    update();
}

void RenderAreaWidget::editBezier(QVector3D point)
{

    if ( (curveBeingEdited == 0 &&
          (controlPointBeingEdited == 0 || controlPointBeingEdited == 1)) ||
         (curveBeingEdited == curves.size() - 1 &&
          (controlPointBeingEdited == 2 || controlPointBeingEdited == 3)))
    {
        curves[curveBeingEdited].setControlPoint(controlPointBeingEdited, point);
        return;
    }

    QVector3D move = point - curves[curveBeingEdited].getControlPoints()[controlPointBeingEdited];
    if (controlPointBeingEdited != 3)
    {
        // moving Rn
        if (controlPointBeingEdited == 1)
        {
            QVector3D ln = curves[curveBeingEdited-1].getControlPoints()[2];
            curves[curveBeingEdited].setControlPoint(controlPointBeingEdited, point);
            curves[curveBeingEdited-1].setControlPoint(2, ln - move );
        }
        // moving Ln
        else
        {
            QVector3D rn = curves[curveBeingEdited+1].getControlPoints()[1];
            curves[curveBeingEdited].setControlPoint(controlPointBeingEdited, point);
            curves[curveBeingEdited+1].setControlPoint(1, rn - move );
        }
    }
    // moving Pn
    else
    {
        QVector3D ln = curves[curveBeingEdited].getControlPoints()[2];
        QVector3D rn = curves[curveBeingEdited+1].getControlPoints()[1];
        // if changing the point between two beziers you have to update on both sides
        curves[curveBeingEdited].setControlPoint(controlPointBeingEdited, point);
        curves[curveBeingEdited+1].setControlPoint(0, point);

        curves[curveBeingEdited].setControlPoint(2, ln + move );
        curves[curveBeingEdited+1].setControlPoint(1, rn + move );
    }

}

void RenderAreaWidget::updatePreviewBezier(QVector3D point)
{
    QVector3D * newControlPoints;
    int size = points.size();
    if (size > 2)
    {
        newControlPoints = calculateBezierControlPoints(points.rbegin()[1], points.rbegin()[0], point, size + 1, &curves.rbegin()[1].getControlPoints()[1]);
        curves.rbegin()[1].setControlPoint(2, newControlPoints[0]);
        curves.back().setControlPoint(1, newControlPoints[1]);
        curves.back().setControlPoint(2, newControlPoints[2]);
        curves.back().setControlPoint(3, point);
        delete &newControlPoints[0];
        delete &newControlPoints[1];
        delete &newControlPoints[2];
    }
    else if (size == 2)
    {
        newControlPoints = calculateBezierControlPoints(points.rbegin()[1], points.rbegin()[0], point, size + 1);
        curves.rbegin()[1].setControlPoint(1, newControlPoints[0]);
        curves.rbegin()[1].setControlPoint(2, newControlPoints[1]);
        curves.back().setControlPoint(1, newControlPoints[2]);
        curves.back().setControlPoint(2, newControlPoints[3]);
        curves.back().setControlPoint(3, point);
        delete &newControlPoints[0];
        delete &newControlPoints[1];
        delete &newControlPoints[2];
        delete &newControlPoints[3];
    }
    else if (size == 1)
    {
        curves.front().setControlPoint(1, (1.0/3.0)*(point-points.front()) + points.front());
        curves.front().setControlPoint(2, (2.0/3.0)*(point-points.front()) + points.front());
        curves.front().setControlPoint(3, point);
    }
}

QVector3D * RenderAreaWidget::calculateBezierControlPoints(QVector3D p0, QVector3D p1, QVector3D p2, int size, QVector3D * rn1)
{
    int n;
    float f;
    float A[4][4];
    QVector3D controlPoints[4];
    std::vector<QVector3D> b;
    QVector3D b0(0,0,0);
    float l0 = p1.distanceToPoint(p0);
    float l1 = p2.distanceToPoint(p1);
    if (l1 == 0)
    {
        l1 = 0.5;
    }
    float pho = l0/(l0+l1);

    if (size != 3)
    {
        n = 3;
        A[0][0] = (1-pho); A[0][1] = pho; A[0][2] =  0;
        A[1][0] =      -2; A[1][1] =   2; A[1][2] = -1;
        A[2][0] =       0; A[2][1] =  -1; A[2][2] =  2;

        b.push_back(p1);
        b.push_back(-1*(*rn1));
        b.push_back(p2);
    }
    else
    {
        n = 4;
        A[0][0] = 2; A[0][1] =      -1; A[0][2] =   0; A[0][3] =  0;
        A[1][0] = 0; A[1][1] = (1-pho); A[1][2] = pho; A[1][3] =  0;
        A[2][0] = 1; A[2][1] =      -2; A[2][2] =   2; A[2][3] = -1;
        A[3][0] = 0; A[3][1] =       0; A[3][2] =  -1; A[3][3] =  2;

        b.push_back(p0);
        b.push_back(p1);
        b.push_back(b0);
        b.push_back(p2);
    }

    // Metodo de Eliminação de Gauss
    for ( int j = 0; j < n-1; j++) {
        for ( int i = j+1; i < n ; i ++) {
            f = A[i][j]/A[j][j];

            for (int k = j; k < n; k++) {
                A[i][k] = A[i][k] - A[j][k]*f;
            }
            b[i] = b[i] - b[j]*f;
        }
    }

    for (int i = n-1; i >= 0; i--) {
        QVector3D s(0,0,0);
        for (int j = n-1; j > i; j--) {
            s = s + A[i][j]*controlPoints[j];
        }
        controlPoints[i] = (b[i] - s)/A[i][i];
    }

    QVector3D* newControlPoints = new QVector3D[4];
    newControlPoints[0] = controlPoints[0];
    newControlPoints[1] = controlPoints[1];
    newControlPoints[2] = controlPoints[2];
    if (size == 3)
        newControlPoints[3] = controlPoints[3];

    return newControlPoints;
}

