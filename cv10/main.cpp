#include "pch.h"
#include "../cv03/convolution.h"
#include "../cv03/convolutionmask.h"

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
  cv::Mat dstValve = cv::Mat(srcValve.rows, srcValve.cols, CV_8UC1);
  
  
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
  
  ConvolutionMask<3> sobelYMask({
                                    1, 2, 1,
                                    0, 0, 0,
                                    -1, -2, -1}, "Y sobel");
  
  
  ConvolutionMask<3> sobelXMask({
                                    -1, 0, 1,
                                    -2, 0, 2,
                                    -1, 0, 1}, "X sobel");
  
  const cv::Mat sobelXMat = applyConvolution<ConvolutionMask<3>>(sobelXMask, srcValve);
  const cv::Mat sobelYMat = applyConvolution<ConvolutionMask<3>>(sobelYMask, srcValve);
  
//  cv::imshow("Sobel x", sobelXMat);
//  cv::imshow("Sobel y", sobelYMat);
  
  
  uint8_t loc_min = 255;
  uint8_t loc_max = 0;
  
  for (int row = 0; row < srcValve.rows; row++) {
    for (int col = 0; col < srcValve.cols; col++) {
      const uint8_t sobelXPix = sobelXMat.at<uint8_t>(row, col);
      const uint8_t sobelYPix = sobelYMat.at<uint8_t>(row, col);
      const uint8_t sobelPix = static_cast<uint8_t>(sqrt(sqr(sobelXPix) + sqr(sobelYPix)));
      
      assert(sobelPix <= 255);
      dstValve.at<uint8_t>(row, col) = sobelPix;
      
      loc_min = sobelPix < loc_min ? sobelPix : loc_min;
      loc_max = sobelPix > loc_max ? sobelPix : loc_max;
    }
  }
  
  printf("Found min: %d, found max: %d\n", loc_min, loc_max);
  
  cv::Mat dst_8uc3_src_img = dstValve.clone();
  for (int row = 0; row < dstValve.rows; row++) {
    for (int col = 0; col < dstValve.cols; col++) {
      float loc_pix = static_cast<float>(dstValve.at<uint8_t>(row, col));
      loc_pix -= static_cast<float>(loc_min);
      loc_pix *= 255.f;
      loc_pix /= static_cast<float>(loc_max - loc_min);
      dst_8uc3_src_img.at<uint8_t>(row, col) = static_cast<uint8_t>(loc_pix);
    }
  }
  
  cv::imshow("Sobel complete normalized", dst_8uc3_src_img);
  //cv::imshow("Sobel complete", dstValve);
  
  cv::waitKey();
}