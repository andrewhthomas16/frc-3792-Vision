// final.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include <ctime>
#include "Blobs.h"
#include "Blob.h"
#include "Point2i.h"
#include "UDP.h"
#include "LookUp.h"

struct tapeLine // Struct to combine a white line with its
{               // corresponding vision tape.
    Blob * tape;
    Blob * line;
};

std::string sendBackData(Blobs * blobs, std::vector<tapeLine> * combos, std::string whichTarg);
void findCombos(Blobs * blobs, std::vector<tapeLine> * combos);
void filter(cv::Scalar lowerRange, cv::Scalar upperRange, cv::Mat & Inputimage, cv::Mat & outputImage);
float distance(float areaIn, float areaPix, float camArea, float camAngleY, float camAngleX);
float facingAngle(float height, float width);
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
          THRESH = 10,
          DIST = 5,
          AREA = 10;

// Values for UDP
const char * IP = "10.37.92.43",
           * PORT= "5800";

// For calculating distance and angle.
const float TAPEAREA = 22,
            BALLAREA = 176.714587,
            CAMANGLEY = 20.25,
            CAMANGLEX = 27,
            CAMAREA = 19200,
            DISTSCALE = 1.2;

//For calculating combos.
const float TAPEYPERCENT = 0.1,
            TAPEXAWAY = 40,
            TAPELINEAWAY = 10,
            LINESTOPROWS = 5;



