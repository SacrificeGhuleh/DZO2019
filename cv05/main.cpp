#include "pch.h"
#include <omp.h>
#include "fouriertransform.h"

/**
 * @file main.cpp
 * @brief Template file for openCV
 * */


int main(void) {
  FourierTransform dftLena("./images/lena64.png", "Lena 64");
  dftLena.processInThreads();
  dftLena.show();
  
//  FourierTransform dftLenaNoise("./images/lena64_noise.png", "Lena 64 Noise");
//  dftLenaNoise.process();
//  dftLenaNoise.show();
    cv::waitKey();
  
  return 0;
}