#ifndef RPPG_hpp
#define RPPG_hpp

#include <string>
#include <stdio.h>
#include <iostream>
#include <QDebug>
#include <QCamera>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace dnn;
using namespace std;

class ProcessFrame : public QObject
{
    Q_OBJECT

public:
    explicit ProcessFrame(QObject *parent = nullptr);
    // Load Settings
    bool load(int camIndex);
    void processFrame(Mat &frameRGB);
    void processThermalFrame(Mat &thermalFrame);

    int64_t get_current_time()
    {
        int64_t tickCount = cv::getTickCount();
        return static_cast<int64_t>((tickCount * 1000.0) / cv::getTickFrequency());
    }

private:
    VideoCapture localCap;
    QString info{};

signals:
    void sendInfo(QString);
};


#endif /* RPPG_hpp */
