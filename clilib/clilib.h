#ifndef CLILIB_H
#define CLILIB_H

#include "clilib_global.h"

#include <string>
#include <iostream>
#include <QtNetwork>

class CLILIBSHARED_EXPORT Clilib
{

public:
    Clilib();
    int OpenSerData(QString);
    QString getHost();
    int getPort();
    QSet<QString> getTags();
    void setHost(QString);
    void setPort(int);

    void doConnect(QObject*);
    void connSucceeded();
    QString dataAvail();
    void publish(QString, QString);
    void subscribe(QString);
    QString someerror();
    void closeConn();

    QTcpSocket * sock = 0;
    bool connected = false;
    int msg_size = -1;

private:
    QString host;
    int port;
    QSet<QString> tags;
};

#endif // CLILIB_H
