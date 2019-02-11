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
    unsigned int getXcheck();
    unsigned int getYCheck();
    cv::Mat getImage();
    Blob getBlob(int i);
    int getNumBlobs();
    
    // Setters
    void setMinThreshold(unsigned int _minThresh);
    void setMinDistance(unsigned int _minDist);
    void setXCheck(unsigned int _xCheck);
    void setYCheck(unsigned int _yCheck);
    void setImage(cv::Mat * _image);
    
private:
    void calcThreshold();
    void addPoints(Blob & blob, int col);
    void sortBlobs();
    const cv::Mat * image;
    std::vector<int> pointsX;
    std::vector<Blob> blobs;
    double center;
    unsigned int minThresh, minDist, xCheck, yCheck;
};
