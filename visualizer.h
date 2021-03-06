#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "config.h"

#include <QMap>
#include <QOpenGLWidget>

class Visualizer : public QOpenGLWidget
{
    Config * config;
    QJsonValue * tick;
    int tick_number;
public:
    QMap<QString, int> elevators_doors;

    Visualizer(QWidget* parent);
    void setConfig(Config * config);
    void drawBackground(QPainter& p);
    void setTick(int tick_number, const QJsonValue& tick);
    void drawElement(QPainter& p, QString entity, int width, double height);
    void printScores(QPainter& p);
    void drawDebug(QPainter& p);

    void initializeGL() ;
    void paintGL() ;
    void resizeGL(int w, int h) ;
};

#endif // VISUALIZER_H
