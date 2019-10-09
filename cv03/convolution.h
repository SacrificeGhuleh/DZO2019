//
// Created by zvone on 02-Oct-19.
//

#ifndef DZO2019_CONVOLUTION_H
#define DZO2019_CONVOLUTION_H

template<class T_MASK>
cv::Mat applyConvolution(const cv::Mat &srcMat) {
  T_MASK mask;
  return applyConvolution(mask, srcMat);
}

template<class T_MASK>
cv::Mat applyConvolution(const T_MASK &mask, const cv::Mat &srcMat) {
  //input mat is greyscale image 8bit int image
  
  std::cout << "Applying convolution with mask: " << std::endl;
  mask.print();
  
  cv::Mat dstMat = srcMat.clone();
  for (int matCol = mask.offset; matCol < srcMat.cols - mask.offset; matCol++) {
    for (int matRow = mask.offset; matRow < srcMat.rows - mask.offset; matRow++) {
      float pixelSum = 0.f;
      for (int maskCol = -mask.offset; maskCol <= mask.offset; maskCol++) {
        for (int maskRow = -mask.offset; maskRow <= mask.offset; maskRow++) {
          float pixf = static_cast<float>(srcMat.at<uint8_t>(matRow + maskRow, matCol + maskCol));
          
          pixf *= mask.at(mask.offset + maskRow, mask.offset + maskCol);
          pixelSum += pixf;
        }
      }
      pixelSum /= mask.scale;
      
      dstMat.at<uint8_t>(matRow, matCol) = pixelSum;
    }
  }
  return dstMat;
}

#endif //DZO2019_CONVOLUTION_H
