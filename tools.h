#ifndef TOOLS_H
#define TOOLS_H
#include "pattern_classifier.h"
#include "pattern_trainer.h"

const int block = 12;
const int step = 6;

int   LoadEngine(const char* src_path,NeuronEngineFloat & engine);
int   PushFileToSamples(const QString & src_path,int cat, feature_engine::FeatureTrainer * trainer);

int PushFileToClassify(const QString &  src_path, feature_engine::FeatureClassifier * classifier,NeuronEngineFloat &  engine);
bool PushFileToClassify(const QString &  src_path, int target_cat ,feature_engine::FeatureClassifier * classifier,NeuronEngineFloat &  engine);

void  PrintEngine(const NeuronEngineFloat &  engine);
void  PrintNeuron(const NeuronFloat *nf);

bool F_learn(const QString imagepath, int cat, NeuronEngineFloat &  engine);
int F_classify(const QString imagepath, NeuronEngineFloat &  engine, QImage &paintimage);
void paint(int paint_start_x, int paint_start_y, QImage &paintimage);

#endif // TOOLS_H
