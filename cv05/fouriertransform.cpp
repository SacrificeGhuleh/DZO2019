#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by zvone on 07-Dec-19.
//

#include "fouriertransform.h"
#include "pch.h"
#include "timer.h"

FourierTransform::FourierTransform(const std::string &fileName, const std::string &name) :
    winSize(300, 300),
    originalWinName(name + " - Original"),
    phaseWinName(name + "- Phase"),
    powerWinName(name + " - Power"),
    originalMat_(cv::imread(fileName, cv::IMREAD_GRAYSCALE)) {
  
  
  if (originalMat_.empty()) {
    throw std::runtime_error("Loaded image empty");
  }
}

//
// Single thread processing
//

void FourierTransform::performFourier() {
  cv::Mat1d normalized;
  
  const int M = originalMat_.rows;
  const int N = originalMat_.cols;
  const double sqrtMN_1 = 1.0 / sqrt(M * N);
  const double PI_2 = M_PI * 2.;
  const double M_1 = 1. / M;
  const double N_1 = 1. / N;
  
  // Convert and normalize
  originalMat_.convertTo(normalized, CV_64FC1, 1.0 / 255.0 * sqrtMN_1);
  fourierTransformMat_ = cv::Mat(M, N, CV_64FC2);
  
  for (int k = 0; k < fourierTransformMat_.rows; k++) {
    for (int l = 0; l < fourierTransformMat_.cols; l++) {
      cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(k, l);
      
      fourierPix[0] = 0.;
      fourierPix[1] = 0.;
      
      for (int m = 0; m < fourierTransformMat_.rows; m++) {
        for (int n = 0; n < fourierTransformMat_.cols; n++) {
          
          double pix = normalized.at<double>(m, n);
          
          double a1 = (static_cast<double>(m) * static_cast<double>(k)) * M_1;
          double a2 = (static_cast<double>(n) * static_cast<double>(l)) * N_1;
          double x = -PI_2 * (a1 + a2);
          
          //Real part
          fourierPix.val[0] += pix * sin(x);
          //imaginary part
          fourierPix.val[1] += pix * cos(x);
        }
      }
      fourierTransformMat_.at<cv::Vec2d>(k, l) = fourierPix;
    }
  }
  
}

void FourierTransform::performPhase() {
  const int M = fourierTransformMat_.rows;
  const int N = fourierTransformMat_.cols;
  
  fourierPhaseMat_ = cv::Mat(M, N, CV_64FC1);
  
  for (int row = 0; row < M; row++) {
    for (int col = 0; col < N; col++) {
      cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(row, col);
      fourierPhaseMat_.at<double>(row, col) = atan(fourierPix[1] / fourierPix[0]);
    }
  }
}

void FourierTransform::performPower() {
  const int M = fourierTransformMat_.rows;
  const int N = fourierTransformMat_.cols;
  
  // Create result matrix
  fourierPowerMat_ = cv::Mat(M, N, CV_64FC1);
  
  for (int row = 0; row < M; row++) {
    for (int col = 0; col < N; col++) {
      cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(row, col);
      const double amplitude = sqrt(fourierPix[0] * fourierPix[0] + fourierPix[1] * fourierPix[1]);
      fourierPowerMat_.at<double>(row, col) = log(amplitude * amplitude);
    }
  }
  
  cv::normalize(fourierPowerMat_, fourierPowerMat_, 0.0, 1.0, cv::NORM_MINMAX);
  flipQuadrants(fourierPowerMat_);
}

void FourierTransform::flipQuadrants(cv::Mat1d &inputMat) {
  // Get half of image rows and cols
  const int halfRows = inputMat.rows / 2;
  const int halfCols = inputMat.cols / 2;
  
  for (int row = 0; row < halfRows; row++) {
    for (int col = 0; col < halfCols; col++) {
      const double topLeftPix = inputMat.at<double>(row, col);
      const double topRightPix = inputMat.at<double>(row, col + halfCols);
      const double bottomLeftPix = inputMat.at<double>(row + halfRows, col);
      const double bottomRightPix = inputMat.at<double>(row + halfRows, col + halfCols);
      
      inputMat.at<double>(row, col) = bottomRightPix; //Top left becomes bottom right
      inputMat.at<double>(row, col + halfCols) = bottomLeftPix; // Top right becomes bottom left
      inputMat.at<double>(row + halfRows, col) = topRightPix; // Bottom left becomes top right
      inputMat.at<double>(row + halfRows, col + halfCols) = topLeftPix; // Bottom righ becomes top left
    }
  }
}

void FourierTransform::process() {
  Timer timer;
  performFourier();
  performPower();
  performPhase();
  printf("Fourier computed in %f seconds\n", timer.elapsed());
}

//
//  Multithread processing
//

