#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{

   Mat image(Mat::zeros(800, 600, CV_8U));
   int fontFace(FONT_HERSHEY_PLAIN);
   double fontScale(2.0);
   int thickness(3);  
   Point textOrg(50, 50);
   putText(image, "Hello, word!", textOrg, fontFace, fontScale, Scalar::all(255), thickness, 8);
   imshow("Hello, word! Press any key...", image);
   waitKey(0);
   return 0;
}