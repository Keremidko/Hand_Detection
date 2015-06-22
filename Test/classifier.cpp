#include "stdafx.h"
#include "classifier.h"
#include <iostream>
#define M_PI 3.14159265358979323846
#define M_E 2.71828182845904523536

using namespace std;
using namespace cv;

Classifier::Classifier()
{
	points.clear();
}

void Classifier::AddPoint(cv::Point3i& pnt)
{
	points.push_back(pnt);
	LearnGaussian();
	//cout << "Added point: " << pnt << endl;
}

void Classifier::LearnGaussian()
{
	double max;
	// Create a vector of doubles to store the
	// hue value of each point in the points array.
	// Hint: It is the 1st point coordinate
	vector<double> hues;
	for (int i = 0; i < points.size(); i++)
		hues.push_back(points[i].x);

	// Calculate the mean and the variance of the hue data and
	// store them to mean_h and var_h correspondigly
	getMeanVar(hues, mean_h, var_h);
	
	// Calculate the maximum value of the learn gaussian function.
	max = 1 / (sqrt(2 * M_PI*var_h));

	// Set thresh_h to 20% of the maximum value.
	thresh_h = max / 5;

	//Saturation
	vector<double> saturs;
	for (int i = 0; i < points.size(); i++)
		saturs.push_back(points[i].y);

	getMeanVar(saturs, mean_s, var_s);

	max = 1 / (sqrt(2 * M_PI*var_s));
	thresh_s = max / 5;

	//Value
	vector<double> values;
	for (int i = 0; i < points.size(); i++)
		values.push_back(points[i].z);

	getMeanVar(values, mean_v, var_v);

	max = 1 / (sqrt(2 * M_PI*var_v));
	thresh_v = max / 5;

	// At the end you should have updated all those variables:
	/*
	cout << "   Mean H: " << mean_h << "   Var H: " << var_h << "   Thresh H: " << thresh_h << endl;
	cout << "   Mean S: " << mean_s << "   Var S: " << var_s << "   Thresh S: " << thresh_s << endl;
	cout << "   Mean V: " << mean_v << "   Var V: " << var_v << "   Thresh V: " << thresh_v << endl;
	*/
}

void Classifier::SegmentImage(const Mat& src, Mat& out) {
	//If the seg is empty allocate memory
	if (out.empty())
	{
		out = Mat(src.rows, src.cols, CV_8UC1);
	}

	// For each point of the src image run the Gaussian classifier
	// and if it is classified as skin set the corresponding pixel
	// in the seg matrix to white, otherwise - black.
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			int index = (j + i * src.cols) * 3;
			Point3i pixel(src.data[index], src.data[index + 1], src.data[index + 2]);
			PixelClass clas = this->Classify(pixel);

			if (clas == BACKGROUND) {
				out.data[j + i * src.cols] = 0;
			}
			else  {
				out.data[j + i * src.cols] = 255;
			}
		}
	}
}

void Classifier::SegmentImage(Mat& src) {
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			int index = (j + i * src.cols) * 3;
			Point3i pixel(src.data[index], src.data[index + 1], src.data[index + 2]);
			PixelClass clas = this->Classify(pixel);

			if (clas == BACKGROUND) {
				src.data[j + i * src.cols] = 0;
			}
			else  {
				src.data[j + i * src.cols] = 255;
			}
		}
	}
}

PixelClass Classifier::Classify(cv::Point3i& pnt)
{
	// Calculate the probability for each coordinate of the point p_h, p_s, p_v
	double ph, ps, pv;
	double denom, power;

	//Hue
	denom = sqrt(2 * M_PI * var_h);
	power = (-1. / 2) * (pow((pnt.x - mean_h), 2) / var_h);
	ph = (1. / denom) * pow(M_E, power);

	//sat
	denom = sqrt(2 * M_PI*var_s);
	power = (-1. / 2) * (pow((pnt.y - mean_s), 2) / var_s);
	ps = (1. / denom)*pow(M_E, power);

	//val
	denom = sqrt(2 * M_PI*var_v);
	power = (-1. / 2)*(pow((pnt.z - mean_v), 2) / var_v);
	pv = (1. / denom)*pow(M_E, power);

	// Calculate the probability of the point as p_h * p_s * p_v
	double prob = ph * ps * pv;

	// Calculate the threshold as a product of the separate thresholds
	double thrsh = thresh_h * thresh_s * thresh_v;

	// Return the class of the pixel. Use the SKIN and BACKGROUND enumaration
	if (prob > thrsh)
		return SKIN;
	return BACKGROUND;
}

void Classifier::getMeanVar(std::vector<double> data, double& mean, double& var)
{
	// Calculate the mean of the values in data
	double sum = 0;
	for (int i = 0; i < data.size(); i++)
		sum += data[i];
	mean = sum / data.size();

	// Calculate the variance of the values in data
	double variance_sum = 0;
	for (int i = 0; i < data.size(); i++)
		variance_sum += (data[i] - mean)*(data[i] - mean);

	var = variance_sum / data.size();
}