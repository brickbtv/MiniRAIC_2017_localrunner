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
#include <QFileDialog>
#include <QProgressDialog>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->frame->layout()->removeWidget(ui->openGLWidget);
    delete ui->openGLWidget;
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    filename = "game.js";
    tick_index = 0;

    this->preConfig();
}

QJsonParseError MainWindow::reparseReplay()
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString gamedump = file.readAll();
    file.close();

    QJsonParseError qerr;
    jsongame = QJsonDocument::fromJson(gamedump.toUtf8(), &qerr);

    return qerr;
}

void MainWindow::preConfig(){
    QJsonParseError qerr = reparseReplay();
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

void MainWindow::slotTimerAlarm()
{
    if (tick_index >= ticks.size()){
        timer->stop();
        return;
    }
    this->vis->setTick(tick_index, ticks.at(tick_index));
    this->vis->repaint();
    tick_index++;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startstop_clicked()
{
    QJsonObject obj = jsongame.object();
    ticks = obj["game_data"].toArray();

    if ( ! timer->isActive()){
        timer->start(10);
    } else
        timer->stop();
}

void MainWindow::on_browse_clicked()
{
    this->filename = QFileDialog::getOpenFileName(this,
            tr("Open game.js"), "",
            tr("Game.js (*.js);"));

    ui->filenamelabel->setText(this->filename);
    QJsonParseError qerr = reparseReplay();
    if (jsongame.isNull() || jsongame.isEmpty())
        qDebug() << qerr.errorString();
}

void MainWindow::on_reset_clicked()
{
    tick_index = 0;

    if (timer->isActive())
        timer->stop();

    this->vis->setTick(tick_index, ticks.at(tick_index));
    this->vis->repaint();
}
