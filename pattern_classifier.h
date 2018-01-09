#ifndef DATAFRAMEFEATUREEXTRACTOR_H
#define DATAFRAMEFEATUREEXTRACTOR_H

#include <QObject>
#include <QVector>
#include "qrineuronlib.h"

namespace feature_engine {
  class FeatureClassifier: public QObject{
    Q_OBJECT
  public:
    virtual int Classify(NeuronEngineFloat & engine,float raw_data[],int raw_len);
  protected:
    qri_neuron_lib::ComputeDownSample downsample_;
  };
}
#endif // DATAFRAMEFEATUREEXTRACTOR_H
