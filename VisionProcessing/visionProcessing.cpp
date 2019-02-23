// visionProcessing.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "Blobs.h"
#include "Blob.h"
#include "Point2i.h"
#include "UDP.h"


std::string sendBackData(Blobs * blobs, std::string whichTarg);
void calcHatchAndBall(Blobs * blobs);
void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & Inputimage, cv::Mat & outputImage);
float distance(float areaIn, float areaPix, float camArea, float camAngleY, float camAngleX);
float angle(float widthPix, float numPixInCam, float camAngleX);
void maintenance(cv::Mat * image, cv::String windowName);

// Values for simulation.
const bool TEST = true,
	       VIDEO = true,
	       RIO = true,
	       BLUR = false,
           HATCH = true;

// Values for the camera.
const double SATURATION = 1,// Values 0 - 1
             BRIGHTNESS = 0,
             CONTRAST = 1;

// Values for calculating Blobs.
const int HEIGHT = 120,
          WIDTH = 160,
          THRESH = 5,
          DIST = 1,
          AREA = 30;

// Values for UDP
const char * IP = "10.37.92.43",
           * PORT= "5800";

// For calculating distance and angle.
const float TAPEAREA = 22,
            BALLAREA = 176.714587,
            CAMANGLEY = 20.25,
            CAMANGLEX = 27;


int main(int argc, char * argv[])
{
    cv::String windowNameRaw = "VideoFeedRaw", windowNameAfter = "VideoFeedAfter";
    cv::Mat * image = new cv::Mat();
    Blobs blobs(image, THRESH, DIST, AREA);
    //UDP udp(IP, PORT);
    double fps = 0, center;

    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, HEIGHT);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, WIDTH);
    cap.set(cv::CAP_PROP_SATURATION, SATURATION);
    cap.set(cv::CAP_PROP_BRIGHTNESS, BRIGHTNESS);
    cap.set(cv::CAP_PROP_CONTRAST, CONTRAST);

    if(!cap.isOpened())
    {
        std::cout << "Video camera was not found." << std::endl;
        return -1;
    }
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
        *image = cv::imread("../frc-3792/Pictures/2019VisionImages/CargoSideStraightDark72in.jpg");

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
        filter(cv::Scalar(40, 0, 163), cv::Scalar(100, 130, 255), *image, *image);
        
        // Blur image to get rid of the bad data points.
        if(BLUR)
            cv::GaussianBlur(*image, *image, cv::Size(9, 9), 2, 2);

        blobs.calcBlobs();
        
        if(TEST)
            cv::putText(*image, std::to_string(CLOCKS_PER_SEC / (clock() - fps)), cv::Point2f(10, 10), cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255));

        if(HATCH) // SPECIFIC TO 2019.
            calcHatchAndBall(&blobs);
        
        // Send data back by getting string from sendBackData() and
        // converting result to char *.
        if(RIO)
            std::cout << sendBackData(& blobs, "TAPE") << std::endl;

        if(TEST) // Show image.
        {
            cv::imshow(windowNameAfter, *image);
            if(cv::waitKey(1) == 27) break;
            fps = clock();
        }
    }
    
    delete image;
    
    if (TEST) // Destroy window with the name windowName.
    {
        cv::destroyWindow(windowNameRaw);
        cv::destroyWindow(windowNameAfter);
    }
    
    return 0;
}

float distance(float areaIn, float areaPix, float camArea, float camAngleY, float camAngleX)

