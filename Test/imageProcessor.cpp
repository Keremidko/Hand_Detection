#include "stdafx.h"
#include "imageProcessor.h"
#include "opencv2/highgui/highgui.hpp"
#include "highgui.h"
#include <math.h>
#include <iostream>

using namespace cv;
using namespace std;

int findBiggestContour(vector<vector<Point>> contours);
double findCos(Point start, Point end, Point far);
double euclidianDistance(Point& p1, Point& p2);
void mergeFingerPoints(vector<Point>& points, vector<Point>& mergedPoints);

const int MERGE_THRESHOLD = 18;

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
	//imshow("border", border);
}

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * This SHIT needs Refactoring!	
 */
void ImageProcessor::CreateConvexHull(Mat& src, vector<Point>& fingerPoints, vector<Point>& insidePoints) {
	vector<vector<Point>> contours;
	Mat input = src.clone();
	findContours(input, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	Scalar colorContours = Scalar(255, 0, 0);
	Scalar colorHull = Scalar(0, 255, 0);
	Scalar colorDefects = Scalar(0, 0, 255);

	vector<vector<Point>> hull(contours.size());
	vector<vector<int>> hull2(contours.size());
	vector<vector<Vec4i>> defects(contours.size());
	for (int i = 0; i < contours.size(); i++) {
		convexHull(Mat(contours[i]), hull[i], false);
		convexHull(Mat(contours[i]), hull2[i], false);
		convexityDefects(Mat(contours[i]), hull2[i], defects[i]);
	}

	Mat drawing = Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++) {	
		drawContours(drawing, contours, i, colorContours, 1);
		drawContours(drawing, hull, i, colorHull, 1);
	}

	//vector<Point> fingerPoints;
	vector<Point> insidePoints;
	vector<Point> mergedPoints;
	if (contours.size() > 0) { 
		int biggestContour = findBiggestContour(contours);

		vector<Vec4i> biggestDefect = defects[biggestContour];
		for (int j = 0; j < biggestDefect.size(); j++) {
			int startIdx = biggestDefect[j][0];
			int endIdx = biggestDefect[j][1];
			int farIdx = biggestDefect[j][2];
			float depth = biggestDefect[j][3]/256;
			Point startPoint = contours[biggestContour][startIdx];
			Point endPoint = contours[biggestContour][endIdx];
			Point farPoint = contours[biggestContour][farIdx];

			double cos = findCos(startPoint, endPoint, farPoint);

			if (cos > -0.1 && depth > 30) {
				//fingerPoints.push_back(startPoint);
				fingerPoints.push_back(endPoint);
				insidePoints.push_back(farPoint);
			}
		}

		drawCircles(drawing, fingerPoints, Scalar(100,0, 255));
		drawCircles(drawing, insidePoints, Scalar(100, 255, 100));
		std::ostringstream str;
		str << "Fingers count:" << fingerPoints.size();
		putText(drawing, str.str(), Point(10,20), CV_FONT_HERSHEY_PLAIN, 1, Scalar(0,0,255));
	}

	imshow("contours+hull", drawing);
}

void ImageProcessor::drawCircles(Mat& drawing, vector<Point>& points, Scalar& color) {
	for (int i = 0; i < points.size(); i++ ) {
		circle(drawing, points[i], 4, color, 7);
	}
}

void mergeFingerPoints(vector<Point>& inputPoints, vector<Point>& mergedPoints) {
	vector<Point> points = inputPoints;

	if (points.size() == 0)
		return;

	for (int i = 0; i < points.size() - 1; i++) {
		Point p1 = points[i];
		Point cache = p1;
		int counter = 1;

		/* Find centroid point for all points near p1, considering MERGE_THRESHOLD distance */
		for (int j = i + 1; j < points.size(); j++) {
			Point p2 = points[j];
			if (euclidianDistance(p1, p2) < MERGE_THRESHOLD) {
				points.erase(std::remove(points.begin(), points.end(), p2), points.end());
				cache += p2;
				counter++;
				break;
			}
		}

		cache = cache / counter;
		mergedPoints.push_back(cache);
	}
}

double euclidianDistance(Point& p1, Point& p2) {
	Point diff = p1 - p2;
	return sqrt(diff.x*diff.x + diff.y + diff.y);
}

double findCos(Point start, Point end, Point far){
	start.x -= far.x;
	start.y -= far.y;
	end.x -= far.x;
	end.y -= far.y;

	double lenStart =  sqrt(start.x * start.x + start.y * start.y);
	double lenEnd = sqrt(end.x * end.x + end.y * end.y);
	double product = start.x * end.x + start.y * end.y;
	return product/(lenStart*lenEnd);
}

int findBiggestContour(vector<vector<Point>> contours)
{
	int indexOfBiggestContour = -1;
	int sizeOfBiggestContour = 0;

	for (int i = 0; i < contours.size(); i++){
		if (contours[i].size() > sizeOfBiggestContour){
			sizeOfBiggestContour = contours[i].size();
			indexOfBiggestContour = i;
		}
	}
	return indexOfBiggestContour;
}
