#include <iostream>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

void findQuadrangles(cv::Mat& input, std::vector<std::vector<cv::Point>>& output) {
	int imageArea = input.size().width * input.size().height;
	std::vector<std::vector<cv::Point>> contours;
	cv::Mat oneChannel(input.size(), CV_8U);

	cv::cvtColor(input, input, CV_BGR2HSV);
	cv::medianBlur(input, input, 9);

	for (int c = 0; c < input.channels(); c++) {
		int ch[] = { c, 0 };
		mixChannels(&input, 1, &oneChannel, 1, ch, 1);

		const int threshold_level = 2;
		for (int l = 0; l < threshold_level; l++) {
			if (l == 0) {
				Canny(oneChannel, oneChannel, 10, 20, 3);
				dilate(oneChannel, oneChannel, Mat(), Point(-1, -1));
			} else {
				oneChannel = oneChannel >= (l + 1) * 255 / threshold_level;
			}

			findContours(oneChannel, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
			
			vector<Point> approx;
			for (size_t i = 0; i < contours.size(); i++) {
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);
				double area = abs(contourArea(Mat(approx)));
				if (approx.size() == 4 && area > 0.2 * imageArea && area < 0.95 * imageArea && isContourConvex(Mat(approx))) {
					output.push_back(approx);
				}
			}
		}
	}
}

void drawQuadrangles(Mat& image, const vector<vector<Point>>& squares) {
	cout << squares.size() << endl;
	for (size_t i = 0; i < squares.size(); i++) {
		cout << "I = " << i << squares[i][0] << endl;
		cout << "I = " << i << squares[i][1] << endl;
		cout << "I = " << i << squares[i][2] << endl;
		cout << "I = " << i << squares[i][3] << endl;
		const Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		polylines(image, &p, &n, 1, true, Scalar(196 * i % 255, 64 + 196 * i % 255, 196 - 196 * i % 255), 3, CV_AA);
	}

	imshow("Squares", image);
}

int main(int argc, char** argv)
{
	auto input = cv::imread("pic1.jpg");

	std::vector<std::vector<cv::Point>> quadrangles;
	findQuadrangles(input.clone(), quadrangles);
	drawQuadrangles(input, quadrangles);

	waitKey(0);
	return 0; 
}
