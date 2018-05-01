#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

/*
	Some usefull structures
*/

struct Paper {
	String file_path;
	vector<Point> trueQuadrangle;
	vector<Point> predQuadrangle;
	float error;

	Paper(String file_path, vector<Point> trueQuadrangle, vector<Point> predQuadrangle, float error)
		: file_path(file_path), trueQuadrangle(trueQuadrangle), predQuadrangle(predQuadrangle), error(error)
	{}
};

struct TestResult {
	double epsF;
	double thr1;
	double thr2;
	float totalError;

	TestResult(double epsF, double thr1, double thr2, float totalError)
		: epsF(epsF), thr1(thr1), thr2(thr2), totalError(totalError)
	{}
};

/*
	Some useless functions
*/

void drawQuadrangles(Mat& image, const vector<vector<Point>>& squares) {
	cout << squares.size() << endl;
	for (size_t i = 0; i < squares.size(); i++) {
		const Point* p = &squares[i][0];
		int n = (int)squares[i].size();
		polylines(image, &p, &n, 1, true, Scalar(196 * i % 255, 64 + 196 * i % 255, 196 - 196 * i % 255), 3, CV_AA);
	}

	imshow("Quadrangles", image);
}

void drawQuadrangle(Mat& image, const vector<Point>& squares) {
	const Point* p = &squares[0];
	int n = (int)squares.size();
	polylines(image, &p, &n, 1, true, Scalar(196, 64, 196), 3, CV_AA);

	imshow("Qudrangle", image);
}

void drawAndSaveQuadrangle(Mat& image, const vector<Point>& squares, string fileName) {
	const Point* p = &squares[0];
	int n = (int)squares.size();
	polylines(image, &p, &n, 1, true, Scalar(196, 64, 196), 3, CV_AA);

	imwrite(fileName, image);
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

/*
	Find quadrangles on input image and save to output vector
*/
void findQuadrangles(cv::Mat& input, std::vector<std::vector<cv::Point>>& output, double threshold1, double threshould2, double epsFactor) {
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
				Canny(oneChannel, oneChannel, threshold1, threshould2, 3);
				dilate(oneChannel, oneChannel, Mat(), Point(-1, -1));
			} else {
				oneChannel = oneChannel >= (l + 1) * 255 / threshold_level;
			}

			findContours(oneChannel, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
			
			vector<Point> approx;
			for (size_t i = 0; i < contours.size(); i++) {
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * epsFactor, true);
				double area = abs(contourArea(Mat(approx)));
				if (approx.size() == 4 && area > 0.1 * imageArea && area < 0.95 * imageArea && isContourConvex(Mat(approx))) {
					output.push_back(approx);
				}
			}
		}
	}
}

/*
	Return float value from 0.0f to 1.0f where 1.0f means max error: no interception between real and prediction quadrangles
*/
float getErrorForPicture(vector<Point>& trueQuadrangle, vector<Point>& predQuadrangle, int widthPicture, int heightPicture) {
	Mat trueMat(heightPicture, widthPicture, CV_8U, Scalar(0));
	Mat predMat(heightPicture, widthPicture, CV_8U, Scalar(0));
	Mat intrMat(heightPicture, widthPicture, CV_8U, Scalar(0));

	drawPolilyne(trueMat, trueQuadrangle);
	drawPolilyne(predMat, predQuadrangle);
	
	bitwise_and(trueMat, predMat, intrMat);

	int trueArea = countNonZero(trueMat);
	int predArea = countNonZero(predMat);
	int intrArea = countNonZero(intrMat);

	float nrmlzdIntrArea = (float)intrArea / (trueArea + predArea);

	return 1.0f - 2.0f * nrmlzdIntrArea;
}

/*
	Split string to strings vector by char
*/
vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> elems;
	while (getline(ss, item, delim)) {
		elems.push_back(move(item));
	}
	return elems;
}

/*
	Do everything:
		read informations about pictures from file head.csv,
		try to find quadrangles/papers on each picture and calculate errors for them
*/
vector<Paper> getPapers(double threshold1, double threshould2, double epsFactor) {
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

		 //cout << fileName << endl;
		 Mat img = imread(fileName);
		 findQuadrangles(img, output, threshold1, threshould2, epsFactor);

		 float error = 1.0f;
		 vector<Point> predQuadrangle;

		 if (output.size() > 0) {
			predQuadrangle = output[0];

			//drawQuadrangle(img, predQuadrangle);
			//drawQuadrangle(img, trueQuadrangle);

			//cv::cvtColor(img, img, CV_HSV2BGR);
			//drawAndSaveQuadrangle(img, predQuadrangle, fileName + ".jpg");
			error = getErrorForPicture(trueQuadrangle, predQuadrangle, img.cols, img.rows);
		 }

		 //cout << "Error = " << error << endl << endl;

		 Paper paper(fileName, trueQuadrangle, predQuadrangle, error);
		 papers.push_back(paper);
	 }

	 return papers;
}

cv::Mutex mutex;

int main(int argc, char** argv)
{
	//getPapers(10, 40, 0.05);
	//return 0;

	vector<thread> threads;
	vector<TestResult> results;

	for (double epsF = 0.01; epsF < 0.10; epsF += 0.01) {
		threads.push_back(thread([&results, epsF] { // Simple implementation of thread executors
			for (double thr1 = 0; thr1 < 80; thr1 += 10) {
				for (double thr2 = thr1; thr2 < 100; thr2 += 10) {
					auto papers = getPapers(thr1, thr2, epsF);

					float totalError = 0.0f;
					for (int i = 0; i < papers.size(); i++) {
						totalError += papers[i].error;
					}

					mutex.lock();
					cout << "TotalError = " << totalError << endl;
					results.push_back(TestResult(epsF, thr1, thr2, totalError));
					mutex.unlock();
				}
			}
		}));
	}

	cout << threads.size() << " threads" << endl;
	for (int i = 0; i < threads.size(); i++) {
		threads[i].join();
	}

	sort(begin(results), end(results), [](TestResult a, TestResult b) {return a.totalError < b.totalError; }); // From smallest error
	
	for each (auto testResult in results) {
		cout << testResult.totalError << ';' << testResult.epsF << ';' << testResult.thr1 << ';' << testResult.thr2 << endl;
	}

	return 0; 
}
