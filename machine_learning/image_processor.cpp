#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <Modifier.hpp>
#include <Classifier.hpp>
#include <memory>

using namespace cv;

class Alg
{
  public:
  static void resizeImg(Mat& src, int maxsize)
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

class ImageProcessor
{
public:
  virtual void load(std::string path) = 0;
  virtual void process() = 0;
  virtual std::vector<int> getProcessedData() = 0;
  ~ImageProcessor() = default;
};

class RimHistogramImageProcessor : public ImageProcessor
{

public:
  RimHistogramImageProcessor()
  {
  }
  void load(std::string path) override
  {
    src = imread(path, 1);
  };

  void process() override
  {
    Alg::re
  };
  std::vector<int> getProcessedData() override{};

private:
  Mat src;
  Mat src_greyscale;
  std::vector<std::unique_ptr<Modifier>> modifiers;
  std::unique_ptr<Classifier> classifier;
};

//TODO: implement Gabor filters, histogram and set every image to the same size
Vec3f findBiggestCircle(const Mat &mat)
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

Mat calculateHistogramInCircleArea(const Mat1b &image, const Vec3f &circle)
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

Mat calculateHistogram(const Mat1b &image)
{

  int bins = 256;
  int histogramData[256];
  int histSize[] = {bins};
  // Set ranges for histogram bins
  float lranges[] = {0, 256};
  const float *ranges[] = {lranges};
  // create matrix for histogram
  cv::Mat hist;
  int channels[] = {0};

  calcHist(&image, 1, channels, cv::Mat(), hist, 1, histSize, ranges, true, false);

  return hist;
}

//TODO: it should be calculated based on data in the area of rim cirgle
void drawHistogram(const Mat &hist, int bins, std::string title)
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

/** @function main */
int main(int argc, char **argv)
{
  Mat src, src_gray;
  //Mat src2 = Mat()

  /// Read the image
  src = imread(argv[1], 1);

  if (!src.data)
  {
    return -1;
  }

  if (src.rows > 500) // resize iamge
  {
    float ratio = (float)src.rows / src.cols;
    std::cout << "ratio: " << ratio << std::endl;
    resize(src, src, Size(500, 500 * ratio));
  }
  //resize(src,src,Size(500, 500*ratio));
  /// Convert it to gray
  cvtColor(src, src_gray, CV_BGR2GRAY);

  double alpha = 1.0; /**< Simple contrast control */
  int beta = 0;

  for (int y = 0; y < src_gray.rows; y++)
  {
    for (int x = 0; x < src_gray.cols; x++)
    {
      for (int c = 0; c < 3; c++)
      {
        src_gray.at<Vec3b>(y, x)[c] =
            saturate_cast<uchar>(alpha * (src_gray.at<Vec3b>(y, x)[c]) + beta);
      }
    }
  }

  /// Reduce the noise so we avoid false circle detection
  GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);

  std::vector<Vec3f> circles;
  std::vector<Vec3f> mainCircles;

  /// Apply the Hough Transform to find the circles
  // HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, /*src_gray.rows/8*/50, 200, 35, 0, 0 );

  // circles.push_back(findBiggestCircle(src_gray));

  Vec3f rimEdge = findBiggestCircle(src_gray);

  //calculateHistogramInCircleArea(src_gray, rimEdge);

  int x = rimEdge[0] - rimEdge[2];
  int y = rimEdge[1] - rimEdge[2];
  int widthandheight = rimEdge[2] * 2;
  Rect imageRoi(x, y, widthandheight, widthandheight); // region of interests

  rimEdge = Vec3f(widthandheight / 2, widthandheight / 2, widthandheight / 2); //move detected circle to new location after image crop

  src_gray = src_gray(imageRoi);
  src = src(imageRoi);

  /// Draw the circles detected
  /* for (size_t i = 0; i < circles.size(); i++)
  {
    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    // circle center
    circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
    // circle outline
    circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
  }*/

  /// Show your results
  //namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
  /*for (int y = 0; y < src.rows; y++)
  {
    for (int x = 0; x < src.cols; x++)
    {
      float distance = sqrt(pow(x-rimEdge[0],2)+pow(y-rimEdge[1],2));
      if(distance <= rimEdge[2])
      {
        //histogram[image.at<uchar>(y, x)]++;
        //(image.at<uchar>(x,y)) ++;
       // m.at<int>(image.at<uchar>(x,y)) ++;
       src.at<Vec3b>(x,y)[0] = 255;
      }
    }
  }*/

  imshow("Hough Circle Transform Demo", src);
  waitKey();
  imshow("Hough Circle gray", src_gray);
  //waitKey();
  //drawHistogram(calculateHistogram(src_gray), 256, "1");

  waitKey();
  Mat tmp;
  cvtColor(src, tmp, CV_BGR2GRAY);
  imshow("greycale without normalization", tmp);
  //std::cout<<"without normalization: "<<tmp<<std::endl;
  waitKey();
  drawHistogram(calculateHistogramInCircleArea(tmp, rimEdge), 256, "histogram od circle without normalization");
  Mat tmp2;
  normalize(tmp, tmp2, 0, 255, NORM_MINMAX);
  imshow("normalized image", tmp2);
  //std::cout<<"with normalization: "<<tmp2<<std::endl;
  drawHistogram(calculateHistogramInCircleArea(tmp2, rimEdge), 256, "3");
  waitKey(0);
  return 0;
}
