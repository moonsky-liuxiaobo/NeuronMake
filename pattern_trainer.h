#ifndef FEATURETRAINER_H
#define FEATURETRAINER_H

#include "pattern_classifier.h"

namespace feature_engine {

  class FeatureTrainer: public QObject
  {
    Q_OBJECT
  public:
    static constexpr int DEFAULT_TRAINING_ITERATIONS=8;

    typedef struct sample_s{
      int cat;
      int size;
      float features[36];
    }sample_t;
    typedef QVector<sample_t> FeatureList;
    /*purpose:push one row of raw data to sample
     *input:
     * @cat, the category of this row
     * @row_data, one row of raw_data
     * @row_len, length of the row data
     *return:
     * if the new sample is accepted
     */
    virtual int PushToSample(int cat,float raw_data[],int raw_len);

    /*purpose: start training the raw data
     *input:
     * @engine, the engine for training
     * @iterations, repeat the training
     *return:
     * how many neurons are used
     */
    virtual int StartTraining(NeuronEngineFloat& engine, int iterations=DEFAULT_TRAINING_ITERATIONS);
    virtual void Reset();

  protected:
    //generate random index
    void GenerateRandomIndex(int array[], int len);
  public:
    FeatureList sample_list_;
    qri_neuron_lib::ComputeDownSample downsampler_;
  };

}

#endif // FEATUREENGINETRAINER_H
