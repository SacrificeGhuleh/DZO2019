#include "pch.h"

void task();

int main(void) {
  task();
}

void task() {
  
  cv::Mat src_8uc3_src_img = cv::imread("../images/moon.jpg", cv::IMREAD_GRAYSCALE);
  if (src_8uc3_src_img.empty()) {
    std::cout << "wtf dude" << std::endl;
    return;
  }
  cv::imshow("vyblity hnusny img", src_8uc3_src_img);
  
  uint8_t loc_min = 255;
  uint8_t loc_max = 0;
  
  for (int row = 0; row < src_8uc3_src_img.rows; row++) {
    for (int col = 0; col < src_8uc3_src_img.cols; col++) {
      uint8_t loc_pix = src_8uc3_src_img.at<uint8_t>(row, col);
      loc_min = loc_pix < loc_min ? loc_pix : loc_min;
      loc_max = loc_pix > loc_max ? loc_pix : loc_max;
    }
  }
  
  printf("Found min: %d, found max: %d\n", loc_min, loc_max);
  
  cv::Mat dst_8uc3_src_img = src_8uc3_src_img.clone();
  
  for (int row = 0; row < src_8uc3_src_img.rows; row++) {
    for (int col = 0; col < src_8uc3_src_img.cols; col++) {
      float loc_pix = static_cast<float>(src_8uc3_src_img.at<uint8_t>(row, col));
      loc_pix -= static_cast<float>(loc_min);
      loc_pix *= 255.f;
      loc_pix /= static_cast<float>(loc_max - loc_min);
      dst_8uc3_src_img.at<uint8_t>(row, col) = static_cast<uint8_t>(loc_pix);
    }
  }
  
  cv::imshow("Mene hnusny, kontrastni img", dst_8uc3_src_img);
  
  cv::waitKey();
}
