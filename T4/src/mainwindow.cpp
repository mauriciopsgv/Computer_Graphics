#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->actionGenerate_Ray_Tracing_Image, SIGNAL(triggered()),
            this, SLOT(generateRayTracingImage()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::generateRayTracingImage()
{
    QImage rayTracingImage = ui->renderWidget->generateRayTracingImage();
    ui->label->setPixmap(QPixmap::fromImage(rayTracingImage));
}
