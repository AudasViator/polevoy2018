#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

const int marginFactor = 6;

int getMarginX(const Point& leftTop, const Point& rightBottom) {
	return (rightBottom.x - leftTop.x) / marginFactor;
}

int getMarginY(const Point& leftTop, const Point& rightBottom) {
	return (rightBottom.y - leftTop.y) / marginFactor;
}

void drawCircle(Mat& mat, const Point& leftTop, const Point& rightBottom, const Scalar& color) {
	auto center =  Point((leftTop.x + rightBottom.x) / 2, (leftTop.y + rightBottom.y) / 2);
	auto radius = (rightBottom.x - leftTop.x - 2 * getMarginX(leftTop, rightBottom)) / 2;
	circle(mat, center, radius, color, CV_FILLED);
}

void drawRectangle(Mat& mat, const Point& leftTop, const Point& rightBottom, const Scalar& color, bool withMargin) {
	auto rectLeftTop = Point(leftTop.x + (withMargin ? getMarginX(leftTop, rightBottom) : 0), leftTop.y + (withMargin ? getMarginY(leftTop, rightBottom) : 0));
	auto rectRightBottom = Point(rightBottom.x - (withMargin ? getMarginX(leftTop, rightBottom) : 0), rightBottom.y - (withMargin ? getMarginY(leftTop, rightBottom) : 0));
	rectangle(mat, rectLeftTop, rectRightBottom, color, CV_FILLED);
}

void drawTriangle(Mat& mat, const Point& leftTop, const Point& rightBottom, const Scalar& color) {
	auto triTop = Point((leftTop.x + rightBottom.x) / 2, leftTop.y + getMarginY(leftTop, rightBottom));
	auto triLeft = Point(leftTop.x + getMarginX(leftTop, rightBottom), rightBottom.y - getMarginY(leftTop, rightBottom));
	auto triRight = Point(rightBottom.x - getMarginX(leftTop, rightBottom), rightBottom.y - getMarginY(leftTop, rightBottom));

	Point points[3] = { triLeft, triTop, triRight };
	const Point *pts[1] = { points };
	int npts[] = { 3 };

	fillPoly(mat, pts, npts, 1, color);
}

int main() {
	const Scalar BLACK = Scalar(0);
	const Scalar GRAY = Scalar(128);
	const Scalar WHITE = Scalar(255);

	const Scalar backgroundColors[6][3] = {
		{ BLACK, GRAY, WHITE },
		{ GRAY, WHITE, BLACK },
		{ WHITE, BLACK, GRAY },
		{ BLACK, GRAY, WHITE },
		{ GRAY, WHITE, BLACK },
		{ WHITE, BLACK, GRAY }
	};

	const Scalar figureColors[6][3] = {
		{ GRAY, WHITE, BLACK },
		{ WHITE, BLACK, GRAY },
		{ BLACK, GRAY, WHITE },
		{ WHITE, BLACK, GRAY },
		{ BLACK, GRAY, WHITE },
		{ GRAY, WHITE, BLACK }
	};

	Mat img(600, 300, CV_8UC1);

	const int stepPx = 300 / 3;
	for (int j = 0; j < 6; j++) {
		for (int i = 0; i < 3; i++) {
			drawRectangle(img, Point(i * stepPx, j * stepPx), Point((i + 1) * stepPx, (j + 1) * stepPx), backgroundColors[j][i], false);

			if (i % 3 == 0) {
				drawCircle(img, Point(i * stepPx, j * stepPx), Point((i + 1) * stepPx, (j + 1) * stepPx), figureColors[j][i]);
			} else if ((i + 1) % 3 == 0) {
				drawRectangle(img, Point(i * stepPx, j * stepPx), Point((i + 1) * stepPx, (j + 1) * stepPx), figureColors[j][i], true);
			} else {
				drawTriangle(img, Point(i * stepPx, j * stepPx), Point((i + 1) * stepPx, (j + 1) * stepPx), figureColors[j][i]);
			}
		}
	}


	Mat kernel(3, 3, CV_32FC1);

	kernel.at<float>(0, 0) = 1;
	kernel.at<float>(1, 0) = 3;
	kernel.at<float>(2, 0) = 1;

	kernel.at<float>(0, 1) = 0;
	kernel.at<float>(1, 1) = 0;
	kernel.at<float>(2, 1) = 0;

	kernel.at<float>(0, 2) = -1;
	kernel.at<float>(1, 2) = -3;
	kernel.at<float>(2, 2) = -1;


	Mat derHor, derVer;
	filter2D(img, derHor, CV_32F, kernel);
	filter2D(img, derVer, CV_32F, kernel.t());

	Mat derHorSquared, derVerSquared;
	pow(derHor, 2, derHorSquared);
	pow(derVer, 2, derVerSquared);

	Mat absGrad;
	pow(derHorSquared + derVerSquared, 0.5, absGrad);
	
	derHor.convertTo(derHor, CV_8U);
	derVer.convertTo(derVer, CV_8U);
	absGrad.convertTo(absGrad, CV_8U);

	Mat readyGrad;
	Mat channels[3] = { derVer, derHor, absGrad };
	merge(channels, 3, readyGrad);
	cvtColor(readyGrad, readyGrad, CV_HSV2BGR);

	Mat rgbImg;
	cvtColor(img, rgbImg, CV_GRAY2BGR);

	Mat joinedImg(img.rows, img.cols * 2, CV_8UC3);
	rgbImg.copyTo(joinedImg(Rect(0, 0, img.cols, img.rows)));
	readyGrad.copyTo(joinedImg(Rect(img.cols, 0, img.cols, img.rows)));

	imshow("L_5_KR", joinedImg);
	imwrite("result/L_5_KR.png", joinedImg);

	waitKey(0);

	return 0;
}