void FourierTransform::performFourierInThreads() {
  cv::Mat1d normalized;
  
  const int M = originalMat_.rows;
  const int N = originalMat_.cols;
  const double sqrtMN_1 = 1.0 / sqrt(M * N);
  const double PI_2 = M_PI * 2.;
  const double M_1 = 1. / M;
  const double N_1 = 1. / N;
  
  // Convert and normalize
  originalMat_.convertTo(normalized, CV_64FC1, 1.0 / 255.0 * sqrtMN_1);
  fourierTransformMat_ = cv::Mat(M, N, CV_64FC2);
  
  #pragma omp parallel for schedule(dynamic, 5) shared(fourierTransformMat_, normalized)
  for (int k = 0; k < fourierTransformMat_.rows; k++) {
    for (int l = 0; l < fourierTransformMat_.cols; l++) {
      cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(k, l);
      
      fourierPix[0] = 0.;
      fourierPix[1] = 0.;
      
      for (int m = 0; m < fourierTransformMat_.rows; m++) {
        for (int n = 0; n < fourierTransformMat_.cols; n++) {
          
          double pix = normalized.at<double>(m, n);
          
          double a1 = (static_cast<double>(m) * static_cast<double>(k)) * M_1;
          double a2 = (static_cast<double>(n) * static_cast<double>(l)) * N_1;
          double x = -PI_2 * (a1 + a2);
          
          //Real part
          fourierPix.val[0] += pix * sin(x);
          //imaginary part
          fourierPix.val[1] += pix * cos(x);
        }
      }
      fourierTransformMat_.at<cv::Vec2d>(k, l) = fourierPix;
    }
  }
  
}

void FourierTransform::performPhaseInThreads() {
  const int M = fourierTransformMat_.rows;
  const int N = fourierTransformMat_.cols;
  
  fourierPhaseMat_ = cv::Mat(M, N, CV_64FC1);
  #pragma omp parallel for schedule(dynamic, 5) shared(fourierPhaseMat_, fourierTransformMat_)
  for (int row = 0; row < M; row++) {
    for (int col = 0; col < N; col++) {
      cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(row, col);
      fourierPhaseMat_.at<double>(row, col) = atan(fourierPix[1] / fourierPix[0]);
    }
  }
}

void FourierTransform::performPowerInThreads() {
  const int M = fourierTransformMat_.rows;
  const int N = fourierTransformMat_.cols;
  
  // Create result matrix
  fourierPowerMat_ = cv::Mat(M, N, CV_64FC1);
  #pragma omp parallel for schedule(dynamic, 5) shared(fourierPowerMat_, fourierTransformMat_)
  for (int row = 0; row < M; row++) {
    for (int col = 0; col < N; col++) {
      cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(row, col);
      const double amplitude = sqrt(fourierPix[0] * fourierPix[0] + fourierPix[1] * fourierPix[1]);
      fourierPowerMat_.at<double>(row, col) = log(amplitude * amplitude);
    }
  }
  
  cv::normalize(fourierPowerMat_, fourierPowerMat_, 0.0, 1.0, cv::NORM_MINMAX);
  flipQuadrantsInThreads(fourierPowerMat_);
}

void FourierTransform::flipQuadrantsInThreads(cv::Mat1d &inputMat) {
  // Get half of image rows and cols
  const int halfRows = inputMat.rows / 2;
  const int halfCols = inputMat.cols / 2;
  #pragma omp parallel for schedule(dynamic, 5) shared(inputMat)
  for (int row = 0; row < halfRows; row++) {
    for (int col = 0; col < halfCols; col++) {
      const double topLeftPix = inputMat.at<double>(row, col);
      const double topRightPix = inputMat.at<double>(row, col + halfCols);
      const double bottomLeftPix = inputMat.at<double>(row + halfRows, col);
      const double bottomRightPix = inputMat.at<double>(row + halfRows, col + halfCols);
      
      inputMat.at<double>(row, col) = bottomRightPix; //Top left becomes bottom right
      inputMat.at<double>(row, col + halfCols) = bottomLeftPix; // Top right becomes bottom left
      inputMat.at<double>(row + halfRows, col) = topRightPix; // Bottom left becomes top right
      inputMat.at<double>(row + halfRows, col + halfCols) = topLeftPix; // Bottom righ becomes top left
    }
  }
}

void FourierTransform::processInThreads() {
  Timer timer;
  performFourierInThreads();
  performPowerInThreads();
  performPhaseInThreads();
  printf("Fourier computed in threads in %f seconds\n", timer.elapsed());
}

//
//  Showing functions
//

void FourierTransform::show() {
  cv::namedWindow(originalWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::namedWindow(phaseWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::namedWindow(powerWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  
  cv::resizeWindow(originalWinName, winSize);
  cv::resizeWindow(phaseWinName, winSize);
  cv::resizeWindow(powerWinName, winSize);
  
  cv::imshow(originalWinName, originalMat_);
  cv::imshow(phaseWinName, fourierPhaseMat_);
  cv::imshow(powerWinName, fourierPowerMat_);
  
//  cv::waitKey();
}

#pragma clang diagnostic pop