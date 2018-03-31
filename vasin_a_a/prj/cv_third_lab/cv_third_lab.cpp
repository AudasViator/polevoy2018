#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

int main() {
	Mat sourceImage = imread("src.png", CV_LOAD_IMAGE_GRAYSCALE);

	Mat hist;

	int channels[] = { 0 };

	float range_0[] = { 0, 255 };
	const float *ranges[] = { range_0 };

	int hbins = 255;
	int histSize[] = { hbins };

	calcHist(&sourceImage, 1, channels, Mat(), hist, 1, histSize, ranges);

	bool left = false;
	bool right = false;

	int quantileLeft = 0, quantileRight = 255;
	float sumLeft = 0, sumRight = 0;

	for (int i = 0; i < hist.size[0]; i++) {

		if (!left && sumLeft / (sourceImage.size[0] * sourceImage.size[1]) < 0.2) {
			sumLeft += hist.at<float>(i);
			quantileLeft = i;
		}
		else {
			left = true;
		}

		if (!right && sumRight / (sourceImage.size[0] * sourceImage.size[1]) < 0.2) {
			sumRight += hist.at<float>(hist.size[0] - 1 - i);
			quantileRight = hist.size[0] - 1 - i;
		}
		else {
			right = true;
		}
	}

	uchar table[256];

	for (int i = 0; i < 256; ++i) {
		if (i < quantileLeft) {
			table[i] = (uchar)0;
		}
		else if (i > quantileRight) {
			table[i] = (uchar)255;
		}
		else {
			table[i] = (uchar)(255.0 / (quantileRight - quantileLeft) * (i - quantileLeft));
		}
	}

	Mat combinedImages(sourceImage.rows, sourceImage.cols * 2, CV_8UC1);
	sourceImage.copyTo(combinedImages(Rect(0, 0, sourceImage.cols, sourceImage.rows)));

	for (int i = 0; i < combinedImages.rows; i++) {
		for (int j = 0; j < combinedImages.cols / 2; j++) {
			combinedImages.at<uchar>(i, j + sourceImage.cols) = table[sourceImage.at<uchar>(i, j)];
		}
	}

	imwrite("result/L_3.png", combinedImages);
	imshow("L_3", combinedImages);

	waitKey(0);

	return 0;
}
