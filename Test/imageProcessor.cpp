#include "stdafx.h"
#include "imageProcessor.h"
#include "opencv2/highgui/highgui.hpp"
#include "highgui.h"

using namespace cv;
using namespace std;

void ImageProcessor::BlurImage(Mat& dest) {
	Mat input = dest.clone();
	GaussianBlur(input, dest, Size(5, 5), 0);
}

void ImageProcessor::ConvertToHsv(Mat& dest) {
	Mat input = dest.clone();
	cvtColor(input, dest, CV_BGR2HSV);
}

void ImageProcessor::MakeUnimodal(Mat& src) {
	for (int i = 0; i < src.rows; ++i)
	{
		for (int j = 0; j < src.cols; ++j)
		{
			int idx = (i * src.cols + j) * 3;
			src.data[idx] = ((int)src.data[idx] + 90) % 180;
		}
	}
}

void ImageProcessor::clearNoise(Mat& src) {
	Mat elem = getStructuringElement(MORPH_ELLIPSE, Size(5,5), Point(0,0));
	imshow("With noise", src);

	Mat inp = src.clone();
	morphologyEx(inp, src, MORPH_OPEN, elem);
	inp = src.clone();
	morphologyEx(inp, src, MORPH_CLOSE, elem);
	Mat border = src.clone();
	morphologyEx(src, border, MORPH_GRADIENT, elem);
	imshow("border", border);
}

void ImageProcessor::CreateConvexHull(Mat& src) {
	vector<vector<Point>> contours;
	Mat input = src.clone();
	findContours(input, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	Scalar colorContours = Scalar(255, 0, 0);
	Scalar colorHull = Scalar(0, 255, 0);

	vector<vector<Point>> hull(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		convexHull(Mat(contours[i]), hull[i], false);
	}

	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++) {
		
		drawContours(drawing, contours, i, colorContours, 1);
		drawContours(drawing, hull, i, colorHull, 1);
	}
	imshow("contours+hull", drawing);
}