#include "process_frame.hpp"


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
void ProcessFrame::processThermalFrame(cv::Mat& thermalFrame) {
    if (localCap.isOpened()) {
        // Apply a colormap to enhance visualization (adjust based on your camera's characteristics)
        cv::applyColorMap(thermalFrame, thermalFrame, cv::COLORMAP_JET);

        // Normalize the thermal data to the range [0, 255] for proper visualization
        cv::normalize(thermalFrame, thermalFrame, 0, 255, cv::NORM_MINMAX);

        // Threshold the image to highlight regions with temperatures above a certain threshold
        cv::Mat hotRegions;
        cv::threshold(thermalFrame, hotRegions, 200, 255, cv::THRESH_BINARY);

        // Create a matrix with a custom color for hot regions
        cv::Mat hotColorMatrix(thermalFrame.size(), thermalFrame.type(), cv::Scalar(0, 0, 255));

        // Blend the hot color matrix with the original thermal frame
        cv::bitwise_and(hotColorMatrix, hotRegions, hotColorMatrix);
        cv::addWeighted(thermalFrame, 1.0, hotColorMatrix, 0.5, 0.0, thermalFrame);
    }
}


