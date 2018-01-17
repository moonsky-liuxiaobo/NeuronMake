#include "neuron_make.h"
#include <time.h>
#include <QTime>

NeuronMake::NeuronMake(QObject *parent)
  :QObject(parent)
{
  TcpSocket = new QTcpSocket();
//  qDebug()<<QObject::tr("client初始化...");
//  TcpSocket->abort();
//  TcpSocket->connectToHost("127.0.0.1", 3001);
//  if(!TcpSocket->isOpen()){
//    qDebug()<<QObject::tr("连接失败！");
//  }
//  connect(TcpSocket, SIGNAL(readyRead()), this, SLOT(readline()));
  qDebug()<<"run...";
  TcpServer = new QTcpServer();
  TcpServer->listen(QHostAddress::Any,4001);
  connect(TcpServer,SIGNAL(newConnection()),this, SLOT(acceptConnect()));
}

void NeuronMake::acceptConnect(){
  //connected QTcpSocket object.
  TcpSocket = TcpServer->nextPendingConnection();
  connect(TcpSocket,SIGNAL(readyRead()),this, SLOT(receiveMessage()));
  sendMessage("connect");
  Sleep(10);
}

void NeuronMake::receiveMessage(){
  readline();
}

void NeuronMake::sendMessage(QString mes){
  TcpSocket->write(mes.toUtf8());
  Sleep(10);
  TcpSocket->waitForBytesWritten();
}

