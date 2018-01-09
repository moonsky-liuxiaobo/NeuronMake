#ifndef SOCKETJS_H
#define SOCKETJS_H
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTcpSocket>
#include <QImage>
#include <windows.h>
#include <QDataStream>
#include <QDir>
#include <QTcpServer>
#include "readimage.h"

using namespace qri_neuron_lib;

class NeuronMake:public QObject
{
  Q_OBJECT
public:
  NeuronMake(QObject *parent=0);


public slots:
  bool readline();
  void acceptConnect();           //接受客户端连接
  void receiveMessage();          //接收消息
  void sendMessage(QString mes);  //发送消息

public:
  QTcpSocket *TcpSocket;
  QTcpServer *TcpServer;
  QByteArray imageData;
  readimage r;
};

#endif // SOCKETJS_H
