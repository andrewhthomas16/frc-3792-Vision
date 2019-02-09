// Blobs.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Blobs.h"
#include "Blob.h"
#include "Point2i.h"


// Constructor
Blobs::Blobs(const cv::Mat * _image)
{
    image = _image;
    minThresh = 100;
    minDist = 10;
}
Blobs::Blobs(const Blobs & copy)
{
    image = copy.image;
    pointsX = copy.pointsX;
    minThresh = 100;
    minDist = 10;
}

// Deconstructor
Blobs::~Blobs()
{
}


/* Method to make a 2d array from a black and white image, where an element
   is considered high when there are more than minThresh white pixels in a
   colunm. */
void Blobs::calcThreshold()
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


// Method to find the center of a 2d array, where the array is defined by
// clacThreshold().
double Blobs::calcCenter()
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


// Method to put pixels into blobs using a 2d array where the
// 2d array is defined by calcThreshold().
void Blobs::calcBlobs()
{
    blobs.clear();
    calcThreshold();
    
    int dist = 0;
    bool newBlob = true;
    
    // Loop through 1D array.
    for(int i = 0; i < pointsX.size(); i++)
    {
        if(pointsX[i] > 0 && newBlob) // Create a new Blob, other Blobs
        {                             // are too far away.
            blobs.push_back(Blob());
            addPoints(blobs.at(blobs.size() - 1), i);
            newBlob = false;
        }
        else if(pointsX[i] > 0 && !newBlob) // There is a Blob nearby, so add
        {                                   // to that Blob.
            addPoints(blobs.at(blobs.size() - 1), i);
        }
        else if(pointsX[i] == 0 && !newBlob) // There is nothing to be added
            dist++;                          // to a Blob, so add one to dist.
        
        if(dist >= minDist) // Check to see if a new Blob need to be made
        {                   // next time pointsX[i] > 0.
            newBlob = true;
            dist = 0;
        }
    }
    
    sortBlobs(); // Sort blobs by array. Largest to smallest.
}


// Method to all points that are white and in image->col to blob.
// This method helps calcBlobs().
void Blobs::addPoints(Blob & blob, int col)
{
	for(int i = 0; i < image->rows; i++) // Loop through colunm.
	{
		cv::Vec3b color = image->at<uchar>(cv::Point(col, i));
            if(color.val[0] >= 200) // Check if pixel(col, i) is white.
                blob.addPoint(col, i);
	}
}


// Function to sort all of the Blobs in blobs. This method utilizes bubble
// sort and sorts highest to lowest.
void Blobs::sortBlobs()
{
    Blob temp;
    for(int i = 0; i < blobs.size(); i++)
        for(int j = 0; j < blobs.size() - 1; i++)
            if(blobs.at(j).size() < blobs.at(j + 1).size())
            {
                temp = blobs.at(j);
                blobs.at(j) = blobs.at(j + 1);
                blobs.at(j + 1) = temp;
            }
}


// Getters
double Blobs::getCenter(){ return center; }

unsigned int Blobs::getMinThreshold(){ return minThresh; }

unsigned int Blobs::getMinDistance(){ return minDist; }

unsigned int Blobs::getXcheck(){ return xCheck; }

unsigned int Blobs::getYCheck(){ return yCheck; }

cv::Mat Blobs::getImage(){ return *image; }

Blob Blobs::getBlob(int i){ return blobs.at(i); }

int Blobs::getNumBlobs(){ return blobs.size(); }


// Setters
void Blobs::setMinThreshold(unsigned int _minThresh){ minThresh = _minThresh; }

void Blobs::setMinDistance(unsigned int _minDist){ minDist = _minDist; }

void Blobs::setXCheck(unsigned int _xCheck){ xCheck = _xCheck; }

void Blobs::setYCheck(unsigned int _yCheck){ yCheck = _yCheck; }

void Blobs::setImage(cv::Mat * _image){ image = _image; }
