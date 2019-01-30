// Blob.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Blob.h"


// Constructor
Blob::Blob(const cv::Mat * _image)
{
    image = _image;
    minThresh = 100;
    minDist = 10;
}
Blob::Blob(const Blob & copy)
{
    image = copy.image;
    pointsX = copy.pointsX;
    minThresh = 100;
    minDist = 10;
}

// Deconstructor
Blob::~Blob()
{
}


void Blob::calcThreshold()
{
  // Make sure there are just enough pointsX elements.
    pointsX.resize(image->cols);
    
    // Reset the PointsX array to make sure all values are set to zero.
    for(int i = 0; i < image->cols; i ++)
      pointsX[i] = 0;

    // Loop through every pixel and check to see if it is white.
    // If it is white add one to corresponding colunm element in pointsX.
    for(int y = 0; y < image->rows; y++)
        for(int x = 0; x < image->cols; x++)
        {
            cv::Vec3b color = image->at<uchar>(cv::Point(x, y));
            if(color.val[0] >= 200)
	      pointsX[x] += 1;
	}

    // Make sure the colunms are over the min threshold for a colunm.
    for(int i = 0; i < pointsX.size(); i++)
        if(pointsX[i] < minThresh)
            pointsX[i] = 0;
}


double Blob::calcCenter()
{
    calcThreshold();

    int numElements = 0;
    center = 0;

    for(int i = 0; i < pointsX.size(); i++)
        if(pointsX[i] > 0)
        {
            center += i + 1;
            numElements++;
        }

    center = center / numElements;
    
    return center;
}


// Getters
double Blob::getCenter(){ return center; }

unsigned int Blob::getMinThreshold(){ return minThresh; }

unsigned int Blob::getMinDistance(){ return minDist; }

unsigned int Blob::getXcheck(){ return xCheck; }

unsigned int Blob::getYCheck(){ return yCheck; }

cv::Mat Blob::getImage(){ return *image; }


// Setters
void Blob::setMinThreshold(unsigned int _minThresh){ minThresh = _minThresh; }

void Blob::setMinDistance(unsigned int _minDist){ minDist = _minDist; }

void Blob::setXCheck(unsigned int _xCheck){ xCheck = _xCheck; }

void Blob::setYCheck(unsigned int _yCheck){ yCheck = _yCheck; }

void Blob::setImage(cv::Mat * _image){ image = _image; }
