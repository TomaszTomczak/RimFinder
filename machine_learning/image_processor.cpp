#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <memory>
#include "Alg.hpp"

using namespace cv; // TODO: remove this in spare time

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
  RimHistogramImageProcessor() = default;
  void load(std::string path) override
  {
    src = imread(path, 1);
  }

  void process() override
  {
    Alg::resizeImg(src, 500);
    Vec3f rimEdge = Alg::findBiggestCircle(src);

    int x = rimEdge[0] - rimEdge[2];
    int y = rimEdge[1] - rimEdge[2];
    int widthandheight = rimEdge[2] * 2;
    Rect imageRoi(x, y, widthandheight, widthandheight); // region of interests

    rimEdge = Vec3f(widthandheight / 2, widthandheight / 2, widthandheight / 2); //move detected circle to new location after image crop
    src = src(imageRoi);
    cvtColor(src, src_grayscale, CV_BGR2GRAY);
    normalize(src_grayscale, src_grayscale, 0, 255, NORM_MINMAX);
    data = calculateHistogramInCircleArea(src_grayscale, rimEdge);
    processed = true;
    imshow("normalized greyscale", src_grayscale);
    imshow("original", src);
    drawHistogram(data);
  }
  std::vector<int> getProcessedData() override{};

private:
  void drawHistogram(int *histogramData, int bins = 256, int hist_height = 256)
  {

    cv::Mat3b hist_image = cv::Mat3b::zeros(hist_height, bins);
    double max_val = 0;
    //minMaxLoc(*data, 0, &max_val);

    for (int i = 0; i < 256; i++)
    {
      if (data[i] >= max_val)
      {

        max_val = data[i];
      }
    }

    std::cout << std::endl
              << "max val: " << max_val << std::endl;

    for (int b = 0; b < bins; b++)
    {
      float const binVal = data[b];
      int const height = cvRound(binVal * hist_height / max_val);
      cv::line(hist_image, cv::Point(b, hist_height - height), cv::Point(b, hist_height), cv::Scalar::all(255));
    }
    cv::imshow("histogram", hist_image);
  }
  int *calculateHistogramInCircleArea(const Mat1b &image, const Vec3f &circle)
  {
    int *histogram = new int[256];

    for (int i = 0; i < 256; i++)
    {
      histogram[i] = 0;
    }
    for (int y = 0; y < image.rows; y++)
    {
      for (int x = 0; x < image.cols; x++)
      {
        float distance = sqrt(pow(x - circle[0], 2) + pow(y - circle[1], 2));
        if (distance <= circle[2])
        {
          histogram[(int)image.at<uchar>(y, x)]++;
        }
      }
    }

    return histogram;
  }

private:
  Mat src;
  Mat src_grayscale;
  bool processed = false;
  int *data;
};

int main(int argc, char **argv)
{

  ImageProcessor *imageProcessor = new RimHistogramImageProcessor();
  imageProcessor->load(argv[1]);
  imageProcessor->process();
  waitKey(0);

  return 0;
}
