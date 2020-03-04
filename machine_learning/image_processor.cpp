#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>

using namespace cv;


Vec3f findBiggestCircle(const Mat& mat)
{
    for(float i=2.0; i<=10.0; i+=0.1)
    {
    std::vector<Vec3f> circles;
    HoughCircles( mat, circles, CV_HOUGH_GRADIENT, 1, /*src_gray.rows/8*/50, 100, 100, mat.rows/i, 0 );
    if(circles.size()>0)
    {
        return circles.front();
    }
    }
    return (0.0,0.0,0.0);
}

/** @function main */
int main(int argc, char** argv)
{
  Mat src, src_gray;

  /// Read the image
  src = imread( argv[1], 1 );

  if( !src.data )
    { return -1; }

  /// Convert it to gray
  cvtColor( src, src_gray, CV_BGR2GRAY );

  double alpha = 1.0 ; /**< Simple contrast control */
int beta = 0;

 for( int y = 0; y < src_gray.rows; y++ )
    { for( int x = 0; x < src_gray.cols; x++ )
         { for( int c = 0; c < 3; c++ )
              {
      src_gray.at<Vec3b>(y,x)[c] =
         saturate_cast<uchar>( alpha*( src_gray.at<Vec3b>(y,x)[c] ) + beta );
             }
    }
    }

  /// Reduce the noise so we avoid false circle detection
  GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );

  std::vector<Vec3f> circles;
  std::vector<Vec3f> mainCircles;

  /// Apply the Hough Transform to find the circles
 // HoughCircles( src_gray, circles, CV_HOUGH_GRADIENT, 1, /*src_gray.rows/8*/50, 100, 100, src_gray.rows/2.5, 0 );

    circles.push_back(findBiggestCircle(src_gray));

  /// Draw the circles detected
  for( size_t i = 0; i < circles.size(); i++ )
  {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // circle center
      circle( src, center, 3, Scalar(0,255,0), -1, 8, 0 );
      // circle outline
      circle( src, center, radius, Scalar(0,0,255), 3, 8, 0 );
   }

  /// Show your results
  namedWindow( "Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE );
  imshow( "Hough Circle Transform Demo", src );

  waitKey(0);
  return 0;
}