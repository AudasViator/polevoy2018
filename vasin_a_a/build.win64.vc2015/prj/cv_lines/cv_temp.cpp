#include <algorithm>
#include <iterator>
#include <limits>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

void biggest_contour()
{
		cv::Mat input = cv::imread("pic1.jpg");

		cv::Mat hsv;
		cv::cvtColor(input, hsv, CV_BGR2HSV);

		std::vector<cv::Mat> channels;
		cv::split(hsv, channels);

		cv::Mat hChannel = channels[0];

		// compute mask (you could use a simple threshold if the image is always as good as the one you provided)
		cv::Mat mask;
		//cv::threshold(hChannel, mask, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
		cv::adaptiveThreshold(hChannel, mask, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 12);

		// find contours (if always so easy to segment as your image, you could just add the black/rect pixels to a vector)
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(mask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

		vector<Vec4i> lines;
		HoughLinesP(mask, lines, 1, CV_PI / 180, 50, 200, 20);
		std::cout << lines.size();
		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			line(input, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
		}

		// display
		cv::imshow("input", input);
		cv::imshow("drawing", mask);
		cv::waitKey(0);

		cv::imwrite("rotatedRect.png", input);

		return;


		/// Draw contours and find biggest contour (if there are other contours in the image, we assume the biggest one is the desired rect)
		// drawing here is only for demonstration!
		int biggestContourIdx = -1;
		float biggestContourArea = 0;
		//cv::Mat drawing = cv::Mat::zeros(mask.size(), CV_8UC3);
		cout << contours.size();
		for (int i = 0; i< contours.size(); i++)
		{
			cv::Scalar color = cv::Scalar(128, 0, 0);
			drawContours(input, contours, i, color, 10, 8, hierarchy, 0, cv::Point());

			float ctArea = cv::contourArea(contours[i]);
			if (ctArea > biggestContourArea)
			{
				biggestContourArea = ctArea;
				biggestContourIdx = i;
			}
		}

		// if no contour found
		if (biggestContourIdx < 0)
		{
			std::cout << "no contour found" << std::endl;
			return;
		}
		drawContours(input, contours, biggestContourIdx, cv::Scalar(0, 0, 128), 10, 8, hierarchy, 0, cv::Point());
		// compute the rotated bounding rect of the biggest contour! (this is the part that does what you want/need)
		cv::RotatedRect boundingBox = cv::minAreaRect(contours[biggestContourIdx]);
		// one thing to remark: this will compute the OUTER boundary box, so maybe you have to erode/dilate if you want something between the ragged lines


		// draw the rotated rect
		cv::Point2f corners[4];
		boundingBox.points(corners);
		cv::line(input, corners[0], corners[1], cv::Scalar(0, 255, 255));
		cv::line(input, corners[1], corners[2], cv::Scalar(0, 255, 255));
		cv::line(input, corners[2], corners[3], cv::Scalar(0, 255, 255));
		cv::line(input, corners[3], corners[0], cv::Scalar(0, 255, 255));

		// display
		cv::imshow("input", input);
		cv::imshow("drawing", mask);
		cv::waitKey(0);

		cv::imwrite("rotatedRect.png", input);
}

void test() {
	Mat sample = imread("pic1.jpg");
	imshow("window", sample);
	waitKey(0);

	// images to work on
	Mat black = Mat(sample.rows, sample.cols, CV_8UC1, Scalar(0));
	Mat clone = sample.clone();

	cv::Mat hsv;
	cv::cvtColor(sample, hsv, CV_BGR2HSV);

	std::vector<cv::Mat> channels;
	cv::split(hsv, channels);

	cv::Mat gray = channels[0];

	// binarization
	//Mat gray;
	cvtColor(sample, gray, CV_BGR2GRAY);
	//threshold(gray, gray, 0, 255, CV_THRESH_OTSU);
	cv::adaptiveThreshold(gray, gray, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 11, 12);

	// find and fill the largest contour
	vector<vector<Point> > contours;
	vector<double> areas;
	findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	for (unsigned int i = 0; i < contours.size(); i++)
	{
		areas.push_back(abs(contourArea(contours[i])));
	}
	vector<double>::iterator biggest = max_element(areas.begin(), areas.end());
	unsigned int ID = distance(areas.begin(), biggest);

	drawContours(black, contours, ID, Scalar(255), -1);
	imshow("window", black);
	waitKey(0);

	// get convexity defects of thelargest contour
	vector<Point> external = contours[ID];
	vector<int> hull;
	vector<Vec4i> defects;
	convexHull(external, hull);
	convexityDefects(external, hull, defects);

	// show defect points
	for (unsigned int i = 0; i < defects.size(); i++)
	{
		circle(clone, external[defects[i][1]], 1, Scalar(0, 255, 255), 3);
	}
	imshow("window", clone);
	waitKey(0);

	// find extremes
	Point tl, tr, bl, br;
	Point p;
	double d_tl, d_tr, d_bl, d_br;
	double m_tl = numeric_limits<double>::max();
	double m_tr = numeric_limits<double>::max();
	double m_bl = numeric_limits<double>::max();
	double m_br = numeric_limits<double>::max();
	for (unsigned int i = 0; i < defects.size(); i++)
	{
		p = external[defects[i][2]];
		d_tl = (double)sqrt((double)pow((double)(p.x), 2) + pow((double)(p.y), 2));
		d_tr = (double)sqrt((double)pow((double)(sample.cols - p.x), 2) + pow((double)(p.y), 2));
		d_bl = (double)sqrt((double)pow((double)(p.x), 2) + pow((double)(sample.rows - p.y), 2));
		d_br = (double)sqrt((double)pow((double)(sample.cols - p.x), 2) + pow((double)(sample.rows - p.y), 2));
		if (d_tl < m_tl)
		{
			tl = p;
			m_tl = d_tl;
		}
		if (d_tr < m_tr)
		{
			tr = p;
			m_tr = d_tr;
		}
		if (d_bl < m_bl)
		{
			bl = p;
			m_bl = d_bl;
		}
		if (d_br < m_br)
		{
			br = p;
			m_br = d_br;
		}
	}

	// draw rectangle
	line(sample, tl, tr, Scalar(0, 255, 255), 3);
	line(sample, tr, br, Scalar(0, 255, 255), 3);
	line(sample, br, bl, Scalar(0, 255, 255), 3);
	line(sample, bl, tl, Scalar(0, 255, 255), 3);
	imshow("window", sample);
	waitKey(0);

	return;
}

int canny(int argc, char** argv)
{
	const char* filename = argc >= 2 ? argv[1] : "pic1.jpg";

	Mat src = imread(filename, 0);
	if (src.empty())
	{
		cout << "can not open " << filename << endl;
		return -1;
	}

	//cv::Mat hsv;
	//cv::cvtColor(src, hsv, CV_BGR2HSV);

	//std::vector<cv::Mat> channels;
	//cv::split(hsv, channels);

	Mat dst, cdst;
	Canny(src, dst, 50, 200, 3);
	cvtColor(dst, cdst, CV_GRAY2BGR);


	vector<Vec4i> lines;
	HoughLinesP(dst, lines, 1, CV_PI / 180, 100, 100, 10);
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
	}

	imshow("source", src);
	imshow("detected lines", cdst);

	waitKey();

	return 0;
}

