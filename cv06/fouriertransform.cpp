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
    inverseWinName(name + " - Inverse"),
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
  
  for (int outerRow = 0; outerRow < fourierTransformMat_.rows; outerRow++) {
    for (int outerCol = 0; outerCol < fourierTransformMat_.cols; outerCol++) {
      cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(outerRow, outerCol);
      
      fourierPix[0] = 0.;
      fourierPix[1] = 0.;
      
      for (int innerRow = 0; innerRow < fourierTransformMat_.rows; innerRow++) {
        for (int innerCol = 0; innerCol < fourierTransformMat_.cols; innerCol++) {
          
          double pix = normalized.at<double>(innerRow, innerCol);
          
          double a1 = (static_cast<double>(innerRow) * static_cast<double>(outerRow)) * M_1;
          double a2 = (static_cast<double>(innerCol) * static_cast<double>(outerCol)) * N_1;
          double x = -PI_2 * (a1 + a2);
          
          //Real part
          fourierPix.val[0] += pix * sin(x);
          //imaginary part
          fourierPix.val[1] += pix * cos(x);
        }
      }
      fourierTransformMat_.at<cv::Vec2d>(outerRow, outerCol) = fourierPix;
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

void FourierTransform::performInverse() {
  const int M = fourierTransformMat_.rows;
  const int N = fourierTransformMat_.cols;
  const double sqrtMN_1 = 1.0 / sqrt(M * N);
  const double PI_2 = M_PI * 2.;
  const double M_1 = 1. / M;
  const double N_1 = 1. / N;
  
  fourierInverseMat_ = cv::Mat(M, N, CV_64FC1);
  
  for (int outerRow = 0; outerRow < fourierInverseMat_.rows; outerRow++) {
    for (int outerCol = 0; outerCol < fourierInverseMat_.cols; outerCol++) {
      double inversePix = 0;
      
      for (int innerRow = 0; innerRow < fourierInverseMat_.rows; innerRow++) {
        for (int innerCol = 0; innerCol < fourierInverseMat_.cols; innerCol++) {
          cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(innerRow, innerCol);
          
          const double a1 = (static_cast<double>(innerRow) * static_cast<double>(outerRow)) * M_1;
          const double a2 = (static_cast<double>(innerCol) * static_cast<double>(outerCol)) * N_1;
          const double x = PI_2 * (a1 + a2);
          
          const double realX = cos(x) * sqrtMN_1;
          const double imagX = sin(x) * sqrtMN_1;
  
          inversePix += fourierPix[1] * realX - fourierPix[0] * imagX;
        }
      }
      fourierInverseMat_.at<double>(outerRow, outerCol) = inversePix;
    }
  }
  cv::normalize(fourierInverseMat_, fourierInverseMat_, 0.0, 1.0, cv::NORM_MINMAX);
}

void FourierTransform::process() {
  Timer timer;
  performFourier();
  performPower();
  performPhase();
  performInverse();
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
  for (int outerRow = 0; outerRow < fourierTransformMat_.rows; outerRow++) {
    for (int outerCol = 0; outerCol < fourierTransformMat_.cols; outerCol++) {
      cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(outerRow, outerCol);
      
      fourierPix[0] = 0.;
      fourierPix[1] = 0.;
      
      for (int innerRow = 0; innerRow < fourierTransformMat_.rows; innerRow++) {
        for (int innerCol = 0; innerCol < fourierTransformMat_.cols; innerCol++) {
          
          double pix = normalized.at<double>(innerRow, innerCol);
          
          double a1 = (static_cast<double>(innerRow) * static_cast<double>(outerRow)) * M_1;
          double a2 = (static_cast<double>(innerCol) * static_cast<double>(outerCol)) * N_1;
          double x = -PI_2 * (a1 + a2);
          
          //Real part
          fourierPix.val[0] += pix * sin(x);
          //imaginary part
          fourierPix.val[1] += pix * cos(x);
        }
      }
      fourierTransformMat_.at<cv::Vec2d>(outerRow, outerCol) = fourierPix;
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

void FourierTransform::performInverseInThreads() {
  const int M = fourierTransformMat_.rows;
  const int N = fourierTransformMat_.cols;
  const double sqrtMN_1 = 1.0 / sqrt(M * N);
  const double PI_2 = M_PI * 2.;
  const double M_1 = 1. / M;
  const double N_1 = 1. / N;
  
  fourierInverseMat_ = cv::Mat(M, N, CV_64FC1);
  
  #pragma omp parallel for schedule(dynamic, 5) shared(fourierTransformMat_, fourierInverseMat_)
  for (int outerRow = 0; outerRow < fourierInverseMat_.rows; outerRow++) {
    for (int outerCol = 0; outerCol < fourierInverseMat_.cols; outerCol++) {
      double inversePix = 0;
      
      for (int innerRow = 0; innerRow < fourierInverseMat_.rows; innerRow++) {
        for (int innerCol = 0; innerCol < fourierInverseMat_.cols; innerCol++) {
          cv::Vec2d fourierPix = fourierTransformMat_.at<cv::Vec2d>(innerRow, innerCol);
          
          const double a1 = (static_cast<double>(innerRow) * static_cast<double>(outerRow)) * M_1;
          const double a2 = (static_cast<double>(innerCol) * static_cast<double>(outerCol)) * N_1;
          const double x = PI_2 * (a1 + a2);
          
          const double realX = cos(x) * sqrtMN_1;
          const double imagX = sin(x) * sqrtMN_1;
          
          inversePix += fourierPix[1] * realX - fourierPix[0] * imagX;
        }
      }
      fourierInverseMat_.at<double>(outerRow, outerCol) = inversePix;
    }
  }
  cv::normalize(fourierInverseMat_, fourierInverseMat_, 0.0, 1.0, cv::NORM_MINMAX);
}

void FourierTransform::processInThreads() {
  Timer timer;
  performFourierInThreads();
  performPowerInThreads();
  performPhaseInThreads();
  performInverseInThreads();
  printf("Fourier computed in threads in %f seconds\n", timer.elapsed());
}

//
//  Showing functions
//

void FourierTransform::show() {
  cv::namedWindow(originalWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::namedWindow(phaseWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::namedWindow(powerWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  cv::namedWindow(inverseWinName, cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
  
  cv::resizeWindow(originalWinName, winSize);
  cv::resizeWindow(phaseWinName, winSize);
  cv::resizeWindow(powerWinName, winSize);
  cv::resizeWindow(inverseWinName, winSize);
  
  cv::imshow(originalWinName, originalMat_);
  cv::imshow(phaseWinName, fourierPhaseMat_);
  cv::imshow(powerWinName, fourierPowerMat_);
  cv::imshow(inverseWinName, fourierInverseMat_);

//  cv::waitKey();
}

#pragma clang diagnostic pop