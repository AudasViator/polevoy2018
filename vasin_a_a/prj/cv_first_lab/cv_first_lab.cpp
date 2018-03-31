#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {
	const int imgSize = 600;
	const float sizeFactor = 0.1f;

	Mat m(imgSize, imgSize, CV_8UC3);

	for (int i = 0; i < m.cols; i++) {
		for (int j = 0; j < m.rows; j++) {
			m.at<Vec3b>(i, j)[0] = (uchar)(sin(i * sizeFactor) * 128 + 128);
			m.at<Vec3b>(i, j)[1] = (uchar)(sin(j * sizeFactor) * 128 + 128);
			m.at<Vec3b>(i, j)[2] = (uchar)((sin(i * sizeFactor) + sin(j * sizeFactor)) * 128 + 128);
		}
	}

	imwrite("result/L_1.png", m);

	imshow("L_1", m);

	waitKey(0);

	return 0;
}
