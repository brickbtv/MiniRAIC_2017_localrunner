#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "visualizer.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void MainLoop();

private slots:
    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    Visualizer * vis;
};

#endif // MAINWINDOW_H
