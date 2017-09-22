#include "config.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "visualizer.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QTimer>
#include <QTime>

QString filename = "game_med.js";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->frame->layout()->removeWidget(ui->openGLWidget);
    delete ui->openGLWidget;

    this->MainLoop();
}

void MainWindow::MainLoop(){
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString gamedump = file.readAll();
    file.close();

    QJsonParseError qerr;
    QJsonDocument jsongame = QJsonDocument::fromJson(gamedump.toUtf8(), &qerr);
    if (jsongame.isNull() || jsongame.isEmpty())
        qDebug() << qerr.errorString();
    else {
        QJsonObject obj = jsongame.object();
        qDebug() << obj["config"];
        Config * cfg = new Config(obj);

        vis = new Visualizer(this);
        vis->setConfig(cfg);
        ui->frame->layout()->addWidget(vis);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString gamedump = file.readAll();
    file.close();

    QJsonParseError qerr;
    QJsonDocument jsongame = QJsonDocument::fromJson(gamedump.toUtf8(), &qerr);
    if (jsongame.isNull() || jsongame.isEmpty())
        qDebug() << qerr.errorString();
    else {
        QJsonObject obj = jsongame.object();
        QJsonArray ticks = obj["game_data"].toArray();
        int tick_index = 0;
        foreach(const QJsonValue & tick, ticks) {
            this->vis->setTick(tick_index++, tick);
            this->vis->repaint();
            Sleep(10);
        }
    }
}