int lines()
{
	cv::Mat input = cv::imread("pic1.jpg");

	cv::Mat hsv;
	cv::cvtColor(input, hsv, CV_BGR2HSV);

	std::vector<cv::Mat> channels;
	cv::split(hsv, channels);
	cv::Mat hChannel = channels[1];

	cv::Mat mask;
	//cv::threshold(hChannel, mask, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	//cv::adaptiveThreshold(hChannel, mask, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 12);
	cv::Canny(hChannel, mask, 25, 100, 3);

	std::vector<cv::Vec4i> lines;
	HoughLinesP(mask, lines, 1, CV_PI / 180, 50, 200, 20);
	std::cout << lines.size() << std::endl;
	for (size_t i = 0; i < lines.size(); i++) {
		cv::Vec4i l = lines[i];
		line(input, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 0, 255), 3, CV_AA);
	}

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(mask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	std::cout << contours.size() << std::endl;

	int biggestContourIdx = -1;
	float biggestContourArea = 0;
	for (int i = 0; i < contours.size(); i++) {
		cv::Scalar color = cv::Scalar(128, 0, 0);
		drawContours(input, contours, i, color, 10, 8, hierarchy, 0, cv::Point());

		float ctArea = cv::contourArea(contours[i]);
		if (ctArea > biggestContourArea)
		{
			biggestContourArea = ctArea;
			biggestContourIdx = i;
		}
	}
	drawContours(input, contours, biggestContourIdx, cv::Scalar(0, 0, 128), 10, 8, hierarchy, 0, cv::Point());

	cv::imshow("input", input);
	cv::imshow("drawing", mask);
	//cv::imshow("hChannel", hChannel);
	//cv::imshow("Channel 1", channels[1]);
	//cv::imshow("Channel 2", channels[2]);
	cv::waitKey(0);

	cv::imwrite("rotatedRect.png", input);

	return 0;
}