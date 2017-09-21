#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>

#include "image.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionCarregar_imagem_triggered();
    void on_actionSalvar_imagem_triggered();
    void on_actionCarregar_resultado_triggered();

    void on_actionSmoothing_triggered();
    void on_actionArestas_triggered();
    void on_actionHaar_triggered();
    void on_actionHaar_inverso_triggered();
    void on_actionRealce_em_Haar_triggered();

private:
    void updateInputLabel();
    void updateOutputLabel();

    Ui::MainWindow *ui;

    Image inputImage;
    Image outputImage;

    QLabel* inputLabel;
    QLabel* outputLabel;
};

#endif // MAINWINDOW_H
