#ifndef NEURONMAKE_H
#define NEURONMAKE_H
#include <QObject>

class NeuronMake:public QObject {
  Q_OBJECT
public:
  NeuronMake(QObject *parent=0);
};

#endif // NEURONMAKE_H
