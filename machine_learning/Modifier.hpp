#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class Modifier
{
    public: 
    virtual void modify(const cv::Mat& mat, cv::Mat& outputMat) = 0;
};