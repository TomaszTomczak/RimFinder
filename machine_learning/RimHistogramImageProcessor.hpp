#include <list>
#include "ImageProcessorIf.hpp"
#include <memory>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class RimHistogramImageProcessor : public ImageProcessorIf
{
public:
struct ProcessedImage
{
    ProcessedImage(std::string path) : path(path){}

    std::string path;
    bool processed = false;
    int* data;
};

  RimHistogramImageProcessor() = default;
  void addImage(std::string path) override;  
  void process() override; 

  std::string getProcessedData() override;


private:
  int *calculateHistogramInCircleArea(const cv::Mat1b &image, const cv::Vec3f &circle);
  
private:
  std::list<std::unique_ptr<ProcessedImage>> images;
};
