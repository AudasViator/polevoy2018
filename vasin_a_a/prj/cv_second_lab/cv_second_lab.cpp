#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
	Mat srcImg = imread("src.png");
	cvtColor(srcImg, srcImg, CV_BGR2HSV);

	vector<Mat> channels;
	split(srcImg, channels);

	Mat rangedHue, rangedSaturation;
	Mat maskHue = channels[0] > 15 & channels[0] < 45; // Для 8-бит Hue от 0 до 180, берём от оранжевого до салатового
	Mat maskSaturation = channels[1] < 20;  // Ненасыщенные цвета

	srcImg.copyTo(rangedHue, maskHue);
	srcImg.copyTo(rangedSaturation, maskSaturation);

	Mat joinedImg(srcImg.rows, srcImg.cols * 3, CV_8UC3);

	srcImg.copyTo(joinedImg(Rect(0, 0, srcImg.cols, srcImg.rows)));
	rangedHue.copyTo(joinedImg(Rect(srcImg.cols, 0, srcImg.cols, srcImg.rows)));
	rangedSaturation.copyTo(joinedImg(Rect(2 * srcImg.cols, 0, srcImg.cols, srcImg.rows)));

	cvtColor(joinedImg, joinedImg, CV_HSV2BGR);

	imwrite("result/L_2.png", joinedImg);
	imshow("L_2", joinedImg);

	waitKey(0);

	return 0;
}
