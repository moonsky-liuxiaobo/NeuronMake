#include <tooldebug.h>
using namespace  qri_neuron_lib;


void tooldebug::PrintDataframe(DataFrame & frame){
  printf("Dataframe: %d X %d Max rows:%d \n",frame.RowLength(),frame.Columns(),frame.Rows());
  for(int i=0;i<frame.Columns();i++){
    printf("[col_%d]\t",i);
    for(int j=0;j<frame.RowLength();j++){
      printf("%.2f, ",(float)frame.Read(j,i));
    }
    printf("\n");
  }
  printf("\n");
}


bool tooldebug::FillRandomDataframe(DataFrame * frame,float min, float max){
  if(frame == NULL )return false;

  const int data_cols=frame->Columns();
  float data_row[data_cols];
  for(int i=0;i<frame->Rows();++i){
    if(FillRandomSingleRow(data_row,data_cols,min,max)){
      frame->Push(data_row,data_cols);
    }
  }
  return true;
}


bool tooldebug::FillRandomSingleRow(float * buffer, int len, float min, float max){
  static int rand_seed=0;
  if(buffer==NULL || len==0)return false;
  int convert_max = static_cast<int>(max);
  int convert_min = static_cast<int>(min);
  if(convert_max<=convert_min){
    return false;
  }

  rand_seed += 1;
  srand(rand_seed);
  for(int i=0;i<len;++i){
    buffer[i] = static_cast<float>(rand()%(convert_max-convert_min)+convert_min);
  }
  return true;
}

bool tooldebug::SaveDataFrameList(std::vector<DataFrame*> framelist,const char * path){
  const int dataBufferLen=1024;
  float dataBuffer[dataBufferLen];

  FILE * file = fopen(path,"w");
  if(file==NULL){
    printf("[%s,%d]Fail to open the output file: %s\n",__FILE__,__LINE__,path);
    return false;
  }

  for(auto iter = framelist.begin();iter!=framelist.end();++iter){
    DataFrame * frame = *iter;
    if(frame==NULL){
      continue;
    }

    int outLen = frame->Serialize(dataBuffer,dataBufferLen);
    if(outLen<0){
      continue;
    }

    fprintf(file,"%d,",frame->category_);
    for(int i=0;i<outLen;++i){
      fprintf(file,"%.2f,",static_cast<float>(dataBuffer[i]));
    }
    fprintf(file,"\n");
  }
  fclose(file);
  return true;
}


void tooldebug::SaveEngine(NeuronEngine* engine,const char * path){
  const int neuron_buffer_size=1024;
  uint8_t neuron_buffer[neuron_buffer_size];

  FILE * file = fopen(path,"w");
  if(file==NULL){
    printf("[%s,%d]Fail to open the output file: %s\n",__FILE__,__LINE__,path);
    return;
  }

  for(int i=0; i<engine->NeuronCount();++i){
    const Neuron* ptrNeuron = engine->ReadNeuron(i);
    if(!ptrNeuron)break;

    fprintf(file,"%d,",ptrNeuron->Cat());
    fprintf(file,"%d,",ptrNeuron->MinAIF());
    fprintf(file,"%d,",ptrNeuron->Aif());
    int len = ptrNeuron->ReadNeuronMem(neuron_buffer,neuron_buffer_size);
    for(int k=0;k<len;++k){
      fprintf(file,"%d,",neuron_buffer[k]);
    }
    fprintf(file,"\n");
  }
  fclose(file);
  return;
}

void tooldebug::SaveEngineFloat(NeuronEngineFloat *engine, const char *path){
  const int neuron_buffer_size = engine->MaxNeuronMemLength();
  float neuron_buffer[neuron_buffer_size];

  FILE * file = fopen(path,"w");
  if(file==NULL){
    printf("[%s,%d]Fail to open the output file: %s\n",__FILE__,__LINE__,path);
    return;
  }

  for(int i=0; i<engine->NeuronCount();++i){
    const NeuronFloat* ptrNeuron = engine->ReadNeuron(i);
    if(!ptrNeuron)break;

    fprintf(file,"%d,",ptrNeuron->Cat());
    fprintf(file,"%f,",ptrNeuron->MinAIF());
    fprintf(file,"%f,",ptrNeuron->Aif());
    int len = ptrNeuron->ReadNeuronMem(neuron_buffer,neuron_buffer_size);
    for(int k=0;k<len;++k){
      fprintf(file,"%f,",neuron_buffer[k]);
    }
    fprintf(file,"\n");
  }
  fclose(file);
}
