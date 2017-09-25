#include "config.h"
#include "mainwindow.h"
#include "mytcpsocket.h"
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
#include <QMessageBox>
#include <QProcess>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->frame->layout()->removeWidget(ui->openGLWidget);
    delete ui->openGLWidget;
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(slotTimerAlarm()));
    filename = ":\\game.js";
    tick_index = 0;
    manualTick = false;
    speed = 1;
    socket_run = false;

    QFile file("Settings.cfg");
    if (file.exists()){
        file.open(QIODevice::ReadOnly|QIODevice::Text);
        QStringList ql = QString(file.readAll()).split(';');
        ui->runnerPath->setText(ql[0]);
        ui->stratOne->setText(ql[1]);
        ui->stratTwo->setText(ql[2]);
        file.close();
    }

    this->preConfig();
}

QJsonParseError MainWindow::reparseReplay()
{
    QProgressDialog *progress = new QProgressDialog("Реплей загружается...", "", 0, 0, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->setCancelButton(false);
    progress->setValue(50);
    progress->show();
    QCoreApplication::processEvents();

    QFile file(filename);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString gamedump = file.readAll();
    if (gamedump.startsWith("var data = ")){
        int leftcut = QString("var data = ").length();
        gamedump = gamedump.mid(leftcut, gamedump.length() - 1 - leftcut);
    }
    file.close();
    progress->setValue(75);
    QCoreApplication::processEvents();
    QJsonParseError qerr;
    jsongame = QJsonDocument::fromJson(gamedump.toUtf8(), &qerr);

    ticks = jsongame.object()["game_data"].toArray();
    ui->ticksSlider->setMaximum(ticks.size());
    ui->counter->setText(QString::number(0) + "/" + QString::number(ticks.size()));

    progress->setValue(100);
    QCoreApplication::processEvents();
    progress->close();
    delete progress;

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
    int maxfast = 1;
    if (speed <=10)
        maxfast = 100/speed;

    for (int i = 0; i < maxfast; i++){
        if (tick_index >= ticks.size()){
            timer->stop();
            return;
        }

        this->vis->setTick(tick_index, ticks.at(tick_index));
        this->vis->repaint();
        ui->counter->setText(QString::number(tick_index) + "/" + QString::number(ticks.size()));
        if (!manualTick){
            tick_index++;
            ui->ticksSlider->setValue(tick_index);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startstop_clicked()
{
    QJsonObject obj = jsongame.object();
    ticks = obj["game_data"].toArray();

    if (tick_index >= ticks.size())
        tick_index = 0;

    if ( ! timer->isActive()){
        timer->start(getSpeed());
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

void MainWindow::on_speed_sliderMoved(int position)
{

}

int MainWindow::getSpeed()
{
    speed = ui->speedSlider->value();

    return speed;
}

void MainWindow::on_speedSlider_actionTriggered(int action)
{
    int speed = getSpeed();
    this->timer->setInterval(speed);
}

void MainWindow::on_ticksSlider_actionTriggered(int action)
{
    int new_tick = ui->ticksSlider->value();
    if (this->timer->isActive()){
        this->timer->stop();
    }
    tick_index = new_tick;
    manualTick = true;
    slotTimerAlarm();
    manualTick = false;
}

QString runnerPath = "D:\\Temp\\aicups-master\\localrunner\\world";
QString cmdStrategy1 = "python D:\\repo\\MiniRAIC2017\\python2_client\\run.py";
QString cmdStrategy2 = "python D:\\repo\\MiniRAIC2017\\bot_01\\run.py";

void MainWindow::on_runGame_clicked()
{
    socket_run = true;
    server = new MyTcpServer(this, this->vis, &this->socket_run);

    QProcess qp;
    qp.start("python " + ui->runnerPath->toPlainText());

    Sleep(1000);
    QProcess qcmd1;
    qcmd1.start(ui->stratOne->toPlainText());
    Sleep(1000);

    QProcess qcmd2;
    qcmd2.start(ui->stratTwo->toPlainText());

    QFile file("Settings.cfg");
    file.open(QIODevice::WriteOnly|QIODevice::Text);
    file.write(QString(ui->runnerPath->toPlainText() + ";").toStdString().c_str());
    file.write(QString(ui->stratOne->toPlainText() + ";").toStdString().c_str());
    file.write(QString(ui->stratTwo->toPlainText() + ";").toStdString().c_str());
    file.close();

    while (true){//socket_run){
        Sleep(1);
        QCoreApplication::processEvents();
    }

    qp.waitForFinished();
}

void MainWindow::on_browse1_clicked()
{
    runnerPath = QFileDialog::getOpenFileName(this,
            tr("Open run.py"), "",
            tr("run.py (*py);"));

    this->ui->runnerPath->setText(runnerPath);
}

void MainWindow::on_browse2_clicked()
{
    cmdStrategy1 = QFileDialog::getOpenFileName(this,
            tr("Open run.py"), "",
            tr("run.py (*py);"));

    this->ui->stratOne->setText(cmdStrategy1);
}

void MainWindow::on_browse3_clicked()
{
    cmdStrategy2 = QFileDialog::getOpenFileName(this,
            tr("Open run.py"), "",
            tr("run.py (*py);"));

    this->ui->stratTwo->setText(cmdStrategy2);
}
