#include "visualizer.h"
#include <QPainter>
#include <QDebug>
#include <QJsonArray>

Visualizer::Visualizer(QWidget* parent) : QOpenGLWidget(parent) {
    this->tick = nullptr;
}

void Visualizer::setConfig(Config* config){
    this->config = config;
}

void Visualizer::initializeGL() {}

void Visualizer::setTick(int tick_number, const QJsonValue & tick) {
    this->tick = new QJsonValue (tick);
    this->tick_number = tick_number;
}

void Visualizer::drawBackground(QPainter& p){
    int canvas_center = rect().right() / 2;
    int floor_height = rect().bottom() / this->config->floors_count;

    QBrush qbred(Qt::darkRed, Qt::Dense7Pattern);
    p.setBrush(qbred);
    p.drawRect(0, 0, canvas_center, rect().bottom());

    QBrush qbblue(Qt::darkBlue, Qt::Dense7Pattern);
    p.setBrush(qbblue);
    p.drawRect(canvas_center, 0, rect().right(), rect().bottom());

    // floors
    p.setPen(Qt::gray);
    for (int i = this->config->floors_first; i <= this->config->floors_count; i++){
        int curr_floor_pos = rect().bottom() - (i - 1) * floor_height;
        p.drawLine(0, curr_floor_pos, rect().right(), curr_floor_pos);
    }
}

void Visualizer::drawElement(QPainter& p, QString entity, int width, double height){
    int canvas_center = rect().right() / 2;
    int floor_height = rect().bottom() / this->config->floors_count;

    QJsonArray elevators = tick->toObject()[entity].toArray();
    qDebug() << this->tick_number;
    foreach(const QJsonValue & element, elevators){
        int el_x = element.toObject()["x"].toInt();
        double el_y = element.toObject()["y"].toDouble();
        int el_state = element.toObject()["state"].toInt();
        QString el_type = element.toObject()["type"].toString();;

        if (el_type == "FIRST_PLAYER")
            p.setPen(Qt::red);
        else
            p.setPen(Qt::blue);

#define TR_X(x) (canvas_center + x)
#define EL_WIDTH (width / 2)
#define EL_HEIGHT (floor_height * height)

        p.drawRect(TR_X(el_x - EL_WIDTH), rect().bottom() - el_y * (float)floor_height + floor_height - EL_HEIGHT - 3, EL_WIDTH * 2, EL_HEIGHT);
    }
}

void Visualizer::paintGL()
{
    QPainter p(this);
    this->drawBackground(p);
    if (this->tick){
        p.setPen(Qt::white);
        p.drawText(20, 20, QString::number(this->tick_number));
        this->drawElement(p, "elevators", 32, 0.75f);
        this->drawElement(p, "passengers", 8, 0.6);
    }
}

void Visualizer::resizeGL(int w, int h)
{

}
