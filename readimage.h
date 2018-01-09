#ifndef READIMAGE_H
#define READIMAGE_H

#include <QString>
#include <QImage>
#include <QFile>
#include "qrineuronlib.h"
#include "tooldebug.h"
//#include "tools.h"

using namespace qri_neuron_lib;

class readimage
{

public:
    int block_x;
    int block_y;
    int step_x;
    int step_y;

    int quick_block_x;
    int quick_block_y;
    int ori_quick_x;
    int ori_quick_y;

    readimage();
    NeuronEngineFloat Engine;
    QImage paintimage;

    int F_learn_00(const QString imagepath, int cat);
    int F_classify_00(const QString imagepath);
    int F_learn_01(const QString imagepath, int cat);
    int F_classify_01(const QString imagepath);
    int F_learn_02(const QString imagepath, int cat);
    int F_classify_02(const QString imagepath);
    int F_learn_quick(const QString imagepath, int cat);
    int F_classify_quick(const QString imagepath, int quick_step_X, int quick_step_Y);
    /* 00:以block，step为基准的灰度值学习
     * 01:以block，step为基准的RGB值学习
     *    一张图片生成多个Neuron，可识别一张图片里的多个不同物体
     * 02:以不同颜色所占每个block的比例为基准的学习
     *    一张图片生成一个Neuron，只能识别一张图片的类型
     * quick:
     *
     */
    void assignment_quick(int q_x, int q_y);
    void assignment_B_S(int b_x, int b_y, int s_x, int s_y);
    void assignment_AIF_MODE(int mode, int norm, float minaif, float maxaif, int max_neuron_memlen);
    void clean();

    int LoadEngine(const char* src_path);
    void paint(int paint_start_x, int paint_start_y, int R, int G, int B);
    void paint_quick(int paint_start_x, int paint_start_y,int ori_x, int ori_y, int R, int G, int B);
    void save(const QString & dst_path);

    char X[500];
    char Y[500];
    int index2;

    typedef struct labeltxt
    {
      QString name;
      int cat;
      int R;
      int G;
      int B;
    }Label;
    Label label[50];
    int CatCount;   //cat计数器


};

#endif // READIMAGE_H
