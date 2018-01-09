#include <QCoreApplication>
#include <iostream>
#include <QString>
#include <QDebug>
#include <QDir>
#include <QTime>
#include "neuron_make.h"

using namespace std;
using namespace qri_neuron_lib;

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QTime time;
  time.start();
  NeuronMake s;
  qDebug()<<time.elapsed()<<"ms";
  return a.exec();
}
