#include <iostream>
#include <stdio.h>
#include "RimHistogramImageProcessor.hpp"
#include <opencv2/core/core.hpp>
#include <fstream>

int main(int argc, char **argv)
{

  ImageProcessorIf *imageProcessor = new RimHistogramImageProcessor();
  imageProcessor->addImage(argv[1]);
  imageProcessor->addImage("img_data/mercedes.jpg");
  imageProcessor->process();
  
  cv::waitKey(0);

  //std::cout<<imageProcessor->getProcessedData();
  std::ofstream myfile;
  myfile.open ("images_data.json");
  myfile << imageProcessor->getProcessedData();
  myfile.close();

  return 0;
}
