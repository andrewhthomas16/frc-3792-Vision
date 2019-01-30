// blobDetection.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include "Blob.h"
#include <ctime>

void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & Inputimage, cv::Mat & outputImage);
void maintenance(cv::Mat * image, cv::String windowName);

const bool TEST = true;
const double SATURATION = 1,// Values 0 - 1
             BRIGHTNESS = 0,
             CONTRAST = 1;
const int THRESH = 100;

int main(int argv, char ** argc)
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
    cv::Mat cannyOutput;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    Blob blob(image);
    blob.setMinThreshold(2);
    
    if(TEST)
      {
	cv::namedWindow(windowNameRaw);
	cv::namedWindow(windowNameAfter);
      }
    
    while(true)
    {
        //*image = cv::imread("/~/Documents/frc-3792/Pictures/2019VisionImages/CargoSideStraightDark72in.jpg");
        cap >> *image;
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

        //std::cout << blob.calcCenter() << std::endl;
	/*cv::Canny(*image, cannyOutput, THRESH, THRESH * 2, 3);
	cv::findContours(cannyOutput, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	std::vector<cv::Moments> mu(contours.size());
	std::vector<cv::Point2f> mc(contours.size());

	for(int i = 0; i < contours.size(); i++)
	  mu[i] = cv::moments(contours[i], false);

	for(int i = 0; i < contours.size(); i++)
	  mc[i] = cv::Point2f(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
	
	std::cout << "Number of Centers: " << contours.size() << std::endl;

	for(int i = 0; i < contours.size(); i++)
	  std::cout << "(" << mc[i].x << ", " << mc[i].y <<")" << std::endl;
	*/

	std::cout << "Center of White Pixels." << blob.calcCenter() << std::endl;
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
