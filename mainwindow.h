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

    void preConfig();

    QJsonParseError reparseReplay();

    int getSpeed();

private slots:
    void on_startstop_clicked();
    void slotTimerAlarm();

    void on_browse_clicked();

    void on_reset_clicked();

    void on_speed_sliderMoved(int position);

    void on_speedSlider_actionTriggered(int action);

    void on_ticksSlider_actionTriggered(int action);

private:
    Ui::MainWindow *ui;
    Visualizer * vis;
    QTimer *timer;
    QJsonDocument jsongame;
    QJsonArray ticks;
    int tick_index;

    QString filename;
    int speed;
    bool manualTick;
};

#endif // MAINWINDOW_H
