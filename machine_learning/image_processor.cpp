#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <list>
#include <math.h>
#include <memory>
#include "Alg.hpp"

using namespace cv; // TODO: remove this in spare time

class ImageProcessor
{
public:
  virtual void addImage(std::string path) = 0;
  virtual void process() = 0;
  virtual std::vector<int> getProcessedData() = 0;
  ~ImageProcessor() = default;
};



class RimHistogramImageProcessor : public ImageProcessor
{
private:
struct ProcessedImage
{
    ProcessedImage(std::string path) : path(path){}

    std::string path;
    bool processed = false;
    int* data;
};
public:
  RimHistogramImageProcessor() = default;
  void addImage(std::string path) override
  {
    ///src = imread(path, 1);
    images.push_back(std::make_unique<ProcessedImage>(path));
  }

  void process() override // this method will be adapted to work in seperate thread
  {
    for(auto& image : images)
    {
    Mat src = imread(image->path);
    Mat src_grayscale;
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
    image->data = calculateHistogramInCircleArea(src_grayscale, rimEdge);
    image->processed = true;
    imshow("normalized greyscale" + image->path, src_grayscale);
    imshow("original" + image->path, src);
    Alg::drawHistogram(image->data);
    }
  }
  std::vector<int> getProcessedData() override{};

private:
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
  std::list<std::unique_ptr<ProcessedImage>> images;
};

int main(int argc, char **argv)
{

  ImageProcessor *imageProcessor = new RimHistogramImageProcessor();
  imageProcessor->addImage(argv[1]);
  imageProcessor->addImage("img_data/mercedes.jpg");
  imageProcessor->process();
  waitKey(0);

  return 0;
}
