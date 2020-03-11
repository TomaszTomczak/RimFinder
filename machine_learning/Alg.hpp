#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv; //TODO: remove using namespace...

class Alg
{
public:
  static void resizeImg(Mat &src, int maxsize)
  {
    if (src.rows > maxsize) // resize iamge
    {
      float ratio = (float)src.rows / src.cols;
      std::cout << "ratio: " << ratio << std::endl;
      resize(src, src, Size(maxsize, maxsize * ratio));
    }
  }

  static Vec3f findBiggestCircle(const Mat &mat)
  {

    Mat src_gray;
    cvtColor(mat, src_gray, CV_BGR2GRAY);

    /// Reduce the noise so we avoid false circle detection
    GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);

    for (float i = 2.0; i <= 10.0; i += 0.1)
    {
      std::vector<Vec3f> circles;
      HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 1, /*src_gray.rows/8*/ 50, 100, 100, src_gray.rows / i, 0);
      if (circles.size() > 0)
      {
        return circles.front();
      }
    }
    return (0.0, 0.0, 0.0);
  }
  static Mat calculateHistogramInCircleArea(const Mat1b &image, const Vec3f &circle)
  {
    int histogram[256];
    Mat m(1, 256, CV_32F, Scalar(0));

    std::cout << "m: " << m << std::endl;

    for (int y = 0; y < image.rows; y++)
    {
      for (int x = 0; x < image.cols; x++)
      {
        float distance = sqrt(pow(x - circle[0], 2) + pow(y - circle[1], 2));
        if (distance <= circle[2])
        {
          //histogram[image.at<uchar>(y, x)]++;
          //(image.at<uchar>(x,y)) ++;
          m.at<int>(image.at<uchar>(x, y))++;
        }
      }
    }
    std::cout << "m: " << m << std::endl;

    return m;
  }
  static void drawHistogram(int *histogramData, int bins = 256, int hist_height = 256)
  {
    cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);
    double max_val = 0;
    //minMaxLoc(*data, 0, &max_val);

    for (int i = 0; i < 256; i++)
    {
      if (histogramData[i] >= max_val)
      {

        max_val = histogramData[i];
      }
    }

    std::cout << std::endl
              << "max val: " << max_val << std::endl;

    for (int b = 0; b < bins; b++)
    {
      float const binVal = histogramData[b];
      int const height = cvRound(binVal * hist_height / max_val);
      cv::line(hist_image, cv::Point(b, hist_height - height), cv::Point(b, hist_height), cv::Scalar::all(255));
    }
    cv::imshow("histogram", hist_image);
  }
  static void drawHistogram(const Mat &hist, int bins, std::string title)
  {
    int const hist_height = 256;
    std::cout << "histogram data: " << hist << std::endl;
    cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);
    double max_val = 0;
    minMaxLoc(hist, 0, &max_val);

    std::cout << std::endl
              << "max val: " << max_val << std::endl;

    for (int b = 0; b < bins; b++)
    {
      float const binVal = hist.at<float>(b);
      int const height = cvRound(binVal * hist_height / max_val);
      cv::line(hist_image, cv::Point(b, hist_height - height), cv::Point(b, hist_height), cv::Scalar::all(255));
    }
    cv::imshow(title, hist_image);
  }
};