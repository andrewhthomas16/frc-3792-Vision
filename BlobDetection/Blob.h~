// Blob.h

#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>

class Blob
{
public:
    // Constructors
    Blob(const cv::Mat * _image);
    Blob(const Blob & copy);

    // Deconstructors
    ~Blob();

    void calcThreshold();
    double calcCenter();

    double getCenter();// Getters
    unsigned int getMinThreshold();
    unsigned int getMinDistance();
    unsigned int getXcheck();
    unsigned int getYCheck();
    cv::Mat getImage();

    void setMinThreshold(unsigned int _minThresh);// Setters
    void setMinDistance(unsigned int _minDist);
    void getXCheck(unsigned int _xCheck);
    void getYCheck(unsigned int _yCheck);
    void setImage(cv::Mat * _image);

private:
    const cv::Mat * image;
    std::vector<int> pointsX;
    double center;
    unsigned int minThresh, minDist, xCheck, yCheck, minX, maxX, minY, maxY;
};
