// videoCalibration.cpp

#include <iostream>
#include <opencv2/opencv.hpp>

float avgH(cv::Mat * orignal, cv::Mat * filtered);
float avgS(cv::Mat * orignal, cv::Mat * filtered);
float avgV(cv::Mat * orignal, cv::Mat * filtered);
void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & Inputimage, cv::Mat & outputImage);
void maintenance(cv::Mat * image, cv::String windowName);

const double SATURATION = 1,// Values 0 - 1
             BRIGHTNESS = 0,
             CONTRAST = 1;


int main(int argc, char * argv[])
{
    cv::VideoCapture cap(0);
    cap.set(CV_CAP_PROP_BUFFERSIZE, 1);
    cap.set(CV_CAP_PROP_AUTOFOCUS, 0);
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 160);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 120);
    cap.set(CV_CAP_PROP_SATURATION, SATURATION);
    cap.set(CV_CAP_PROP_BRIGHTNESS, BRIGHTNESS);
    cap.set(CV_CAP_PROP_CONTRAST, CONTRAST);
    
    if(!cap.isOpened())
    {
        std::cout << "Video camera was not found." << std::endl;
        return -1;
    }

    cv::Mat * image = new cv::Mat(), * filtered = new cv::Mat();
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

	maintenance(image, windowNameAfter);
        
        // Filter image based off of a lower and upper Range of color.
        // The ranges are H: 0 - 100,  S: 0 - 255,  V: 0 - 255.
        filter(cv::Scalar(82, 75, 50), cv::Scalar(90, 225, 255), *image, *filtered);

        cv::imshow(windowNameAfter, *filtered);
	
        std::cout << "Average H Value: " << avgH(image, filtered)
                  << "\tAverage S Value: " << avgS(image, filtered)
                  << "\tAverage V Value: " << avgV(image, filtered) << std::endl;
	if(cv::waitKey(1) > 0) break;
    }

    cv::destroyWindow(windowNameRaw);
    cv::destroyWindow(windowNameAfter);
    
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
	  if(x < original->cols && y < original->rows)
	    if(filtered->at<cv::Vec3b>(x, y)[0] > 200)
            {
                avgH += original->at<cv::Vec3b>(x, y)[0];
		total++;
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
	  if(x < filtered->cols && y < filtered->rows)
	    if(filtered->at<cv::Vec3b>(x, y)[0] > 200)
	      {
	      avgS += original->at<cv::Vec3b>(x, y)[1];
	      total++;
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
           if(x < filtered->cols && y < filtered->rows)
	   if(filtered->at<cv::Vec3b>(x, y)[0] > 200)
            {
                avgV += original->at<cv::Vec3b>(x, y)[2];
		total++;
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


// Steps that have to be taken every loop, but aren't needed.
void maintenance(cv::Mat * image, cv::String windowName)
{
    // Change the color space from BGR to HSV.
    cv::cvtColor(*image , *image, cv::COLOR_BGR2HSV);
}
