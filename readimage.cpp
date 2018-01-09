#include <QColor>
#include <QDebug>
#include <QRgb>
#include <QPoint>
#include "readimage.h"

using namespace qri_neuron_lib;


readimage::readimage()
{
  block_x = 16;
  block_y = 16;
  step_x = 10;
  step_y = 10;

  quick_block_x = 0;
  quick_block_y = 0;
  ori_quick_x = 0;
  ori_quick_y = 0;

  memset(X, 0, 500);
  memset(Y, 0, 500);
  index2 = 0;
  CatCount = 0;   //cat计数器

//  typedef struct _block_info
//  {
//    int ID;
//    int x;
//    int y;
//    //float info[block_x*block_y];
//  }block_info;
}

void readimage::assignment_B_S(int b_x, int b_y, int s_x, int s_y){
  qDebug()<<"b_x:"<<b_x<<",b_y:"<<b_y<<",s_x:"<<s_x<<",s_y:"<<s_y;
  block_x = b_x;
  block_y = b_y;
  step_x = s_x;
  step_y = s_y;
}

void readimage::assignment_quick(int q_x, int q_y){
  ori_quick_x = q_x;
  ori_quick_y = q_y;
  while(q_x*q_y>2014){
    q_x = q_x/2;
    q_y = q_y/2;
  }
  quick_block_x = q_x;
  quick_block_y = q_y;
  qDebug()<<"quick_block_x:"<<quick_block_x<<",quick_block_y:"<<quick_block_y;
}

void readimage::assignment_AIF_MODE(int mode, int norm, float minaif, float maxaif, int max_neuron_memlen){
  /*
   * NORM_L1=0
   * NORM_LSUP=1
   * MODE_RBF=0
   * MODE_KNN=1
   */
  qDebug()<<"mode:"<<mode<<",norm:"<<norm<<",minaif:"<<minaif<<",maxaif:"<<maxaif<<",max_neuron_memlen:"<<max_neuron_memlen;
  Engine.Begin(mode,norm,minaif,maxaif,max_neuron_memlen);
}

void readimage::clean(){
  block_x = 16;
  block_y = 16;
  step_x = 10;
  step_y = 10;
  CatCount = 0;
  Engine.ResetEngine();
}

void readimage::paint(int paint_start_x, int paint_start_y, int R, int G, int B){
  QPoint point;
  const Q_DECL_UNUSED QRgb  RGB_MASK = qRgb(R, G, B);;
  qDebug()<<paint_start_x<<","<<paint_start_y;
  for(int i=paint_start_x; i<paint_start_x+block_x; i++){
    point.setX(i);
    point.setY(paint_start_y);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int i=paint_start_x; i<paint_start_x+block_x; i++){
    point.setX(i);
    point.setY(paint_start_y+block_y);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int j=paint_start_y; j<paint_start_y+block_y; j++){
    point.setX(paint_start_x);
    point.setY(j);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int j=paint_start_y; j<paint_start_y+block_y; j++){
    point.setX(paint_start_x+block_x);
    point.setY(j);
    paintimage.setPixel(point, RGB_MASK);
  }
}

void readimage::paint_quick(int paint_start_x, int paint_start_y,int ori_x, int ori_y, int R, int G, int B){
  QPoint point;
  const Q_DECL_UNUSED QRgb  RGB_MASK = qRgb(R, G, B);;
  qDebug()<<paint_start_x<<","<<paint_start_y;
  for(int i=paint_start_x; i<paint_start_x+ori_x; i++){
    point.setX(i);
    point.setY(paint_start_y);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int i=paint_start_x; i<paint_start_x+ori_x; i++){
    point.setX(i);
    point.setY(paint_start_y+ori_y);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int j=paint_start_y; j<paint_start_y+ori_y; j++){
    point.setX(paint_start_x);
    point.setY(j);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int j=paint_start_y; j<paint_start_y+ori_y; j++){
    point.setX(paint_start_x+ori_y);
    point.setY(j);
    paintimage.setPixel(point, RGB_MASK);
  }
}

void readimage::save(const QString & dst_path){
  //const QString & dst_path = "Neurons.csv";
  tooldebug t;
  t.SaveEngineFloat(&Engine, dst_path.toLatin1().data());
}

