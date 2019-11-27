#include "pch.h"

template<class T>
inline T sqr(const T &val) {
  return val * val;
}

struct RLDUserData {
  cv::Mat &src_8uc3_img;
  cv::Mat &undistorted_8uc3_img;
  int K1;
  int K2;
  
  RLDUserData(const int K1, const int K2, cv::Mat &src_8uc3_img, cv::Mat &undistorted_8uc3_img) :
      K1(K1), K2(K2),
      src_8uc3_img(
          src_8uc3_img),
      undistorted_8uc3_img(
          undistorted_8uc3_img) {
    
  }
};

cv::Vec3b bilinear(const cv::Mat &srcMat, const double row, const double col) {
  cv::Vec3b interpolatedPix;
  
  const int upperRow = static_cast<int>(ceil(row));
  const int lowerRow = static_cast<int>(floor(row));
  const int upperCol = static_cast<int>(ceil(col));
  const int lowerCol = static_cast<int>(floor(col));
  
  const auto &pixUU = srcMat.at<cv::Vec3b>(upperRow, upperCol);
  const auto &pixUL = srcMat.at<cv::Vec3b>(upperRow, lowerCol);
  const auto &pixLU = srcMat.at<cv::Vec3b>(lowerRow, upperCol);
  const auto &pixLL = srcMat.at<cv::Vec3b>(lowerRow, lowerCol);
  
  double x = row - lowerRow;
  double y = col - lowerCol;
  
  // Interpolate
  interpolatedPix = (pixLL * (1 - x) * (1 - y)) +
                    (pixUL * x * (1 - y)) +
                    (pixLU * (1 - x) * y) +
                    (pixUU * x * y);
  
  return interpolatedPix;
}

void geom_dist(const cv::Mat &src_8uc3_img, cv::Mat &dst_8uc3_img, bool bili, double K1 = 1.0, double K2 = 1.0) {
  const int centerCol = src_8uc3_img.cols / 2;
  const int centerRow = src_8uc3_img.rows / 2;
  const double R = sqrt(sqr(centerCol) + sqr(centerRow));
  
  for (int row = 0; row < src_8uc3_img.rows; row++) {
    for (int col = 0; col < src_8uc3_img.cols; col++) {
      const double RNew = sqr((col - centerCol) / R) + sqr((row - centerRow) / R);
      
      const double taylor = 1. + K1 * RNew + K2 * sqr(RNew);
      
      const double colNew = (col - centerCol) * (1. / taylor) + centerCol;
      const double rowNew = (row - centerRow) * (1. / taylor) + centerRow;
      
      //dst_8uc3_img.at<cv::Vec3b>(row, col) = src_8uc3_img.at<cv::Vec3b>(rowNew, colNew);
      dst_8uc3_img.at<cv::Vec3b>(row, col) = bilinear(src_8uc3_img, rowNew, colNew);
      
    }
  }
}


void apply_rld(int id, void *user_data) {
  RLDUserData *rld_user_data = (RLDUserData *) user_data;
  
  geom_dist(rld_user_data->src_8uc3_img, rld_user_data->undistorted_8uc3_img, !false, rld_user_data->K1 / 100.0,
            rld_user_data->K2 / 100.0);
  cv::imshow("Geom Dist", rld_user_data->undistorted_8uc3_img);
}

int main() {
  cv::Mat src_8uc3_img, geom_8uc3_img;
  RLDUserData rld_user_data(3.0, 1.0, src_8uc3_img, geom_8uc3_img);
  
  src_8uc3_img = cv::imread("images/distorted_window.jpg", cv::IMREAD_COLOR);
  if (src_8uc3_img.empty()) {
    printf("Unable to load image!\n");
    exit(-1);
  }
  
  cv::namedWindow("Original Image");
  cv::imshow("Original Image", src_8uc3_img);
  
  geom_8uc3_img = src_8uc3_img.clone();
  apply_rld(0, (void *) &rld_user_data);
  
  cv::namedWindow("Geom Dist");
  cv::imshow("Geom Dist", geom_8uc3_img);
  
  cv::createTrackbar("K1", "Geom Dist", &rld_user_data.K1, 100, apply_rld, &rld_user_data);
  cv::createTrackbar("K2", "Geom Dist", &rld_user_data.K2, 100, apply_rld, &rld_user_data);
  
  cv::waitKey(0);
  
  return 0;
}

