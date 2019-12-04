#include "pch.h"
#include "../cv03/convolution.h"
#include "../cv03/convolutionmask.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */

template<class T>
inline T sqr(const T &val) {
  return val * val;
}

int main(void) {
  /**
   * Load image.
   */
  const cv::Mat srcValve = cv::imread("./images/valve.png", cv::IMREAD_GRAYSCALE);
  cv::Mat dstValve = cv::Mat(srcValve.rows, srcValve.cols, CV_64FC1);
  
  
  /**
   * Check, if the image is empty.
   */
  if (srcValve.empty()) {
    std::cout << "WTF" << std::endl;
    return -1;
  }
  
  
  /**
   * If the image is not empty, show it.
   */
  cv::imshow("Valve", srcValve);
  
  ConvolutionMask<3> sobelXMask({
                                    -1, +0, +1,
                                    -2, +0, +2,
                                    -1, +0, +1},
                                "X sobel");
  
  ConvolutionMask<3> sobelYMask({
                                    -1, -2, -1,
                                    +0, +0, +0,
                                    +1, +2, +1},
                                "Y sobel");
  
  const cv::Mat sobelXMat = applyConvolution<ConvolutionMask<3>>(sobelXMask, srcValve);
  const cv::Mat sobelYMat = applyConvolution<ConvolutionMask<3>>(sobelYMask, srcValve);
  
  cv::imshow("Sobel x", sobelXMat);
  cv::imshow("Sobel y", sobelYMat);
  
  for (int row = 0; row < srcValve.rows; row++) {
    for (int col = 0; col < srcValve.cols; col++) {
      const double sobelXPix = sobelXMat.at<uint8_t>(row, col);
      const double sobelYPix = sobelYMat.at<uint8_t>(row, col);
      const double sobelPix = sqrt(sobelXPix * sobelXPix + sobelYPix * sobelYPix);
      
      dstValve.at<double>(row, col) = sobelPix;
    }
  }
  cv::normalize(dstValve, dstValve, 0.0f, 255.0f, cv::NORM_MINMAX);

//  cv::imshow("Sobel complete normalized", dst_8uc3_src_img);
  cv::imshow("Sobel complete", dstValve);
  
  cv::Mat sobelMat = srcValve.clone();
  
  cv::waitKey();
}