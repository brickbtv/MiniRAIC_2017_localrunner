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

#define TR_X(x) ((canvas_center + x))
#define EL_WIDTH (width / 2)
#define EL_HEIGHT (floor_height * height)

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

    // ladder
    p.setPen(Qt::white);
    int ladder_with_offset = config->ladder_position - 30;
    p.drawLine(TR_X(ladder_with_offset), 0, TR_X(ladder_with_offset), rect().bottom());
    p.drawLine(TR_X(-ladder_with_offset), 0, TR_X(-ladder_with_offset), rect().bottom());
}

void Visualizer::drawElement(QPainter& p, QString entity, int width, double height){
    int canvas_center = rect().right() / 2;
    int floor_height = rect().bottom() / this->config->floors_count;

    QJsonArray elevators = tick->toObject()[entity].toArray();
    QMap<QString, int> counter;
    int elev_iter = 0;
    foreach(const QJsonValue & element, elevators){
        int el_x = element.toObject()["x"].toInt();
        double el_y = element.toObject()["y"].toDouble();
        int el_state = element.toObject()["state"].toInt();
        QString el_type = element.toObject()["type"].toString();;

        if (el_type == "FIRST_PLAYER")
            p.setPen(Qt::red);
        else
            p.setPen(Qt::blue);

        p.drawRect(TR_X(el_x - EL_WIDTH),
                   rect().bottom() - el_y * (float)floor_height + floor_height - EL_HEIGHT - 3,
                   EL_WIDTH * 2,
                   EL_HEIGHT);

        QString key = QString::number(el_x) + ":" + QString::number(el_y, 'g', 4);
        if (counter.contains(key))
            counter[key] ++;
        else
            counter[key] = 1;

        // Elevator doors
        if (entity == "elevators"){
            QString elev_id = QString::number(el_x);
            if (elevators_doors.contains(elev_id)) {
                if (el_state == 4){
                    elevators_doors[elev_id] ++;
                    elevators_doors[elev_id] > 100?elevators_doors[elev_id]=100:0;
                } else if (el_state == 2){
                    elevators_doors[elev_id] --;
                    elevators_doors[elev_id] < 0?elevators_doors[elev_id]=0:0;
                } else if (el_state == 1)
                    elevators_doors[elev_id] = 100;
                else if (el_state == 3 || el_state == 0)
                    elevators_doors[elev_id] = 0;

                QBrush tbrush = p.brush();
                p.setBrush(Qt::yellow);
                p.drawRect(TR_X(el_x - EL_WIDTH),
                           rect().bottom() - el_y * (float)floor_height + floor_height - EL_HEIGHT - 3,
                           EL_WIDTH * 2 * (elevators_doors[elev_id])/100,
                           EL_HEIGHT);
                p.setBrush(tbrush);
            } else {
                elevators_doors.insert(elev_id, 0);
            }
            elev_iter++;
        }
    }

    //if (entity == "elevators")
    //    qDebug() << elevators_doors;

    QMap<QString,int>::iterator it = counter.begin();
    for(;it != counter.end(); ++it)
    {
       if (it.value() > 1){
           QStringList coords = it.key().split(':');

           int x = TR_X(QString(coords.at(0)).toInt());
           int y = rect().bottom() - QString(coords.at(1)).toDouble() * (float)floor_height + floor_height/4;

           p.setPen(Qt::gray);
           p.drawText(x,
                      y,
                      QString::number(it.value()));
       }
    }
}

void Visualizer::printScores(QPainter& p){
    int canvas_center = rect().right() / 2;
    p.setPen(Qt::white);
    QJsonObject scores = tick->toObject()["scores"].toObject();
    int score_pos = config->ladder_position - 200;
    p.drawText(TR_X(-score_pos), 10, "First Player score: " + QString::number(scores["FIRST_PLAYER"].toInt()));
    p.drawText(TR_X(score_pos), 10, "Second Player score: " + QString::number(scores["SECOND_PLAYER"].toInt()));
}

void Visualizer::drawDebug(QPainter& p){
    int canvas_center = rect().right() / 2;
    int floor_height = rect().bottom() / this->config->floors_count;
    QJsonObject debug = tick->toObject()["debug"].toObject();
    //qDebug() << debug;
    if (debug.contains("FIRST_PLAYER")){
         QJsonArray log_ar = debug["FIRST_PLAYER"].toObject()["logs"].toArray();
         foreach (QJsonValue val, log_ar){
            QStringList ql = val.toString().split(';');
            if (ql[0] == "e_mov"){
                //qDebug() << ql;
                int eln = QString(ql[1]).toInt();
                int x = 0;
                int y1 = QString(ql[2]).toInt();
                int y2 = QString(ql[3]).toInt();

                x = -(config->elevators_first_pos + config->elevators_offset*((int)(eln/2)));

                p.setPen(Qt::yellow);
                int st_y = rect().bottom() - y1*floor_height + floor_height / 2;
                int en_y = rect().bottom() - y2*floor_height + floor_height / 2;
                p.drawLine(TR_X(x), st_y, TR_X(x), en_y);
                p.drawLine(TR_X(x-3), st_y - 3, TR_X(x+3), st_y + 3);
                p.drawLine(TR_X(x-3), st_y + 3, TR_X(x+3), st_y - 3);
                p.drawEllipse(QPoint(TR_X(x), en_y), 4, 4);
            }
            if (ql[0] == "f_pot"){
                //qDebug() << ql;
                int eln = QString(ql[1]).toInt();
                QStringList potential = QString(ql[2]).split(',');
                int x = -(config->elevators_first_pos + config->elevators_offset*((int)(eln/2)));

                p.setPen(Qt::yellow);
                int i = 1;
                foreach(QString pot, potential){
                    int y = rect().bottom() - i*floor_height + floor_height / 2;
                    p.drawText(TR_X(x + 15), y, pot);
                    i++;
                }
            }
         }
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
        this->printScores(p);
        this->drawDebug(p);
    }
}

void Visualizer::resizeGL(int w, int h)
{

}
