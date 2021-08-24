#include "stdafx.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "classifier.h"
#include "imageProcessor.h"

#include <iostream>

using namespace cv;
using namespace std;

char key = '_';
Mat src, roi, seg;
Classifier seg_cls;
ImageProcessor imgProcessor;
VideoCapture cam;
vector<Point> fingerPoints, insidePoints;
bool train = false;
const Rect roi_rect(0, 0, 400, 300);

bool event_set = false;

string classify(vector<Point>& inputPoints);

void initCamera();

int main()
{
	initCamera();

	do
	{
		fingerPoints.clear();
		insidePoints.clear();

		//get roi
		cam >> src;
		roi = src(roi_rect).clone();
		
		//preprocessing
		imgProcessor.BlurImage(roi);
		imgProcessor.ConvertToHsv(roi);
		imgProcessor.MakeUnimodal(roi);
		

		seg_cls.SegmentImage(roi, seg);
		imgProcessor.clearNoise(seg);
		imgProcessor.CreateConvexHull(seg, fingerPoints, insidePoints);
		imgProcessor.drawCircles(src, fingerPoints, Scalar(255, 100, 0));
		imgProcessor.drawCircles(src, insidePoints, Scalar(100, 255, 0));

		std::ostringstream str;
		
		str << "Fingers count:" << fingerPoints.size() + 1;
		cv::putText(src, str.str(), Point(10, 30), cv::FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0), 2);

		std::ostringstream clsString;
		clsString << "Class: " + classify(fingerPoints);
		cv::putText(src, clsString.str(), Point(10, 60), cv::FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0), 2);

		// Display degug info
		rectangle(src, roi_rect, Scalar(0, 0, 255), 2);
		imshow("Image", src);
		imshow("Segmented", seg);

		key = waitKey(33);
	} while (key != 'q');

	return 0;
}

string classify(vector<Point>& inputPoints) {
	int size = inputPoints.size();
	switch (size) {
	case 5:
		return "open";
	case 4:
		return "open";
	case 3:
		return "scissors";
	case 2:
		return "rock";
	case 1 :
		return "closed";
	default :
		return "unknown";
	}
}

static void onMouse(int event, int x, int y, int flags, void* data)
{

	switch (event)
	{
	case cv::EVENT_LBUTTONUP: {
		int index = (x + y * roi.cols) * 3;
		Point3i pixel(roi.data[index], roi.data[index + 1], roi.data[index + 2]);
		seg_cls.AddPoint(pixel);
	}
	default:
		break;
	}
}

void initCamera() {
	cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
	cv::setMouseCallback("Image", onMouse, 0);

	cam = VideoCapture(0);

	if (!cam.isOpened())
	{
		cout << "Cannot open camera." << endl;
	}
}