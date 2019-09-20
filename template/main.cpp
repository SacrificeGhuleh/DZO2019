#include "pch.h"

int main(void) {
  cv::Mat src_8uc3_src_img = cv::imread("../images/lena.png", cv::IMREAD_COLOR);
  
  if (src_8uc3_src_img.empty()) {
    std::cout << "WTF" << std::endl;
    return -1;
  }
  
  cv::imshow("Lena", src_8uc3_src_img);
  cv::waitKey();
}