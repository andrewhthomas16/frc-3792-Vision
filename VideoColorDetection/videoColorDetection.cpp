#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argv, char ** argc)
{
    cv::VideoCapture cap(0);
    // Read the image file.
    cv::Mat image;

    // Check for failure
    if(!cap.open(0))
    {
        std::cout << "Could not open or find the image" << std::endl;
        cin.get();
        return -1;
    }
    while(true)
    {
        cap >> image;
        
        // Change the color space from BGR to HSV.
        cv::cvtColor(image, image,  COLOR_BGR2HSV);

        // Make a name for a window
        cv::String windowName = "Hue Wheel!";

        // Make a window with name WindowName.
        cv::namedWindow(windowName);

        // Create new image to hold filtered image.
        cv::Mat mask;

        // Create an upper and lower scalar to filter colors.
        cv::Scalar lowerRange = Scalar(310, 10, 0), upperRange = Scalar(50, 360, 360);

        if(lowerRange[0] > upperRange[0]) // If the range passes zero then
        {                                 // extra steps need to be taken to
            cv::Mat lowerPic, upperPic;   // filter image.

            // Filter lowerRange to zero.
            cv::inRange(image, lowerRange, cv::Scalar(0, upperRange[1], upperRange[2]), lowerPic);

            //Filter zero to upperRange.
            cv::inRange(image, cv::Scalar(0, lowerRange[1], lowerRange[2]), upperRange, upperPic);

            // Combine upperPic and lowerPic together into mask.
            cv::addWeighted(lowerPic, 1.0, upperPic, 1.0, 0.0, mask);

        }
        else
        {

            // Filter image
            cv::inRange(image, lowerRange, upperRange, mask);
        }

        //Blur image to get rid of bad data points
        cv::GaussianBlur(mask, mask, Size(9, 9), 2, 2);

        cv::Moments m = moments(mask, true);
        cv::Point p(m.m10/m.m00, m.m01/m.m00);

        std::cout << Mat(p) << std::endl;

        cv::circle(mask, p, 5, cv::Scalar(128, 0, 0), -1);
    
        // Show mask.
        cv::imshow(windowName, mask);
    
        // Wait for keyboard input.
        cv::waitKey(0);

        // Destroy window with name windowName.
        cv::destroyWindow(windowName);
    }

    return 0;
}
