// visionProcessing.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "Blobs.h"
#include "Blob.h"
#include "Point2i.h"
#include "UDP.h"

std::string sendBackData(Blobs * blobs);
void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & Inputimage, cv::Mat & outputImage);
void maintenance(cv::Mat * image, cv::String windowName);

const bool TEST = true, VIDEO = true;
const double SATURATION = 1,// Values 0 - 1
             BRIGHTNESS = 0,
             CONTRAST = 1;
const int THRESH = 120,
DIST = 40;
const char * IP = "10.37.92.43",
* PORT= "5800";


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
    
    if(!cap.isOpened())
    {
        std::cout << "Video camera was not found." << std::endl;
        return -1;
    }
    
    cv::String windowNameRaw = "VideoFeedRaw", windowNameAfter = "VideoFeedAfter";
    cv::Mat * image = new cv::Mat();
    Blobs blobs(image);
    blobs.setMinThreshold(THRESH);
    blobs.setMinDistance(DIST);
    UDP udp(IP, PORT);
    double fps = 0, center;
    
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
        
        filter(cv::Scalar(70, 10, 0), cv::Scalar(140, 255, 255), *image, *image);
        
        // Blur image to get rid of the bad data points.
        cv::GaussianBlur(*image, *image, cv::Size(9, 9), 2, 2);
        
        blobs.calcBlobs();
        
        //if(blobs.getNumBlobs() > 0)
        // std::cout << blobs.getBlob(0).averageX() << std::endl;
        
        std::cout << blobs.getNumBlobs() << std::endl;
        //std::cout << "Center of White Pixels." << blobs.calcBlobs() << std::endl;
        cv::putText(*image, std::to_string(CLOCKS_PER_SEC / (clock() - fps)), cv::Point2f(10, 10), cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255));
        
        // Send data back by getting string from sendBackData() and
        // converting result to char *.
        udp.send(const_cast<char *>(sendBackData(& blobs).c_str()));
        
        if(TEST)// Show image.
        {
            cv::imshow(windowNameAfter, *image);
            if(cv::waitKey(1) == 27) break;
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


// Function to compile information to send over UDP.
std::string sendBackData(Blobs * blobs)
{
    std::string sendBack;
    int i = 1;
    
    if(blobs->getNumBlobs() > 0) // Add the first Blob cords if there is one.
        sendBack = std::to_string(blobs->getBlob(0).averageX()) + "," + std::to_string(blobs->getBlob(0).averageY());
    
    do
    {
        if(i < blobs->getNumBlobs()) // Add the rest of the Blob cords.
            sendBack += ", " + std::to_string(blobs->getBlob(i).averageX()) + "," + std::to_string(blobs->getBlob(i).averageY());
        else
            break;
    }while(true);
    
    return sendBack;
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

