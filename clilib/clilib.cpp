#include "clilib.h"


using namespace std;

Clilib::Clilib()
{
}

int Clilib::OpenSerData(QString file) {
    QFile in(file);

    if(in.open(QIODevice::ReadOnly)) {
        QString line = in.readLine();
        line.chop(4);
        host = line;

        line = in.readLine();
        port = line.toInt();

        in.close();
        return 0;

    }
    return -1;
}

QString Clilib::getHost() {
    return host;
}

int Clilib::getPort() {
    return port;
}

QSet<QString> Clilib::getTags() {
    return tags;
}

void Clilib::setHost(QString ip) {
    host = ip;
}

void Clilib::setPort(int p) {
    port = p;
}

void Clilib::doConnect(QObject * parent) {
    sock = new QTcpSocket(parent);
    sock->connectToHost(host, port);

}

void Clilib::connSucceeded() {
    cout << "conn succeded" << endl;
    connected = true;
}



int len_needs_bytes = 5;
int msg_needs_bytes = 0;
QString msg = "";
QString msg_len = "";

QString Clilib::dataAvail() {
  cout<<"Waiting: "<< sock->bytesAvailable() <<endl;
  if(len_needs_bytes > 0){
        auto msg_size_rcv = sock -> read(len_needs_bytes);
        cout<< "Aktualnie: " << msg_len.toUtf8().constData() << endl;
        len_needs_bytes -= msg_size_rcv.size();
        msg_len.append(QString::fromUtf8(msg_size_rcv));
  }

  if(len_needs_bytes == 0){
      cout << msg_len.toUtf8().constData() << endl;
      msg_len.remove(QRegExp("^[0]*"));
      msg_needs_bytes = msg_len.toInt();
      cout << "Rozmiar do przeczytania: " << msg_needs_bytes << endl;

      if(msg_needs_bytes == 0){
          len_needs_bytes = 5;
      }
  }

  if(msg_needs_bytes > 0) {
      auto msg_data = sock->read(msg_needs_bytes);
      msg_needs_bytes -= msg_data.size();
      msg.append(QString::fromUtf8(msg_data));
      cout << "Potrzebuję wciąż: " << msg_needs_bytes << endl;

      if(msg_needs_bytes == 0){
          QString msgToReturn = msg;
          len_needs_bytes = 5;
          msg_len = "";
          msg = "";
          cout << "Przeczytałem całą wiadomość, oto ona: "<< endl << msgToReturn.toUtf8().constData() << endl;
          return msgToReturn;
      }
  }

  return "";
}



void Clilib::publish(QString tag, QString msg) {

    if (msg.isEmpty()) {msg = "[empty message]";}

    auto data = (tag + "/:" + msg + "\n").toUtf8();

    QString msg_size = QString("%1").arg(data.size(), 5, 10, QChar('0'));
    auto size = (msg_size).toUtf8();
    sock->write(size);

    sock->write(data);

}


void Clilib::subscribe(QString tag) {

    tags.insert(tag);

    auto data = ("#" + tag).toUtf8();

    QString msg_size = QString("%1").arg(data.size(), 5, 10, QChar('0'));
    auto size = (msg_size).toUtf8();
    sock->write(size);

    sock->write(data);

}

QString Clilib::someerror() {
    return sock->errorString();
}

void Clilib::closeConn() {
    if (sock->state() == QTcpSocket::ConnectedState) {
        sock->write((QString("%1").arg(9, 5, 10, QChar('0'))).toUtf8());
        sock -> write("closeconn");
        sock->close();
        tags.clear();
    }
}
