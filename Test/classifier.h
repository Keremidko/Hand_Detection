#ifndef CLASSIFIER_HPP
#define CLASSIFIER_HPP

#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

enum PixelClass{ SKIN, BACKGROUND };

class Classifier
{
private:
	std::vector<cv::Point3i> points;

	// Gaussian distribution params for Hue
	double mean_h;
	double var_h;
	double thresh_h;

	// Gaussian distribution params for Saturation
	double mean_s;
	double var_s;
	double thresh_s;

	// Gaussian distribution params for Value
	double mean_v;
	double var_v;
	double thresh_v;

	void getMeanVar(std::vector<double> data, double& mean, double& var);
	void LearnGaussian();

public:
	Classifier();
	void AddPoint(cv::Point3i& pnt);

	void SegmentImage(Mat& img);
	void SegmentImage(const Mat& src, Mat& out);
	PixelClass Classify(cv::Point3i& pnt);
};

#endif
