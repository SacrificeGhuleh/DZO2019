//
// Created by zvone on 07-Dec-19.
//

#ifndef DZO2019_FOURIERTRANSFORM_H
#define DZO2019_FOURIERTRANSFORM_H


#include <opencv2/core/mat.hpp>

class FourierTransform {
public:
  FourierTransform(const std::string &fileName, const std::string &name = "");
  
  void process();
  
  void processInThreads();
  
  void show();

protected:
  void performFourier();
  
  void performFourierInThreads();
  
  void performPhase();
  
  void performPhaseInThreads();
  
  void performPower();
  
  void performPowerInThreads();
  
  static void flipQuadrantsInThreads(cv::Mat1d &inputMat);
  
  static void flipQuadrants(cv::Mat1d &inputMat);
  
  const cv::Size winSize;
  const std::string originalWinName;
  const std::string phaseWinName;
  const std::string powerWinName;
  
  cv::Mat1b originalMat_;
  cv::Mat2d fourierTransformMat_;
  cv::Mat1d fourierPhaseMat_;
  cv::Mat1d fourierPowerMat_;
  
};


#endif //DZO2019_FOURIERTRANSFORM_H
