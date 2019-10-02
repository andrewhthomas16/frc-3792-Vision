#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;


int main(int argv, char ** argc)
{
    //Read the image file
    Mat image = imread("/home/student/Pictures/Fin.png");

    //Check for failure
    if(image.empty())
    {
        cout << "Could not open or find the image" << endl;
        cin.get();
        return -1;
    }

    String windowName = "FIN";

    namedWindow(windowName);

    imshow(windowName, image);

    waitKey(0);

    destroyWindow(windowName);

    return 0;
}
