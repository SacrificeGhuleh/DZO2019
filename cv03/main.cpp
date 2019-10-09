#include "pch.h"
#include "convolutionmask.h"
#include "convolution.h"

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */

int main(void) {
  /**
   * Load image.
   */
  cv::Mat src_8uc1_src_img = cv::imread("./images/lena.png", cv::IMREAD_GRAYSCALE);
  
  /**
   * Check, if the image is empty.
   */
  if (src_8uc1_src_img.empty()) {
    std::cout << "WTF" << std::endl;
    return -1;
  }
  
  /**
   * If the image is not empty, show it.
   */
  cv::imshow("Lena", src_8uc1_src_img);
  
  ConvolutionMask<3> sobelXMask({-1, -2, -1, 0, 0, 0, 1, 2, 1}, "X sobel");
  
  cv::imshow("BoxBlur3x3", applyConvolution<BoxBlur3x3ConvolutionMask>(src_8uc1_src_img));
  
  cv::imshow("GausianBlur3x3", applyConvolution<GausianBlur3x3ConvolutionMask>(src_8uc1_src_img));
  
  cv::imshow("GausianBlur5x5", applyConvolution<GausianBlur5x5ConvolutionMask>(src_8uc1_src_img));
  
  cv::imshow("Sobel x", applyConvolution<ConvolutionMask<3>>(sobelXMask, src_8uc1_src_img));
  
  cv::waitKey();
}