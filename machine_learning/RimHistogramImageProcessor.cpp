#include "RimHistogramImageProcessor.hpp"
#include "Alg.hpp"
#include <type_traits>


void RimHistogramImageProcessor::addImage(std::string path)
{
    ///src = imread(path, 1);
    images.push_back(std::make_unique<RimHistogramImageProcessor::ProcessedImage>(path));
}
void RimHistogramImageProcessor::process()
  {
    for(auto& image : images)
    {
    cv::Mat src = cv::imread(image->path);
    cv::Mat src_grayscale;
    Alg::resizeImg(src, 500);
    cv::Vec3f rimEdge = Alg::findBiggestCircle(src);

    int x = rimEdge[0] - rimEdge[2];
    int y = rimEdge[1] - rimEdge[2];
    int widthandheight = rimEdge[2] * 2;
    cv::Rect imageRoi(x, y, widthandheight, widthandheight); // region of interests

    rimEdge = cv::Vec3f(widthandheight / 2, widthandheight / 2, widthandheight / 2); //move detected circle to new location after image crop
    src = src(imageRoi);
    cv::cvtColor(src, src_grayscale, CV_BGR2GRAY);
    cv::normalize(src_grayscale, src_grayscale, 0, 255, NORM_MINMAX);
    image->data = calculateHistogramInCircleArea(src_grayscale, rimEdge);
    image->processed = true;
    cv::imshow("normalized greyscale" + image->path, src_grayscale);
    cv::imshow("original" + image->path, src);
    Alg::drawHistogram(image->data);
    }
  }

  std::string RimHistogramImageProcessor::getProcessedData() 
  {
    std::ostringstream oss;
     // it will be json in the future
    for(auto& i : images)
    {
        oss<<"Path: ";
        oss<<i->path;
        oss<<"\n";  
        oss<<"Data: [";
        for(int x =0; x<256; x++)
        {
            oss<<i->data[x]<<" ";
        }       
        oss<<"]\n";
    }
    return oss.str();
  };

  int* RimHistogramImageProcessor::calculateHistogramInCircleArea(const cv::Mat1b &image, const cv::Vec3f &circle)
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