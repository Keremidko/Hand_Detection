#include "stdafx.h"
#include "preprocessor.h"

using namespace cv;

void Preprocessor::BlurImage(Mat& dest) {
	Mat input = dest.clone();
	GaussianBlur(input, dest, Size(5, 5), 0);
}

void Preprocessor::ConvertToHsv(Mat& dest) {
	Mat input = dest.clone();
	cvtColor(input, dest, CV_BGR2HSV);
}

void Preprocessor::MakeUnimodal(Mat& src) {
	for (int i = 0; i < src.rows; ++i)
	{
		for (int j = 0; j < src.cols; ++j)
		{
			int idx = (i * src.cols + j) * 3;
			src.data[idx] = ((int)src.data[idx] + 90) % 180;
		}
	}
}