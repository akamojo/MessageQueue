#include "widget.h"
#include "ui_widget.h"
#include "clilib.h"
#include <string>
#include <iostream>

using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    cli = new Clilib;

    QDir directory = QDir::current();
    directory.cdUp();
    if(cli->OpenSerData(directory.absoluteFilePath("ser_data.txt")) == -1) {
        cout << "Opening data file failed!" << endl;
    }

    ui->connHost->setText(cli->getHost());
    ui->connPort->setValue(cli->getPort());

    connect(ui->connButton, &QPushButton::pressed, this, &Widget::doConnect);
    connect(ui->pubButton, &QPushButton::pressed, this, &Widget::publish);
    connect(ui->subButton, &QPushButton::pressed, this, &Widget::subscribe);

    ui->tagGroup->setEnabled(false);
    ui->msgGroup->setEnabled(false);

}


Widget::~Widget() {
    delete ui;
}


void Widget::doConnect() {

    cli->setHost(ui->connHost->text());
    cli->setPort(ui->connPort->text().toInt());

    cli->doConnect(this);

    connect(cli->sock, &QTcpSocket::connected, this, &Widget::connSucceeded);
    connect(cli->sock, &QTcpSocket::readyRead, this, &Widget::dataAvail);

    connect(cli->sock, (void (QTcpSocket::*) (QTcpSocket::SocketError) ) &QTcpSocket::error, this, &Widget::someerror);

}


void Widget::connSucceeded() {
    cli->connSucceeded();
    ui->connGroup->setEnabled(false);
    ui->tagGroup->setEnabled(true);
    ui->msgGroup->setEnabled(true);
}


void Widget::dataAvail() {

    ui->msgRecv->append(cli->dataAvail());
}


void Widget::publish() {

    cli->publish(ui->msgTagInput->text(), ui->msgInput->toPlainText());

    ui->msgTagInput->clear();
    ui->msgInput->clear();

}


void Widget::subscribe() {

    cli->subscribe(ui->tagInput->text());

    ui->tagList->clear();
    for(auto i : cli->getTags()) {
        ui->tagList->append(((QString) i));
    }
    ui->tagInput->clear();
}


void Widget::someerror(QTcpSocket::SocketError) {
    QMessageBox::critical(this,"Error", cli->someerror());
}


void Widget::closeEvent(QCloseEvent *event) {
    if(cli->connected == true) {
        cli->closeConn();
    }
    event->accept();
}
