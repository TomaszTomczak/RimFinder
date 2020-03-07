#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;

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

Mat calculateHistogram(Mat1b const &image)
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
void drawHistogram(const Mat &hist, int bins)
{
  int const hist_height = 256;
  cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);
  double max_val = 0;
  minMaxLoc(hist, 0, &max_val);

  for (int b = 0; b < bins; b++)
  {
    float const binVal = hist.at<float>(b);
    int const height = cvRound(binVal * hist_height / max_val);
    cv::line(hist_image, cv::Point(b, hist_height - height), cv::Point(b, hist_height), cv::Scalar::all(255));
  }
  cv::imshow("histogram", hist_image);
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
    float ratio = src.rows / src.cols;
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

  int x = rimEdge[0] - rimEdge[2];
  int y = rimEdge[1] - rimEdge[2];
  int widthandheight = rimEdge[2] * 2;
  Rect imageRoi(x, y, widthandheight, widthandheight); // region of interests

  src_gray = src_gray(imageRoi);
  src = src(imageRoi);

  /// Draw the circles detected
  for (size_t i = 0; i < circles.size(); i++)
  {
    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    // circle center
    circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
    // circle outline
    circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
  }

  /// Show your results
  //namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
  imshow("Hough Circle Transform Demo", src);
  waitKey();
  imshow("Hough Circle gray", src_gray);
  waitKey();
  drawHistogram(calculateHistogram(src_gray), 256);
  waitKey(0);
  return 0;
}