int readimage::LoadEngine(const char* src_path){
  if(src_path==NULL)return 0;
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("[%s,%d]Fail to open file %s",__FILE__,__LINE__,src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  //step1.reset engine
  Engine.ResetEngine();

  Engine.BeginRestoreMode();
  while(!src_text.atEnd()){
    QString str_record = src_text.readLine();
    QStringList str_list = str_record.split(',');
    if(str_list.size()<3)continue;

    //read cat,min_aif, aif
    int cat =  str_list[0].toInt();
    int min_aif = str_list[1].toFloat();
    int aif = str_list[2].toFloat();
    //read buffer
    const int neuron_size=1024;
    float neuron_buffer[neuron_size];
    int neuron_len=0;
    for(int i=0;i<str_list.size()-3;++i){
      neuron_buffer[i]=str_list[3+i].toFloat();
      ++neuron_len;
    }

    //restore
    Engine.RestoreNeuron(neuron_buffer,neuron_len,cat,aif,min_aif);
  }
  Engine.EndRestoreMode();
  return Engine.NeuronCount();
}

int readimage::F_learn_00(const QString imagepath, int cat){
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    qDebug()<<"load learn image path:"<<imagepath<<"failed!";
    return 0;
  }
  float Sub[block_x*block_y];
  memset(Sub, 0, block_x*block_y);
  int index = 0;
  for(int i=0; i<loadimage.width()-block_x; i=i+step_x ){
    for(int j=0 ; j<loadimage.height()-block_y; j=j+step_y){
      QImage cutimage = loadimage.copy(i,j,block_x,block_y);
      for(int m=0; m<cutimage.width(); m++){
        for(int n=0; n<cutimage.height(); n++){
          color = QColor(cutimage.pixel(m,n));
          //Subample
          int greylevel = (color.red()+color.green()+color.blue());
          Sub[index] = greylevel;
          index++;
        }
      }
      index = 0;
      Engine.Learn(cat, Sub, block_x*block_y);
      memset(Sub, 0, block_x*block_y);
    }
  }
  return 1;
}

int readimage::F_classify_00(const QString imagepath){
  //step：load image;
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    printf("load learn image path: %s failed!\n", imagepath.toStdString().data());
    return 0;
  }
  if(!paintimage.load(imagepath)){
    printf("paint learn image path: %s failed!\n", imagepath.toStdString().data());
    return 0;
  }
  float Sub[block_x*block_y];
  memset(Sub, 0, block_x*block_y);
  int index1 = 0;

#if 0
  qDebug()<<"width:   "<<loadimage.width();
  qDebug()<<"height:  "<<loadimage.height();
  qDebug()<<"block_x: "<<block_x;
  qDebug()<<"block_y: "<<block_y;
  qDebug()<<"step_x:  "<<step_x;
  qDebug()<<"step_y:  "<<step_y;
#endif

  for(int i=0; i<loadimage.width()-block_x; i=i+step_x){
    for(int j=0 ; j<loadimage.height()-block_y; j=j+step_y){
      QImage cutimage = loadimage.copy(i,j,block_x,block_y);
      for(int m=0; m<cutimage.width(); m++){
        for(int n=0; n<cutimage.height(); n++){
          color = QColor(cutimage.pixel(m,n));
          //Subample
          int greylevel = (color.red()+color.green()+color.blue());
          Sub[index1] = greylevel;
          index1++;
        }
      }
      index1 = 0;
      int cat = Engine.Classify(Sub, block_x*block_y);
      for(int n=0; n<CatCount; n++){
        if((cat==label[n].cat)&&(cat!=1)){
          paint(i, j, label[n].R, label[n].G, label[n].B);
        }
      }
      memset(Sub, 0, block_x*block_y);
    }
  }
  return 1;
}

int readimage::F_learn_01(const QString imagepath, int cat){
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    qDebug()<<"load learn image path:"<<imagepath<<"failed!";
    return 0;
  }
  float Sub[block_x*block_y*3];
  memset(Sub, 0, block_x*block_y*3);
  int index = 0;
  for(int i=0; i<loadimage.width()-block_x; i=i+step_x){
    for(int j=0 ; j<loadimage.height()-block_y; j=j+step_y){
      QImage cutimage = loadimage.copy(i,j,block_x,block_y);
      for(int m=0; m<cutimage.width(); m++){
        for(int n=0; n<cutimage.height(); n++){
          color = QColor(cutimage.pixel(m,n));
          //Subample
          Sub[index] = color.red();
          Sub[index+block_x*block_y] = color.green();
          Sub[index+block_x*block_y*2] = color.blue();
          index++;
        }
      }
      index = 0;
      Engine.Learn(cat, Sub, block_x*block_y*3);
      memset(Sub, 0, block_x*block_y*3);
    }
  }
  return 1;
}

int readimage::F_classify_01(const QString imagepath){
  //step：load image;
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    printf("load learn image path: %s failed!\n", imagepath.toStdString().data());
    return 0;
  }
  if(!paintimage.load(imagepath)){
    printf("paint learn image path: %s failed!\n", imagepath.toStdString().data());
    return 0;
  }
  float Sub[block_x*block_y*3];
  memset(Sub, 0, block_x*block_y*3);
  int index = 0;

#if 0
  qDebug()<<"width:   "<<loadimage.width();
  qDebug()<<"height:  "<<loadimage.height();
  qDebug()<<"block_x: "<<block_x;
  qDebug()<<"block_y: "<<block_y;
  qDebug()<<"step_x:  "<<step_x;
  qDebug()<<"step_y:  "<<step_y;
