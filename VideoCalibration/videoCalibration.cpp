// videoCalibration.cpp

#include <iostream>
#include <opencv2/opencv.hpp>

float avgH(cv::Mat * orignal, cv::Mat * filtered);
float avgS(cv::Mat * orignal, cv::Mat * filtered);
float avgV(cv::Mat * orignal, cv::Mat * filtered);
void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & Inputimage, cv::Mat & outputImage);


int main(int argc, char * argv[])
{
    cv::VideoCapture cap(0);

    if(!cap.isOpened())
    {
        std::cout << "Video camera was not found." << std::endl;
        return -1;
    }

    cv::Mat * image, * filtered;
    cv::String windowNameRaw = "VideoFeedRaw", windowNameAfter = "VideoFeedAfter";
    cv::namedWindow(windowNameRaw);
    cv::namedWindow(windowNameAfter);
    
    while(true)
    {
        cap >> * image;
        if(image->empty())
        {
            std::cout << "Could not load image" << std::endl;
            return -1;
        }
        cv::imshow(windowNameRaw, *image);
        
        // Filter image based off of a lower and upper Range of color.
        // The ranges are H: 0 - 100,  S: 0 - 255,  V: 0 - 255.
        filter(cv::Scalar(70, 23, 50), cv::Scalar(100, 255, 255), *image, *filtered);

        cv::imshow(windowNameAfter, *image);

        std::cout << "Average H Value: " << avgH(image, filtered)
                  << "\tAverage S Value: " << avgS(image, filtered)
                  << "\tAverage V Value: " << avgV(image, filtered) << std::endl;
    }
    
    return 0;
}


// Function to find the average H value in a filtered image.
float avgH(cv::Mat * original, cv::Mat * filtered)
{
    float avgH = 0, total = 0;
    cv::Vec3b originalColor, filteredColor;
    for(int x = 0; x < filtered->rows; x++)
        for(int y = 0; y < filtered->cols; y++)
        {
           filteredColor = filtered->at<uchar>(cv::Point(x, y));
            if(filteredColor.val[0] >= 200)
            {
                originalColor = original->at<uchar>(cv::Point(x, y));
                avgH += originalColor.val[0];
            }
        }

    if(total == 0)
        return -1;

    return avgH / total;
}


// Function to find the average S value in a filtered image.
float avgS(cv::Mat * original, cv::Mat * filtered)
{
    float avgS = 0, total = 0;
    cv::Vec3b originalColor, filteredColor;
    for(int x = 0; x < filtered->rows; x++)
        for(int y = 0; y < filtered->cols; y++)
        {
           filteredColor = filtered->at<uchar>(cv::Point(x, y));
            if(filteredColor.val[0] >= 200)
            {
                originalColor = original->at<uchar>(cv::Point(x, y));
                avgS += originalColor.val[1];
            }
        }

    if(total == 0)
        return -1;

    return avgS / total;
}


// Function to find the average V value in a filtered image.
float avgV(cv::Mat * original, cv::Mat * filtered)
{
    float avgV = 0, total = 0;
    cv::Vec3b originalColor, filteredColor;
    for(int x = 0; x < filtered->rows; x++)
        for(int y = 0; y < filtered->cols; y++)
        {
           filteredColor = filtered->at<uchar>(cv::Point(x, y));
            if(filteredColor.val[0] >= 200)
            {
                originalColor = original->at<uchar>(cv::Point(x, y));
                avgV += originalColor.val[2];
            }
        }

    if(total == 0)
        return -1;

    return avgV / total;
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
