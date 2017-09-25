#include "mytcpsocket.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTime>

MyTcpServer::MyTcpServer(QObject *parent, Visualizer * vis, bool * run) :
    QObject(parent)
{
    this->vis = vis;
    server = new QTcpServer(this);

    // whenever a user connects, it will emit signal
    connect(server, SIGNAL(newConnection()),
            this, SLOT(slotNewConnection()));

    if(!server->listen(QHostAddress::Any, 49000))
    {
        qDebug() << "Server could not start";
    }
    else
    {
        qDebug() << "Server started!";
    }

    m_nNextBlockSize = 0;
    this->run = run;
}

void MyTcpServer::slotNewConnection()
{
    // need to grab the socket
    QTcpSocket *pClientSocket = server->nextPendingConnection();

    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );
    tick_count = 0;
}

void MyTcpServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QByteArray qba = pClientSocket->readAll();
    if (qba.size() > 0){
        if (QString(qba) == "exit"){
            *this->run = false;
            return;
        }

        QStringList few = QString(qba).split('$');
        foreach (QString line, few) {
            if (line.isEmpty())
                continue;

            QJsonParseError jpe;
            QJsonDocument jsond = QJsonDocument::fromJson(QByteArray(line.toStdString().c_str()), &jpe);

            vis->setTick(tick_count++, jsond.array().at(0));
            vis->repaint();
        }
    }

}
