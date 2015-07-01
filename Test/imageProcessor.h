#include <vector>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

class ImageProcessor
{
public:
	void BlurImage(Mat& src);
	void ConvertToHsv(Mat& src);
	void MakeUnimodal(Mat& src);
	void clearNoise(Mat& src);
	void CreateConvexHull(Mat& src, vector<Point>& points, vector<Point>& insidePoints);
	void drawCircles(Mat& drawing, vector<Point>& points, Scalar& color);
	void putText(Mat& drawing, string text);
};