#endif

  for(int i=0; i<loadimage.width()-block_x; i=i+step_x ){
    for(int j=0 ; j<loadimage.height()-block_y; j=j+step_y){
      QImage cutimage = loadimage.copy(i,j,block_x,block_y);
      for(int m=0; m<cutimage.width(); m++){
        for(int n=0; n<cutimage.height(); n++){
          color = QColor(cutimage.pixel(m,n));
          //Subample
          Sub[index] = color.red();
          Sub[index+block_x*block_y] = color.green();
          Sub[index+block_x*block_y*2] = color.blue();
          index++;
        }
      }
      index = 0;
      int cat = Engine.Classify(Sub, block_x*block_y*3);
      for(int n=0; n<CatCount; n++){
        if((cat==label[n].cat)&&(cat!=1)){
          paint(i, j, label[n].R, label[n].G, label[n].B);
        }
      }
      memset(Sub, 0, block_x*block_y*3);
    }
  }
  return 1;
}

int readimage::F_learn_02(const QString imagepath, int cat){
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    qDebug()<<"load learn image path:"<<imagepath<<"failed!";
    return 0;
  }
  float Sub[block_x*block_y];
  memset(Sub, 0, block_x*block_y);
  int index = 0;
  float index_grey = 0;
  QImage image = loadimage.scaled(100, 100);
  float h = image.height()/(block_x-1);
  float w = image.width()/(block_y-1);
  qDebug()<<"H:"<<h<<",W:"<<w;
  for(int i=0; i<image.width(); i=i+h ){
    for(int j=0 ; j<image.height(); j=j+w){
      QImage cutimage = image.copy(i,j,h,w);
      for(int m=0; m<cutimage.width(); m++){
        for(int n=0; n<cutimage.height(); n++){
          color = QColor(cutimage.pixel(m,n));
          int grey = (color.red()+color.green()+color.blue())/3;
          if(grey!=37)index_grey++;
        }
      }
      Sub[index] = index_grey/(h*w)*100;
      index_grey = 0;
      index++;
    }
  }
  Engine.Learn(cat, Sub, block_x*block_y);
  memset(Sub, 0, block_x*block_y);
  return 1;
}

int readimage::F_classify_02(const QString imagepath){
  //step：load image;
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    printf("load learn image path: %s failed!\n", imagepath.toStdString().data());
    return 0;
  }
  float Sub[block_x*block_y];
  memset(Sub, 0, block_x*block_y);
  int index = 0;
  float index_grey = 0;
  QImage image = loadimage.scaled(100, 100);
  float h = image.height()/(block_x-1);
  float w = image.width()/(block_y-1);
  qDebug()<<"H:"<<h<<",W:"<<w;
  for(int i=0; i<image.width(); i=i+h){
    for(int j=0 ; j<image.height(); j=j+w){
      QImage cutimage = image.copy(i,j,h,w);
      for(int m=0; m<cutimage.width(); m++){
        for(int n=0; n<cutimage.height(); n++){
          color = QColor(cutimage.pixel(m,n));
          int grey = (color.red()+color.green()+color.blue())/3;
          if(grey!=37)index_grey++;
        }
      }
      Sub[index] = index_grey/(h*w)*100;
      index_grey = 0;
      index++;
    }
  }
  return Engine.Classify(Sub, block_x*block_y);
}

int readimage::F_learn_quick(const QString imagepath, int cat){
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    qDebug()<<"load learn image path:"<<imagepath<<"failed!";
    return 0;
  }

  QImage image = loadimage.scaled(quick_block_x, quick_block_y);
  float Sub[quick_block_x*quick_block_y];
  memset(Sub, 0, quick_block_x*quick_block_y);
  int index = 0;
  for(int i=0; i<image.width(); i++){
    for(int j=0 ; j<image.height(); j++){
      color = QColor(image.pixel(i,j));
      int grey = (color.red()+color.green()+color.blue())/3;
      Sub[index] = grey;
      index++;
    }
  }
  int number = Engine.Learn(cat, Sub, quick_block_x*quick_block_y);
  memset(Sub, 0, quick_block_x*quick_block_y);
  return number;
}

int readimage::F_classify_quick(const QString imagepath, int quick_step_X, int quick_step_Y){
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    printf("load learn image path: %s failed!\n", imagepath.toStdString().data());
    return 0;
  }
  float Sub[quick_block_x*quick_block_y];
  memset(Sub, 0, quick_block_x*quick_block_y);

  for(int i=0; i<loadimage.width(); i=i+quick_step_X){
    for(int j=0 ; j<loadimage.height(); j=j+quick_step_Y){
      QImage cutimage = loadimage.copy(i,j,quick_block_x,quick_block_x);
      int index=0;
      for(int m=0; m<cutimage.width(); m++){
        for(int n=0; n<cutimage.height(); n++){
          color = QColor(cutimage.pixel(m,n));
          int grey = (color.red()+color.green()+color.blue())/3;
          Sub[index] = grey;
          index++;
        }
      }
      int cat = Engine.Classify(Sub, quick_block_x*quick_block_x);
      for(int n=0; n<CatCount; n++){
        if((cat==label[n].cat)&&(cat!=0)){
          paint_quick(i, j, ori_quick_x, ori_quick_y, label[n].R, label[n].G, label[n].B);
        }
      }
    }
  }
  return 1;
}
