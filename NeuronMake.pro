QT += core
QT += gui
QT += network

CONFIG += c++11

TARGET = NeuronMake
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    readimage.cpp \
    tooldebug.cpp \
    neuron_make.cpp

DISTFILES +=

HEADERS += \
    readimage.h \
    tooldebug.h \
    neuron_make.h

INCLUDEPATH += C:/Users/sky/Desktop/QriNeuronKit-master/QriNeuronLib
LIBS += -L"C:/Users/sky/Desktop/QriNeuronKit-master/build-QriNeuronLib-Desktop_Qt_5_7_0_MinGW_32bit-Debug/debug" -lQriNeuronLib
