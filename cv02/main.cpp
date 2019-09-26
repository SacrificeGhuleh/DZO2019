#include "pch.h"

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */

const int gammaSliderMin = 0;
const int gammaSliderMax = 200;
int gammaSlider = 100;

/**
 * @brief 32 bit float 1 channel destination image with corrected gamma.
 */
cv::Mat dst32fc1ImgGammaCorrected;

/**
 * @brief 32 bit float 1 channel source image.
 */
cv::Mat src32fc1ImgGammaOriginal;

/**
 * @brief callback function for trackbar
 */
static void onTrackbar(int, void *) {
  //printf("slider: %d, max: %d, gamma: %f\n", gammaSlider, gammaSliderMax, gammaSlider/100.f);
  
  /**
   * For each pixel, get original pixel and set it to power of gamma value.
   */
  for (int row = 0; row < dst32fc1ImgGammaCorrected.rows; row++) {
    for (int col = 0; col < dst32fc1ImgGammaCorrected.cols; col++) {
      float loc_pix = static_cast<float>(src32fc1ImgGammaOriginal.at<float>(row, col));
      dst32fc1ImgGammaCorrected.at<float>(row, col) = powf(loc_pix, static_cast<float>(gammaSlider) / 100.f);
    }
  }
  
  cv::Mat dst8uc3ShowImage(dst32fc1ImgGammaCorrected.rows, dst32fc1ImgGammaCorrected.cols * 2, CV_8UC3);
  
  for (int row = 0; row < dst8uc3ShowImage.rows; row++) {
    for (int col = 0; col < dst8uc3ShowImage.cols / 2; col++) {
      float pix1 = src32fc1ImgGammaOriginal.at<float>(row, col);
      float pix2 = dst32fc1ImgGammaCorrected.at<float>(row, col);
      
      dst8uc3ShowImage.at<cv::Vec3b>(row, col) = cv::Vec3b(pix1 * 255.f, pix1 * 255.f, pix1 * 255.f);
      dst8uc3ShowImage.at<cv::Vec3b>(row, dst32fc1ImgGammaCorrected.cols + col) = cv::Vec3b(pix2 * 255.f, pix2 * 255.f,
                                                                                            pix2 * 255.f);
    }
  }

//  cv::imshow("CV2 | Gamma correction", dst32fc1ImgGammaCorrected);
  cv::imshow("CV2 | Gamma correction", dst8uc3ShowImage);
}

/**
 * @brief Tests gamma correction
 * @param g gamma coeficient
 * @remark g should be in <0,2>
 */
void testGamma(float g) {
  cv::Mat src_32fc1_corrected = src32fc1ImgGammaOriginal.clone();
  if (g > 2.f)
    g = 2.f;
  if (g < 0.f)
    g = 0.f;
  
  /**
   * For each pixel, get original pixel and set it to power of gamma value.
   */
  for (int row = 0; row < src_32fc1_corrected.rows; row++) {
    for (int col = 0; col < src_32fc1_corrected.cols; col++) {
      float loc_pix = static_cast<float>(src32fc1ImgGammaOriginal.at<float>(row, col));
      src_32fc1_corrected.at<float>(row, col) = powf(loc_pix, g);
    }
  }
  char winTitle[30] = {0};
  sprintf(winTitle, "Gamma corrected by %f", g);
  cv::imshow(winTitle, src_32fc1_corrected);
}

int main(void) {
  /**
   * Load image.
   */
  cv::Mat src_8uc1_src_img = cv::imread("./images/moon.jpg", cv::IMREAD_GRAYSCALE);
  
  /**
   * Check, if the image is empty
   */
  if (src_8uc1_src_img.empty()) {
    std::cout << "WTF" << std::endl;
    return -1;
  }
  /**
   * Convert image to float
   */
  src_8uc1_src_img.convertTo(src32fc1ImgGammaOriginal, CV_32FC1, 1.0 / 255.0);
  
  /**
   * Copy original image to destination image
   */
  dst32fc1ImgGammaCorrected = src32fc1ImgGammaOriginal.clone();

//  testGamma(0.0f);
//  testGamma(0.5f);
//  testGamma(1.0f);
//  testGamma(1.5f);
//  testGamma(2.f);
  
  /**
   * Create new image, composit of two gamma images
   */
  cv::Mat dst8uc3ShowImage(dst32fc1ImgGammaCorrected.rows, dst32fc1ImgGammaCorrected.cols * 2, CV_8UC3);
  
  for (int row = 0; row < dst8uc3ShowImage.rows; row++) {
    for (int col = 0; col < dst8uc3ShowImage.cols / 2; col++) {
      float pix1 = src32fc1ImgGammaOriginal.at<float>(row, col);
      float pix2 = dst32fc1ImgGammaCorrected.at<float>(row, col);
      
      dst8uc3ShowImage.at<cv::Vec3b>(row, col) = cv::Vec3b(pix1 * 255.f, pix1 * 255.f, pix1 * 255.f);
      dst8uc3ShowImage.at<cv::Vec3b>(row, dst32fc1ImgGammaCorrected.cols + col) = cv::Vec3b(pix2 * 255.f, pix2 * 255.f,
                                                                                            pix2 * 255.f);
    }
  }
  
  
  cv::namedWindow("CV2 | Gamma correction", cv::WINDOW_AUTOSIZE);
  
  cv::imshow("CV2 | Gamma correction", dst8uc3ShowImage);
  cv::createTrackbar("Gamma", "CV2 | Gamma correction", &gammaSlider, gammaSliderMax, onTrackbar);
  
  cv::waitKey();
}