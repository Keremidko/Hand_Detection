#include "stdafx.h"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "classifier.h"
#include "preprocessor.h"

#include <iostream>

using namespace cv;
using namespace std;

char key = '_';
Mat src, roi, seg;
Classifier seg_cls;
Preprocessor preprocessor;
VideoCapture cam;
const Rect roi_rect(0, 0, 400, 300);

void initCamera();

int main()
{
	initCamera();

	do
	{
		//get roi
		cam >> src;
		roi = src(roi_rect).clone();
		
		//preprocessing
		preprocessor.BlurImage(roi);
		preprocessor.ConvertToHsv(roi);
		preprocessor.MakeUnimodal(roi);

		seg_cls.SegmentImage(roi, seg);
		// TO DO
		// Create morphological structuring element - rectangle with size 5x5
		// Execute morphological opening on the seg matrix and store the result into morph


		// Display degug info
		rectangle(src, roi_rect, Scalar(0, 0, 255), 2);
		imshow("Image", src);
		imshow("Segmented", seg);

		key = waitKey(33);
	} while (key != 'q');

	return 0;
}

void onMouse(int event, int x, int y, int flags, void* data)
{
	switch (event)
	{
	case CV_EVENT_LBUTTONUP: { 
		int index = (x + y * roi.cols) * 3;
		Point3i pixel(roi.data[index], roi.data[index + 1], roi.data[index + 2]);
		seg_cls.AddPoint(pixel);
	}
	default:
		break;
	}
}

void initCamera() {
	namedWindow("Image");
	setMouseCallback("Image", onMouse);

	cam = VideoCapture(0);

	if (!cam.isOpened())
	{
		cout << "Cannot open camera." << endl;
	}
}