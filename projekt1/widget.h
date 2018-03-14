#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets>
#include <QtNetwork>
#include <QWidget>

#include "clilib.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void doConnect();
    void connSucceeded();
    void dataAvail();
    void publish();
    void subscribe();
    void someerror(QTcpSocket::SocketError);
    void closeEvent(QCloseEvent *event);

    Clilib * cli;

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
