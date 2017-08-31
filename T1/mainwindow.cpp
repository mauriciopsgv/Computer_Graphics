#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStatusBar>
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->checkBox,
            SIGNAL( stateChanged(int) ),
            ui->openGLWidget,
            SLOT(showControlPoints(int)) );
}

MainWindow::~MainWindow()
{
    delete ui;
}
