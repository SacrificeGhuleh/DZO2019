#include "pch.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */

static const int HIST_SIZE = 256;

float matMin(const cv::Mat &srcMat) {
  float min = 1;
  
  for (int i = 0; i < srcMat.rows; i++) {
    float newMin = srcMat.at<float>(i);
    
    min = srcMat.at<float>(i) < min ? newMin : min;
  }
  
  return min;
}

cv::Mat calcCdf(const cv::Mat &hist) {
  // Calculate image integral
  cv::Mat cdfInt;
  cv::integral(hist, cdfInt, CV_32F);
  
  // Remove first column and row, since integral adds W + 1 and H + 1
  return cdfInt(cv::Rect(1, 1, 1, hist.rows - 1));
}

cv::Mat calcHistogram(const cv::Mat &src) {
  cv::Mat hist = cv::Mat::zeros(HIST_SIZE, 1, CV_32FC1);
  
  for (int y = 0; y < src.rows; y++) {
    for (int x = 0; x < src.cols; x++) {
      uchar srcPx = src.at<uchar>(y, x);
      hist.at<float>(srcPx)++;
    }
  }
  
  return hist;
}

cv::Mat drawHistogram(const cv::Mat &hist) {
  cv::Mat histNorm;
  cv::normalize(hist, histNorm, 0.0f, 255.0f, cv::NORM_MINMAX);
  cv::Mat dst = cv::Mat::zeros(HIST_SIZE, HIST_SIZE, CV_8UC1);
  
  for (int x = 0; x < dst.cols; x++) {
    int vertical = cv::saturate_cast<int>(histNorm.at<float>(x));
    for (int y = HIST_SIZE - 1; y > (HIST_SIZE - vertical - 1); y--) {
      dst.at<uchar>(y, x) = 255;
    }
  }
  
  return dst;
}

int main() {
  const cv::Mat srcUnequalizedMat = cv::imread("./images/uneq.jpg", cv::IMREAD_GRAYSCALE);
  cv::Mat dstEqualizedMat = cv::Mat::zeros(srcUnequalizedMat.rows, srcUnequalizedMat.cols, CV_32FC1);
  
  // Get rows and cols
  int rows = srcUnequalizedMat.rows;
  int cols = srcUnequalizedMat.cols;
  
  
  // Calc histogram and img integral (cdf)
  const cv::Mat srcHist = calcHistogram(srcUnequalizedMat);
  const cv::Mat srcCdf = calcCdf(srcHist);
  
  // Get cdf min
  const float min = matMin(srcCdf);
  
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      int srcPx = static_cast<int>(srcUnequalizedMat.at<uchar>(y, x));
      float cdf = srcCdf.at<float>(srcPx);
      
      dstEqualizedMat.at<float>(y, x) = roundf((static_cast<float>(cdf - min) / static_cast<float>(rows * cols - min)) * (HIST_SIZE - 1));
    }
  }
  
  // Normalize result
  cv::normalize(dstEqualizedMat, dstEqualizedMat, 0.0f, 1.0f, cv::NORM_MINMAX);
  
  // Show results
  cv::imshow("Source", srcUnequalizedMat);
  cv::imshow("Result", dstEqualizedMat);
  cv::imshow("Source Histogram", drawHistogram(srcHist));
  cv::imshow("Result Histogram", drawHistogram(srcCdf));
  
  cv::waitKey();
  return 0;
}