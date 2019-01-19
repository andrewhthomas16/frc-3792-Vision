#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argv, char ** argc)
{
    // Read the image file.
    cv::Mat image = cv::imread("/home/student/Pictures/Example.jpg");

    // Check for failure
    if(image.empty())
    {
        std::cout << "Could not open or find the image" << std::endl;
        cin.get();
        return -1;
    }

    // Resize the image to fit on the screen.
    cv::resize(image, image, cv::Size(500, 500));

    // Change the color space from BGR to HSV.
    cv::cvtColor(image, image,  COLOR_BGR2HSV);

    // Make a name for a window
    cv::String windowName = "Hue Wheel!";

    // Make a window with name WindowName.
    cv::namedWindow(windowName, WINDOW_AUTOSIZE);

    // Create new image to hold filtered image.
    cv::Mat mask;

     // Create an upper and lower scalar to filter colors.
    // The ranges are H: 0 - 180,  S: 0 -255,  V: 0 - 255.
    cv::Scalar lowerRange = Scalar(20, 180, 100), upperRange = Scalar(40, 255, 255);

    if(lowerRange[0] > upperRange[0]) // If the range passes zero then extra
    {                                 // steps need to be taken to filter image.
        cv::Mat lowerPic, upperPic;

        // Filter lowerRange to zero.
        cv::inRange(image, lowerRange, cv::Scalar(180, upperRange[1], upperRange[2]), lowerPic);

        //Filter zero to upperRange.
        cv::inRange(image, cv::Scalar(0, lowerRange[1], lowerRange[2]), upperRange, upperPic);
        
        //Combine upperPic and lowerPic together into mask.
        cv::addWeighted(lowerPic, 1.0, upperPic, 1.0, 0.0, mask);

    }
    else  // Filter image
        cv::inRange(image, lowerRange, upperRange, mask);

    //Blur image to get rid of bad data points
    cv::GaussianBlur(mask, mask, Size(9, 9), 2, 2);
    
    //Create a list of parameters to find blobs
    cv::SimpleBlobDetector::Params params;
    params.filterByCircularity = true;
    params.minCircularity = 0.1;

    //Create a SimpleBlobDetector with the parameters that were just made
    cv::Ptr<cv::SimpleBlobDetector> detector = cv::SimpleBlobDetector::create(params); //Default parameters

    std::vector<cv::KeyPoint> keypoints;

    //Detect keyPoints
    detector->detect(mask, keypoints);

    
    
    // Show mask.
    cv::imshow(windowName, mask);
    
    // Wait for keyboard input.
    cv::waitKey(0);

    //Destroy window with name windowName.
    cv::destroyWindow(windowName);

    return 0;
}
