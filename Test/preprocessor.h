#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

class Preprocessor
{
public:
	void BlurImage(Mat& src);
	void ConvertToHsv(Mat& src);
	void MakeUnimodal(Mat& src);
};
