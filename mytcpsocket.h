#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H

#include "visualizer.h"

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class MyTcpServer : public QObject
{
    Q_OBJECT

    QTcpServer * server;
    quint16     m_nNextBlockSize;
    Visualizer * vis;
    bool * run;
    int tick_count;
public:
    explicit MyTcpServer(QObject *parent = nullptr, Visualizer * vis = nullptr, bool * b = nullptr);

signals:
public slots:
    void slotNewConnection();
    void slotReadClient   ();
};

#endif // MYTCPSOCKET_H