bool NeuronMake::readline(){
  QByteArray backtextdate;
  backtextdate = TcpSocket->readLine();
  //backtextdate = "clean";
  //backtextdate = "save image/Neurons111.csv";
  //backtextdate = "loadNeurons image/Neurons.csv";
  //backtextdate = "initialize_B_S 16 16 8 8";
  //backtextdate = "initialize_MODE 0 0 2 90000000 2048";
  //backtextdate = "learn image/learn image/Neurons.csv 00";
  //backtextdate = "classify image/i/1.bmp 00";

  //backtextdate = "initialize_quick 200 30";
  //backtextdate = "learn_Quick image/learn/1_face.bmp 9 0 0 0 00";
  //backtextdate = "classify_Quick image/Neurons.csv image/i/10_0009.bmp 00";

  //backtextdate = "learn_Quick_file image/learn/1_face.bmp 9 0 0 0 00";
  //backtextdate = "classify_Quick_file image/Neurons.csv image/i/10_0009.bmp 00"

  QString information = backtextdate;
  QStringList LIST = information.split(" ");
  qDebug()<<LIST.length();

  if(LIST[0]=="print_struct"){
    for(int i=0; i<r.CatCount; i++){
      qDebug()<<"name:"<<r.label[i].name<<",cat:"<<r.label[i].cat<<",R:"<<r.label[i].R<<",G:"<<r.label[i].G<<",B:"<<r.label[i].B;
    }
  }

  if(LIST[0]=="print_Neuron"){

  }

  if(LIST[0]=="loadNeurons"){
    sendMessage("begin");
    QString loadpath = LIST[1];
    QString SaveTxt = loadpath.replace(loadpath.length()-3, 3, "txt");
    r.LoadEngine(loadpath.toLatin1().data());
    r.loadlabel(SaveTxt);
    sendMessage("ok");
  }

  if(LIST[0]=="clean"){
    sendMessage("begin");
    r.clean();
    sendMessage("ok");
  }

  if(LIST[0]=="save"){
    sendMessage("begin");
    QString savepath = LIST[1];
    QString savetxt = savepath.replace(savepath.length()-3, 3, "txt");
    r.save(savepath);
    r.savelabel(savetxt);
    sendMessage("ok");
  }

  if(LIST[0]=="initialize_B_S"){
    sendMessage("begin");
    if(LIST.length()!=5){
      qDebug()<<"Input Parameter Error!";
      TcpSocket->write("100");
      TcpSocket->waitForBytesWritten();
      return 0;
    }
    QString B_x = LIST[1];
    QString B_y = LIST[2];
    QString S_x = LIST[3];
    QString S_y = LIST[4];
    r.assignment_B_S(B_x.toInt(), B_y.toInt(), S_x.toInt(), S_y.toInt());
    sendMessage("ok");
  }

  if(LIST[0]=="initialize_MODE"){
    sendMessage("begin");
    if(LIST.length()!=6){
      qDebug()<<"Input Parameter Error!";
      TcpSocket->write("100");
      TcpSocket->waitForBytesWritten();
      return 0;
    }
    QString mude = LIST[1];
    QString norm = LIST[2];
    QString minAif = LIST[3];
    QString maxAif = LIST[4];
    QString neuron_memlen = LIST[5];
    r.assignment_AIF_MODE(mude.toInt(), norm.toInt(), minAif.toInt(), maxAif.toInt(), neuron_memlen.toInt());
    sendMessage("ok");
  }

  if(LIST[0]=="learn"){
    sendMessage("begin");
    if(LIST.length()!=4){
      qDebug()<<"Input Parameter Error!";
      sendMessage("100");
      return 0;
    }
    if((LIST[3]=="00")|(LIST[3]=="01")){
      //step1：加载label文件，为cat赋值
      QString CatFilrPath = LIST[1] + "/label.txt";
      int re = r.loadlabel(CatFilrPath);
      if(re!=1){
        sendMessage(QString::number(re));
      }
      //step2：加载目标文件夹，遍历每一个文件
      QDir dir(LIST[1]);
      dir.setFilter(QDir::Files|QDir::Hidden|QDir::NoSymLinks);
      QFileInfoList list = dir.entryInfoList();
      for (int i=0; i<list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QStringList Path_Cat = fileInfo.fileName().split('_');
        //如果命名不合法则不执行
        if(Path_Cat.length()!=2){
          qDebug()<<"illegal!";
          break;
        }
        QString Path_Cat_ = Path_Cat[1];
        QStringList Path_Cat__ = Path_Cat_.split('.');
        QString Cat = Path_Cat__[0];
        int cat_ = 0;
        //step3:遍历为cat_赋值
        for(int n=0; n<r.CatCount; n++){
          if(Cat==r.label[n].name) cat_=r.label[n].cat;
        }
        //step4:开始学习
        if(cat_!=0){
          qDebug()<<fileInfo.filePath();
          if(LIST[3]=="00")  r.F_learn_00(fileInfo.filePath(), cat_);
          if(LIST[3]=="01")  r.F_learn_01(fileInfo.filePath(), cat_);
        }
      }
      //step5：保存csv文件
      QString SavePath = LIST[2];
      r.save(SavePath);
      //step6：保存txt文件
      QString SaveTxt = SavePath.replace(SavePath.length()-3, 3, "txt");
      qDebug()<<"===============>>"<<SaveTxt;
      QFile FileTxt(SaveTxt);
      if(!FileTxt.open(QIODevice::WriteOnly|QIODevice::Text)){
        qDebug()<<"txt cant't be creation!";
        sendMessage("103");//创建label.txt失败
        return 0;
      }
      QTextStream WriteTxt(&FileTxt);
      int w = r.CatCount;
      for(int x=0; x<w; x++){
        WriteTxt<<r.label[x].name<<" "<<r.label[x].cat<<" "<<r.label[x].R<<" "<<r.label[x].G<<" "<<r.label[x].B<<endl;
      }
      FileTxt.close();
    }
    sendMessage("ok");
  }
  
  if(LIST[0]=="classify"){
    sendMessage("begin");
    if(LIST.length()!=3){
      qDebug()<<"Input Parameter Error!";
      sendMessage("100");
      return 0;
    }
    QString ClassifyPath = LIST[1];
    //r.LoadEngine(LoadPath.toLatin1().data());
    if(LIST[2]=="00"){
      int result = r.F_classify_00(ClassifyPath.toLatin1().data());
      if(result==0){
        qDebug()<<"load image fault!";
        sendMessage("105");//加载待识别的图片失败
        return 0;
      }
    }
    if(LIST[2]=="01"){
      int result = r.F_classify_01(ClassifyPath.toLatin1().data());
      if(result==0){
        qDebug()<<"load image fault!";
        sendMessage("105");//加载待识别的图片失败
        return 0;
      }
    }
    sendMessage("ok");
  }

  if(LIST[0]=="initialize_quick"){
    sendMessage("begin");
    if(LIST.length()!=3){
      qDebug()<<"Input Parameter Error!";
      sendMessage("100");
      return 0;
    }
    QString quick_x = LIST[1];
    QString quick_y = LIST[2];
    r.assignment_quick(quick_x.toInt(), quick_y.toInt());
    sendMessage("ok");
  }

  if(LIST[0]=="learn_Quick"){
    sendMessage("begin");
    if(LIST.length()!=7){
      qDebug()<<"Input Parameter Error!";
      sendMessage("100");
      return 0;
    }
    QString quick_path = LIST[1];
    QString quick_cat = LIST[2];
    QString quick_R = LIST[3];
    QString quick_G = LIST[4];
    QString quick_B = LIST[5];

    QStringList name_List = quick_path.split("/");
    QString quick_name_S = name_List[name_List.length()-1];
    QStringList name_ = quick_name_S.split(".");
    QString quick_name = name_[0];
    bool New = true;
    qDebug()<<"quick_name:"<<quick_name;
    qDebug()<<"quick_cat:"<<quick_cat;
    for(int b=0; b<r.CatCount; b++){
      if(quick_cat.toInt()==r.label[b].cat) New = false;
    }
    if(New){
      r.label[r.CatCount].name = quick_name;
      r.label[r.CatCount].cat = quick_cat.toInt();
      r.label[r.CatCount].R = quick_R.toInt();
      r.label[r.CatCount].G = quick_G.toInt();
      r.label[r.CatCount].B = quick_B.toInt();
      r.CatCount++;
    }

    if(LIST[6]=="00"){
      r.F_learn_quick_00(quick_path, quick_cat.toInt());
    }
    if(LIST[6]=="01"){
      r.F_learn_quick_01(quick_path, quick_cat.toInt());
    }
    if(LIST[6]=="02"){
      r.F_learn_quick_02(quick_path, quick_cat.toInt());
    }
    sendMessage(QString::number(r.Engine.NeuronCount(), 10).toUtf8());
    sendMessage("ok");
  }

  //backtextdate = "classify_Quick image/i/1.bmp 30 40 00";
  if(LIST[0]=="classify_Quick"){
    sendMessage("begin");
    if(LIST.length()!=5){
      qDebug()<<"Input Parameter Error!";
      TcpSocket->write("100");
      TcpSocket->waitForBytesWritten();
      return 0;
    }
    QString quick_img_path = LIST[1];
    QString quick_x = LIST[2];
    QString quick_y = LIST[3];
    if(LIST[4]=="00"){
      r.F_classify_quick_00(quick_img_path, quick_x.toInt(), quick_y.toInt());
    }
    if(LIST[4]=="01"){
      r.F_classify_quick_01(quick_img_path, quick_x.toInt(), quick_y.toInt());
    }
    sendMessage("ok");
  }
  return 0;
}
