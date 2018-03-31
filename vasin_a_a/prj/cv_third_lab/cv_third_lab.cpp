#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main() {
	Mat src = imread("src.png", CV_LOAD_IMAGE_GRAYSCALE);

	Mat hist;
	int channels[] = { 0 };
	float range[] = { 0, 255 };
	const float *ranges[] = { range };
	int hbins = 255;
	int histSize[] = { hbins };
	calcHist(&src, 1, channels, Mat(), hist, 1, histSize, ranges);

	bool left = false;
	bool right = false;
	int binLeft = 0, binRight = 255;
	float sumLeft = 0, sumRight = 0;

	for (int i = 0; i < hist.size[0]; i++) {
		if (!left && sumLeft / (src.cols * src.rows) < 0.2) {
			sumLeft += hist.at<float>(i);
			binLeft = i;
		} else {
			left = true;
		}

		if (!right && sumRight / (src.cols * src.rows) < 0.2) {
			sumRight += hist.at<float>(hist.size[0] - 1 - i);
			binRight = hist.size[0] - 1 - i;
		} else {
			right = true;
		}
	}

	uchar matchTable[256];
	for (int i = 0; i < 256; ++i) {
		if (i < binLeft) {
			matchTable[i] = (uchar)0;
		}
		else if (i > binRight) {
			matchTable[i] = (uchar)255;
		}
		else {
			matchTable[i] = (uchar)(255.0 / (binRight - binLeft) * (i - binLeft));
		}
	}
	
	Mat dst(src.clone());
	for (int i = 0; i < dst.rows; i++) {
		for (int j = 0; j < dst.cols; j++) {
			dst.at<uchar>(i, j) = matchTable[dst.at<uchar>(i, j)];
		}
	}

	Mat combinedImages(src.rows, src.cols * 2, CV_8U);
	src.copyTo(combinedImages(Rect(0, 0, src.cols, src.rows)));
	dst.copyTo(combinedImages(Rect(src.cols, 0, src.cols, src.rows)));

	imshow("L_3", combinedImages);
	imwrite("result/L_3.png", combinedImages);

	waitKey(0);

	return 0;
}
