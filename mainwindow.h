#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "visualizer.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QMainWindow>
#include <QTimer>

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
    void slotTimerAlarm();

private:
    Ui::MainWindow *ui;
    Visualizer * vis;
    QTimer *timer;
    QJsonDocument jsongame;
    QJsonArray ticks;
    int tick_index;
};

#endif // MAINWINDOW_H
