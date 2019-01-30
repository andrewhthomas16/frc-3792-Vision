// videoCalibration.cpp

#include <iostream>
#include <opencv2/opencv.hpp>

void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & Inputimage, cv::Mat & outputImage);
void maintenance(cv::Mat * image, cv::String windowName);

const double SATURATION = 1,// Values 0 - 1
             BRIGHTNESS = 0,
             CONTRAST = 1;


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
    
    if(!cap.isOpened())
    {
        std::cout << "Video camera was not found." << std::endl;
        return -1;
    }

    cv::Mat * image = new cv::Mat(), * filtered = new cv::Mat();
    cv::String WindowNameRaw = "VideoFeedRaw", windowNameAfter = "VideoFeedAfter", trackBarWindow = "Track Bar";
    int minHue = 0, maxHue = 180, minSat = 0, maxSat = 255, minVal = 0, maxVal = 255;
    
    cv::namedWindow(windowNameAfter);
    cv::namedWindow(trackBarWindow);
    cv::namedWindow(WindowNameRaw);

    cv::createTrackbar(cv::String("Min Hue"), trackBarWindow, &minHue, 180);
    cv::createTrackbar(cv::String("Max Hue"), trackBarWindow, &maxHue, 180);
    cv::createTrackbar(cv::String("Min Saturation"), trackBarWindow, &minSat, 255);
    cv::createTrackbar(cv::String("Max Saturation"), trackBarWindow, &maxSat, 255);
    cv::createTrackbar(cv::String("Min Value"), trackBarWindow, &minVal, 255);
    cv::createTrackbar(cv::String("Max Value"), trackBarWindow, &maxVal, 255);
    
    while(true)
    {
        cap >> * image;
        //*image = cv::imread("/home/student/Pictures/Example.jpg"); 
        
        if(image->empty())
        {
            std::cout << "Could not load image" << std::endl;
            return -1;
        }
        cv::imshow(WindowNameRaw, *image);
	//cv::imshow(trackBarWindow, );

	maintenance(image, windowNameAfter);
        
        // Filter image based off of a lower and upper Range of color.
        // The ranges are H: 0 - 180,  S: 0 - 255,  V: 0 - 255.
        filter(cv::Scalar(minHue, minSat, minVal), cv::Scalar(maxHue, maxSat, maxVal), *image, *filtered);

        cv::imshow(windowNameAfter, *filtered);
        if(cv::waitKey(1) > 0) break;
    }
        cv::destroyWindow(windowNameAfter);
        cv::destroyWindow(trackBarWindow);
        
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
