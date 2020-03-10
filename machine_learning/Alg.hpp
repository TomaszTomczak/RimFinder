#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class Alg
{
    static Vec3f findBiggestCircle(const Mat &mat)
    {
        
        for (float i = 2.0; i <= 10.0; i += 0.1)
        {
            std::vector<Vec3f> circles;
            HoughCircles(mat, circles, CV_HOUGH_GRADIENT, 1, /*src_gray.rows/8*/ 50, 100, 100, mat.rows / i, 0);
            if (circles.size() > 0)
            {
                return circles.front();
            }
        }
        return (0.0, 0.0, 0.0);
    }
};