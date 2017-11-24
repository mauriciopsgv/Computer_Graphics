#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionGenerate_Ray_Tracing_Image, SIGNAL(triggered(bool)),
            ui->renderWidget, SLOT(generateRayTracingImage(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}
