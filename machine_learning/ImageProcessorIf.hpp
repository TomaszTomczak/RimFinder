#include <vector>
#include <string>

class ImageProcessorIf
{
public:
  virtual void addImage(std::string path) = 0;
  virtual void process() = 0;
  virtual std::vector<int> getProcessedData() = 0;
  ~ImageProcessorIf() = default;
};