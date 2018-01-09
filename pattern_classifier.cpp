#include "pattern_classifier.h"
#include <stdlib.h>
#include <QDebug>

using namespace std;
using namespace qri_neuron_lib;
using namespace feature_engine;

int FeatureClassifier::Classify(NeuronEngineFloat & engine,float raw_data[], int raw_len){
  static float features[36];
  int len = this->downsample_.Process(features,raw_data,raw_len);
  if(len){
    return engine.Classify(features,len);
  }
  return 0;
}
