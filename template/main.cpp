#include "pch.h"

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */

int main(void) {
  /**
   * Load image.
   */
  cv::Mat src_8uc3_src_img = cv::imread("./images/lena.png", cv::IMREAD_COLOR);
  
  /**
   * Check, if the image is empty.
   */
  if (src_8uc3_src_img.empty()) {
    std::cout << "WTF" << std::endl;
    return -1;
  }
  
  /**
   * If the image is not empty, show it.
   */
  cv::imshow("Lena", src_8uc3_src_img);
  cv::waitKey();
}