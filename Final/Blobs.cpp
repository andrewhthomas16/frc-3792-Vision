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
Blobs::Blobs(const cv::Mat * _image, const unsigned int _minThresh,
      const unsigned int _minDist, const unsigned int _minArea)
{
    image = _image;
    minThresh = _minThresh;
    minDist = _minDist;
    minArea = _minArea;
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


/* Method to make a 2d array from the colunms of a black and white image,
   where an elementis considered high when there are more than minThresh
   white pixels in a colunm. */
void Blobs::calcThreshX()
{
    // Make sure there are just enough pointsX elements.
    pointsX.resize(image->cols);

    // Reset the pointsX array to make sure all values are set to zero.
    for(int i = 0; i < image->cols; i ++)
        pointsX[i] = 0;
    
    // Loop through every pixel and check to see if it is white.
    // If it is white add one to corresponding colunm element in pointsX.
    for(int y = 0; y < image->rows; y++)
        for(int x = 0; x < image->cols; x++)
        {
            cv::Vec3b color = image->at<uchar>(cv::Point(x, y));
            if(color.val[0] >= 100)
                pointsX[x] += 1;
        }
    
    // Make sure the colunms are over the min threshold for a colunm.
    for(int i = 0; i < pointsX.size(); i++)
        if(pointsX[i] < minThresh)
            pointsX[i] = 0;
}


/* Method to make a 2d array from the rows of a black and white image,
 where an elementis considered high when there are more than minThresh
 white pixels in a colunm. */
void Blobs::calcThreshY()
{
    // Make sure there are just enough pointsY elements.
    pointsY.resize(image->rows);
    
    // Reset the pointsY array to make sure all values are set to zero.
    for(int i = 0; i < image->rows; i ++)
        pointsY[i] = 0;
    
    // Loop through every pixel and check to see if it is white.
    // If it is white add one to corresponding colunm element in pointsY.
    for(int x = 0; x < image->cols; x++)
        for(int y = 0; y < image->rows; y++)
        {
            cv::Vec3b color = image->at<uchar>(cv::Point(x, y));
            if(color.val[0] >= 100)
                pointsY[y] += 1;
        }
    
    // Make sure the colunms are over the min threshold for a colunm.
    for(int i = 0; i < pointsY.size(); i++)
        if(pointsY[i] < minThresh)
            pointsY[i] = 0;
}


// Method to find the center of a 2d array, where the array is defined by
// clacThreshold().
double Blobs::calcCenter()
{
    calcThreshX();
    
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
    calcThreshX();
    calcThreshY();
    std::vector<Point2i> interval;
    
    for(int i = 0; i < pointsY.size(); i++) // Loop through rows.
        if(pointsY[i] > 0) // Check to see if there is a blob/interval.
        {
            interval.push_back(Point2i());
            interval[interval.size() - 1].x = i;
            int dist = 0;
            bool stop = false;
            
            for(i; i < pointsY.size() && !stop; i++) // Find where the
            {                                        // blob/interval stops.
                if(pointsY[i] == 0 && dist <= minDist) // There is nothing in this
                    dist++;                            // colunm.
                else if(pointsY[i] == 0 && dist > minDist) // min distance has been
                {                                          // surpassed.
                    stop = true;
                    interval[interval.size() - 1].y = i - minDist;
                }
                else
                    dist = 0;
                
                if(i == pointsY.size() - 1) // You are at the edge of the image.
                {                           // This is the end of the interval.
                    interval[interval.size() - 1].y = i;
                    stop = true;
                }
            }
        }
    
    for(int i = 0; i < interval.size(); i++) // Loop through intervals to find
    {                                        // all the blobs on the interval.
        int dist = 0;
        bool newBlob = true;
        for(int x = interval[i].x; x < interval[i].y; x++)
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
    }

    for(int i = 0; i < blobs.size(); i++)
       	if(blobs.at(i).size() < minArea)
        {
            blobs.erase(blobs.begin() + i);
            i--;
        }

    sortBlobs(); // Sort blobs by array. Largest to smallest.
}


// A method to combine blobs into the lower element in blobs.
void Blobs::combineBlobs(int i, int j)
{
	if(i < blobs.size() && j < blobs.size())
	{
		if(i < j)
		{
			blobs.at(i) += blobs.at(j);
			blobs.erase(blobs.begin() + j);
		}
		else if(j < i)
		{
			blobs.at(j) += blobs.at(i);
			blobs.erase(blobs.begin() + i);
		}
	}
}


// Method to all points that are white and in image->col to blob.
// This method helps calcBlobs().
void Blobs::addPoints(Blob & blob, int col)
{
	for(int i = 0; i < image->rows; i++) // Loop through colunms.
	{
	    cv::Vec3b color = image->at<uchar>(cv::Point(col, i));
            if(color.val[0] >= 100) // Check to see if (col, i) is white.
                blob.addPoint(col, i);
	}
}


// Function to sort all of the Blobs in blobs. This method utilizes bubble
// sort and sorts highest to lowest.
void Blobs::sortBlobs()
{
    Blob temp;
    for(int i = 0; i < blobs.size(); i++)
        for(int j = 0; j < blobs.size() - 1; j++)
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
unsigned int Blobs::getMinArea(){ return minArea; }
unsigned int Blobs::getXcheck(){ return xCheck; }
unsigned int Blobs::getYCheck(){ return yCheck; }
cv::Mat Blobs::getImage(){ return *image; }
Blob * Blobs::getBlob(int i){ return &blobs.at(i); }
int Blobs::getNumBlobs(){ return blobs.size(); }


// Setters
void Blobs::setMinThreshold(unsigned int _minThresh){ minThresh = _minThresh; }
void Blobs::setMinDistance(unsigned int _minDist){ minDist = _minDist; }
void Blobs::setMinArea(unsigned int _minArea) { minArea = _minArea; }
void Blobs::setXCheck(unsigned int _xCheck){ xCheck = _xCheck; }
void Blobs::setYCheck(unsigned int _yCheck){ yCheck = _yCheck; }
void Blobs::setImage(cv::Mat * _image){ image = _image; }
