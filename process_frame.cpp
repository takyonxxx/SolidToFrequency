#include "process_frame.hpp"

using namespace cv;
using namespace std;

ProcessFrame::ProcessFrame(QObject* parent)
    :QObject(parent)
{
}

bool ProcessFrame::load(int camIndex) {

    try {
        localCap.open(camIndex);
    } catch (cv::Exception& e) {
        std::cerr << "OpenCV Exception: " << e.what() << std::endl;
        std::cerr << "Could not open camera with index: " << camIndex << std::endl;
    }

    return true;
}

void ProcessFrame::processFrame(Mat &frameRGB) {
    if (localCap.isOpened()) {
        // Convert the frame to HSV
        cv::cvtColor(frameRGB, frameRGB, cv::COLOR_BGR2HSV);

        // Define a range for green color in HSV
        cv::Scalar lowerGreen(30, 30, 30);  // Adjust these values based on your specific green color
        cv::Scalar upperGreen(80, 255, 255);

        // Create a binary mask where pixels within the green range are white, others are black
        cv::Mat greenMask;
        cv::inRange(frameRGB, lowerGreen, upperGreen, greenMask);

        // Create a matrix with the original frame values for non-green pixels
        cv::Mat nonGreenMatrix = frameRGB.clone();
        nonGreenMatrix.setTo(cv::Scalar(0, 0, 0), greenMask);

        // Set green pixels in the original frame to a specific green color
        frameRGB.setTo(cv::Scalar(60, 255, 255), greenMask);

        // Blend the modified green frame with the original frame
        frameRGB = frameRGB + nonGreenMatrix;

        // Convert back to BGR
        cv::cvtColor(frameRGB, frameRGB, cv::COLOR_HSV2BGR);
    }
}


