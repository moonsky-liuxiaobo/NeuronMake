#include "tools.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QImageReader>

using namespace feature_engine;

void PrintEngine(const NeuronEngineFloat &  engine){
  //dump all neurons
  printf("Neuron number is %d\n", engine.NeuronCount());
  for(int i=0;i<engine.NeuronCount();++i){
    const NeuronFloat * nf = engine.ReadNeuron(i);
    PrintNeuron(nf);
  }
}

void PrintNeuron(const NeuronFloat* nf){
  const int max_buffer_size=1024;
  float nf_buffer[max_buffer_size];
  printf("cat=%d, min_aif=%.2f, aif=%.2f, vector:",nf->Cat(),nf->MinAIF(),nf->Aif());
  int len = nf->ReadNeuronMem(nf_buffer,max_buffer_size);
  for(int k=0;k<len;++k){
    printf(" %.2f ",nf_buffer[k]);
  }
  printf("]\n");
}

int PushFileToSamples(const QString & src_path,int cat, feature_engine::FeatureTrainer * trainer){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<"Fail to open "<<src_path;
    return 0;
  }
  QTextStream src_text(&src_file);

  //step1. read and push adll data to the frame list
  src_text.readLine();
  int sample_size=0;

  QVector<float> sample_data;
  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    if(str_data.size()==0)continue;

    //step1 push raw data to engine
    sample_data.push_back(str_data.at(0).toFloat());
  }
  sample_size=trainer->PushToSample(cat,&sample_data[0],sample_data.size());

  src_file.close();
  return sample_size;
}
int PushFileToClassify(const QString &  src_path ,feature_engine::FeatureClassifier * classifier,NeuronEngineFloat &  engine){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<"Fail to open "<<src_path;
    return 0;
  }
  QTextStream src_text(&src_file);

  QVector<float> raw_data;

  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    if(str_data.size()==0)continue;

    //step1. push to data
    raw_data.push_back(str_data.at(0).toFloat());
  }

  int ret = classifier->Classify(engine,&raw_data[0],raw_data.size());
  src_file.close();
  return ret;
}
bool PushFileToClassify(const QString &  src_path, int target_cat ,feature_engine::FeatureClassifier * classifier,NeuronEngineFloat &  engine){
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    qDebug()<<"Fail to open "<<src_path;
    return 0;
  }
  QTextStream src_text(&src_file);

  QVector<float> raw_data;

  while(!src_text.atEnd()){
    QString str_line = src_text.readLine();
    str_line.replace('\"',"");
    QStringList str_data = str_line.split(",");
    if(str_data.size()==0)continue;

    //step1. push to data
    raw_data.push_back(str_data.at(0).toFloat());
//    qDebug()<<"str_data.at(0).toFloat():"<<str_data.at(0).toFloat();
  }

  int ret = classifier->Classify(engine,&raw_data[0],raw_data.size());
  src_file.close();
  return (ret==target_cat);
}
int LoadEngine(const char* src_path, NeuronEngineFloat & engine){
  if(src_path==NULL)return 0;
  QFile src_file(src_path);
  if(!src_file.open(QFile::ReadOnly)){
    printf("[%s,%d]Fail to open file %s",__FILE__,__LINE__,src_path);
    return 0;
  }
  QTextStream src_text(&src_file);

  //step1.reset engie
  engine.ResetEngine();

  engine.BeginRestoreMode();
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
    engine.RestoreNeuron(neuron_buffer,neuron_len,cat,aif,min_aif);
  }
  engine.EndRestoreMode();
  return engine.NeuronCount();
}

bool F_learn(const QString imagepath, int cat, NeuronEngineFloat &  engine){
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    qDebug()<<"load learn image path:"<<imagepath<<"failed!";
    return 0;
  }
  float Sub[block*block] = {};
  int index = 0;
  for(int i=0; i<loadimage.width()-block; i=i+step ){
    for(int j=0 ; j<loadimage.height()-block; j=j+step){
      QImage cutimage = loadimage.copy(i,j,block,block);
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
      engine.Learn(cat, Sub, block*block);
      Sub[block*block] = {};
    }
  }
  return 1;
}

int F_classify(const QString imagepath, NeuronEngineFloat & engine, QImage &paintimage){

  //QPixmap loadimage;
  QImage loadimage;
  QColor color;
  if(!loadimage.load(imagepath)){
    printf("load learn image path: %s failed!\n", imagepath.toStdString().data());
    return 0;
  }
  float Sub[block*block] = {};
  int index = 0;
  for(int i=0; i<loadimage.width()-block-1; i=i+step ){
    for(int j=0 ; j<loadimage.height()-block-1; j=j+step){
      QImage cutimage = loadimage.copy(i,j,block,block);
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
      if(engine.Classify(Sub, block*block)==1){
        paint(i, j, paintimage);
        qDebug()<<i<<"."<<j;
      }
      Sub[block*block] = {};
    }
  }
  return 1;
}

void paint(int paint_start_x, int paint_start_y, QImage & paintimage){
  QPoint point;
  const Q_DECL_UNUSED QRgb  RGB_MASK = 0x00ffffff;
  for(int i=paint_start_x; i<paint_start_x+block; i++){
    point.setX(i);
    point.setY(paint_start_y);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int i=paint_start_x; i<paint_start_x+block; i++){
    point.setX(i);
    point.setY(paint_start_y+block);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int j=paint_start_y; j<paint_start_y+block; j++){
    point.setX(paint_start_x);
    point.setY(j);
    paintimage.setPixel(point, RGB_MASK);
  }
  for(int j=paint_start_y; j<paint_start_y+block; j++){
    point.setX(paint_start_x+block);
    point.setY(j);
    paintimage.setPixel(point, RGB_MASK);
  }
}
