#include "pch.h"
#include <omp.h>

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */

//cv::Mat fourier(const cv::Mat &srcMat);

static const cv::Size winSize(300, 300);
static const std::string lenaWinName = "Lena";
static const std::string fourierRealWinName = "Fourier real";
static const std::string fourierImaginaryWinName = "Fourier imaginary";
static const std::string phaseWinName = "Phase";
static const std::string powerWinName = "Power";

cv::Mat fourier(const cv::Mat &srcMat);

cv::Mat phase(const cv::Mat &srcMat);

cv::Mat power(const cv::Mat &srcMat);

void flipQuadrants(cv::Mat &inOutMat);

cv::Mat phasePower(const cv::Mat &srcMat);


int main(void) {
  
  /**
   * Load image.
   */
  cv::Mat src_8uc1_img = cv::imread("./images/lena64.png");
  
  /**
   * Check, if the image is empty.
   */
  assert(!src_8uc1_img.empty());
  
  cv::Mat src_64fc1_img;
  
  /**
   * Convert ui8 image to fl64
   */
  src_8uc1_img.convertTo(src_64fc1_img, CV_64FC1, 1.0f / 255.0f);
  
  cv::namedWindow(lenaWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::resizeWindow(lenaWinName, winSize);
  cv::imshow(lenaWinName, src_64fc1_img);
  
  cv::waitKey(1);
  
  const cv::Mat fourierMat = fourier(src_64fc1_img);
  
  cv::Mat fourierRealPart(fourierMat.rows, fourierMat.cols, CV_64FC1);
  cv::Mat fourierImaginaryPart(fourierMat.rows, fourierMat.cols, CV_64FC1);
//  cv::Mat phaseMat(fourierMat.rows, fourierMat.cols, CV_64FC1);
  
  cv::Mat fourierSplit[2];
  cv::split(fourierMat, fourierSplit);

//
//  for (int row = 0; row < fourierMat.rows; row++) {
//    for (int col = 0; col < fourierMat.cols; col++) {
//      cv::Vec2d pix = fourierMat.at<cv::Vec2d>(row, col);
//      fourierRealPart.at<double>(row, col) = pix[0];
//      fourierImaginaryPart.at<double>(row, col) = pix[1];
//    }
//  }
//
  const cv::Mat phaseMat = phase(fourierMat);
  const cv::Mat powerMat = power(fourierMat);
  
  cv::namedWindow(fourierRealWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::namedWindow(fourierImaginaryWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::namedWindow(phaseWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::namedWindow(powerWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  
  cv::resizeWindow(fourierRealWinName, winSize);
  cv::resizeWindow(fourierImaginaryWinName, winSize);
  cv::resizeWindow(phaseWinName, winSize);
  cv::resizeWindow(powerWinName, winSize);
  
  cv::imshow(fourierRealWinName, fourierSplit[1]);
  cv::imshow(fourierImaginaryWinName, fourierSplit[0]);
  cv::imshow(phaseWinName, phaseMat);
  cv::imshow(powerWinName, powerMat);
  
  cv::waitKey();
}

cv::Mat fourier(const cv::Mat &srcMat) {
  const int M = srcMat.rows;
  const int N = srcMat.cols;
  const double sqrtMN_1 = 1.0 / sqrt(M * N);
  const double PI_2 = M_PI * 2.;
  const double M_1 = 1. / M;
  const double N_1 = 1. / N;
  
  cv::Mat normalized;
  srcMat.convertTo(normalized, CV_64FC1, sqrtMN_1);
  
  cv::Mat fourierOutput(M, N, CV_64FC2);
  
  for (int k = 0; k < fourierOutput.rows; k++) {
    for (int l = 0; l < fourierOutput.cols; l++) {
      cv::Vec2d fourierPix = fourierOutput.at<cv::Vec2d>(k, l);
      
      fourierPix[0] = 0.;
      fourierPix[1] = 0.;
      
      for (int m = 0; m < fourierOutput.rows; m++) {
        for (int n = 0; n < fourierOutput.cols; n++) {
          
          double lenaPix = normalized.at<double>(m, n);
          
          double a1 = (static_cast<double>(m) * static_cast<double>(k)) * M_1;
          double a2 = (static_cast<double>(n) * static_cast<double>(l)) * N_1;
          double x = -PI_2 * (a1 + a2);
          
          //Real part
          fourierPix.val[0] += lenaPix * sin(x);
          //imaginary part
          fourierPix.val[1] += lenaPix * cos(x);
        }
      }
      fourierOutput.at<cv::Vec2d>(k, l) = fourierPix;
    }
  }
  return fourierOutput;
}

cv::Mat phase(const cv::Mat &srcMat) {
  int M = srcMat.rows;
  int N = srcMat.cols;
  // Create result matrix
  cv::Mat dest(M, N, CV_64FC1);
  
  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      cv::Vec2d F = srcMat.at<cv::Vec2d>(m, n);
      dest.at<double>(m, n) = atan(F[1] / F[0]);
    }
  }
  flipQuadrants(dest);
  return dest;
}

cv::Mat power(const cv::Mat &srcMat) {
  int M = srcMat.rows;
  int N = srcMat.cols;
  
  cv::Mat dest(M, N, CV_64FC1);
  
  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      cv::Vec2d F = srcMat.at<cv::Vec2d>(m, n);
      double amp = sqrt((F[0] * F[0]) + (F[1] * F[1]));
      dest.at<double>(m, n) = log(amp * amp);
    }
  }
  
  // Normalize and flip quadrants
  cv::normalize(dest, dest, 0.0, 1.0, cv::NORM_MINMAX);
  flipQuadrants(dest);
  return dest;
}

cv::Mat phasePower(const cv::Mat &srcMat) {
  const int M = srcMat.rows;
  const int N = srcMat.cols;
  
  cv::Mat phasePowerMat(M, N, CV_64FC2);
  
  for (int m = 0; m < M; m++) {
    for (int n = 0; n < N; n++) {
      cv::Vec2d F = srcMat.at<cv::Vec2d>(m, n);
      phasePowerMat.at<double>(m, n) = atan(F[1] / F[0]);
    }
  }
  return phasePowerMat;
}

void flipQuadrants(cv::Mat &inOutMat) {
  const int hRows = inOutMat.rows / 2;
  const int hCols = inOutMat.cols / 2;
  
  for (int y = 0; y < hRows; y++) {
    for (int x = 0; x < hCols; x++) {
      // Extract px from each quadrant
      double tl = inOutMat.at<double>(y, x); // top left px
      double tr = inOutMat.at<double>(y, x + hCols); // top right px
      double bl = inOutMat.at<double>(y + hRows, x); // bottom left px
      double br = inOutMat.at<double>(y + hRows, x + hCols); // bottom right px
      
      // Compose image
      inOutMat.at<double>(y, x) = br;
      inOutMat.at<double>(y, x + hCols) = bl;
      inOutMat.at<double>(y + hRows, x) = tr;
      inOutMat.at<double>(y + hRows, x + hCols) = tl;
    }
  }
}

