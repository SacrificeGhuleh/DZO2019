#include "pch.h"
#include <omp.h>
#include <opencv2/imgproc.hpp>  //cv::imread
#include "timer.h"

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */

static const double SIGMA = 0.015;
static const double LAMBDA = 0.1;
static const unsigned int ITERATIONS = 100;
static const unsigned int DRAW_EACH_N = 10;


template<class T>
inline T sqr(T val) {
  return val * val;
}

inline double g(double I, double sigma) {
  return exp(-(sqr(I) / sqr(sigma)));
}

cv::Mat anisotropic(const cv::Mat &src_64fc1_img, const int iterations, const int drawEachNFrames = 1);

void anisotropicStep(cv::Mat &src_64fc1_img);

int main(void) {
  /**
   * Load image.
   */
  cv::Mat src_8uc1_img = cv::imread("./images/valve.png", cv::IMREAD_GRAYSCALE);
  
  /**
   * Check, if the image is empty.
   */
  assert(!src_8uc1_img.empty());
  
  cv::Mat src_64fc1_img;
  
  /**
   * Convert ui8 image to fl64
   */
  src_8uc1_img.convertTo(src_64fc1_img, CV_64FC1, 1.0f / 255.0f);
  
  /**
   * Check, if converted float image is empty.
   */
  assert(!src_64fc1_img.empty());
  
  /**
   * Show original image.
   */
  
  cv::Mat anisotropic_64fc1_img = anisotropic(src_64fc1_img, ITERATIONS, DRAW_EACH_N);
  assert(!anisotropic_64fc1_img.empty());
  
  cv::Mat dst8uc3ShowImage(src_64fc1_img.rows, src_64fc1_img.cols * 2, CV_8UC3);
  
  for (int row = 0; row < dst8uc3ShowImage.rows; row++) {
    for (int col = 0; col < dst8uc3ShowImage.cols / 2; col++) {
      double pix1 = src_64fc1_img.at<double>(row, col);
      double pix2 = anisotropic_64fc1_img.at<double>(row, col);
      
      dst8uc3ShowImage.at<cv::Vec3b>(row, col) = cv::Vec3b(pix1 * 255.f, pix1 * 255.f, pix1 * 255.f);
      dst8uc3ShowImage.at<cv::Vec3b>(row, src_64fc1_img.cols + col) = cv::Vec3b(pix2 * 255.f, pix2 * 255.f,
                                                                                pix2 * 255.f);
    }
  }
  
  
  cv::putText(dst8uc3ShowImage, //target image
              "Original img", //text
              cv::Point(10, 30), //top-left position
              cv::FONT_HERSHEY_DUPLEX,
              1.0,
              CV_RGB(255, 0, 0), //font color
              2);
  
  cv::putText(dst8uc3ShowImage, //target image
              "Filtered", //text
              cv::Point((dst8uc3ShowImage.cols / 2) + 10, 30), //top-left position
              cv::FONT_HERSHEY_DUPLEX,
              1.0,
              CV_RGB(255, 0, 0), //font color
              2);
  
  cv::putText(dst8uc3ShowImage, //target image
              "Iterations: " + std::to_string(ITERATIONS), //text
              cv::Point((dst8uc3ShowImage.cols / 2) + 10, 60), //top-left position
              cv::FONT_HERSHEY_DUPLEX,
              1.0,
              CV_RGB(255, 0, 0), //font color
              2);
  
  
  cv::imshow("Original, anisotropic", dst8uc3ShowImage);
//  cv::imshow("Original fl64", src_64fc1_img);
//  cv::imshow("Anisotropic filter", anisotropic_64fc1_img);
  cv::waitKey();
}

cv::Mat anisotropic(const cv::Mat &src_64fc1_img, const int iterations, const int drawEachNFrames) {
  cv::Mat dst_64fc1_img = src_64fc1_img.clone();
  assert(!dst_64fc1_img.empty());
  assert(!tmp_64fc1_img.empty());
  cv::Mat dst8uc3ShowImage(dst_64fc1_img.rows, dst_64fc1_img.cols, CV_8UC3);
  
  
  for (int iteration = 1; iteration <= iterations; iteration++) {
    
    Timer timer;
    timer.reset();
    anisotropicStep(dst_64fc1_img);
    double elapsed = timer.elapsed();
    printf("Iteration %05d Elapsed: %f ms\r", iteration, elapsed*1000.);
    if (iteration % drawEachNFrames == 0) {
      for (int row = 0; row < dst8uc3ShowImage.rows; row++) {
        for (int col = 0; col < dst8uc3ShowImage.cols; col++) {
          double pix = dst_64fc1_img.at<double>(row, col);
          dst8uc3ShowImage.at<cv::Vec3b>(row, col) = cv::Vec3b(pix * 255.f, pix * 255.f, pix * 255.f);
        }
      }
      cv::putText(dst8uc3ShowImage, //target image
                  "Filtered", //text
                  cv::Point(10, 30), //top-left position
                  cv::FONT_HERSHEY_DUPLEX,
                  1.0,
                  CV_RGB(255, 0, 0), //font color
                  2);
      
      cv::putText(dst8uc3ShowImage, //target image
                  "Iterations: " + std::to_string(iteration), //text
                  cv::Point(10, 60), //top-left position
                  cv::FONT_HERSHEY_DUPLEX,
                  1.0,
                  CV_RGB(255, 0, 0), //font color
                  2);
      
      cv::imshow("AnisotropicProgress", dst8uc3ShowImage);
      cv::waitKey(1);
    }
  }
  cv::waitKey();
  cv::destroyWindow("AnisotropicProgress");
  return dst_64fc1_img;
}

void anisotropicStep(cv::Mat &src_64fc1_img) {
  cv::Mat tmp_64fc1_img = src_64fc1_img.clone();
  #pragma omp parallel for num_threads(4)
  for (int col = 1; col < src_64fc1_img.cols - 1; col++) {
    for (int row = 1; row < src_64fc1_img.rows - 1; row++) {
      double I = tmp_64fc1_img.at<double>(row, col);
      
      double N = tmp_64fc1_img.at<double>(row - 1, col);
      double S = tmp_64fc1_img.at<double>(row + 1, col);
      double E = tmp_64fc1_img.at<double>(row, col + 1);
      double W = tmp_64fc1_img.at<double>(row, col - 1);
      
      double deltaN = N - I;
      double deltaS = S - I;
      double deltaE = E - I;
      double deltaW = W - I;
      
      double cN = g(deltaN, SIGMA);
      double cS = g(deltaS, SIGMA);
      double cE = g(deltaE, SIGMA);
      double cW = g(deltaW, SIGMA);
      
      double It = I + (LAMBDA * ((cN * deltaN) + (cS * deltaS) + (cE * deltaE) + (cW * deltaW)));
      src_64fc1_img.at<double>(row, col) = It;
    }
  }
}