// Function to compile information to send over UDP.
std::string sendBackData(Blobs * blobs, std::string whichTarg)
{
    std::transform(whichTarg.begin(), whichTarg.end(), whichTarg.begin(), ::toupper);
    
    std::string sendBack;
    int i = 1;
    int ballTape = -1;
    float dist, ang;
    
    
    if(whichTarg == "TAPE") // If you are looking for the vision tape.
    {
        for(int i = 0; i < 3 && i < blobs->getNumBlobs(); i++) // Try to find ball vision tape.
            if(blobs->getBlob(i)->averageY() < HEIGHT / 2)
            {
                // Find the distance and angle to the ball.
                // Put the two values in UDP string.
                dist = distance(TAPEAREA, blobs->getBlob(i)->area(), HEIGHT / 2, WIDTH / 2);
                if(blobs->getBlob(i)->averageX() < WIDTH / 2) // Angle to left.
                    ang = angle((WIDTH / 2) - blobs->getBlob(i)->averageX(), WIDTH / 2, CAMANGLEX);
                else // Angle to right.
                    ang = angle(blobs->getBlob(i)->averageX() - (WIDTH / 2), WIDTH / 2, CAMANGLEX);
                sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", ";
                ballTape = i;
                break;
            }
        
        if(ballTape == -1) // If a ball was not found.
            sendBack += " , ,";
        
        if(ballTape == -1 && blobs->getNumBlobs() > 0)
        {
            for(int i = 0; i < 3 && i < blobs->getNumBlobs(); i++)
                if(blobs->getBlob(i)->averageY() > HEIGHT / 2)
                {
                    // Find the distance and angle to the ball.
                    // Put the two values in UDP string.
                    dist = distance(TAPEAREA, blobs->getBlob(i)->area(), HEIGHT / 2, WIDTH / 2);
                    if(blobs->getBlob(i)->averageX() < WIDTH / 2)// Angle to left.
                        ang = angle((WIDTH / 2) - blobs->getBlob(i)->averageX(), WIDTH, CAMANGLEX);
                    else // Angle to right.
                        ang = angle(blobs->getBlob(i)->averageX() - (WIDTH / 2), WIDTH / 2, CAMANGLEX);
                    sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", ";
			break;
                }
        }
        else
        {
            for(int i = 0; i < 3 && i < blobs->getNumBlobs(); i++)// Try to find the right side hatch vision tape.
            if(blobs->getBlob(i)->averageY() > HEIGHT / 2 && blobs->getBlob(i)->averageX() > blobs->getBlob(ballTape)->averageX())
            {
                // Find the distance and angle to the ball.
                // Put the two values in UDP string.
                dist = distance(TAPEAREA, blobs->getBlob(i)->area(), HEIGHT / 2, WIDTH / 2);
                if(blobs->getBlob(i)->averageX() < WIDTH / 2) // Angle to left.
                    ang = angle((WIDTH / 2) - blobs->getBlob(i)->averageX(), WIDTH / 2, CAMANGLEX);
                else // Angle to right.
                    ang = angle(blobs->getBlob(i)->averageX() - (WIDTH / 2), WIDTH / 2, CAMANGLEX);
                sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", ";
            }
            
            for(int i = 0; i < 3 && i < blobs->getNumBlobs(); i++) // Try to find left side hatch vision tape.
                if(blobs->getBlob(i)->averageY() > HEIGHT / 2 && blobs->getBlob(i)->averageX() < blobs->getBlob(ballTape)->averageX())
                {
                    // Find the distance and angle to the ball.
                    // Put the two values in UDP string.
                    dist = distance(TAPEAREA, blobs->getBlob(i)->area(), HEIGHT / 2, WIDTH / 2);
                    if(blobs->getBlob(i)->averageX() < WIDTH / 2) // Angle to left.
                        ang = angle((WIDTH / 2) - blobs->getBlob(i)->averageX(), WIDTH / 2, CAMANGLEX);
                    else // Angle to right.
                        ang = angle(blobs->getBlob(i)->averageX() - (WIDTH / 2), WIDTH / 2, CAMANGLEX);
                    sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", ";
                }
            

        }
    }
    else if(whichTarg == "BALL" && blobs->getNumBlobs() > 0)
    { // If you are looking for a ball.
        if(blobs->getNumBlobs() > 0)
        {
            dist = distance(BALLAREA, blobs->getBlob(0)->area(), HEIGHT / 2, WIDTH / 2);
            if(blobs->getBlob(0)->averageX() < WIDTH / 2) // Angle to left.
                ang = angle((WIDTH / 2) - blobs->getBlob(0)->averageX(), WIDTH / 2, CAMANGLEX);
            else // Angle to right.
                ang = angle(blobs->getBlob(0)->averageX() - (WIDTH / 2), WIDTH / 2, CAMANGLEX);
            sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", ";
        }
    }
    
    return sendBack;
}


// Function for putting to vision targets in one Blob.
// THIS FUNCTION IS SPECIFIC TO THE 2019 SEASON.
void calcHatchAndBall(Blobs * blobs)
{
    int ball = -1;
    // Calculate ball vision tape if there is any.
    for(int i = 0; i < 6 && i < blobs->getNumBlobs(); i++)
        if(blobs->getBlob(i)->averageY() < HEIGHT / 2) // If blob is in the top half.
        {
            for(int j = i + 1; j < 6 && j < blobs->getNumBlobs(); j++)
            {
                if(blobs->getBlob(j)->averageY() < HEIGHT / 2)
                    blobs->combineBlobs(i, j); // Combine blob(i) and blob(j) if
            }                                  // blob(j) is also in the top half.
            ball = i;
        }
    
    // Calculate the hatch vision tape.
    for(int i = 0; i < 6 && i < blobs->getNumBlobs(); i++)
        if(blobs->getBlob(i)->averageY() > HEIGHT / 2)
        {
            if(ball == -1)
            {
                for(int j = i + 1; j < blobs->getNumBlobs(); j++)
                    if(blobs->getBlob(j)->averageY() > HEIGHT / 2)
                        blobs->combineBlobs(i, j);
            }
            else
            {
                if(blobs->getBlob(i)->averageX() < blobs->getBlob(ball)->averageX())
                { // Check for and combine left side hatch vision tape.
                    for(int j = i + 1; j < blobs->getNumBlobs(); j++)
                        if(blobs->getBlob(j)->averageY() > HEIGHT / 2 && blobs->getBlob(j)->averageX() < blobs->getBlob(ball)->averageX())
                            blobs->combineBlobs(i, j);
                }
                else if(blobs->getBlob(i)->averageX() > blobs->getBlob(ball)->averageX())
                {// Check for and combine right side vision tape.
                    for(int j = i + 1; j < blobs->getNumBlobs(); j++)
                        if(blobs->getBlob(j)->averageY() > HEIGHT / 2 && blobs->getBlob(j)->averageX() > blobs->getBlob(ball)->averageX())
                            blobs->combineBlobs(i, j);
                }
            }
        }
            
}


/* Function to find the distance to an object given the height of the object
   in inches, the hieght of the object in pixels, number of pixels in the
   camera, and field of the view Y direction in degrees. */
float distance(float areaIn, float areaPix, float camArea, float camAngleY, float camAngleX)
{
    const float radConv = 3.14159265 / 180;
    return sqrt(((areaIn * camArea) / areaPix) * 4 * tan(camAngleY * radConv) * tan(camAngleX * radConv));
}


/* Function to find the angle to an object given the distance to the object
   the width in pixels, and the number of of pixels in the camera. */
float angle(float widthPix, float numPixInCam, float camAngleX)
{
    const float radConv = 3.14159265 / 180;
    return atan(tan(camAngleX * radConv) * (widthPix / numPixInCam)) / radConv;
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

