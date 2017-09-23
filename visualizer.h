#ifndef VISUALIZER_H
#define VISUALIZER_H

#include "config.h"

#include <QOpenGLWidget>

class Visualizer : public QOpenGLWidget
{
    Config * config;
    QJsonValue * tick;
    int tick_number;
public:
    Visualizer(QWidget* parent);
    void setConfig(Config * config);
    void drawBackground(QPainter& p);
    void setTick(int tick_number, const QJsonValue& tick);
    void drawElement(QPainter& p, QString entity, int width, double height);
    void printScores(QPainter& p);

    void initializeGL() ;
    void paintGL() ;
    void resizeGL(int w, int h) ;
};

#endif // VISUALIZER_H
