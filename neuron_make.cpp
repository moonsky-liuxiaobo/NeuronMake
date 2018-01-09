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

  TcpServer = new QTcpServer();
  TcpServer->listen(QHostAddress::Any,4001);
  connect(TcpServer,SIGNAL(newConnection()),this, SLOT(acceptConnect()));
}

void NeuronMake::acceptConnect(){
    //Returns the next pending connection as a connected QTcpSocket object.
  TcpSocket = TcpServer->nextPendingConnection();
  connect(TcpSocket,SIGNAL(readyRead()),this, SLOT(receiveMessage()));
  sendMessage("connect");
  Sleep(10);
}

void NeuronMake::receiveMessage(){
  //QByteArray data = TcpSocket->readAll();
  //QString str(data);
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
  QString str(backtextdate);
  qDebug()<<str;

  //backtextdate = "clean";
  //backtextdate = "save image/Neurons111.csv";
  //backtextdate = "loadNeurons image/Neurons.csv";
  //backtextdate = "initialize_B_S 16 16 8 8";
  //backtextdate = "initialize_MODE 0 0 2 90000000 2048";
  //backtextdate = "learn image/learn image/Neurons.csv 01";
  //backtextdate = "classify image/i/10_0009.bmp 02";

  //backtextdate = "initialize_quick 200 30";
  //backtextdate = "learn_Quick image/learn/1_face.bmp 9 0 0 0 00";
  //backtextdate = "classify_Quick image/Neurons.csv image/i/10_0009.bmp 02";

  QString information = backtextdate;
  QStringList LIST = information.split(" ");

  if(LIST[0]=="loadNeurons"){
    sendMessage("begin");
    QString loadpath = LIST[1];
    r.LoadEngine(loadpath.toLatin1().data());
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
    r.save(savepath);
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
      TcpSocket->write("100");
      TcpSocket->waitForBytesWritten();
      return 0;
    }
    if((LIST[3]=="00")|(LIST[3]=="01")){
      //step1：加载label文件，为cat赋值
      QString CatFilrPath = LIST[1] + "/label.txt";
      QFile CatFileTXT(CatFilrPath);
      if(!CatFileTXT.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"Can't open the label file!"<<endl;
        TcpSocket->write("101");//待学习的文件夹下没有label.txt文件
        TcpSocket->waitForBytesWritten();
        return 0;
      }
      while(!CatFileTXT.atEnd()){
        QString line = CatFileTXT.readLine();
        QStringList LineList = line.split(" ");
        if(LineList.length()!=5){
          qDebug()<<"one line was Error!";
          TcpSocket->write("102");//label文件中有一行参数错误
          TcpSocket->waitForBytesWritten();
          break;
        }
        QString S_Cat = LineList[1];
        QString S_R = LineList[2];
        QString S_G = LineList[3];
        QString S_B = LineList[4];
        qDebug()<<S_Cat.toInt()<<","<<S_R.toInt()<<","<<S_G.toInt()<<","<<S_B.toInt();
        r.label[r.CatCount].name = LineList[0];
        r.label[r.CatCount].cat = S_Cat.toInt();
        r.label[r.CatCount].R = S_R.toInt();
        r.label[r.CatCount].G = S_G.toInt();
        r.label[r.CatCount].B = S_B.toInt();
        r.CatCount++;
      }
#if 0
      for(int n=0; n<r.CatCount; n++){
        qDebug()<<"label["<<n<<"]:name"<<r.label[n].name;
        qDebug()<<"label["<<n<<"]:cat"<<r.label[n].cat;
        qDebug()<<"label["<<n<<"]:R"<<r.label[n].R;
        qDebug()<<"label["<<n<<"]:G"<<r.label[n].G;
        qDebug()<<"label["<<n<<"]:B"<<r.label[n].B;
      }
      qDebug()<<"CatCount:"<<r.CatCount;
#endif
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
      qDebug()<<"==============="<<SaveTxt;
      QFile FileTxt(SaveTxt);
      if(!FileTxt.open(QIODevice::WriteOnly|QIODevice::Text)){
        qDebug()<<"txt cant't be creation!";
        TcpSocket->write("103");//创建label.txt失败
        TcpSocket->waitForBytesWritten();
        return 0;
      }
      QTextStream WriteTxt(&FileTxt);
      int w = r.CatCount;
      for(int x=0; x<w; x++){
        WriteTxt<<r.label[x].name<<" "<<r.label[x].cat<<" "<<r.label[x].R<<" "<<r.label[x].G<<" "<<r.label[x].B<<endl;
      }
      FileTxt.close();
//      TcpSocket->write(QString::number(r.Engine.NeuronCount(), 10).toUtf8());
//      TcpSocket->waitForBytesWritten();
//      TcpSocket->write("learn_ok");
//      TcpSocket->waitForBytesWritten();
    }
    sendMessage("ok");
  }
  
  if(LIST[0]=="classify"){
    sendMessage("begin");
    if(LIST.length()!=3){
      qDebug()<<"Input Parameter Error!";
      TcpSocket->write("100");
      TcpSocket->waitForBytesWritten();
      return 0;
    }
    //step2：加载label
    QString SavePath = LIST[1];
    QString SaveTxt = SavePath.replace(SavePath.length()-3, 3, "txt");
    QFile CatFileTXT(SaveTxt);
    if(!CatFileTXT.open(QIODevice::ReadOnly | QIODevice::Text)) {
      qDebug()<<"Can't open the label file!"<<endl;
      TcpSocket->write("104");//无法读取label.txt文件
      TcpSocket->waitForBytesWritten();
      return 0;
    }
    while(!CatFileTXT.atEnd()){
      QString  line = CatFileTXT.readLine();
      QStringList LineList = line.split(" ");
      if(LineList.length()<1){
        qDebug()<<"one line was Error!";
        TcpSocket->write("102");//label文件中有一行参数错误
        TcpSocket->waitForBytesWritten();
        break;
      }
    }
#if 0
    for(int n=0; n<r.CatCount; n++){
      qDebug()<<"label["<<n<<"]:name"<<r.label[n].name;
      qDebug()<<"label["<<n<<"]:cat"<<r.label[n].cat;
      qDebug()<<"label["<<n<<"]:color"<<r.label[n].color;
    }
    qDebug()<<"CatCount:"<<r.CatCount;
#endif
    QString LoadPath = LIST[1];
    QString ClassifyPath = LIST[2];
    r.LoadEngine(LoadPath.toLatin1().data());
    if(LIST[5]=="02"){
      int result = r.F_classify_02(ClassifyPath.toLatin1().data());
      if(result==0){
        qDebug()<<"load image fault!";
        TcpSocket->write("105");//加载待识别的图片失败
        TcpSocket->waitForBytesWritten();
        return 0;
      }
    }
    sendMessage("ok");
#if 0
//      int hei = r.paintimage.height();
//      QString Hei = QString::number(hei);
//      QByteArray byte_height = Hei.toUtf8();
//      int wid = r.paintimage.width();
//      QString Wid = QString::number(wid);
//      QByteArray byte_width = Wid.toUtf8();

//      TcpSocket->write(byte_height);
//      TcpSocket->write(",");
//      TcpSocket->write(byte_width);
//      TcpSocket->waitForBytesWritten();

//      TcpSocket->write(r.X, r.index2);
//      TcpSocket->write(r.Y, r.index2);
//      TcpSocket->waitForBytesWritten();
//      qDebug()<<"byte_height:"<<byte_height;
//      qDebug()<<"byte_width :"<<byte_width;
//      qDebug()<<"r.index2   :"<<r.index2;
#endif
  }

  if(LIST[0]=="initialize_quick"){
    sendMessage("begin");
    if(LIST.length()!=3){
      qDebug()<<"Input Parameter Error!";
      TcpSocket->write("100");
      TcpSocket->waitForBytesWritten();
      return 0;
    }
    QString quick_x = LIST[1];
    QString quick_y = LIST[2];
    r.assignment_quick(quick_x.toInt(), quick_y.toInt());
    sendMessage("ok");
  }

  //backtextdate = "learn_Quick image/learn/1_face.bmp 9 0 0 0 03";
  if(LIST[0]=="learn_Quick"){
    sendMessage("begin");
    if(LIST.length()!=7){
      qDebug()<<"Input Parameter Error!";
      TcpSocket->write("100");
      TcpSocket->waitForBytesWritten();
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
    r.label[r.CatCount].name = quick_name;
    r.label[r.CatCount].cat = quick_cat.toInt();
    r.label[r.CatCount].R = quick_R.toInt();
    r.label[r.CatCount].G = quick_G.toInt();
    r.label[r.CatCount].B = quick_B.toInt();
    r.CatCount++;
    if(LIST[6]=="00"){
      r.F_learn_quick(quick_path, quick_cat.toInt());
    }
    sendMessage(QString::number(r.Engine.NeuronCount(), 10).toUtf8());
    sendMessage("ok");
  }

  //backtextdate = "classify_Quick image/i/10_0009.bmp 300 40 02";
  if(LIST[0]=="classify_Quick"){
    sendMessage("begin");
    if(LIST.length()!=3){
      qDebug()<<"Input Parameter Error!";
      TcpSocket->write("100");
      TcpSocket->waitForBytesWritten();
      return 0;
    }
    QString quick_img_path = LIST[1];
    QString quick_x = LIST[2];
    QString quick_y = LIST[3];
    if(LIST[0]=="00"){
      r.F_classify_quick(quick_img_path, quick_x.toInt(), quick_y.toInt());
    }
    sendMessage("ok");
  }
  return 0;

}
