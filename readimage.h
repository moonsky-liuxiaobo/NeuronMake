#ifndef READIMAGE_H
#define READIMAGE_H

#include <QString>
#include <QImage>
#include <QFile>
#include <QDir>
#include "qrineuronlib.h"
#include "tooldebug.h"

using namespace qri_neuron_lib;

class readimage
{
public:
    //自由模式：滑动框的大小
    int block_x;
    int block_y;
    //自由模式：滑动框的步长
    int step_x;
    int step_y;
    //固定模式：初始化画框的大小
    int ori_quick_x;
    int ori_quick_y;
    //固定模式：00 learn_classify_quick_01 的参数
    int quick_block_x;
    int quick_block_y;
    //固定模式：01 用于分割最小的quick学习识别
    int quick_rect_01;
    //固定模式：01 每行按照最小rect分割的个数
    int quick_rect_x_number;
    int quick_rect_y_number;
    //固定模式：01 学习或者识别的数组长度，由初始化的画框和quick_rect_01所决定
    int array_len_01;

    readimage();
    NeuronEngineFloat Engine;
    QImage paintimage;
    //通用:初始化
    void assignment_AIF_MODE(int mode, int norm, float minaif, float maxaif, int max_neuron_memlen);
    //通用:清除
    void clean();
    //通用:加载知识
    int LoadEngine(const char* src_path);
    //通用:绘制图片，block固定大小
    void paint(int paint_start_x, int paint_start_y, int R, int G, int B);
    //通用:绘制图片，任意大小
    void paint_quick(int paint_start_x, int paint_start_y,int ori_x, int ori_y, int R, int G, int B);
    //通用:保存知识
    void save(const QString & dst_path);
    //通用:保存结构体
    int savelabel(const QString &lab_path);
    //通用:加载结构体
    int loadlabel(const QString &lab_path);
    //通用:打印知识
    int printNeuron();

    //自由模式：初始化block，step
    void assignment_B_S(int b_x, int b_y, int s_x, int s_y);
    /* 自由模式：
     * 00:以block，step为基准的灰度值学习
     * 01:以block，step为基准的RGB值学习
     *    一张图片生成多个Neuron，可识别一张图片里的多个不同物体
     * 02:以不同颜色所占每个block的比例为基准的学习
     *    一张图片生成一个Neuron，只能识别一张图片的类型
     * quick:
     *
     */
    int F_learn_00(const QString imagepath, int cat);
    int F_classify_00(const QString imagepath);
    int F_learn_01(const QString imagepath, int cat);
    int F_classify_01(const QString imagepath);
    int F_learn_02(const QString imagepath, int cat);
    int F_classify_02(const QString imagepath);

    //固定模式：初始化画框
    void assignment_quick(int q_x, int q_y);
    /*固定模式
     * 00：采用Qt自带的scaled函数模糊处理图片
     * 01：采用像素点分块，计算每块的像素的统计特征
     */
    int F_learn_quick_00(const QString imagepath, int cat);
    int F_classify_quick_00(const QString imagepath, int quick_step_X, int quick_step_Y);
    int F_learn_quick_01(const QString imagepath, int cat);
    int F_classify_quick_01(const QString imagepath, int quick_step_X, int quick_step_Y);
    int F_learn_quick_02(const QString imagepath, int cat);
    int F_classify_quick_02(const QString imagepath, int quick_step_X, int quick_step_Y);
    //保存学习的name，cat，RGB值
    typedef struct labeltxt
    {
      QString name;
      int cat;
      int R;
      int G;
      int B;
    }Label;
    Label label[60];
    //cat计数器
    int CatCount;
};

#endif // READIMAGE_H
