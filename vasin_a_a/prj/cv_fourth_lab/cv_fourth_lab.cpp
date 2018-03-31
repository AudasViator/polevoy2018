#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>

using namespace cv;

int main(int argc, char** argv)
{
	Mat src, dst;

	src = imread("src.png", CV_LOAD_IMAGE_GRAYSCALE);
	equalizeHist(src, src);

	Mat kernel;
	kernel = Mat::ones(1, 2, CV_32F);
	kernel.at<float>(0, 0) = -0.5;
	kernel.at<float>(0, 1) = 0.5;

	filter2D(src, dst, -1, kernel);

	Mat joinedImg(src.rows, src.cols * 2, CV_8U);
	src.copyTo(joinedImg(Rect(0, 0, src.cols, src.rows)));
	dst.copyTo(joinedImg(Rect(src.cols, 0, src.cols, src.rows)));

	imshow("L_4", joinedImg);
	imwrite("result/L_4.png", joinedImg);

	waitKey(0);

	return 0;
}
