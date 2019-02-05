// blobsDetection.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Blobs.h"
#include "Blob.h"
#inlcude "Point2i.h"
#include <ctime>

void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & Inputimage, cv::Mat & outputImage);
void maintenance(cv::Mat * image, cv::String windowName);

const bool TEST = true, VIDEO = true;
const double SATURATION = 1,// Values 0 - 1
             BRIGHTNESS = 0,
             CONTRAST = 1;
const int THRESH = 2;

int main(int argc, char * argv[])
{
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
    cap.set(cv::CAP_PROP_AUTOFOCUS, 0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 160);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 120);
    cap.set(cv::CAP_PROP_SATURATION, SATURATION);
    cap.set(cv::CAP_PROP_BRIGHTNESS, BRIGHTNESS);
    cap.set(cv::CAP_PROP_CONTRAST, CONTRAST);
    //cv::Rect crop(0, 160, 320, 640);

    cv::Mat * image = new cv::Mat();
    
    if(!cap.isOpened())
    {
        std::cout << "Video camera was not found." << std::endl;
        return -1;
    }
    
    cv::String windowNameRaw = "VideoFeedRaw", windowNameAfter = "VideoFeedAfter";

    double fps = 0, center;
    Blobs blobs(image);
    blobs.setMinThreshold(THRESH);
    
    if(TEST)
    {
        cv::namedWindow(windowNameRaw);
        cv::namedWindow(windowNameAfter);
    }
    
    while(true)
    {
        if(VIDEO)
            cap >> *image;
        else
            *image = cv::imread("/~/Documents/frc-3792/Pictures/2019VisionImages/    CargoSideStraightDark72in.jpg");
        //*image =new image(crop);
        
        if(TEST)
            cv::imshow(windowNameRaw, *image);
        
        // Check for failure
        if(image->empty())
        {
            std::cout << "Could not open or find the image" << std::endl;
            return -1;
        }
        
        maintenance(image, windowNameAfter);
                
        // Filter image based off of a lower and upper Range of color.
        // The ranges are H: 0 - 100,  S: 0 - 255,  V: 0 - 255.
        filter(cv::Scalar(5, 5, 100), cv::Scalar(170, 100, 255), *image, *image);
        
        // Blur image to get rid of the bad data points.
        cv::GaussianBlur(*image, *image, cv::Size(9, 9), 2, 2);

        blobs.calcBlobs();
        
        if(blobs.getNumBlobs() > 0)
            blobs.getBlob(0).averageX();
        
        std::cout << "Center of White Pixels." << blobs.calcBlobs() << std::endl;
        cv::putText(*image, std::to_string(CLOCKS_PER_SEC / (clock() - fps)), cv::Point2f(10, 10), cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255));
      
        if(TEST)// Show image.
        {
            cv::imshow(windowNameAfter, *image);
            if(cv::waitKey(1) > 0) break;
            fps = clock();
        }
    }

    delete image;
    
    if (TEST)// Destroy window with the name windowName.
    {
        cv::destroyWindow(windowNameRaw);
        cv::destroyWindow(windowNameAfter);
    }
    
    return 0;
}


// Filter the inputImage based off of the upper and lower range 
// and store the result in outputImage.
void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & inputImage, cv::Mat & outputImage)
{
    if (lowerRange[0] > upperRange[0])// If the range passes zero then extra
    {                                // steps need to be taken to filter the image.
        cv::Mat lowerPic, upperPic;
        
        // Filter lower range to zero.
        cv::inRange(inputImage, lowerRange, cv::Scalar(180, upperRange[1], upperRange[2]), lowerPic);
        
        // Filter zero to upperRange.
        cv::inRange(inputImage, cv::Scalar(0, lowerRange[1], lowerRange[2]), upperRange, upperPic);
        
        // Combine upperPic and lowerPic together.
        cv::addWeighted(lowerPic, 1.0, upperPic, 1.0, 0.0, outputImage);
        }
    else// Filter image
        cv::inRange(inputImage, lowerRange, upperRange, outputImage);
}


// Steps that have to be taken every loop, but aren't needed.
void maintenance(cv::Mat * image, cv::String windowName)
{
    // Change the color space from BGR to HSV.
    cv::cvtColor(*image , *image, cv::COLOR_BGR2HSV);
}
