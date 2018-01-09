#include "pattern_trainer.h"
#include "qrineuronlib.h"
#include <QDebug>

using namespace std;
using namespace feature_engine;
using namespace qri_neuron_lib;

int FeatureTrainer::PushToSample(int cat,float raw_data[],int raw_len){
  sample_t s;
  s.cat = cat;
  //process downsample
  memset(s.features,0,sizeof(float)*36);
  this->downsampler_.SetBlocks(36);
  s.size = this->downsampler_.Process(s.features,raw_data,raw_len);
  if(s.size){
    sample_list_.push_back(s);
  }
  return sample_list_.size();
}
int FeatureTrainer::StartTraining(NeuronEngineFloat &engine, int iterations){
  if(iterations<=0)return 0;

  //step1. reset engine
  engine.ResetEngine();


  //step3. start iterations
  for(int iter=0;iter<iterations;++iter){
    //step1. gerenate rand array
    const int rand_size = sample_list_.size();
    int rand_array[rand_size];
    GenerateRandomIndex(rand_array,rand_size);

    //step2.train the frame
    for(int i=0;i<rand_size;++i){
      int index = rand_array[i];
      sample_t s = sample_list_.at(index);

      //train the vector
      engine.Learn(s.cat,s.features,s.size);
    }
  }

  return engine.NeuronCount();
}
void FeatureTrainer::GenerateRandomIndex(int array[], int len){
  static unsigned long rand_counter=0;

  for(int i=0;i<len;i++){
      array[i]=i;
  }
  for(int i=len-1;i>0;i--){
      srand(rand_counter+time(NULL));
      rand_counter+=1;
      int index=rand()%(i+1);
      if(index!=i){
          uint8_t tmp = array[i];
          array[i] = array[index];
          array[index] = tmp;
      }
  }
}
void FeatureTrainer::Reset(){
  sample_list_.clear();
}
