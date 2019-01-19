#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argv, char ** argc)
{
    // Read the image file.
    cv::Mat image = cv::imread("/home/student/Pictures/IMG_1775.jpg");

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
    //cv::resizeWindow(windowName, 600, 600);

    // Create new image to hold filtered image.
    cv::Mat mask;

     // Create an upper and lower scalar to filter colors.
    cv::Scalar lowerRange = Scalar(25, 10, 0), upperRange = Scalar(55, 255, 255);

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

    //Find the center of all the points found of a certain color.
    cv::Moments m = moments(mask, true);
    cv::Point p(m.m10/m.m00, m.m01/m.m00);

    //Find what the points would be if the image was 100 by 100.
    double x = p.x * (100.0 / mask.cols);
    double y = p.y * (100.0 / mask.rows);

    //Output the center.
    std::cout << "The center is equal to [" << x << ", "<< y
              << "] where there are 100 rows and colunms." << std::endl;

    //Put the center point on the image.
    cv::circle(mask, p, 5, cv::Scalar(0, 0, 0), -1);
    
    // Show mask.
    cv::imshow(windowName, mask);
    
    // Wait for keyboard input.
    cv::waitKey(0);

    //Destroy window with name windowName.
    cv::destroyWindow(windowName);

    return 0;
}