int main(int argc, char * argv[])
{
    cv::String windowNameRaw = "VideoFeedRaw", windowNameAfter = "VideoFeedAfter";
    cv::Mat * image = new cv::Mat();
    Blobs blobs(image, THRESH, DIST, AREA);
    std::vector<tapeLine> comboBlobs;
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
        filter(cv::Scalar(40, 0, 190), cv::Scalar(180, 255, 255), *image, *image);
        
        // Blur image to get rid of the bad data points.
        if(BLUR)
            cv::GaussianBlur(*image, *image, cv::Size(9, 9), 2, 2);

        blobs.calcBlobs();
        
        if(TEST)
            cv::putText(*image, std::to_string(CLOCKS_PER_SEC / (clock() - fps)), cv::Point2f(10, 10), cv::FONT_HERSHEY_PLAIN, 0.8, cv::Scalar(255, 255, 255));

        if(HATCH) // SPECIFIC TO 2019.
            findCombos(& blobs, & comboBlobs);
        
        // Send data back by getting string from sendBackData() and
        // converting result to char *.
        if(RIO)
            std::cout << sendBackData(& blobs, & comboBlobs, "TAPE") << std::endl;

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


// Function to compile information to send over UDP.
std::string sendBackData(Blobs * blobs, std::vector<tapeLine> * combos, std::string whichTarg)
{
    std::transform(whichTarg.begin(), whichTarg.end(), whichTarg.begin(), ::toupper);
    
    std::string sendBack;
    int i = 1;
    int ballTape = -1;
    float dist, ang, facingAng;
    
    if(combos->size() == 1) // There is only one pair of vision tape and line.
    {
        // distance
        dist = DISTSCALE * distance(TAPEAREA, combos->at(0).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
        // angle.
        ang = angle(combos->at(0).tape->width(), CAMAREA, CAMANGLEX);
        // facing angle
        facingAng = facingAngle(combos->at(0).line->height(), combos->at(0).line->width());
        sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng);
    }
    else if(combos->size() == 2) // There is are two pairs of vision tape and line.
    {
        if(combos->at(0).tape->average(). y > combos->at(1).tape->average().y)
        { // The first element is the ball port.
            // distance
            dist = DISTSCALE * distance(TAPEAREA, combos->at(0).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
            // angle
            ang = angle(combos->at(0).tape->width(), CAMAREA, CAMANGLEX);
            // facing angle
            facingAng = facingAngle(combos->at(0).line->height(), combos->at(0).line->width());
            sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng) + ", ";
            // distance
            dist = DISTSCALE * distance(TAPEAREA, combos->at(1).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
            // angle
            ang = angle(combos->at(1).tape->width(), CAMAREA, CAMANGLEX);
            // facing angle
            facingAng = facingAngle(combos->at(1).line->height(), combos->at(1).line->width());
            sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng);
        }
        else if(combos->at(1).tape->average(). y > combos->at(0).tape->average().y)
        { // The first element is the hatch port.
            // distance
            dist = DISTSCALE * distance(TAPEAREA, combos->at(1).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
            // angle
            ang = angle(combos->at(1).tape->width(), CAMAREA, CAMANGLEX);
            // facing angle
            facingAng = facingAngle(combos->at(1).line->height(), combos->at(1).line->width());
            sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng) + ", ";
            // distance
            dist = DISTSCALE * distance(TAPEAREA, combos->at(0).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
            // angle
            ang = angle(combos->at(0).tape->width(), CAMAREA, CAMANGLEX);
            // facing angle
            facingAng = facingAngle(combos->at(0).line->height(), combos->at(0).line->width());
            sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng);
        }
        else
        { // Your looking at the cargo ship.
            int index = 0;
            for(int i = 1; i < combos->size(); i++)
            {
                if(std::abs(combos->at(i).tape->average().x - (WIDTH / 2)) < std::abs(combos->at(index).tape->average().x - (WIDTH / 2)))
                    index = i;
            }
            
            //distance
            dist = DISTSCALE * distance(TAPEAREA, combos->at(index).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
            // angle
            ang = angle(combos->at(index).tape->width(), CAMAREA, CAMANGLEX);
            // facing angle
            facingAng = facingAngle(combos->at(index).line->height(), combos->at(index).line->width());
            sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng);
        }
            
    }
    else if(combos->size() > 2) // There is are more than two pairs of
    {                           // vision tape and line.
        bool ballHatchFound = false;
        for(int i = 0; i < combos->size() && !ballHatchFound; i++)
            for(int j = i + 1; j < combos->size() && !ballHatchFound; j++)
            {
                if(combos->at(i).tape->average(). y > combos->at(j).tape->average().y)
                { // i is ball, j is hatch.
                    // distance
                    dist = DISTSCALE * distance(TAPEAREA, combos->at(i).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
                    // angle
                    ang = angle(combos->at(i).tape->width(), CAMAREA, CAMANGLEX);
                    // facing angle
                    facingAng = facingAngle(combos->at(i).line->height(), combos->at(i).line->width());
                    sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng) + ", ";
                    // distance
                    dist = DISTSCALE * distance(TAPEAREA, combos->at(j).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
                    // angle
                    ang = angle(combos->at(j).tape->width(), CAMAREA, CAMANGLEX);
                    // facing angle
                    facingAng = facingAngle(combos->at(j).line->height(), combos->at(j).line->width());
                    sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng);

                    ballHatchFound = true;
                }
                else if (combos->at(j).tape->average().y > combos->at(i).tape->average().y)
                { // j is ball, i is hatch.
                    // distance
                    dist = DISTSCALE * distance(TAPEAREA, combos->at(j).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
                    // angle
                    ang = angle(combos->at(j).tape->width(), CAMAREA, CAMANGLEX);
                    // facing angle
                    facingAng = facingAngle(combos->at(j).line->height(), combos->at(j).line->width());
                    sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng) + ", ";
                    // distance
                    dist = DISTSCALE * distance(TAPEAREA, combos->at(i).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
                    // angle
                    ang = angle(combos->at(i).tape->width(), CAMAREA, CAMANGLEX);
                    // facing angle
                    facingAng = facingAngle(combos->at(i).line->height(), combos->at(i).line->width());
                    sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng);

                    ballHatchFound = true;
                }
            }
        
        if(!ballHatchFound)
        { // Your looking at the cargo ship.
            int index = 0;
            for(int i = 1; i < combos->size(); i++)
            {
                if(std::abs(combos->at(i).tape->average().x - (WIDTH / 2)) < std::abs(combos->at(index).tape->average().x - (WIDTH / 2)))
                    index = i;
            }
            
            //distance
            dist = DISTSCALE * distance(TAPEAREA, combos->at(index).tape->area(), CAMAREA, CAMANGLEY, CAMANGLEX);
            // angle
            ang = angle(combos->at(index).tape->width(), CAMAREA, CAMANGLEX);
            // facing angle
            facingAng = facingAngle(combos->at(index).line->height(), combos->at(index).line->width());
            sendBack += std::to_string(dist) + ", " + std::to_string(ang) + ", " + std::to_string(facingAng) + ", 0, 0, 0";
        }
    }
    else
        sendBack = "0, 0, 0, 0, 0, 0";

	return sendBack;
}


// Function for putting to vision targets in one Blob.
// THIS FUNCTION IS SPECIFIC TO THE 2019 SEASON.
void findCombos(Blobs * blobs, std::vector<tapeLine> * combos)
{
    combos->clear();
    for(int i = 0; i < blobs->getNumBlobs(); i++) // Combine vision tapes.
        for(int j = i + 1; j < blobs->getNumBlobs(); j++)
            if(std::abs(blobs->getBlob(i)->average().y - blobs->getBlob(j)->average().y) < blobs->getBlob(i)->height() * TAPEYPERCENT && std::abs(blobs->getBlob(i)->average().x - blobs->getBlob(j)->average().x) < TAPEXAWAY)
                blobs->combineBlobs(i, j);
    
    for(int i = 0; i < blobs->getNumBlobs() - 1; i++) // Create tape line combos.
        for(int j = i + 1; j < blobs->getNumBlobs(); j++)
        {
            if(std::abs(blobs->getBlob(i)->average().x - blobs->getBlob(j)->topRowsAverageX(LINESTOPROWS)) < TAPELINEAWAY)
            {
                if(blobs->getBlob(i)->average().y < blobs->getBlob(j)->average().y)
                    combos->push_back({ blobs->getBlob(i), blobs->getBlob(j) });
                else
                    combos->push_back({ blobs->getBlob(j), blobs->getBlob(i) });
            }
            else if(std::abs(blobs->getBlob(i)->topRowsAverageX(LINESTOPROWS) - blobs->getBlob(j)->average().x) < TAPELINEAWAY)
            {
                if(blobs->getBlob(i)->average().y < blobs->getBlob(j)->average().y)
                    combos->push_back({ blobs->getBlob(i), blobs->getBlob(j) });
                else
                    combos->push_back({ blobs->getBlob(j), blobs->getBlob(i) });
            }
        }
}


/* Function to find the distance to an object given the height of the object
   in inches, the hieght of the object in pixels, number of pixels in the
   camera, and field of the view Y direction in degrees. */
float distance(float areaIn, float areaPix, float camArea, float camAngleY, float camAngleX)
{
    const float radConv = 3.14159265 / 180;
    return sqrt(((areaIn * camArea) / areaPix) / (4 * tan(camAngleY * radConv) * tan(camAngleX * radConv)));
}


/* Function to find the angle to an object given the distance to the object
   the width in pixels, and the number of of pixels in the camera. */
float angle(float widthPix, float numPixInCam, float camAngleX)
{
    const float radConv = 3.14159265 / 180;
    return atan(tan(camAngleX * radConv) * (widthPix / numPixInCam)) / radConv;
}


/* Function to find the facing angle based off of the bounding box of the white
   lines in the 2019 frc game.*/
float facingAngle(float height, float width)
{
    const float radConv = 3.14159265 / 180;
    return atan((height / width) * radConv);
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


