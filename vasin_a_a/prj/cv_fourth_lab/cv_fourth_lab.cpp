#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

int main(int argc, char** argv)
{
	/// Declare variables
	Mat src, dst;

	Mat kernel;
	Point anchor;
	double delta;
	int ddepth;
	int kernel_size;
	char* window_name = "filter2D Demo";


	src = imread("pic1.jpg");
	cvtColor(src, src, CV_BGR2GRAY);
	equalizeHist(src, src);

	if (!src.data)
	{
		return -1;
	}

	/// Create window
	namedWindow(window_name, CV_WINDOW_AUTOSIZE);

	/// Initialize arguments for the filter
	anchor = Point(-1, -1);
	delta = 0;
	ddepth = -1;

	kernel = Mat::ones(1, 2, CV_32F);
	kernel.at<float>(0, 0) = -0.5;
	kernel.at<float>(0, 1) = 0.5;

	//kernel = Mat::ones(2, 1, CV_32F);
	//kernel.at<float>(0, 0) = 0.5;
	//kernel.at<float>(1, 0) = -0.5;

	/// Apply filter
	filter2D(src, dst, ddepth, kernel, anchor, delta, BORDER_DEFAULT);
	imshow(window_name, dst);
	imshow("src", src);

	waitKey(0);

	return 0;
}