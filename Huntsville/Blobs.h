// Blob.h

#pragma once
#include <iostream>
#include <opencv2/opencv.hpp>
#include "Point2i.h"
#include "Blob.h"

class Blobs
{
public:
    // Constructors
    Blobs(const cv::Mat * _image);
    Blobs(const cv::Mat * _image, const unsigned int _minThresh,
          const unsigned int _minDist, const unsigned int _minArea);
    Blobs(const Blobs & copy);
    
    // Deconstructors
    ~Blobs();
    
    double calcCenter();
    void calcBlobs();
    void combineBlobs(int i, int j);

    // Getters
    double getCenter();
    unsigned int getMinThreshold();
    unsigned int getMinDistance();
    unsigned int getMinArea();
    unsigned int getXcheck();
    unsigned int getYCheck();
    cv::Mat getImage();
    Blob * operator [](int i);
    int size();
    
    // Setters
    void setMinThreshold(unsigned int _minThresh);
    void setMinDistance(unsigned int _minDist);
    void setMinArea(unsigned int _minArea);
    void setXCheck(unsigned int _xCheck);
    void setYCheck(unsigned int _yCheck);
    void setImage(cv::Mat * _image);
    
private:
    void calcThreshX();
    void calcThreshY();
    void addPoints(Blob & blob, Point2i interval, int col);
    int calcThreshInt(Point2i interval, int col);
    void sortBlobs();
    const cv::Mat * image;
    std::vector<int> pointsX;
    std::vector<int> pointsY;
    std::vector<Blob> blobs;
    double center;
    unsigned int minThresh, minDist, minArea, xCheck, yCheck;
};
