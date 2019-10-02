#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
	cv::VideoCapture cap(0);
	cv::Mat image;
	cv::String windowName = "videofeed";
	//cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

	while(true)
	{
		cap.grab();
		cap.retrieve(image);
		std::cout << "x" << std::endl;
		cv::imshow("videoFeed", image);
		if(cv::waitKey(30) >= 0)
			break;
	}

	//cv::waitKey(0);
	cv::destroyWindow(windowName);

	return 0;
}
