#include <iostream>
#include <fstream>
#include <string>

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
		const Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		polylines(image, &p, &n, 1, true, Scalar(196 * i % 255, 64 + 196 * i % 255, 196 - 196 * i % 255), 3, CV_AA);
	}
	
	imshow("Squares", image);
}

void drawPolilyne(Mat& image, vector<Point>& points) {
	for (size_t i = 0; i < points.size(); i++) {
		const Point *pts[1] = { &points[0] };
		int npts[] = { points.size() };
		const Point* p = &points[0];
		int n = points.size();
		fillPoly(image, &p, &n, 1, Scalar(255));
	}
}

float getErrorForPicture(vector<Point>& trueQuadrangle, vector<Point>& predQuadrangle, int widthPicture, int heightPicture) {
	Mat trueMat(widthPicture, heightPicture, CV_8U, Scalar(0));
	Mat predMat(widthPicture, heightPicture, CV_8U, Scalar(0));
	Mat intrMat(widthPicture, heightPicture, CV_8U, Scalar(0));

	drawPolilyne(trueMat, trueQuadrangle);
	drawPolilyne(predMat, predQuadrangle);
	
	bitwise_and(trueMat, predMat, intrMat);

	int trueArea = countNonZero(trueMat);
	int predArea = countNonZero(predMat);
	int intrArea = countNonZero(intrMat);

	float nrmlzdTrueArea = (float)trueArea / (trueArea + predArea);
	float nrmlzdPredArea = (float)predArea / (trueArea + predArea);
	float nrmlzdIntrArea = (float)intrArea / (trueArea + predArea);

	cout << "TrueArea = " << trueArea << "; PredArea = " << predArea << "; IntrArea = " << intrArea << endl;
	cout << "nrmlzdTrueArea = " << nrmlzdTrueArea << "; nrmlzdPredArea = " << nrmlzdPredArea << "; nrmlzdIntrArea = " << nrmlzdIntrArea << endl;


	return 0.5f * nrmlzdPredArea + 1.5f * nrmlzdTrueArea - 2.0f * nrmlzdIntrArea;
}

std::vector<std::string> split(const std::string &s, char delim) {
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(std::move(item));
	}
	return elems;
}

struct Paper {
	String file_path;
	vector<Point> trueQuadrangle;
	vector<Point> predQuadrangle;
	float error;

	Paper(String file_path, vector<Point> trueQuadrangle, vector<Point> predQuadrangle, float error)
		: file_path(file_path), trueQuadrangle(trueQuadrangle), predQuadrangle(predQuadrangle), error(error)
	{}
};

vector<Paper> getPapers() {
	vector<Paper> papers;
	ifstream file("head.csv");

	 std::string line;
	 while (std::getline(file, line)) {
		 auto values = split(line, ';'); // file_name; p1x; p1y; ... ; p4x; p4y

		 string fileName = values[0];

		 int p1x = stoi(values[1]);
		 int p1y = stoi(values[2]);
		 int p2x = stoi(values[3]);
		 int p2y = stoi(values[4]);
		 int p3x = stoi(values[5]);
		 int p3y = stoi(values[6]);
		 int p4x = stoi(values[7]);
		 int p4y = stoi(values[8]);

		 vector<Point> trueQuadrangle;
		 trueQuadrangle.push_back(Point(p1x, p1y));
		 trueQuadrangle.push_back(Point(p2x, p2y));
		 trueQuadrangle.push_back(Point(p3x, p3y));
		 trueQuadrangle.push_back(Point(p4x, p4y));

		 vector<vector<Point>> output;

		 cout << fileName << endl;
		 Mat img = imread(fileName);
		 findQuadrangles(img, output);
		 vector<Point> predQuadrangle = output[0];

		 float error = getErrorForPicture(trueQuadrangle, predQuadrangle, img.cols, img.rows);

		 cout << "Error = " << error << endl << endl;

		 Paper paper(fileName, trueQuadrangle, predQuadrangle, error);
		 papers.push_back(paper);
	 }

	 return papers;
}

int main(int argc, char** argv)
{
	auto papers = getPapers();

	float totalError = 0.0f;
	for (int i = 0; i < papers.size(); i++) {
		totalError += papers[i].error;
	}

	cout << "TotalError = " << totalError << endl;

	waitKey(0);

	return 0; 
}
